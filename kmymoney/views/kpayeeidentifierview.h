/*
 * This file is part of KMyMoney, A Personal Finance Manager for KDE
 * Copyright (C) 2014 Christian Dávid <christian-david@web.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef KPAYEEIDENTIFIERVIEW_H
#define KPAYEEIDENTIFIERVIEW_H

#include <QWidget>

#include "mymoney/mymoneypayeeidentifiercontainer.h"
#include "widgets/styleditemdelegateforwarder.h"

namespace Ui
{
class KPayeeIdentifierView;
};

class KPayeeIdentifierView : public QWidget
{
  Q_OBJECT

public:
  KPayeeIdentifierView(QWidget* parent);
  ~KPayeeIdentifierView();
  QList<payeeIdentifier> identifiers() const;

signals:
  void dataChanged();

public slots:
  void setSource(MyMoneyPayeeIdentifierContainer data);

private slots:
  void removeSelected();

private:
  Ui::KPayeeIdentifierView* ui;
};

class payeeIdentifierDelegate : public StyledItemDelegateForwarder
{
  Q_OBJECT
public:
  payeeIdentifierDelegate(QObject* parent = 0);
  virtual QAbstractItemDelegate* getItemDelegate(const QModelIndex& index) const;
};

#endif // KPAYEEIDENTIFIERVIEW_H
