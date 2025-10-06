/*src/MainWindow.cpp*/

/* src/MainWindow.cpp */
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
#include "SettingsDialog.h"
#include "GlobalHotkeyManager.h"
#include "HotkeyCaptureDialog.h"

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
#include <QSettings>
#include <QMediaPlayer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // --- 1. ИНИЦИАЛИЗАЦИЯ СЛУЖЕБНЫХ ОБЪЕКТОВ ---
    m_audioEngine = new AudioEngine(this);
    if (!m_audioEngine->init()) {
        // TODO: Handle audio engine initialization failure more gracefully
        QMessageBox::critical(this, tr("Fatal Error"), tr("Failed to initialize audio engine. The application will now close."));
        // В реальном приложении можно было бы запланировать закрытие, но для простоты пока оставим так
    }
    m_metaDataReader = new QMediaPlayer(this);
    m_hotkeyManager = new GlobalHotkeyManager(this);
    connect(m_hotkeyManager, &GlobalHotkeyManager::hotkeyActivated, this, [this](int row){
        playTrackAtRow(row);
    });

    // --- 2. СОЗДАНИЕ ДЕЙСТВИЙ (ACTIONS) ---
    // Меню File
    m_newAction = new QAction(tr("&New Playlist"), this);
    m_isRepeatEnabled = false;
    m_openAction = new QAction(tr("&Open Playlist"), this);
    m_importAction = new QAction(tr("&Import Audio"), this);
    m_saveAction = new QAction(tr("&Save Playlist"), this);
    m_saveAsAction = new QAction(tr("Save Playlist &As..."), this);
    m_exitAction = new QAction(tr("&Exit"), this);

    m_newAction->setShortcut(QKeySequence::New);
    m_openAction->setShortcut(QKeySequence::Open);
    m_importAction->setShortcut(tr("Ctrl+I"));
    m_saveAction->setShortcut(QKeySequence::Save);
    m_saveAsAction->setShortcut(QKeySequence::SaveAs); // Standard is Ctrl+Shift+S, but we'll use what's requested
    m_exitAction->setShortcut(QKeySequence::Quit); // Ctrl+Q

    // Меню Edit
    m_cutAction = new QAction(tr("Cu&t"), this);
    m_copyAction = new QAction(tr("&Copy"), this);
    m_settingsAction = new QAction(tr("&Settings..."), this);
    m_pasteAction = new QAction(tr("&Paste"), this);
    m_settingsAction->setShortcut(tr("Ctrl+P"));

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
    m_headphonesVolumeSlider = new QSlider(Qt::Horizontal, this);
    m_micVolumeSlider = new QSlider(Qt::Horizontal, this);
    m_headphonesMuteButton = new QToolButton(this);
    m_micMuteButton = new QToolButton(this);

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
    m_fileMenu->addAction(m_importAction);
    m_fileMenu->addSeparator();
    m_fileMenu->addAction(m_exitAction);

    m_editMenu->addAction(m_cutAction);
    m_editMenu->addAction(m_copyAction);
    m_editMenu->addAction(m_pasteAction);
    m_editMenu->addSeparator();
    m_editMenu->addAction(m_settingsAction);
    
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
    m_progressSlider->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QWidget* spacer = new QWidget();
    spacer->setFixedWidth(20); // Добавляем небольшой отступ в 20 пикселей
    m_playbackToolBar->addWidget(spacer);

    // --- Громкость наушников ---
    m_headphonesMuteButton->setCheckable(true);
    m_headphonesMuteButton->setChecked(false);
    m_headphonesVolumeSlider->setRange(0, 100);
    m_headphonesVolumeSlider->setValue(80);
    m_headphonesVolumeSlider->setFixedWidth(120);
    updateHeadphonesVolumeIcon(m_headphonesVolumeSlider->value());

    // --- Громкость микрофона ---
    m_micMuteButton->setCheckable(true);
    m_micMuteButton->setChecked(false);
    m_micVolumeSlider->setRange(0, 100);
    m_micVolumeSlider->setValue(80);
    m_micVolumeSlider->setFixedWidth(120);
    updateMicVolumeIcon(m_micVolumeSlider->value());

    // Добавляем виджеты на панель инструментов
    m_playbackToolBar->addWidget(m_headphonesMuteButton);
    m_playbackToolBar->addWidget(m_headphonesVolumeSlider);
    m_playbackToolBar->addWidget(m_micMuteButton);
    m_playbackToolBar->addWidget(m_micVolumeSlider);

    addToolBar(m_playbackToolBar);
    m_headphonesVolume = m_headphonesVolumeSlider->value(); // Сохраняем начальное значение
    m_micVolume = m_micVolumeSlider->value();

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
    m_soundTableWidget->setEditTriggers(QAbstractItemView::EditKeyPressed); // Разрешаем редактирование по F2
    m_soundTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_soundTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    m_soundTableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    
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
    connect(m_newAction, &QAction::triggered, this, &MainWindow::onNewTriggered);
    connect(m_openAction, &QAction::triggered, this, &MainWindow::onOpenTriggered);
    connect(m_importAction, &QAction::triggered, this, &MainWindow::onImportTriggered);
    connect(m_saveAction, &QAction::triggered, this, &MainWindow::onSaveTriggered);
    connect(m_saveAsAction, &QAction::triggered, this, &MainWindow::onSaveAsTriggered);
    connect(m_settingsAction, &QAction::triggered, this, &MainWindow::onSettingsClicked);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAboutClicked);
    connect(m_offlineManualAction, &QAction::triggered, this, &MainWindow::onOfflineManualClicked);
    connect(m_aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);

    // Меню Window
    connect(m_minimizeAction, &QAction::triggered, this, &MainWindow::showMinimized);
    connect(m_keepOnTopAction, &QAction::toggled, this, &MainWindow::onKeepOnTopToggled);
    connect(m_fullscreenAction, &QAction::toggled, this, [this](bool checked){ checked ? showFullScreen() : showNormal(); });

    // Audio
    connect(m_audioEngine, &AudioEngine::durationReady, m_progressSlider, &QSlider::setMaximum);
    connect(m_audioEngine, &AudioEngine::positionChanged, this, &MainWindow::onPositionChanged);
    connect(m_audioEngine, &AudioEngine::playbackFinished, this, &MainWindow::onPlaybackFinished);

    // Панель инструментов
    connect(m_playAction, &QAction::triggered, this, &MainWindow::onPlayClicked);
    connect(m_pauseAction, &QAction::triggered, this, &MainWindow::onPauseClicked);
    connect(m_stopAction, &QAction::triggered, this, &MainWindow::onStopClicked);
    connect(m_nextAction, &QAction::triggered, this, &MainWindow::onNextClicked);
    connect(m_prevAction, &QAction::triggered, this, &MainWindow::onPrevClicked);
    connect(m_progressSlider, &QSlider::sliderMoved, this, &MainWindow::onProgressSliderMoved);
    connect(m_headphonesVolumeSlider, &QSlider::valueChanged, this, &MainWindow::onHeadphonesVolumeChanged);
    connect(m_headphonesMuteButton, &QToolButton::clicked, this, &MainWindow::onHeadphonesMuteClicked);
    connect(m_micVolumeSlider, &QSlider::valueChanged, this, &MainWindow::onMicVolumeChanged);
    connect(m_micMuteButton, &QToolButton::clicked, this, &MainWindow::onMicMuteClicked);

    // Строка состояния
    connect(m_headphonesButton, &QToolButton::toggled, this, &MainWindow::onHeadphonesToggle);
    connect(m_allButton, &QToolButton::toggled, this, &MainWindow::onAllToggle);
    connect(m_repeatButton, &QToolButton::toggled, this, &MainWindow::onRepeatToggle);

    // Таблица
    connect(m_soundTableWidget, &QTableWidget::itemDoubleClicked, this, &MainWindow::onSoundTableDoubleClicked);
    connect(m_soundTableWidget, &QTableWidget::itemChanged, this, &MainWindow::onSoundItemChanged);
    connect(m_soundTableWidget, &QTableWidget::customContextMenuRequested, this, &MainWindow::onSoundTableContextMenuRequested);

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

    playTrackAtRow(item->row());
}

void MainWindow::onSettingsClicked()
{
    SettingsDialog dialog(this);
    dialog.exec();
}

void MainWindow::onNewTriggered()
{
    // TODO: Prompt to save if modified
    m_soundTableWidget->setRowCount(0);
    m_currentPlaylistPath.clear();
}

void MainWindow::onOpenTriggered()
{
    const QString libraryPath = getLibraryPath();
    const QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        tr("Open Files"),
        libraryPath,
        tr("Playlist Files (*.osdpl)"));

    if (fileNames.isEmpty()) {
        return;
    }

    QFile file(fileNames.first());
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QMessageBox::warning(this, tr("Error"), tr("Could not open playlist file."));
            return;
        }

        m_soundTableWidget->setRowCount(0); // Очищаем таблицу

        QTextStream in(&file);
        while (!in.atEnd()) {
            QString line = in.readLine();
            if (!line.trimmed().isEmpty()) {
                addSoundFile(line);
            }
        }
        file.close();
        m_currentPlaylistPath = fileNames.first();
        qDebug() << "Playlist loaded from" << m_currentPlaylistPath;
}

void MainWindow::onSoundTableContextMenuRequested(const QPoint &pos)
{
    QTableWidgetItem *item = m_soundTableWidget->itemAt(pos);
    if (!item) {
        return; // Клик не по треку
    }

    QMenu contextMenu(this);
    QAction *renameAction = contextMenu.addAction(tr("Rename"));
    QAction *assignHotkeyAction = contextMenu.addAction(tr("Assign Hotkey"));

    contextMenu.addSeparator();
    QAction *moveUpAction = contextMenu.addAction(tr("Move Up"));
    QAction *moveDownAction = contextMenu.addAction(tr("Move Down"));
    QAction *duplicateAction = contextMenu.addAction(tr("Duplicate"));
    contextMenu.addSeparator();
    QAction *trimAction = contextMenu.addAction(tr("Trim"));
    trimAction->setEnabled(false); // Пока недоступно
    contextMenu.addSeparator();
    QAction *removeAction = contextMenu.addAction(tr("Remove from Playlist"));

    // Отключаем "Вверх", если трек первый
    if (item->row() == 0) {
        moveUpAction->setEnabled(false);
    }
    // Отключаем "Вниз", если трек последний
    if (item->row() == m_soundTableWidget->rowCount() - 1) {
        moveDownAction->setEnabled(false);
    }

    connect(renameAction, &QAction::triggered, this, &MainWindow::onRenameTrack);
    connect(assignHotkeyAction, &QAction::triggered, this, &MainWindow::onAssignHotkey);
    connect(removeAction, &QAction::triggered, this, &MainWindow::onRemoveTrack);
    connect(duplicateAction, &QAction::triggered, this, &MainWindow::onDuplicateTrack);
    connect(moveUpAction, &QAction::triggered, this, &MainWindow::onMoveTrackUp);
    connect(moveDownAction, &QAction::triggered, this, &MainWindow::onMoveTrackDown);

    contextMenu.exec(m_soundTableWidget->viewport()->mapToGlobal(pos));
}

void MainWindow::onRenameTrack()
{
    QTableWidgetItem *item = m_soundTableWidget->currentItem();
    if (item) {
        // Нас интересует только колонка с тегом
        QTableWidgetItem* tagItem = m_soundTableWidget->item(item->row(), 1);
        if (tagItem) {
            m_soundTableWidget->editItem(tagItem);
        }
    }
}

void MainWindow::onRemoveTrack()
{
    int currentRow = m_soundTableWidget->currentRow();
    m_hotkeyManager->unregisterHotkey(currentRow);
    if (currentRow >= 0) {
        m_soundTableWidget->removeRow(currentRow);
        updateIndexes();
    }
}

void MainWindow::onDuplicateTrack()
{
    int currentRow = m_soundTableWidget->currentRow();
    if (currentRow < 0) return;

    QTableWidgetItem *tagItem = m_soundTableWidget->item(currentRow, 1);
    if (tagItem) {
        QString filePath = tagItem->data(Qt::UserRole).toString();
        addSoundFile(filePath);
    }
}

void MainWindow::onMoveTrackUp()
{
    int currentRow = m_soundTableWidget->currentRow();
    if (currentRow > 0) {
        m_soundTableWidget->insertRow(currentRow - 1);
        for (int col = 0; col < m_soundTableWidget->columnCount(); ++col) {
            m_soundTableWidget->setItem(currentRow - 1, col, m_soundTableWidget->takeItem(currentRow + 1, col));
        }
        m_soundTableWidget->removeRow(currentRow + 1);
        m_soundTableWidget->setCurrentCell(currentRow - 1, 0);
        updateIndexes();
    }
}

void MainWindow::onMoveTrackDown()
{
    int currentRow = m_soundTableWidget->currentRow();
    if (currentRow < m_soundTableWidget->rowCount() - 1) {
        m_soundTableWidget->insertRow(currentRow + 2);
        for (int col = 0; col < m_soundTableWidget->columnCount(); ++col) {
            m_soundTableWidget->setItem(currentRow + 2, col, m_soundTableWidget->takeItem(currentRow, col));
        }
        m_soundTableWidget->removeRow(currentRow);
        m_soundTableWidget->setCurrentCell(currentRow + 1, 0);
        updateIndexes();
    }
}

void MainWindow::onSoundItemChanged(QTableWidgetItem *item)
{
    // Убедимся, что изменилась именно ячейка с тегом (именем)
    if (item->column() != 1) {
        return;
    }

    QString oldFilePath = item->data(Qt::UserRole).toString();
    QFileInfo oldFileInfo(oldFilePath);

    QString newFileName = item->text();
    // Если пользователь не добавил расширение, сохраняем старое
    if (!newFileName.endsWith("." + oldFileInfo.suffix())) {
        newFileName += "." + oldFileInfo.suffix();
    }

    QString newFilePath = oldFileInfo.absolutePath() + "/" + newFileName;

    if (oldFilePath == newFilePath) {
        return; // Имя не изменилось
    }

    QFile file(oldFilePath);
    if (file.rename(newFilePath)) {
        // Успешно переименовали файл, теперь обновим путь в данных ячейки
        item->setData(Qt::UserRole, newFilePath);
        qDebug() << "Renamed" << oldFilePath << "to" << newFilePath;
    } else {
        // Ошибка переименования, вернем старое имя в ячейку
        qWarning() << "Failed to rename" << oldFilePath << "to" << newFilePath;
        QMessageBox::warning(this, tr("Rename Error"), tr("Could not rename the file on disk."));
        item->setText(oldFileInfo.fileName()); // Откат имени в таблице
    }
}
void MainWindow::onImportTriggered()
{
    const QString libraryPath = getLibraryPath();
    const QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        tr("Import Audio Files"),
        libraryPath,
        tr("Audio Files (*.mp3 *.wav *.flac *.ogg)"));

    for (const QString &fileName : fileNames) {
        if (!fileName.isEmpty()) {
            addSoundFile(fileName);
        }
    }
}

void MainWindow::onSaveTriggered()
{
    if (m_currentPlaylistPath.isEmpty()) {
        onSaveAsTriggered();
    } else {
        savePlaylist(m_currentPlaylistPath);
    }
}

void MainWindow::onSaveAsTriggered()
{
    const QString libraryPath = getLibraryPath();
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Playlist"), libraryPath, tr("Playlist Files (*.osdpl)"));

    if (!fileName.isEmpty()) {
        savePlaylist(fileName);
    }
}

void MainWindow::savePlaylist(const QString& fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Could not save playlist file."));
        return;
    }

    QTextStream out(&file);
    for (int i = 0; i < m_soundTableWidget->rowCount(); ++i) {
        QTableWidgetItem *item = m_soundTableWidget->item(i, 1); // Колонка "Tag"
        if (item) {
            QString filePath = item->data(Qt::UserRole).toString();
            if (!filePath.isEmpty()) {
                out << filePath << "\n";
            }
        }
    }

    file.close();
    m_currentPlaylistPath = fileName;
    qDebug() << "Playlist saved to" << m_currentPlaylistPath;
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

        playTrackAtRow(currentRow);
    }
}

void MainWindow::onPauseClicked()
{
    m_audioEngine->pause();
    updatePlaybackButtons(false);
}

void MainWindow::onNextClicked()
{
    const int rowCount = m_soundTableWidget->rowCount();
    if (rowCount == 0) {
        return;
    }

    int nextRow = m_soundTableWidget->currentRow() + 1;
    if (nextRow >= rowCount) {
        nextRow = 0; // Зацикливание на начало списка
    }

    playTrackAtRow(nextRow);
}

void MainWindow::onPrevClicked()
{
    const int rowCount = m_soundTableWidget->rowCount();
    if (rowCount == 0) {
        return;
    }

    int prevRow = m_soundTableWidget->currentRow() - 1;
    if (prevRow < 0) {
        prevRow = rowCount - 1; // Зацикливание на конец списка
    }

    playTrackAtRow(prevRow);
}

void MainWindow::playTrackAtRow(int row)
{
    if (row < 0 || row >= m_soundTableWidget->rowCount()) {
        qDebug() << "Invalid row index to play:" << row;
        return;
    }

    QTableWidgetItem *tagItem = m_soundTableWidget->item(row, 1);
    if (!tagItem) {
        qDebug() << "Invalid tag item at row" << row;
        return;
    }

    const QString filePath = tagItem->data(Qt::UserRole).toString();
    if (filePath.isEmpty()) {
        qDebug() << "No file path associated with row" << row;
        return;
    }

    m_soundTableWidget->setCurrentCell(row, 0); // Выделяем новую строку
    m_audioEngine->playSound(filePath);
    updatePlaybackButtons(true);
}

void MainWindow::onPlaybackFinished()
{
    if (m_isRepeatEnabled) {
        // Если включен повтор, просто проигрываем текущий трек заново
        playTrackAtRow(m_soundTableWidget->currentRow());
    } else {
        // Иначе, останавливаем воспроизведение
        updatePlaybackButtons(false);
        m_playAction->setEnabled(true); // Но кнопка Play должна быть доступна
        m_progressSlider->setValue(0);
    }
}

void MainWindow::onStopClicked()
{
    m_audioEngine->stopAllSounds(); // <-- ИСПОЛЬЗУЕМ НАШ ДВИЖОК
    updatePlaybackButtons(false); // Обновляем UI немедленно
    qDebug() << "Stop command sent to audio engine.";
}

void MainWindow::onPositionChanged(ma_uint64 position)
{
    // Блокируем сигналы, чтобы избежать рекурсивного вызова onProgressSliderMoved
    m_progressSlider->blockSignals(true);
    m_progressSlider->setValue(position);
    m_progressSlider->blockSignals(false);
}

void MainWindow::onProgressSliderMoved(int position)
{
    m_audioEngine->seek(position);
}

void MainWindow::onHeadphonesVolumeChanged(int value)
{
    // Конвертируем значение слайдера (0-99) в громкость (0.0-1.0)
    float volume = static_cast<float>(value) / 100.0f;
    m_audioEngine->setMonitoringVolume(volume);
    updateHeadphonesVolumeIcon(value);

    // Если звук не выключен, сохраняем текущее значение
    if (value > 0) {
        m_headphonesVolume = value;
    }
}

void MainWindow::onMicVolumeChanged(int value)
{
    // TODO: Реализовать громкость для микса в AudioEngine
    qDebug() << "Mic volume changed to:" << value;
    updateMicVolumeIcon(value);

    if (value > 0) {
        m_micVolume = value;
    }
}

void MainWindow::onHeadphonesMuteClicked(bool checked)
{
    if (checked) { // Mute
        m_headphonesVolumeSlider->setValue(0);
    } else { // Unmute
        m_headphonesVolumeSlider->setValue(m_headphonesVolume);
    }
}

void MainWindow::onMicMuteClicked(bool checked)
{
    if (checked) { // Mute
        m_micVolumeSlider->setValue(0);
    } else { // Unmute
        m_micVolumeSlider->setValue(m_micVolume);
    }
}

void MainWindow::updateHeadphonesVolumeIcon(int value)
{
    if (value == 0) {
        m_headphonesMuteButton->setIcon(QIcon(":/icons/headphones-off.png"));
        m_headphonesMuteButton->setChecked(true);
    } else {
        m_headphonesMuteButton->setIcon(QIcon(":/icons/headphones.png"));
        m_headphonesMuteButton->setChecked(false);
    }
}

void MainWindow::updateMicVolumeIcon(int value)
{
    if (value == 0) {
        m_micMuteButton->setIcon(QIcon(":/icons/microphone-off.png"));
        m_micMuteButton->setChecked(true);
    } else {
        m_micMuteButton->setIcon(QIcon(":/icons/microphone.png"));
        m_micMuteButton->setChecked(false);
    }
}

void MainWindow::onHeadphonesToggle(bool checked) { qDebug() << "Headphones output" << (checked ? "ENABLED" : "DISABLED"); }
void MainWindow::onAllToggle(bool checked) { qDebug() << "All (mic) output" << (checked ? "ENABLED" : "DISABLED"); }
void MainWindow::onRepeatToggle(bool checked)
{
    m_isRepeatEnabled = checked;
    qDebug() << "Repeat" << (checked ? "ON" : "OFF");
}

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

void MainWindow::onAssignHotkey()
{
    const int currentRow = m_soundTableWidget->currentRow();
    if (currentRow < 0) {
        return;
    }

    HotkeyCaptureDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QKeySequence hotkey = dialog.getHotkey();

        // Сначала отменяем регистрацию старого хоткея для этой строки, если он был
        m_hotkeyManager->unregisterHotkey(currentRow);

        if (!hotkey.isEmpty()) {
            // Регистрируем новый хоткей
            if (m_hotkeyManager->registerHotkey(hotkey, currentRow)) {
                m_soundTableWidget->item(currentRow, 3)->setText(hotkey.toString(QKeySequence::NativeText));
            } else {
                QMessageBox::warning(this, tr("Hotkey Error"), tr("Failed to register hotkey. It might be already in use by another application."));
                m_soundTableWidget->item(currentRow, 3)->setText("None");
            }
        }
    }
}

QString MainWindow::getLibraryPath() const
{
    QSettings settings("kkrugley", "OpenSoundDeck");
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    return settings.value("library/path", defaultPath).toString();
}