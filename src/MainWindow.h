// src/MainWindow.h

#pragma once

#include <QWidget>

// Вперед объявляем классы, чтобы не подключать лишние заголовки здесь.
// Это хорошая практика, которая ускоряет компиляцию.
class QListWidget;
class QPushButton;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

// private slots - это секция для функций, которые будут реагировать на события (сигналы)
private slots:
    void onAddSoundClicked();
    void onRemoveSoundClicked();

// private - секция для внутренних данных класса
private:
    // Указатели на элементы нашего интерфейса
    QListWidget *m_soundListWidget; // Список для отображения звуков
    QPushButton *m_addButton;       // Кнопка "Добавить"
    QPushButton *m_removeButton;    // Кнопка "Удалить"
};