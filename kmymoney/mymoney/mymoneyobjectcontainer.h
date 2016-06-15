/***************************************************************************
                          mymoneyobjectcontainer.h
                             -------------------
    copyright            : (C) 2006 by Thomas Baumgart
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

#ifndef MYMONEYOBJECTCONTAINER_H
#define MYMONEYOBJECTCONTAINER_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QObject>
#include <QString>
#include <QList>

// ----------------------------------------------------------------------------
// Project Includes

#include <kmm_mymoney_export.h>
#include <mymoneytransaction.h>
#include <mymoneyaccount.h>
#include <mymoneyinstitution.h>
#include <mymoneypayee.h>
#include <mymoneytag.h>
#include <mymoneyobject.h>
#include <mymoneysecurity.h>
#include <onlinejob.h>
#include <mymoneycostcenter.h>
#include <imymoneystorage.h>

/**
  * @author Thomas Baumgart
  */

/**
  * This class represents a generic container for all MyMoneyObject derived objects.
  */
class KMM_MYMONEY_EXPORT MyMoneyObjectContainer : public QObject
{
  Q_OBJECT
public:
  MyMoneyObjectContainer();
  ~MyMoneyObjectContainer();

  const MyMoneyAccount& account(const QString& id);
  const MyMoneyPayee& payee(const QString& id);
  const MyMoneyTag& tag(const QString& id);
  const MyMoneySecurity& security(const QString& id);
  const MyMoneyInstitution& institution(const QString& id);
  const MyMoneySchedule& schedule(const QString& id);
  const MyMoneyCostCenter costCenter(const QString& id);

  void account(QList<MyMoneyAccount>& list);
  void payee(QList<MyMoneyPayee>& list);
  void tag(QList<MyMoneyTag>& list);
  void institution(QList<MyMoneyInstitution>& list);
  void costCenter(QList<MyMoneyCostCenter>& list);

  void preloadAccount(const QList<MyMoneyAccount>& list);
  void preloadPayee(const QList<MyMoneyPayee>& list);
  void preloadTag(const QList<MyMoneyTag>& list);
  void preloadInstitution(const QList<MyMoneyInstitution>& list);
  void preloadSecurity(const QList<MyMoneySecurity>& list);
  void preloadSchedule(const QList<MyMoneySchedule>& list);
  void preloadOnlineJob(const QList<onlineJob>& list);
  void preloadCostCenter(const QList<MyMoneyCostCenter>& list);

  void preloadAccount(const MyMoneyAccount& account);
  void preloadSecurity(const MyMoneySecurity& security);
  void preloadPayee(const MyMoneyPayee& payee);
  void preloadTag(const MyMoneyTag& tag);
  void preloadInstitution(const MyMoneyInstitution& institution);
  void preloadSchedule(const MyMoneySchedule& schedule);
  void preloadOnlineJob(const onlineJob& job);

  void clear(const QString& id);
  void clear(IMyMoneyStorage* storage = 0);

  const MyMoneyAccount& accountByName(const QString& name) const;

  const MyMoneyObject * object(const QString& id) const;

  /**
   * This method refreshes an already existing object in the container
   * with a copy from the engine. The object is identified by its @a id.
   * If the object is unknown or the @a id is empty, nothing is done.
   */
  void refresh(const QString& id);

private:
  struct Private;
  Private * const d;
};

#endif


