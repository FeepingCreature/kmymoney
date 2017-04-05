/***************************************************************************
                          kavailablecurrencydlg.h  -  description
                             -------------------
    begin                : Sat Apr 01 2017
    copyright            : (C) 2017 Łukasz Wojniłowicz <lukasz.wojnilowicz@gmail.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KAVAILABLECURRENCYEDITDLG_H
#define KAVAILABLECURRENCYEDITDLG_H

// ----------------------------------------------------------------------------
// QT Includes

// ----------------------------------------------------------------------------
// KDE Includes

#include <QDialog>
#include <ktreewidgetsearchlinewidget.h>

// ----------------------------------------------------------------------------
// Project Includes

namespace Ui
{
class KAvailableCurrencyDlg;
}

class QTreeWidgetItem;

class KAvailableCurrencyDlg : public QDialog
{
  Q_OBJECT
public:
  KAvailableCurrencyDlg(QWidget *parent = 0);
  ~KAvailableCurrencyDlg();

  Ui::KAvailableCurrencyDlg*   ui;

protected slots:
  void slotLoadCurrencies();
  void slotItemSelectionChanged();

private:
  KTreeWidgetSearchLineWidget*  m_searchWidget;
};

#endif
