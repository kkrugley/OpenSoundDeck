/* src/HelpDialog.cpp */
/*
* OpenSoundDeck
* Copyright (C) 2025 Pavel Kruhlei
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*/

#include "HelpDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QDesktopServices>
#include <QUrl>

HelpDialog::HelpDialog(QWidget *parent)
: QDialog(parent)
{
    setWindowTitle(tr("Get Help"));
    setMinimumSize(400, 280);
    resize(450, 320);
    setupUI();
}

HelpDialog::~HelpDialog() = default;

void HelpDialog::setupUI()
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // Title
    auto *titleLabel = new QLabel(tr("Need Help or Have Feedback?"), this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    mainLayout->addSpacing(15);

    // Help sections
    auto *sectionsLabel = new QLabel(
        tr("<b>Quick Help:</b>"),
        this);
    mainLayout->addWidget(sectionsLabel);

    auto *helpText = new QLabel(
        tr("<ul>"
           "<li><b>Import sounds:</b> Use File → Import Audio Files or drag and drop</li>"
           "<li><b>Assign hotkeys:</b> Right-click a sound and select 'Assign Hotkey'</li>"
           "<li><b>Virtual microphone:</b> Configure in Settings → Audio</li>"
           "</ul>"),
        this);
    helpText->setWordWrap(true);
    mainLayout->addWidget(helpText);

    auto *feedbackLabel = new QLabel(
        tr("<b>Need more help or want to suggest a feature?</b>"),
        this);
    mainLayout->addWidget(feedbackLabel);

    auto *feedbackText = new QLabel(
        tr("Visit our help center for documentation, FAQs, and to contact the team."),
        this);
    feedbackText->setWordWrap(true);
    mainLayout->addWidget(feedbackText);

    mainLayout->addStretch();

    // Button box with native styling
    auto *buttonBox = new QDialogButtonBox(this);
    m_helpButton = new QPushButton(tr("Open Help Center"), this);
    m_closeButton = new QPushButton(tr("Close"), this);

    buttonBox->addButton(m_helpButton, QDialogButtonBox::ActionRole);
    buttonBox->addButton(m_closeButton, QDialogButtonBox::RejectRole);

    connect(m_helpButton, &QPushButton::clicked,
            this, &HelpDialog::onOpenHelpPage);
    connect(m_closeButton, &QPushButton::clicked,
            this, &QDialog::reject);

    mainLayout->addWidget(buttonBox);
}

void HelpDialog::onOpenHelpPage()
{
    QDesktopServices::openUrl(QUrl("https://opensounddeck.vercel.app/help"));
}
