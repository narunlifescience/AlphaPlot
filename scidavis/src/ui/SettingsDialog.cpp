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
   
   Description : central settings dialog
*/

#include "aSettingsListView.h"
#include "SettingsDialog.h"
#include "SettingsPage.h"
#include "ui_SettingsDialog.h"
#include "ApplicationSettingsPage.h"
#include "GeneralAppreanceSettings.h"

#include <QStandardItemModel>
#include <QStandardItem>
#include <QDebug>

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
  setWindowIcon(QIcon());
  setModal(true);
  setMinimumSize(sizeHint());

  // Colors (TODO: use some central qpalette handling)
  baseColor_ = palette().color(QPalette::Base);
  fontColor_ = palette().color(QPalette::Text);

  // adjust layout spacing & margins.
  ui_->settingGridLayout->setSpacing(3);
  ui_->settingGridLayout->setContentsMargins(0, 0, 0, 0);
  ui_->headerHorizontalLayout->setSpacing(0);
  ui_->scrollVerticalLayout->setSpacing(0);
  ui_->scrollVerticalLayout->setContentsMargins(3, 3, 3, 3);
  ui_->scrollVerticalLayout->setContentsMargins(0, 0, 0, 0);
  ui_->stackGridLayout->setSpacing(0);
  ui_->stackGridLayout->setContentsMargins(0, 0, 0, 0);

  // Setup search box.
  ui_->searchBox->setMaximumWidth(300);
  ui_->searchBox->setToolTip(tr("search"));
  // ui->searchBox->setStyle();

  // Prepare buttons
  ui_->configureButton->setIcon(QIcon(":/data/gtk-preferences.svg"));
  ui_->settingsButton->setIcon(QIcon(":/data/go-previous.svg"));
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
  ui_->configureButton->setMenu(settingsViewMenu_);

  // Prepare scrollarea.
  QString scrollbackcol =
      "QScrollArea {background-color : "
      "rgba(%1,%2,%3,%4); border: 0;}";
  ui_->scrollArea->setStyleSheet(scrollbackcol.arg(baseColor_.red())
                                     .arg(baseColor_.green())
                                     .arg(baseColor_.blue())
                                     .arg(baseColor_.alpha()));

  // Prepare label.
  QString label_font_color =
      "QLabel {color : rgba(%1,%2,%3,%4);"
      " padding-left: 10px;"
      " padding-right: 10px;"
      " padding-top: 10px;"
      " padding-bottom: 10px}";
  ui_->generalLabel->setStyleSheet(label_font_color.arg(fontColor_.red())
                                       .arg(fontColor_.green())
                                       .arg(fontColor_.blue())
                                       .arg(150));
  ui_->plot2dLabel->setStyleSheet(label_font_color.arg(fontColor_.red())
                                      .arg(fontColor_.green())
                                      .arg(fontColor_.blue())
                                      .arg(150));
  ui_->generalLabel->hide();
  ui_->tableLabel->hide();
  ui_->plot2dLabel->hide();
  ui_->plot3dLabel->hide();
  ui_->fittingLabel->hide();
  ui_->scriptingLabel->hide();

  // Add pages to stack widget
  addPage(General, Page_GeneralApplication, new ApplicationSettingsPage(this));
  addPage(General, Page_GeneralConfirmation, new ApplicationSettingsPage(this));
  addPage(General, Page_GeneralAppearance, new GeneralAppreanceSettings(this));

  // Make standard item models for listviews & add items
  QStandardItemModel* iStandardModel = new QStandardItemModel(this);
  QStandardItem* item1 =
      new QStandardItem(QIcon(":/data/document-open-remote.png"), "Open");
  QStandardItem* item2 =
      new QStandardItem(QIcon(":/data/document-save.png"), "Save");
  QStandardItem* item3 =
      new QStandardItem(QIcon(":/data/drive-removable-media-usb-pendrive.png"),
                        "Removable Drive");
  QStandardItem* item4 =
      new QStandardItem(QIcon(":/data/download.png"), "Download");
  QStandardItem* item5 =
      new QStandardItem(QIcon(":/data/ipodtouchicon.png"), "Ipod Touch Device");
  QStandardItem* item6 = new QStandardItem(
      QIcon(":/data/multimedia-player-ipod-mini-pink.png"), "Ipod Mini");
  QStandardItem* item7 = new QStandardItem(QIcon(":/data/qtlogo.png"), "Qt");
  iStandardModel->appendRow(item1);
  iStandardModel->appendRow(item2);
  iStandardModel->appendRow(item3);
  iStandardModel->appendRow(item4);
  iStandardModel->appendRow(item5);
  iStandardModel->appendRow(item6);
  iStandardModel->appendRow(item7);

  // Set model to the view
  ui_->generalListView->setModel(generalSettingsModel_);
  ui_->tableListView->setModel(tableSettingsModel_);
  ui_->plot2dListView->setModel(iStandardModel);
  ui_->plot3dListView->setModel(plot3dSettingsModel_);
  ui_->fittingListView->setModel(fittingSettingsModel_);
  ui_->scriptingListView->setModel(scriptingSettingsModel_);

  // Signals & slot connections
  connect(ui_->settingsButton, SIGNAL(clicked()), this,
          SLOT(getBackToRootSettingsPage()));
  connect(ui_->generalListView, SIGNAL(clicked(const QModelIndex&)), this,
          SLOT(generalEnsureSelection(const QModelIndex&)));
  connect(ui_->plot2dListView, SIGNAL(clicked(const QModelIndex&)), this,
          SLOT(plot2dEnsureSelection(const QModelIndex&)));
  connect(ui_->generalListView, SIGNAL(doubleClicked(QModelIndex)), this,
          SLOT(test(QModelIndex)));
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
      page->setTitle(tr("General ") + page->windowTitle());
      if (ui_->generalLabel->isHidden()) ui_->generalLabel->show();
      break;
    case Table:
      tableSettingsModel_->appendRow(item);
      page->setTitle(tr("Table ") + page->windowTitle());
      if (ui_->tableLabel->isHidden()) ui_->tableLabel->show();
      break;
    case Plot2d:
      plot2dSettingsModel_->appendRow(item);
      page->setTitle(tr("2DPlot ") + page->windowTitle());
      if (ui_->plot2dLabel->isHidden()) ui_->plot2dLabel->show();
      break;
    case Plot3d:
      plot3dSettingsModel_->appendRow(item);
      page->setTitle(tr("3DPlot ") + page->windowTitle());
      if (ui_->plot3dLabel->isHidden()) ui_->plot3dLabel->show();
      break;
    case Fitting:
      fittingSettingsModel_->appendRow(item);
      page->setTitle(tr("Fitting ") + page->windowTitle());
      if (ui_->fittingLabel->isHidden()) ui_->fittingLabel->show();
      break;
    case Scripting:
      scriptingSettingsModel_->appendRow(item);
      page->setTitle(tr("Scripting ") + page->windowTitle());
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
}

void SettingsDialog::handleResizeBeforeListviewVisible(aListView*& listView) {
  if (listView->model()->rowCount() > 0) {
    listView->setFixedHeight(
        ((((listView->model()->rowCount() * listView->gridSize().width()) /
           (width() - 50)) +
          1) *
         listView->gridSize().width()) +
        aListView::a_listview_height_padding);
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
  ui_->stackedWidget->setCurrentIndex(Page_RootSettings);
  ui_->searchBox->show();
  ui_->configureButton->show();
  ui_->settingsButton->setEnabled(false);
}

void SettingsDialog::test(QModelIndex mod) {
  qDebug() << "sugnal recived " << mod.row() << mod.data().toString();
  ui_->stackedWidget->setCurrentIndex(mod.row() + 1);
  ui_->searchBox->hide();
  ui_->configureButton->hide();
  ui_->settingsButton->setEnabled(true);
}

// when ported to QT5, use lambda functions insted of slots.
// Clear Multiple selection & ensure single item selection
void SettingsDialog::generalEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->generalListView->setCurrentIndex(index);
}

void SettingsDialog::tableEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->tableListView->setCurrentIndex(index);
}

void SettingsDialog::plot2dEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->plot2dListView->setCurrentIndex(index);
}

void SettingsDialog::plot3dEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->plot3dListView->setCurrentIndex(index);
}

void SettingsDialog::fittingEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->fittingListView->setCurrentIndex(index);
}

void SettingsDialog::scriptingEnsureSelection(const QModelIndex& index) {
  clearAllSelection();
  ui_->scriptingListView->setCurrentIndex(index);
}
