/***************************************************************************
                         kmymoneyaccountcombo  -  description
                            -------------------
   begin                : Mon May 31 2004
   copyright            : (C) 2000-2004 by Michael Edwardes
   email                : mte@users.sourceforge.net
                          Javier Campos Morales <javi_c@users.sourceforge.net>
                          Felix Rodriguez <frodriguez@users.sourceforge.net>
                          John C <thetacoturtle@users.sourceforge.net>
                          Thomas Baumgart <ipwizard@users.sourceforge.net>
                          Kevin Tambascio <ktambascio@users.sourceforge.net>
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KMYMONEYACCOUNTCOMBO_H
#define KMYMONEYACCOUNTCOMBO_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QMouseEvent>
#include <QKeyEvent>
#include <QList>
#include <QItemSelection>
#include <QPushButton>

// ----------------------------------------------------------------------------
// KDE Includes

#include <kcombobox.h>

// ----------------------------------------------------------------------------
// Project Includes

#include "kmymoneyutils.h"
#include "accountsmodel.h"
#include "models/onlinebankingaccountsfilterproxymodel.h"


/**
  * A proxy model used to filter all the data from the core accounts model leaving
  * only the name of the accounts so this model can be used in the account
  * completion combo.
  *
  * It shows only the first column (account name) and makes top level items non-selectable.
  *
  * @see AccountsModel
  * @see AccountsFilterProxyModel
  *
  * @author Cristian Onet 2010
  * @author Christian David
  */
template <class baseProxyModel>
class AccountNamesFilterProxyModelTpl : public baseProxyModel
{
public:
  AccountNamesFilterProxyModelTpl(QObject *parent = 0);

  virtual Qt::ItemFlags flags(const QModelIndex &index) const;

protected:
  bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
};

/**
 * @brief "typedef" for AccountNamesFilterProxyModelTpl<AccountsFilterProxyModel>
 *
 * To create valid Qt moc data this class inherits the template and uses Q_OBJECT.
 *
 * @code typedef AccountNamesFilterProxyModelTpl<AccountsFilterProxyModel> AccountNamesFilterProxyModel;
 * should work as well
 */
class AccountNamesFilterProxyModel : public AccountNamesFilterProxyModelTpl<AccountsFilterProxyModel>
{
  Q_OBJECT
public:
  AccountNamesFilterProxyModel(QObject* parent = 0)
      : AccountNamesFilterProxyModelTpl< AccountsFilterProxyModel >(parent) {}
};

/**
 * @brief OnlineBankingAccountFilterProxyModel showing only the name column
 *
 * Is equivalent to AccountNamesFilterProxyModel using OnlineBankingAccountFilterProxyModel as base.
 */
typedef AccountNamesFilterProxyModelTpl<OnlineBankingAccountsFilterProxyModel> OnlineBankingAccountNamesFilterProxyModel;


/**
  *
  *
  * @author Cristian Onet
  */
class KMyMoneyAccountCombo : public KComboBox
{
  Q_OBJECT
public:
  explicit KMyMoneyAccountCombo(QSortFilterProxyModel *model, QWidget *parent = 0);
  explicit KMyMoneyAccountCombo(QWidget *parent = 0);
  ~KMyMoneyAccountCombo();

  void setSelected(const QString& id);
  const QString& getSelected() const;

  void setModel(QSortFilterProxyModel *model);

  /**
   * Overridden to get specific behavior
   */
  void setEditable(bool isEditable);

  virtual bool eventFilter(QObject* o, QEvent* e);

public slots:
  void expandAll();
  void collapseAll();
  virtual void showPopup();
  virtual void hidePopup();

protected:
  virtual void wheelEvent(QWheelEvent *ev);

protected slots:
  void activated();
  void makeCompletion(const QString& txt);
  void selectItem(const QModelIndex& index);

signals:
  void accountSelected(const QString&);

private:
  class Private;
  QScopedPointer<Private> const d;
};


template <class baseProxyModel>
AccountNamesFilterProxyModelTpl<baseProxyModel>::AccountNamesFilterProxyModelTpl(QObject *parent)
    : baseProxyModel(parent)
{
}

/**
 * Top items are not selectable because they are not real accounts but are only used for grouping.
 */
template <class baseProxyModel>
Qt::ItemFlags AccountNamesFilterProxyModelTpl<baseProxyModel>::flags(const QModelIndex &index) const
{
  if (!index.parent().isValid())
    return baseProxyModel::flags(index) & ~Qt::ItemIsSelectable;
  return baseProxyModel::flags(index);
}

/**
 * Filter all but the first column.
 */
template <class baseProxyModel>
bool AccountNamesFilterProxyModelTpl<baseProxyModel>::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
  Q_UNUSED(source_parent)
  if (source_column == 0)
    return true;
  return false;
}
#endif
