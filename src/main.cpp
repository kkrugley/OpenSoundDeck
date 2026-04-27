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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "MainWindow.h"
#include "Settings.h"

#include <QApplication>
#include <QLoggingCategory>
#include <QStyleFactory>
#include <QPalette>
#include <QColor>
#include <QProcess>

void setupLightTheme(QApplication& app);
void setupDarkTheme(QApplication& app);

void setupTheme(QApplication& app, ThemeMode theme)
{
    if (theme == ThemeMode::System) {
        app.setStyle(QStyleFactory::create("Fusion"));
        return;
    }

    if (theme == ThemeMode::Dark) {
        setupDarkTheme(app);
        return;
    }

    setupLightTheme(app);
}

void setupLightTheme(QApplication& app)
{
    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette lightPalette;
    lightPalette.setColor(QPalette::Window, QColor(245, 245, 245));
    lightPalette.setColor(QPalette::WindowText, QColor(33, 33, 33));
    lightPalette.setColor(QPalette::Base, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::AlternateBase, QColor(245, 245, 245));
    lightPalette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::ToolTipText, QColor(33, 33, 33));
    lightPalette.setColor(QPalette::Text, QColor(33, 33, 33));
    lightPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(150, 150, 150));
    lightPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(150, 150, 150));
    lightPalette.setColor(QPalette::Button, QColor(230, 230, 230));
    lightPalette.setColor(QPalette::ButtonText, QColor(33, 33, 33));
    lightPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(150, 150, 150));
    lightPalette.setColor(QPalette::Highlight, QColor(0, 122, 204));
    lightPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    lightPalette.setColor(QPalette::Link, QColor(0, 102, 204));
    lightPalette.setColor(QPalette::LinkVisited, QColor(51, 51, 153));

    app.setPalette(lightPalette);

    app.setStyleSheet(
        "QMainWindow { background-color: #f5f5f5; }\n"
        "QToolBar { background-color: #e8e8e8; border: none; padding: 4px; spacing: 4px; }\n"
        "QToolButton { background-color: transparent; border: none; border-radius: 4px; padding: 4px; }\n"
        "QToolButton:hover { background-color: #d0d0d0; }\n"
        "QToolButton:pressed { background-color: #c0c0c0; }\n"
        "QToolButton:checked { background-color: #007acc; }\n"
        "QToolButton:checked:hover { background-color: #006bb3; }\n"
        "QTableWidget { background-color: #ffffff; border: 1px solid #d0d0d0; gridline-color: #e0e0e0; }\n"
        "QTableWidget::item:selected { background-color: #007acc; color: white; }\n"
        "QHeaderView::section { background-color: #e8e8e8; color: #333333; padding: 6px; border: none; border-right: 1px solid #d0d0d0; border-bottom: 1px solid #d0d0d0; font-weight: bold; }\n"
        "QSlider::groove:horizontal { border: 1px solid #999999; height: 4px; background: #e0e0e0; margin: 2px 0; }\n"
        "QSlider::handle:horizontal { background: #007acc; border: none; width: 16px; margin: -4px 0; border-radius: 8px; }\n"
        "QSlider::sub-page:horizontal { background: #007acc; border: 1px solid #007acc; height: 4px; }\n"
        "QMenuBar { background-color: #e8e8e8; border-bottom: 1px solid #d0d0d0; }\n"
        "QMenuBar::item:selected { background-color: #007acc; color: white; }\n"
        "QMenu { background-color: #ffffff; border: 1px solid #d0d0d0; }\n"
        "QMenu::item:selected { background-color: #007acc; color: white; }\n"
        "QStatusBar { background-color: #e8e8e8; border-top: 1px solid #d0d0d0; }"
    );
}

void setupDarkTheme(QApplication& app)
{
    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::WindowText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Base, QColor(30, 30, 30));
    darkPalette.setColor(QPalette::AlternateBase, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::ToolTipBase, QColor(45, 45, 48));
    darkPalette.setColor(QPalette::ToolTipText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Text, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(120, 120, 120));
    darkPalette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(120, 120, 120));
    darkPalette.setColor(QPalette::Button, QColor(60, 60, 64));
    darkPalette.setColor(QPalette::ButtonText, QColor(240, 240, 240));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(120, 120, 120));
    darkPalette.setColor(QPalette::Highlight, QColor(0, 122, 204));
    darkPalette.setColor(QPalette::HighlightedText, QColor(255, 255, 255));
    darkPalette.setColor(QPalette::Link, QColor(100, 180, 255));
    darkPalette.setColor(QPalette::LinkVisited, QColor(150, 150, 255));

    app.setPalette(darkPalette);

    app.setStyleSheet(
        "QMainWindow { background-color: #2d2d30; }\n"
        "QToolBar { background-color: #3c3c40; border: none; padding: 4px; spacing: 4px; }\n"
        "QToolButton { background-color: transparent; border: none; border-radius: 4px; padding: 4px; }\n"
        "QToolButton:hover { background-color: #505055; }\n"
        "QToolButton:pressed { background-color: #606065; }\n"
        "QToolButton:checked { background-color: #007acc; }\n"
        "QToolButton:checked:hover { background-color: #006bb3; }\n"
        "QTableWidget { background-color: #1e1e1e; border: 1px solid #505055; gridline-color: #404040; color: #f0f0f0; }\n"
        "QTableWidget::item:selected { background-color: #007acc; color: white; }\n"
        "QHeaderView::section { background-color: #3c3c40; color: #f0f0f0; padding: 6px; border: none; border-right: 1px solid #505055; border-bottom: 1px solid #505055; font-weight: bold; }\n"
        "QSlider::groove:horizontal { border: 1px solid #666; height: 4px; background: #404040; margin: 2px 0; }\n"
        "QSlider::handle:horizontal { background: #007acc; border: none; width: 16px; margin: -4px 0; border-radius: 8px; }\n"
        "QSlider::sub-page:horizontal { background: #007acc; border: 1px solid #007acc; height: 4px; }\n"
        "QMenuBar { background-color: #3c3c40; border-bottom: 1px solid #505055; }\n"
        "QMenuBar::item:selected { background-color: #007acc; color: white; }\n"
        "QMenu { background-color: #2d2d30; border: 1px solid #505055; }\n"
        "QMenu::item:selected { background-color: #007acc; color: white; }\n"
        "QStatusBar { background-color: #3c3c40; border-top: 1px solid #505055; }\n"
        "QGroupBox { color: #f0f0f0; border: 1px solid #505055; }\n"
        "QLabel { color: #f0f0f0; }\n"
        "QLineEdit { background-color: #1e1e1e; color: #f0f0f0; border: 1px solid #505055; }\n"
        "QComboBox { background-color: #3c3c40; color: #f0f0f0; border: 1px solid #505055; }\n"
        "QSpinBox { background-color: #1e1e1e; color: #f0f0f0; border: 1px solid #505055; }\n"
        "QCheckBox { color: #f0f0f0; }\n"
        "QTabWidget::pane { border: 1px solid #505055; }\n"
        "QTabBar::tab { background-color: #3c3c40; color: #f0f0f0; border: 1px solid #505055; padding: 8px; }\n"
        "QTabBar::tab:selected { background-color: #007acc; }\n"
        "QDialog { background-color: #2d2d30; }"
    );
}

int main(int argc, char *argv[])
{
    qDebug() << "[1] Starting application...";

    QApplication app(argc, argv);
    qDebug() << "[2] QApplication created";

    Settings* settings = Settings::instance();
    qDebug() << "[3] Settings initialized";

    setupTheme(app, settings->theme());
    qDebug() << "[4] Theme applied:" << static_cast<int>(settings->theme());

    try {
        qDebug() << "[5] Creating MainWindow...";
        MainWindow window;
        qDebug() << "[6] MainWindow created, showing...";

        if (settings->startMinimizedToTray()) {
            qDebug() << "Starting minimized to tray (not yet implemented)";
            window.show();
        } else {
            window.show();
        }

    qDebug() << "[7] Window shown, starting event loop...";
    int result = app.exec();

    // Check for restart request
    if (result == 100) {
        // Restart requested
        QProcess::startDetached(qApp->arguments()[0], qApp->arguments().mid(1));
        result = 0;
    }

    Settings::destroyInstance();
    return result;
    } catch (const std::exception& e) {
        qCritical() << "Fatal error:" << e.what();
        Settings::destroyInstance();
        return 1;
    }
}
