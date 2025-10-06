/* src/SettingsDialog.cpp*/
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

#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QStandardPaths>
#include <QSettings>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Settings"));
    setMinimumSize(500, 300);

    m_tabWidget = new QTabWidget(this);
    m_tabWidget->addTab(createGeneralTab(), tr("General"));
    m_tabWidget->addTab(createAudioTab(),   tr("Audio"));
    m_tabWidget->addTab(createHotkeysTab(), tr("Hotkeys"));
    m_tabWidget->addTab(createInterfaceTab(), tr("Interface"));
    m_tabWidget->addTab(createDevicesTab(), tr("Devices"));

    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Apply);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &SettingsDialog::onAccepted);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(m_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &SettingsDialog::saveSettings);


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(m_tabWidget);
    mainLayout->addWidget(m_buttonBox);

    loadSettings();
}

void SettingsDialog::loadSettings()
{
    QSettings settings("pavel-kruhlei", "OpenSoundDeck");
    QString defaultPath = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
    QString libraryPath = settings.value("library/path", defaultPath).toString();
    m_libraryPathLineEdit->setText(libraryPath);
}

void SettingsDialog::saveSettings()
{
    QSettings settings("pavel-kruhlei", "OpenSoundDeck");
    settings.setValue("library/path", m_libraryPathLineEdit->text());
    qDebug() << "Settings saved. Library path:" << m_libraryPathLineEdit->text();
}

void SettingsDialog::onAccepted()
{
    saveSettings();
    accept();
}

void SettingsDialog::onBrowseLibraryPath()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select Library Folder"),
                                                    m_libraryPathLineEdit->text(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) {
        m_libraryPathLineEdit->setText(dir);
    }
}

QWidget* SettingsDialog::createGeneralTab()
{
    QWidget *generalWidget = new QWidget;
    QFormLayout *layout = new QFormLayout(generalWidget);

    m_libraryPathLineEdit = new QLineEdit;
    QPushButton *browseButton = new QPushButton(tr("Browse..."));
    connect(browseButton, &QPushButton::clicked, this, &SettingsDialog::onBrowseLibraryPath);

    QHBoxLayout *pathLayout = new QHBoxLayout;
    pathLayout->addWidget(m_libraryPathLineEdit);
    pathLayout->addWidget(browseButton);

    layout->addRow(tr("Sound Library Path:"), pathLayout);

    return generalWidget;
}

// --- Placeholder Tabs ---
QWidget* SettingsDialog::createAudioTab()     { return new QLabel(tr("Audio settings will be here.")); }
QWidget* SettingsDialog::createHotkeysTab()   { return new QLabel(tr("Hotkey settings will be here.")); }
QWidget* SettingsDialog::createInterfaceTab() { return new QLabel(tr("Interface settings will be here.")); }
QWidget* SettingsDialog::createDevicesTab()   { return new QLabel(tr("Device settings will be here.")); }
