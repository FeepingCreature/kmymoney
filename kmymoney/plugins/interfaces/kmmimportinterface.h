/***************************************************************************
                          kmmimportinterface.h
                             -------------------
    begin                : Mon Apr 14 2008
    copyright            : (C) 2008 Thomas Baumgart
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

#ifndef KMMIMPORTINTERFACE_H
#define KMMIMPORTINTERFACE_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QUrl>

// ----------------------------------------------------------------------------
// KDE Includes

class KMyMoneyApp;

// ----------------------------------------------------------------------------
// Project Includes

#include "importinterface.h"

namespace KMyMoneyPlugin
{

/**
  * This class represents the implementation of the
  * ImportInterface.
  */
class KMMImportInterface : public ImportInterface
{
  Q_OBJECT

public:
  KMMImportInterface(KMyMoneyApp* app, QObject* parent, const char* name = 0);
  ~KMMImportInterface() {}

  QUrl selectFile(const QString& title, const QString& path, const QString& mask, QFileDialog::FileMode mode, QWidget *widget) const;

private:
  KMyMoneyApp*    m_app;
};

} // namespace
#endif
