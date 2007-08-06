/***************************************************************************
    File                 : TableItemDelegate.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Item delegate for TableView

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

#include <QPainter>
#include <QModelIndex>
#include "TableItemDelegate.h"
#include "TableModel.h"

TableItemDelegate::TableItemDelegate()
{
	d_masking_color = QColor(0xff,0,0);
}

void TableItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                    const QModelIndex &index) const
{
	 emit userInput(index);
	 QItemDelegate::setModelData(editor, model, index);
}

void TableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const
{
	QItemDelegate::paint(painter, option, index);
	if (!index.data(TableModel::MaskingRole).toBool())
		return;
	painter->save();
	// masked cells are displayed as hatched
	painter->fillRect(option.rect, QBrush(d_masking_color, Qt::BDiagPattern));
	painter->restore();
}
