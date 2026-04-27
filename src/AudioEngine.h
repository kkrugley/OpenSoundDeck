/* src/AudioEngine.h */

/*
* OpenSoundDeck
* Copyright (C) 2025 Pavel Kruhlei
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/

#pragma once

#include <QObject>
#include <QString>
#include <atomic>
#include <QTimer>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <QQueue>
#include <QByteArray>
#include <QList>

// Miniaudio type definitions (must be before forward declarations)
// Note: These typedefs must match miniaudio.h definitions
using ma_uint64 = unsigned long long;
using ma_uint32 = unsigned int;
using ma_int64 = long long;
// ma_result defined in miniaudio.h as typedef int ma_result;

// Forward declaration for miniaudio
struct ma_context;
struct ma_device;
struct ma_device_config;
struct ma_decoder;

struct AudioFrame {
    float left;
    float right;
};

// Device information structure
struct AudioDeviceInfo {
    QString id;
    QString name;
    bool isDefault;
    bool isVirtual;
};

struct DeviceList {
    QList<AudioDeviceInfo> inputDevices;
    QList<AudioDeviceInfo> outputDevices;
    QList<AudioDeviceInfo> virtualDevices;
};

class AudioEngine : public QObject
{
    Q_OBJECT

public:
    enum PlaybackState {
        Stopped,
        Playing,
        Paused
    };

    explicit AudioEngine(QObject *parent = nullptr);
    ~AudioEngine();

    bool init();
    static AudioEngine* instance();

    // Sound playback
    void playSound(const QString& filePath);
    void pause();
    void resume();
    void stopAllSounds();
    void seek(ma_uint64 positionMillis);

    // Volume controls (0.0 to 1.0)
    void setMicVolume(float volume);
    void setFileVolume(float volume);
    void setMonitorVolume(float volume); // For headphone monitoring

    // Getters
    PlaybackState getPlaybackState() const;
    float getMicVolume() const { return m_micVolume.load(); }
    float getFileVolume() const { return m_fileVolume.load(); }
    float getMonitorVolume() const { return m_monitorVolume.load(); }

    // Device enumeration
    void refreshDeviceList();
    const DeviceList& getDeviceList() const { return m_deviceList; }

    // Get duration of audio file (in milliseconds)
    static qint64 getAudioFileDuration(const QString& filePath);

signals:
    void positionChanged(ma_uint64 positionMillis);
    void durationReady(ma_uint64 durationMillis);
    void playbackFinished();
    void error(const QString& message);
    void devicesRefreshed();

private slots:
    void onUpdatePositionTimer();
    void postPlaybackFinished();

private:
    // Miniaudio context
    ma_context* m_context;

    // Three devices:
    // 1. Microphone input capture
    // 2. File playback output (to virtual mic)
    // 3. Monitor output (to headphones - optional)
    ma_device* m_micDevice;
    ma_device* m_playbackDevice;
    ma_device* m_monitorDevice;

    // Decoder for playing file
    std::atomic<ma_decoder*> m_pDecoder;

    // Ring buffer for microphone data
    // Thread-safe circular buffer for audio frames
    struct RingBuffer {
        std::vector<float> data;
        std::atomic<size_t> writePos{0};
        std::atomic<size_t> readPos{0};
        size_t capacity;
        QMutex mutex;

        void init(size_t frames, int channels);
        bool write(const float* input, size_t frames, int channels);
        bool read(float* output, size_t frames, int channels);
        size_t available() const;
        void clear();
    };
    RingBuffer m_micRingBuffer;

    // Audio format
    int m_sampleRate;
    int m_channels;
    int m_format;

    // Volumes
    std::atomic<float> m_micVolume{1.0f};
    std::atomic<float> m_fileVolume{0.8f};
    std::atomic<float> m_monitorVolume{0.8f};

    // State
    std::atomic<bool> m_isInitialized{false};
    std::atomic<PlaybackState> m_playbackState{Stopped};
    std::atomic<ma_uint64> m_seekRequestMillis{static_cast<ma_uint64>(-1)};
    std::atomic<ma_uint64> m_currentPositionMillis{static_cast<ma_uint64>(0)};
    std::atomic<ma_uint64> m_durationMillis{static_cast<ma_uint64>(0)};

    // Device list
    DeviceList m_deviceList;
    QMutex m_deviceListMutex;

    QTimer* m_positionUpdateTimer;
    static AudioEngine* s_instance;

    // Callbacks
    static void micDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    static void playbackDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    static void monitorDataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);

    // Mixing function
    void mixAudio(float* output, const float* micData, const float* fileData,
        size_t frameCount, float micVol, float fileVol);

    // Device enumeration helpers
    void enumerateDevices();
    bool isVirtualDevice(const QString& name);
};
