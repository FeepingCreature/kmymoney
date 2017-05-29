/***************************************************************************
                             ksettingsgeneral.cpp
                             --------------------
    copyright            : (C) 2005 by Thomas Baumgart
    email                : ipwizard@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ksettingsgeneral.h"

// ----------------------------------------------------------------------------
// QT Includes

#include <QButtonGroup>
#include <QFileDialog>
#include <QLabel>

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include <kmymoneydateinput.h>

KSettingsGeneral::KSettingsGeneral(QWidget* parent) :
    KSettingsGeneralDecl(parent)
{
  // hide the internally used date field
  kcfg_StartDate->hide();

  // setup connections, so that the sort optios get loaded once the edit fields are filled
  connect(kcfg_StartDate, SIGNAL(dateChanged(QDate)), this, SLOT(slotLoadStartDate(QDate)));

  // setup connections, so that changes by the user are forwarded to the (hidden) edit fields
  connect(m_startDateEdit, SIGNAL(dateChanged(QDate)), kcfg_StartDate, SLOT(setDate(QDate)));

  connect(choosePath, SIGNAL(pressed()), this, SLOT(slotChooseLogPath()));
}

KSettingsGeneral::~KSettingsGeneral()
{
}

void KSettingsGeneral::slotChooseLogPath()
{
    QString filePath = QFileDialog::getExistingDirectory(this, i18n("Choose file path"), QDir::homePath());
    kcfg_logPath->setText(filePath);
    slotUpdateLogTypes();
}

void KSettingsGeneral::slotLoadStartDate(const QDate&)
{
  // only need this once
  disconnect(kcfg_StartDate, SIGNAL(dateChanged(QDate)), this, SLOT(slotLoadStartDate(QDate)));
  m_startDateEdit->setDate(kcfg_StartDate->date());
}

void KSettingsGeneral::slotUpdateLogTypes()
{
    bool enable = kcfg_logPath->text().isEmpty() ? false : true;
    kcfg_logImportedStatements->setEnabled(enable);
    kcfg_logOfxTransactions->setEnabled(enable);
    if (!enable)
    {
        kcfg_logImportedStatements->setChecked(enable);
        kcfg_logOfxTransactions->setChecked(enable);
    }
}

void KSettingsGeneral::showEvent(QShowEvent *event)
{
    KSettingsGeneralDecl::showEvent(event);
    slotUpdateLogTypes();
}
