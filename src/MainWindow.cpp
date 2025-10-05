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
#include <QMenuBar>
#include <QMenu>
#include <QStatusBar>
#include <QToolButton>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    m_metaDataReader = new QMediaPlayer(this);
    connect(m_metaDataReader, &QMediaPlayer::durationChanged, this, &MainWindow::onDurationChanged);

    // Exit action
    m_exitAction = new QAction(tr("&Exit"), this);
    m_exitAction->setShortcut(QKeySequence::Quit);

    // Dropdown menu
    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_editMenu = menuBar()->addMenu(tr("&Edit"));
    m_playMenu = menuBar()->addMenu(tr("&Play"));
    m_windowMenu = menuBar()->addMenu(tr("&Window"));
    m_helpMenu = menuBar()->addMenu(tr("&Help"));
    
    // 3. Dropdown File category
    m_fileMenu->addAction(m_exitAction);

    m_playAction = new QAction(style()->standardIcon(QStyle::SP_MediaPlay), tr("Play"), this);
    m_pauseAction = new QAction(style()->standardIcon(QStyle::SP_MediaPause), tr("Pause"), this);
    m_stopAction = new QAction(style()->standardIcon(QStyle::SP_MediaStop), tr("Stop"), this);

    m_playbackToolBar = new QToolBar(tr("Playback"), this);
    m_playbackToolBar->addAction(m_playAction);
    m_playbackToolBar->addAction(m_pauseAction);
    m_playbackToolBar->addAction(m_stopAction);

    m_progressSlider = new QSlider(Qt::Horizontal, this);
    m_playbackToolBar->addWidget(m_progressSlider);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_playbackToolBar->addWidget(spacer);

    m_headphonesVolumeSlider = new QSlider(Qt::Vertical, this);
    m_micVolumeSlider = new QSlider(Qt::Vertical, this);

    m_headphonesVolumeSlider->setMaximumHeight(40);
    m_micVolumeSlider->setMaximumHeight(40);
    m_playbackToolBar->addWidget(m_headphonesVolumeSlider);
    m_playbackToolBar->addWidget(m_micVolumeSlider);
    
    addToolBar(m_playbackToolBar);
    
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    m_soundTableWidget = new QTableWidget();
 
    mainLayout->addWidget(m_soundTableWidget);
    
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

    connect(m_playAction, &QAction::triggered, this, &MainWindow::onPlayClicked);
    connect(m_pauseAction, &QAction::triggered, this, &MainWindow::onPauseClicked);
    connect(m_stopAction, &QAction::triggered, this, &MainWindow::onStopClicked);
    
    connect(m_progressSlider, &QSlider::sliderMoved, this, &MainWindow::onProgressSliderMoved);
    connect(m_headphonesVolumeSlider, &QSlider::valueChanged, this, &MainWindow::onHeadphonesVolumeChanged);
    connect(m_micVolumeSlider, &QSlider::valueChanged, this, &MainWindow::onMicVolumeChanged);
    connect(m_exitAction, &QAction::triggered, this, &MainWindow::onExitTriggered);
    connect(m_headphonesButton, &QToolButton::toggled, this, &MainWindow::onHeadphonesToggle);
    connect(m_allButton, &QToolButton::toggled, this, &MainWindow::onAllToggle);
    connect(m_repeatButton, &QToolButton::toggled, this, &MainWindow::onRepeatToggle);

    setWindowTitle("OpenSoundDeck v0.1 (dev)");
    resize(800, 600);
    setMinimumSize(500, 400);

    m_headphonesButton = new QToolButton(this);
    m_headphonesButton->setText("H");
    m_headphonesButton->setCheckable(true); // Делаем кнопку "залипающей"
    m_headphonesButton->setChecked(true);   // Включаем по умолчанию
    m_headphonesButton->setToolTip(tr("Output to headphones"));

    m_allButton = new QToolButton(this);
    m_allButton->setText("A");
    m_allButton->setCheckable(true);
    m_allButton->setChecked(true);
    m_allButton->setToolTip(tr("Output to all (mic)"));

    m_repeatButton = new QToolButton(this);
    m_repeatButton->setText(QString::fromUtf8("↻")); // Используем символ Unicode
    m_repeatButton->setCheckable(true);
    m_repeatButton->setToolTip(tr("Repeat playback"));

    m_statusLabel = new QLabel(tr("Ready"), this);
    
    // 2. Добавляем виджеты в строку состояния
    // statusBar() сам создает QStatusBar, если его еще нет
    statusBar()->addWidget(m_headphonesButton);
    statusBar()->addWidget(m_allButton);
    statusBar()->addWidget(m_statusLabel);

    // addPermanentWidget добавляет виджет справа
    statusBar()->addPermanentWidget(m_repeatButton);
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

    QTableWidgetItem *indexItem = new QTableWidgetItem();
    m_soundTableWidget->setItem(newRow, 0, indexItem);

    QFileInfo fileInfo(filePath);
    QTableWidgetItem *tagItem = new QTableWidgetItem(fileInfo.fileName());
    
    QTableWidgetItem *durationItem = new QTableWidgetItem(tr("Loading..."));
    
    durationItem->setData(Qt::UserRole, newRow);
    
    QTableWidgetItem *hotkeyItem = new QTableWidgetItem("None");

    m_soundTableWidget->setItem(newRow, 1, tagItem);
    m_soundTableWidget->setItem(newRow, 2, durationItem);
    m_soundTableWidget->setItem(newRow, 3, hotkeyItem);

    m_metaDataReader->setSource(QUrl::fromLocalFile(filePath));

    updateIndexes();
    qDebug() << "Started reading metadata for:" << filePath;
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
    // ...
}

void MainWindow::onExitTriggered()
{
    qApp->quit();
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

void MainWindow::onProgressSliderMoved(int position)
{
    qDebug() << "Progress slider moved to position:" << position;
}

void MainWindow::onHeadphonesVolumeChanged(int value)
{
    qDebug() << "Headphones volume changed to:" << value;
}

void MainWindow::onMicVolumeChanged(int value)
{
    qDebug() << "Mic volume changed to:" << value;
}

void MainWindow::onHeadphonesToggle(bool checked)
{
    qDebug() << "Headphones output" << (checked ? "ENABLED" : "DISABLED");
    // Здесь будет логика включения/выключения вывода в наушники
}

void MainWindow::onAllToggle(bool checked)
{
    qDebug() << "All (mic) output" << (checked ? "ENABLED" : "DISABLED");
    // Здесь будет логика включения/выключения вывода в микрофон
}

void MainWindow::onRepeatToggle(bool checked)
{
    qDebug() << "Repeat" << (checked ? "ON" : "OFF");
    // Здесь будет логика повтора воспроизведения
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