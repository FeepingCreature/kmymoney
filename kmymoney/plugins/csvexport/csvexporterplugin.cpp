/***************************************************************************
                            csvexporterplugin.cpp
                       (based on ofximporterplugin.cpp)
                             -------------------
    begin                : Wed Mar 20 2013
    copyright            : (C) 2013 by Allan Anderson
    email                : agander93@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "csvexporterplugin.h"
#include "csvexportdlg.h"
#include "csvwriter.h"
#include <assert.h>
// ----------------------------------------------------------------------------
// QT Includes

#include <QFile>
#include <QDebug>
#include <QUrl>

// ----------------------------------------------------------------------------
// KDE Includes

#include <KFile>
#include <KMessageBox>
#include <KActionCollection>
#include <KSharedConfig>
#include <KLocalizedString>

// ----------------------------------------------------------------------------
// Project Includes

#include "kmymoney.h"

CsvExporterPlugin::CsvExporterPlugin() :
    KMyMoneyPlugin::Plugin(nullptr, "csvexport"/*must be the same as X-KDE-PluginInfo-Name*/)
{
  setComponentName("kmm_csvexport", i18n("CSV exporter"));
  setXMLFile("kmm_csvexport.rc");
  createActions();
  // For information, announce that we have been loaded.
  qDebug("KMyMoney csvexport plugin loaded");
}

CsvExporterPlugin::~CsvExporterPlugin()
{
}

void CsvExporterPlugin::createActions()
{
  m_action = actionCollection()->addAction("file_export_csv");
  m_action->setText(i18n("&CSV..."));
  connect(m_action, SIGNAL(triggered(bool)), this, SLOT(slotCsvExport()));
}

void CsvExporterPlugin::slotCsvExport()
{
  m_dlg = new CsvExportDlg();
  if (m_dlg->exec()) {
    if (okToWriteFile(QUrl::fromUserInput(m_dlg->filename()))) {
      m_dlg->setWindowTitle(i18nc("CSV Exporter dialog title", "CSV Exporter"));
      CsvWriter* writer = new CsvWriter;
      writer->m_plugin = this;
      connect(writer, SIGNAL(signalProgress(int,int)), m_dlg, SLOT(slotStatusProgressBar(int,int)));

      writer->write(m_dlg->filename(), m_dlg->accountId(),
                    m_dlg->accountSelected(), m_dlg->categorySelected(),
                    m_dlg->startDate(), m_dlg->endDate(),
                    m_dlg->separator());
    }
  }
}

bool CsvExporterPlugin::okToWriteFile(const QUrl &url)
{
  Q_UNUSED(url)

  // check if the file exists and warn the user
  bool reallySaveFile = true;

  // TODO: port this to KF5
#if 0
  if (KIO::NetAccess::exists(url, KIO::NetAccess::SourceSide, 0)) {
    if (KMessageBox::warningYesNo(0, QString("<qt>") + i18n("The file <b>%1</b> already exists. Do you really want to overwrite it?", url.toDisplayString(QUrl::PreferLocalFile)) + QString("</qt>"), i18n("File already exists")) != KMessageBox::Yes)
      reallySaveFile = false;
  }
#endif
  return reallySaveFile;
}

#include "csvexporterplugin.moc"
