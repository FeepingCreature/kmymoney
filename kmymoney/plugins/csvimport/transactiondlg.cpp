/*******************************************************************************
*                               transactiondlg.cpp
*                             ------------------
* begin                       : Sat Jan 01 2010
* copyright                   : (C) 2010 by Allan Anderson
* email                       : agander93@gmail.com
* * copyright                 : (C) 2016 by Łukasz Wojniłowicz
* email                       : lukasz.wojnilowicz@gmail.com
********************************************************************************/

/*******************************************************************************
*                                                                              *
*   This program is free software; you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License as published by       *
*   the Free Software Foundation; either version 2 of the License, or          *
*   (at your option) any later version.                                        *
*                                                                              *
********************************************************************************/

#include "transactiondlg.h"

// ----------------------------------------------------------------------------
// QT Headers

#include <QDesktopWidget>

// ----------------------------------------------------------------------------
// KDE Headers

#include <KLocalizedString>
#include <KColorScheme>

TransactionDlg::TransactionDlg(const QStringList& colList, const QStringList& colHeaders, const qint8& typeCol,
                         const QList<MyMoneyStatement::Transaction::EAction>& validActionTypes) : ui(new Ui::TransactionDlg)
{
  ui->setupUi(this);

  m_colorBrush = KColorScheme(QPalette::Normal).background(KColorScheme::PositiveBackground);
  m_colorBrushText = KColorScheme(QPalette::Normal).foreground(KColorScheme::PositiveText);
  m_errorBrush = KColorScheme(QPalette::Normal).background(KColorScheme::NegativeBackground);
  m_errorBrushText = KColorScheme(QPalette::Normal).foreground(KColorScheme::NegativeText);

  // initialize action types list according to cbActionTypes
  m_actionTypes << MyMoneyStatement::Transaction::eaBuy <<
                 MyMoneyStatement::Transaction::eaSell <<
                 MyMoneyStatement::Transaction::eaCashDividend <<
                 MyMoneyStatement::Transaction::eaReinvestDividend <<
                 MyMoneyStatement::Transaction::eaShrsin <<
                 MyMoneyStatement::Transaction::eaShrsout <<
                 MyMoneyStatement::Transaction::eaInterest;

  QIcon icon = QIcon::fromTheme(QStringLiteral("dialog-ok"), QIcon::fromTheme(QStringLiteral("finish")));
  if (!icon.availableSizes().isEmpty())
    m_iconYes = icon.pixmap(icon.availableSizes().first());
  else
    m_iconYes = QPixmap();

  icon = QIcon::fromTheme(QStringLiteral("dialog-cancel"), QIcon::fromTheme(QStringLiteral("stop")));
  if (!icon.availableSizes().isEmpty())
    m_iconNo = icon.pixmap(icon.availableSizes().first());
  else
    m_iconNo = QPixmap();

  m_buttonOK = ui->buttonBox->button(QDialogButtonBox::Ok);
  m_buttonCancel = ui->buttonBox->button(QDialogButtonBox::Cancel);

  m_buttonOK->setEnabled(false);
  ui->cbActionTypes->setCurrentIndex(-1);

  connect(ui->cbActionTypes, SIGNAL(currentIndexChanged(int)), this, SLOT(slotActionSelected(int)));
  m_typeColumn = typeCol;
  m_validActionTypes = validActionTypes;
  displayLine(colList, colHeaders, typeCol);
  iconifyActionTypesComboBox(validActionTypes);
  updateWindowSize();
}

TransactionDlg::~TransactionDlg()
{
  delete ui;
}

void TransactionDlg::updateWindowSize()
{
  int columnsWidth = 0;
  int headerWidth;
  int newWidth;
  QRect screen = QApplication::desktop()->availableGeometry();

  layout()->invalidate();
  layout()->activate();

  columnsWidth += ui->tableWidget->verticalHeader()->width();
  for (int col = 0; col < ui->tableWidget->columnCount(); ++col)
    columnsWidth += ui->tableWidget->columnWidth(col);
  headerWidth = ui->tableWidget->width() -
                ui->tableWidget->contentsMargins().left() -
                ui->tableWidget->contentsMargins().right();

  newWidth = columnsWidth - headerWidth + frameGeometry().width();
  if (newWidth > screen.width())
    newWidth = screen.width();
  newWidth -= frameGeometry().width() - geometry().width();
  QRect dlg = geometry();
  dlg.setWidth(newWidth);
  dlg.moveTo((screen.width() - dlg.width()) / 2,
             (screen.height() - dlg.height()) / 2);
  setGeometry(dlg);
}


void TransactionDlg::displayLine(const QStringList& colList, const QStringList& colHeaders, const qint8& typeCol)
{
  qint8 colCount = colList.count();
  ui->tableWidget->setColumnCount(colCount);
  ui->tableWidget->setHorizontalHeaderLabels(colHeaders);
  for (int col = 0; col < colCount; ++col) {
    QTableWidgetItem *item = new QTableWidgetItem;
    item->setText(colList.value(col));
    if (col == typeCol) {
      item->setBackground(m_errorBrush);
      item->setForeground(m_errorBrushText);
    }
    ui->tableWidget->setItem(0, col, item);
  }
  ui->tableWidget->resizeColumnsToContents();
}

void TransactionDlg::slotActionSelected(int index)
{
  QTableWidgetItem *item = ui->tableWidget->item(0, m_typeColumn);
  if (m_validActionTypes.contains(m_actionTypes.value(index))) {
    item->setBackground(m_colorBrush);
    item->setForeground(m_colorBrushText);
    m_buttonOK->setEnabled(true);
  } else {
    item->setBackground(m_errorBrush);
    item->setForeground(m_errorBrushText);
    m_buttonOK->setEnabled(false);
  }
}

MyMoneyStatement::Transaction::EAction TransactionDlg::getActionType()
{
  return m_actionTypes.value(ui->cbActionTypes->currentIndex());
}

void TransactionDlg::iconifyActionTypesComboBox(const QList<MyMoneyStatement::Transaction::EAction>& validActionTypes)
{
  for (int i = 0; i < m_actionTypes.count(); ++i) {
    if (validActionTypes.contains(m_actionTypes[i]))
      ui->cbActionTypes->setItemIcon(i, m_iconYes);
    else
      ui->cbActionTypes->setItemIcon(i, m_iconNo);
  }
}
