// src/main.cpp

#include <QApplication>
#include "MainWindow.h"
#include <QLoggingCategory>

int main(int argc, char *argv[])
{
    // --- НАСТРОЙКА ЛОГИРОВАНИЯ ---

    // Устанавливаем правило:
    // 1. "*.debug=false" - ВЫКЛЮЧАЕМ все отладочные сообщения по умолчанию.
    // 2. "\n" - разделитель правил.
    // 3. "default.debug=true" - ВКЛЮЧАЕМ отладочные сообщения только
    //    для дефолтной категории, которую использует qDebug().
    QLoggingCategory::setFilterRules("*.debug=false\ndefault.debug=true");

    // Формат вывода оставляем прежним, он удобен.
    qSetMessagePattern("%{type}: %{message}");

    // --- КОНЕЦ НАСТРОЙКИ ---

    QApplication app(argc, argv);

    MainWindow window;
    window.show();

    return app.exec();
}