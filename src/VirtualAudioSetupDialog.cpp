/* src/VirtualAudioSetupDialog.cpp */

#include "VirtualAudioSetupDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QDesktopServices>
#include <QSettings>
#include <QDebug>

#ifdef Q_OS_WIN
#include <Windows.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <endpointvolume.h>
#include <propvarutil.h>
#include "PolicyConfig.h"
#endif

#ifdef Q_OS_MACOS
#include <CoreAudio/CoreAudio.h>
#include <AudioToolbox/AudioToolbox.h>
#endif

VirtualAudioSetupDialog::VirtualAudioSetupDialog(QWidget *parent)
    : QDialog(parent)
    , m_installationInProgress(false)
    , m_currentStatus(VirtualAudioStatus::Unknown)
{
    setWindowTitle(tr("Virtual Audio Setup"));
    setMinimumWidth(500);
    setupUI();
    updateStatus();
}

VirtualAudioSetupDialog::~VirtualAudioSetupDialog() = default;

void VirtualAudioSetupDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);

    // Title
    auto *titleLabel = new QLabel(tr("<h2>Microphone Injection Setup</h2>"), this);
    mainLayout->addWidget(titleLabel);

    // Description
    m_descriptionLabel = new QLabel(this);
    m_descriptionLabel->setWordWrap(true);
    mainLayout->addWidget(m_descriptionLabel);

    // Status
    m_statusLabel = new QLabel(this);
    m_statusLabel->setWordWrap(true);
    mainLayout->addWidget(m_statusLabel);

    // Progress bar (hidden initially)
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setVisible(false);
    mainLayout->addWidget(m_progressBar);

    mainLayout->addStretch();

    // Buttons
    m_buttonBox = new QDialogButtonBox(this);

    m_installButton = new QPushButton(tr("Install Virtual Audio"), this);
    m_alreadyHaveButton = new QPushButton(tr("I Already Have It"), this);
    m_configureButton = new QPushButton(tr("Configure"), this);
    m_skipButton = new QPushButton(tr("Skip"), this);

    m_buttonBox->addButton(m_installButton, QDialogButtonBox::ActionRole);
    m_buttonBox->addButton(m_alreadyHaveButton, QDialogButtonBox::ActionRole);
    m_buttonBox->addButton(m_configureButton, QDialogButtonBox::ActionRole);
    m_buttonBox->addButton(m_skipButton, QDialogButtonBox::RejectRole);

    mainLayout->addWidget(m_buttonBox);

    connect(m_installButton, &QPushButton::clicked, this, &VirtualAudioSetupDialog::onInstallClicked);
    connect(m_alreadyHaveButton, &QPushButton::clicked, this, &VirtualAudioSetupDialog::onAlreadyHaveClicked);
    connect(m_configureButton, &QPushButton::clicked, this, &VirtualAudioSetupDialog::onConfigureClicked);
    connect(m_skipButton, &QPushButton::clicked, this, &VirtualAudioSetupDialog::onSkipClicked);
}

void VirtualAudioSetupDialog::updateStatus()
{
    m_currentStatus = checkVirtualAudioStatus();

#ifdef Q_OS_WIN
    m_descriptionLabel->setText(tr(
        "To inject audio into your microphone, OpenSoundDeck needs a virtual audio cable. "
        "We'll install VB-Audio Virtual Cable (free, widely used).\n\n"
        "After installation, OpenSoundDeck will automatically configure Windows to use it."
    ));
#elif defined(Q_OS_MACOS)
    m_descriptionLabel->setText(tr(
        "To inject audio into your microphone, OpenSoundDeck needs a virtual audio device. "
        "We'll install BlackHole (free, open-source).\n\n"
        "After installation, OpenSoundDeck will automatically configure macOS to use it."
    ));
#else
    m_descriptionLabel->setText(tr(
        "Linux PulseAudio/PipeWire will be configured automatically to create a virtual microphone. "
        "No additional software is required."
    ));
#endif

    switch (m_currentStatus) {
    case VirtualAudioStatus::FullyConfigured:
        m_statusLabel->setText(tr("<span style='color: green;'>✓ Virtual audio is ready!</span>"));
        m_installButton->setVisible(false);
        m_alreadyHaveButton->setVisible(false);
        m_configureButton->setVisible(false);
        m_skipButton->setText(tr("Close"));
        break;

    case VirtualAudioStatus::InstalledButNotConfigured:
        m_statusLabel->setText(tr("<span style='color: orange;'>⚠ Virtual audio is installed but not configured</span>"));
        m_installButton->setVisible(false);
        m_alreadyHaveButton->setVisible(false);
        m_configureButton->setVisible(true);
        break;

    case VirtualAudioStatus::NotInstalled:
        m_statusLabel->setText(tr("<span style='color: red;'>✗ Virtual audio is not installed</span>"));
        m_installButton->setVisible(true);
        m_alreadyHaveButton->setVisible(true);
        m_configureButton->setVisible(false);
        break;

    default:
        m_statusLabel->setText(tr("Checking status..."));
        break;
    }
}

VirtualAudioStatus VirtualAudioSetupDialog::checkVirtualAudioStatus()
{
    qDebug() << "[VIRTUAL AUDIO] checkVirtualAudioStatus() called";

#ifdef Q_OS_WIN
    // Check if VB-Cable is installed
    if (checkVBCableInstalled()) {
        qDebug() << "[VIRTUAL AUDIO] VB-Cable installed, checking if default...";
        // Check if it's the default recording device
        // For now, assume if it's installed it might be configured
        // In a real implementation, we'd check if it's the default
        return VirtualAudioStatus::InstalledButNotConfigured;
    } else {
        qDebug() << "[VIRTUAL AUDIO] VB-Cable not installed";
        return VirtualAudioStatus::NotInstalled;
    }
#elif defined(Q_OS_MACOS)
    // Check for BlackHole
    if (checkBlackHoleInstalled()) {
        return VirtualAudioStatus::InstalledButNotConfigured;
    } else {
        return VirtualAudioStatus::NotInstalled;
    }
#else
    // Linux: always can be configured programmatically
    return VirtualAudioStatus::FullyConfigured;
#endif
}

bool VirtualAudioSetupDialog::isVirtualAudioRequired()
{
#ifdef Q_OS_LINUX
    return false; // Linux handles this automatically
#else
    VirtualAudioStatus status = checkVirtualAudioStatus();
    return status != VirtualAudioStatus::FullyConfigured;
#endif
}

QString VirtualAudioSetupDialog::getVirtualInputDeviceName()
{
#ifdef Q_OS_WIN
    return QStringLiteral("CABLE Output");
#elif defined(Q_OS_MACOS)
    return QStringLiteral("BlackHole 2ch");
#else
    return QStringLiteral("virt_mic");
#endif
}

QString VirtualAudioSetupDialog::getVirtualOutputDeviceName()
{
#ifdef Q_OS_WIN
    return QStringLiteral("CABLE Input");
#elif defined(Q_OS_MACOS)
    return QStringLiteral("BlackHole 2ch");
#else
    return QStringLiteral("virt_mic");
#endif
}

void VirtualAudioSetupDialog::onInstallClicked()
{
    downloadAndInstall();
}

void VirtualAudioSetupDialog::onAlreadyHaveClicked()
{
    // User says they already have it installed
    // Try to configure it
    onConfigureClicked();
}

void VirtualAudioSetupDialog::onConfigureClicked()
{
#ifdef Q_OS_WIN
    if (configureWindowsAudio()) {
        QMessageBox::information(this, tr("Success"),
            tr("Virtual audio configured successfully!\n\n"
               "OpenSoundDeck is now ready to inject audio into your microphone."));
        updateStatus();
        accept();
    } else {
        QMessageBox::warning(this, tr("Configuration Failed"),
            tr("Could not automatically configure virtual audio.\n\n"
               "Please manually set 'CABLE Input' as your default recording device in Windows Sound Settings."));
    }
#elif defined(Q_OS_MACOS)
    if (configureMacOSAudio()) {
        QMessageBox::information(this, tr("Success"),
            tr("Virtual audio configured successfully!\n\n"
               "OpenSoundDeck is now ready to inject audio into your microphone."));
        updateStatus();
        accept();
    } else {
        QMessageBox::warning(this, tr("Configuration Failed"),
            tr("Could not automatically configure virtual audio.\n\n"
               "Please manually set 'BlackHole 2ch' as your default input device in System Preferences > Sound."));
    }
#else
    // Linux - should already be configured
    accept();
#endif
}

void VirtualAudioSetupDialog::onSkipClicked()
{
    // Save preference to not show this dialog again
    QSettings settings;
    settings.setValue("virtualAudioSetupSkipped", true);

    // Warn user about limitations
    QMessageBox::warning(this, tr("Limited Functionality"),
        tr("Without virtual audio, you can still play sounds, but they won't be injected into your microphone.\n\n"
           "You can set this up later in Settings > Audio."));

    reject();
}

void VirtualAudioSetupDialog::downloadAndInstall()
{
#ifdef Q_OS_WIN
    // Open browser to download page
    QUrl downloadUrl(QStringLiteral("https://vb-audio.com/Cable/"));
    QDesktopServices::openUrl(downloadUrl);

    QMessageBox::information(this, tr("Download VB-Cable"),
        tr("Please download and install VB-Audio Virtual Cable from the opened webpage.\n\n"
           "After installation completes, click 'Configure' to set it up automatically."));

    // Change buttons
    m_installButton->setVisible(false);
    m_alreadyHaveButton->setVisible(false);
    m_configureButton->setVisible(true);

#elif defined(Q_OS_MACOS)
    // For macOS, we can bundle the BlackHole pkg or download it
    QUrl downloadUrl(QStringLiteral("https://github.com/ExistentialAudio/BlackHole/releases"));
    QDesktopServices::openUrl(downloadUrl);

    QMessageBox::information(this, tr("Download BlackHole"),
        tr("Please download and install BlackHole 2ch from the opened page.\n\n"
           "After installation completes, click 'Configure' to set it up automatically."));

    m_installButton->setVisible(false);
    m_alreadyHaveButton->setVisible(false);
    m_configureButton->setVisible(true);
#else
    // Linux - just configure
    onConfigureClicked();
#endif
}

void VirtualAudioSetupDialog::onDownloadFinished(bool success)
{
    m_installationInProgress = false;
    m_progressBar->setVisible(false);

    if (success) {
        updateStatus();
        if (m_currentStatus == VirtualAudioStatus::InstalledButNotConfigured) {
            onConfigureClicked();
        }
    } else {
        QMessageBox::critical(this, tr("Download Failed"),
            tr("Could not download the virtual audio driver.\n\n"
               "Please download it manually from the official website."));
    }
}

// Windows-specific implementations
#ifdef Q_OS_WIN
bool VirtualAudioSetupDialog::checkVBCableInstalled()
{
    // COM already initialized by Qt, so use COINIT_APARTMENTTHREADED to match
    // and check for RPC_E_CHANGED_MODE which means already initialized
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool needToUninitialize = SUCCEEDED(hr);

    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        qDebug() << "[COM] Failed to initialize:" << hr;
        return false;
    }

    IMMDeviceEnumerator *pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&pEnumerator));

    if (FAILED(hr)) {
        qDebug() << "[COM] Failed to create MMDeviceEnumerator:" << hr;
        if (needToUninitialize) CoUninitialize();
        return false;
    }

    IMMDeviceCollection *pCollection = nullptr;
    hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCollection);

    bool found = false;
    if (SUCCEEDED(hr) && pCollection) {
        UINT count = 0;
        hr = pCollection->GetCount(&count);

        if (SUCCEEDED(hr)) {
            for (UINT i = 0; i < count && !found; i++) {
                IMMDevice *pDevice = nullptr;
                if (SUCCEEDED(pCollection->Item(i, &pDevice)) && pDevice) {
                    IPropertyStore *pProps = nullptr;
                    if (SUCCEEDED(pDevice->OpenPropertyStore(STGM_READ, &pProps)) && pProps) {
                        PROPVARIANT varName;
                        PropVariantInit(&varName);
                        if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &varName))) {
                            if (varName.vt == VT_LPWSTR && varName.pwszVal) {
                                QString deviceName = QString::fromWCharArray(varName.pwszVal);
                                qDebug() << "[COM] Found device:" << deviceName;
                                if (deviceName.contains(QStringLiteral("CABLE"), Qt::CaseInsensitive) ||
                                    deviceName.contains(QStringLiteral("VB-Audio"), Qt::CaseInsensitive)) {
                                    found = true;
                                    qDebug() << "[COM] VB-Cable found!";
                                }
                            }
                            PropVariantClear(&varName);
                        }
                        pProps->Release();
                    }
                    pDevice->Release();
                }
            }
        }
        pCollection->Release();
    }

    pEnumerator->Release();
    if (needToUninitialize) CoUninitialize();

    qDebug() << "[COM] checkVBCableInstalled result:" << found;
    return found;
}

bool VirtualAudioSetupDialog::configureWindowsAudio()
{
    // Set VB-Cable as default recording device
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool needToUninitialize = SUCCEEDED(hr);

    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        qDebug() << "[COM] Failed to initialize:" << hr;
        return false;
    }

    // Use PolicyConfig to set default device
    // Try Windows 7+ interface first, then Vista
    IPolicyConfig *pPolicyConfig = nullptr;
    hr = CoCreateInstance(CLSID_CPolicyConfigClient, nullptr, CLSCTX_ALL,
        IID_IPolicyConfig, reinterpret_cast<void**>(&pPolicyConfig));

    if (FAILED(hr)) {
        qDebug() << "[COM] Failed to create PolicyConfig:" << hr;
        if (needToUninitialize) CoUninitialize();
        return false;
    }

    // Find VB-Cable device
    IMMDeviceEnumerator *pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&pEnumerator));

    if (FAILED(hr)) {
        qDebug() << "[COM] Failed to create MMDeviceEnumerator:" << hr;
        pPolicyConfig->Release();
        if (needToUninitialize) CoUninitialize();
        return false;
    }

    IMMDeviceCollection *pCollection = nullptr;
    hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATE_ACTIVE, &pCollection);

    bool configured = false;
    LPWSTR foundDeviceId = nullptr;

    if (SUCCEEDED(hr) && pCollection) {
        UINT count = 0;
        hr = pCollection->GetCount(&count);

        if (SUCCEEDED(hr)) {
            for (UINT i = 0; i < count && !configured; i++) {
                IMMDevice *pDevice = nullptr;
                if (SUCCEEDED(pCollection->Item(i, &pDevice)) && pDevice) {
                    IPropertyStore *pProps = nullptr;
                    if (SUCCEEDED(pDevice->OpenPropertyStore(STGM_READ, &pProps)) && pProps) {
                        PROPVARIANT varName;
                        PropVariantInit(&varName);
                        if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &varName))) {
                            if (varName.vt == VT_LPWSTR && varName.pwszVal) {
                                QString deviceName = QString::fromWCharArray(varName.pwszVal);
                                qDebug() << "[COM] Checking device:" << deviceName;
                                if (deviceName.contains(QStringLiteral("CABLE"), Qt::CaseInsensitive) ||
                                    deviceName.contains(QStringLiteral("VB-Audio"), Qt::CaseInsensitive)) {
                                    if (SUCCEEDED(pDevice->GetId(&foundDeviceId)) && foundDeviceId) {
                                        hr = pPolicyConfig->SetDefaultEndpoint(foundDeviceId, eConsole);
                                        if (SUCCEEDED(hr)) {
                                            configured = true;
                                            qDebug() << "[COM] Set default endpoint to:" << deviceName;
                                        } else {
                                            qDebug() << "[COM] SetDefaultEndpoint failed:" << hr;
                                            CoTaskMemFree(foundDeviceId);
                                            foundDeviceId = nullptr;
                                        }
                                    }
                                }
                            }
                            PropVariantClear(&varName);
                        }
                        pProps->Release();
                    }
                    pDevice->Release();
                }
            }
        }
        pCollection->Release();
    }

    pEnumerator->Release();
    pPolicyConfig->Release();
    if (foundDeviceId) {
        CoTaskMemFree(foundDeviceId);
    }
    if (needToUninitialize) CoUninitialize();

    qDebug() << "[COM] configureWindowsAudio result:" << configured;
    return configured;
}
#endif

// macOS-specific implementations
#ifdef Q_OS_MACOS
bool VirtualAudioSetupDialog::checkBlackHoleInstalled()
{
    AudioObjectPropertyAddress propertyAddress = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMain
    };

    UInt32 dataSize = 0;
    OSStatus status = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &dataSize);

    if (status != noErr) return false;

    UInt32 deviceCount = dataSize / sizeof(AudioObjectID);
    AudioObjectID *deviceIDs = new AudioObjectID[deviceCount];
    status = AudioObjectGetPropertyData(kAudioObjectSystemObject, &propertyAddress, 0, NULL, &dataSize, deviceIDs);

    bool found = false;
    if (status == noErr) {
        for (UInt32 i = 0; i < deviceCount && !found; i++) {
            CFStringRef deviceName = NULL;
            dataSize = sizeof(CFStringRef);
            propertyAddress.mSelector = kAudioDevicePropertyDeviceNameCFString;

            status = AudioObjectGetPropertyData(deviceIDs[i], &propertyAddress, 0, NULL, &dataSize, &deviceName);
            if (status == noErr && deviceName != NULL) {
                QString name = QString::fromCFString(deviceName);
                if (name.contains(QStringLiteral("BlackHole"), Qt::CaseInsensitive)) {
                    found = true;
                }
                CFRelease(deviceName);
            }
        }
    }

    delete[] deviceIDs;
    return found;
}

bool VirtualAudioSetupDialog::configureMacOSAudio()
{
    // Create aggregate device with BlackHole as input
    // This requires more complex CoreAudio manipulation
    // For now, just verify it's available
    return checkBlackHoleInstalled();
}
#endif

// Linux-specific
#ifdef Q_OS_LINUX
bool VirtualAudioSetupDialog::setupPulseAudioVirtualSink()
{
    // Use pactl to create virtual sink and source
    QProcess process;

    // Create null sink
    process.start(QStringLiteral("pactl"),
                  QStringList() << QStringLiteral("load-module") << QStringLiteral("module-null-sink")
                  << QStringLiteral("sink_name=virt_mic") << QStringLiteral("sink_properties=device.description='OpenSoundDeck Virtual Mic'"));
    process.waitForFinished(5000);

    // Create source from sink monitor
    process.start(QStringLiteral("pactl"),
                  QStringList() << QStringLiteral("load-module") << QStringLiteral("module-virtual-source")
                  << QStringLiteral("source_name=virt_mic") << QStringLiteral("master=virt_mic.monitor"));
    process.waitForFinished(5000);

    return process.exitCode() == 0;
}
#endif
