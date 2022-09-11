/***************************************************************************
    File                 : ObjectBrowserTreeItemModel.h
    Project              : AlphaPlot
    Description          : Object item base class
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

#include "ObjectBrowserTreeItem.h"

#include "2Dplot/Axis2D.h"
#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Bar2D.h"
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
#include "Note.h"
#include "Table.h"
#include "core/IconLoader.h"

ObjectBrowserTreeItem::ObjectBrowserTreeItem(QVariant data,
                                             const ObjectType &type,
                                             ObjectBrowserTreeItem *parentItem)
    : itemData_(data), itemDataType_(type), parentItem_(parentItem) {
  if (parentItem_) parentItem_->childItems_.append(this);
}

ObjectBrowserTreeItem::RoleData ObjectBrowserTreeItem::value() const {
  bool status = false;
  ObjectBrowserTreeItem::RoleData roledata;
  switch (itemDataType_) {
    case ObjectBrowserTreeItem::ObjectType::None: {
      roledata.name = QObject::tr("(None)");
      roledata.tooltip = QObject::tr("(None)");
      roledata.icon = IconLoader::load("clear-loginfo", IconLoader::General);
    } break;
    case ObjectBrowserTreeItem::ObjectType::BaseWindow: {
      roledata.name = QObject::tr("Window");
      roledata.tooltip = QObject::tr("Window");
      roledata.icon = IconLoader::load("view-console", IconLoader::LightDark);
    } break;
    case ObjectBrowserTreeItem::ObjectType::TableWindow: {
      Table *table = getObjectTreeItem<Table>(&status);
      if (status) {
        roledata.name = table->getItemName();
        roledata.tooltip = table->getItemTooltip();
        roledata.icon = table->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::TableDimension:
    case ObjectBrowserTreeItem::ObjectType::MatrixDimension: {
      roledata.name = QObject::tr("Rows X Cols");
      roledata.tooltip = QObject::tr("Rows X Cols");
      roledata.icon = IconLoader::load("goto-cell", IconLoader::LightDark);
    } break;
    case ObjectBrowserTreeItem::ObjectType::MatrixWindow: {
      Matrix *matrix = getObjectTreeItem<Matrix>(&status);
      if (status) {
        roledata.name = matrix->getItemName();
        roledata.tooltip = matrix->getItemTooltip();
        roledata.icon = matrix->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::NoteWindow: {
      Note *note = getObjectTreeItem<Note>(&status);
      if (status) {
        roledata.name = note->getItemName();
        roledata.tooltip = note->getItemTooltip();
        roledata.icon = note->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DWindow: {
      Layout2D *layout = getObjectTreeItem<Layout2D>(&status);
      if (status) {
        roledata.name = layout->getItemName();
        roledata.tooltip = layout->getItemTooltip();
        roledata.icon = layout->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DWindow: {
      Layout3D *layout = getObjectTreeItem<Layout3D>(&status);
      if (status) {
        roledata.name = layout->getItemName();
        roledata.tooltip = layout->getItemTooltip();
        roledata.icon = layout->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DCanvas: {
      Plot2D *plot = getObjectTreeItem<Plot2D>(&status);
      if (status) {
        roledata.name = plot->getItemName();
        roledata.tooltip = plot->getItemTooltip();
        roledata.icon = plot->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DLayout: {
      Layout2D *layout = parentItem_->getObjectTreeItem<Layout2D>(&status);
      if (!status) break;
      AxisRect2D *axisrect = getObjectTreeItem<AxisRect2D>(&status);
      if (!status) break;
      if (layout && axisrect) {
        QPair<int, int> rowcol = layout->getAxisRectRowCol(axisrect);
        int index = layout->getLayoutRectGridIndex(rowcol) + 1;
        QString text = axisrect->getItemName().arg(
            QString::number(index), QString::number(rowcol.first + 1),
            QString::number(rowcol.second + 1));
        roledata.name = text;
        roledata.tooltip = text;
        roledata.icon = axisrect->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DLegend: {
      Legend2D *legend = getObjectTreeItem<Legend2D>(&status);
      if (status) {
        roledata.name = legend->getItemName();
        roledata.tooltip = legend->getItemTooltip();
        roledata.icon = legend->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DAxis: {
      Axis2D *axis = getObjectTreeItem<Axis2D>(&status);
      if (status) {
        roledata.name = axis->getItemName();
        roledata.tooltip = axis->getItemTooltip();
        roledata.icon = axis->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DTextItem: {
      TextItem2D *textitem = getObjectTreeItem<TextItem2D>(&status);
      if (status) {
        roledata.name = textitem->getItemName();
        roledata.tooltip = textitem->getItemTooltip();
        roledata.icon = textitem->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DLineItem: {
      LineItem2D *lineitem = getObjectTreeItem<LineItem2D>(&status);
      if (status) {
        roledata.name = lineitem->getItemName();
        roledata.tooltip = lineitem->getItemTooltip();
        roledata.icon = lineitem->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DImageItem: {
      ImageItem2D *imageitem = getObjectTreeItem<ImageItem2D>(&status);
      if (status) {
        roledata.name = imageitem->getItemName();
        roledata.tooltip = imageitem->getItemTooltip();
        roledata.icon = imageitem->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DLSGraph: {
      LineSpecial2D *ls = getObjectTreeItem<LineSpecial2D>(&status);
      if (status) {
        roledata.name = ls->getItemName();
        roledata.tooltip = ls->getItemTooltip();
        roledata.icon = ls->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DErrorBar: {
      ErrorBar2D *err = getObjectTreeItem<ErrorBar2D>(&status);
      if (status) {
        roledata.name = err->getItemName();
        roledata.tooltip = err->getItemTooltip();
        roledata.icon = err->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DCurve: {
      Curve2D *curve = getObjectTreeItem<Curve2D>(&status);
      if (status) {
        roledata.name = curve->getItemName();
        roledata.tooltip = curve->getItemTooltip();
        roledata.icon = curve->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DStatBox: {
      StatBox2D *statbox = getObjectTreeItem<StatBox2D>(&status);
      if (status) {
        roledata.name = statbox->getItemName();
        roledata.tooltip = statbox->getItemTooltip();
        roledata.icon = statbox->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DVector: {
      Vector2D *vec = getObjectTreeItem<Vector2D>(&status);
      if (status) {
        roledata.name = vec->getItemName();
        roledata.tooltip = vec->getItemTooltip();
        roledata.icon = vec->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DChannelGraph: {
      Channel2D *channel = getObjectTreeItem<Channel2D>(&status);
      if (status) {
        roledata.name = channel->getItemName();
        roledata.tooltip = channel->getItemTooltip();
        roledata.icon = channel->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DBarGraph: {
      Bar2D *bar = getObjectTreeItem<Bar2D>(&status);
      if (status) {
        roledata.name = bar->getItemName();
        roledata.tooltip = bar->getItemTooltip();
        roledata.icon = bar->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DPieGraph: {
      Pie2D *pie = getObjectTreeItem<Pie2D>(&status);
      if (status) {
        roledata.name = pie->getItemName();
        roledata.tooltip = pie->getItemTooltip();
        roledata.icon = pie->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DColorMap: {
      ColorMap2D *cmap = getObjectTreeItem<ColorMap2D>(&status);
      if (status) {
        roledata.name = cmap->getItemName();
        roledata.tooltip = cmap->getItemTooltip();
        roledata.icon = cmap->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot2DGrid: {
      GridPair2D *grid = getObjectTreeItem<GridPair2D>(&status);
      if (status) {
        roledata.name = grid->getItemName();
        roledata.tooltip = grid->getItemTooltip();
        roledata.icon = grid->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DCanvas: {
      roledata.name = QObject::tr("Canvas");
      roledata.tooltip = QObject::tr("Canvas");
      roledata.icon = IconLoader::load("view-image", IconLoader::LightDark);
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DTheme: {
      roledata.name = QObject::tr("Theme");
      roledata.tooltip = QObject::tr("Theme");
      roledata.icon = IconLoader::load("theme", IconLoader::General);
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DAxisValue: {
      Layout3D *layout = parentItem_->getObjectTreeItem<Layout3D>(&status);
      if (!status) break;
      QValue3DAxis *axis = getObjectTreeItem<QValue3DAxis>(&status);
      if (!status) break;
      switch (layout->getPlotType()) {
        case Graph3DCommon::Plot3DType::Surface: {
          if (axis == layout->getSurface3DModifier()->getGraph()->axisX()) {
            roledata.name = QObject::tr("X Axis(Val)");
            roledata.tooltip = QObject::tr("X Axis(Val)");
            roledata.icon =
                IconLoader::load("graph2d-axis-bottom", IconLoader::LightDark);
          } else if (axis ==
                     layout->getSurface3DModifier()->getGraph()->axisY()) {
            roledata.name = QObject::tr("Y Axis(Val)");
            roledata.tooltip = QObject::tr("Y Axis(Val)");
            roledata.icon =
                IconLoader::load("graph2d-axis-left", IconLoader::LightDark);
          } else if (axis ==
                     layout->getSurface3DModifier()->getGraph()->axisZ()) {
            roledata.name = QObject::tr("Z Axis(Val)");
            roledata.tooltip = QObject::tr("Z Axis(Val)");
            roledata.icon =
                IconLoader::load("graph2d-axis-right", IconLoader::LightDark);
          }
        } break;
        case Graph3DCommon::Plot3DType::Scatter: {
          if (axis == layout->getScatter3DModifier()->getGraph()->axisX()) {
            roledata.name = QObject::tr("X Axis(Val)");
            roledata.tooltip = QObject::tr("X Axis(Val)");
            roledata.icon =
                IconLoader::load("graph2d-axis-bottom", IconLoader::LightDark);
          } else if (axis ==
                     layout->getScatter3DModifier()->getGraph()->axisY()) {
            roledata.name = QObject::tr("Y Axis(Val)");
            roledata.tooltip = QObject::tr("Y Axis(Val)");
            roledata.icon =
                IconLoader::load("graph2d-axis-left", IconLoader::LightDark);
          } else if (axis ==
                     layout->getScatter3DModifier()->getGraph()->axisZ()) {
            roledata.name = QObject::tr("Z Axis(Val)");
            roledata.tooltip = QObject::tr("Z Axis(Val)");
            roledata.icon =
                IconLoader::load("graph2d-axis-right", IconLoader::LightDark);
          }
        } break;
        case Graph3DCommon::Plot3DType::Bar: {
          if (axis == layout->getBar3DModifier()->getGraph()->valueAxis()) {
            roledata.name = QObject::tr("Z Axis(Val)");
            roledata.tooltip = QObject::tr("Z Axis(Val)");
            roledata.icon =
                IconLoader::load("graph2d-axis-right", IconLoader::LightDark);
          }
        } break;
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DAxisCatagory: {
      Layout3D *layout = parentItem_->getObjectTreeItem<Layout3D>(&status);
      if (!status) break;
      QCategory3DAxis *axis = getObjectTreeItem<QCategory3DAxis>(&status);
      if (!status) break;
      switch (layout->getPlotType()) {
        case Graph3DCommon::Plot3DType::Surface:
        case Graph3DCommon::Plot3DType::Scatter:
          break;
        case Graph3DCommon::Plot3DType::Bar: {
          if (axis == layout->getBar3DModifier()->getGraph()->rowAxis()) {
            roledata.name = QObject::tr("X Axis(Cat)");
            roledata.tooltip = QObject::tr("X Axis(Cat)");
            roledata.icon =
                IconLoader::load("graph2d-axis-bottom", IconLoader::LightDark);
          } else if (axis ==
                     layout->getBar3DModifier()->getGraph()->columnAxis()) {
            roledata.name = QObject::tr("Y Axis(Cat)");
            roledata.tooltip = QObject::tr("Y Axis(Cat)");
            roledata.icon =
                IconLoader::load("graph2d-axis-left", IconLoader::LightDark);
          }
        } break;
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DBar: {
      Bar3D *bar = getObjectTreeItem<Bar3D>(&status);
      if (status) {
        roledata.name = bar->getItemName();
        roledata.tooltip = bar->getItemTooltip();
        roledata.icon = bar->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DScatter: {
      Scatter3D *scatter = getObjectTreeItem<Scatter3D>(&status);
      if (status) {
        roledata.name = scatter->getItemName();
        roledata.tooltip = scatter->getItemTooltip();
        roledata.icon = scatter->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DSurface: {
      Surface3D *surface = getObjectTreeItem<Surface3D>(&status);
      if (status) {
        roledata.name = surface->getItemName();
        roledata.tooltip = surface->getItemTooltip();
        roledata.icon = surface->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DBarDataBlock: {
      DataBlockBar3D *data = getObjectTreeItem<DataBlockBar3D>(&status);
      if (status) {
        roledata.name = data->getItemName();
        roledata.tooltip = data->getItemTooltip();
        roledata.icon = data->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DScatterDataBlock: {
      DataBlockScatter3D *data = getObjectTreeItem<DataBlockScatter3D>(&status);
      if (status) {
        roledata.name = data->getItemName();
        roledata.tooltip = data->getItemTooltip();
        roledata.icon = data->getItemIcon();
      }
    } break;
    case ObjectBrowserTreeItem::ObjectType::Plot3DSurfaceDataBlock: {
      DataBlockSurface3D *data = getObjectTreeItem<DataBlockSurface3D>(&status);
      if (status) {
        roledata.name = data->getItemName();
        roledata.tooltip = data->getItemTooltip();
        roledata.icon = data->getItemIcon();
      }
    } break;
    default:
      qDebug() << "Object Tree Item not handled "
               << static_cast<int>(itemDataType_);
      break;
  }
  return roledata;
}

ObjectBrowserTreeItem *ObjectBrowserTreeItem::create(
    QVariant data, const ObjectType &type, ObjectBrowserTreeItem *parentItem) {
  return new ObjectBrowserTreeItem(data, type, parentItem);
}

ObjectBrowserTreeItem::~ObjectBrowserTreeItem() { qDeleteAll(childItems_); }

bool ObjectBrowserTreeItem::removeChild(ObjectBrowserTreeItem *item) {
  if (!childItems_.contains(item)) return false;

  childItems_.removeOne(item);
  return true;
}

ObjectBrowserTreeItem *ObjectBrowserTreeItem::child(int row) {
  if (row < 0 || row >= childItems_.size()) return nullptr;
  return childItems_.at(row);
}

int ObjectBrowserTreeItem::childCount() const { return childItems_.count(); }

int ObjectBrowserTreeItem::columnCount() const { return 1; }

QVariant ObjectBrowserTreeItem::data(const Qt::ItemDataRole &role) {
  if (role == Qt::ItemDataRole::UserRole) return itemData_;

  ObjectBrowserTreeItem::RoleData datarole = value();
  switch (role) {
    case Qt::ItemDataRole::DisplayRole:
      return datarole.name;
      break;
    case Qt::ItemDataRole::ToolTipRole:
      return datarole.tooltip;
      break;
    case Qt::ItemDataRole::DecorationRole:
      return datarole.icon;
      break;
    default:
      return QVariant();
  }
  return QVariant();
}

ObjectBrowserTreeItem::ObjectType ObjectBrowserTreeItem::dataType() const {
  return itemDataType_;
}

int ObjectBrowserTreeItem::row() const {
  if (parentItem_)
    return parentItem_->childItems_.indexOf(
        const_cast<ObjectBrowserTreeItem *>(this));

  return 0;
}

ObjectBrowserTreeItem *ObjectBrowserTreeItem::parentItem() {
  return parentItem_;
}
