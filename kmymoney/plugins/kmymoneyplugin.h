/*
 * This file is part of KMyMoney, A Personal Finance Manager for KDE
 * Copyright (C) 2005 Thomas Baumgart <ipwizard@users.sourceforge.net>
 * Copyright (C) 2015 Christian Dávid <christian-david@web.de>
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

#ifndef KMYMONEYPLUGIN_H
#define KMYMONEYPLUGIN_H

// ----------------------------------------------------------------------------
// QT Includes

#include <QObject>

// ----------------------------------------------------------------------------
// KDE Includes

#include <KXMLGUIClient>
class KAction;
class KToggleAction;

// ----------------------------------------------------------------------------
// Project Includes

#include <viewinterface.h>
#include <statementinterface.h>
#include <importinterface.h>
#include <kmm_plugin_export.h>

/**
 * @defgroup KMyMoneyPlugin
 *
 * KMyMoney knows several types of plugins. The most common and generic one is KMyMoneyPlugin::Plugin. There are seveal other classes a plugin can inherit from
 * to create a native integration into KMyMoney.
 *
 * Another group of plugins are just loaded on demand and offer special functions with a tight integration into KMyMoney. Whenever possible you should use this kind of plugins.
 * At the moment this are the onlineTask and payeeIdentifierData.
 *
 * @{
 */

namespace KMyMoneyPlugin
{

/**
 * This class describes the interface between KMyMoney and it's plugins.
 *
 * The plugins are based on Qt 5's plugin system. So you must compile json information into the plugin.
 * KMyMoney looks into the folder "${PLUGIN_INSTALL_DIR}/kmymoney/" and loads all plugins found there (if the user did not deactivate the plugin).
 *
 * @see http://doc.qt.io/qt-5/plugins-howto.html
 */
class KMM_PLUGIN_EXPORT Plugin : public QObject, public KXMLGUIClient
{
  Q_OBJECT
public:
  Plugin(QObject* parent = nullptr, const char* name = "");
  virtual ~Plugin();

protected:
  /** See KMyMoneyApp::toggleAction() for a description */
  KToggleAction* toggleAction(const QString& name) const;

  // define interface classes here. The interface classes provide a mechanism
  // for the plugin to interact with KMyMoney
  // they are defined in the following form for an interface
  // named Xxx:
  //
  // XxxInterface* xxxInterface();
  ViewInterface*          viewInterface() const;
  StatementInterface*     statementInterface() const;
  ImportInterface*        importInterface() const;
};

/**
   * This class describes the interface between the KMyMoney
   * application and it's ONLINE-BANKING plugins. All online banking plugins
   * must provide this interface.
   *
   * A good tutorial on how to design and develop a plugin
   * structure for a KDE application (e.g. KMyMoney) can be found at
   * http://developer.kde.org/documentation/tutorials/developing-a-plugin-structure/index.html
   *
 */
class KMM_PLUGIN_EXPORT OnlinePlugin
{
public:
  OnlinePlugin() {}
  virtual ~OnlinePlugin() {}

  virtual void protocols(QStringList& protocolList) const = 0;

  /**
   * This method returns a pointer to a widget representing an additional
   * tab that will be added to the KNewAccountDlg. The string referenced
   * with @a tabName will be filled with the text that should be placed
   * on the tab. It should return 0 if no additional tab is needed.
   *
   * Information about the account can be taken out of @a account.
   *
   * Once the pointer to the widget is returned to KMyMoney, it takes care
   * of destruction of all included widgets when the dialog is closed. The plugin
   * can access the widgets created after the call to storeConfigParameters()
   * happened.
   */
  virtual QWidget* accountConfigTab(const MyMoneyAccount& account, QString& tabName) = 0;

  /**
   * This method is called by the framework whenever it is time to store
   * the configuration data maintained by the plugin. The plugin should use
   * the widgets created in accountConfigTab() to extract the current values.
   *
   * @param current The @a current container contains the current settings
   */
  virtual MyMoneyKeyValueContainer onlineBankingSettings(const MyMoneyKeyValueContainer& current) = 0;

  /**
   * This method is called by the framework when the user wants to map
   * a KMyMoney account onto an online account. The KMyMoney account is identified
   * by @a acc and the online provider should store its data in @a onlineBankingSettings
   * upon success.
   *
   * @retval true if account is mapped
   * @retval false if account is not mapped
   */
  virtual bool mapAccount(const MyMoneyAccount& acc, MyMoneyKeyValueContainer& onlineBankingSettings) = 0;

  /**
   * This method is called by the framework when the user wants to update
   * a KMyMoney account with data from an online account. The KMyMoney account is identified
   * by @a acc. The online provider should read its data from acc.onlineBankingSettings().
   * @a true is returned upon success. The plugin might consider to stack the requests
   * in case @a moreAccounts is @p true. @a moreAccounts defaults to @p false.
   *
   * @retval true if account is updated
   * @retval false if account is not updated
   */
  virtual bool updateAccount(const MyMoneyAccount& acc, bool moreAccounts = false) = 0;
};

/**
  * This class describes the interface between the KMyMoney
  * application and it's IMPORTER plugins. All importer plugins
  * must provide this interface.
  *
  * A good tutorial on how to design and develop a plugin
  * structure for a KDE application (e.g. KMyMoney) can be found at
  * http://developer.kde.org/documentation/tutorials/developing-a-plugin-structure/index.html
  *
  */
class KMM_PLUGIN_EXPORT ImporterPlugin
{
public:
  ImporterPlugin() {}
  virtual ~ImporterPlugin() {}

  /**
    * This method returns the english-language name of the format
    * this plugin imports, e.g. "OFX"
    *
    * @return QString Name of the format
    */
  virtual QString formatName() const = 0;

  /**
    * This method returns the filename filter suitable for passing to
    * KFileDialog::setFilter(), e.g. "*.ofx *.qfx" which describes how
    * files of this format are likely to be named in the file system
    *
    * @return QString Filename filter string
    */
  virtual QString formatFilenameFilter() const = 0;

  /**
    * This method returns whether this plugin is able to import
    * a particular file.
    *
    * @param filename Fully-qualified pathname to a file
    *
    * @return bool Whether the indicated file is importable by this plugin
    */
  virtual bool isMyFormat(const QString& filename) const = 0;

  /**
    * Import a file
    *
    * @param filename File to import
    *
    * @return bool Whether the import was successful.
    */
  virtual bool import(const QString& filename) = 0;

  /**
    * Returns the error result of the last import
    *
    * @return QString English-language name of the error encountered in the
    *  last import, or QString() if it was successful.
    *
    */
  virtual QString lastError() const = 0;

};

} // end of namespace

Q_DECLARE_INTERFACE(KMyMoneyPlugin::OnlinePlugin, "org.kmymoney.plugin.onlineplugin")
Q_DECLARE_INTERFACE(KMyMoneyPlugin::ImporterPlugin, "org.kmymoney.plugin.importerplugin")


/** @} */

#endif
