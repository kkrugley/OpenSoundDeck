/* src/SettingsDialog.cpp */
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

#include "SettingsDialog.h"
#include "Settings.h"
#include "AudioEngine.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSettings>
#include <QDebug>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QSlider>
#include <QGroupBox>
#include <QMessageBox>
#include <QListWidget>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
    , m_needsRestart(false)
{
    setWindowTitle(tr("Settings"));
    setMinimumSize(640, 480);
    resize(640, 480);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(createGeneralTab(), tr("General"));
    m_tabWidget->addTab(createAudioTab(), tr("Audio"));
    m_tabWidget->addTab(createHotkeysTab(), tr("Hotkeys"));
    m_tabWidget->addTab(createDevicesTab(), tr("Devices"));

    m_buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply,
        this);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked,
        this, &SettingsDialog::saveSettings);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);
    mainLayout->addWidget(m_buttonBox);

    loadSettings();
}

void SettingsDialog::loadSettings()
{
    Settings* s = Settings::instance();

    // General
    int langIndex = m_languageCombo->findData(s->language());
    if (langIndex >= 0) m_languageCombo->setCurrentIndex(langIndex);

    int themeIndex = m_themeCombo->findData(static_cast<int>(s->theme()));
    if (themeIndex >= 0) m_themeCombo->setCurrentIndex(themeIndex);

    m_startupWithSystemCheck->setChecked(s->startupWithSystem());
    m_startMinimizedCheck->setChecked(s->startMinimizedToTray());
    m_autoSaveCheck->setChecked(s->autoSave());
    m_checkUpdatesCheck->setChecked(s->checkForUpdates());
    m_libraryPathLineEdit->setText(s->libraryPath());

    // Audio
    int modeIndex = m_defaultOutputModeCombo->findData(static_cast<int>(s->defaultOutputMode()));
    if (modeIndex >= 0) m_defaultOutputModeCombo->setCurrentIndex(modeIndex);

    m_normalizationCheck->setChecked(s->normalizationEnabled());
    m_voiceCalibrationSlider->setValue(static_cast<int>(s->voiceCalibrationLevel() * 100));
    m_calibrationValueLabel->setText(QString::number(static_cast<int>(s->voiceCalibrationLevel() * 100)) + "%");
    m_voiceBlockerCheck->setChecked(s->voiceBlockerEnabled());

    int bufferIndex = m_bufferSizeCombo->findData(s->bufferSize());
    if (bufferIndex >= 0) m_bufferSizeCombo->setCurrentIndex(bufferIndex);

    int sampleRateIndex = m_sampleRateCombo->findData(s->sampleRate());
    if (sampleRateIndex >= 0) m_sampleRateCombo->setCurrentIndex(sampleRateIndex);

    m_fadeDurationSpin->setValue(s->fadeDuration());

    // Hotkeys
    m_numpadModeCheck->setChecked(s->numpadModeEnabled());
    m_indexHotkeysCheck->setChecked(s->indexHotkeysEnabled());
    m_specialHotkeysCheck->setChecked(s->specialHotkeysEnabled());
    m_conflictDetectionCheck->setChecked(s->conflictDetectionEnabled());

    // Devices - will be populated when devices are detected
    int fallbackIndex = m_fallbackModeCombo->findData(static_cast<int>(s->deviceFallbackMode()));
    if (fallbackIndex >= 0) m_fallbackModeCombo->setCurrentIndex(fallbackIndex);

    qDebug() << "Settings loaded successfully";
}

void SettingsDialog::saveSettings()
{
    Settings* s = Settings::instance();

    // Check for theme change
    ThemeMode oldTheme = s->theme();
    ThemeMode newTheme = static_cast<ThemeMode>(m_themeCombo->currentData().toInt());

    // Check for language change
    QString oldLang = s->language();
    QString newLang = m_languageCombo->currentData().toString();

    // General
    s->setLanguage(newLang);
    s->setTheme(newTheme);
    s->setStartupWithSystem(m_startupWithSystemCheck->isChecked());
    s->setStartMinimizedToTray(m_startMinimizedCheck->isChecked());
    s->setAutoSave(m_autoSaveCheck->isChecked());
    s->setCheckForUpdates(m_checkUpdatesCheck->isChecked());
    s->setLibraryPath(m_libraryPathLineEdit->text());

    // Audio
    s->setDefaultOutputMode(static_cast<AudioOutputMode>(m_defaultOutputModeCombo->currentData().toInt()));
    s->setNormalizationEnabled(m_normalizationCheck->isChecked());
    s->setVoiceCalibrationLevel(m_voiceCalibrationSlider->value() / 100.0f);
    s->setVoiceBlockerEnabled(m_voiceBlockerCheck->isChecked());
    s->setBufferSize(m_bufferSizeCombo->currentData().toInt());
    s->setSampleRate(m_sampleRateCombo->currentData().toInt());
    s->setFadeDuration(m_fadeDurationSpin->value());

    // Hotkeys
    s->setNumpadModeEnabled(m_numpadModeCheck->isChecked());
    s->setIndexHotkeysEnabled(m_indexHotkeysCheck->isChecked());
    s->setSpecialHotkeysEnabled(m_specialHotkeysCheck->isChecked());
    s->setConflictDetectionEnabled(m_conflictDetectionCheck->isChecked());

    // Devices
    s->setInputDevice(m_inputDeviceCombo->currentData().toString());
    s->setOutputDevice(m_outputDeviceCombo->currentData().toString());
    s->setVirtualDevice(m_virtualDeviceCombo->currentData().toString());
    s->setDeviceFallbackMode(static_cast<DeviceFallbackMode>(m_fallbackModeCombo->currentData().toInt()));

    s->sync();
    qDebug() << "Settings saved";

    // Apply theme immediately if changed
    if (oldTheme != newTheme) {
        applyTheme();
    }

    // Check if language changed
    if (oldLang != newLang) {
        m_needsRestart = true;
        int ret = QMessageBox::question(this, tr("Language Changed"),
            tr("The language setting has been changed. The application needs to restart for the change to take effect.\n\n"
               "Do you want to restart the application now?"),
            QMessageBox::Yes | QMessageBox::No);

        if (ret == QMessageBox::Yes) {
            // Signal main window to restart
            QApplication::exit(100); // Special exit code for restart
        }
    }
}

void SettingsDialog::onAccepted()
{
    saveSettings();
    accept();
}

void SettingsDialog::onBrowseLibraryPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Library Folder"),
        m_libraryPathLineEdit->text(),
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        m_libraryPathLineEdit->setText(dir);
    }
}

void SettingsDialog::onExportSettings()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Export Settings"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/opensounddeck-backup.json",
        tr("JSON Files (*.json)"));

    if (fileName.isEmpty()) return;

    QSettings originalSettings("pavel-kruhlei", "OpenSoundDeck");
    QJsonObject settingsObj;

    for (const QString& key : originalSettings.allKeys()) {
        settingsObj[key] = originalSettings.value(key).toString();
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(settingsObj).toJson());
        file.close();
        QMessageBox::information(this, tr("Export Complete"),
            tr("Settings exported successfully to:\n%1").arg(fileName));
    } else {
        QMessageBox::warning(this, tr("Export Failed"),
            tr("Could not write to file:\n%1").arg(fileName));
    }
}

void SettingsDialog::onImportSettings()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import Settings"),
        QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation),
        tr("JSON Files (*.json)"));

    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Import Failed"),
            tr("Could not read file:\n%1").arg(fileName));
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isObject()) {
        QMessageBox::warning(this, tr("Import Failed"),
            tr("Invalid settings file format."));
        return;
    }

    int ret = QMessageBox::question(this, tr("Confirm Import"),
        tr("This will overwrite all current settings. Continue?"),
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        QJsonObject settingsObj = doc.object();
        QSettings destSettings("pavel-kruhlei", "OpenSoundDeck");
        destSettings.clear();

        for (auto it = settingsObj.begin(); it != settingsObj.end(); ++it) {
            destSettings.setValue(it.key(), it.value().toVariant());
        }

        loadSettings();
        QMessageBox::information(this, tr("Import Complete"),
            tr("Settings imported successfully.\n\n"
               "Some changes may require a restart to take full effect."));
    }
}

void SettingsDialog::onResetDefaults()
{
    int ret = QMessageBox::warning(this, tr("Reset Settings"),
        tr("This will reset ALL settings to default values. This cannot be undone.\n\nContinue?"),
        QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        Settings::instance()->resetToDefaults();
        loadSettings();
        QMessageBox::information(this, tr("Settings Reset"),
            tr("All settings have been reset to defaults.\n\n"
               "Some changes may require a restart to take full effect."));
    }
}

void SettingsDialog::onCheckForUpdates()
{
    QMessageBox::information(this, tr("Check for Updates"),
        tr("Update checking will be implemented soon.\n\n"
           "Please check the GitHub repository for the latest version."));
}

void SettingsDialog::onBufferSizeChanged(int index)
{
    Q_UNUSED(index)
}

void SettingsDialog::onSampleRateChanged(int index)
{
    Q_UNUSED(index)
}

void SettingsDialog::onThemeChanged()
{
    // Theme is applied when clicking Apply or OK
}

void SettingsDialog::onLanguageChanged()
{
    // Language requires restart - handled in saveSettings
}

void SettingsDialog::applySettingsImmediately()
{
    // Apply theme
    applyTheme();
}

void SettingsDialog::applyTheme()
{
    Settings* s = Settings::instance();
    ThemeMode theme = s->theme();

    // This is a simplified theme application
    // In reality, we need access to the setupTheme functions from main.cpp
    // For now, we emit a signal or use a callback
    // This will be connected in MainWindow
    emit s->settingsChanged();
}

// ============================================================================
// Tab Creation Methods
// ============================================================================

QWidget* SettingsDialog::createGeneralTab()
{
    QWidget *widget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    mainLayout->setSpacing(15);

    // Language & Theme Group
    QGroupBox *appearanceGroup = new QGroupBox(tr("Appearance"), this);
    QFormLayout *appearanceLayout = new QFormLayout(appearanceGroup);

    m_languageCombo = new QComboBox(this);
    m_languageCombo->addItem(tr("System Default"), "system");
    m_languageCombo->addItem(tr("English"), "en");
    m_languageCombo->addItem(tr("Russian"), "ru");
    m_languageCombo->addItem(tr("German"), "de");
    m_languageCombo->addItem(tr("French"), "fr");
    m_languageCombo->addItem(tr("Spanish"), "es");
    connect(m_languageCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsDialog::onLanguageChanged);
    appearanceLayout->addRow(tr("Language:"), m_languageCombo);

    m_themeCombo = new QComboBox(this);
    m_themeCombo->addItem(tr("Light"), static_cast<int>(ThemeMode::Light));
    m_themeCombo->addItem(tr("Dark"), static_cast<int>(ThemeMode::Dark));
    m_themeCombo->addItem(tr("System"), static_cast<int>(ThemeMode::System));
    connect(m_themeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsDialog::onThemeChanged);
    appearanceLayout->addRow(tr("Theme:"), m_themeCombo);

    mainLayout->addWidget(appearanceGroup);

    // Startup Group
    QGroupBox *startupGroup = new QGroupBox(tr("Startup"), this);
    QVBoxLayout *startupLayout = new QVBoxLayout(startupGroup);

    m_startupWithSystemCheck = new QCheckBox(tr("Start with system"), this);
    m_startMinimizedCheck = new QCheckBox(tr("Start minimized to system tray"), this);
    m_autoSaveCheck = new QCheckBox(tr("Auto-save sound list on changes"), this);
    m_checkUpdatesCheck = new QCheckBox(tr("Check for updates on startup"), this);

    startupLayout->addWidget(m_startupWithSystemCheck);
    startupLayout->addWidget(m_startMinimizedCheck);
    startupLayout->addWidget(m_autoSaveCheck);
    startupLayout->addWidget(m_checkUpdatesCheck);

    mainLayout->addWidget(startupGroup);

    // Library Path Group
    QGroupBox *libraryGroup = new QGroupBox(tr("Sound Library"), this);
    QHBoxLayout *libraryLayout = new QHBoxLayout(libraryGroup);

    m_libraryPathLineEdit = new QLineEdit(this);
    m_browsePathButton = new QPushButton(tr("Browse..."), this);
    connect(m_browsePathButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseLibraryPath);

    libraryLayout->addWidget(m_libraryPathLineEdit, 1);
    libraryLayout->addWidget(m_browsePathButton);

    mainLayout->addWidget(libraryGroup);

    // Backup & Reset Group
    QGroupBox *backupGroup = new QGroupBox(tr("Backup & Reset"), this);
    QHBoxLayout *backupLayout = new QHBoxLayout(backupGroup);

    m_exportButton = new QPushButton(tr("Export Settings..."), this);
    m_importButton = new QPushButton(tr("Import Settings..."), this);
    m_resetButton = new QPushButton(tr("Reset to Defaults"), this);
    m_resetButton->setStyleSheet("QPushButton { color: red; }");

    connect(m_exportButton, &QPushButton::clicked, this, &SettingsDialog::onExportSettings);
    connect(m_importButton, &QPushButton::clicked, this, &SettingsDialog::onImportSettings);
    connect(m_resetButton, &QPushButton::clicked, this, &SettingsDialog::onResetDefaults);

    backupLayout->addWidget(m_exportButton);
    backupLayout->addWidget(m_importButton);
    backupLayout->addStretch();
    backupLayout->addWidget(m_resetButton);

    mainLayout->addWidget(backupGroup);
    mainLayout->addStretch();

    return widget;
}

QWidget* SettingsDialog::createAudioTab()
{
    QWidget *widget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    mainLayout->setSpacing(15);

    // Output Mode Group
    QGroupBox *outputGroup = new QGroupBox(tr("Default Output Mode"), this);
    QVBoxLayout *outputLayout = new QVBoxLayout(outputGroup);

    QLabel *outputHelpLabel = new QLabel(
        tr("This determines where new sounds play by default:"), this);
    outputLayout->addWidget(outputHelpLabel);

    m_defaultOutputModeCombo = new QComboBox(this);
    m_defaultOutputModeCombo->addItem(tr("Speakers only (you hear)"), static_cast<int>(AudioOutputMode::Speakers));
    m_defaultOutputModeCombo->addItem(tr("Microphone only (others hear)"), static_cast<int>(AudioOutputMode::Mic));
    m_defaultOutputModeCombo->addItem(tr("Both (default)"), static_cast<int>(AudioOutputMode::Both));
    outputLayout->addWidget(m_defaultOutputModeCombo);

    mainLayout->addWidget(outputGroup);

    // Volume & Normalization Group
    QGroupBox *volumeGroup = new QGroupBox(tr("Volume Control"), this);
    QFormLayout *volumeLayout = new QFormLayout(volumeGroup);

    m_normalizationCheck = new QCheckBox(tr("Enable auto-normalization"), this);
    volumeLayout->addRow(QString(), m_normalizationCheck);

    QHBoxLayout *calibrationLayout = new QHBoxLayout;
    m_voiceCalibrationSlider = new QSlider(Qt::Horizontal, this);
    m_voiceCalibrationSlider->setRange(0, 100);
    m_calibrationValueLabel = new QLabel("0%", this);
    connect(m_voiceCalibrationSlider, &QSlider::valueChanged, this, [this](int value) {
        m_calibrationValueLabel->setText(QString::number(value) + "%");
    });
    calibrationLayout->addWidget(m_voiceCalibrationSlider);
    calibrationLayout->addWidget(m_calibrationValueLabel);
    volumeLayout->addRow(tr("Voice calibration level:"), calibrationLayout);

    m_voiceBlockerCheck = new QCheckBox(tr("Block voice during sound playback"), this);
    QLabel *voiceBlockerHelp = new QLabel(
        tr("When enabled, microphone is muted while playing sounds (cleaner output)"), this);
    voiceBlockerHelp->setStyleSheet("color: gray; font-size: 11px;");
    volumeLayout->addRow(QString(), m_voiceBlockerCheck);
    volumeLayout->addRow(QString(), voiceBlockerHelp);

    mainLayout->addWidget(volumeGroup);

    // Technical Settings Group
    QGroupBox *techGroup = new QGroupBox(tr("Technical Settings"), this);
    QFormLayout *techLayout = new QFormLayout(techGroup);

    m_bufferSizeCombo = new QComboBox(this);
    m_bufferSizeCombo->addItem(tr("64 samples (lowest latency)"), 64);
    m_bufferSizeCombo->addItem(tr("128 samples (low latency)"), 128);
    m_bufferSizeCombo->addItem(tr("256 samples (balanced)"), 256);
    m_bufferSizeCombo->addItem(tr("512 samples (default)"), 512);
    m_bufferSizeCombo->addItem(tr("1024 samples (stable)"), 1024);
    m_bufferSizeCombo->addItem(tr("2048 samples (most stable)"), 2048);
    connect(m_bufferSizeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsDialog::onBufferSizeChanged);
    techLayout->addRow(tr("Buffer size:"), m_bufferSizeCombo);

    m_sampleRateCombo = new QComboBox(this);
    m_sampleRateCombo->addItem(tr("44100 Hz (CD quality)"), 44100);
    m_sampleRateCombo->addItem(tr("48000 Hz (default)"), 48000);
    m_sampleRateCombo->addItem(tr("96000 Hz (high quality)"), 96000);
    connect(m_sampleRateCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
        this, &SettingsDialog::onSampleRateChanged);
    techLayout->addRow(tr("Sample rate:"), m_sampleRateCombo);

    m_fadeDurationSpin = new QSpinBox(this);
    m_fadeDurationSpin->setRange(0, 1000);
    m_fadeDurationSpin->setSuffix(" ms");
    m_fadeDurationSpin->setSingleStep(10);
    techLayout->addRow(tr("Fade in/out duration:"), m_fadeDurationSpin);

    mainLayout->addWidget(techGroup);
    mainLayout->addStretch();

    return widget;
}

QWidget* SettingsDialog::createHotkeysTab()
{
    QWidget *widget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    mainLayout->setSpacing(15);

    // Hotkey Modes Group
    QGroupBox *modesGroup = new QGroupBox(tr("Hotkey Modes"), this);
    QVBoxLayout *modesLayout = new QVBoxLayout(modesGroup);

    m_numpadModeCheck = new QCheckBox(tr("Enable Numpad hotkeys (Alt+1..9 for quick access)"), this);
    m_indexHotkeysCheck = new QCheckBox(tr("Enable Index hotkeys (Ctrl+Alt+number for sequential access)"), this);
    m_specialHotkeysCheck = new QCheckBox(tr("Enable Special hotkeys (random playback from category)"), this);
    m_conflictDetectionCheck = new QCheckBox(tr("Detect and warn about conflicting hotkeys"), this);

    modesLayout->addWidget(m_numpadModeCheck);
    modesLayout->addWidget(m_indexHotkeysCheck);
    modesLayout->addWidget(m_specialHotkeysCheck);
    modesLayout->addWidget(m_conflictDetectionCheck);

    mainLayout->addWidget(modesGroup);

    // Assigned Hotkeys Info
    QGroupBox *assignedGroup = new QGroupBox(tr("Currently Assigned Hotkeys"), this);
    QVBoxLayout *assignedLayout = new QVBoxLayout(assignedGroup);

    m_hotkeyListLabel = new QLabel(tr("Hotkeys are assigned by right-clicking on sounds in the main window."), this);
    m_hotkeyListLabel->setWordWrap(true);
    assignedLayout->addWidget(m_hotkeyListLabel);

    mainLayout->addWidget(assignedGroup);
    mainLayout->addStretch();

    return widget;
}

QWidget* SettingsDialog::createDevicesTab()
{
    QWidget *widget = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(widget);
    mainLayout->setSpacing(15);

    // Audio Devices Group
    QGroupBox *devicesGroup = new QGroupBox(tr("Audio Devices"), this);
    QFormLayout *devicesLayout = new QFormLayout(devicesGroup);

    m_inputDeviceCombo = new QComboBox(this);
    m_inputDeviceCombo->addItem(tr("Default"), "");
    devicesLayout->addRow(tr("Input device (microphone):"), m_inputDeviceCombo);

    m_outputDeviceCombo = new QComboBox(this);
    m_outputDeviceCombo->addItem(tr("Default"), "");
    devicesLayout->addRow(tr("Output device (headphones):"), m_outputDeviceCombo);

    m_virtualDeviceCombo = new QComboBox(this);
    m_virtualDeviceCombo->addItem(tr("Default / Auto-detect"), "");
    devicesLayout->addRow(tr("Virtual cable device:"), m_virtualDeviceCombo);

    m_refreshDevicesButton = new QPushButton(tr("Refresh Device List"), this);
    connect(m_refreshDevicesButton, &QPushButton::clicked, this, [this]() {
        // TODO: Enumerate devices via AudioEngine
        AudioEngine* engine = AudioEngine::instance();
        if (engine) {
            engine->refreshDeviceList();
            // Repopulate combos
            m_inputDeviceCombo->clear();
            m_outputDeviceCombo->clear();
            m_virtualDeviceCombo->clear();

            m_inputDeviceCombo->addItem(tr("Default"), "");
            m_outputDeviceCombo->addItem(tr("Default"), "");
            m_virtualDeviceCombo->addItem(tr("Default / Auto-detect"), "");

            const auto& devices = engine->getDeviceList();
            for (const auto& dev : devices.inputDevices) {
                m_inputDeviceCombo->addItem(dev.name, dev.id);
            }
            for (const auto& dev : devices.outputDevices) {
                m_outputDeviceCombo->addItem(dev.name, dev.id);
            }
            for (const auto& dev : devices.virtualDevices) {
                m_virtualDeviceCombo->addItem(dev.name, dev.id);
            }
        }
    });

    QHBoxLayout *refreshLayout = new QHBoxLayout;
    refreshLayout->addStretch();
    refreshLayout->addWidget(m_refreshDevicesButton);
    devicesLayout->addRow(QString(), refreshLayout);

    mainLayout->addWidget(devicesGroup);

    // Fallback Behavior Group
    QGroupBox *fallbackGroup = new QGroupBox(tr("Device Fallback Behavior"), this);
    QVBoxLayout *fallbackLayout = new QVBoxLayout(fallbackGroup);

    QLabel *fallbackHelp = new QLabel(
        tr("What to do when a selected device becomes unavailable:"), this);
    fallbackLayout->addWidget(fallbackHelp);

    m_fallbackModeCombo = new QComboBox(this);
    m_fallbackModeCombo->addItem(tr("Pause playback"), static_cast<int>(DeviceFallbackMode::Pause));
    m_fallbackModeCombo->addItem(tr("Auto-switch to default device"), static_cast<int>(DeviceFallbackMode::AutoSwitch));
    m_fallbackModeCombo->addItem(tr("Ignore and continue"), static_cast<int>(DeviceFallbackMode::Ignore));
    fallbackLayout->addWidget(m_fallbackModeCombo);

    mainLayout->addWidget(fallbackGroup);
    mainLayout->addStretch();

    return widget;
}
