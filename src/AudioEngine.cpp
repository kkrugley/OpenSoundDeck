// src/AudioEngine.cpp

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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "AudioEngine.h"
#include <QDebug>
#include <QMetaObject> // Для безопасного вызова методов между потоками

#define MA_DEBUG_OUTPUT
#define MA_IMPLEMENTATION
#include "miniaudio.h"

void AudioEngine::dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount)
{
    AudioEngine* engine = static_cast<AudioEngine*>(pDevice->pUserData);
    if (engine == nullptr) {
        return;
    }

    // Загружаем указатель на декодер атомарно.
    ma_decoder* pDecoder = engine->m_pDecoder.load();

    if (pDecoder == nullptr) {
        // Если декодера нет, просто заполняем буфер тишиной.
        ma_silence_pcm_frames(pOutput, frameCount, pDevice->playback.format, pDevice->playback.channels);
        return;
    }

    // 1. Проверка на запрос перемотки
    ma_int64 seekRequest = engine->m_seekRequestMillis.exchange(-1);
    if (seekRequest != -1) {
        ma_uint64 targetFrame = (seekRequest * pDecoder->outputSampleRate) / 1000;
        ma_decoder_seek_to_pcm_frame(pDecoder, targetFrame);
        engine->m_currentPositionMillis.store(seekRequest);
    }

    // 2. Чтение данных
    ma_uint64 framesRead = 0;
    ma_result result = ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, &framesRead);

    // 3. Применение громкости
    if (result == MA_SUCCESS && framesRead > 0) {
        // Умножаем каждый семпл на значение громкости
        ma_apply_volume_factor_pcm_frames_f32((float*)pOutput, framesRead, pDecoder->outputChannels, engine->m_monitoringVolume.load());
    }

    // 3. Обновление текущей позиции
    ma_uint64 positionIncrement = (framesRead * 1000) / pDecoder->outputSampleRate;
    engine->m_currentPositionMillis.fetch_add(positionIncrement);

    if (framesRead < frameCount) {
        // Файл закончился. Безопасно просим главный поток вызвать postPlaybackFinished()
        QMetaObject::invokeMethod(engine, "postPlaybackFinished", Qt::QueuedConnection);
    }
}

AudioEngine::AudioEngine(QObject *parent)
    : QObject(parent),
      m_context(new ma_context),
      m_playbackDevice(new ma_device),
      m_pDecoder(nullptr), // Инициализируем атомарный указатель как nullptr
      m_monitoringVolume(0.8f), // Начальная громкость 80%
      m_isDeviceInitialized(false),      
      m_playbackState(Stopped),
      m_seekRequestMillis(-1),
      m_currentPositionMillis(0)
{
    m_positionUpdateTimer = new QTimer(this);
    m_positionUpdateTimer->setInterval(100); // Обновлять позицию 10 раз в секунду
    connect(m_positionUpdateTimer, &QTimer::timeout, this, &AudioEngine::onUpdatePositionTimer);
}

AudioEngine::~AudioEngine()
{
    stopAllSounds(); 

    if (m_isDeviceInitialized) {
        ma_device_uninit(m_playbackDevice);
    }

    delete m_context;
    delete m_playbackDevice;
    // Указатель m_pDecoder управляется атомарно и удаляется в stopAllSounds
}

bool AudioEngine::init()
{
    if (ma_context_init(NULL, 0, NULL, m_context) != MA_SUCCESS) {
        qCritical() << "Failed to initialize miniaudio context.";
        return false;
    }
    qDebug() << "Miniaudio context initialized.";
    return true;
}

void AudioEngine::playSound(const QString &filePath)
{
    // Сначала останавливаем любой играющий звук
    stopAllSounds();

    // Создаем новый декодер
    ma_decoder* pNewDecoder = new ma_decoder;
    if (ma_decoder_init_file(filePath.toStdString().c_str(), NULL, pNewDecoder) != MA_SUCCESS) {
        qWarning() << "Failed to open or decode file:" << filePath;
        delete pNewDecoder;
        return;
    }

    // Если устройство не запущено, запускаем его
    if (m_isDeviceInitialized && !ma_device_is_started(m_playbackDevice)) {
        if (ma_device_start(m_playbackDevice) != MA_SUCCESS) {
            qWarning() << "Failed to re-start playback device.";
            return;
        }
    }

    // Инициализируем и запускаем устройство, если оно еще не работает
    if (!m_isDeviceInitialized) {
        ma_device_config config = ma_device_config_init(ma_device_type_playback);
        config.playback.format   = pNewDecoder->outputFormat;
        config.playback.channels = pNewDecoder->outputChannels;
        config.sampleRate        = pNewDecoder->outputSampleRate;
        config.dataCallback      = dataCallback;
        config.pUserData         = this;

        if (ma_device_init(m_context, &config, m_playbackDevice) != MA_SUCCESS) {
            qWarning() << "Failed to initialize playback device.";
            ma_decoder_uninit(pNewDecoder);
            delete pNewDecoder;
            return;
        }

        if (ma_device_start(m_playbackDevice) != MA_SUCCESS) {
            qWarning() << "Failed to start playback device.";
            ma_device_uninit(m_playbackDevice);
            ma_decoder_uninit(pNewDecoder);
            delete pNewDecoder;
            return;
        }
        m_isDeviceInitialized = true;
    }

    // Получаем длительность и отправляем сигнал в UI
    ma_uint64 durationFrames;
    ma_decoder_get_length_in_pcm_frames(pNewDecoder, &durationFrames);
    ma_uint64 durationMillis = (durationFrames * 1000) / pNewDecoder->outputSampleRate;
    emit durationReady(durationMillis);

    // Атомарно подменяем указатель на новый декодер
    m_pDecoder.store(pNewDecoder);

    m_currentPositionMillis.store(0);
    m_positionUpdateTimer->start();

    m_playbackState = Playing;
    qDebug() << "Playback started for:" << filePath;
}

void AudioEngine::pause()
{
    if (m_playbackState == Playing && m_isDeviceInitialized && ma_device_is_started(m_playbackDevice)) {
        ma_device_stop(m_playbackDevice);
        m_positionUpdateTimer->stop();
        m_playbackState = Paused;
        qDebug() << "Playback paused.";
    }
}

void AudioEngine::resume()
{
    if (m_playbackState == Paused && m_isDeviceInitialized) {
        if (ma_device_start(m_playbackDevice) != MA_SUCCESS) {
            qWarning() << "Failed to resume playback device.";
            stopAllSounds(); // В случае ошибки останавливаем все
            return;
        }
        m_positionUpdateTimer->start();
        m_playbackState = Playing;
        qDebug() << "Playback resumed.";
    }
}

void AudioEngine::stopAllSounds()
{
    // Атомарно забираем указатель на текущий декодер и заменяем его на nullptr
    ma_decoder* pOldDecoder = m_pDecoder.exchange(nullptr);

    // Если был старый декодер, безопасно его удаляем
    if (pOldDecoder != nullptr) {
        ma_decoder_uninit(pOldDecoder);
        delete pOldDecoder;
        qDebug() << "Previous sound stopped and decoder uninitialized.";
    }

    if (m_isDeviceInitialized && ma_device_is_started(m_playbackDevice)) {
        ma_device_stop(m_playbackDevice);
        m_positionUpdateTimer->stop();
        m_currentPositionMillis.store(0);
        m_playbackState = Stopped;
        qDebug() << "Playback device stopped.";
    }
}

void AudioEngine::seek(ma_uint64 positionMillis)
{
    m_seekRequestMillis.store(positionMillis);
}

void AudioEngine::setMonitoringVolume(float volume)
{
    float clampedVolume = std::max(0.0f, std::min(1.0f, volume));
    m_monitoringVolume.store(clampedVolume);
    qDebug() << "Monitoring volume set to" << clampedVolume;
}

AudioEngine::PlaybackState AudioEngine::getPlaybackState() const
{
    return m_playbackState;
}

void AudioEngine::onUpdatePositionTimer()
{
    emit positionChanged(m_currentPositionMillis.load());
}

// Этот слот будет вызван безопасно в главном потоке
void AudioEngine::postPlaybackFinished()
{
    stopAllSounds();
    emit playbackFinished();
    qDebug() << "Playback finished signal emitted.";
}
