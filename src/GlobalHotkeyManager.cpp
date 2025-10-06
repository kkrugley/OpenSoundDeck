#include "GlobalHotkeyManager.h"
#include <QDebug>
#include <QApplication>

#ifdef Q_OS_WIN
#include <Windows.h>
#elif defined(Q_OS_LINUX)
#include <QGuiApplication>
#include <X11/Xlib.h>
#include <xcb/xcb.h> // <--- Добавляем заголовок для XCB
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
}

GlobalHotkeyManager::~GlobalHotkeyManager()
{
    unregisterAll();
    qApp->removeNativeEventFilter(this);
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
    QNativeInterface::QX11Application* x11App = qApp->nativeInterface<QNativeInterface::QX11Application>();
    if (!x11App) {
        qWarning() << "Cannot register global hotkey: not running on X11.";
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
