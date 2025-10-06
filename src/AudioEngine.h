// src/AudioEngine.h

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
 
#pragma once

#include <QObject>
#include <QString>
#include <atomic> // Для атомарных операций
#include "miniaudio.h"

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
    void playSound(const QString& filePath);
    void pause();
    void resume();
    void stopAllSounds();
    void setMonitoringVolume(float volume);
    PlaybackState getPlaybackState() const;

signals:
    // Сигналы для обратной связи с UI
    void positionChanged(ma_uint64 positionMillis);
    void durationReady(ma_uint64 durationMillis);
    void playbackFinished();

private:
    static void dataCallback(ma_device* pDevice, void* pOutput, const void* pInput, ma_uint32 frameCount);
    void postPlaybackFinished(); // Вспомогательная функция для безопасного вызова сигнала

private:
    ma_context* m_context;
    ma_device* m_playbackDevice;
    std::atomic<ma_decoder*> m_pDecoder; // Атомарный указатель на декодер

    float m_monitoringVolume;
    bool m_isDeviceInitialized;
    PlaybackState m_playbackState;
};