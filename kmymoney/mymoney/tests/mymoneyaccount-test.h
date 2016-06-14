/***************************************************************************
                          mymoneyaccounttest.h
                          -------------------
    copyright            : (C) 2002 by Thomas Baumgart
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

#ifndef MYMONEYACCOUNTTEST_H
#define MYMONEYACCOUNTTEST_H

#include <QtCore/QObject>

class MyMoneyAccount;

class MyMoneyAccountTest : public QObject
{
  Q_OBJECT

protected:
  MyMoneyAccount *m;

private slots:
  void init();
  void cleanup();
  void testEmptyConstructor();
  void testConstructor();
  void testSetFunctions();
  void testCopyConstructor();
  void testAssignmentConstructor();
  void testSubAccounts();
  void testEquality();
  void testWriteXML();
  void testReadXML();
  void testHasReferenceTo();
  void testAdjustBalance();
  void testSetClosed();
  void testIsIncomeExpense();
  void testIsAssetLiability();
  void testIsLoan();
  void addReconciliation();
  void reconciliationHistory();
};

#endif
