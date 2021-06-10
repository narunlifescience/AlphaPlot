/* This file is part of AlphaPlot.
   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : central settings dialog */

#include "SettingsDialog.h"

#include <QDebug>
#include <QStandardItem>
#include <QStandardItemModel>

#include "../core/IconLoader.h"
#include "../widgets/aSettingsListView.h"
#include "FittingSettings.h"
#include "GeneralApplicationSettings.h"
#include "GeneralAppreanceSettings.h"
#include "GeneralConfirmationSettings.h"
#include "GeneralNumericFormatSettings.h"
#include "SettingsPage.h"
#include "TableBasicSettings.h"
#include "TableColorSettings.h"
#include "TableFontSettings.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent),
      ui_(new Ui_SettingsDialog),
      settingsViewMenu_(new QMenu(this)),
      settingsViewGroup_(new QActionGroup(this)),
      treeView_(new QAction(this)),
      iconView_(new QAction(this)),
      generalSettingsModel_(new QStandardItemModel(this)),
      tableSettingsModel_(new QStandardItemModel(this)),
      plot2dSettingsModel_(new QStandardItemModel(this)),
      plot3dSettingsModel_(new QStandardItemModel(this)),
      fittingSettingsModel_(new QStandardItemModel(this)),
      scriptingSettingsModel_(new QStandardItemModel(this)) {
  ui_->setupUi(this);
  setWindowTitle(tr("Settings"));
  setWindowIcon(IconLoader::load("edit-preference", IconLoader::LightDark));
  setModal(true);
  setMinimumSize(sizeHint());

  // Colors (TODO: use some central qpalette handling)
  baseColor_ = qApp->palette().color(QPalette::Base);
  fontColor_ = qApp->palette().color(QPalette::Text);

  // adjust layout spacing & margins.
  ui_->settingGridLayout->setSpacing(3);
  ui_->settingGridLayout->setContentsMargins(0, 0, 0, 0);
  ui_->headerHorizontalLayout->setSpacing(0);
  ui_->scrollVerticalLayout->setSpacing(0);
  ui_->scrollVerticalLayout->setContentsMargins(3, 3, 3, 3);
  ui_->scrollVerticalLayout->setContentsMargins(0, 0, 0, 0);
  ui_->stackGridLayout->setSpacing(0);
  ui_->stackGridLayout->setContentsMargins(0, 0, 0, 0);
  ui_->scrollAreaWidgetContents->setContentsMargins(0, 0, 0, 0);
  // ui_->scrollAreaWidgetContents->setSpacing(0);

  // Setup search box.
  ui_->searchBox->setMaximumWidth(300);
  ui_->searchBox->setToolTip(tr("search"));

  // Prepare buttons
  ui_->settingsButton->setIcon(
      IconLoader::load("go-previous", IconLoader::LightDark));
  ui_->settingsButton->setEnabled(false);

  // Add settings configure menu items(tree/icon view)
  settingsViewGroup_->addAction(treeView_);
  settingsViewGroup_->addAction(iconView_);
  treeView_->setText(tr("Tree view"));
  iconView_->setText(tr("Icon view"));
  treeView_->setCheckable(true);
  iconView_->setCheckable(true);
  settingsViewMenu_->addAction(treeView_);
  settingsViewMenu_->addAction(iconView_);

  // Prepare scrollarea.
  QString scrollbackcol =
      ".QScrollArea {border: 0; background-color: rgba(%1,%2,%3,%4);}";
  ui_->scrollArea->setStyleSheet(scrollbackcol.arg(baseColor_.red())
                                     .arg(baseColor_.green())
                                     .arg(baseColor_.blue())
                                     .arg(baseColor_.alpha()));
  ui_->settingsButton->setStyleSheet(
      "QPushButton {background-color : rgba(0,0,0,0);border: 0 "
      "rgba(0,0,0,0);}");

  // Prepare label.
  QString label_font_color =
      "QLabel {background-color : rgba(0,0,0,0);"
      " padding-left: 10px;"
      " padding-right: 10px;"
      " padding-top: 10px;"
      " padding-bottom: 10px; border: 0 rgba(0,0,0,0);}";
  ui_->generalLabel->setStyleSheet(label_font_color);
  ui_->tableLabel->setStyleSheet(label_font_color);
  ui_->plot2dLabel->setStyleSheet(label_font_color);
  ui_->plot3dLabel->setStyleSheet(label_font_color);
  ui_->fittingLabel->setStyleSheet(label_font_color);
  ui_->scriptingLabel->setStyleSheet(label_font_color);
  ui_->generalLabel->hide();
  ui_->tableLabel->hide();
  ui_->plot2dLabel->hide();
  ui_->plot3dLabel->hide();
  ui_->fittingLabel->hide();
  ui_->scriptingLabel->hide();

  // Add pages to stack widget
  // addPage(General, Page_RootSettings, new ApplicationSettingsPage(this));
  ApplicationSettingsPage* generalapplicationsettings =
      new ApplicationSettingsPage(this);
  GeneralConfirmationSettings* generalconfirmationsettings =
      new GeneralConfirmationSettings(this);
  GeneralAppreanceSettings* generalappearancesettings =
      new GeneralAppreanceSettings(this);
  GeneralNumericFormatSettings* generalnumericformatsettings =
      new GeneralNumericFormatSettings(this);
  TableBasicSettings* tablebasicsettings = new TableBasicSettings(this);
  TableColorSettings* tablecolorsettings = new TableColorSettings(this);
  TableFontSettings* tablefontsettings = new TableFontSettings(this);
  FittingSettings* fittingsettings = new FittingSettings(this);
  addPage(General, Page_GeneralApplication, generalapplicationsettings);
  addPage(General, Page_GeneralConfirmation, generalconfirmationsettings);
  addPage(General, Page_GeneralAppearance, generalappearancesettings);
  addPage(General, Page_GeneralNumericFormat, generalnumericformatsettings);
  addPage(Table, Page_TableBasic, tablebasicsettings);
  addPage(Table, Page_TableColor, tablecolorsettings);
  addPage(Table, Page_TableFont, tablefontsettings);
  addPage(Fitting, Page_Fitting, fittingsettings);

  connect(generalapplicationsettings,
          &ApplicationSettingsPage::generalapplicationsettingsupdate, this,
          &SettingsDialog::generalapplicationsettingsupdates);
  connect(generalconfirmationsettings,
          &GeneralConfirmationSettings::generalconfirmationsettingsupdate, this,
          &SettingsDialog::generalconfirmationsettingsupdates);
  connect(generalappearancesettings,
          &GeneralAppreanceSettings::generalappreancesettingsupdate, this,
          &SettingsDialog::generalappreancesettingsupdates);
  connect(
      generalappearancesettings,
      &GeneralAppreanceSettings::generalappreancesettingsupdate, this, [=]() {
        baseColor_ = ui_->searchBox->palette().color(QPalette::Base);
        fontColor_ = palette().color(QPalette::Text);
        QString scrollbackcol =
            ".QScrollArea {border: 0; background-color: rgba(%1,%2,%3,%4);}";
        ui_->scrollArea->setStyleSheet(scrollbackcol.arg(baseColor_.red())
                                           .arg(baseColor_.green())
                                           .arg(baseColor_.blue())
                                           .arg(baseColor_.alpha()));
      });
  connect(generalnumericformatsettings,
          &GeneralNumericFormatSettings::generalnumericformatsettingsupdate,
          this, &SettingsDialog::generalnumericformatsettingsupdates);
  connect(tablebasicsettings, &TableBasicSettings::tablebasicsettingsupdate,
          this, &SettingsDialog::tablebasicsettingsupdates);
  connect(tablecolorsettings, &TableColorSettings::tablecolorsettingsupdate,
          this, &SettingsDialog::tablecolorsettingsupdates);
  connect(tablefontsettings, &TableFontSettings::tablefontsettingsupdate,
          this, &SettingsDialog::tablefontsettingsupdates);

  // Set model to the view
  ui_->generalListView->setModel(generalSettingsModel_);
  ui_->tableListView->setModel(tableSettingsModel_);
  ui_->plot2dListView->setModel(plot2dSettingsModel_);
  ui_->plot3dListView->setModel(plot3dSettingsModel_);
  ui_->fittingListView->setModel(fittingSettingsModel_);
  ui_->scriptingListView->setModel(scriptingSettingsModel_);

  // Signals & slot connections
  connect(ui_->settingsButton, SIGNAL(clicked()), this,
          SLOT(getBackToRootSettingsPage()));
  connect(ui_->generalListView, SIGNAL(clicked(const QModelIndex&)), this,
          SLOT(generalEnsureSelection(const QModelIndex&)));
  connect(ui_->tableListView, SIGNAL(clicked(const QModelIndex&)), this,
          SLOT(tableEnsureSelection(const QModelIndex&)));
  connect(ui_->plot2dListView, SIGNAL(clicked(const QModelIndex&)), this,
          SLOT(plot2dEnsureSelection(const QModelIndex&)));
  connect(ui_->plot3dListView, SIGNAL(clicked(const QModelIndex&)), this,
          SLOT(plot3dEnsureSelection(const QModelIndex&)));
  connect(ui_->fittingListView, SIGNAL(clicked(const QModelIndex&)), this,
          SLOT(fittingEnsureSelection(const QModelIndex&)));
  connect(ui_->scriptingListView, SIGNAL(clicked(const QModelIndex&)), this,
          SLOT(scriptingEnsureSelection(const QModelIndex&)));
}

SettingsDialog::~SettingsDialog() { delete ui_; }

QSize SettingsDialog::sizeHint() const { return QSize(610, 420); }

void SettingsDialog::resizeEvent(QResizeEvent*) {
  // Adjust listviews height according to icon & grid size.
  // Before listview becomes visible, we need to handle
  // resize here.
  // Pretty nasty hacks but seems to work..
  if (!ui_->generalListView->autoAdjustHeight()) {
    handleResizeBeforeListviewVisible(ui_->generalListView);
  }
  if (!ui_->tableListView->autoAdjustHeight()) {
    handleResizeBeforeListviewVisible(ui_->tableListView);
  }
  if (!ui_->plot2dListView->autoAdjustHeight()) {
    handleResizeBeforeListviewVisible(ui_->plot2dListView);
  }
  if (!ui_->plot3dListView->autoAdjustHeight()) {
    handleResizeBeforeListviewVisible(ui_->plot3dListView);
  }
  if (!ui_->fittingListView->autoAdjustHeight()) {
    handleResizeBeforeListviewVisible(ui_->fittingListView);
  }
  if (!ui_->scriptingListView->autoAdjustHeight()) {
    handleResizeBeforeListviewVisible(ui_->scriptingListView);
  }
}

void SettingsDialog::addPage(Catagory catogory, Page id, SettingsPage* page) {
  QStandardItem* item = new QStandardItem;
  item->setText(page->windowTitle());
  item->setIcon(page->windowIcon());

  switch (catogory) {
    case General:
      generalSettingsModel_->appendRow(item);
      if (ui_->generalLabel->isHidden()) ui_->generalLabel->show();
      break;
    case Table:
      tableSettingsModel_->appendRow(item);
      if (ui_->tableLabel->isHidden()) ui_->tableLabel->show();
      break;
    case Plot2d:
      plot2dSettingsModel_->appendRow(item);
      if (ui_->plot2dLabel->isHidden()) ui_->plot2dLabel->show();
      break;
    case Plot3d:
      plot3dSettingsModel_->appendRow(item);
      if (ui_->plot3dLabel->isHidden()) ui_->plot3dLabel->show();
      break;
    case Fitting:
      fittingSettingsModel_->appendRow(item);
      if (ui_->fittingLabel->isHidden()) ui_->fittingLabel->show();
      break;
    case Scripting:
      scriptingSettingsModel_->appendRow(item);
      if (ui_->scriptingLabel->isHidden()) ui_->scriptingLabel->show();
      break;
    default:
      // Will never reach
      qDebug() << "Settings page item not added";
      delete item;
      item = 0;
      break;
  }

  // Add page to stackwidget at position id
  ui_->stackedWidget->insertWidget(id, page);
  // ui_->stackedWidget->addWidget(page);
}

void SettingsDialog::handleResizeBeforeListviewVisible(
    aSettingsListView*& listView) {
  if (listView->model()->rowCount() > 0) {
    listView->setFixedHeight(
        ((((listView->model()->rowCount() * listView->gridSize().width()) /
           (width() - 50)) +
          1) *
         listView->gridSize().width()) +
        aSettingsListView::listViewHeightPadding_);
  } else {
    listView->setFixedHeight(0);
  }
}

void SettingsDialog::clearAllSelection() {
  ui_->generalListView->clearSelection();
  ui_->tableListView->clearSelection();
  ui_->plot2dListView->clearSelection();
  ui_->plot3dListView->clearSelection();
  ui_->fittingListView->clearSelection();
  ui_->scriptingListView->clearSelection();
}

void SettingsDialog::getBackToRootSettingsPage() {
  QWidget* w = ui_->stackedWidget->currentWidget();
  SettingsPage* sp = dynamic_cast<SettingsPage*>(w);
  bool result = true;
  if (sp) {
    result = sp->settingsChangeCheck();
  }
  if (result) {
    ui_->stackedWidget->setCurrentIndex(Page_RootSettings);
    ui_->searchBox->show();
    ui_->settingsButton->setEnabled(false);
  }
}

// when ported to QT5, use lambda functions insted of slots.
// Clear Multiple selection & ensure single item selection
void SettingsDialog::generalEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->generalListView->setCurrentIndex(index);
  ui_->stackedWidget->setCurrentIndex(index.row() + 1);
  ui_->searchBox->hide();
  ui_->settingsButton->setEnabled(true);
}

void SettingsDialog::tableEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->tableListView->setCurrentIndex(index);
  ui_->stackedWidget->setCurrentIndex(index.row() +
                                      generalSettingsModel_->rowCount() + 1);
  ui_->searchBox->hide();
  ui_->settingsButton->setEnabled(true);
}

void SettingsDialog::plot2dEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->plot2dListView->setCurrentIndex(index);
  ui_->stackedWidget->setCurrentIndex(index.row() +
                                      generalSettingsModel_->rowCount() +
                                      tableSettingsModel_->rowCount() + 1);
  ui_->searchBox->hide();
  ui_->settingsButton->setEnabled(true);
}

void SettingsDialog::plot3dEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->plot3dListView->setCurrentIndex(index);
  ui_->stackedWidget->setCurrentIndex(
      index.row() + generalSettingsModel_->rowCount() +
      tableSettingsModel_->rowCount() + plot2dSettingsModel_->rowCount() + 1);
  ui_->searchBox->hide();
  ui_->settingsButton->setEnabled(true);
}

void SettingsDialog::fittingEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->fittingListView->setCurrentIndex(index);
  ui_->stackedWidget->setCurrentIndex(
      index.row() + generalSettingsModel_->rowCount() +
      tableSettingsModel_->rowCount() + plot2dSettingsModel_->rowCount() +
      plot3dSettingsModel_->rowCount() + 1);
  ui_->searchBox->hide();
  ui_->settingsButton->setEnabled(true);
}

void SettingsDialog::scriptingEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->scriptingListView->setCurrentIndex(index);
  ui_->stackedWidget->setCurrentIndex(
      index.row() + generalSettingsModel_->rowCount() +
      tableSettingsModel_->rowCount() + plot2dSettingsModel_->rowCount() +
      plot3dSettingsModel_->rowCount() + fittingSettingsModel_->rowCount() + 1);
  ui_->searchBox->hide();
  ui_->settingsButton->setEnabled(true);
}
