/***************************************************************************
                          kbalanceaxis.cpp  -  description
                             -------------------
    begin                : Sun Jul 18 2010
    copyright            : (C) 2010 by Alvaro Soliverez
    email                : asoliverez@kde.org

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "kbalanceaxis.h"

// ----------------------------------------------------------------------------
// QT Includes


// ----------------------------------------------------------------------------
// KDE Includes

// ----------------------------------------------------------------------------
// Project Includes


KBalanceAxis::KBalanceAxis()
    : KChart::CartesianAxis()
{
}

KBalanceAxis::KBalanceAxis(KChart::AbstractCartesianDiagram* parent)
    : KChart::CartesianAxis(parent)
{
}

const QString KBalanceAxis::customizedLabel(const QString& label) const
{
  //TODO: make precision variable
  int precision = 2;

  //format as money using base currency or the included accounts
  return QLocale().toString(label.toDouble(), precision);
}
