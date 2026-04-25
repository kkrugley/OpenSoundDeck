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
#include <QStyleFactory>
#include <QPalette>
#include <QColor>

void setupLightTheme(QApplication& app)
{
    // Set Fusion style for consistent look across platforms
    app.setStyle(QStyleFactory::create("Fusion"));

    // Define light theme colors
    QPalette lightPalette;

    // Background colors
    lightPalette.setColor(QPalette::Window, QColor(245, 245, 245));       // Main background
    lightPalette.setColor(QPalette::WindowText, QColor(33, 33, 33));      // Text color
    lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));        // Input fields background
    lightPalette.setColor(QPalette::AlternateBase, QColor(245, 245, 245)); // Alternate row color
    lightPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));  // Tooltip background
    lightPalette.setColor(QPalette::ToolTipText, QColor(33, 33, 33));     // Tooltip text

    // Text colors
    lightPalette.setColor(QPalette::Text, QColor(33, 33, 33));           // Main text
    lightPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(150, 150, 150)); // Disabled text
    lightPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(150, 150, 150));

    // Button colors
    lightPalette.setColor(QPalette::Button, QColor(230, 230, 230));      // Button background
    lightPalette.setColor(QPalette::ButtonText, QColor(33, 33, 33));     // Button text
    lightPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(150, 150, 150));

    // Highlight colors (selection)
    lightPalette.setColor(QPalette::Highlight, QColor(0, 122, 204));     // Selection background
    lightPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255)); // Selection text

    // Link colors
    lightPalette.setColor(QPalette::Link, QColor(0, 102, 204));
    lightPalette.setColor(QPalette::LinkVisited, QColor(51, 51, 153));

    // Apply the palette
    app.setPalette(lightPalette);

    // Set application stylesheet for additional styling
    app.setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QToolBar {
            background-color: #e8e8e8;
            border: none;
            padding: 4px;
            spacing: 4px;
        }
        QToolButton {
            background-color: transparent;
            border: none;
            border-radius: 4px;
            padding: 4px;
        }
        QToolButton:hover {
            background-color: #d0d0d0;
        }
        QToolButton:pressed {
            background-color: #c0c0c0;
        }
        QToolButton:checked {
            background-color: #007acc;
        }
        QToolButton:checked:hover {
            background-color: #006bb3;
        }
        QTableWidget {
            background-color: #ffffff;
            border: 1px solid #d0d0d0;
            gridline-color: #e0e0e0;
        }
        QTableWidget::item:selected {
            background-color: #007acc;
            color: white;
        }
        QHeaderView::section {
            background-color: #e8e8e8;
            color: #333333;
            padding: 6px;
            border: none;
            border-right: 1px solid #d0d0d0;
            border-bottom: 1px solid #d0d0d0;
            font-weight: bold;
        }
        QSlider::groove:horizontal {
            border: 1px solid #999999;
            height: 4px;
            background: #e0e0e0;
            margin: 2px 0;
        }
        QSlider::handle:horizontal {
            background: #007acc;
            border: none;
            width: 16px;
            margin: -4px 0;
            border-radius: 8px;
        }
        QSlider::sub-page:horizontal {
            background: #007acc;
            border: 1px solid #007acc;
            height: 4px;
        }
        QMenuBar {
            background-color: #e8e8e8;
            border-bottom: 1px solid #d0d0d0;
        }
        QMenuBar::item:selected {
            background-color: #007acc;
            color: white;
        }
        QMenu {
            background-color: #ffffff;
            border: 1px solid #d0d0d0;
        }
        QMenu::item:selected {
            background-color: #007acc;
            color: white;
        }
        QStatusBar {
            background-color: #e8e8e8;
            border-top: 1px solid #d0d0d0;
        }
    )");
}

int main(int argc, char *argv[])
{
    qDebug() << "[1] Starting application...";

    QApplication app(argc, argv);
    qDebug() << "[2] QApplication created";

    // Apply light theme
    setupLightTheme(app);
    
    try {
        qDebug() << "[3] Creating MainWindow...";
        MainWindow window;
        qDebug() << "[4] MainWindow created, showing...";
        window.show();
        qDebug() << "[5] Window shown, starting event loop...";
        return app.exec();
    } catch (const std::exception& e) {
        qCritical() << "Fatal error:" << e.what();
        return 1;
    }
}