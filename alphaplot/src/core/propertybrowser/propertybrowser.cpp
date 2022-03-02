/* This file is part of AlphaPlot.
   Copyright 2022, Arun Narayanankutty <n.arun.lifescience@gmail.com>

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

   Description : PropertyBrowser */

#include "propertybrowser.h"

#include <QDebug>

#include "2Dplot/Layout2D.h"
#include "3Dplot/Layout3D.h"
#include "Matrix.h"
#include "MyWidget.h"
#include "Note.h"
#include "Table.h"
#include "core/propertybrowser/ObjectBrowserTreeItemModel.h"
#include "ui_propertybrowser.h"

PropertyBrowser::PropertyBrowser(QWidget *parent, ApplicationWindow *app)
    : QDockWidget(parent), app_(app), ui_(new Ui_PropertyBrowser) {
  Q_ASSERT(app_);
  ui_->setupUi(this);
  setWindowTitle(tr("Property Browser"));
  setWindowIcon(QIcon());

  ui_->dockWidgetContents->layout()->setContentsMargins(0, 0, 0, 0);
  ui_->objectView->setFrameShape(QFrame::NoFrame);
  ui_->propertyView->setFrameShape(QFrame::NoFrame);
  ui_->objectView->setContextMenuPolicy(Qt::CustomContextMenu);
  ui_->objectView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui_->propertyView->setSelectionMode(QAbstractItemView::SingleSelection);

  ui_->propertyView->setFocusPolicy(Qt::NoFocus);
  connect(ui_->objectView, &QTreeView::customContextMenuRequested, this,
          &PropertyBrowser::showObjectContextMenu);

  /*connect(ui_->objectView->model(),
          SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
          SLOT(selectObjectItem(QTreeWidgetItem *)));
  connect(objectbrowser_, &MyTreeWidget::activate,
          [=](MyWidget *widget) { app_->activateWindow(widget); });
  connect(objectbrowser_, &MyTreeWidget::itemRootContextMenuRequested, app_,
          &ApplicationWindow::showWindowTitleBarMenu);
  connect(objectbrowser_, &MyTreeWidget::itemContextMenuRequested, app_,
          &ApplicationWindow::itemContextMenuRequested);
  connect(boolManager_, SIGNAL(valueChanged(QtProperty *, bool)), this,
          SLOT(valueChange(QtProperty *, const bool &)));
  connect(colorManager_, SIGNAL(valueChanged(QtProperty *, QColor)), this,
          SLOT(valueChange(QtProperty *, const QColor &)));
  connect(rectManager_, SIGNAL(valueChanged(QtProperty *, QRect)), this,
          SLOT(valueChange(QtProperty *, const QRect &)));
  connect(doubleManager_, SIGNAL(valueChanged(QtProperty *, double)), this,
          SLOT(valueChange(QtProperty *, const double &)));
  connect(stringManager_, SIGNAL(valueChanged(QtProperty *, QString)), this,
          SLOT(valueChange(QtProperty *, const QString &)));
  connect(intManager_, SIGNAL(valueChanged(QtProperty *, int)), this,
          SLOT(valueChange(QtProperty *, const int)));
  connect(enumManager_, SIGNAL(valueChanged(QtProperty *, int)), this,
          SLOT(enumValueChange(QtProperty *, const int)));
  connect(fontManager_, SIGNAL(valueChanged(QtProperty *, QFont)), this,
          SLOT(valueChange(QtProperty *, const QFont &)));
  connect(datetimeManager_, &QtDateTimePropertyManager::valueChanged, this,
          &PropertyEditor::datetimeValueChange);
  connect(sizeManager_, SIGNAL(valueChanged(QtProperty *, QSize)), this,
          SLOT(valueChange(QtProperty *, const QSize &)));
  connect(this, &PropertyEditor::refreshCanvasRect, [=]() {
    QTreeWidgetItem *item = objectbrowser_->currentItem();
    if (item && static_cast<MyTreeWidget::PropertyItemType>(
                    item->data(0, Qt::UserRole).value<int>()) ==
                    MyTreeWidget::PropertyItemType::Plot2DCanvas) {
      Plot2D *plotcanvas =
          getgraph2dobject<Plot2D>(objectbrowser_->currentItem());
      sizeManager_->setValue(canvaspropertysizeitem_,
                             QSize(plotcanvas->geometry().width(),
                                   plotcanvas->geometry().height()));
    }
    if (item && static_cast<MyTreeWidget::PropertyItemType>(
                    item->data(0, Qt::UserRole).value<int>()) ==
                    MyTreeWidget::PropertyItemType::Plot2DLayout) {
      AxisRect2D *axisrect =
          getgraph2dobject<AxisRect2D>(objectbrowser_->currentItem());
      rectManager_->setValue(layoutpropertyrectitem_, axisrect->outerRect());
    }
  });
  connect(this, &PropertyEditor::rescaleAxis2D, [=](Axis2D *axis) {
    QTreeWidgetItem *item = objectbrowser_->currentItem();
    if (item && static_cast<MyTreeWidget::PropertyItemType>(
                    item->data(0, Qt::UserRole).value<int>()) ==
                    MyTreeWidget::PropertyItemType::Plot2DAxis) {
      Axis2D *ax = getgraph2dobject<Axis2D>(objectbrowser_->currentItem());
      if (ax == axis && ax) {
        switch (ax->gettickertype_axis()) {
          case Axis2D::TickerType::Text:
          case Axis2D::TickerType::Log:
          case Axis2D::TickerType::Value:
          case Axis2D::TickerType::Pi:
          case Axis2D::TickerType::Time:
            doubleManager_->setValue(axispropertyfromitem_,
                                     axis->getfrom_axis());
            doubleManager_->setValue(axispropertytoitem_, axis->getto_axis());
            break;
          case Axis2D::TickerType::DateTime:
            datetimeManager_->setValue(
                axispropertytickerdatetimefrom_,
                QCPAxisTickerDateTime::keyToDateTime(axis->getfrom_axis()));
            datetimeManager_->setValue(
                axispropertytickerdatetimeto_,
                QCPAxisTickerDateTime::keyToDateTime(axis->getto_axis()));
            break;
        }
      }
    }
  });*/
}

PropertyBrowser::~PropertyBrowser() { delete ui_; }

void PropertyBrowser::populateObjectBrowser(MyWidget *widget) {
  if (widget == nullptr) {
    ui_->objectView->setModel(app_->getObjectModel());
  } else if (qobject_cast<Table *>(widget)) {
    Table *table = qobject_cast<Table *>(widget);
    ui_->objectView->setModel(table->getObjectModel());
  } else if (qobject_cast<Matrix *>(widget)) {
    Matrix *matrix = qobject_cast<Matrix *>(widget);
    ui_->objectView->setModel(matrix->getObjectModel());
  } else if (qobject_cast<Note *>(widget)) {
    Note *note = qobject_cast<Note *>(widget);
    ui_->objectView->setModel(note->getObjectModel());
  } else if (qobject_cast<Layout2D *>(widget)) {
    Layout2D *gd = qobject_cast<Layout2D *>(widget);
    ui_->objectView->setModel(gd->getObjectModel());
  } else if (qobject_cast<Layout3D *>(widget)) {
    Layout3D *layout = qobject_cast<Layout3D *>(widget);
    ui_->objectView->setModel(layout->getObjectModel());
  }
}

void PropertyBrowser::showObjectContextMenu(const QPoint &point) {
  QModelIndex index = ui_->objectView->indexAt(point);
  // QPoint globalPos = ui_->objectView->viewport()->mapToGlobal(point);
  if (!index.isValid()) return;
  ObjectBrowserTreeItem *item =
      static_cast<ObjectBrowserTreeItem *>(index.internalPointer());
  switch (item->dataType()) {
    case ObjectBrowserTreeItem::ObjectType::BaseWindow: {
      ObjectBrowserTreeItem *parentitem = item->parentItem();
      switch (parentitem->dataType()) {
        case ObjectBrowserTreeItem::ObjectType::TableWindow:
        case ObjectBrowserTreeItem::ObjectType::MatrixWindow:
        case ObjectBrowserTreeItem::ObjectType::NoteWindow:
        case ObjectBrowserTreeItem::ObjectType::Plot2DWindow:
        case ObjectBrowserTreeItem::ObjectType::Plot3DWindow:
          app_->showWindowTitleBarMenu();
          return;
          break;
        default:
          return;
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DLayout: {
      Layout2D *layout = item->parentItem()
                             ->data(Qt::ItemDataRole::UserRole)
                             .value<Layout2D *>();
      AxisRect2D *axisrect =
          item->data(Qt::ItemDataRole::UserRole).value<AxisRect2D *>();
      if (!axisrect || !layout) return;
      app_->itemContextMenuRequested(layout, axisrect);
      return;
    } break;
    default:
      break;
  }
}
