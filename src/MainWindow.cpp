// src/MainWindow.cpp

#include "MainWindow.h"

// Подключаем заголовки для всех используемых классов
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout> // Вертикальный layout
#include <QHBoxLayout> // Горизонтальный layout
#include <QDebug>      // Для вывода отладочной информации в консоль
#include <QFileDialog> // Класс для диалога выбора файлов
#include <QStandardPaths> // Для получения стандартных путей (например, домашней папки)

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    // --- 1. Создание виджетов ---
    // this в качестве аргумента говорит, что MainWindow будет владеть
    // этими виджетами и удалит их, когда будет удалена сама.
    m_soundListWidget = new QListWidget(this);
    m_addButton = new QPushButton("Add Sound", this);
    m_removeButton = new QPushButton("Remove Sound", this);

    // --- 2. Создание менеджеров компоновки (Layouts) ---
    // Главный layout, который будет располагать элементы вертикально
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Вспомогательный layout для кнопок, располагает их горизонтально
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    // --- 3. Настройка компоновки ---
    // Добавляем кнопки в горизонтальный layout
    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_removeButton);
    
    // Добавляем наш список звуков в главный вертикальный layout
    mainLayout->addWidget(m_soundListWidget);
    // Добавляем горизонтальный layout с кнопками под списком
    mainLayout->addLayout(buttonLayout);

    // Устанавливаем главный layout для нашего окна
    this->setLayout(mainLayout);

    // --- 4. Соединение сигналов и слотов ---
    // Это ключевой механизм Qt. Мы "говорим":
    // "Когда на кнопке m_addButton произойдет сигнал clicked(),
    // вызови у этого (this) объекта метод onAddSoundClicked()".
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::onAddSoundClicked);
    connect(m_removeButton, &QPushButton::clicked, this, &MainWindow::onRemoveSoundClicked);
    
    // Настройки окна оставляем здесь же
    setWindowTitle("OpenSoundDeck v0.1 (dev)");
    resize(800, 600);
}

MainWindow::~MainWindow()
{
    // Qt автоматически удалит все дочерние виджеты, так что здесь пока пусто
}

// --- Реализация слотов ---

void MainWindow::onRemoveSoundClicked()
{
    qDebug() << "Remove Sound button was clicked!";
}

void MainWindow::onAddSoundClicked()
{
    // 1. Определяем, откуда начать поиск файлов.
    // QStandardPaths::MusicLocation - это стандартная папка "Музыка" в системе.
    // Если она не найдена, используем домашнюю директорию.
    const QString startDir = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);

    // 2. Вызываем статический метод getOpenFileName, который создает и показывает диалог.
    const QString filePath = QFileDialog::getOpenFileName(
        this,                                   // Родительское окно
        tr("Open Sound File"),                  // Заголовок окна диалога
        startDir,                               // Стартовая директория
        tr("Audio Files (*.mp3 *.wav *.ogg *.flac);;All Files (*)") // Фильтр файлов
    );

    // 3. Проверяем результат. Если пользователь нажал "Отмена", filePath будет пустой.
    if (filePath.isEmpty()) {
        qDebug() << "No file selected.";
        return; // Выходим из функции
    }

    // 4. Если файл выбран, добавляем его путь в наш список.
    m_soundListWidget->addItem(filePath);
    qDebug() << "Added sound:" << filePath;
}