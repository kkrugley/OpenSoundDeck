/* src/SupportDialog.cpp */
/*
* OpenSoundDeck
* Copyright (C) 2025 Pavel Kruhlei
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/

#include "SupportDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDesktopServices>
#include <QUrl>

SupportDialog::SupportDialog(QWidget *parent)
: QDialog(parent)
{
    setWindowTitle(tr("Support OpenSoundDeck"));
    setMinimumSize(400, 250);
    resize(450, 280);
    setupUI();
}

SupportDialog::~SupportDialog() = default;

void SupportDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    auto *titleLabel = new QLabel(tr("Support the Project"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(10);

    // Description
    auto *descLabel = new QLabel(
        tr("OpenSoundDeck is a free and open-source soundboard application. "
           "Your support helps us continue development and add new features."),
        this);
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(descLabel);

    mainLayout->addStretch();

    // Button box with native styling
    auto *buttonBox = new QDialogButtonBox(this);
    m_supportButton = new QPushButton(tr("Support Project"), this);
    m_closeButton = new QPushButton(tr("Close"), this);

    buttonBox->addButton(m_supportButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_closeButton, QDialogButtonBox::RejectRole);

    connect(m_supportButton, &QPushButton::clicked,
            this, &SupportDialog::onOpenSupportPage);
    connect(m_closeButton, &QPushButton::clicked,
            this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
}

void SupportDialog::onOpenSupportPage()
{
    QDesktopServices::openUrl(QUrl("https://opensounddeck.vercel.app/support"));
}
