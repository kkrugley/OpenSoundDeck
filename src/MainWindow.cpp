// src/MainWindow.cpp

#include "MainWindow.h"

#include <QTableWidget>
#include <QVBoxLayout>
#include <QDebug>
#include <QFileDialog>
#include <QStandardPaths>
#include <QHeaderView>
#include <QToolBar>
#include <QStyle>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QMediaPlayer>
#include <QSlider>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // ... (Создание Actions и ToolBar остается без изменений) ...
    m_playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    m_pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    m_stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);
    m_metaDataReader = new QMediaPlayer(this);

    m_playbackToolBar = new QToolBar(tr("Playback"), this);
    m_playbackToolBar->addAction(m_playAction);
    m_playbackToolBar->addAction(m_pauseAction);
    m_playbackToolBar->addAction(m_stopAction);
    addToolBar(m_playbackToolBar);
    
    connect(m_metaDataReader, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);
    
    // --- Центральный виджет ---
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    m_soundTableWidget = new QTableWidget(); // Создаем без родителя
    
    // УДАЛЯЕМ ВСЕ, ЧТО СВЯЗАНО С КНОПКАМИ И buttonLayout
    
    mainLayout->addWidget(m_soundTableWidget);
    
    // --- Настройка таблицы ---
    // *** ДОБАВЛЯЕМ ДВЕ СТРОКИ ДЛЯ DRAG & DROP ***
    setAcceptDrops(true); // Разрешаем принимать "брошенные" объекты на все окно
    m_soundTableWidget->setAcceptDrops(true); // И на саму таблицу тоже
    
    m_soundTableWidget->setColumnCount(4);
    // ... (остальная настройка таблицы без изменений) ...
    m_soundTableWidget->setHorizontalHeaderLabels({tr("Index"), tr("Tag"), tr("Duration"), tr("Hotkey")});
    m_soundTableWidget->verticalHeader()->setVisible(false);
    m_soundTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_soundTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_soundTableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_soundTableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_soundTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_soundTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_soundTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // --- Соединение сигналов и слотов ---
    // УДАЛЯЕМ connect для старых кнопок
    connect(m_playAction, &QAction::triggered, this, &MainWindow::onPlayClicked);
    connect(m_pauseAction, &QAction::triggered, this, &MainWindow::onPauseClicked);
    connect(m_stopAction, &QAction::triggered, this, &MainWindow::onStopClicked);
    
    // ... (Настройки окна без изменений) ...
    setWindowTitle("OpenSoundDeck v0.1 (dev)");
    resize(800, 600);
    setMinimumSize(500, 400);
}

MainWindow::~MainWindow() {}

// --- НОВЫЕ ОБРАБОТЧИКИ СОБЫТИЙ ---

// Этот метод вызывается, когда мышь с зажатым файлом ВХОДИТ в область окна
void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    // Проверяем, содержит ли перетаскиваемый объект ссылки на файлы (URL)
    if (event->mimeData()->hasUrls()) {
        // Если да, то "принимаем" событие. Курсор изменится, показывая, что сюда можно "бросать".
        event->acceptProposedAction();
    }
}

// Этот метод вызывается, когда мы ОТПУСКАЕМ мышь над окном
void MainWindow::dropEvent(QDropEvent *event)
{
    // Проходим по всем ссылкам, которые нам "бросили" (можно бросить несколько файлов сразу)
    for (const QUrl &url : event->mimeData()->urls()) {
        const QString filePath = url.toLocalFile();
        // Проверяем, что это действительно локальный файл, а не ссылка на сайт
        if (!filePath.isEmpty()) {
            addSoundFile(filePath); // Вызываем нашу новую функцию для добавления
        }
    }
}

void MainWindow::addSoundFile(const QString& filePath)
{
    const int newRow = m_soundTableWidget->rowCount();
    m_soundTableWidget->insertRow(newRow);

    QTableWidgetItem *indexItem = new QTableWidgetItem();
    m_soundTableWidget->setItem(newRow, 0, indexItem);

    QFileInfo fileInfo(filePath);
    QTableWidgetItem *tagItem = new QTableWidgetItem(fileInfo.fileName());
    
    // --- НОВЫЙ КОД ДЛЯ DURATION ---
    // 1. Создаем ячейку для длительности и пока ставим в нее "Loading..."
    QTableWidgetItem *durationItem = new QTableWidgetItem(tr("Loading..."));
    
    // 2. Сохраняем номер строки прямо в ячейке. Это хитрый трюк,
    //    чтобы потом в слоте onDurationChanged знать, какую ячейку обновлять.
    durationItem->setData(Qt::UserRole, newRow);
    
    QTableWidgetItem *hotkeyItem = new QTableWidgetItem("None");

    m_soundTableWidget->setItem(newRow, 1, tagItem);
    m_soundTableWidget->setItem(newRow, 2, durationItem); // Ставим ячейку с "Loading..."
    m_soundTableWidget->setItem(newRow, 3, hotkeyItem);

    // 3. Запускаем чтение метаданных
    m_metaDataReader->setSource(QUrl::fromLocalFile(filePath));

    updateIndexes();
    qDebug() << "Started reading metadata for:" << filePath;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 1. Проверяем, была ли нажата именно клавиша Delete.
    if (event->key() == Qt::Key_Delete) {
        
        // 2. Проверяем, что фокус находится именно на нашей таблице.
        //    Это нужно, чтобы случайно не удалить звук, когда фокус
        //    находится на каком-то другом элементе (в будущем).
        if (m_soundTableWidget->hasFocus()) {
            
            // 3. Получаем текущую выделенную строку.
            const int currentRow = m_soundTableWidget->currentRow();
            
            if (currentRow >= 0) {
                // 4. Если строка выделена, удаляем ее и обновляем индексы.
                m_soundTableWidget->removeRow(currentRow);
                updateIndexes();
                qDebug() << "Removed sound at row" << currentRow << "with Delete key.";
            }
        }
    } else {
        // 5. Если была нажата любая другая клавиша, мы передаем
        //    событие родительскому классу для стандартной обработки
        //    (например, для навигации по таблице стрелками).
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::updateIndexes()
{
    // ...
}

void MainWindow::onPlayClicked()
{
    qDebug() << "Play action triggered!";
}

void MainWindow::onPauseClicked()
{
    qDebug() << "Pause action triggered!";
}

void MainWindow::onStopClicked()
{
    qDebug() << "Stop action triggered!";
}

void MainWindow::onDurationChanged(qint64 duration)
{
    // Длительность приходит в миллисекундах.
    if (duration > 0) {

        for (int i = m_soundTableWidget->rowCount() - 1; i >= 0; --i) {
            QTableWidgetItem* item = m_soundTableWidget->item(i, 2); 
            if (item && item->text() == tr("Loading...")) {
                int seconds = duration / 1000;
                int minutes = seconds / 60;
                seconds %= 60;
                
                // Форматируем в строку "М:СС"
                const QString formattedDuration = QString("%1:%2")
                    .arg(minutes)
                    .arg(seconds, 2, 10, QChar('0'));

                item->setText(formattedDuration);
                qDebug() << "Duration found:" << formattedDuration;
                
               break;
            }
        }
    }
}