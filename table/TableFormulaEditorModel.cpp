/***************************************************************************
    File                 : TableFormulaEditorModel.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Model class wrapping a TableModel to edit the formulas

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

#include "TableFormulaEditorModel.h"
#include <QColor>
#include <QBrush>

TableFormulaEditorModel::TableFormulaEditorModel( TableModel * table_model, QObject * parent )
	: QAbstractTableModel( parent ), d_table_model(table_model)
{
}

TableFormulaEditorModel::~TableFormulaEditorModel()
{
}

Qt::ItemFlags TableFormulaEditorModel::flags(const QModelIndex & index ) const
{
	if (index.isValid())
	{
		if (index.column() % 2 == 0)
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
		else
			return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	}
	else
		return Qt::ItemIsEnabled;
}


QVariant TableFormulaEditorModel::data(const QModelIndex &index, int role) const
{
	if( !index.isValid() )
		return QVariant();
	
	int row = index.row();
	int col = index.column();

	switch(role)
	{
		case Qt::ToolTipRole:
		case Qt::EditRole:
		case Qt::DisplayRole:
		if (col % 2 == 0)
			return QVariant(d_table_model->column(col/2)->formula(row));
		else
			return QVariant(d_table_model->data(d_table_model->index(row, col/2+1, QModelIndex()), role));
		case Qt::BackgroundRole:
			{
				if(col % 2 == 1)
					return QVariant(QBrush(QColor(0xdd,0xdd,0xdd)));
				else
					return QVariant();
			}
	}

	return QVariant();
}

QVariant TableFormulaEditorModel::headerData(int section, Qt::Orientation orientation,
		int role) const
{
	if(orientation == Qt::Horizontal)
		return d_table_model->headerData(section/2, orientation, role);
	else
		return d_table_model->headerData(section, orientation, role);
}

int TableFormulaEditorModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return d_table_model->rowCount();
}

int TableFormulaEditorModel::columnCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent)
	return d_table_model->columnCount()*2;
}

bool TableFormulaEditorModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (!index.isValid())
		return false;

	int row = index.row();
	int col = index.column();

	if(col % 2 != 0) return;

	switch(role)
	{  
		case Qt::EditRole:
			d_table_model->column(col/2)->setFormula(row, value.toString());  
			emit dataChanged(index, index);
			return true;
	}

	return false;
}

