#pragma once

#include <QtGlobal> // For qintptr and qHash
#include <QObject>
#include <QAbstractNativeEventFilter>
#include <QKeySequence>
#include <QHash>

// Forward declarations for D-Bus on Linux
#if defined(Q_OS_LINUX) && defined(USE_DBUS_PORTAL)
class QDBusInterface;
class QDBusPendingCallWatcher;
#endif

class GlobalHotkeyManager : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT

public:
    explicit GlobalHotkeyManager(QObject *parent = nullptr);
    ~GlobalHotkeyManager();

    // Платформо-зависимые детали
#ifdef Q_OS_LINUX
    // Для Linux (X11) мы будем использовать комбинацию KeyCode + Modifiers
    // Структура должна быть public, чтобы qHash мог ее видеть
    struct X11Hotkey {
        uint keycode;
        uint modifiers;
        bool operator==(const X11Hotkey& other) const {
            return keycode == other.keycode && modifiers == other.modifiers;
        }
    };

    // Wayland/Portal структуры (только когда USE_DBUS_PORTAL определен)
    #if defined(USE_DBUS_PORTAL)
    struct PortalShortcut {
        QString shortcutId;
        QString description;
        QString preferredTrigger;
        int trackRow;
    };
    #endif
#endif

    bool registerHotkey(const QKeySequence& sequence, int trackRow);
    void unregisterHotkey(int trackRow);
    void unregisterAll();

signals:
    void hotkeyActivated(int trackRow);

protected:
    // Эта функция будет перехватывать системные события
    bool nativeEventFilter(const QByteArray &eventType, void *message, qintptr *result) override;

private:
    // Хранит соответствие между хоткеем и строкой трека
    QHash<QKeySequence, int> m_registeredHotkeys;

    // Платформо-зависимые детали
#ifdef Q_OS_WIN
    // Для Windows мы будем использовать числовые ID для хоткеев
    QHash<int, int> m_nativeKeyToRow; // <Native ID, Track Row>
    int m_nextNativeId = 1;
#elif defined(Q_OS_LINUX)
    QHash<X11Hotkey, int> m_nativeKeyToRow;

    // Wayland/xdg-desktop-portal support
    #if defined(USE_DBUS_PORTAL)
    bool initPortal();
    void createPortalSession();
    void bindShortcutsToPortal();
    void unbindShortcutsFromPortal();
    void configureShortcuts();
    QString keySequenceToPortalTrigger(const QKeySequence& sequence) const;
    QString keyToPortalKey(Qt::Key key) const;

    bool m_usePortal = false;
    QDBusInterface* m_portalInterface = nullptr;
    QString m_sessionHandle;
    QString m_sessionToken;
    QList<PortalShortcut> m_pendingPortalShortcuts;
    QHash<QString, int> m_portalIdToRow; // shortcutId -> trackRow
    int m_nextPortalId = 1;
    #endif
#elif defined(Q_OS_MACOS)
    // Для macOS будем использовать EventHotKeyRef
    QHash<void*, int> m_nativeKeyToRow; // <EventHotKeyRef, Track Row>
#endif

private slots:
#if defined(Q_OS_LINUX) && defined(USE_DBUS_PORTAL)
    void onPortalSessionCreated(QDBusPendingCallWatcher* watcher);
    void onShortcutsBound(QDBusPendingCallWatcher* watcher);
    void onPortalSessionResponse(uint response, const QVariantMap& results);
    void onBindShortcutsResponse(uint response, const QVariantMap& results);
    void onPortalActivated(const QString& shortcutId);
    void onPortalDeactivated(const QString& shortcutId);
    void onShortcutsChanged(const QString& sessionHandle);
#endif
};

// --- ИСПРАВЛЕНИЕ для QHash ---
// Хэш-функция для нашей структуры X11Hotkey.
// Должна быть в глобальном пространстве имен или в std.
inline uint qHash(const GlobalHotkeyManager::X11Hotkey &key, uint seed = 0) {
    return qHash(key.keycode, seed) ^ qHash(key.modifiers, seed);
}
