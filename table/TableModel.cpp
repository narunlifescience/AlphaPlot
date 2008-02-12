/***************************************************************************
    File                 : TableModel.cpp
    Project              : SciDAVis
    Description          : Model storing data for a table
    --------------------------------------------------------------------
    Copyright            : (C) 2007 Tilman Hoener zu Siederdissen (thzs*gmx.net)
                           (replace * with @ in the email addresses) 

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

#include "TableModel.h"
#include "tablecommands.h"
#include <QString>
#include <QDate>
#include <QTime>
#include <QBrush>
#include <QFont>
#include <QIcon>
#include <QPixmap>
#include <QFontMetrics>
#include <QUndoStack>
#include <QItemSelectionModel>


TableModel::TableModel( QObject * parent )
	: QAbstractItemModel( parent )
{
	d_column_count = 0;
	d_row_count = 0;
	d_name = "TableModel";
	d_selection_model = new QItemSelectionModel(this, this);
}

TableModel::~TableModel()
{
}

Qt::ItemFlags TableModel::flags(const QModelIndex & index ) const
{
	if (index.isValid())
		return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
	else
		return Qt::ItemIsEnabled;
}


QVariant TableModel::data(const QModelIndex &index, int role) const
{
	if( !index.isValid() )
		return QVariant();
	
	int row = index.row();
	int column = index.column();
	Column* col_ptr = d_columns.value(column);
	if(!col_ptr)
		return QVariant();

	QString postfix;
	switch(role)
	{
		case Qt::ToolTipRole:
				if(col_ptr->isMasked(row))
					postfix = " " + tr("(masked)");
				if(col_ptr->isInvalid(row))
					return QVariant(tr("invalid cell","tooltip string for invalid rows") + postfix);
		case Qt::EditRole:
				if(col_ptr->isInvalid(row))
					return QVariant();
		case Qt::DisplayRole:
			{
				if(col_ptr->isInvalid(row))
					return QVariant(tr("invalid","string for invalid rows"));
				
				AbstractSimpleFilter *out_fltr = col_ptr->outputFilter();
				out_fltr->input(0, col_ptr);
				return QVariant(out_fltr->textAt(row) + postfix);
			}
		case Qt::ForegroundRole:
			{
				if(col_ptr->isInvalid(index.row()))
					return QVariant(QBrush(QColor(0xff,0,0))); // invalid -> red letters
				else
					return QVariant(QBrush(QColor(0,0,0)));
			}
		case MaskingRole:
			return QVariant(col_ptr->isMasked(row));
		case FormulaRole:
			return QVariant(col_ptr->formula(row));
	}

	return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
		int role) const
{

	if( !( (role == Qt::ToolTipRole) || (role == Qt::DecorationRole) ||
	       (role == Qt::DisplayRole || role == Qt::EditRole) ) )
		return QVariant();
		
	if(orientation == Qt::Horizontal)
	{
		if( role == Qt::DecorationRole) 
			return QVariant(d_columns.at(section)->icon());
		else 
			return d_horizontal_header_data.at(section);
	}
	else if (orientation == Qt::Vertical)
		return d_vertical_header_data.at(section);

	return QVariant();
}

int TableModel::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return d_row_count;
}

int TableModel::columnCount(const QModelIndex & parent) const
{
	Q_UNUSED(parent)
	return d_column_count;
}

bool TableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
	if (!index.isValid())
		return false;

	int row = index.row();

	switch(role)
	{  
		case Qt::EditRole:
			{
				Column* col_ptr = d_columns.at(index.column());
				AbstractSimpleFilter *in_fltr = col_ptr->inputFilter();
				Column* sd( new Column("temp", QStringList(value.toString())) );
				in_fltr->input(0, sd);
				// remark: the validity of the cell is determined by the input filter
				col_ptr->copy(in_fltr->output(0), 0, row, 1);  
				emit dataChanged(index, index);
				return true;
			}
		case MaskingRole:
			{
				d_columns.at(index.column())->setMasked(row, value.toBool());  
				emit dataChanged(index, index);
				return true;
			}
		case FormulaRole:
			{
				d_columns.at(index.column())->setFormula(row, value.toString());  
				emit dataChanged(index, index);
				return true;
			}
	}
	
	return false;
}

QModelIndex TableModel::index(int row, int column, const QModelIndex &parent) const
{
	Q_UNUSED(parent)
	return createIndex(row, column);
}

QModelIndex TableModel::parent(const QModelIndex & child) const
{
	Q_UNUSED(child)
    return QModelIndex();
}


AbstractColumn* TableModel::output(int port) const
{
	if( (port < 0) || (port >= d_column_count) || !d_columns.value(port))
		return 0;
	
	return d_columns.at(port);
}

void TableModel::replaceColumns(int first, QList<Column*> new_cols)
{
	if( (first < 0) || (first + new_cols.size() > d_column_count) )
		return;

	int count = new_cols.size();
	emit columnsAboutToBeReplaced(first, new_cols.count());
	for(int i=0; i<count; i++)
	{
		int rows = new_cols.at(i)->rowCount();
		if(rows > d_row_count)
			setRowCount(rows); 

		if(d_columns.at(first+i))
			d_columns.at(first+i)->notifyReplacement(new_cols.at(i));

		d_columns[first+i] = new_cols.at(i);
		connectColumn(new_cols.at(i));
	}
	updateHorizontalHeader(first, first+count-1);
	emit columnsReplaced(first, new_cols.count());
	emit dataChanged(index(0, first), index(d_row_count-1, first+count-1));
}

void TableModel::emitDataChanged(int top, int left, int bottom, int right)
{
	emit dataChanged(index(top, left), index(bottom, right));
}

void TableModel::emitColumnChanged(Column * col)
{
	int index = columnIndex(col);
	emitDataChanged(0, index, col->rowCount()-1, index);
}

void TableModel::insertColumns(int before, QList<Column*> cols)
{
	int count = cols.count();

	if( (count < 1) || (before > d_column_count) )
		return;

	Q_ASSERT(before >= 0);

	int i, rows;
	for(i=0; i<count; i++)
	{
		rows = cols.at(i)->rowCount();
		if(rows > d_row_count)
			setRowCount(rows); 
	}

	beginInsertColumns(QModelIndex(), before, before+count-1);
	emit columnsAboutToBeInserted(before, cols);
	for(int i=count-1; i>=0; i--)
	{
		d_columns.insert(before, cols.at(i));
		connectColumn(cols.at(i));
	}
	d_column_count += count;
	updateHorizontalHeader(before, before+count-1);
	emit columnsInserted(before, cols.count());
	endInsertColumns();	
}

void TableModel::removeColumns(int first, int count)
{
	if( (count < 1) || (first >= d_column_count) )
		return;

	Q_ASSERT(first >= 0);

	Q_ASSERT(first+count <= d_column_count);

	beginRemoveColumns(QModelIndex(), first, first+count-1);
	emit columnsAboutToBeRemoved(first, count);
	for(int i=count-1; i>=0; i--)
	{
		disconnectColumn(d_columns.at(first));
		d_columns.removeAt(first);
	}
	d_column_count -= count;
	updateHorizontalHeader(first, d_column_count);
	emit columnsRemoved(first, count);
	endRemoveColumns();	
}

void TableModel::appendColumns(QList<Column*> cols)
{
	insertColumns(d_column_count, cols);
}

void TableModel::setRowCount(int rows)
{
	int diff = rows - d_row_count;
	if(diff == 0) 
		return;

	if(diff > 0)
	{
		beginInsertRows(QModelIndex(), d_row_count, rows-1);
		d_row_count = rows;
		updateVerticalHeader(d_row_count - diff);
		endInsertRows();
	}
	else
	{
		beginRemoveRows(QModelIndex(), rows, d_row_count-1);
		d_row_count = rows;
		endRemoveRows();
	}
}

void TableModel::updateVerticalHeader(int start_row)
{
	int current_size = d_vertical_header_data.size(), i;
	for(i=start_row; i<current_size; i++)
		d_vertical_header_data.replace(i, QString::number(i+1));
	for(; i<d_row_count; i++)
		d_vertical_header_data << QString::number(i+1);
	emit headerDataChanged(Qt::Vertical, start_row, d_row_count -1);	
}

void TableModel::updateHorizontalHeader(int start_col, int end_col)
{
	while(d_horizontal_header_data.size() < d_column_count)
		d_horizontal_header_data << QString();

	if(numColsWithPD(SciDAVis::X)>1)
	{
		int x_cols = 0;
		for (int i=0; i<d_column_count; i++)
		{
			if (d_columns.at(i)->plotDesignation() == SciDAVis::X)
				composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[X" + QString::number(++x_cols) +"]");
			else if (d_columns.at(i)->plotDesignation() == SciDAVis::Y)
			{
				if(x_cols>0)
					composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[Y"+ QString::number(x_cols) +"]");
				else
					composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[Y]");
			}
			else if (d_columns.at(i)->plotDesignation() == SciDAVis::Z)
			{
				if(x_cols>0)
					composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[Z"+ QString::number(x_cols) +"]");
				else
					composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[Z]");
			}
			else if (d_columns.at(i)->plotDesignation() == SciDAVis::xErr)
			{
				if(x_cols>0)
					composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[xEr"+ QString::number(x_cols) +"]");
				else
					composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[xEr]");
			}
			else if (d_columns.at(i)->plotDesignation() == SciDAVis::yErr)
			{
				if(x_cols>0)
					composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[yEr"+ QString::number(x_cols) +"]");
				else
					composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[yEr]");
			}
			else
				composeColumnHeader(i, d_columns.at(i)->columnLabel());
		}
	}
	else
	{
		for (int i=0; i<d_column_count; i++)
		{
			if (d_columns.at(i)->plotDesignation() == SciDAVis::X)
				composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[X]");
			else if(d_columns.at(i)->plotDesignation() == SciDAVis::Y)
				composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[Y]");
			else if(d_columns.at(i)->plotDesignation() == SciDAVis::Z)
				composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[Z]");
			else if(d_columns.at(i)->plotDesignation() == SciDAVis::xErr)
				composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[xEr]");
			else if(d_columns.at(i)->plotDesignation() == SciDAVis::yErr)
				composeColumnHeader(i, d_columns.at(i)->columnLabel()+"[yEr]");
			else
				composeColumnHeader(i, d_columns.at(i)->columnLabel());
		}
	}
	emit headerDataChanged(Qt::Horizontal, start_col, end_col);	
}

void TableModel::composeColumnHeader(int col, const QString& label)
{
	if (col >= d_horizontal_header_data.size())
		d_horizontal_header_data << label;
	else
		d_horizontal_header_data.replace(col, label);
}

QString TableModel::columnHeader(int col)
{
	return headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();
}

int TableModel::numColsWithPD(SciDAVis::PlotDesignation pd)
{
	int count = 0;
	
	for (int i=0; i<d_column_count; i++)
		if(d_columns.at(i)->plotDesignation() == pd)
			count++;
	
	return count;
}

void TableModel::connectColumn(Column* col)
{
	connect(col, SIGNAL(descriptionChanged(AbstractColumn *)), this, 
			SLOT(handleDescriptionChange(AbstractColumn *)));
	connect(col, SIGNAL(plotDesignationChanged(AbstractColumn *)), this, 
			SLOT(handlePlotDesignationChange(AbstractColumn *)));
	connect(col, SIGNAL(modeChanged(AbstractColumn *)), this, 
			SLOT(handleDataChange(AbstractColumn *)));
	connect(col, SIGNAL(dataChanged(AbstractColumn *)), this, 
			SLOT(handleDataChange(AbstractColumn *)));
	connect(col, SIGNAL(modeChanged(AbstractColumn *)), this, 
			SLOT(handleDescriptionChange(AbstractColumn *)));
	connect(col, SIGNAL(rowsAboutToBeInserted(AbstractColumn *, int, int)), this, 
			SLOT(handleRowsAboutToBeInserted(AbstractColumn *,int,int)));
	connect(col, SIGNAL(rowsInserted(AbstractColumn *, int, int)), this, 
			SLOT(handleRowsInserted(AbstractColumn *,int,int))); 
	connect(col, SIGNAL(rowsAboutToBeRemoved(AbstractColumn *, int, int)), this, 
			SLOT(handleRowsAboutToBeRemoved(AbstractColumn *,int,int))); 
	connect(col, SIGNAL(rowsRemoved(AbstractColumn *, int, int)), this, 
			SLOT(handleRowsRemoved(AbstractColumn *,int,int))); 
	connect(col, SIGNAL(maskingChanged(AbstractColumn *)), this, 
			SLOT(handleDataChange(AbstractColumn *))); 
}

void TableModel::disconnectColumn(Column* col)
{
	disconnect(col, 0, this, 0);
}

void TableModel::handleDescriptionChange(AbstractColumn * col)
{
	int index = columnIndex(static_cast<Column *>(col));
	updateHorizontalHeader(index, index);
}

void TableModel::handlePlotDesignationChange(AbstractColumn * col)
{
	int index = columnIndex(static_cast<Column *>(col));
	updateHorizontalHeader(index, d_column_count-1);
}

void TableModel::handleDataChange(AbstractColumn * col)
{
	int index = columnIndex(static_cast<Column *>(col));
	emitDataChanged(0, index, d_row_count-1, index);	
}

void TableModel::handleRowsAboutToBeInserted(AbstractColumn * col, int before, int count)
{
	int new_size = col->rowCount() + count; 
	if(before <= col->rowCount() && new_size > d_row_count)
	{
		emit requestResize(new_size);
		if(d_row_count != new_size) // request was ignored
			setRowCount(new_size);
	}
}

void TableModel::handleRowsInserted(AbstractColumn * col, int before, int count)
{
	Q_UNUSED(count);
	int index = columnIndex(static_cast<Column *>(col));
	if(before <= col->rowCount())
		emitDataChanged(before, index, col->rowCount()-1, index);
}


void TableModel::handleRowsAboutToBeRemoved(AbstractColumn * col, int first, int count)
{
	Q_UNUSED(col);
	Q_UNUSED(first);
	Q_UNUSED(count);
}

void TableModel::handleRowsRemoved(AbstractColumn * col, int first, int count)
{
	Q_UNUSED(count);
	int index = columnIndex(static_cast<Column *>(col));
	emitDataChanged(first, index, d_row_count-1, index);
}

void TableModel::moveColumn(int from, int to)
{
	if( from < 0 || from >= d_column_count) return;
	if( to < 0 || to >= d_column_count) return;
	
	d_columns.move(from, to);
	updateHorizontalHeader(from, to);
	emitDataChanged(0, from, d_column_count-1, to);
}

int TableModel::selectedColumnCount(bool full)
{
	int count = 0;
	int cols = columnCount();
	for (int i=0; i<cols; i++)
		if(isColumnSelected(i, full)) count++;
	return count;
}

int TableModel::selectedColumnCount(SciDAVis::PlotDesignation pd)
{
	int count = 0;
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		if( isColumnSelected(i, false) && (column(i)->plotDesignation() == pd) ) count++;

	return count;
}

bool TableModel::isColumnSelected(int col, bool full)
{
	if(full)
		return d_selection_model->isColumnSelected(col, QModelIndex());
	else
		return d_selection_model->columnIntersectsSelection(col, QModelIndex());
}

QList<Column*> TableModel::selectedColumns(bool full)
{
	QList<Column*> list;
	int cols = columnCount();
	for (int i=0; i<cols; i++)
		if(isColumnSelected(i, full)) list << column(i);

	return list;
}

int TableModel::selectedRowCount(bool full)
{
	int count = 0;
	int rows = rowCount();
	for (int i=0; i<rows; i++)
		if(isRowSelected(i, full)) count++;
	return count;
}

bool TableModel::isRowSelected(int row, bool full)
{
	if(full)
		return d_selection_model->isRowSelected(row, QModelIndex());
	else
		return d_selection_model->rowIntersectsSelection(row, QModelIndex());
}

void TableModel::selectAll()
{
	QItemSelection sel(index(0, 0), index(rowCount()-1, columnCount()-1));
	d_selection_model->select(sel, QItemSelectionModel::Select);
}

int TableModel::firstSelectedColumn(bool full)
{
	int cols = columnCount();
	for (int i=0; i<cols; i++)
	{
		if(isColumnSelected(i, full))
			return i;
	}
	return -1;
}

int TableModel::lastSelectedColumn(bool full)
{
	int cols = columnCount();
	for(int i=cols-1; i>=0; i--)
		if(isColumnSelected(i, full)) return i;

	return -1;
}

int TableModel::firstSelectedRow(bool full)
{
	int rows = rowCount();
	for (int i=0; i<rows; i++)
	{
		if(isRowSelected(i, full))
			return i;
	}
	return -1;
}

int TableModel::lastSelectedRow(bool full)
{
	int rows = rowCount();
	for(int i=rows-1; i>=0; i--)
		if(isRowSelected(i, full)) return i;

	return -1;
}

bool TableModel::isCellSelected(int row, int col)
{
	if(row < 0 || col < 0 || row >= rowCount() || col >= columnCount()) return false;

	return d_selection_model->isSelected( index(row, col) );
}


