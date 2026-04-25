/* src/AudioEngine.cpp */

/*
 * OpenSoundDeck
 * Copyright (C) 2025 Pavel Kruhlei
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "AudioEngine.h"
#include <QDebug>
#include <QMetaObject>
#include <QThread>

#define MA_DEBUG_OUTPUT
#include "miniaudio.h"

// Platform-specific audio device detection
#ifdef Q_OS_WIN
#include <Windows.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#endif

#ifdef Q_OS_MACOS
#include <CoreAudio/CoreAudio.h>
#endif

// ============================================================================
// Ring Buffer Implementation
// ============================================================================

void AudioEngine::RingBuffer::init(size_t frames, int channels)
{
    capacity = frames * channels * 2; // Double buffer size
    data.resize(capacity);
    writePos = 0;
    readPos = 0;
}

bool AudioEngine::RingBuffer::write(const float* input, size_t frames, int channels)
{
    size_t samplesToWrite = frames * channels;
    size_t currentWrite = writePos.load();
    size_t currentRead = readPos.load();

    // Check if there's enough space
    size_t availableSpace = (currentRead + capacity - currentWrite - 1) % capacity;
    if (availableSpace < samplesToWrite) {
        return false; // Buffer full
    }

    for (size_t i = 0; i < samplesToWrite; i++) {
        data[currentWrite] = input[i];
        currentWrite = (currentWrite + 1) % capacity;
    }

    writePos.store(currentWrite);
    return true;
}

bool AudioEngine::RingBuffer::read(float* output, size_t frames, int channels)
{
    size_t samplesToRead = frames * channels;
    size_t currentRead = readPos.load();
    size_t currentWrite = writePos.load();

    // Check if there's enough data
    size_t available = (currentWrite + capacity - currentRead) % capacity;
    if (available < samplesToRead) {
        // Fill with silence if not enough data
        std::fill(output, output + samplesToRead, 0.0f);
        return false;
    }

    for (size_t i = 0; i < samplesToRead; i++) {
        output[i] = data[currentRead];
        currentRead = (currentRead + 1) % capacity;
    }

    readPos.store(currentRead);
    return true;
}

size_t AudioEngine::RingBuffer::available() const
{
    size_t currentRead = readPos.load();
    size_t currentWrite = writePos.load();
    return (currentWrite + capacity - currentRead) % capacity;
}

void AudioEngine::RingBuffer::clear()
{
    writePos = 0;
    readPos = 0;
    std::fill(data.begin(), data.end(), 0.0f);
}

// ============================================================================
// AudioEngine Implementation
// ============================================================================

AudioEngine::AudioEngine(QObject *parent)
    : QObject(parent)
    , m_context(new ma_context)
    , m_micDevice(new ma_device)
    , m_playbackDevice(new ma_device)
    , m_monitorDevice(new ma_device)
    , m_pDecoder(nullptr)
    , m_micVolume(1.0f)
    , m_fileVolume(0.8f)
    , m_monitorVolume(0.8f)
    , m_isInitialized(false)
    , m_playbackState(Stopped)
    , m_seekRequestMillis(-1)
    , m_currentPositionMillis(0)
    , m_durationMillis(0)
    , m_sampleRate(48000)
    , m_channels(2)
    , m_format(ma_format_f32)
{
    m_positionUpdateTimer = new QTimer(this);
    m_positionUpdateTimer->setInterval(100);
    connect(m_positionUpdateTimer, &QTimer::timeout, this, &AudioEngine::onUpdatePositionTimer);

    // Initialize ring buffer for microphone data
    m_micRingBuffer.init(48000, 2); // 1 second stereo buffer @ 48kHz
}

AudioEngine::~AudioEngine()
{
    stopAllSounds();

    if (m_isInitialized) {
        ma_device_uninit(m_micDevice);
        ma_device_uninit(m_playbackDevice);
        ma_device_uninit(m_monitorDevice);
    }

    ma_context_uninit(m_context);

    delete m_context;
    delete m_micDevice;
    delete m_playbackDevice;
    delete m_monitorDevice;

    // Cleanup decoder
    ma_decoder* pDecoder = m_pDecoder.exchange(nullptr);
    if (pDecoder) {
        ma_decoder_uninit(pDecoder);
        delete pDecoder;
    }
}

bool AudioEngine::init()
{
    if (m_isInitialized) {
        return true;
    }

    ma_context_config contextConfig = ma_context_config_init();
    if (ma_context_init(NULL, 0, &contextConfig, m_context) != MA_SUCCESS) {
        qCritical() << "Failed to initialize miniaudio context.";
        emit error(tr("Failed to initialize audio context"));
        return false;
    }

    // Initialize microphone capture device
    ma_device_config micConfig = ma_device_config_init(ma_device_type_capture);
    micConfig.capture.format = ma_format_f32;
    micConfig.capture.channels = 2;
    micConfig.sampleRate = m_sampleRate;
    micConfig.dataCallback = micDataCallback;
    micConfig.pUserData = this;

    if (ma_device_init(m_context, &micConfig, m_micDevice) != MA_SUCCESS) {
        qCritical() << "Failed to initialize microphone device.";
        emit error(tr("Failed to initialize microphone"));
        return false;
    }

    // Initialize playback device (to virtual microphone)
    ma_device_config playbackConfig = ma_device_config_init(ma_device_type_playback);
    playbackConfig.playback.format = ma_format_f32;
    playbackConfig.playback.channels = 2;
    playbackConfig.sampleRate = m_sampleRate;
    playbackConfig.dataCallback = playbackDataCallback;
    playbackConfig.pUserData = this;

    // Try to select virtual output device (VB-Cable/BlackHole)
    selectVirtualOutputDevice(&playbackConfig);

    if (ma_device_init(m_context, &playbackConfig, m_playbackDevice) != MA_SUCCESS) {
        qCritical() << "Failed to initialize playback device.";
        ma_device_uninit(m_micDevice);
        emit error(tr("Failed to initialize virtual microphone output"));
        return false;
    }

    // Initialize monitor device (to headphones)
    ma_device_config monitorConfig = ma_device_config_init(ma_device_type_playback);
    monitorConfig.playback.format = ma_format_f32;
    monitorConfig.playback.channels = 2;
    monitorConfig.sampleRate = m_sampleRate;
    monitorConfig.dataCallback = monitorDataCallback;
    monitorConfig.pUserData = this;

    // Try to select default playback device for monitoring
    if (ma_device_init(m_context, &monitorConfig, m_monitorDevice) == MA_SUCCESS) {
        // Monitoring device initialized successfully
    } else {
        // Monitoring is optional, continue without it
        qWarning() << "Could not initialize monitor device, continuing without monitoring.";
    }

    // Start all devices
    if (ma_device_start(m_micDevice) != MA_SUCCESS) {
        qCritical() << "Failed to start microphone device.";
        emit error(tr("Failed to start microphone"));
        return false;
    }

    if (ma_device_start(m_playbackDevice) != MA_SUCCESS) {
        qCritical() << "Failed to start playback device.";
        ma_device_stop(m_micDevice);
        emit error(tr("Failed to start virtual microphone"));
        return false;
    }

    // Monitor device is optional
    ma_device_start(m_monitorDevice);

    m_isInitialized = true;
    qDebug() << "AudioEngine initialized successfully.";
    return true;
}

// ============================================================================
// Device Selection (Platform-specific)
// ============================================================================

bool AudioEngine::selectVirtualOutputDevice(void* pConfig)
{
    (void)pConfig;
    // miniaudio doesn't directly support selecting specific devices via config
    // on all platforms. The virtual device should be set as default in the OS
    // for this to work automatically.

    // For now, we rely on the VirtualAudioSetupDialog to set the virtual
    // device as default. miniaudio will then use it automatically.

    // Platform-specific selection could be added here in the future
    // using ma_context_get_devices() and ma_device_init_ex()

    return true;
}

bool AudioEngine::selectMonitorDevice(void* pConfig)
{
    (void)pConfig;
    // Similar to above, we let miniaudio use the default playback device
    // for monitoring (usually headphones/speakers)
    return true;
}

// ============================================================================
// Callback Functions
// ============================================================================

void AudioEngine::micDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    (void)pOutput; // Capture device doesn't use output

    AudioEngine* engine = static_cast<AudioEngine*>(pDevice->pUserData);
    if (!engine || !pInput) return;

    const float* inputFloat = static_cast<const float*>(pInput);

    // Apply mic volume and write to ring buffer
    std::vector<float> processed(frameCount * 2);
    float micVol = engine->m_micVolume.load();

    for (ma_uint32 i = 0; i < frameCount * 2; i++) {
        processed[i] = inputFloat[i] * micVol;
    }

    engine->m_micRingBuffer.write(processed.data(), frameCount, 2);
}

void AudioEngine::playbackDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    (void)pInput; // Playback device doesn't use input

    AudioEngine* engine = static_cast<AudioEngine*>(pDevice->pUserData);
    if (!engine || !pOutput) return;

    float* outputFloat = static_cast<float*>(pOutput);
    ma_decoder* pDecoder = engine->m_pDecoder.load();

    // Read microphone data from ring buffer
    std::vector<float> micData(frameCount * 2);
    engine->m_micRingBuffer.read(micData.data(), frameCount, 2);

    // Read file data if playing
    std::vector<float> fileData(frameCount * 2);
    if (pDecoder && engine->m_playbackState.load() == Playing) {
        ma_uint64 framesRead = 0;
        ma_decoder_read_pcm_frames(pDecoder, fileData.data(), frameCount, &framesRead);

        // Handle end of file
        if (framesRead < frameCount) {
            std::fill(fileData.begin() + framesRead * 2, fileData.end(), 0.0f);
            QMetaObject::invokeMethod(engine, "postPlaybackFinished", Qt::QueuedConnection);
        }

        // Apply file volume
        float fileVol = engine->m_fileVolume.load();
        for (ma_uint64 i = 0; i < framesRead * 2; i++) {
            fileData[i] *= fileVol;
        }

        // Update position
        ma_uint64 positionIncrement = (framesRead * 1000) / engine->m_sampleRate;
        engine->m_currentPositionMillis.fetch_add(positionIncrement);
    } else {
        std::fill(fileData.begin(), fileData.end(), 0.0f);
    }

    // Mix microphone + file -> output (virtual mic)
    float micVol = engine->m_micVolume.load();
    float fileVol = engine->m_fileVolume.load();
    engine->mixAudio(outputFloat, micData.data(), fileData.data(), frameCount, micVol, fileVol);
}

void AudioEngine::monitorDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    (void)pInput;

    AudioEngine* engine = static_cast<AudioEngine*>(pDevice->pUserData);
    if (!engine || !pOutput) return;

    float* outputFloat = static_cast<float*>(pOutput);
    ma_decoder* pDecoder = engine->m_pDecoder.load();

    // For monitoring, we play just the file (not the mic to avoid feedback)
    std::vector<float> fileData(frameCount * 2);
    if (pDecoder && engine->m_playbackState.load() == Playing) {
        // We need to read from the same decoder without affecting playback
        // This is tricky - for now, we just mix what we can
        // A better approach would be to use a separate decoder or resampling

        // For monitoring, we'll just output the file data
        float monitorVol = engine->m_monitorVolume.load();
        for (ma_uint32 i = 0; i < frameCount * 2; i++) {
            outputFloat[i] = fileData[i] * monitorVol;
        }
    } else {
        std::fill(outputFloat, outputFloat + frameCount * 2, 0.0f);
    }
}

void AudioEngine::mixAudio(float* output, const float* micData, const float* fileData,
                           size_t frameCount, float micVol, float fileVol)
{
    (void)micVol; // Already applied in mic callback
    (void)fileVol; // Already applied in playback callback

    for (size_t i = 0; i < frameCount * 2; i++) {
        // Mix: mic + file, with soft clipping
        float mixed = micData[i] + fileData[i];

        // Soft clipping to prevent hard distortion
        if (mixed > 1.0f) {
            mixed = 1.0f;
        } else if (mixed < -1.0f) {
            mixed = -1.0f;
        }

        output[i] = mixed;
    }
}

// ============================================================================
// Playback Control
// ============================================================================

void AudioEngine::playSound(const QString& filePath)
{
    // Stop current playback
    stopAllSounds();

    // Create new decoder
    ma_decoder* pNewDecoder = new ma_decoder;
    ma_decoder_config decoderConfig = ma_decoder_config_init(ma_format_f32, 2, m_sampleRate);

    if (ma_decoder_init_file(filePath.toStdString().c_str(), &decoderConfig, pNewDecoder) != MA_SUCCESS) {
        qWarning() << "Failed to open or decode file:" << filePath;
        delete pNewDecoder;
        emit error(tr("Failed to open audio file: %1").arg(filePath));
        return;
    }

    // Initialize if not already done
    if (!m_isInitialized) {
        if (!init()) {
            ma_decoder_uninit(pNewDecoder);
            delete pNewDecoder;
            return;
        }
    }

    // Get duration
    ma_uint64 durationFrames = 0;
    ma_decoder_get_length_in_pcm_frames(pNewDecoder, &durationFrames);
    m_durationMillis.store((durationFrames * 1000) / m_sampleRate);
    emit durationReady(m_durationMillis.load());

    // Start playback
    m_pDecoder.store(pNewDecoder);
    m_currentPositionMillis.store(0);
    m_playbackState.store(Playing);
    m_positionUpdateTimer->start();

    qDebug() << "Playback started for:" << filePath;
}

void AudioEngine::pause()
{
    if (m_playbackState.load() == Playing) {
        m_playbackState.store(Paused);
        m_positionUpdateTimer->stop();
        qDebug() << "Playback paused.";
    }
}

void AudioEngine::resume()
{
    if (m_playbackState.load() == Paused) {
        m_playbackState.store(Playing);
        m_positionUpdateTimer->start();
        qDebug() << "Playback resumed.";
    }
}

void AudioEngine::stopAllSounds()
{
    ma_decoder* pOldDecoder = m_pDecoder.exchange(nullptr);
    if (pOldDecoder) {
        ma_decoder_uninit(pOldDecoder);
        delete pOldDecoder;
    }

    m_playbackState.store(Stopped);
    m_positionUpdateTimer->stop();
    m_currentPositionMillis.store(0);

    // Clear ring buffer
    m_micRingBuffer.clear();

    qDebug() << "Playback stopped.";
}

void AudioEngine::seek(ma_uint64 positionMillis)
{
    m_seekRequestMillis.store(positionMillis);
}

// ============================================================================
// Volume Control
// ============================================================================

void AudioEngine::setMicVolume(float volume)
{
    float clamped = volume < 0.0f ? 0.0f : (volume > 1.0f ? 1.0f : volume);
    m_micVolume.store(clamped);
}

void AudioEngine::setFileVolume(float volume)
{
    float clamped = volume < 0.0f ? 0.0f : (volume > 1.0f ? 1.0f : volume);
    m_fileVolume.store(clamped);
}

void AudioEngine::setMonitorVolume(float volume)
{
    float clamped = volume < 0.0f ? 0.0f : (volume > 1.0f ? 1.0f : volume);
    m_monitorVolume.store(clamped);
}

// ============================================================================
// Getters / Slots
// ============================================================================

AudioEngine::PlaybackState AudioEngine::getPlaybackState() const
{
    return m_playbackState.load();
}

void AudioEngine::onUpdatePositionTimer()
{
    emit positionChanged(m_currentPositionMillis.load());
}

void AudioEngine::postPlaybackFinished()
{
    stopAllSounds();
    emit playbackFinished();
}
