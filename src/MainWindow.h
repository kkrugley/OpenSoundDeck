// src/MainWindow.h

#pragma once

#include <QMainWindow>
#include <QKeyEvent>

class QTableWidget;
class QToolBar;
class QAction;
class QDragEnterEvent;
class QDropEvent;
class QKeyEvent;
class QMediaPlayer;
class QSlider;
class QMenu;
class QMenuBar;
class QStatusBar;
class QToolButton;
class QLabel;

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
    void onDurationChanged(qint64 duration);
    void onProgressSliderMoved(int position);
    void onHeadphonesVolumeChanged(int value);
    void onMicVolumeChanged(int value);
    void onExitTriggered();    
    void onHeadphonesToggle(bool checked);
    void onAllToggle(bool checked);
    void onRepeatToggle(bool checked);
    void onPlayerPositionChanged(qint64 position);
    void onPlayerDurationChanged(qint64 duration);
    void onPlayerStateChanged(QMediaPlayer::PlaybackState state);


protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void updateIndexes();
    void addSoundFile(const QString& filePath);

    // System Menu
    QMenuBar *m_menuBar;
    QMenu *m_fileMenu;
    QMenu *m_editMenu;
    QMenu *m_playMenu;
    QMenu *m_windowMenu;
    QMenu *m_helpMenu;
    QAction *m_exitAction;
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

    QMediaPlayer *m_metaDataReader;
    QMediaPlayer *m_player;
};