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

#include <QAction>
#include <QDebug>

#include "2Dplot/Bar2D.h"
#include "2Dplot/Channel2D.h"
#include "2Dplot/ColorMap2D.h"
#include "2Dplot/Curve2D.h"
#include "2Dplot/DataManager2D.h"
#include "2Dplot/ErrorBar2D.h"
#include "2Dplot/ImageItem2D.h"
#include "2Dplot/Layout2D.h"
#include "2Dplot/Legend2D.h"
#include "2Dplot/LineItem2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "2Dplot/Pie2D.h"
#include "2Dplot/TextItem2D.h"
#include "2Dplot/widgets/ArrangeLegend2D.h"
#include "3Dplot/DataManager3D.h"
#include "3Dplot/Layout3D.h"
#include "ApplicationWindow.h"
#include "Matrix.h"
#include "MyWidget.h"
#include "Note.h"
#include "ObjectBrowserTreeItem.h"
#include "ObjectBrowserTreeItemModel.h"
#include "PropertyItem.h"
#include "PropertyItemDelegate.h"
#include "PropertyItemModel.h"
#include "QItemSelectionModel"
#include "Table.h"
#include "core/IconLoader.h"
#include "future/core/column/Column.h"
#include "memory"
#include "ui_propertybrowser.h"

PropertyBrowser::PropertyBrowser(QWidget *parent, ApplicationWindow *app)
    : QDockWidget(parent), app_(app), ui_(new Ui_PropertyBrowser) {
  Q_ASSERT(app_);
  ui_->setupUi(this);
  setWindowTitle(tr("Property Browser"));
  setWindowIcon(QIcon());

  objectItemModel_ = new ObjectBrowserTreeItemModel(ui_->objectView);
  objectSelectionModel_ = new QItemSelectionModel(objectItemModel_);
  propItemModel_ = new PropertyItemModel(ui_->propertyView);

  ui_->dockWidgetContents->layout()->setContentsMargins(0, 0, 0, 0);
  ui_->objectView->setFrameShape(QFrame::NoFrame);
  ui_->objectView->setContextMenuPolicy(Qt::CustomContextMenu);
  ui_->objectView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui_->objectView->setAnimated(true);
  ui_->objectView->setModel(objectItemModel_);
  ui_->propertyView->setFrameShape(QFrame::NoFrame);
  ui_->propertyView->setRootIsDecorated(true);
  ui_->propertyView->setAlternatingRowColors(true);
  ui_->propertyView->setUniformRowHeights(true);
  ui_->propertyView->setEditTriggers(
      QAbstractItemView::EditTrigger::CurrentChanged);
  ui_->propertyView->setAnimated(true);
  ui_->propertyView->setSelectionMode(QAbstractItemView::SingleSelection);
  ui_->propertyView->setItemsExpandable(false);
  ui_->propertyView->setItemDelegate(
      new PropertyItemDelegate(ui_->propertyView));
  ui_->propertyView->setModel(propItemModel_);

  ui_->propertyView->setFocusPolicy(Qt::NoFocus);
  connect(ui_->objectView, &QTreeView::customContextMenuRequested, this,
          &PropertyBrowser::showObjectContextMenu);
  connect(objectItemModel_, &ObjectBrowserTreeItemModel::modelResetComplete,
          ui_->objectView, &QTreeView::expandAll);
  connect(propItemModel_, &PropertyItemModel::modelResetComplete,
          ui_->propertyView, &QTreeView::expandAll);
  connect(propItemModel_, &PropertyItemModel::namechange, this,
          &PropertyBrowser::MyWidgetNameChange);
  connect(propItemModel_, &PropertyItemModel::labelchange, this,
          &PropertyBrowser::MyWidgetLabelChange);
  connect(propItemModel_, &PropertyItemModel::repaintViewer, this,
          [=]() { ui_->propertyView->viewport()->repaint(); });
}

PropertyBrowser::~PropertyBrowser() { delete ui_; }

void PropertyBrowser::populateObjectBrowser(MyWidget *widget) {
  propItemModel_->removeAll();
  if (widget == nullptr) {
    objectItemModel_->buildUpNone();
  } else if (qobject_cast<Table *>(widget)) {
    Table *table = qobject_cast<Table *>(widget);
    objectItemModel_->buildUpTable(table);
  } else if (qobject_cast<Matrix *>(widget)) {
    Matrix *matrix = qobject_cast<Matrix *>(widget);
    objectItemModel_->buildUpMatrix(matrix);
  } else if (qobject_cast<Note *>(widget)) {
    Note *note = qobject_cast<Note *>(widget);
    objectItemModel_->buildUpNote(note);
  } else if (qobject_cast<Layout2D *>(widget)) {
    Layout2D *gd = qobject_cast<Layout2D *>(widget);
    objectItemModel_->buildUpGraph2D(gd);
    connect(gd, &Layout2D::addedOrRemoved, this, [=]() {
      MyWidget *active =
          qobject_cast<MyWidget *>(app_->d_workspace->activeSubWindow());
      if (active == widget) {
        objectItemModel_->buildUpGraph2D(gd);
        propItemModel_->removeAll();
      }
    });
  } else if (qobject_cast<Layout3D *>(widget)) {
    Layout3D *layout = qobject_cast<Layout3D *>(widget);
    objectItemModel_->buildUpGraph3D(layout);
    connect(layout, &Layout3D::dataAdded, this, [=]() {
      MyWidget *active =
          qobject_cast<MyWidget *>(app_->d_workspace->activeSubWindow());
      if (active == widget) {
        objectItemModel_->buildUpGraph3D(layout);
        propItemModel_->removeAll();
      }
    });
  }
  connect(ui_->objectView->selectionModel(),
          &QItemSelectionModel::currentChanged, this,
          &PropertyBrowser::selectObjectItem);
}

void PropertyBrowser::MyWidgetNameChange(MyWidget *widget,
                                         const QString &string) {
  bool status = false;
  if (widget) {
    status = app_->renameWindow(widget, string);
    if (status)
      objectItemModel_->setHeaderData(1, Qt::Horizontal, QVariant(string),
                                      Qt::DisplayRole);
  }
}

void PropertyBrowser::MyWidgetLabelChange(MyWidget *widget,
                                          const QString &string) {
  if (widget) {
    widget->setWindowLabel(string);
    app_->setListViewLabel(widget->name(), string);
  }
}

void PropertyBrowser::showObjectContextMenu(const QPoint &point) {
  bool showmenu = false;
  bool status = false;
  QModelIndex index = ui_->objectView->indexAt(point);
  QMenu cm;
  QMenu cloneaxismenu(tr("Clone Axis..."), &cm);
  QPoint globalPos = ui_->objectView->viewport()->mapToGlobal(point);
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
      Layout2D *layout =
          item->parentItem()->getObjectTreeItem<Layout2D>(&status);
      AxisRect2D *axisrect = item->getObjectTreeItem<AxisRect2D>(&status);
      if (!axisrect || !layout) return;
      app_->itemContextMenuRequested(layout, axisrect);
      return;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DLegend: {
      QAction *lorder = cm.addAction(tr("Legend Reorder"));
      Legend2D *legend = item->getObjectTreeItem<Legend2D>(&status);
      if (!status) return;
      connect(lorder, &QAction::triggered, this, [=]() {
        std::unique_ptr<ArrangeLegend2D> arlegend(
            new ArrangeLegend2D(this, legend));
        arlegend->exec();
      });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DAxis: {
      Axis2D *ax = item->getObjectTreeItem<Axis2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect = ax->getaxisrect_axis();
      cm.addMenu(&cloneaxismenu);
      if (ax->getorientation_axis() == Axis2D::AxisOreantation::Top ||
          ax->getorientation_axis() == Axis2D::AxisOreantation::Bottom) {
        QAction *clonetopaxis = cloneaxismenu.addAction(
            ax->getItemIcon(), tr("Clone To New Top Axis"));
        QAction *clonebottomaxis = cloneaxismenu.addAction(
            ax->getItemIcon(), tr("Clone To New Bottom Axis"));
        connect(clonetopaxis, &QAction::triggered, this, [=]() {
          Axis2D *newaxis = axisrect->addAxis2D(Axis2D::AxisOreantation::Top,
                                                ax->gettickertype_axis());
          ax->clone(newaxis);
          emit axisrect->Axis2DCloned(ax);
        });
        connect(clonebottomaxis, &QAction::triggered, this, [=]() {
          Axis2D *newaxis = axisrect->addAxis2D(Axis2D::AxisOreantation::Bottom,
                                                ax->gettickertype_axis());
          ax->clone(newaxis);
          emit axisrect->Axis2DCloned(ax);
        });
      } else {
        QAction *cloneleftaxis = cloneaxismenu.addAction(
            ax->getItemIcon(), tr("Clone To New Left Axis"));
        QAction *clonerightaxis = cloneaxismenu.addAction(
            ax->getItemIcon(), tr("Clone To New Right Axis"));
        connect(cloneleftaxis, &QAction::triggered, this, [=]() {
          Axis2D *newaxis = axisrect->addAxis2D(Axis2D::AxisOreantation::Left,
                                                ax->gettickertype_axis());
          ax->clone(newaxis);
          emit axisrect->Axis2DCloned(ax);
        });
        connect(clonerightaxis, &QAction::triggered, this, [=]() {
          Axis2D *newaxis = axisrect->addAxis2D(Axis2D::AxisOreantation::Right,
                                                ax->gettickertype_axis());
          ax->clone(newaxis);
          emit axisrect->Axis2DCloned(ax);
        });
      }
      QAction *removeaxis =
          cm.addAction(IconLoader::load("clear-loginfo", IconLoader::General),
                       tr("Remove Axis"));
      cm.addAction(removeaxis);
      connect(removeaxis, &QAction::triggered, this, [=]() {
        bool result = axisrect->removeAxis2D(ax);
        if (!result) {
          qDebug() << "unable to remove axis from 2d plot";
          return;
        }
      });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DTextItem: {
      TextItem2D *textitem = item->getObjectTreeItem<TextItem2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect = textitem->getaxisrect();
      QAction *layerup = cm.addAction(
          IconLoader::load("edit-up", IconLoader::LightDark), tr("Layer Up"));
      QAction *layerdown =
          cm.addAction(IconLoader::load("edit-down", IconLoader::LightDark),
                       tr("Layer Down"));
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(layerup, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(textitem->layer(),
                            QCustomPlot::LayerInsertMode::limAbove);
      });
      connect(layerdown, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(textitem->layer(),
                            QCustomPlot::LayerInsertMode::limBelow);
      });
      connect(remove, &QAction::triggered, this,
              [=]() { axisrect->removeTextItem2D(textitem); });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DLineItem: {
      LineItem2D *lineitem = item->getObjectTreeItem<LineItem2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect = lineitem->getaxisrect();
      QAction *layerup = cm.addAction(
          IconLoader::load("edit-up", IconLoader::LightDark), tr("Layer Up"));
      QAction *layerdown =
          cm.addAction(IconLoader::load("edit-down", IconLoader::LightDark),
                       tr("Layer Down"));
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(layerup, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(lineitem->layer(),
                            QCustomPlot::LayerInsertMode::limAbove);
      });
      connect(layerdown, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(lineitem->layer(),
                            QCustomPlot::LayerInsertMode::limBelow);
      });
      connect(remove, &QAction::triggered, this,
              [=]() { axisrect->removeLineItem2D(lineitem); });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DImageItem: {
      ImageItem2D *imageitem = item->getObjectTreeItem<ImageItem2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect = imageitem->getaxisrect();
      QAction *layerup = cm.addAction(
          IconLoader::load("edit-up", IconLoader::LightDark), tr("Layer Up"));
      QAction *layerdown =
          cm.addAction(IconLoader::load("edit-down", IconLoader::LightDark),
                       tr("Layer Down"));
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(layerup, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(imageitem->layer(),
                            QCustomPlot::LayerInsertMode::limAbove);
      });
      connect(layerdown, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(imageitem->layer(),
                            QCustomPlot::LayerInsertMode::limBelow);
      });
      connect(remove, &QAction::triggered, this,
              [=]() { axisrect->removeImageItem2D(imageitem); });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DLSGraph: {
      LineSpecial2D *ls = item->getObjectTreeItem<LineSpecial2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect = ls->getxaxis()->getaxisrect_axis();
      QAction *data = cm.addAction(tr("Go To Columns..."));
      connect(data, &QAction::triggered, this, [=]() {
        DataBlockGraph *data = ls->getdatablock_lsplot();
        Table *table = data->gettable();
        Column *xcol = data->getxcolumn();
        Column *ycol = data->getycolumn();
        app_->activateWindow(table);
        table->selectColumn(table->colIndex(xcol->name()));
        table->selectColumn(table->colIndex(ycol->name()));
      });
      QAction *layerup = cm.addAction(
          IconLoader::load("edit-up", IconLoader::LightDark), tr("Layer Up"));
      QAction *layerdown =
          cm.addAction(IconLoader::load("edit-down", IconLoader::LightDark),
                       tr("Layer Down"));
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(layerup, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(ls->layer(),
                            QCustomPlot::LayerInsertMode::limAbove);
      });
      connect(layerdown, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(ls->layer(),
                            QCustomPlot::LayerInsertMode::limBelow);
      });
      connect(remove, &QAction::triggered, this,
              [=]() { axisrect->removeLineSpecial2D(ls); });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DChannelGraph: {
      Channel2D *channel = item->getObjectTreeItem<Channel2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect =
          channel->getChannelFirst()->getxaxis()->getaxisrect_axis();
      QAction *data = cm.addAction(tr("Go To Columns..."));
      connect(data, &QAction::triggered, this, [=]() {
        DataBlockGraph *data1 =
            channel->getChannelFirst()->getdatablock_lsplot();
        DataBlockGraph *data2 =
            channel->getChannelSecond()->getdatablock_lsplot();
        Table *table = data1->gettable();
        Column *xcol = data1->getxcolumn();
        Column *ycol1 = data1->getycolumn();
        Column *ycol2 = data2->getycolumn();
        app_->activateWindow(table);
        table->selectColumn(table->colIndex(xcol->name()));
        table->selectColumn(table->colIndex(ycol1->name()));
        table->selectColumn(table->colIndex(ycol2->name()));
      });
      QAction *layerup = cm.addAction(
          IconLoader::load("edit-up", IconLoader::LightDark), tr("Layer Up"));
      QAction *layerdown =
          cm.addAction(IconLoader::load("edit-down", IconLoader::LightDark),
                       tr("Layer Down"));
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(layerup, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(channel->getChannelFirst()->layer(),
                            QCustomPlot::LayerInsertMode::limAbove);
      });
      connect(layerdown, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(channel->getChannelFirst()->layer(),
                            QCustomPlot::LayerInsertMode::limBelow);
      });
      connect(remove, &QAction::triggered, this,
              [=]() { axisrect->removeChannel2D(channel); });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DCurve: {
      Curve2D *curve = item->getObjectTreeItem<Curve2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect = curve->getxaxis()->getaxisrect_axis();
      if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Function) {
        QAction *funcdetails = cm.addAction(tr("Function Details"));
        connect(funcdetails, &QAction::triggered, this, [=]() {
          QMessageBox::information(this, tr("Function Details 2D"),
                                   curve->getItemTooltip());
        });
      } else {
        QAction *data = cm.addAction(tr("Go To Columns..."));
        connect(data, &QAction::triggered, this, [=]() {
          DataBlockCurve *data = curve->getdatablock_cplot();
          Table *table = data->gettable();
          Column *xcol = data->getxcolumn();
          Column *ycol = data->getycolumn();
          app_->activateWindow(table);
          table->selectColumn(table->colIndex(xcol->name()));
          table->selectColumn(table->colIndex(ycol->name()));
        });
      }
      QAction *layerup = cm.addAction(
          IconLoader::load("edit-up", IconLoader::LightDark), tr("Layer Up"));
      QAction *layerdown =
          cm.addAction(IconLoader::load("edit-down", IconLoader::LightDark),
                       tr("Layer Down"));
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(layerup, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(curve->layer(),
                            QCustomPlot::LayerInsertMode::limAbove);
      });
      connect(layerdown, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(curve->layer(),
                            QCustomPlot::LayerInsertMode::limBelow);
      });
      connect(remove, &QAction::triggered, this,
              [=]() { axisrect->removeCurve2D(curve); });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DStatBox: {
      StatBox2D *sb = item->getObjectTreeItem<StatBox2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect = sb->getxaxis()->getaxisrect_axis();
      QAction *data = cm.addAction(tr("Go To Columns..."));
      connect(data, &QAction::triggered, this, [=]() {
        Table *table = sb->gettable_statbox();
        Column *col = sb->getcolumn_statbox();
        app_->activateWindow(table);
        table->selectColumn(table->colIndex(col->name()));
      });
      QAction *layerup = cm.addAction(
          IconLoader::load("edit-up", IconLoader::LightDark), tr("Layer Up"));
      QAction *layerdown =
          cm.addAction(IconLoader::load("edit-down", IconLoader::LightDark),
                       tr("Layer Down"));
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(layerup, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(sb->layer(),
                            QCustomPlot::LayerInsertMode::limAbove);
      });
      connect(layerdown, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(sb->layer(),
                            QCustomPlot::LayerInsertMode::limBelow);
      });
      connect(remove, &QAction::triggered, this,
              [=]() { axisrect->removeStatBox2D(sb); });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DVector: {
      Vector2D *vec = item->getObjectTreeItem<Vector2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect = vec->getxaxis()->getaxisrect_axis();
      QAction *data = cm.addAction(tr("Go To Columns..."));
      connect(data, &QAction::triggered, this, [=]() {
        Table *table = vec->gettable_vecplot();
        Column *col1 = vec->getfirstcol_vecplot();
        Column *col2 = vec->getsecondcol_vecplot();
        Column *col3 = vec->getthirdcol_vecplot();
        Column *col4 = vec->getfourthcol_vecplot();
        app_->activateWindow(table);
        table->selectColumn(table->colIndex(col1->name()));
        table->selectColumn(table->colIndex(col2->name()));
        table->selectColumn(table->colIndex(col3->name()));
        table->selectColumn(table->colIndex(col4->name()));
      });
      QAction *layerup = cm.addAction(
          IconLoader::load("edit-up", IconLoader::LightDark), tr("Layer Up"));
      QAction *layerdown =
          cm.addAction(IconLoader::load("edit-down", IconLoader::LightDark),
                       tr("Layer Down"));
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(layerup, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(vec->layer(),
                            QCustomPlot::LayerInsertMode::limAbove);
      });
      connect(layerdown, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(vec->layer(),
                            QCustomPlot::LayerInsertMode::limBelow);
      });
      connect(remove, &QAction::triggered, this,
              [=]() { axisrect->removeVector2D(vec); });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DBarGraph: {
      Bar2D *bar = item->getObjectTreeItem<Bar2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect = bar->getxaxis()->getaxisrect_axis();
      QAction *data = cm.addAction(tr("Go To Columns..."));
      if (bar->ishistogram_barplot()) {
        connect(data, &QAction::triggered, this, [=]() {
          DataBlockHist *data = bar->getdatablock_histplot();
          Table *table = data->gettable();
          Column *col = data->getcolumn();
          app_->activateWindow(table);
          table->selectColumn(table->colIndex(col->name()));
        });
      } else {
        connect(data, &QAction::triggered, this, [=]() {
          DataBlockBar *data = bar->getdatablock_barplot();
          Table *table = data->gettable();
          Column *xcol = data->getxcolumn();
          Column *ycol = data->getycolumn();
          app_->activateWindow(table);
          table->selectColumn(table->colIndex(xcol->name()));
          table->selectColumn(table->colIndex(ycol->name()));
        });
      }
      QAction *layerup = cm.addAction(
          IconLoader::load("edit-up", IconLoader::LightDark), tr("Layer Up"));
      QAction *layerdown =
          cm.addAction(IconLoader::load("edit-down", IconLoader::LightDark),
                       tr("Layer Down"));
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(layerup, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(bar->layer(),
                            QCustomPlot::LayerInsertMode::limAbove);
      });
      connect(layerdown, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(bar->layer(),
                            QCustomPlot::LayerInsertMode::limBelow);
      });
      connect(remove, &QAction::triggered, this,
              [=]() { axisrect->removeBar2D(bar); });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DPieGraph: {
      Pie2D *pie = item->getObjectTreeItem<Pie2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect = pie->getaxisrect();
      QAction *data = cm.addAction(tr("Go To Columns..."));
      connect(data, &QAction::triggered, this, [=]() {
        Table *table = pie->gettable_pieplot();
        Column *xcol = pie->getxcolumn_pieplot();
        Column *ycol = pie->getycolumn_pieplot();
        app_->activateWindow(table);
        table->selectColumn(table->colIndex(xcol->name()));
        table->selectColumn(table->colIndex(ycol->name()));
      });
      QAction *layerup = cm.addAction(
          IconLoader::load("edit-up", IconLoader::LightDark), tr("Layer Up"));
      QAction *layerdown =
          cm.addAction(IconLoader::load("edit-down", IconLoader::LightDark),
                       tr("Layer Down"));
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(layerup, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(pie->layer(),
                            QCustomPlot::LayerInsertMode::limAbove);
      });
      connect(layerdown, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(pie->layer(),
                            QCustomPlot::LayerInsertMode::limBelow);
      });
      connect(remove, &QAction::triggered, this,
              [=]() { axisrect->removePie2D(pie); });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DColorMap: {
      ColorMap2D *cmap = item->getObjectTreeItem<ColorMap2D>(&status);
      if (!status) return;
      AxisRect2D *axisrect = cmap->getxaxis()->getaxisrect_axis();
      QAction *data = cm.addAction(tr("Go To Matrix..."));
      connect(data, &QAction::triggered, this, [=]() {
        Matrix *matrix = cmap->getmatrix_colormap();
        app_->activateWindow(matrix);
      });
      QAction *layerup = cm.addAction(
          IconLoader::load("edit-up", IconLoader::LightDark), tr("Layer Up"));
      QAction *layerdown =
          cm.addAction(IconLoader::load("edit-down", IconLoader::LightDark),
                       tr("Layer Down"));
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(layerup, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(cmap->layer(),
                            QCustomPlot::LayerInsertMode::limAbove);
      });
      connect(layerdown, &QAction::triggered, this, [=]() {
        axisrect->moveLayer(cmap->layer(),
                            QCustomPlot::LayerInsertMode::limBelow);
      });
      connect(remove, &QAction::triggered, this,
              [=]() { axisrect->removeColorMap2D(cmap); });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DErrorBar: {
      ErrorBar2D *err = item->getObjectTreeItem<ErrorBar2D>(&status);
      if (!status) return;
      QAction *data = cm.addAction(tr("Go To Columns..."));
      connect(data, &QAction::triggered, this, [=]() {
        DataBlockError *errdata = err->getdatablock_error();
        Table *table = errdata->gettable();
        Column *col = errdata->geterrorcolumn();
        app_->activateWindow(table);
        table->selectColumn(table->colIndex(col->name()));
      });
      QAction *remove = cm.addAction(
          IconLoader::load("clear-loginfo", IconLoader::General), tr("Remove"));
      connect(remove, &QAction::triggered, this, [=]() {
        switch (err->getGraphType()) {
          case ErrorBar2D::GraphType::LineSpecial: {
            LineSpecial2D *ls = err->getlinespecial2d_errorbar();
            if (ls)
              (err->geterrortype_errorbar() ==
               QCPErrorBars::ErrorType::etKeyError)
                  ? ls->removeXerrorBar()
                  : ls->removeYerrorBar();
          } break;
          case ErrorBar2D::GraphType::Curve: {
            Curve2D *curve = err->getcurve2d_errorbar();
            if (curve)
              (err->geterrortype_errorbar() ==
               QCPErrorBars::ErrorType::etKeyError)
                  ? curve->removeXerrorBar()
                  : curve->removeYerrorBar();
          } break;
          case ErrorBar2D::GraphType::Bar: {
            Bar2D *bar = err->getbar2d_errorbar();
            if (bar)
              (err->geterrortype_errorbar() ==
               QCPErrorBars::ErrorType::etKeyError)
                  ? bar->removeXerrorBar()
                  : bar->removeYerrorBar();
          } break;
        }
      });
      showmenu = true;
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DSurfaceDataBlock: {
      DataBlockSurface3D *block =
          item->getObjectTreeItem<DataBlockSurface3D>(&status);
      if (!status) return;
      if (!block->ismatrix()) {
        QAction *funcdetails = cm.addAction(tr("Function Details"));
        connect(funcdetails, &QAction::triggered, this, [=]() {
          QMessageBox::information(this, tr("Function Details 3D"),
                                   block->getItemTooltip());
        });
        showmenu = true;
      }
    } break;
    default:
      showmenu = false;
      break;
  }
  if (showmenu) cm.exec(globalPos);
}

void PropertyBrowser::selectObjectItem(const QModelIndex &current,
                                       const QModelIndex &previous) {
  Q_UNUSED(previous)
  if (!current.isValid()) return;
  ObjectBrowserTreeItem *item =
      static_cast<ObjectBrowserTreeItem *>(current.internalPointer());
  if (current.internalPointer())
    if (item) {
      propItemModel_->buildUp(item);
      // span first column for separators
      for (int i = 0; i < propItemModel_->rowCount(); i++) {
        PropertyItem *pitem = static_cast<PropertyItem *>(
            propItemModel_->index(i, 0).internalPointer());
        if (pitem->propertyType() == PropertyItem::PropertyType::Separator) {
          ui_->propertyView->setFirstColumnSpanned(
              i, propItemModel_->parent(propItemModel_->index(i, 0)), true);
        }
      }
    }
}
