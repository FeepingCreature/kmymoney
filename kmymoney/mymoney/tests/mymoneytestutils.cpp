/***************************************************************************
                          mymoneytestutils.cpp
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

#include "mymoneyutils.h"

#include "mymoneyexception.h"

QString unexpectedExceptionString(const MyMoneyException &e)
{
  return QString("Unexpected exception: %1 thrown in %2:%3")
         .arg(e.what())
         .arg(e.file())
         .arg(e.line());
}

