/*src/MainWindow.h*/

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

#include <QMainWindow>
#include <QKeyEvent>
#include "AudioEngine.h"

class QTableWidget;
class QTableWidgetItem;
class QToolBar;
class QAction;
class QDragEnterEvent;
class QDropEvent;
class QKeyEvent;
class QSlider;
class QMenu;
class QMenuBar;
class QStatusBar;
class QToolButton;
class QLabel;
class QMediaPlayer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPlayClicked();
    void onPauseClicked();
    void onStopClicked();
    void onOpenTriggered();
    void onSoundTableDoubleClicked(QTableWidgetItem *item);
    void onExitTriggered();    

    void onProgressSliderMoved(int position);
    void onHeadphonesVolumeChanged(int value);
    void onMicVolumeChanged(int value);
    void onHeadphonesToggle(bool checked);
    void onAllToggle(bool checked);
    void onRepeatToggle(bool checked);
    void onDurationChanged(qint64 duration);
    void onAboutClicked();
    void onKeepOnTopToggled(bool checked);
    void onOfflineManualClicked();
    void onPlaybackFinished();


protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void updateIndexes();
    void addSoundFile(const QString& filePath);
    void updatePlaybackButtons(bool isPlaying);

    // System Menu
    QMenuBar *m_menuBar;
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_playMenu;
    QMenu *m_windowMenu;
    QMenu *m_helpMenu;

    // File Actions
    QAction *m_newAction;
    QAction *m_openAction;
    QAction *m_saveAction;
    QAction *m_saveAsAction;
    QAction *m_exitAction;

    // Edit Actions
    QAction *m_cutAction;
    QAction *m_copyAction;
    QAction *m_pasteAction;

    // Play Actions
    QAction *m_nextAction;
    QAction *m_prevAction;

    // Window Actions
    QAction *m_minimizeAction;
    QAction *m_fullscreenAction;
    QAction *m_keepOnTopAction;

    //

    QTableWidget *m_soundTableWidget;

    // Sound Panel
    QToolBar *m_playbackToolBar;
    QAction *m_playAction;
    QAction *m_pauseAction;
    QAction *m_stopAction;
    QSlider *m_progressSlider;
    QSlider *m_headphonesVolumeSlider;
    QSlider *m_micVolumeSlider;
    //

    // Bottom status bar
    QStatusBar *m_statusBar;
    QToolButton *m_headphonesButton;
    QToolButton *m_allButton;
    QToolButton *m_repeatButton;
    QLabel *m_statusLabel;
    // 
    
    // Help Actions
    QAction *m_aboutAction;
    QAction *m_offlineManualAction;
    QAction *m_aboutQtAction;

    QMediaPlayer *m_metaDataReader;
    AudioEngine *m_audioEngine;
};