/* src/Settings.h */
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

#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>

// Audio output modes
enum class AudioOutputMode {
    Speakers = 0, // Local monitoring only
    Mic = 1,      // Virtual mic only (others hear)
    Both = 2      // Both (default)
};

// Theme modes
enum class ThemeMode {
    Light = 0,
    Dark = 1,
    System = 2
};

// Device fallback behavior
enum class DeviceFallbackMode {
    Pause = 0,      // Pause playback
    AutoSwitch = 1, // Try to switch to available device
    Ignore = 2      // Continue with null device
};

class Settings : public QObject
{
    Q_OBJECT

public:
    static Settings* instance();
    static void destroyInstance();

    // General
    QString language() const;
    void setLanguage(const QString& lang);

    ThemeMode theme() const;
    void setTheme(ThemeMode theme);

    bool startupWithSystem() const;
    void setStartupWithSystem(bool enabled);

    bool startMinimizedToTray() const;
    void setStartMinimizedToTray(bool enabled);

    bool autoSave() const;
    void setAutoSave(bool enabled);

    QString libraryPath() const;
    void setLibraryPath(const QString& path);

    bool checkForUpdates() const;
    void setCheckForUpdates(bool enabled);

    // Audio
    AudioOutputMode defaultOutputMode() const;
    void setDefaultOutputMode(AudioOutputMode mode);

    bool normalizationEnabled() const;
    void setNormalizationEnabled(bool enabled);

    float voiceCalibrationLevel() const;
    void setVoiceCalibrationLevel(float level);

    bool voiceBlockerEnabled() const;
    void setVoiceBlockerEnabled(bool enabled);

    int bufferSize() const;
    void setBufferSize(int size);

    int sampleRate() const;
    void setSampleRate(int rate);

    int fadeDuration() const; // in milliseconds
    void setFadeDuration(int duration);

    // Hotkeys
    bool numpadModeEnabled() const;
    void setNumpadModeEnabled(bool enabled);

    bool indexHotkeysEnabled() const;
    void setIndexHotkeysEnabled(bool enabled);

    bool specialHotkeysEnabled() const;
    void setSpecialHotkeysEnabled(bool enabled);

    bool conflictDetectionEnabled() const;
    void setConflictDetectionEnabled(bool enabled);

    // Devices
    QString inputDevice() const;
    void setInputDevice(const QString& device);

    QString outputDevice() const;
    void setOutputDevice(const QString& device);

    QString virtualDevice() const;
    void setVirtualDevice(const QString& device);

    DeviceFallbackMode deviceFallbackMode() const;
    void setDeviceFallbackMode(DeviceFallbackMode mode);

    // Reset to defaults
    void resetToDefaults();

    // Sync to disk
    void sync();

signals:
    void settingsChanged();
    void themeChanged(ThemeMode newTheme);
    void languageChanged(const QString& newLanguage);

private:
    explicit Settings(QObject *parent = nullptr);
    ~Settings();

    QSettings* m_settings;
    static Settings* s_instance;

    // Default values
    static constexpr ThemeMode DEFAULT_THEME = ThemeMode::System;
    static constexpr bool DEFAULT_STARTUP_WITH_SYSTEM = false;
    static constexpr bool DEFAULT_START_MINIMIZED = false;
    static constexpr bool DEFAULT_AUTO_SAVE = true;
    static constexpr AudioOutputMode DEFAULT_OUTPUT_MODE = AudioOutputMode::Both;
    static constexpr bool DEFAULT_NORMALIZATION = false;
    static constexpr float DEFAULT_VOICE_CALIBRATION = 0.0f;
    static constexpr bool DEFAULT_VOICE_BLOCKER = false;
    static constexpr int DEFAULT_BUFFER_SIZE = 512;
    static constexpr int DEFAULT_SAMPLE_RATE = 48000;
    static constexpr int DEFAULT_FADE_DURATION = 50;
    static constexpr bool DEFAULT_NUMPAD_MODE = true;
    static constexpr bool DEFAULT_INDEX_HOTKEYS = true;
    static constexpr bool DEFAULT_SPECIAL_HOTKEYS = false;
    static constexpr bool DEFAULT_CONFLICT_DETECTION = true;
    static constexpr DeviceFallbackMode DEFAULT_FALLBACK_MODE = DeviceFallbackMode::AutoSwitch;
};
