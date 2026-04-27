/* src/Settings.cpp */
/*
* OpenSoundDeck
* Copyright (C) 2025 Pavel Kruhlei
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "Settings.h"
#include <QStandardPaths>
#include <QDebug>

Settings* Settings::s_instance = nullptr;

Settings* Settings::instance()
{
    if (!s_instance) {
        s_instance = new Settings();
    }
    return s_instance;
}

void Settings::destroyInstance()
{
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings("pavel-kruhlei", "OpenSoundDeck", this))
{
    qDebug() << "Settings initialized. File location:" << m_settings->fileName();
}

Settings::~Settings()
{
    m_settings->sync();
}

void Settings::sync()
{
    m_settings->sync();
    emit settingsChanged();
}

// ============================================================================
// General Settings
// ============================================================================

QString Settings::language() const
{
    return m_settings->value("general/language", "system").toString();
}

void Settings::setLanguage(const QString& lang)
{
    QString oldLang = m_settings->value("general/language").toString();
    if (oldLang != lang) {
        m_settings->setValue("general/language", lang);
        emit languageChanged(lang);
        emit settingsChanged();
    }
}

ThemeMode Settings::theme() const
{
    return static_cast<ThemeMode>(m_settings->value("general/theme",
        static_cast<int>(DEFAULT_THEME)).toInt());
}

void Settings::setTheme(ThemeMode theme)
{
    ThemeMode oldTheme = static_cast<ThemeMode>(m_settings->value("general/theme").toInt());
    if (oldTheme != theme) {
        m_settings->setValue("general/theme", static_cast<int>(theme));
        emit themeChanged(theme);
        emit settingsChanged();
    }
}

bool Settings::startupWithSystem() const
{
    return m_settings->value("general/startupWithSystem", DEFAULT_STARTUP_WITH_SYSTEM).toBool();
}

void Settings::setStartupWithSystem(bool enabled)
{
    m_settings->setValue("general/startupWithSystem", enabled);
    emit settingsChanged();
}

bool Settings::startMinimizedToTray() const
{
    return m_settings->value("general/startMinimizedToTray", DEFAULT_START_MINIMIZED).toBool();
}

void Settings::setStartMinimizedToTray(bool enabled)
{
    m_settings->setValue("general/startMinimizedToTray", enabled);
    emit settingsChanged();
}

bool Settings::autoSave() const
{
    return m_settings->value("general/autoSave", DEFAULT_AUTO_SAVE).toBool();
}

void Settings::setAutoSave(bool enabled)
{
    m_settings->setValue("general/autoSave", enabled);
    emit settingsChanged();
}

QString Settings::libraryPath() const
{
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    return m_settings->value("general/libraryPath", defaultPath).toString();
}

void Settings::setLibraryPath(const QString& path)
{
    m_settings->setValue("general/libraryPath", path);
    emit settingsChanged();
}

bool Settings::checkForUpdates() const
{
    return m_settings->value("general/checkForUpdates", true).toBool();
}

void Settings::setCheckForUpdates(bool enabled)
{
    m_settings->setValue("general/checkForUpdates", enabled);
    emit settingsChanged();
}

// ============================================================================
// Audio Settings
// ============================================================================

AudioOutputMode Settings::defaultOutputMode() const
{
    return static_cast<AudioOutputMode>(m_settings->value("audio/defaultOutputMode",
        static_cast<int>(DEFAULT_OUTPUT_MODE)).toInt());
}

void Settings::setDefaultOutputMode(AudioOutputMode mode)
{
    m_settings->setValue("audio/defaultOutputMode", static_cast<int>(mode));
    emit settingsChanged();
}

bool Settings::normalizationEnabled() const
{
    return m_settings->value("audio/normalizationEnabled", DEFAULT_NORMALIZATION).toBool();
}

void Settings::setNormalizationEnabled(bool enabled)
{
    m_settings->setValue("audio/normalizationEnabled", enabled);
    emit settingsChanged();
}

float Settings::voiceCalibrationLevel() const
{
    return m_settings->value("audio/voiceCalibrationLevel", DEFAULT_VOICE_CALIBRATION).toFloat();
}

void Settings::setVoiceCalibrationLevel(float level)
{
    m_settings->setValue("audio/voiceCalibrationLevel", level);
    emit settingsChanged();
}

bool Settings::voiceBlockerEnabled() const
{
    return m_settings->value("audio/voiceBlockerEnabled", DEFAULT_VOICE_BLOCKER).toBool();
}

void Settings::setVoiceBlockerEnabled(bool enabled)
{
    m_settings->setValue("audio/voiceBlockerEnabled", enabled);
    emit settingsChanged();
}

int Settings::bufferSize() const
{
    return m_settings->value("audio/bufferSize", DEFAULT_BUFFER_SIZE).toInt();
}

void Settings::setBufferSize(int size)
{
    m_settings->setValue("audio/bufferSize", size);
    emit settingsChanged();
}

int Settings::sampleRate() const
{
    return m_settings->value("audio/sampleRate", DEFAULT_SAMPLE_RATE).toInt();
}

void Settings::setSampleRate(int rate)
{
    m_settings->setValue("audio/sampleRate", rate);
    emit settingsChanged();
}

int Settings::fadeDuration() const
{
    return m_settings->value("audio/fadeDuration", DEFAULT_FADE_DURATION).toInt();
}

void Settings::setFadeDuration(int duration)
{
    m_settings->setValue("audio/fadeDuration", duration);
    emit settingsChanged();
}

// ============================================================================
// Hotkey Settings
// ============================================================================

bool Settings::numpadModeEnabled() const
{
    return m_settings->value("hotkeys/numpadModeEnabled", DEFAULT_NUMPAD_MODE).toBool();
}

void Settings::setNumpadModeEnabled(bool enabled)
{
    m_settings->setValue("hotkeys/numpadModeEnabled", enabled);
    emit settingsChanged();
}

bool Settings::indexHotkeysEnabled() const
{
    return m_settings->value("hotkeys/indexHotkeysEnabled", DEFAULT_INDEX_HOTKEYS).toBool();
}

void Settings::setIndexHotkeysEnabled(bool enabled)
{
    m_settings->setValue("hotkeys/indexHotkeysEnabled", enabled);
    emit settingsChanged();
}

bool Settings::specialHotkeysEnabled() const
{
    return m_settings->value("hotkeys/specialHotkeysEnabled", DEFAULT_SPECIAL_HOTKEYS).toBool();
}

void Settings::setSpecialHotkeysEnabled(bool enabled)
{
    m_settings->setValue("hotkeys/specialHotkeysEnabled", enabled);
    emit settingsChanged();
}

bool Settings::conflictDetectionEnabled() const
{
    return m_settings->value("hotkeys/conflictDetectionEnabled", DEFAULT_CONFLICT_DETECTION).toBool();
}

void Settings::setConflictDetectionEnabled(bool enabled)
{
    m_settings->setValue("hotkeys/conflictDetectionEnabled", enabled);
    emit settingsChanged();
}

// ============================================================================
// Device Settings
// ============================================================================

QString Settings::inputDevice() const
{
    return m_settings->value("devices/inputDevice", QString()).toString();
}

void Settings::setInputDevice(const QString& device)
{
    m_settings->setValue("devices/inputDevice", device);
    emit settingsChanged();
}

QString Settings::outputDevice() const
{
    return m_settings->value("devices/outputDevice", QString()).toString();
}

void Settings::setOutputDevice(const QString& device)
{
    m_settings->setValue("devices/outputDevice", device);
    emit settingsChanged();
}

QString Settings::virtualDevice() const
{
    return m_settings->value("devices/virtualDevice", QString()).toString();
}

void Settings::setVirtualDevice(const QString& device)
{
    m_settings->setValue("devices/virtualDevice", device);
    emit settingsChanged();
}

DeviceFallbackMode Settings::deviceFallbackMode() const
{
    return static_cast<DeviceFallbackMode>(m_settings->value("devices/fallbackMode",
        static_cast<int>(DEFAULT_FALLBACK_MODE)).toInt());
}

void Settings::setDeviceFallbackMode(DeviceFallbackMode mode)
{
    m_settings->setValue("devices/fallbackMode", static_cast<int>(mode));
    emit settingsChanged();
}

// ============================================================================
// Reset to Defaults
// ============================================================================

void Settings::resetToDefaults()
{
    m_settings->clear();
    emit settingsChanged();
    sync();
}
