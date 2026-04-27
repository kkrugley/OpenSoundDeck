/* src/SupportDialog.h */
/*
* OpenSoundDeck
* Copyright (C) 2025 Pavel Kruhlei
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/

#pragma once

#include <QDialog>

class QPushButton;
class QLabel;

class SupportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SupportDialog(QWidget *parent = nullptr);
    ~SupportDialog();

private slots:
    void onOpenSupportPage();

private:
    void setupUI();

    QPushButton* m_supportButton;
    QPushButton* m_closeButton;
};
