/***************************************************************************
                             kaccountssview.h
                             -------------------
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

#ifndef KACCOUNTSVIEW_H
#define KACCOUNTSVIEW_H

// ----------------------------------------------------------------------------
// QT Includes

// ----------------------------------------------------------------------------
// KDE Includes

#include <KListWidget>

// ----------------------------------------------------------------------------
// Project Includes

#include <mymoneyaccount.h>
#include <mymoneyutils.h>
#include "accountsmodel.h"

#include "ui_kaccountsviewdecl.h"

class KMyMoneyAccountTreeView;

/**
  * This class implements the accounts hierarchical and iconic 'view'.
  */

class KAccountsViewDecl : public QWidget, public Ui::KAccountsViewDecl
{
public:
  KAccountsViewDecl(QWidget *parent) : QWidget(parent) {
    setupUi(this);
  }

};
class KAccountsView : public KAccountsViewDecl
{
  Q_OBJECT
private:

public:
  KAccountsView(QWidget *parent = 0);
  virtual ~KAccountsView();

public slots:
  void slotLoadAccounts(void);

  /**
    * Override the base class behaviour to include all updates that
    * happened in the meantime and restore the layout.
    */
  void showEvent(QShowEvent * event);

  /**
    * update the account objects if their icon position has changed since
    * the last time.
    *
    * @param action must be KMyMoneyView::preSave, otherwise this slot is a NOP.
    */
  //void slotUpdateIconPos(unsigned int action);

  //void slotReconcileAccount(const MyMoneyAccount& acc, const QDate& reconciliationDate, const MyMoneyMoney& endingBalance);

protected:
  typedef enum {
    ListView = 0,
    IconView,
    // insert new values above this line
    MaxViewTabs
  } AccountsViewTab;

  /**
    * This method loads the accounts for the respective tab.
    *
    * @param tab which tab should be loaded
    */
  void loadAccounts(AccountsViewTab tab);
  void loadListView(void);
  //void loadIconView(void);
  void loadIconGroups(void);

  /**
    * This method loads all the subaccounts recursively of a given root account
    *
    */
  void loadAccountIconsIntoList(const MyMoneyAccount& parentAccount, KListWidget* listWidget);

  /**
    * This method returns a pointer to the currently selected
    * account icon or 0 if no icon is selected.
    */
  QListWidgetItem* selectedIcon(void) const;

  QPoint point(const QString& str) const;
  QString point(const QPoint& val) const;

protected slots:
  void slotNetWorthChanged(const MyMoneyMoney &);
  void slotTabCurrentChanged(QWidget*);
  void slotSelectIcon(QListWidgetItem* item);
  void slotOpenContextMenu(MyMoneyAccount account);
  void slotAssetsSelectIcon(void);
  void slotAssetsOpenContextMenu(const QPoint& point);
  void slotLiabilitiesSelectIcon(void);
  void slotLiabilitiesOpenContextMenu(const QPoint& point);
  void slotEquitiesSelectIcon(void);
  void slotEquitiesOpenContextMenu(const QPoint& point);
  void slotOpenObject(QListWidgetItem* item);
  void slotExpandCollapse(void);
  void slotUnusedIncomeExpenseAccountHidden(void);

signals:
  /**
    * This signal serves as proxy for KMyMoneyAccountTree::selectObject()
    *
    * @param obj const reference to object
    */
  void selectObject(const MyMoneyObject& obj);

  /**
    * This signal serves as proxy for
    * KMyMoneyAccountTree::openContextMenu(const MyMoneyObject&)
    *
    * @param obj const reference to object
    */
  void openContextMenu(const MyMoneyObject& obj);

  /**
    * This signal will be emitted when the left mouse button is double
    * clicked (actually the KDE executed setting is used) on an object.
    *
    * @param obj const reference to object
    */
  void openObject(const MyMoneyObject& obj);

  /**
    * This signal is emitted, when the user selected to reparent the
    * account @p acc to be a subordinate account of @p parent.
    *
    * @param acc const reference to account to be reparented
    * @param parent const reference to new parent account
    */
  void reparent(const MyMoneyAccount& acc, const MyMoneyAccount& parent);

private:
  MyMoneyAccount                      m_reconciliationAccount;
  QMap<QString, MyMoneySecurity>      m_securityMap;
  QMap<QString, unsigned long>        m_transactionCountMap;

  /// set if a view needs to be reloaded during show()
  bool                                m_needReload[MaxViewTabs];
  bool                                m_haveUnusedCategories;

  AccountsFilterProxyModel            *m_filterProxyModel;
};

#endif
