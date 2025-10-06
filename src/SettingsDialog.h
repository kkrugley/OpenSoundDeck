/* src/SettingsDialog.h*/
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

#pragma once

#include <QDialog>

class QTabWidget;
class QLineEdit;
class QPushButton;
class QDialogButtonBox;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

private slots:
    void onBrowseLibraryPath();
    void onAccepted();

private:
    void loadSettings();
    void saveSettings();

    QWidget* createGeneralTab();
    QWidget* createAudioTab();
    QWidget* createHotkeysTab();
    QWidget* createInterfaceTab();
    QWidget* createDevicesTab();

    QTabWidget* m_tabWidget;
    QDialogButtonBox* m_buttonBox;

    // General Tab widgets
    QLineEdit* m_libraryPathLineEdit;
};
