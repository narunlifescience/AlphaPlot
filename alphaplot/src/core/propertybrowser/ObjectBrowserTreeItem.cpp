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

#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Layout2D.h"

ObjectBrowserTreeItem::ObjectBrowserTreeItem(QVariant data,
                                             const ObjectType &type,
                                             ObjectBrowserTreeItem *parentItem)
    : itemData_(data),
      itemDataType_(type),
      parentItem_(parentItem),
      childItems_(QVector<ObjectBrowserTreeItem *>()) {
  if (parentItem_ &&
      itemDataType_ != ObjectBrowserTreeItem::ObjectType::Plot2DLayout)
    parentItem_->childItems_.append(this);
}

ObjectBrowserTreeItem::~ObjectBrowserTreeItem() {
  // qDebug() << "destroying" << static_cast<int>(itemDataType_);
}

void ObjectBrowserTreeItem::addChildAppropriately() {
  if (!parentItem_) return;
  if (parentItem_->childItems_.contains(this)) return;
  switch (itemDataType_) {
    case ObjectBrowserTreeItem::ObjectType::Plot2DLayout: {
      Layout2D *layout = parentItem_->itemData_.value<Layout2D *>();
      AxisRect2D *axisrect = itemData_.value<AxisRect2D *>();
      // extract axisrect items
      ObjectBrowserTreeItem *successoritem = nullptr;
      if (layout && axisrect) {
        QList<AxisRect2D *> axisrectlist = layout->getAxisRectList();
        for (int i = 0; i < axisrectlist.count(); i++) {
          if (axisrectlist.at(i) == axisrect && axisrect != axisrectlist.last())
            successoritem = axisrectlist.at(i + 1);
        }
      }
      // insert child in order
      (successoritem == nullptr)
          ? parentItem_->childItems_.append(this)
          : parentItem_->childItems_.insert(
                parentItem_->childItems_.indexOf(successoritem), this);
    } break;
    default:
      qDebug() << "AppendChildAppropriately() called but with no effect!";
      break;
  }
}

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
  switch (itemDataType_) {
    case ObjectBrowserTreeItem::ObjectType::Plot2DLayout: {
      Layout2D *layout = parentItem_->itemData_.value<Layout2D *>();
      AxisRect2D *axisrect = itemData_.value<AxisRect2D *>();
      if (!layout || !axisrect) return QVariant();
      QPair<int, int> rowcol = layout->getAxisRectRowCol(axisrect);
      int index = layout->getLayoutRectGridIndex(rowcol) + 1;
      switch (role) {
        case Qt::ItemDataRole::DisplayRole:
        case Qt::ItemDataRole::ToolTipRole:
          return QVariant(getItemName().arg(
              QString::number(index), QString::number(rowcol.first + 1),
              QString::number(rowcol.second + 1)));
          break;
        case Qt::ItemDataRole::DecorationRole:
          return getItemIcon();
          break;
        default:
          return QVariant();
      }
    } break;
    default: {
      switch (role) {
        case Qt::ItemDataRole::DisplayRole:
          return getItemName();
          break;
        case Qt::ItemDataRole::ToolTipRole:
          return getItemTooltip();
          break;
        case Qt::ItemDataRole::DecorationRole:
          return getItemIcon();
          break;
        default:
          return QVariant();
      }
    } break;
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

  return -1;
}

ObjectBrowserTreeItem *ObjectBrowserTreeItem::parentItem() {
  return parentItem_;
}
