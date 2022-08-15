/***************************************************************************
    File                 : ObjectBrowserTreeItemModel.h
    Project              : AlphaPlot
    Description          : Object item model class
    --------------------------------------------------------------------
    Copyright            : (C) 2022 Arun Narayanankutty
                               <n.arun.lifescience@gmail.com>

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#include "ObjectBrowserTreeItemModel.h"

#include <QIcon>
#include <QItemSelectionModel>

#include "2Dplot/Axis2D.h"
#include "2Dplot/Channel2D.h"
#include "2Dplot/ColorMap2D.h"
#include "2Dplot/Curve2D.h"
#include "2Dplot/ErrorBar2D.h"
#include "2Dplot/GridPair2D.h"
#include "2Dplot/ImageItem2D.h"
#include "2Dplot/Layout2D.h"
#include "2Dplot/Legend2D.h"
#include "2Dplot/LineItem2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "2Dplot/Pie2D.h"
#include "2Dplot/Plot2D.h"
#include "2Dplot/StatBox2D.h"
#include "2Dplot/TextItem2D.h"
#include "2Dplot/Vector2D.h"
#include "3Dplot/Bar3D.h"
#include "3Dplot/DataManager3D.h"
#include "3Dplot/Layout3D.h"
#include "3Dplot/Scatter3D.h"
#include "3Dplot/Surface3D.h"
#include "Matrix.h"
#include "MyWidget.h"
#include "Note.h"
#include "ObjectBrowserTreeItem.h"
#include "Table.h"
#include "core/IconLoader.h"

ObjectBrowserTreeItemModel::ObjectBrowserTreeItemModel(QObject *parent)
    : QAbstractItemModel(parent) {
  rootItem_ = ObjectBrowserTreeItem::create(
      QVariant::fromValue<MyWidget *>(nullptr),
      ObjectBrowserTreeItem::ObjectType::None, nullptr);
}

ObjectBrowserTreeItemModel::~ObjectBrowserTreeItemModel() { delete rootItem_; }

QVariant ObjectBrowserTreeItemModel::data(const QModelIndex &index,
                                          int role) const {
  if (!index.isValid()) return QVariant();
  ObjectBrowserTreeItem *item =
      static_cast<ObjectBrowserTreeItem *>(index.internalPointer());
  return item->data(static_cast<Qt::ItemDataRole>(role));
}

Qt::ItemFlags ObjectBrowserTreeItemModel::flags(
    const QModelIndex &index) const {
  if (!index.isValid()) return Qt::NoItemFlags;

  return QAbstractItemModel::flags(index);
}

QVariant ObjectBrowserTreeItemModel::headerData(int section,
                                                Qt::Orientation orientation,
                                                int role) const {
  Q_UNUSED(section);
  if (!rootItem_) return QVariant();
  if (orientation == Qt::Horizontal) switch (role) {
      case Qt::ItemDataRole::DisplayRole:
        return rootItem_->data(Qt::ItemDataRole::DisplayRole);
        break;
      case Qt::ItemDataRole::DecorationRole:
        return rootItem_->data(Qt::ItemDataRole::DecorationRole);
        break;
      case Qt::ItemDataRole::ToolTipRole:
        return rootItem_->data(Qt::ItemDataRole::ToolTipRole);
        break;
      default:
        return QVariant();
    }

  return QVariant();
}

QModelIndex ObjectBrowserTreeItemModel::index(int row, int column,
                                              const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent)) return QModelIndex();

  ObjectBrowserTreeItem *parentItem;

  if (!parent.isValid())
    parentItem = rootItem_;
  else
    parentItem = static_cast<ObjectBrowserTreeItem *>(parent.internalPointer());

  ObjectBrowserTreeItem *childItem = parentItem->child(row);
  if (childItem) return createIndex(row, column, childItem);
  return QModelIndex();
}

QModelIndex ObjectBrowserTreeItemModel::parent(const QModelIndex &index) const {
  if (!index.isValid()) return QModelIndex();

  ObjectBrowserTreeItem *childItem =
      static_cast<ObjectBrowserTreeItem *>(index.internalPointer());
  ObjectBrowserTreeItem *parentItem = childItem->parentItem();
  if (parentItem == rootItem_) return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}

int ObjectBrowserTreeItemModel::rowCount(const QModelIndex &parent) const {
  ObjectBrowserTreeItem *parentItem = nullptr;

  if (!parent.isValid())
    parentItem = rootItem_;
  else
    parentItem = static_cast<ObjectBrowserTreeItem *>(parent.internalPointer());

  return parentItem->childCount();
}

int ObjectBrowserTreeItemModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid())
    return static_cast<ObjectBrowserTreeItem *>(parent.internalPointer())
        ->columnCount();
  return rootItem_->columnCount();
}

void ObjectBrowserTreeItemModel::updateProperty(ObjectBrowserTreeItem *item) {
  int column = 1;
  int numChild = rootItem_->childCount();
  for (int row = 0; row < numChild; row++) {
    ObjectBrowserTreeItem *child = rootItem_->child(row);
    if (child == item) {
      // child->updateData();
      QModelIndex data = this->index(row, column, QModelIndex());
      if (data.isValid()) {
        // child->assignProperty(&prop);
        emit dataChanged(data, data);
        emit headerDataChanged(Qt::Horizontal, 0, 1);
        // updateChildren(child, column, data);
      }
      break;
    }
  }
}

void ObjectBrowserTreeItemModel::updateChildren(ObjectBrowserTreeItem *item,
                                                int column,
                                                const QModelIndex &parent) {
  int numChild = item->childCount();
  if (numChild > 0) {
    QModelIndex topLeft = this->index(0, column, parent);
    QModelIndex bottomRight = this->index(numChild, column, parent);
    emit dataChanged(topLeft, bottomRight);
  }
}

bool ObjectBrowserTreeItemModel::setHeaderData(int section,
                                               Qt::Orientation orientation,
                                               const QVariant &value,
                                               int role) {
  emit headerDataChanged(orientation, section, section);
}

void ObjectBrowserTreeItemModel::buildUpNone() {
  beginResetModel();
  delete rootItem_;
  rootItem_ = ObjectBrowserTreeItem::create(
      QVariant::fromValue<MyWidget *>(nullptr),
      ObjectBrowserTreeItem::ObjectType::None, nullptr);
  endResetModel();
  emit modelResetComplete();
}

void ObjectBrowserTreeItemModel::buildUpTable(Table *table) {
  beginResetModel();
  delete rootItem_;
  MyWidget *widget = static_cast<MyWidget *>(table);
  rootItem_ = ObjectBrowserTreeItem::create(
      QVariant::fromValue<Table *>(table),
      ObjectBrowserTreeItem::ObjectType::TableWindow, nullptr);
  ObjectBrowserTreeItem::create(QVariant::fromValue<MyWidget *>(widget),
                                ObjectBrowserTreeItem::ObjectType::BaseWindow,
                                rootItem_);
  ObjectBrowserTreeItem::create(
      QVariant::fromValue<Table *>(table),
      ObjectBrowserTreeItem::ObjectType::TableDimension, rootItem_);
  endResetModel();
  emit modelResetComplete();
}

void ObjectBrowserTreeItemModel::buildUpMatrix(Matrix *matrix) {
  beginResetModel();
  delete rootItem_;
  MyWidget *widget = static_cast<MyWidget *>(matrix);
  rootItem_ = ObjectBrowserTreeItem::create(
      QVariant::fromValue<Matrix *>(matrix),
      ObjectBrowserTreeItem::ObjectType::MatrixWindow, nullptr);
  ObjectBrowserTreeItem::create(QVariant::fromValue<MyWidget *>(widget),
                                ObjectBrowserTreeItem::ObjectType::BaseWindow,
                                rootItem_);
  ObjectBrowserTreeItem::create(
      QVariant::fromValue<Matrix *>(matrix),
      ObjectBrowserTreeItem::ObjectType::MatrixDimension, rootItem_);
  endResetModel();
  emit modelResetComplete();
}

void ObjectBrowserTreeItemModel::buildUpNote(Note *note) {
  beginResetModel();
  delete rootItem_;
  MyWidget *widget = static_cast<MyWidget *>(note);
  rootItem_ = ObjectBrowserTreeItem::create(
      QVariant::fromValue<Note *>(note),
      ObjectBrowserTreeItem::ObjectType::NoteWindow, nullptr);
  ObjectBrowserTreeItem::create(QVariant::fromValue<MyWidget *>(widget),
                                ObjectBrowserTreeItem::ObjectType::BaseWindow,
                                rootItem_);
  endResetModel();
  emit modelResetComplete();
}

void ObjectBrowserTreeItemModel::buildUpGraph2D(Layout2D *layout) {
  beginResetModel();
  delete rootItem_;
  MyWidget *widget = static_cast<MyWidget *>(layout);
  rootItem_ = ObjectBrowserTreeItem::create(
      QVariant::fromValue<Layout2D *>(layout),
      ObjectBrowserTreeItem::ObjectType::Plot2DWindow, nullptr);
  ObjectBrowserTreeItem::create(QVariant::fromValue<MyWidget *>(widget),
                                ObjectBrowserTreeItem::ObjectType::BaseWindow,
                                rootItem_);
  ObjectBrowserTreeItem::create(
      QVariant::fromValue<Plot2D *>(layout->getPlotCanwas()),
      ObjectBrowserTreeItem::ObjectType::Plot2DCanvas, rootItem_);
  QList<AxisRect2D *> axisrectlist = layout->getAxisRectList();
  foreach (AxisRect2D *axisrect, axisrectlist) {
    ObjectBrowserTreeItem *axritem = ObjectBrowserTreeItem::create(
        QVariant::fromValue<AxisRect2D *>(axisrect),
        ObjectBrowserTreeItem::ObjectType::Plot2DLayout, rootItem_);

    // Legend
    ObjectBrowserTreeItem::create(
        QVariant::fromValue<Legend2D *>(axisrect->getLegend()),
        ObjectBrowserTreeItem::ObjectType::Plot2DLegend, axritem);

    // Axis items
    QList<Axis2D *> xaxes = axisrect->getXAxes2D();
    QList<Axis2D *> yaxes = axisrect->getYAxes2D();
    foreach (Axis2D *axis, xaxes) {
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<Axis2D *>(axis),
          ObjectBrowserTreeItem::ObjectType::Plot2DAxis, axritem);
    }
    foreach (Axis2D *axis, yaxes) {
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<Axis2D *>(axis),
          ObjectBrowserTreeItem::ObjectType::Plot2DAxis, axritem);
    }
    // plottables & items vector of element
    auto textitems = axisrect->getTextItemVec();
    auto lineitems = axisrect->getLineItemVec();
    auto imageitems = axisrect->getImageItemVec();
    auto graphvec = axisrect->getLsVec();
    auto curvevec = axisrect->getCurveVec();
    auto statboxvec = axisrect->getStatBoxVec();
    auto vectorvec = axisrect->getVectorVec();
    auto channelvec = axisrect->getChannelVec();
    auto barvec = axisrect->getBarVec();
    auto pievec = axisrect->getPieVec();
    auto colormapvec = axisrect->getColorMapVec();
    auto layervec = axisrect->getLayerVec();
    // reverse layer list order
    for (int k = 0, s = layervec.size(), max = (s / 2); k < max; k++)
#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
      layervec.swapItemsAt(k, s - (1 + k));
#else
      layervec.swap(k, s - (1 + k));
#endif
    foreach (QCPLayer *layer, layervec) {
      bool layerfound = false;
      // Text items
      foreach (TextItem2D *textitem, textitems) {
        if (layer == textitem->layer()) {
          ObjectBrowserTreeItem::create(
              QVariant::fromValue<TextItem2D *>(textitem),
              ObjectBrowserTreeItem::ObjectType::Plot2DTextItem, axritem);
          layerfound = true;
          textitems.removeOne(textitem);
          break;
        }
      }
      if (layerfound) continue;

      // Line items
      foreach (LineItem2D *lineitem, lineitems) {
        if (layer == lineitem->layer()) {
          ObjectBrowserTreeItem::create(
              QVariant::fromValue<LineItem2D *>(lineitem),
              ObjectBrowserTreeItem::ObjectType::Plot2DLineItem, axritem);
          layerfound = true;
          lineitems.removeOne(lineitem);
          break;
        }
      }
      if (layerfound) continue;

      // Image items
      foreach (ImageItem2D *imageitem, imageitems) {
        if (layer == imageitem->layer()) {
          ObjectBrowserTreeItem::create(
              QVariant::fromValue<ImageItem2D *>(imageitem),
              ObjectBrowserTreeItem::ObjectType::Plot2DImageItem, axritem);
          layerfound = true;
          imageitems.removeOne(imageitem);
          break;
        }
      }
      if (layerfound) continue;

      // LS plots
      foreach (LineSpecial2D *ls, graphvec) {
        if (layer == ls->layer()) {
          ObjectBrowserTreeItem *itemls = ObjectBrowserTreeItem::create(
              QVariant::fromValue<LineSpecial2D *>(ls),
              ObjectBrowserTreeItem::ObjectType::Plot2DLSGraph, axritem);
          ErrorBar2D *xerr = ls->getxerrorbar_lsplot();
          if (xerr) {
            ObjectBrowserTreeItem::create(
                QVariant::fromValue<ErrorBar2D *>(xerr),
                ObjectBrowserTreeItem::ObjectType::Plot2DErrorBar, itemls);
          }
          ErrorBar2D *yerr = ls->getyerrorbar_lsplot();
          if (yerr) {
            ObjectBrowserTreeItem::create(
                QVariant::fromValue<ErrorBar2D *>(yerr),
                ObjectBrowserTreeItem::ObjectType::Plot2DErrorBar, itemls);
          }
          layerfound = true;
          graphvec.removeOne(ls);
          break;
        }
      }
      if (layerfound) continue;

      // Curve 2D plots
      foreach (Curve2D *curve, curvevec) {
        if (layer == curve->layer()) {
          ObjectBrowserTreeItem *itemls = ObjectBrowserTreeItem::create(
              QVariant::fromValue<Curve2D *>(curve),
              ObjectBrowserTreeItem::ObjectType::Plot2DCurve, axritem);
          ErrorBar2D *xerr = curve->getxerrorbar_curveplot();
          if (xerr) {
            ObjectBrowserTreeItem::create(
                QVariant::fromValue<ErrorBar2D *>(xerr),
                ObjectBrowserTreeItem::ObjectType::Plot2DErrorBar, itemls);
          }
          ErrorBar2D *yerr = curve->getyerrorbar_curveplot();
          if (yerr) {
            ObjectBrowserTreeItem::create(
                QVariant::fromValue<ErrorBar2D *>(yerr),
                ObjectBrowserTreeItem::ObjectType::Plot2DErrorBar, itemls);
          }
          layerfound = true;
          curvevec.removeOne(curve);
          break;
        }
      }
      if (layerfound) continue;

      // Statbox plots
      foreach (StatBox2D *statbox, statboxvec) {
        if (layer == statbox->layer()) {
          ObjectBrowserTreeItem::create(
              QVariant::fromValue<StatBox2D *>(statbox),
              ObjectBrowserTreeItem::ObjectType::Plot2DStatBox, axritem);
          layerfound = true;
          statboxvec.removeOne(statbox);
          break;
        }
      }
      if (layerfound) continue;

      // Vector 2D Plots
      foreach (Vector2D *vec, vectorvec) {
        if (layer == vec->layer()) {
          ObjectBrowserTreeItem::create(
              QVariant::fromValue<Vector2D *>(vec),
              ObjectBrowserTreeItem::ObjectType::Plot2DVector, axritem);
          layerfound = true;
          vectorvec.removeOne(vec);
          break;
        }
      }
      if (layerfound) continue;

      // Channel 2D plots
      foreach (Channel2D *channel, channelvec) {
        if (layer == channel->getChannelFirst()->layer()) {
          ObjectBrowserTreeItem::create(
              QVariant::fromValue<Channel2D *>(channel),
              ObjectBrowserTreeItem::ObjectType::Plot2DChannelGraph, axritem);
          layerfound = true;
          channelvec.removeOne(channel);
          break;
        }
      }
      if (layerfound) continue;

      // Bar 2D plots
      foreach (Bar2D *bar, barvec) {
        if (layer == bar->layer()) {
          ObjectBrowserTreeItem *itemls = ObjectBrowserTreeItem::create(
              QVariant::fromValue<Bar2D *>(bar),
              ObjectBrowserTreeItem::ObjectType::Plot2DBarGraph, axritem);
          ErrorBar2D *xerr = bar->getxerrorbar_barplot();
          if (xerr) {
            ObjectBrowserTreeItem::create(
                QVariant::fromValue<ErrorBar2D *>(xerr),
                ObjectBrowserTreeItem::ObjectType::Plot2DErrorBar, itemls);
          }
          ErrorBar2D *yerr = bar->getyerrorbar_barplot();
          if (yerr) {
            ObjectBrowserTreeItem::create(
                QVariant::fromValue<ErrorBar2D *>(yerr),
                ObjectBrowserTreeItem::ObjectType::Plot2DErrorBar, itemls);
          }
          layerfound = true;
          barvec.removeOne(bar);
          break;
        }
      }
      if (layerfound) continue;

      // Pie 2D plots
      foreach (Pie2D *pie, pievec) {
        if (layer == pie->layer()) {
          ObjectBrowserTreeItem::create(
              QVariant::fromValue<Pie2D *>(pie),
              ObjectBrowserTreeItem::ObjectType::Plot2DPieGraph, axritem);
          layerfound = true;
          pievec.removeOne(pie);
          break;
        }
      }
      if (layerfound) continue;

      // Colormap 2D plots
      foreach (ColorMap2D *cmap, colormapvec) {
        if (layer == cmap->layer()) {
          ObjectBrowserTreeItem::create(
              QVariant::fromValue<ColorMap2D *>(cmap),
              ObjectBrowserTreeItem::ObjectType::Plot2DColorMap, axritem);
          layerfound = true;
          colormapvec.removeOne(cmap);
          break;
        }
      }
      if (layerfound) continue;
    }
    // Grid2D
    ObjectBrowserTreeItem::create(
        QVariant::fromValue<GridPair2D *>(axisrect->getGridPair()),
        ObjectBrowserTreeItem::ObjectType::Plot2DGrid, axritem);
  }
  endResetModel();
  emit modelResetComplete();
}

void ObjectBrowserTreeItemModel::buildUpGraph3D(Layout3D *layout) {
  beginResetModel();
  delete rootItem_;
  MyWidget *widget = static_cast<MyWidget *>(layout);
  rootItem_ = ObjectBrowserTreeItem::create(
      QVariant::fromValue<Layout3D *>(layout),
      ObjectBrowserTreeItem::ObjectType::Plot3DWindow, nullptr);
  ObjectBrowserTreeItem::create(QVariant::fromValue<MyWidget *>(widget),
                                ObjectBrowserTreeItem::ObjectType::BaseWindow,
                                rootItem_);

  switch (layout->getPlotType()) {
    case Graph3DCommon::Plot3DType::Bar: {
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QAbstract3DGraph *>(
              layout->getBar3DModifier()->getGraph()),
          ObjectBrowserTreeItem::ObjectType::Plot3DCanvas, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<Q3DTheme *>(
              layout->getBar3DModifier()->getGraph()->activeTheme()),
          ObjectBrowserTreeItem::ObjectType::Plot3DTheme, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QCategory3DAxis *>(
              layout->getBar3DModifier()->getGraph()->rowAxis()),
          ObjectBrowserTreeItem::ObjectType::Plot3DAxisCatagoryX, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QCategory3DAxis *>(
              layout->getBar3DModifier()->getGraph()->columnAxis()),
          ObjectBrowserTreeItem::ObjectType::Plot3DAxisCatagoryY, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QValue3DAxis *>(
              layout->getBar3DModifier()->getGraph()->valueAxis()),
          ObjectBrowserTreeItem::ObjectType::Plot3DAxisValueZ, rootItem_);
      ObjectBrowserTreeItem *item = ObjectBrowserTreeItem::create(
          QVariant::fromValue<Bar3D *>(layout->getBar3DModifier()),
          ObjectBrowserTreeItem::ObjectType::Plot3DBar, rootItem_);
      foreach (DataBlockBar3D *data, layout->getBar3DModifier()->getData()) {
        ObjectBrowserTreeItem::create(
            QVariant::fromValue<DataBlockBar3D *>(data),
            ObjectBrowserTreeItem::ObjectType::Plot3DBarDataBlock, item);
      }
    } break;
    case Graph3DCommon::Plot3DType::Scatter: {
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QAbstract3DGraph *>(
              layout->getScatter3DModifier()->getGraph()),
          ObjectBrowserTreeItem::ObjectType::Plot3DCanvas, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<Q3DTheme *>(
              layout->getScatter3DModifier()->getGraph()->activeTheme()),
          ObjectBrowserTreeItem::ObjectType::Plot3DTheme, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QValue3DAxis *>(
              layout->getScatter3DModifier()->getGraph()->axisX()),
          ObjectBrowserTreeItem::ObjectType::Plot3DAxisValueX, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QValue3DAxis *>(
              layout->getScatter3DModifier()->getGraph()->axisY()),
          ObjectBrowserTreeItem::ObjectType::Plot3DAxisValueY, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QValue3DAxis *>(
              layout->getScatter3DModifier()->getGraph()->axisZ()),
          ObjectBrowserTreeItem::ObjectType::Plot3DAxisValueZ, rootItem_);
      ObjectBrowserTreeItem *item = ObjectBrowserTreeItem::create(
          QVariant::fromValue<Scatter3D *>(layout->getScatter3DModifier()),
          ObjectBrowserTreeItem::ObjectType::Plot3DScatter, rootItem_);
      foreach (DataBlockScatter3D *data,
               layout->getScatter3DModifier()->getData()) {
        ObjectBrowserTreeItem::create(
            QVariant::fromValue<DataBlockScatter3D *>(data),
            ObjectBrowserTreeItem::ObjectType::Plot3DScatterDataBlock, item);
      }
    } break;
    case Graph3DCommon::Plot3DType::Surface: {
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QAbstract3DGraph *>(
              layout->getSurface3DModifier()->getGraph()),
          ObjectBrowserTreeItem::ObjectType::Plot3DCanvas, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<Q3DTheme *>(
              layout->getSurface3DModifier()->getGraph()->activeTheme()),
          ObjectBrowserTreeItem::ObjectType::Plot3DTheme, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QValue3DAxis *>(
              layout->getSurface3DModifier()->getGraph()->axisX()),
          ObjectBrowserTreeItem::ObjectType::Plot3DAxisValueX, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QValue3DAxis *>(
              layout->getSurface3DModifier()->getGraph()->axisY()),
          ObjectBrowserTreeItem::ObjectType::Plot3DAxisValueY, rootItem_);
      ObjectBrowserTreeItem::create(
          QVariant::fromValue<QValue3DAxis *>(
              layout->getSurface3DModifier()->getGraph()->axisZ()),
          ObjectBrowserTreeItem::ObjectType::Plot3DAxisValueZ, rootItem_);
      ObjectBrowserTreeItem *item = ObjectBrowserTreeItem::create(
          QVariant::fromValue<Surface3D *>(layout->getSurface3DModifier()),
          ObjectBrowserTreeItem::ObjectType::Plot3DSurface, rootItem_);
      foreach (DataBlockSurface3D *data,
               layout->getSurface3DModifier()->getData()) {
        ObjectBrowserTreeItem::create(
            QVariant::fromValue<DataBlockSurface3D *>(data),
            ObjectBrowserTreeItem::ObjectType::Plot3DSurfaceDataBlock, item);
      }
    } break;
  }
  endResetModel();
  emit modelResetComplete();
}
