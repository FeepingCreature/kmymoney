/*******************************************************************************
*                                 csvwizard.cpp
*                              --------------------
* begin                       : Thur Jan 01 2015
* copyright                   : (C) 2015 by Allan Anderson
* email                       : agander93@gmail.com
* copyright                   : (C) 2016 by Łukasz Wojniłowicz
* email                       : lukasz.wojnilowicz@gmail.com
********************************************************************************/

/*******************************************************************************
*                                                                              *
*   This program is free software; you can redistribute it and/or modify       *
*   it under the terms of the GNU General Public License as published by       *
*   the Free Software Foundation; either version 2 of the License, or          *
*   (at your option) any later version.                                        *
*                                                                              *
********************************************************************************/

#include "csvwizard.h"

// ----------------------------------------------------------------------------
// QT Includes

#include <QDesktopWidget>
#include <QCloseEvent>
#include <QDebug>
#include <QTextCodec>
#include <QAbstractButton>

// ----------------------------------------------------------------------------
// KDE Includes

#include <KMessageBox>
#include <KLocalizedString>
#include <KConfigGroup>
#include <KColorScheme>

// ----------------------------------------------------------------------------
// Project Includes

#include "ui_csvwizard.h"
#include "ui_introwizardpage.h"
#include "ui_separatorwizardpage.h"
#include "ui_rowswizardpage.h"
#include "ui_bankingwizardpage.h"
#include "ui_investmentwizardpage.h"
#include "ui_priceswizardpage.h"
#include "ui_formatswizardpage.h"

#include "ui_securitiesdlg.h"
#include "ui_securitydlg.h"

CSVWizard::CSVWizard(CsvImporterPlugin* plugin, CSVImporter* importer) :
    ui(new Ui::CSVWizard),
    m_plugin(plugin),
    m_imp(importer),
    m_wiz(new QWizard)
{
  ui->setupUi(this);
  ui->tableView->setModel(m_imp->m_file->m_model);
  readWindowSize(CSVImporter::configFile());
  m_wiz->setWizardStyle(QWizard::ClassicStyle);
  ui->horizontalLayout->addWidget(m_wiz);
  m_curId = -1;
  m_lastId = -1;
  m_wiz->installEventFilter(this); // event filter for escape key presses

  m_wiz->button(QWizard::BackButton)->setIcon(QIcon::fromTheme(QStringLiteral("go-previous")));
  m_wiz->button(QWizard::CancelButton)->setIcon(QIcon::fromTheme(QStringLiteral("dialog-cancel"),
                                                                    QIcon::fromTheme(QStringLiteral("stop"))));
  m_wiz->button(QWizard::CustomButton2)->setIcon(QIcon::fromTheme(QStringLiteral("kmymoney")));
  m_wiz->button(QWizard::FinishButton)->setIcon(QIcon::fromTheme(QStringLiteral("dialog-ok-apply"),
                                                                    QIcon::fromTheme(QStringLiteral("finish"))));
  m_wiz->button(QWizard::CustomButton1)->setIcon(QIcon::fromTheme(QStringLiteral("system-file-manager")));
  m_wiz->button(QWizard::CustomButton3)->setIcon(QIcon::fromTheme(QStringLiteral("invest-applet")));
  m_wiz->button(QWizard::NextButton)->setIcon(QIcon::fromTheme(QStringLiteral("go-next")));

  m_pageIntro = new IntroPage(this, m_imp);
  m_wiz->setPage(PageIntro, m_pageIntro);

  m_pageSeparator = new SeparatorPage(this, m_imp);
  m_wiz->setPage(PageSeparator, m_pageSeparator);

  m_pageRows = new RowsPage(this, m_imp);
  m_wiz->setPage(PageRows, m_pageRows);

  m_pageFormats = new FormatsPage(this, m_imp);
  m_wiz->setPage(PageFormats, m_pageFormats);

  showStage();
  m_wiz->button(QWizard::CustomButton1)->setEnabled(false);

  m_stageLabels << ui->label_intro << ui->label_separators << ui->label_rows << ui->label_columns << ui->label_columns << ui->label_columns << ui->label_formats;
  m_pageFormats->setFinalPage(true);

  this->setAttribute(Qt::WA_DeleteOnClose, true);

  connect(m_wiz->button(QWizard::FinishButton), &QAbstractButton::clicked, this, &CSVWizard::slotClose);
  connect(m_wiz->button(QWizard::CancelButton), &QAbstractButton::clicked, this, &QWidget::close);
  connect(m_wiz->button(QWizard::CustomButton1), &QAbstractButton::clicked, this, &CSVWizard::fileDialogClicked);
  connect(m_wiz->button(QWizard::CustomButton2), &QAbstractButton::clicked, this, &CSVWizard::importClicked);
  connect(m_wiz->button(QWizard::CustomButton3), &QAbstractButton::clicked, this, &CSVWizard::saveAsQIFClicked);
  connect(m_wiz, SIGNAL(currentIdChanged(int)), this, SLOT(slotIdChanged(int)));

  ui->tableView->setWordWrap(false);

  m_vScrollBar = ui->tableView->verticalScrollBar();
  m_vScrollBar->setTracking(false);

  m_clearBrush = KColorScheme(QPalette::Normal).background(KColorScheme::NormalBackground);
  m_clearBrushText = KColorScheme(QPalette::Normal).foreground(KColorScheme::NormalText);
  m_colorBrush = KColorScheme(QPalette::Normal).background(KColorScheme::PositiveBackground);
  m_colorBrushText = KColorScheme(QPalette::Normal).foreground(KColorScheme::PositiveText);
  m_errorBrush = KColorScheme(QPalette::Normal).background(KColorScheme::NegativeBackground);
  m_errorBrushText = KColorScheme(QPalette::Normal).foreground(KColorScheme::NegativeText);

  show();
}

CSVWizard::~CSVWizard()
{
  delete ui;
  if (m_wiz)
    delete m_wiz;
}

void CSVWizard::showStage()
{
  QString str = ui->label_intro->text();
  ui->label_intro->setText(QLatin1String("<b>") + str + QLatin1String("/<b>"));
}

void CSVWizard::readWindowSize(const KSharedConfigPtr& config) {
  KConfigGroup miscGroup(config, CSVImporter::m_confMiscName);
  m_initialWidth = miscGroup.readEntry(CSVImporter::m_miscSettingsConfName.value(ConfWidth), 800);
  m_initialHeight = miscGroup.readEntry(CSVImporter::m_miscSettingsConfName.value(ConfHeight), 400);
}

void CSVWizard::saveWindowSize(const KSharedConfigPtr& config) {
  KConfigGroup miscGroup(config, CSVImporter::m_confMiscName);
  m_initialHeight = this->geometry().height();
  m_initialWidth = this->geometry().width();
  miscGroup.writeEntry(CSVImporter::m_miscSettingsConfName.value(ConfWidth), m_initialWidth);
  miscGroup.writeEntry(CSVImporter::m_miscSettingsConfName.value(ConfHeight), m_initialHeight);
  miscGroup.sync();
}

void CSVWizard::slotIdChanged(int id)
{
  QString txt;
  m_lastId = m_curId;
  m_curId = id;
  if ((m_lastId == -1) || (m_curId == -1)) {
    return;
  }
  txt = m_stageLabels[m_lastId]->text();
  txt.remove(QRegularExpression(QStringLiteral("[<b>/]")));
  m_stageLabels[m_lastId]->setText(txt);

  txt = m_stageLabels[m_curId]->text();
  txt = QLatin1String("<b>") + txt + QLatin1String("</b>");
  m_stageLabels[m_curId]->setText(txt);
}

void CSVWizard::clearColumnsBackground(const int col)
{
  QList<int> columnList;
  columnList << col;
  clearColumnsBackground(columnList);
}

void CSVWizard::clearColumnsBackground(const QList<int> &columnList)
{
  QStandardItemModel *model = m_imp->m_file->m_model;
  for (int i = m_imp->m_profile->m_startLine; i <= m_imp->m_profile->m_endLine; ++i) {
    foreach (const auto j, columnList) {
      model->item(i, j)->setBackground(m_clearBrush);
      model->item(i, j)->setForeground(m_clearBrushText);
    }
  }
}

void CSVWizard::clearBackground()
{
  QStandardItemModel *model = m_imp->m_file->m_model;
  int rowCount = model->rowCount();
  int colCount = model->columnCount();
  for (int i = 0; i < rowCount; ++i) {
    for (int j = 0; j < colCount; ++j) {
      model->item(i, j)->setBackground(m_clearBrush);
      model->item(i, j)->setForeground(m_clearBrushText);
    }
  }
}

void CSVWizard::markUnwantedRows()
{
  QStandardItemModel *model = m_imp->m_file->m_model;
  int rowCount = model->rowCount();
  int colCount = model->columnCount();
  QBrush brush;
  QBrush brushText;
  for (int i = 0; i < rowCount; ++i) {
    if ((i < m_imp->m_profile->m_startLine) || (i > m_imp->m_profile->m_endLine)) {
      brush = m_errorBrush;
      brushText = m_errorBrushText;
    } else {
      brush = m_clearBrush;
      brushText = m_clearBrushText;
    }
    for (int j = 0; j < colCount; ++j) {
      model->item(i, j)->setBackground(brush);
      model->item(i, j)->setForeground(brushText);
    }
  }
}

void CSVWizard::updateWindowSize()
{
  QTableView *table = this->ui->tableView;
  table->resizeColumnsToContents();
  this->repaint();

  QRect screen = QApplication::desktop()->availableGeometry();    //get available screen size
  QRect wizard = this->frameGeometry();                           //get current wizard size

  int newWidth = table->contentsMargins().left() +
                 table->contentsMargins().right() +
                 table->horizontalHeader()->length() +
                 table->verticalHeader()->width() +
                 (wizard.width() - table->width());
  if (table->verticalScrollBar()->isEnabled()) {
    if (!table->verticalScrollBar()->isVisible() && // vertical scrollbar may be not visible after repaint...
        table->horizontalScrollBar()->isVisible())  // ...so use horizontal scrollbar dimension
      newWidth += table->horizontalScrollBar()->height();
    else
      newWidth += table->verticalScrollBar()->width();
  }

  int newHeight = table->contentsMargins().top() +
                  table->contentsMargins().bottom() +
                  table->verticalHeader()->length() +
                  table->horizontalHeader()->height() +
                  (wizard.height() - table->height());

  if (table->horizontalScrollBar()->isEnabled()) {
    if (!table->horizontalScrollBar()->isVisible() && // horizontal scrollbar may be not visible after repaint...
        table->verticalScrollBar()->isVisible())      // ...so use vertical scrollbar dimension
      newHeight += table->verticalScrollBar()->width();
    else
      newHeight += table->horizontalScrollBar()->height();
  }

  // limit wizard size to screen size
  if (newHeight > screen.height())
    newHeight = screen.height();

  if (newWidth > screen.width())
    newWidth = screen.width();


  // don't shrink wizard if required size is less than initial
  if (newWidth < this->m_initialWidth)
    newWidth = this->m_initialWidth;
  if (newHeight < this->m_initialHeight)
    newHeight = this->m_initialHeight;

  newWidth -= (wizard.width() - this->geometry().width());      // remove window frame
  newHeight -= (wizard.height() - this->geometry().height());

  wizard.setWidth(newWidth);
  wizard.setHeight(newHeight);
  wizard.moveTo((screen.width() - wizard.width()) / 2,
                (screen.height() - wizard.height()) / 2);
  this->setGeometry(wizard);
}

void CSVWizard::closeEvent(QCloseEvent *event)
{
  this->m_plugin->m_action->setEnabled(true); // reenable File->Import->CSV
  event->accept();
}

bool CSVWizard::eventFilter(QObject *object, QEvent *event)
{
  // prevent the QWizard part of CSVWizard window from closing on Escape key press
  if (object == this->m_wiz) {
    if (event->type() == QEvent::KeyPress) {
      QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
      if (keyEvent->key() == Qt::Key_Escape) {
        close();
        return true;
      }
    }
  }
  return false;
}

void CSVWizard::slotClose()
{
  m_imp->m_profile->m_lastUsedDirectory = m_imp->m_file->m_inFileName;
  m_imp->m_profile->writeSettings(CSVImporter::configFile());
  m_imp->profilesAction(m_imp->m_profile->type(), ProfilesUpdateLastUsed, m_imp->m_profile->m_profileName, m_imp->m_profile->m_profileName);
  close();
}

void CSVWizard::fileDialogClicked()
{
  m_imp->profileFactory(m_pageIntro->m_profileType, m_pageIntro->ui->m_profiles->currentText());
  bool profileExists = m_imp->m_profile->readSettings(CSVImporter::configFile());

  if (!m_imp->m_file->getInFileName(m_imp->m_profile->m_lastUsedDirectory))
    return;

  saveWindowSize(CSVImporter::configFile());
  m_imp->m_file->readFile(m_imp->m_profile);
  m_imp->m_file->setupParser(m_imp->m_profile);

  m_skipSetup = m_pageIntro->ui->m_skipSetup->isChecked();

  switch(m_imp->m_profile->type()) {
    case ProfileInvest:
      if (!m_pageInvestment) {
        m_pageInvestment = new InvestmentPage(this, m_imp);
        m_wiz->setPage(CSVWizard::PageInvestment, m_pageInvestment);
      }
      break;
    case ProfileBank:
      if (!m_pageBanking) {
        m_pageBanking = new BankingPage(this, m_imp);
        m_wiz->setPage(CSVWizard::PageBanking, m_pageBanking);
      }
      break;
    case ProfileStockPrices:
    case ProfileCurrencyPrices:
      if (!m_pagePrices) {
        m_pagePrices = new PricesPage(this, m_imp);
        m_wiz->setPage(CSVWizard::PagePrices, m_pagePrices);
      }
      break;
    default:
      return;
  }

  m_wiz->next();  //go to separator page

  if (m_skipSetup && profileExists)
    for (int i = 0; i < 4; i++) //programmaticaly go through separator-, rows-, investment-/bank-, formatspage
      m_wiz->next();
}

void CSVWizard::importClicked()
{
  switch (m_imp->m_profile->type()) {
    case ProfileBank:
      if (!m_pageBanking->validateCreditDebit())
        return;
      break;
    case ProfileInvest:
      if (!m_pageInvestment->validateActionType())
        return;
      break;
    default:
      break;
  }

  if (!m_imp->createStatement(m_st))
    return;
  slotClose();
  emit m_imp->statementReady(m_st);
}

void CSVWizard::saveAsQIFClicked()
{
  switch (m_imp->m_profile->type()) {
    case ProfileBank:
      if (!m_pageBanking->validateCreditDebit())
        return;
      break;
    case ProfileInvest:
      if (!m_pageInvestment->validateActionType())
        return;
      break;
    default:
      break;
  }

  bool isOK = m_imp->createStatement(m_st);
  if (!isOK || m_st.m_listTransactions.isEmpty())
    return;

  QString outFileName = m_imp->m_file->m_inFileName;
  outFileName.truncate(outFileName.lastIndexOf('.'));
  outFileName.append(QLatin1String(".qif"));
  outFileName = QFileDialog::getSaveFileName(this, i18n("Save QIF"), outFileName, i18n("QIF Files (*.qif)"));
  if (outFileName.isEmpty())
    return;
  QFile oFile(outFileName);
  oFile.open(QIODevice::WriteOnly);
  switch (m_imp->m_profile->type()) {
    case ProfileBank:
      m_pageBanking->makeQIF(m_st, oFile);
      break;
    case ProfileInvest:
      m_pageInvestment->makeQIF(m_st, oFile);
      break;
    default:
      break;
  }
  oFile.close();
}

//-------------------------------------------------------------------------------------------------------
IntroPage::IntroPage(CSVWizard *dlg, CSVImporter *imp) :
  CSVWizardPage(dlg, imp),
  ui(new Ui::IntroPage),
  m_pageLayout(0)
{
  ui->setupUi(this);
}

IntroPage::~IntroPage()
{
  delete ui;
}

int IntroPage::nextId() const
{
  return CSVWizard::PageSeparator;
}

void IntroPage::initializePage()
{
  m_imp->m_file->m_model->clear();

  wizard()->setButtonText(QWizard::CustomButton1, i18n("Select File"));
  wizard()->button(QWizard::CustomButton1)->setToolTip(i18n("A profile must be selected before selecting a file."));
  QList<QWizard::WizardButton> layout;
  layout << QWizard::Stretch <<
            QWizard::CustomButton1 <<
            QWizard::CancelButton;
  wizard()->setButtonLayout(layout);

  ui->m_profiles->lineEdit()->setClearButtonEnabled(true);

  connect(ui->m_profiles, SIGNAL(currentIndexChanged(int)), this, SLOT(slotComboSourceIndexChanged(int)));
  connect(ui->m_add, SIGNAL(clicked()), this, SLOT(slotAddProfile()));
  connect(ui->m_remove, SIGNAL(clicked()), this, SLOT(slotRemoveProfile()));
  connect(ui->m_rename, SIGNAL(clicked()), this, SLOT(slotRenameProfile()));
  connect(ui->m_profilesBank, SIGNAL(toggled(bool)), this, SLOT(slotBankRadioToggled(bool)));
  connect(ui->m_profilesInvest, SIGNAL(toggled(bool)), this, SLOT(slotInvestRadioToggled(bool)));
  connect(ui->m_profilesCurrencyPrices, SIGNAL(toggled(bool)), this, SLOT(slotCurrencyPricesRadioToggled(bool)));
  connect(ui->m_profilesStockPrices, SIGNAL(toggled(bool)), this, SLOT(slotStockPricesRadioToggled(bool)));
  if (m_dlg->m_initialHeight == -1 || m_dlg->m_initialWidth == -1) {
    m_dlg->m_initialHeight = m_dlg->geometry().height();
    m_dlg->m_initialWidth = m_dlg->geometry().width();
  } else {
    //resize wizard to its initial size and center it
    m_dlg->setGeometry(
          QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            QSize(m_dlg->m_initialWidth, m_dlg->m_initialHeight),
            QApplication::desktop()->availableGeometry()
            )
          );
  }
}

bool IntroPage::validatePage()
{
  return true;
}

void IntroPage::slotAddProfile()
{
  profileChanged(ProfilesAdd);
}

void IntroPage::slotRemoveProfile()
{
  profileChanged(ProfilesRemove);
}

void IntroPage::slotRenameProfile()
{
  profileChanged(ProfilesRename);
}

void IntroPage::profileChanged(const profilesActionE action)
{
  QString cbText = ui->m_profiles->currentText();
  if (cbText.isEmpty()) // you cannot neither add nor remove empty name profile or rename to empty name
    return;

  int cbIndex = ui->m_profiles->currentIndex();

  switch (action) {
    case ProfilesRename:
    case ProfilesAdd:
    {
      int dupIndex = m_profiles.indexOf(QRegularExpression (cbText));
      if (dupIndex == cbIndex && cbIndex != -1)  // if profile name wasn't changed then return
        return;
      else if (dupIndex != -1) {    // profile with the same name already exists
        ui->m_profiles->setItemText(cbIndex, m_profiles.value(cbIndex));
        KMessageBox::information(m_dlg,
                                 i18n("<center>Profile <b>%1</b> already exists.<br>"
                                      "Please enter another name</center>", cbText));
        return;
      }
      break;
    }
    case ProfilesRemove:
      if (m_profiles.value(cbIndex) != cbText) // user changed name of the profile and tries to remove it
        return;
      break;
    default:
      break;
  }

  if (CSVImporter::profilesAction(m_profileType, action, m_profiles.value(cbIndex), cbText)) {
    switch (action) {
      case ProfilesAdd:
        m_profiles.append(cbText);
        ui->m_profiles->addItem(cbText);
        ui->m_profiles->setCurrentIndex(m_profiles.count() - 1);
        KMessageBox::information(m_dlg,
                                 i18n("<center>Profile <b>%1</b> has been added.</center>", cbText));
        break;
      case ProfilesRemove:
        m_profiles.removeAt(cbIndex);
        ui->m_profiles->removeItem(cbIndex);
        KMessageBox::information(m_dlg,
                                 i18n("<center>Profile <b>%1</b> has been removed.</center>",
                                      cbText));
        break;
      case ProfilesRename:
        ui->m_profiles->setItemText(cbIndex, cbText);
        KMessageBox::information(m_dlg,
                                 i18n("<center>Profile name has been renamed from <b>%1</b> to <b>%2</b>.</center>",
                                      m_profiles.value(cbIndex), cbText));
        m_profiles[cbIndex] = cbText;
        break;
      default:
        break;
    }
  }
}

void IntroPage::slotComboSourceIndexChanged(int idx)
{
  if (idx == -1) {
    wizard()->button(QWizard::CustomButton1)->setEnabled(false);
    ui->m_skipSetup->setEnabled(false);
    ui->m_remove->setEnabled(false);
    ui->m_rename->setEnabled(false);
  }
  else {
    wizard()->button(QWizard::CustomButton1)->setEnabled(true);
    ui->m_skipSetup->setEnabled(true);
    ui->m_remove->setEnabled(true);
    ui->m_rename->setEnabled(true);
  }
}

void IntroPage::profileTypeChanged(const profileTypeE profileType, bool toggled)
{
  if (!toggled)
    return;

  KConfigGroup profilesGroup(CSVImporter::configFile(), CSVImporter::m_confProfileNames);
  m_profileType = profileType;
  QString profileTypeStr;
  switch (m_profileType) {
    case ProfileBank:
      ui->m_profilesInvest->setChecked(false);
      ui->m_profilesStockPrices->setChecked(false);
      ui->m_profilesCurrencyPrices->setChecked(false);
      break;
    case ProfileInvest:
      ui->m_profilesBank->setChecked(false);
      ui->m_profilesStockPrices->setChecked(false);
      ui->m_profilesCurrencyPrices->setChecked(false);
      break;
    case ProfileStockPrices:
      ui->m_profilesBank->setChecked(false);
      ui->m_profilesInvest->setChecked(false);
      ui->m_profilesCurrencyPrices->setChecked(false);
      break;
    case ProfileCurrencyPrices:
      ui->m_profilesBank->setChecked(false);
      ui->m_profilesInvest->setChecked(false);
      ui->m_profilesStockPrices->setChecked(false);
      break;
    default:
      break;
  }
  profileTypeStr = CSVImporter::m_profileConfPrefix.value(m_profileType);

  m_profiles = profilesGroup.readEntry(profileTypeStr, QStringList());
  int priorProfile = profilesGroup.readEntry(CSVImporter::m_confPriorName + profileTypeStr, 0);
  ui->m_profiles->clear();
  ui->m_profiles->addItems(m_profiles);
  ui->m_profiles->setCurrentIndex(priorProfile);
  ui->m_profiles->setEnabled(true);
  ui->m_add->setEnabled(true);
}

void IntroPage::slotBankRadioToggled(bool toggled)
{
  profileTypeChanged(ProfileBank, toggled);
}

void IntroPage::slotInvestRadioToggled(bool toggled)
{
  profileTypeChanged(ProfileInvest, toggled);
}

void IntroPage::slotCurrencyPricesRadioToggled(bool toggled)
{
  profileTypeChanged(ProfileCurrencyPrices, toggled);
}

void IntroPage::slotStockPricesRadioToggled(bool toggled)
{
  profileTypeChanged(ProfileStockPrices, toggled);
}

SeparatorPage::SeparatorPage(CSVWizard *dlg, CSVImporter *imp) :
  CSVWizardPage(dlg, imp),
  ui(new Ui::SeparatorPage)
{
  ui->setupUi(this);

  m_pageLayout = new QVBoxLayout;
  ui->horizontalLayout->insertLayout(0, m_pageLayout);
}

SeparatorPage::~SeparatorPage()
{
  delete ui;
}

void SeparatorPage::initializePage()
{
  // comboboxes are preset to -1 and, in new profile case, can be set here to -1 as well ...
  initializeEncodingCombobox();
  ui->m_encoding->setCurrentIndex(ui->m_encoding->findData(m_imp->m_profile->m_encodingMIBEnum));
  ui->m_fieldDelimiter->setCurrentIndex(m_imp->m_profile->m_fieldDelimiterIndex);
  ui->m_textDelimiter->setCurrentIndex(m_imp->m_profile->m_textDelimiterIndex);

  // ... so connect their signals after setting them ...
  connect(ui->m_encoding, SIGNAL(currentIndexChanged(int)), this, SLOT(encodingChanged(int)));
  connect(ui->m_fieldDelimiter, SIGNAL(currentIndexChanged(int)), this, SLOT(fieldDelimiterChanged(int)));
  connect(ui->m_textDelimiter, SIGNAL(currentIndexChanged(int)), this, SLOT(textDelimiterChanged(int)));

  // ... and ensure that their signal receivers will always be called
  emit ui->m_encoding->currentIndexChanged(ui->m_encoding->currentIndex());
  emit ui->m_fieldDelimiter->currentIndexChanged(ui->m_fieldDelimiter->currentIndex());
  emit ui->m_textDelimiter->currentIndexChanged(ui->m_textDelimiter->currentIndex());
  m_dlg->updateWindowSize();

  QList<QWizard::WizardButton> layout;
  layout << QWizard::Stretch << QWizard::BackButton << QWizard::NextButton << QWizard::CancelButton;
  wizard()->setButtonLayout(layout);
}

void SeparatorPage::initializeEncodingCombobox()
{
  ui->m_encoding->blockSignals(true);
  ui->m_encoding->clear();

  QList<QTextCodec *>   codecs;
  QMap<QString, QTextCodec *> codecMap;
  QRegExp iso8859RegExp(QLatin1Literal("ISO[- ]8859-([0-9]+).*"));

  foreach (const auto mib, QTextCodec::availableMibs()) {
    QTextCodec *codec = QTextCodec::codecForMib(mib);

    QString sortKey = codec->name().toUpper();
    int rank;

    if (sortKey.startsWith(QLatin1String("UTF-8"))) {             // krazy:exclude=strings
      rank = 1;
    } else if (sortKey.startsWith(QLatin1String("UTF-16"))) {            // krazy:exclude=strings
      rank = 2;
    } else if (iso8859RegExp.exactMatch(sortKey)) {
      if (iso8859RegExp.cap(1).size() == 1)
        rank = 3;
      else
        rank = 4;
    } else {
      rank = 5;
    }
    sortKey.prepend(QChar('0' + rank));

    codecMap.insert(sortKey, codec);
  }
  codecs = codecMap.values();

  foreach (const auto codec, codecs)
    ui->m_encoding->addItem(codec->name(), codec->mibEnum());
  ui->m_encoding->blockSignals(false);
}

void SeparatorPage::encodingChanged(const int index)
{
  if (index == -1) {
    ui->m_encoding->setCurrentIndex(ui->m_encoding->findText("UTF-8"));
    return;
  } else if (index == ui->m_encoding->findData(m_imp->m_profile->m_encodingMIBEnum))
    return;
  m_imp->m_profile->m_encodingMIBEnum = ui->m_encoding->currentData().toInt();
  m_imp->m_file->readFile(m_imp->m_profile);
  emit completeChanged();
}

void SeparatorPage::fieldDelimiterChanged(const int index)
{
  if (index == -1 &&                                        // if field delimiter isn't set...
      !m_imp->m_autodetect.value(AutoFieldDelimiter))  // ... and user disabled autodetecting...
    return;                                                 // ... then wait for him to choose
  else if (index == m_imp->m_profile->m_fieldDelimiterIndex)
    return;

  m_imp->m_profile->m_fieldDelimiterIndex = index;
  m_imp->m_file->readFile(m_imp->m_profile);      // get column count, we get with this fieldDelimiter
  m_imp->m_file->setupParser(m_imp->m_profile);

  if (index == -1) {
    ui->m_fieldDelimiter->blockSignals(true);
    ui->m_fieldDelimiter->setCurrentIndex(m_imp->m_profile->m_fieldDelimiterIndex);
    ui->m_fieldDelimiter->blockSignals(false);
  }
  m_dlg->updateWindowSize();
  emit completeChanged();
}

void SeparatorPage::textDelimiterChanged(const int index)
{
  if (index == -1) {                                  // if text delimiter isn't set...
    ui->m_textDelimiter->setCurrentIndex(0);        // ...then set it to 0, as for now there is no better idea how to detect it
    return;
  }

  m_imp->m_profile->m_textDelimiterIndex = index;
  m_imp->m_file->setupParser(m_imp->m_profile);

  if (index == -1) {
    ui->m_textDelimiter->blockSignals(true);
    ui->m_textDelimiter->setCurrentIndex(m_imp->m_profile->m_textDelimiterIndex);
    ui->m_textDelimiter->blockSignals(false);
  }
  emit completeChanged();
}

bool SeparatorPage::isComplete() const
{
  bool rc = false;
  if (ui->m_encoding->currentIndex() != -1 &&
      ui->m_fieldDelimiter->currentIndex() != -1 &&
      ui->m_textDelimiter->currentIndex() != -1) {
    switch(m_imp->m_profile->type()) {
      case ProfileBank:
        if (m_imp->m_file->m_columnCount > 2)
          rc = true;
        break;
      case ProfileInvest:
        if (m_imp->m_file->m_columnCount > 3)
          rc = true;
        break;
      case ProfileCurrencyPrices:
      case ProfileStockPrices:
        if (m_imp->m_file->m_columnCount > 1)
          rc = true;
        break;
      default:
        break;
    }
  }
  return rc;
}

bool SeparatorPage::validatePage()
{
  return true;
}

void SeparatorPage::cleanupPage()
{
  //  On completion with error force use of 'Back' button.
  //  ...to allow resetting of 'Skip setup'
  disconnect(ui->m_fieldDelimiter, SIGNAL(currentIndexChanged(int)), this, SLOT(fieldDelimiterChanged(int)));
  disconnect(ui->m_textDelimiter, SIGNAL(currentIndexChanged(int)), this, SLOT(textDelimiterChanged(int)));
  m_dlg->m_pageIntro->initializePage();  //  Need to show button(QWizard::CustomButton1) not 'NextButton'
}

RowsPage::RowsPage(CSVWizard *dlg, CSVImporter *imp) :
  CSVWizardPage(dlg, imp),
    ui(new Ui::RowsPage)
{
  ui->setupUi(this);
  m_pageLayout = new QVBoxLayout;
  ui->horizontalLayout->insertLayout(0, m_pageLayout);
}

RowsPage::~RowsPage()
{
  delete ui;
}

void RowsPage::initializePage()
{
//  disconnect(ui->m_startLine, &QSpinBox::valueChanged, this, &RowsPage::startRowChanged);
//  disconnect(ui->m_endLine, &QSpinBox::valueChanged, this, &RowsPage::endRowChanged);

  ui->m_startLine->setMaximum(m_imp->m_file->m_rowCount);
  ui->m_endLine->setMaximum(m_imp->m_file->m_rowCount);
  ui->m_startLine->setValue(m_imp->m_profile->m_startLine + 1);
  ui->m_endLine->setValue(m_imp->m_profile->m_endLine + 1);

  m_dlg->markUnwantedRows();
  m_dlg->m_vScrollBar->setValue(m_imp->m_profile->m_startLine);

  connect(ui->m_startLine, SIGNAL(valueChanged(int)), this, SLOT(startRowChanged(int)));;
  connect(ui->m_endLine, SIGNAL(valueChanged(int)), this, SLOT(endRowChanged(int)));

  QList<QWizard::WizardButton> layout;
  layout << QWizard::Stretch <<
            QWizard::BackButton <<
            QWizard::NextButton <<
            QWizard::CancelButton;
  wizard()->setButtonLayout(layout);
}

void RowsPage::cleanupPage()
{
  disconnect(ui->m_startLine, SIGNAL(valueChanged(int)), this, SLOT(startRowChanged(int)));;
  disconnect(ui->m_endLine, SIGNAL(valueChanged(int)), this, SLOT(endRowChanged(int)));
  m_dlg->clearBackground();
}

int RowsPage::nextId() const
{
  int ret;
  switch (m_imp->m_profile->type()) {
    case ProfileBank:
      ret = CSVWizard::PageBanking;
      break;
    case ProfileInvest:
      ret = CSVWizard::PageInvestment;
      break;
    case ProfileStockPrices:
    case ProfileCurrencyPrices:
      ret = CSVWizard::PagePrices;
      break;
    default:
      ret = CSVWizard::PageRows;
      break;
  }
  return ret;
}

void RowsPage::startRowChanged(int val)
{
  if (val > m_imp->m_file->m_rowCount) {
    ui->m_startLine->setValue(m_imp->m_file->m_rowCount);
    return;
  }
  --val;
  if (val > m_imp->m_profile->m_endLine) {
    ui->m_startLine->setValue(m_imp->m_profile->m_endLine);
    return;
  }
  m_imp->m_profile->m_startLine = val;
  m_dlg->m_vScrollBar->setValue(val);
  m_dlg->markUnwantedRows();
}

void RowsPage::endRowChanged(int val)
{
  if (val > m_imp->m_file->m_rowCount) {
    ui->m_endLine->setValue(m_imp->m_file->m_rowCount);
    return;
  }
  --val;
  if (val < m_imp->m_profile->m_startLine) {
    ui->m_endLine->setValue(m_imp->m_profile->m_startLine);
    return;
  }
  m_imp->m_profile->m_trailerLines = m_imp->m_file->m_rowCount - val;
  m_imp->m_profile->m_endLine = val;
  m_dlg->markUnwantedRows();
}





FormatsPage::FormatsPage(CSVWizard *dlg, CSVImporter *imp) :
  CSVWizardPage(dlg, imp),
    ui(new Ui::FormatsPage)
{
  ui->setupUi(this);
  m_pageLayout = new QVBoxLayout;
  ui->horizontalLayout->insertLayout(0, m_pageLayout);
}

FormatsPage::~FormatsPage()
{
  delete ui;
}

void FormatsPage::initializePage()
{
  disconnect(ui->comboBox_dateFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(dateFormatChanged(int)));
  disconnect(ui->comboBox_decimalSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(decimalSymbolChanged(int)));
  m_isDecimalSymbolOK = false;
  m_isDateFormatOK = false;
  QList<QWizard::WizardButton> layout;
  layout << QWizard::Stretch
         << QWizard::CustomButton3
         << QWizard::CustomButton2
         << QWizard::BackButton
         << QWizard::FinishButton
         << QWizard::CancelButton;
  wizard()->setOption(QWizard::HaveCustomButton2, true);
  wizard()->setButtonText(QWizard::CustomButton2, i18n("Import CSV"));
  wizard()->setOption(QWizard::HaveCustomButton3, true);
  wizard()->setButtonText(QWizard::CustomButton3, i18n("Make QIF File"));
  wizard()->setButtonLayout(layout);
  wizard()->button(QWizard::CustomButton2)->setEnabled(false);
  wizard()->button(QWizard::CustomButton3)->setEnabled(false);
  wizard()->button(QWizard::FinishButton)->setEnabled(false);

  ui->comboBox_thousandsDelimiter->setEnabled(false);

  ui->comboBox_dateFormat->setCurrentIndex(m_imp->m_profile->m_dateFormatIndex); // put before connect to not emit update signal
  connect(ui->comboBox_dateFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(dateFormatChanged(int)));
  emit ui->comboBox_dateFormat->currentIndexChanged(m_imp->m_profile->m_dateFormatIndex); // emit update signal manually regardless of change to combobox

  ui->comboBox_decimalSymbol->setCurrentIndex(m_imp->m_profile->m_decimalSymbolIndex);    // put before connect to not emit update signal
  connect(ui->comboBox_decimalSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(decimalSymbolChanged(int)));
  emit ui->comboBox_decimalSymbol->currentIndexChanged(m_imp->m_profile->m_decimalSymbolIndex); // emit update signal manually regardless of change to combobox

  if (m_dlg->m_skipSetup &&
      wizard()->button(QWizard::CustomButton2)->isEnabled())
    m_dlg->importClicked();
}

void FormatsPage::decimalSymbolChanged(int index)
{
  QList<int> columns = m_imp->getNumericalColumns();
  switch (index) {
    case -1:
      if (!m_imp->m_autodetect.value(AutoDecimalSymbol))
        break;
    case 2:
    {
      ui->comboBox_decimalSymbol->blockSignals(true);
      m_imp->m_profile->m_decimalSymbolIndex = 2;
      int failColumn = m_imp->detectDecimalSymbols(columns);
      if (failColumn != -2) {
        KMessageBox::sorry(this, i18n("<center>Autodetect could not detect your decimal symbol in column %1.</center>"
                                      "<center>Try manual selection to see problematic cells and correct your data.</center>", failColumn), i18n("CSV import"));
        ui->comboBox_decimalSymbol->setCurrentIndex(-1);
        ui->comboBox_thousandsDelimiter->setCurrentIndex(-1);
      } else if (index == -1) { // if detection went well and decimal symbol was unspecified then we'll be specifying it
        int firstDecSymbol = m_imp->m_decimalSymbolIndexMap.first();
        bool allSymbolsEqual = true;
        foreach (const auto mapDecSymbol, m_imp->m_decimalSymbolIndexMap) {
          if (firstDecSymbol != mapDecSymbol)
            allSymbolsEqual = false;
        }
        if (allSymbolsEqual) {   // if symbol in all columns is equal then set it...
          m_imp->m_profile->m_decimalSymbolIndex = firstDecSymbol;
          ui->comboBox_decimalSymbol->setCurrentIndex(firstDecSymbol);
          ui->comboBox_thousandsDelimiter->setCurrentIndex(firstDecSymbol);
        } else {  // else set to auto
          m_imp->m_profile->m_decimalSymbolIndex = 2;
          ui->comboBox_decimalSymbol->setCurrentIndex(2);
          ui->comboBox_thousandsDelimiter->setCurrentIndex(2);
        }
      }
      ui->comboBox_decimalSymbol->blockSignals(false);
      break;
    }
    default:
      foreach (const auto column, columns)
        m_imp->m_decimalSymbolIndexMap.insert(column, index);
      ui->comboBox_thousandsDelimiter->setCurrentIndex(index);
      m_imp->m_profile->m_decimalSymbolIndex = index;
  }

  m_isDecimalSymbolOK = validateDecimalSymbols(columns);
  emit completeChanged();
}

bool FormatsPage::validateDecimalSymbols(const QList<int> &columns)
{
  bool isOK = true;
  foreach (const auto col, columns) {
    m_imp->m_file->m_parse->setDecimalSymbol(m_imp->m_decimalSymbolIndexMap.value(col));
    m_imp->m_file->m_parse->setThousandsSeparator(m_imp->m_decimalSymbolIndexMap.value(col));
    m_dlg->clearColumnsBackground(col);
    for (int row = m_imp->m_profile->m_startLine; row <= m_imp->m_profile->m_endLine; ++row) {
      QStandardItem *item = m_imp->m_file->m_model->item(row, col);
      QString rawNumber = item->text();
       m_imp->m_file->m_parse->possiblyReplaceSymbol(rawNumber);
      if (!m_imp->m_file->m_parse->invalidConversion() ||
          rawNumber.isEmpty()) {                   // empty strings are welcome
        item->setBackground(m_dlg->m_colorBrush);
        item->setForeground(m_dlg->m_colorBrushText);
      } else {
        isOK = false;
        m_dlg->ui->tableView->scrollTo(item->index(), QAbstractItemView::EnsureVisible);
        item->setBackground(m_dlg->m_errorBrush);
        item->setForeground(m_dlg->m_errorBrushText);
      }
    }

  }
  return isOK;
}

void FormatsPage::dateFormatChanged(const int index)
{
  if (index == -1)
    return;

  int col = m_imp->m_profile->m_colTypeNum.value(ColumnDate);
  m_imp->m_profile->m_dateFormatIndex = index;
  m_imp->m_convertDate->setDateFormatIndex(index);
  m_isDateFormatOK = validateDateFormat(col);
  if (!m_isDateFormatOK) {
    KMessageBox::sorry(this, i18n("<center>There are invalid date formats in column '%1'.</center>"
                                  "<center>Please check your selections.</center>"
                                  , col + 1), i18n("CSV import"));
  }
  emit completeChanged();
}

bool FormatsPage::validateDateFormat(const int col)
{
  m_dlg->clearColumnsBackground(col);

  bool isOK = true;
  for (int row = m_imp->m_profile->m_startLine; row <= m_imp->m_profile->m_endLine; ++row) {
      QStandardItem* item = m_imp->m_file->m_model->item(row, col);
      QDate dat = m_imp->m_convertDate->convertDate(item->text());
      if (dat == QDate()) {
        isOK = false;
        m_dlg->ui->tableView->scrollTo(item->index(), QAbstractItemView::EnsureVisible);
        item->setBackground(m_dlg->m_errorBrush);
        item->setForeground(m_dlg->m_errorBrushText);
      } else {
        item->setBackground(m_dlg->m_colorBrush);
        item->setForeground(m_dlg->m_colorBrushText);
      }
  }
  return isOK;
}

bool FormatsPage::isComplete() const
{
  const bool enable = m_isDecimalSymbolOK && m_isDateFormatOK;
  wizard()->button(QWizard::CustomButton2)->setEnabled(enable);
  if (m_imp->m_profile->type() != ProfileStockPrices &&
      m_imp->m_profile->type() != ProfileCurrencyPrices)
    wizard()->button(QWizard::CustomButton3)->setEnabled(enable);
  return enable;
}

void FormatsPage::cleanupPage()
{
  disconnect(ui->comboBox_dateFormat, SIGNAL(currentIndexChanged(int)), this, SLOT(dateFormatChanged(int)));
  disconnect(ui->comboBox_decimalSymbol, SIGNAL(currentIndexChanged(int)), this, SLOT(decimalSymbolChanged(int)));

  QList<int> columns = m_imp->getNumericalColumns();
  columns.append(m_imp->m_profile->m_colTypeNum.value(ColumnDate));
  m_dlg->clearColumnsBackground(columns);
  m_dlg->m_st = MyMoneyStatement();  // any change on investment/banking page invalidates created statement

  QList<QWizard::WizardButton> layout;
  layout << QWizard::Stretch <<
            QWizard::BackButton <<
            QWizard::NextButton <<
            QWizard::CancelButton;
  wizard()->setButtonLayout(layout);
}
