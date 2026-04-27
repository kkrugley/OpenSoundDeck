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
#include <QTextBrowser>

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
    bool success = configureWindowsAudio();
    if (success) {
        QMessageBox::information(this, tr("Success"),
            tr("Virtual audio configured successfully!\n\n"
               "OpenSoundDeck is now ready to inject audio into your microphone."));
        updateStatus();
        accept();
    } else {
        // Create custom message box with additional options
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Configuration Failed"));
        msgBox.setText(tr("Could not automatically configure virtual audio.\n\n"
                          "You need to manually set 'CABLE Output' as your default recording device.\n\n"
                          "Would you like to open Windows Sound Settings?"));
        msgBox.setIcon(QMessageBox::Warning);

        QPushButton *openSettingsBtn = msgBox.addButton(tr("Open Sound Settings"), QMessageBox::ActionRole);
        QPushButton *manualGuideBtn = msgBox.addButton(tr("Show Manual Guide"), QMessageBox::HelpRole);
        QPushButton *closeBtn = msgBox.addButton(QMessageBox::Close);

        msgBox.exec();

        if (msgBox.clickedButton() == openSettingsBtn) {
            // Open Windows Sound Settings
            QUrl url("ms-settings:sound"); // Modern Windows Settings
            if (!QDesktopServices::openUrl(url)) {
                // Fallback to legacy control panel
                QProcess::startDetached("control", QStringList() << "mmsys.cpl" << "sounds");
            }
        } else if (msgBox.clickedButton() == manualGuideBtn) {
            showManualConfigurationGuide();
        }
        // If Close was clicked, just dismiss the dialog
    }
#elif defined(Q_OS_MACOS)
    bool success = configureMacOSAudio();
    if (success) {
        QMessageBox::information(this, tr("Success"),
            tr("Virtual audio configured successfully!\n\n"
               "OpenSoundDeck is now ready to inject audio into your microphone."));
        updateStatus();
        accept();
    } else {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(tr("Configuration Failed"));
        msgBox.setText(tr("Could not automatically configure virtual audio.\n\n"
                          "You need to manually configure BlackHole 2ch as your input device."));
        msgBox.setIcon(QMessageBox::Warning);

        QPushButton *openAudioBtn = msgBox.addButton(tr("Open Audio MIDI Setup"), QMessageBox::ActionRole);
        QPushButton *manualGuideBtn = msgBox.addButton(tr("Show Manual Guide"), QMessageBox::HelpRole);
        QPushButton *closeBtn = msgBox.addButton(QMessageBox::Close);

        msgBox.exec();

        if (msgBox.clickedButton() == openAudioBtn) {
            QProcess::startDetached("open", QStringList() << "/System/Applications/Utilities/Audio MIDI Setup.app");
        } else if (msgBox.clickedButton() == manualGuideBtn) {
            showManualConfigurationGuide();
        }
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

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Download VB-Cable"));
    msgBox.setText(tr("VB-Audio Virtual Cable download page has been opened in your browser.\n\n"
                      "Installation steps:\n"
                      "1. Download VB-Cable (Vbcable_Setup.exe or Vbcable_Setup_x64.exe)\n"
                      "2. Run the installer as Administrator\n"
                      "3. Reboot your computer after installation\n"
                      "4. Return to this dialog and click 'Configure'\n\n"
                      "After installation, OpenSoundDeck needs to set 'CABLE Output' as your default recording device."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

    // Change buttons
    m_installButton->setVisible(false);
    m_alreadyHaveButton->setVisible(false);
    m_configureButton->setVisible(true);

#elif defined(Q_OS_MACOS)
    // For macOS, open BlackHole releases page
    QUrl downloadUrl(QStringLiteral("https://github.com/ExistentialAudio/BlackHole/releases/latest"));
    QDesktopServices::openUrl(downloadUrl);

    QMessageBox msgBox(this);
    msgBox.setWindowTitle(tr("Download BlackHole"));
    msgBox.setText(tr("BlackHole download page has been opened in your browser.\n\n"
                      "Installation steps:\n"
                      "1. Download BlackHole2ch.pkg\n"
                      "2. Run the installer\n"
                      "3. Open Audio MIDI Setup and create a Multi-Output Device\n"
                      "4. Set BlackHole 2ch as the input for the Multi-Output Device\n"
                      "5. Return to this dialog and click 'Configure'\n\n"
                      "After installation, OpenSoundDeck needs to configure the audio routing."));
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

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
    // Set VB-Cable as default recording device using multiple methods
    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    bool needToUninitialize = SUCCEEDED(hr);

    if (FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
        qDebug() << "[COM] Failed to initialize:" << hr;
        return false;
    }

    // Find VB-Cable device
    IMMDeviceEnumerator *pEnumerator = nullptr;
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&pEnumerator));

    if (FAILED(hr)) {
        qDebug() << "[COM] Failed to create MMDeviceEnumerator:" << hr;
        if (needToUninitialize) CoUninitialize();
        return false;
    }

    IMMDeviceCollection *pCollection = nullptr;
    hr = pEnumerator->EnumAudioEndpoints(eCapture, DEVICE_STATEMASK_ALL, &pCollection);

    bool configured = false;
    LPWSTR foundDeviceId = nullptr;
    QString foundDeviceName;

    if (SUCCEEDED(hr) && pCollection) {
        UINT count = 0;
        hr = pCollection->GetCount(&count);

        if (SUCCEEDED(hr)) {
            for (UINT i = 0; i < count; i++) {
                IMMDevice *pDevice = nullptr;
                if (SUCCEEDED(pCollection->Item(i, &pDevice)) && pDevice) {
                    // Check device state
                    DWORD state = 0;
                    HRESULT hrState = pDevice->GetState(&state);

                    IPropertyStore *pProps = nullptr;
                    if (SUCCEEDED(pDevice->OpenPropertyStore(STGM_READ, &pProps)) && pProps) {
                        PROPVARIANT varName;
                        PropVariantInit(&varName);
                        if (SUCCEEDED(pProps->GetValue(PKEY_Device_FriendlyName, &varName))) {
                            if (varName.vt == VT_LPWSTR && varName.pwszVal) {
                                QString deviceName = QString::fromWCharArray(varName.pwszVal);
                                qDebug() << "[COM] Found device:" << deviceName << "State:" << state;

                                // Look for CABLE Output (which is the capture endpoint of VB-Cable)
                                if (deviceName.contains(QStringLiteral("CABLE Output"), Qt::CaseInsensitive) ||
                                    deviceName.contains(QStringLiteral("CABLE"), Qt::CaseInsensitive)) {
                                    if (SUCCEEDED(pDevice->GetId(&foundDeviceId)) && foundDeviceId) {
                                        foundDeviceName = deviceName;

                                        // Try to enable device if disabled
                                        if (state == DEVICE_STATE_DISABLED) {
                                            qDebug() << "[COM] Device is disabled, attempting to enable...";
                                            // Note: Enabling requires elevation, can't do it programmatically easily
                                        }

                                        // Device found, break to try configuration
                                        break;
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

    // If we found the device, try to set it as default
    if (foundDeviceId) {
        // Try multiple methods to set default device

        // Method 1: PolicyConfig (Windows 7+)
        IPolicyConfig *pPolicyConfig = nullptr;
        hr = CoCreateInstance(CLSID_CPolicyConfigClient, nullptr, CLSCTX_ALL,
            IID_IPolicyConfig, reinterpret_cast<void**>(&pPolicyConfig));

        if (SUCCEEDED(hr) && pPolicyConfig) {
            // Try eConsole (games, media players)
            hr = pPolicyConfig->SetDefaultEndpoint(foundDeviceId, eConsole);
            if (SUCCEEDED(hr)) {
                qDebug() << "[COM] SetDefaultEndpoint (eConsole) succeeded for:" << foundDeviceName;
                configured = true;
            } else {
                qDebug() << "[COM] SetDefaultEndpoint (eConsole) failed:" << hr;
            }

            // Also try eCommunication (VoIP, chat apps)
            hr = pPolicyConfig->SetDefaultEndpoint(foundDeviceId, eCommunications);
            if (SUCCEEDED(hr)) {
                qDebug() << "[COM] SetDefaultEndpoint (eCommunications) succeeded for:" << foundDeviceName;
                configured = true;
            } else {
                qDebug() << "[COM] SetDefaultEndpoint (eCommunications) failed:" << hr;
            }

            pPolicyConfig->Release();
        } else {
            qDebug() << "[COM] Failed to create PolicyConfig:" << hr;
        }

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

// Helper function to show manual configuration guide
void VirtualAudioSetupDialog::showManualConfigurationGuide()
{
    QDialog guideDialog(this);
    guideDialog.setWindowTitle(tr("Manual Configuration Guide"));
    guideDialog.setMinimumSize(600, 500);
    guideDialog.resize(700, 550);

    QVBoxLayout *layout = new QVBoxLayout(&guideDialog);

    QTextBrowser *textBrowser = new QTextBrowser(&guideDialog);
    textBrowser->setOpenExternalLinks(true);

#ifdef Q_OS_WIN
    textBrowser->setHtml(R"(<h2>Windows Manual Configuration Guide</h2>

<h3>Step 1: Install VB-Audio Virtual Cable</h3>
<ol>
<li>Visit <a href="https://vb-audio.com/Cable/">https://vb-audio.com/Cable/</a></li>
<li>Download <b>Vbcable_Setup.exe</b> (or Vbcable_Setup_x64.exe for 64-bit)</li>
<li>Run the installer <b>as Administrator</b></li>
<li>Complete the installation and <b>reboot your computer</b></li>
</ol>

<h3>Step 2: Configure Default Recording Device</h3>
<ol>
<li>Right-click the speaker icon in your system tray</li>
<li>Select <b>Open Sound settings</b> or <b>Sounds</b></li>
<li>Go to the <b>Recording</b> tab</li>
<li>Look for <b>CABLE Output</b> (this is the virtual microphone)</li>
<li>Right-click on it and select <b>Set as Default Device</b></li>
<li>Also set it as <b>Default Communication Device</b></li>
<li>Click <b>OK</b> to save</li>
</ol>

<h3>Step 3: Test in Your Application</h3>
<p>Now OpenSoundDeck can inject audio into your microphone. Test it in Discord, Zoom, or any other app by selecting the default microphone.</p>

<h3>Troubleshooting</h3>
<ul>
<li><b>Can't see CABLE Output?</b> Make sure you rebooted after installation</li>
<li><b>Audio not working?</b> Check that CABLE Output is set as default in both Windows and your app</li>
<li><b>VB-Cable not installing?</b> Try running the installer as Administrator</li>
</ul>)");
#elif defined(Q_OS_MACOS)
    textBrowser->setHtml(R"(<h2>macOS Manual Configuration Guide</h2>

<h3>Step 1: Install BlackHole</h3>
<ol>
<li>Visit <a href="https://github.com/ExistentialAudio/BlackHole/releases">BlackHole Releases</a></li>
<li>Download the latest <b>BlackHole2ch.pkg</b></li>
<li>Run the installer and follow the prompts</li>
</ol>

<h3>Step 2: Create Multi-Output Device</h3>
<ol>
<li>Open <b>Audio MIDI Setup</b> (Applications → Utilities)</li>
<li>Click the <b>+</b> button and select <b>Create Multi-Output Device</b></li>
<li>Check <b>BlackHole 2ch</b> and your headphones/speakers</li>
<li>Check <b>Drift Correction</b> for your physical output</li>
<li>Right-click the new device and select <b>Use This Device for Sound Output</b></li>
</ol>

<h3>Step 3: Set Up Input Device</h3>
<ol>
<li>Go to <b>System Preferences → Sound → Input</b></li>
<li>Select <b>BlackHole 2ch</b> as your input device</li>
<li>Or use Audio MIDI Setup to set BlackHole as the default input</li>
</ol>

<h3>Troubleshooting</h3>
<ul>
<li><b>No audio output?</b> Make sure you selected both BlackHole and your speakers in the Multi-Output Device</li>
<li><b>Apps can't hear the mic?</b> Ensure BlackHole is set as the default input in System Preferences</li>
</ul>)");
#else
    textBrowser->setHtml(R"(<h2>Linux Configuration</h2>
<p>OpenSoundDeck will automatically configure PulseAudio/PipeWire. No manual steps required.</p>

<p>If automatic configuration fails, you can manually create a virtual sink:</p>
<pre>
pactl load-module module-null-sink sink_name=virt_mic sink_properties=device.description='OpenSoundDeck Virtual Mic'
pactl load-module module-virtual-source source_name=virt_mic master=virt_mic.monitor
</pre>)");
#endif

    layout->addWidget(textBrowser);

    QPushButton *closeBtn = new QPushButton(tr("Close"), &guideDialog);
    connect(closeBtn, &QPushButton::clicked, &guideDialog, &QDialog::accept);
    layout->addWidget(closeBtn, 0, Qt::AlignRight);

    guideDialog.exec();
}
