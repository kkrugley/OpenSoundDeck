#include "GlobalHotkeyManager.h"
#include <QDebug>
#include <QApplication>

#ifdef Q_OS_WIN
#include <Windows.h>
#elif defined(Q_OS_LINUX)
#include <QGuiApplication>
#include <X11/Xlib.h>
#include <xcb/xcb.h> // <--- Добавляем заголовок для XCB

// Wayland portal support
#if defined(USE_DBUS_PORTAL)
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusPendingCallWatcher>
#include <QtDBus/QDBusPendingReply>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusArgument>
#include <QtDBus/QDBusMetaType>
#include <QWindow>
#include <QUuid>
#endif

#elif defined(Q_OS_MACOS)
#include <Carbon/Carbon.h>
#endif

// --- Вспомогательные функции для конвертации QKeySequence в нативные коды ---
#ifdef Q_OS_WIN
static quint32 nativeModifiers(Qt::KeyboardModifiers modifiers) {
    quint32 native = 0;
    if (modifiers & Qt::ShiftModifier) native |= MOD_SHIFT;
    if (modifiers & Qt::ControlModifier) native |= MOD_CONTROL;
    if (modifiers & Qt::AltModifier) native |= MOD_ALT;
    if (modifiers & Qt::MetaModifier) native |= MOD_WIN;
    return native;
}

static quint32 nativeKey(Qt::Key key) {
    // https://doc.qt.io/qt-5/qt.html#Key-enum
    // https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
    if (key >= Qt::Key_F1 && key <= Qt::Key_F24) {
        return VK_F1 + (key - Qt::Key_F1);
    }
    // ... можно добавить больше клавиш по необходимости
    return key;
}
#elif defined(Q_OS_LINUX)
static uint nativeModifiersX11(Qt::KeyboardModifiers modifiers) {
    uint native = 0;
    if (modifiers & Qt::ShiftModifier) native |= ShiftMask;
    if (modifiers & Qt::ControlModifier) native |= ControlMask;
    if (modifiers & Qt::AltModifier) native |= Mod1Mask; // Обычно Alt
    if (modifiers & Qt::MetaModifier) native |= Mod4Mask; // Обычно Super/Win
    return native;
}
#endif


GlobalHotkeyManager::GlobalHotkeyManager(QObject *parent) : QObject(parent)
{
    qApp->installNativeEventFilter(this);

#if defined(Q_OS_LINUX) && defined(USE_DBUS_PORTAL)
    // Check if running under Wayland
    QString sessionType = qEnvironmentVariable("XDG_SESSION_TYPE");
    if (sessionType == "wayland") {
        qDebug() << "Wayland session detected, attempting to use xdg-desktop-portal";
        m_usePortal = initPortal();
        if (m_usePortal) {
            qDebug() << "Successfully initialized xdg-desktop-portal for global shortcuts";
        } else {
            qWarning() << "Failed to initialize xdg-desktop-portal. Global shortcuts may not work on Wayland.";
        }
    }
#endif
}

GlobalHotkeyManager::~GlobalHotkeyManager()
{
    unregisterAll();
    // Only remove filter if application still exists
    if (qApp) {
        qApp->removeNativeEventFilter(this);
    }

#if defined(Q_OS_LINUX) && defined(USE_DBUS_PORTAL)
    if (m_usePortal && !m_sessionHandle.isEmpty()) {
        // Close the portal session
        QDBusInterface sessionInterface(
            "org.freedesktop.portal.Desktop",
            m_sessionHandle,
            "org.freedesktop.portal.Session",
            QDBusConnection::sessionBus()
        );
        sessionInterface.call("Close");
    }
    delete m_portalInterface;
#endif
}

bool GlobalHotkeyManager::registerHotkey(const QKeySequence& sequence, int trackRow)
{
    if (sequence.isEmpty()) {
        return false;
    }

    // --- ИСПРАВЛЕНИЕ для Qt6 ---
    QKeyCombination combo = sequence[0];
    Qt::KeyboardModifiers modifiers = combo.keyboardModifiers();
    Qt::Key key = combo.key();

#ifdef Q_OS_WIN
    quint32 nativeMod = nativeModifiers(modifiers);
    quint32 nativeK = nativeKey(key);
    int hotkeyId = m_nextNativeId++;

    if (RegisterHotKey(NULL, hotkeyId, nativeMod, nativeK)) {
        qDebug() << "Registered hotkey ID" << hotkeyId;
        m_nativeKeyToRow.insert(hotkeyId, trackRow);
        m_registeredHotkeys.insert(sequence, trackRow);
        return true;
    } else {
        qWarning() << "Failed to register hotkey:" << sequence.toString();
        return false;
    }
    #elif defined(Q_OS_LINUX)
#if defined(USE_DBUS_PORTAL)
    // If using portal (Wayland), queue the shortcut for binding
    if (m_usePortal) {
        PortalShortcut shortcut;
        shortcut.shortcutId = QString("track_%1").arg(m_nextPortalId++);
        // Use the hotkey string as description initially
        shortcut.description = sequence.toString();
        shortcut.preferredTrigger = keySequenceToPortalTrigger(sequence);
        shortcut.trackRow = trackRow;

        m_pendingPortalShortcuts.append(shortcut);
        m_portalIdToRow.insert(shortcut.shortcutId, trackRow);
        m_registeredHotkeys.insert(sequence, trackRow);

        // If we already have a session, bind immediately
        if (!m_sessionHandle.isEmpty()) {
            bindShortcutsToPortal();
        } else if (m_sessionToken.isEmpty()) {
            // No session yet and not creating one - start creation
            createPortalSession();
        }
        return true;
    }
#endif

    // Fall back to X11 implementation
    QNativeInterface::QX11Application* x11App = qApp->nativeInterface<QNativeInterface::QX11Application>();
    if (!x11App) {
        qWarning() << "Cannot register global hotkey: not running on X11 and portal unavailable.";
        return false;
    }
    Display* display = x11App->display();

    if (!display) return false;

    uint keycode = XKeysymToKeycode(display, key);
    uint modifiersX11 = nativeModifiersX11(modifiers);

    // Захватываем клавишу на рутовом окне
    XGrabKey(display, keycode, modifiersX11, DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);
    // Также захватываем с NumLock, CapsLock и т.д.
    XGrabKey(display, keycode, modifiersX11 | Mod2Mask, DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync); // NumLock
    XGrabKey(display, keycode, modifiersX11 | LockMask, DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync); // CapsLock
    XGrabKey(display, keycode, modifiersX11 | Mod2Mask | LockMask, DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);

    X11Hotkey hotkey = {keycode, modifiersX11};
    m_nativeKeyToRow.insert(hotkey, trackRow);
    m_registeredHotkeys.insert(sequence, trackRow);
    qDebug() << "Registered hotkey:" << sequence.toString();
    return true;
#elif defined(Q_OS_MACOS)
    EventHotKeyRef hotKeyRef;
    EventTypeSpec eventType;
    eventType.eventClass = kEventClassKeyboard;
    eventType.eventKind = kEventHotKeyPressed;

    // Установка обработчика
    InstallApplicationEventHandler(&HotKeyHandler, 1, &eventType, this, NULL);

    UInt32 keyId = key; // Может потребоваться более сложная конвертация
    UInt32 keyModifiers = 0;
    if (modifiers & Qt::ControlModifier) keyModifiers |= controlKey;
    if (modifiers & Qt::AltModifier) keyModifiers |= optionKey;
    if (modifiers & Qt::ShiftModifier) keyModifiers |= shiftKey;
    if (modifiers & Qt::MetaModifier) keyModifiers |= cmdKey;

    EventHotKeyID hotKeyID;
    hotKeyID.signature = 'htk1';
    hotKeyID.id = m_nextNativeId++;

    OSStatus err = RegisterEventHotKey(keyId, keyModifiers, hotKeyID, GetApplicationEventTarget(), 0, &hotKeyRef);
    if (err == noErr) {
        m_nativeKeyToRow.insert(hotKeyRef, trackRow);
        m_registeredHotkeys.insert(sequence, trackRow);
        return true;
    }
    return false;
#else
    qWarning() << "Global hotkeys not supported on this platform.";
    return false;
#endif
}

void GlobalHotkeyManager::unregisterHotkey(int trackRow)
{
    QKeySequence sequence = m_registeredHotkeys.key(trackRow);
    if (sequence.isEmpty()) return;

#ifdef Q_OS_WIN
    int hotkeyId = m_nativeKeyToRow.key(trackRow);
    if (hotkeyId != 0) {
        UnregisterHotKey(NULL, hotkeyId);
        m_nativeKeyToRow.remove(hotkeyId);
    }
    #elif defined(Q_OS_LINUX)
#if defined(USE_DBUS_PORTAL)
    if (m_usePortal) {
        // Find the shortcut ID for this track row
        QString shortcutIdToRemove;
        for (auto it = m_portalIdToRow.begin(); it != m_portalIdToRow.end(); ++it) {
            if (it.value() == trackRow) {
                shortcutIdToRemove = it.key();
                break;
            }
        }

        if (!shortcutIdToRemove.isEmpty()) {
            m_portalIdToRow.remove(shortcutIdToRemove);

            // Remove from pending list if not yet bound
            for (int i = 0; i < m_pendingPortalShortcuts.size(); ++i) {
                if (m_pendingPortalShortcuts[i].shortcutId == shortcutIdToRemove) {
                    m_pendingPortalShortcuts.removeAt(i);
                    break;
                }
            }

            // Note: xdg-desktop-portal doesn't have an explicit "unbind" method.
            // Shortcuts are unbound when the session is closed or when
            // BindShortcuts is called with an empty list. For simplicity,
            // we just remove from our tracking here. A full implementation
            // could re-call BindShortcuts with the remaining shortcuts.
        }
        m_registeredHotkeys.remove(sequence);
        return;
    }
#endif

    // Fall back to X11 implementation
    QNativeInterface::QX11Application* x11App = qApp->nativeInterface<QNativeInterface::QX11Application>();
    if (!x11App) {
        // This should not happen if registerHotkey succeeded, but for safety...
        return;
    }
    Display* display = x11App->display();
    if (!display) return;

    // --- ИСПРАВЛЕНИЕ для Qt6 ---
    QKeyCombination combo = sequence[0];
    Qt::KeyboardModifiers modifiers = combo.keyboardModifiers();
    Qt::Key key = combo.key();
    uint keycode = XKeysymToKeycode(display, key);
    uint modifiersX11 = nativeModifiersX11(modifiers);

    XUngrabKey(display, keycode, modifiersX11, DefaultRootWindow(display));
    XUngrabKey(display, keycode, modifiersX11 | Mod2Mask, DefaultRootWindow(display));
    XUngrabKey(display, keycode, modifiersX11 | LockMask, DefaultRootWindow(display));
    XUngrabKey(display, keycode, modifiersX11 | Mod2Mask | LockMask, DefaultRootWindow(display));

    X11Hotkey hotkey = {keycode, modifiersX11};
    m_nativeKeyToRow.remove(hotkey);
#elif defined(Q_OS_MACOS)
    void* hotKeyRef = m_nativeKeyToRow.key(trackRow);
    if (hotKeyRef) {
        UnregisterEventHotKey((EventHotKeyRef)hotKeyRef);
        m_nativeKeyToRow.remove(hotKeyRef);
    }
#endif
    m_registeredHotkeys.remove(sequence);
}

void GlobalHotkeyManager::unregisterAll()
{
    for (int row : m_registeredHotkeys.values()) {
        unregisterHotkey(row);
    }
}

bool GlobalHotkeyManager::nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result)
{
#ifdef Q_OS_WIN
    if (eventType == "windows_generic_MSG") {
        MSG* msg = static_cast<MSG*>(message);
        if (msg->message == WM_HOTKEY) {
            int hotkeyId = msg->wParam;
            if (m_nativeKeyToRow.contains(hotkeyId)) {
                emit hotkeyActivated(m_nativeKeyToRow.value(hotkeyId));
                return true;
            }
        }
    }
#elif defined(Q_OS_LINUX)
    if (eventType == "xcb_generic_event_t") {
        xcb_generic_event_t* event = static_cast<xcb_generic_event_t*>(message);
        if ((event->response_type & ~0x80) == XCB_KEY_PRESS) {
            xcb_key_press_event_t* keyEvent = (xcb_key_press_event_t*)event;
            X11Hotkey hotkey = {keyEvent->detail, keyEvent->state & ~Mod2Mask & ~LockMask};
            if (m_nativeKeyToRow.contains(hotkey)) {
                emit hotkeyActivated(m_nativeKeyToRow.value(hotkey));
                return true;
            }
        }
    }
#elif defined(Q_OS_MACOS)
    // Обработка для macOS будет в отдельной функции-обработчике
#endif
    return false;
}

#ifdef Q_OS_MACOS
OSStatus HotKeyHandler(EventHandlerCallRef nextHandler, EventRef theEvent, void *userData)
{
    GlobalHotkeyManager* manager = (GlobalHotkeyManager*)userData;
    EventHotKeyID hotKeyID;
    GetEventParameter(theEvent, kEventParamDirectObject, typeEventHotKeyID, NULL, sizeof(hotKeyID), NULL, &hotKeyID);

    // Здесь нужно найти соответствующий hotKeyRef по hotKeyID.id
    // Это упрощение, в реальном коде потребуется более сложный маппинг.
    // Пока что будем считать, что мы можем найти trackRow.
    // emit manager->hotkeyActivated(trackRow);

    return CallNextEventHandler(nextHandler, theEvent);
}
#endif

// ============================================================================
// Wayland/xdg-desktop-portal implementation for Linux
// ============================================================================
#if defined(Q_OS_LINUX) && defined(USE_DBUS_PORTAL)

static QString generateToken()
{
    return QUuid::createUuid().toString(QUuid::Id128).left(12);
}

bool GlobalHotkeyManager::initPortal()
{
    // Check if we can connect to the portal service
    QDBusConnection bus = QDBusConnection::sessionBus();
    if (!bus.isConnected()) {
        qWarning() << "D-Bus session bus not available";
        return false;
    }

    // Try to access the GlobalShortcuts portal
    m_portalInterface = new QDBusInterface(
        "org.freedesktop.portal.Desktop",
        "/org/freedesktop/portal/desktop",
        "org.freedesktop.portal.GlobalShortcuts",
        bus,
        this
    );

    if (!m_portalInterface->isValid()) {
        qWarning() << "GlobalShortcuts portal not available:" << m_portalInterface->lastError().message();
        return false;
    }

    // Check portal version (we need at least version 1)
    QDBusMessage versionMsg = m_portalInterface->call("org.freedesktop.DBus.Properties.Get",
                                                        "org.freedesktop.portal.GlobalShortcuts", "version");
    if (versionMsg.type() == QDBusMessage::ReplyMessage) {
        uint version = versionMsg.arguments().first().toUInt();
        qDebug() << "GlobalShortcuts portal version:" << version;
        if (version < 1) {
            qWarning() << "GlobalShortcuts portal version" << version << "is too old (need >= 1)";
            return false;
        }
    }

    // Connect to portal signals - they have multiple parameters
    // We need to use a wrapper to extract the shortcut_id parameter
    // Activated signal: (session_handle o, shortcut_id s, timestamp t, options a{sv})
    QDBusConnection::sessionBus().connect("org.freedesktop.portal.Desktop",
                                          "/org/freedesktop/portal/desktop",
                                          "org.freedesktop.portal.GlobalShortcuts",
                                          "Activated",
                                          this,
                                          SLOT(onPortalActivated(QString)));

    // Deactivated signal: (session_handle o, shortcut_id s, timestamp t, options a{sv})
    QDBusConnection::sessionBus().connect("org.freedesktop.portal.Desktop",
                                          "/org/freedesktop/portal/desktop",
                                          "org.freedesktop.portal.GlobalShortcuts",
                                          "Deactivated",
                                          this,
                                          SLOT(onPortalDeactivated(QString)));

    // ShortcutsChanged signal: (session_handle o, shortcuts a(sa{sv}))
    QDBusConnection::sessionBus().connect("org.freedesktop.portal.Desktop",
                                          "/org/freedesktop/portal/desktop",
                                          "org.freedesktop.portal.GlobalShortcuts",
                                          "ShortcutsChanged",
                                          this,
                                          SLOT(onShortcutsChanged(QString)));

    return true;
}

void GlobalHotkeyManager::createPortalSession()
{
    if (!m_portalInterface || !m_portalInterface->isValid()) {
        qWarning() << "Cannot create portal session: interface not available";
        return;
    }

    m_sessionToken = generateToken();
    QString handleToken = generateToken();

    // Create the options vardict
    QVariantMap options;
    options["session_handle_token"] = m_sessionToken;
    options["handle_token"] = handleToken;

    // Call CreateSession asynchronously
    QDBusPendingCall call = m_portalInterface->asyncCall("CreateSession", options);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &GlobalHotkeyManager::onPortalSessionCreated);
}

void GlobalHotkeyManager::onPortalSessionCreated(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;
    watcher->deleteLater();

    if (reply.isError()) {
        qWarning() << "Failed to create portal session:" << reply.error().message();
        return;
    }

    // The reply contains a handle to a Request object
    QString requestPath = reply.value().path();
    qDebug() << "Created portal request:" << requestPath;

    // Connect to the Response signal on the Request object
    // Response signal: (response u, results a{sv})
    bool connected = QDBusConnection::sessionBus().connect(
        "org.freedesktop.portal.Desktop",
        requestPath,
        "org.freedesktop.portal.Request",
        "Response",
        this,
        SLOT(onPortalSessionResponse(uint, QVariantMap))
    );

    if (!connected) {
        qWarning() << "Failed to connect to Request::Response signal";
    }
}

void GlobalHotkeyManager::onPortalSessionResponse(uint response, const QVariantMap& results)
{
    if (response != 0) {
        qWarning() << "User denied permission for global shortcuts session or an error occurred";
        m_usePortal = false;
        return;
    }

    // Extract the session handle from results
    if (results.contains("session_handle")) {
        m_sessionHandle = results["session_handle"].toString();
        qDebug() << "Portal session created:" << m_sessionHandle;
    } else {
        // Fallback: construct session handle based on the token
        m_sessionHandle = QString("/org/freedesktop/portal/desktop/session/%1/%2")
                              .arg(QDBusConnection::sessionBus().baseService().replace(':', '_').replace('.', '_'))
                              .arg(m_sessionToken);
        qDebug() << "Portal session created (fallback):" << m_sessionHandle;
    }

    // Now bind any pending shortcuts
    if (!m_pendingPortalShortcuts.isEmpty()) {
        bindShortcutsToPortal();
    }
}

void GlobalHotkeyManager::bindShortcutsToPortal()
{
    if (!m_portalInterface || m_sessionHandle.isEmpty() || m_pendingPortalShortcuts.isEmpty()) {
        return;
    }

    // Build the shortcuts array
    QList<QVariant> shortcutsList;
    for (const PortalShortcut& shortcut : m_pendingPortalShortcuts) {
        QVariantMap shortcutInfo;
        shortcutInfo["description"] = shortcut.description;
        if (!shortcut.preferredTrigger.isEmpty()) {
            shortcutInfo["preferred_trigger"] = shortcut.preferredTrigger;
        }

        QList<QVariant> entry;
        entry << shortcut.shortcutId;
        entry << shortcutInfo;
        shortcutsList.append(QVariant(entry));
    }

    // Get the main window handle for parent_window
    QString parentWindow;
    if (QWindow* window = qApp->topLevelWindows().first()) {
        // For Wayland, we need the surface identifier
        // Format: "wayland:<handle>" or just empty string if not available
        parentWindow = "wayland:" + QString::number(window->winId());
    }

    QString handleToken = generateToken();
    QVariantMap options;
    options["handle_token"] = handleToken;

    // Call BindShortcuts
    QDBusPendingCall call = m_portalInterface->asyncCall("BindShortcuts",
                                                          m_sessionHandle,
                                                          shortcutsList,
                                                          parentWindow,
                                                          options);

    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &GlobalHotkeyManager::onShortcutsBound);
}

void GlobalHotkeyManager::onShortcutsBound(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply<QDBusObjectPath> reply = *watcher;
    watcher->deleteLater();

    if (reply.isError()) {
        qWarning() << "Failed to bind shortcuts:" << reply.error().message();
        return;
    }

    QString requestPath = reply.value().path();
    qDebug() << "BindShortcuts request created:" << requestPath;

    // Connect to the Response signal to get the result
    bool connected = QDBusConnection::sessionBus().connect(
        "org.freedesktop.portal.Desktop",
        requestPath,
        "org.freedesktop.portal.Request",
        "Response",
        this,
        SLOT(onBindShortcutsResponse(uint, QVariantMap))
    );

    if (!connected) {
        qWarning() << "Failed to connect to BindShortcuts Response signal";
    }
}

void GlobalHotkeyManager::onBindShortcutsResponse(uint response, const QVariantMap& results)
{
    if (response != 0) {
        qWarning() << "User denied permission for global shortcuts or an error occurred";
        return;
    }

    qDebug() << "Shortcuts bound successfully";

    // Parse the returned shortcuts
    if (results.contains("shortcuts")) {
        QList<QVariant> shortcuts = results["shortcuts"].toList();
        for (const QVariant& entry : shortcuts) {
            QList<QVariant> shortcutEntry = entry.toList();
            if (shortcutEntry.size() >= 2) {
                QString shortcutId = shortcutEntry[0].toString();
                QVariantMap info = shortcutEntry[1].toMap();
                QString triggerDesc = info.value("trigger_description").toString();
                qDebug() << "Bound shortcut:" << shortcutId << "->" << triggerDesc;
            }
        }
    }

    // Clear pending shortcuts as they're now bound
    m_pendingPortalShortcuts.clear();
}

void GlobalHotkeyManager::onPortalActivated(const QString& shortcutId)
{
    // Note: Portal sends multiple parameters, but we use Q_ARG in connect
    // to extract just the shortcut_id (second parameter)
    qDebug() << "Portal shortcut activated:" << shortcutId;
    if (m_portalIdToRow.contains(shortcutId)) {
        emit hotkeyActivated(m_portalIdToRow.value(shortcutId));
    }
}

void GlobalHotkeyManager::onPortalDeactivated(const QString& shortcutId)
{
    qDebug() << "Portal shortcut deactivated:" << shortcutId;
    // Not used for this application, but could be useful for future features
    Q_UNUSED(shortcutId)
}

void GlobalHotkeyManager::onShortcutsChanged(const QString& sessionHandle)
{
    qDebug() << "Portal shortcuts changed for session:" << sessionHandle;
    // Could re-list shortcuts here if needed
    Q_UNUSED(sessionHandle)
}

QString GlobalHotkeyManager::keySequenceToPortalTrigger(const QKeySequence& sequence) const
{
    if (sequence.isEmpty()) {
        return QString();
    }

    QKeyCombination combo = sequence[0];
    Qt::KeyboardModifiers modifiers = combo.keyboardModifiers();
    Qt::Key key = combo.key();

    QStringList parts;

    // Add modifiers in XDG format
    if (modifiers & Qt::ControlModifier) parts << "<Control>";
    if (modifiers & Qt::AltModifier) parts << "<Alt>";
    if (modifiers & Qt::ShiftModifier) parts << "<Shift>";
    if (modifiers & Qt::MetaModifier) parts << "<Super>";

    // Add the key
    parts << keyToPortalKey(key);

    return parts.join("");
}

QString GlobalHotkeyManager::keyToPortalKey(Qt::Key key) const
{
    // Function keys F1-F24
    if (key >= Qt::Key_F1 && key <= Qt::Key_F24) {
        return QString("F%1").arg(key - Qt::Key_F1 + 1);
    }

    // Number keys
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        return QString::number(key - Qt::Key_0);
    }

    // Letter keys
    if (key >= Qt::Key_A && key <= Qt::Key_Z) {
        return QString(QChar::fromLatin1('a' + (key - Qt::Key_A)));
    }

    // Special keys
    switch (key) {
    case Qt::Key_Escape: return "Escape";
    case Qt::Key_Tab: return "Tab";
    case Qt::Key_Backspace: return "BackSpace";
    case Qt::Key_Return: return "Return";
    case Qt::Key_Enter: return "Return";
    case Qt::Key_Insert: return "Insert";
    case Qt::Key_Delete: return "Delete";
    case Qt::Key_Home: return "Home";
    case Qt::Key_End: return "End";
    case Qt::Key_PageUp: return "Page_Up";
    case Qt::Key_PageDown: return "Page_Down";
    case Qt::Key_Left: return "Left";
    case Qt::Key_Right: return "Right";
    case Qt::Key_Up: return "Up";
    case Qt::Key_Down: return "Down";
    case Qt::Key_Space: return "space";
    case Qt::Key_Slash: return "slash";
    case Qt::Key_Backslash: return "backslash";
    case Qt::Key_BracketLeft: return "bracketleft";
    case Qt::Key_BracketRight: return "bracketright";
    case Qt::Key_Minus: return "minus";
    case Qt::Key_Equal: return "equal";
    case Qt::Key_Semicolon: return "semicolon";
    case Qt::Key_Apostrophe: return "apostrophe";
    case Qt::Key_Comma: return "comma";
    case Qt::Key_Period: return "period";
    case Qt::Key_grave: return "grave";
    default: break;
    }

    // For other keys, try to get the character
    QString text = QKeySequence(key).toString();
    if (!text.isEmpty()) {
        return text.toLower();
    }

    return QString();
}

void GlobalHotkeyManager::configureShortcuts()
{
    if (!m_portalInterface || m_sessionHandle.isEmpty()) {
        qWarning() << "Cannot configure shortcuts: portal not initialized";
        return;
    }

    // Get the main window handle for parent_window
    QString parentWindow;
    if (QWindow* window = qApp->topLevelWindows().first()) {
        parentWindow = "wayland:" + QString::number(window->winId());
    }

    QString handleToken = generateToken();
    QVariantMap options;
    options["handle_token"] = handleToken;

    // Call ConfigureShortcuts - this will show a configuration dialog
    QDBusMessage msg = m_portalInterface->call("ConfigureShortcuts",
                                              m_sessionHandle,
                                              parentWindow,
                                              options);

    if (msg.type() != QDBusMessage::ReplyMessage) {
        qWarning() << "Failed to open shortcuts configuration:" << msg.errorMessage();
        return;
    }

    qDebug() << "Shortcuts configuration dialog opened";
}

void GlobalHotkeyManager::unbindShortcutsFromPortal()
{
    // Re-bind with empty list to unbind all shortcuts
    // Note: In a full implementation, we might want to track which shortcuts
    // are currently bound and only unbind specific ones
    if (!m_portalInterface || m_sessionHandle.isEmpty()) {
        return;
    }

    // To unbind, we call BindShortcuts with an empty list
    QList<QVariant> emptyShortcutsList;
    QString parentWindow;
    if (QWindow* window = qApp->topLevelWindows().first()) {
        parentWindow = "wayland:" + QString::number(window->winId());
    }

    QVariantMap options;
    options["handle_token"] = generateToken();

    m_portalInterface->asyncCall("BindShortcuts",
                                  m_sessionHandle,
                                  emptyShortcutsList,
                                  parentWindow,
                                  options);
}

#endif // defined(Q_OS_LINUX) && defined(USE_DBUS_PORTAL)
