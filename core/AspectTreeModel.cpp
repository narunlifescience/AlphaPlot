/***************************************************************************
    File                 : AspectTreeModel.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
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
#include "AspectView.h"
#include <QDateTime>
#include <QIcon>
#include <QMenu>

AspectTreeModel::AspectTreeModel(shared_ptr<AbstractAspect> root, QObject *parent)
	: QAbstractItemModel(parent), d_root(root)
{
	connect(d_root->abstractAspectSignalEmitter(), SIGNAL(aspectDescriptionChanged(AbstractAspect *)), 
		this, SLOT(aspectDescriptionChanged(AbstractAspect *)));
	connect(d_root->abstractAspectSignalEmitter(), SIGNAL(aspectAboutToBeAdded(AbstractAspect *, int)), 
		this, SLOT(aspectAboutToBeAdded(AbstractAspect *, int)));
	connect(d_root->abstractAspectSignalEmitter(), SIGNAL(aspectAboutToBeRemoved(AbstractAspect *, int)), 
		this, SLOT(aspectAboutToBeRemoved(AbstractAspect *, int)));
	connect(d_root->abstractAspectSignalEmitter(), SIGNAL(aspectAdded(AbstractAspect *, int)), 
		this, SLOT(aspectAdded(AbstractAspect *, int)));
	connect(d_root->abstractAspectSignalEmitter(), SIGNAL(aspectRemoved(AbstractAspect *, int)), 
		this, SLOT(aspectRemoved(AbstractAspect *, int)));
}

AspectTreeModel::~AspectTreeModel()
{
	disconnect(d_root->abstractAspectSignalEmitter(),0,this,0);
}

QModelIndex AspectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent)) return QModelIndex();
	if(!parent.isValid())
	{
		if(row != 0) return QModelIndex();
		return createIndex(row, column, d_root.get());
	}
	AbstractAspect *parent_aspect = static_cast<AbstractAspect*>(parent.internalPointer());
	AbstractAspect *child_aspect = parent_aspect->child(row).get();
	if (!child_aspect) return QModelIndex();
	return createIndex(row, column, child_aspect);
}

QModelIndex AspectTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid()) return QModelIndex();
	AbstractAspect *parent_aspect = static_cast<AbstractAspect*>(index.internalPointer())->parentAspect();
	if (!parent_aspect) return QModelIndex();
	return modelIndexOfAspect(parent_aspect);
}

int AspectTreeModel::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid()) return 1;
	AbstractAspect *parent_aspect =  static_cast<AbstractAspect*>(parent.internalPointer());
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
			{
				QMenu *menu = new QMenu();
				if(aspect->view()) aspect->view()->createContextMenu(menu);
				aspect->createContextMenu(menu);
				return QVariant::fromValue(static_cast<QWidget *>(menu));
			}
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
	emit dataChanged(modelIndexOfAspect(aspect), modelIndexOfAspect(aspect, 3));
}

void AspectTreeModel::aspectAboutToBeAdded(AbstractAspect *parent, int index)
{
	beginInsertRows(modelIndexOfAspect(parent), index, index);
}

void AspectTreeModel::aspectAdded(AbstractAspect *parent, int index)
{
	Q_UNUSED(index)
	endInsertRows();
	emit dataChanged(modelIndexOfAspect(parent), modelIndexOfAspect(parent, 3));
}

void AspectTreeModel::aspectAboutToBeRemoved(AbstractAspect *parent, int index)
{
	beginRemoveRows(modelIndexOfAspect(parent), index, index);
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
