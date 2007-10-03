/***************************************************************************
    File                 : AspectTreeModel.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Represents a tree of AbstractAspect objects as a
                           Qt item model.

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
#include "AspectTreeModel.h"
#include <QDateTime>
#include <QIcon>

AspectTreeModel::AspectTreeModel(AbstractAspect *root, QObject *parent)
	: QAbstractItemModel(parent), d_root(root)
{
	d_root->addAspectObserver(this);
}

AspectTreeModel::~AspectTreeModel()
{
	d_root->removeAspectObserver(this);
}

QModelIndex AspectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent)) return QModelIndex();
	AbstractAspect *parent_aspect = parent.isValid() ? static_cast<AbstractAspect*>(parent.internalPointer()) : d_root;
	AbstractAspect *child_aspect = parent_aspect->child(row);
	if (!child_aspect) return QModelIndex();
	return createIndex(row, column, child_aspect);
}

QModelIndex AspectTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid()) return QModelIndex();
	AbstractAspect *parent_aspect = static_cast<AbstractAspect*>(index.internalPointer())->parentAspect();
	if (!parent_aspect || !parent_aspect->parentAspect()) return QModelIndex();
	return indexOfAspect(parent_aspect);
}

int AspectTreeModel::rowCount(const QModelIndex &parent) const
{
	AbstractAspect *parent_aspect = parent.isValid() ? static_cast<AbstractAspect*>(parent.internalPointer()) : d_root;
	return parent_aspect->childCount();
}

int AspectTreeModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 4;
}

QVariant AspectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(orientation != Qt::Horizontal || role != Qt::DisplayRole) return QVariant();
	switch(section) {
		case 0: return tr("Name");
		case 1: return tr("Type");
		case 2: return tr("Created");
		case 3: return tr("Comment");
		default: return QVariant();
	}
}

QVariant AspectTreeModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) return QVariant();
	AbstractAspect *aspect = static_cast<AbstractAspect*>(index.internalPointer());
	switch(role) {
		case Qt::DisplayRole:
		case Qt::EditRole:
			switch(index.column()) {
				case 0: return aspect->name();
				case 1: return aspect->className();
				case 2: return aspect->creationTime().toString();
				case 3: return aspect->comment();
				default: return QVariant();
			}
		case Qt::ToolTipRole:
			return aspect->caption();
		case Qt::DecorationRole:
			return index.column() == 0 ? aspect->icon() : QIcon();
		case ContextMenuRole:
			return QVariant::fromValue((QWidget*)aspect->createContextMenu());
		default:
			return QVariant();
	}
}

Qt::ItemFlags AspectTreeModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) return 0;
	Qt::ItemFlags result = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	if (index.column() == 0 || index.column() == 3)
		result |= Qt::ItemIsEditable;
	return result;
}

void AspectTreeModel::aspectDescriptionChanged(AbstractAspect *aspect)
{
	emit dataChanged(indexOfAspect(aspect), indexOfAspect(aspect, 3));
}

void AspectTreeModel::aspectAboutToBeAdded(AbstractAspect *parent, int index)
{
	beginInsertRows(indexOfAspect(parent), index, index);
}

void AspectTreeModel::aspectAdded(AbstractAspect *aspect)
{
	Q_UNUSED(aspect);
	endInsertRows();
	emit dataChanged(indexOfAspect(aspect->parentAspect()), indexOfAspect(aspect->parentAspect(),3));
}

void AspectTreeModel::aspectAboutToBeRemoved(AbstractAspect *aspect)
{
	beginRemoveRows(indexOfAspect(aspect->parentAspect()), aspect->index(), aspect->index());
}

void AspectTreeModel::aspectRemoved(AbstractAspect *parent, int index)
{
	Q_UNUSED(parent); Q_UNUSED(index);
	endRemoveRows();
}

bool AspectTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	if (!index.isValid() || role != Qt::EditRole) return false;
	AbstractAspect *aspect = static_cast<AbstractAspect*>(index.internalPointer());
	switch (index.column()) {
		case 0:
			aspect->setName(value.toString());
			break;
		case 3:
			aspect->setComment(value.toString());
			break;
		default:
			return false;
	}
	emit dataChanged(index, index);
	return true;
}
