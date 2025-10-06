/*src/MainWindow.cpp*/

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

#include "MainWindow.h"

#include <QApplication>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QStandardPaths>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QStandardPaths>
#include <QHeaderView>
#include <QToolBar>
#include <QStyle>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QSlider>
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QToolButton>
#include <QLabel>
#include <QMessageBox>
#include <QMediaPlayer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // --- 1. ИНИЦИАЛИЗАЦИЯ СЛУЖЕБНЫХ ОБЪЕКТОВ ---
    m_audioEngine = new AudioEngine(this);
    if (!m_audioEngine->init()) {
        QMessageBox::critical(this, tr("Fatal Error"), tr("Failed to initialize audio engine. The application will now close."));
        // В реальном приложении можно было бы запланировать закрытие, но для простоты пока оставим так
    }
    m_metaDataReader = new QMediaPlayer(this);

    // --- 2. СОЗДАНИЕ ДЕЙСТВИЙ (ACTIONS) ---
    // Меню File
    m_newAction = new QAction(tr("&New"), this);
    m_openAction = new QAction(tr("&Open..."), this);
    m_saveAction = new QAction(tr("&Save"), this);
    m_saveAsAction = new QAction(tr("Save &As..."), this);
    m_exitAction = new QAction(tr("&Exit"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);

    // Меню Edit
    m_cutAction = new QAction(tr("Cu&t"), this);
    m_copyAction = new QAction(tr("&Copy"), this);
    m_pasteAction = new QAction(tr("&Paste"), this);

    // Меню Help
    m_aboutAction = new QAction(tr("&About"), this);
    m_offlineManualAction = new QAction(tr("Offline &Manual"), this);
    m_aboutQtAction = new QAction(tr("About &Qt"), this);


    // Панель инструментов (Playback)
    m_playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    m_pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    m_stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
    m_nextAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipForward), tr("Next"), this);
    m_prevAction = new QAction(style()->standardIcon(QStyle::SP_MediaSkipBackward), tr("Previous"), this);

    // Меню Window
    m_minimizeAction = new QAction(tr("Mi&nimize"), this);
    m_fullscreenAction = new QAction(tr("Toggle &Fullscreen"), this);
    m_keepOnTopAction = new QAction(tr("Keep Above Others"), this);
    m_keepOnTopAction->setCheckable(true);
    m_fullscreenAction->setCheckable(true);

    // Устанавливаем начальное состояние кнопок
    m_playAction->setEnabled(true);
    m_pauseAction->setEnabled(false);
    m_stopAction->setEnabled(false);

    // --- 3. СОЗДАНИЕ ВИДЖЕТОВ ИНТЕРФЕЙСА ---
    // Верхнее меню
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_editMenu = menuBar()->addMenu(tr("&Edit"));
    m_playMenu = menuBar()->addMenu(tr("&Play"));
    m_windowMenu = menuBar()->addMenu(tr("&Window"));
    m_helpMenu = menuBar()->addMenu(tr("&Help"));

    // Панель инструментов
    m_playbackToolBar = new QToolBar(tr("Playback"), this);
    m_progressSlider = new QSlider(Qt::Horizontal, this);
    m_headphonesVolumeSlider = new QSlider(Qt::Vertical, this);
    m_micVolumeSlider = new QSlider(Qt::Vertical, this);

    // Центральная область
    m_soundTableWidget = new QTableWidget();

    // Строка состояния
    m_headphonesButton = new QToolButton(this);
    m_allButton = new QToolButton(this);
    m_repeatButton = new QToolButton(this);
    m_statusLabel = new QLabel(tr("Ready"), this);

    // --- 4. НАСТРОЙКА ВИДЖЕТОВ И КОМПОНОВКА ---
    // Меню
    m_fileMenu->addAction(m_newAction);
    m_fileMenu->addAction(m_openAction);
    m_fileMenu->addAction(m_saveAction);
    m_fileMenu->addAction(m_saveAsAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    m_editMenu->addAction(m_cutAction);
    m_editMenu->addAction(m_copyAction);
    m_editMenu->addAction(m_pasteAction);

    m_playMenu->addAction(m_playAction);
    m_playMenu->addAction(m_pauseAction);
    m_playMenu->addAction(m_stopAction);
    m_playMenu->addSeparator();
    m_playMenu->addAction(m_prevAction);
    m_playMenu->addAction(m_nextAction);

    m_windowMenu->addAction(m_minimizeAction);
    m_windowMenu->addAction(m_fullscreenAction);
    m_windowMenu->addSeparator();
    m_windowMenu->addAction(m_keepOnTopAction);

    m_helpMenu->addAction(m_aboutAction);
    m_helpMenu->addAction(m_offlineManualAction);
    m_helpMenu->addSeparator();
    m_helpMenu->addAction(m_aboutQtAction);
    
    // Панель инструментов
    m_playbackToolBar->addAction(m_playAction);
    m_playbackToolBar->addAction(m_pauseAction);
    m_playbackToolBar->addAction(m_stopAction);
    m_playbackToolBar->addWidget(m_progressSlider);
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_playbackToolBar->addWidget(spacer);
    m_headphonesVolumeSlider->setMaximumHeight(40);
    m_micVolumeSlider->setMaximumHeight(40);
    m_playbackToolBar->addWidget(m_headphonesVolumeSlider);
    m_playbackToolBar->addWidget(m_micVolumeSlider);
    addToolBar(m_playbackToolBar);

    // Центральный виджет
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(m_soundTableWidget);
    
    // Настройка таблицы
    setAcceptDrops(true); 
    m_soundTableWidget->setAcceptDrops(true);
    m_soundTableWidget->setColumnCount(4);
    m_soundTableWidget->setHorizontalHeaderLabels({tr("Index"), tr("Tag"), tr("Duration"), tr("Hotkey")});
    m_soundTableWidget->verticalHeader()->setVisible(false);
    m_soundTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_soundTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_soundTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_soundTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_soundTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_soundTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_soundTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // Строка состояния
    m_headphonesButton->setText("H");
    m_headphonesButton->setCheckable(true);
    m_headphonesButton->setChecked(true);
    m_headphonesButton->setToolTip(tr("Output to headphones"));
    m_allButton->setText("A");
    m_allButton->setCheckable(true);
    m_allButton->setChecked(true);
    m_allButton->setToolTip(tr("Output to all (mic)"));
    m_repeatButton->setText(QString::fromUtf8("↻"));
    m_repeatButton->setCheckable(true);
    m_repeatButton->setToolTip(tr("Repeat playback"));
    statusBar()->addWidget(m_headphonesButton);
    statusBar()->addWidget(m_allButton);
    statusBar()->addWidget(m_statusLabel);
    statusBar()->addPermanentWidget(m_repeatButton);

    // --- 5. СОЕДИНЕНИЕ СИГНАЛОВ И СЛОТОВ ---
    // Служебные
    connect(m_metaDataReader, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);

    // Меню
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::onExitTriggered);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::onOpenTriggered);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);
    connect(m_offlineManualAction, &QAction::triggered, this, &MainWindow::onOfflineManualClicked);
    connect(m_aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);

    // Меню Window
    connect(m_minimizeAction, &QAction::triggered, this, &MainWindow::showMinimized);
    connect(m_keepOnTopAction, &QAction::toggled, this, &MainWindow::onKeepOnTopToggled);
    connect(m_fullscreenAction, &QAction::toggled, this, [this](bool checked){ checked ? showFullScreen() : showNormal(); });

    // Audio
    connect(m_audioEngine, &AudioEngine::durationReady, m_progressSlider, &QSlider::setMaximum);
    connect(m_audioEngine, &AudioEngine::playbackFinished, this, &MainWindow::onPlaybackFinished);

    // Панель инструментов
    connect(m_playAction, &QAction::triggered, this, &MainWindow::onPlayClicked);
    connect(m_pauseAction, &QAction::triggered, this, &MainWindow::onPauseClicked);
    connect(m_stopAction, &QAction::triggered, this, &MainWindow::onStopClicked);
    connect(m_progressSlider, &QSlider::sliderMoved, this, &MainWindow::onProgressSliderMoved);
    connect(m_headphonesVolumeSlider, &QSlider::valueChanged, this, &MainWindow::onHeadphonesVolumeChanged);
    connect(m_micVolumeSlider, &QSlider::valueChanged, this, &MainWindow::onMicVolumeChanged);

    // Строка состояния
    connect(m_headphonesButton, &QToolButton::toggled, this, &MainWindow::onHeadphonesToggle);
    connect(m_allButton, &QToolButton::toggled, this, &MainWindow::onAllToggle);
    connect(m_repeatButton, &QToolButton::toggled, this, &MainWindow::onRepeatToggle);

    // Таблица
    connect(m_soundTableWidget, &QTableWidget::itemDoubleClicked, this, &MainWindow::onSoundTableDoubleClicked);

    // --- 6. НАСТРОЙКИ ОКНА ---
    setWindowTitle("OpenSoundDeck v0.1 (dev)");
    setWindowIcon(QIcon(":/icons/app-icon.png"));
    resize(800, 600);
    setMinimumSize(500, 400);
}

MainWindow::~MainWindow() {}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{

    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    for (const QUrl &url : event->mimeData()->urls()) {
        const QString filePath = url.toLocalFile();
        if (!filePath.isEmpty()) {
            addSoundFile(filePath);
        }
    }
}

void MainWindow::addSoundFile(const QString& filePath)
{
    const int newRow = m_soundTableWidget->rowCount();
    m_soundTableWidget->insertRow(newRow);

    QFileInfo fileInfo(filePath);
    QTableWidgetItem *tagItem = new QTableWidgetItem(fileInfo.fileName());
    tagItem->setData(Qt::UserRole, filePath);
    
    QTableWidgetItem *durationItem = new QTableWidgetItem(tr("Loading..."));
    QTableWidgetItem *hotkeyItem = new QTableWidgetItem("None");

    m_soundTableWidget->setItem(newRow, 1, tagItem);
    m_soundTableWidget->setItem(newRow, 2, durationItem);
    m_soundTableWidget->setItem(newRow, 3, hotkeyItem);

    m_metaDataReader->setSource(QUrl::fromLocalFile(filePath));

    updateIndexes();
    qDebug() << "Added sound:" << filePath;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        
        if (m_soundTableWidget->hasFocus()) {
            
            const int currentRow = m_soundTableWidget->currentRow();
            
            if (currentRow >= 0) {
                m_soundTableWidget->removeRow(currentRow);
                updateIndexes();
                qDebug() << "Removed sound at row" << currentRow << "with Delete key.";
            }
        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::updateIndexes()
{
    // Проходим по всем строкам таблицы
    for (int i = 0; i < m_soundTableWidget->rowCount(); ++i) {
        // Пытаемся получить ячейку в первой колонке (Index)
        QTableWidgetItem *item = m_soundTableWidget->item(i, 0);
        
        // ЕСЛИ ЯЧЕЙКИ НЕТ (item == nullptr), ТО СОЗДАЕМ ЕЕ
        if (!item) {
            item = new QTableWidgetItem();
            m_soundTableWidget->setItem(i, 0, item);
        }
        
        // Устанавливаем правильный номер (i + 1)
        item->setText(QString::number(i + 1));
    }
}

void MainWindow::onExitTriggered()
{
    qApp->quit();
}

void MainWindow::onAboutClicked()
{
    QMessageBox aboutBox(this);
    aboutBox.setWindowTitle(tr("About OpenSoundDeck"));
    aboutBox.setIconPixmap(QPixmap(":/icons/app-icon.png").scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    aboutBox.setTextFormat(Qt::RichText);
    aboutBox.setText("<b>OpenSoundDeck</b>");
    aboutBox.setInformativeText(
        tr("Version 0.1 (dev)<br><br>"
           "Your cross-platform, open-source soundboard for voice chats and streams.<br><br>"
           "Copyright &copy; 2025 Pavel Kruhlei & Contributors<br>"
           "Licensed under the GNU General Public License v3.0<br><br>"
           "<a href='https://github.com/kkrugley/OpenSoundDeck'>Visit on GitHub</a>")
    );
    aboutBox.setStandardButtons(QMessageBox::Ok);
    aboutBox.exec();
}

void MainWindow::onSoundTableDoubleClicked(QTableWidgetItem *item)
{
    if (!item) {
        return;
    }

    const int row = item->row();
    QTableWidgetItem *tagItem = m_soundTableWidget->item(row, 1);
    if (!tagItem) {
        qDebug() << "Invalid tag item on double-click.";
        return;
    }

    const QString filePath = tagItem->data(Qt::UserRole).toString();
    if (filePath.isEmpty()) {
        qDebug() << "No file path associated with this item.";
        return;
    }

    m_audioEngine->playSound(filePath);
    updatePlaybackButtons(true);
}

void MainWindow::onOpenTriggered()
{
    const QStringList fileNames = QFileDialog::getOpenFileNames(this,
                                                              tr("Open Audio Files"),
                                                              QStandardPaths::writableLocation(QStandardPaths::MusicLocation),
                                                              tr("Audio Files (*.mp3 *.wav *.flac *.ogg)"));

    for (const QString &fileName : fileNames) {
        if (!fileName.isEmpty()) {
            addSoundFile(fileName);
        }
    }
}

void MainWindow::onOfflineManualClicked()
{
    QMessageBox::information(this, tr("Offline Manual"), tr("This feature is not implemented yet."));
}

void MainWindow::onKeepOnTopToggled(bool checked)
{
    // Сохраняем текущее состояние окна, так как setWindowFlags может его сбросить
    const bool isMax = isMaximized();
    const bool isMin = isMinimized();

    Qt::WindowFlags flags = windowFlags();
    if (checked) {
        flags |= Qt::WindowStaysOnTopHint;
    } else {
        flags &= ~Qt::WindowStaysOnTopHint;
    }

    // Применяем новые флаги. Это может скрыть окно.
    setWindowFlags(flags);

    // Восстанавливаем состояние окна и показываем его
    if (isMax) showMaximized();
    else if (isMin) showMinimized();
    else show();
}

void MainWindow::onPlayClicked()
{
    if (m_audioEngine->getPlaybackState() == AudioEngine::Paused) {
        m_audioEngine->resume();
    } else {
        const int currentRow = m_soundTableWidget->currentRow();
        if (currentRow < 0 || m_soundTableWidget->rowCount() == 0) {
            qDebug() << "No sound selected to play.";
            return;
        }

        QTableWidgetItem *tagItem = m_soundTableWidget->item(currentRow, 1);
        if (!tagItem) {
            qDebug() << "Invalid item at selected row.";
            return;
        }

        const QString filePath = tagItem->data(Qt::UserRole).toString();
        m_audioEngine->playSound(filePath);
    }

    updatePlaybackButtons(true);
}

void MainWindow::onPauseClicked()
{
    m_audioEngine->pause();
    updatePlaybackButtons(false);
}

void MainWindow::onPlaybackFinished()
{
    // Если воспроизведение закончилось, UI должен быть как при остановке
    updatePlaybackButtons(false);
    m_playAction->setEnabled(true); // Но кнопка Play должна быть доступна
    m_progressSlider->setValue(0);
}

void MainWindow::onStopClicked()
{
    m_audioEngine->stopAllSounds(); // <-- ИСПОЛЬЗУЕМ НАШ ДВИЖОК
    updatePlaybackButtons(false); // Обновляем UI немедленно
    qDebug() << "Stop command sent to audio engine.";
}

void MainWindow::onProgressSliderMoved(int position)
{
    // TODO: Реализовать перемотку в AudioEngine
    // m_audioEngine->seek(position);
    qDebug() << "Seek is not implemented yet.";
}

void MainWindow::onHeadphonesVolumeChanged(int value)
{
    // Конвертируем значение слайдера (0-99) в громкость (0.0-1.0)
    float volume = static_cast<float>(value) / 99.0f;
    m_audioEngine->setMonitoringVolume(volume);
}

void MainWindow::onMicVolumeChanged(int value)
{
    // TODO: Реализовать громкость для микса в AudioEngine
    qDebug() << "Mic volume changed to:" << value;
}

void MainWindow::onHeadphonesToggle(bool checked) { qDebug() << "Headphones output" << (checked ? "ENABLED" : "DISABLED"); }
void MainWindow::onAllToggle(bool checked) { qDebug() << "All (mic) output" << (checked ? "ENABLED" : "DISABLED"); }
void MainWindow::onRepeatToggle(bool checked) { qDebug() << "Repeat" << (checked ? "ON" : "OFF"); }

void MainWindow::onDurationChanged(qint64 duration)
{
    if (duration > 0) {
        for (int i = m_soundTableWidget->rowCount() - 1; i >= 0; --i) {
            QTableWidgetItem* item = m_soundTableWidget->item(i, 2); 
            if (item && item->text() == tr("Loading...")) {
                int seconds = duration / 1000;
                int minutes = seconds / 60;
                seconds %= 60;
                const QString formattedDuration = QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));
                item->setText(formattedDuration);
                qDebug() << "Duration found:" << formattedDuration;
                break;
            }
        }
    }
}

void MainWindow::updatePlaybackButtons(bool isPlaying)
{
    m_playAction->setEnabled(!isPlaying);
    m_pauseAction->setEnabled(isPlaying);
    m_stopAction->setEnabled(isPlaying);
}