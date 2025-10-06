/*src/main.cpp*/

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