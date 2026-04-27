/* src/SettingsDialog.h */
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

#include <QDialog>
#include <QMap>

class QTabWidget;
class QLineEdit;
class QPushButton;
class QDialogButtonBox;
class QComboBox;
class QCheckBox;
class QSpinBox;
class QSlider;
class QLabel;
class QListWidget;
class QGroupBox;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    bool needsRestart() const { return m_needsRestart; }

private slots:
    void onAccepted();
    void onBrowseLibraryPath();
    void onExportSettings();
    void onImportSettings();
    void onResetDefaults();
    void onCheckForUpdates();
    void onBufferSizeChanged(int index);
    void onSampleRateChanged(int index);
    void onThemeChanged();
    void onLanguageChanged();

private:
    void loadSettings();
    void saveSettings();
    void applySettingsImmediately();
    void applyTheme();

    bool m_needsRestart = false;

    // Tab creation methods
    QWidget* createGeneralTab();
    QWidget* createAudioTab();
    QWidget* createHotkeysTab();
    QWidget* createDevicesTab();

    // General Tab
    QComboBox* m_languageCombo;
    QComboBox* m_themeCombo;
    QCheckBox* m_startupWithSystemCheck;
    QCheckBox* m_startMinimizedCheck;
    QCheckBox* m_autoSaveCheck;
    QCheckBox* m_checkUpdatesCheck;
    QLineEdit* m_libraryPathLineEdit;
    QPushButton* m_browsePathButton;
    QPushButton* m_exportButton;
    QPushButton* m_importButton;
    QPushButton* m_resetButton;

    // Audio Tab
    QComboBox* m_defaultOutputModeCombo;
    QCheckBox* m_normalizationCheck;
    QSlider* m_voiceCalibrationSlider;
    QLabel* m_calibrationValueLabel;
    QCheckBox* m_voiceBlockerCheck;
    QComboBox* m_bufferSizeCombo;
    QComboBox* m_sampleRateCombo;
    QSpinBox* m_fadeDurationSpin;

    // Hotkeys Tab
    QCheckBox* m_numpadModeCheck;
    QCheckBox* m_indexHotkeysCheck;
    QCheckBox* m_specialHotkeysCheck;
    QCheckBox* m_conflictDetectionCheck;
    QLabel* m_hotkeyListLabel;

    // Devices Tab
    QComboBox* m_inputDeviceCombo;
    QComboBox* m_outputDeviceCombo;
    QComboBox* m_virtualDeviceCombo;
    QComboBox* m_fallbackModeCombo;
    QPushButton* m_refreshDevicesButton;

    QTabWidget* m_tabWidget;
    QDialogButtonBox* m_buttonBox;
};
