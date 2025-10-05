// src/MainWindow.h

#pragma once

// Меняем на QMainWindow
#include <QMainWindow>
#include <QKeyEvent>

// Вперед объявляем классы
class QTableWidget;
class QToolBar;
class QAction;
class QDragEnterEvent;
class QDropEvent;
class QMediaPlayer;
class QSlider;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

// Убираем слоты для старых кнопок
private slots:
    void onPlayClicked();
    void onPauseClicked();
    void onStopClicked();
    void onDurationChanged(qint64 duration);
    void onProgressSliderMoved(int position);
    void onHeadphonesVolumeChanged(int value);
    void onMicVolumeChanged(int value);

// Добавляем защищенную секцию для переопределения обработчиков событий
protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void updateIndexes();
    void addSoundFile(const QString& filePath); // Новая вспомогательная функция

    // Центральная часть
    QTableWidget *m_soundTableWidget;

    // Верхняя панель
    QToolBar *m_playbackToolBar;
    QAction *m_playAction;
    QAction *m_pauseAction;
    QAction *m_stopAction;
    QSlider *m_progressSlider;
    QSlider *m_headphonesVolumeSlider;
    QSlider *m_micVolumeSlider;


    QMediaPlayer *m_metaDataReader;
};