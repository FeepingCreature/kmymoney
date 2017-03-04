/***************************************************************************
                          kreportconfigurationdlg.h  -  description
                             -------------------
    begin                : Mon Jun 21 2004
    copyright            : (C) 2000-2004 by Michael Edwardes
    email                : mte@users.sourceforge.net
                           Javier Campos Morales <javi_c@users.sourceforge.net>
                           Felix Rodriguez <frodriguez@users.sourceforge.net>
                           John C <thetacoturtle@users.sourceforge.net>
                           Thomas Baumgart <ipwizard@users.sourceforge.net>
                           Kevin Tambascio <ktambascio@users.sourceforge.net>
                           Ace Jones <ace.j@hotpop.com>
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KREPORTCONFIGURATIONFILTERDLG_H
#define KREPORTCONFIGURATIONFILTERDLG_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QVector>
#include <QDoubleValidator>

// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes

#include "kfindtransactiondlg.h"
#include "mymoneyreport.h"
#include "mymoneybudget.h"
#include "kmymoneyreportconfigtabimpl.h"


class kMyMoneyReportConfigTab1Decl;
class kMyMoneyReportConfigTab2Decl;
class kMyMoneyReportConfigTab3Decl;
class kMyMoneyReportConfigTabChartDecl;
class kMyMoneyReportConfigTabRangeDecl;
class MyMoneyBudget;

/**
  * @author Ace Jones
  */
class KReportConfigurationFilterDlg : public KFindTransactionDlg
{
  Q_OBJECT
public:
  explicit KReportConfigurationFilterDlg(MyMoneyReport report, QWidget *parent = 0);
  ~KReportConfigurationFilterDlg();

  const MyMoneyReport& getConfig() const {
    return m_currentState;
  }

protected:
  kMyMoneyReportConfigTab1Decl* m_tab1;
  kMyMoneyReportConfigTab2Decl* m_tab2;
  kMyMoneyReportConfigTab3Decl* m_tab3;
  kMyMoneyReportConfigTabChartDecl* m_tabChart;
  kMyMoneyReportConfigTabRangeDecl* m_tabRange;

  MyMoneyReport m_initialState;
  MyMoneyReport m_currentState;

protected slots:
  void slotRowTypeChanged(int);
  void slotColumnTypeChanged(int);
  void slotReset();
  void slotSearch();
  void slotShowHelp();
  /**
   * This is to enable/disable the check to Include Transfers based on whether Categories are filtered or not.
   * This is because if Categories are filtered, transfers will not be included anyway
   */
  virtual void slotDateChanged();

  void slotUpdateCheckTransfers();
  void slotUpdateColumnsCombo();
  void slotLogAxisChanged(int state);

private:
  QVector<MyMoneyBudget> m_budgets;
};
#endif
