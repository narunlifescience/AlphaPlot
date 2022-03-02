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

#include "ObjectBrowserTreeItem.h"
#include <QIcon>

ObjectBrowserTreeItemModel::ObjectBrowserTreeItemModel(
    ObjectBrowserTreeItem *item, QObject *parent)
    : QAbstractItemModel(parent) {
  Q_ASSERT(item != nullptr);
  rootItem_ = item;
}

ObjectBrowserTreeItemModel::~ObjectBrowserTreeItemModel() {
  if (rootItem_->dataType() == ObjectBrowserTreeItem::ObjectType::None)
    delete rootItem_;
}

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
  if (orientation == Qt::Horizontal) switch (role) {
      case Qt::ItemDataRole::DisplayRole:
        return rootItem_->getItemName();
        break;
      case Qt::ItemDataRole::DecorationRole:
        return rootItem_->getItemIcon();
        break;
      case Qt::ItemDataRole::ToolTipRole:
        return rootItem_->getItemTooltip();
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
  if (parent.column() > 0) return 0;

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
