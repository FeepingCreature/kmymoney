/***************************************************************************
                          kgpgkeyselectiondlg.h
                             -------------------
    copyright            : (C) 2008 by Thomas Baumgart
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

#ifndef KGPGKEYSELECTIONDLG_H
#define KGPGKEYSELECTIONDLG_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QDialog>

// ----------------------------------------------------------------------------
// KDE Includes

#include <KEditListWidget>

class KLed;

// ----------------------------------------------------------------------------
// Project Includes

/**
  * @author Thomas Baumgart
  */
class KGpgKeySelectionDlg : public QDialog
{
  Q_OBJECT
public:

  explicit KGpgKeySelectionDlg(QWidget* parent = 0);
  virtual ~KGpgKeySelectionDlg() {}

  /**
   * preset the key list with the given key ids in @a list
   */
  void setKeys(const QStringList& list);

  /**
   * Returns the list of keys currently listed in the KEditListWidget
   */
  const QStringList keys() const {
    return m_listWidget->items();
  }

protected slots:
  void slotIdChanged();
  void slotKeyListChanged();

private:
  KEditListWidget*   m_listWidget;
  KLed*           m_keyLed;
  bool            m_needCheckList;
  bool            m_listOk;
  int             m_checkCount;
};

#endif
