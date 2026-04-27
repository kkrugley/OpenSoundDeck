/* src/VirtualAudioSetupDialog.h */

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
 */

#pragma once

#include <QDialog>

class QLabel;
class QPushButton;
class QProgressBar;
class QDialogButtonBox;

enum class VirtualAudioStatus {
    NotInstalled,
    InstalledButNotConfigured,
    FullyConfigured,
    Unknown
};

class VirtualAudioSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit VirtualAudioSetupDialog(QWidget *parent = nullptr);
    ~VirtualAudioSetupDialog();

    // Check if virtual audio is properly set up
    static VirtualAudioStatus checkVirtualAudioStatus();
    static bool isVirtualAudioRequired();

    // Platform-specific device names
    static QString getVirtualInputDeviceName();
    static QString getVirtualOutputDeviceName();

private slots:
    void onInstallClicked();
    void onAlreadyHaveClicked();
    void onConfigureClicked();
    void onSkipClicked();
    void onDownloadFinished(bool success);

private:
    void setupUI();
    void updateStatus();
    void downloadAndInstall();
    void showManualConfigurationGuide();

#ifdef Q_OS_WIN
    static bool checkVBCableInstalled();
    bool configureWindowsAudio();
    void launchVBCableInstaller(const QString &installerPath);
#endif

#ifdef Q_OS_MACOS
    static bool checkBlackHoleInstalled();
    bool configureMacOSAudio();
    void launchBlackHoleInstaller(const QString &installerPath);
#endif

#ifdef Q_OS_LINUX
    bool setupPulseAudioVirtualSink();
#endif

    QLabel *m_statusLabel;
    QLabel *m_descriptionLabel;
    QProgressBar *m_progressBar;
    QPushButton *m_installButton;
    QPushButton *m_alreadyHaveButton;
    QPushButton *m_configureButton;
    QPushButton *m_skipButton;
    QDialogButtonBox *m_buttonBox;

    bool m_installationInProgress;
    VirtualAudioStatus m_currentStatus;
};
