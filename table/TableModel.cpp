/***************************************************************************
    File                 : TableModel.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Model storing data for a table

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
//#include "tablecommands.h"
#include <QString>
#include <QDate>
#include <QTime>
#include <QBrush>
#include <QFont>
#include <QFontMetrics>
#include <QUndoStack>


TableModel::TableModel( QObject * parent )
	: QAbstractItemModel( parent )
{
	d_column_count = 0;
	d_row_count = 0;
	d_show_comments = false;
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
	shared_ptr<Column> col_ptr = d_columns.value(column);
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
				
				shared_ptr<AbstractSimpleFilter> out_fltr = col_ptr->outputFilter();
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
	}

	return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation,
		int role) const
{
	if( !( (role == Qt::ToolTipRole) ||
	       (role == Qt::DisplayRole || role == Qt::EditRole) ) )
		return QVariant();
		
	if(orientation == Qt::Horizontal)
		return d_horizontal_header_data.at(section);
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
	
	if(role == Qt::EditRole)
	{  
			shared_ptr<Column> col_ptr = d_columns.at(index.column());
			shared_ptr<AbstractSimpleFilter> in_fltr = d_columns.at(index.column())->inputFilter();
			shared_ptr<Column> sd(new Column("temp", SciDAVis::Text));
			sd->setTextAt(0, value.toString());
			in_fltr->input(0, sd);
			// remark: the validity of the cell is determined by the input filter
			col_ptr->copy(in_fltr->output(0), 0, row, 1);  
			emit dataChanged(index, index);
			return true;
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


shared_ptr<AbstractColumn> TableModel::output(int port) const
{
	if( (port < 0) || (port >= d_column_count) || !d_columns.value(port))
		return shared_ptr<AbstractColumn>();
	
	return d_columns.at(port);
}

void TableModel::replaceColumns(int first, QList< shared_ptr<Column> > new_cols)
{
	if( (first < 0) || (first + new_cols.size() > d_column_count) )
		return;

	int count = new_cols.size();
	emit columnsAboutToBeReplaced(first, new_cols.count());
	for(int i=0; i<count; i++)
	{
		int rows = new_cols.at(i)->rowCount();
		if(rows > d_row_count)
			appendRows(rows-d_row_count); // append rows to resize table

		if(d_columns.at(first+i))
			d_columns.at(first+i)->notifyReplacement(new_cols.at(i));

		d_columns[first+i] = new_cols.at(i);
	}
	updateHorizontalHeader(first, first+count-1);
	emit columnsReplaced(first, new_cols.count());
	emit dataChanged(index(0, first, QModelIndex()), index(d_row_count-1, first+count-1, QModelIndex()));
}

void TableModel::emitDataChanged(int top, int left, int bottom, int right)
{
	emit dataChanged(index(top, left, QModelIndex()), index(bottom, right, QModelIndex()));
}

void TableModel::insertColumns(int before, QList< shared_ptr<Column> > cols)
{
	int count = cols.count();

	if(count < 1) 
		return;

	if(before < 0)
		before = 0;
	if(before > d_column_count)
		before = d_column_count;

	int i, rows;
	for(i=0; i<count; i++)
	{
		rows = cols.at(i)->rowCount();
		if(rows > d_row_count)
			appendRows(rows-d_row_count); // append rows to resize table
	}

	beginInsertColumns(QModelIndex(), before, before+count-1);
	emit columnsAboutToBeInserted(before, cols);
	for(int i=count-1; i>=0; i--)
		d_columns.insert(before, cols.at(i));
	d_column_count += count;
	updateHorizontalHeader(before, before+count-1);
	columnsInserted(before, cols.count());
	endInsertColumns();	
}

void TableModel::removeColumns(int first, int count)
{
	if(count < 1) 
		return;
	if(first >= d_column_count)
		return;
	if(first < 0)
		return;

	if(first+count > d_column_count)
		count = d_column_count - first;

	beginRemoveColumns(QModelIndex(), first, first+count-1);
	emit columnsAboutToBeRemoved(first, count);
	for(int i=count-1; i>=0; i--)
		d_columns.removeAt(first);
	d_column_count -= count;
	updateHorizontalHeader(first, d_column_count);
	emit columnsRemoved(first, count);
	endRemoveColumns();	
}

void TableModel::appendColumns(QList< shared_ptr<Column>  > cols)
{
	insertColumns(d_column_count, cols);
}

void TableModel::removeRows(int first, int count)
{
	if( (count < 1) || (first < 0) || (first >= d_row_count) )
		return;

	if(first+count > d_row_count)
		count = d_row_count - first;

	beginRemoveRows(QModelIndex(), first, first+count-1);
	for(int col=0; col<d_column_count; col++)
	{
		int row_count = d_columns.at(col)->rowCount();
		if(row_count > first)
		{
			int current_count = count;
			if(first+current_count > row_count)
				current_count = row_count - first;
			d_columns.at(col)->removeRows(first, current_count);
		}
	}
	d_row_count -= count;
	updateVerticalHeader(first);
	endRemoveRows();
}

void TableModel::insertRows(int first, int count)
{
	if(count < 1) 
		return;

	if(first < 0)
		first = 0;

	if(first > d_row_count)
		appendRows(count);
	else
	{
		beginInsertRows(QModelIndex(), first, first+count-1);
		for(int col=0; col<d_column_count; col++)
		{
			if(d_columns.at(col)->rowCount() <= first) // no need to append empty rows
				continue;
			d_columns.at(col)->insertEmptyRows(first, count);
		}
		d_row_count += count;
		updateVerticalHeader(first);
		endInsertRows();
	}

}

void TableModel::appendRows(int count)
{
	if(count < 1) 
		return;

	beginInsertRows(QModelIndex(), d_row_count, d_row_count+count-1);
	d_row_count += count;
	updateVerticalHeader(d_row_count - count);
	endInsertRows();
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
	QString s = label;
	if (d_show_comments)
	{
		int lines = 10; // TODO: this needs improvement
		s.remove("\n");
		s += "\n" + QString(lines, '_') + "\n"  + d_columns.at(col)->columnComment();
	}
	
	if (col >= d_horizontal_header_data.size())
		d_horizontal_header_data << s;
	else
		d_horizontal_header_data.replace(col, s);
}

//TODO: header update must be done by a signal connection
/*
void TableModel::setColumnLabel(int column, const QString& label)
{
	d_columns.at(column)->setLabel(label);
	updateHorizontalHeader(column, column);
}
*/

//TODO: header update must be done by a signal connection 
/*
void TableModel::setColumnComment(int column, const QString& comment)
{
	d_columns.at(column)->setComment(comment);
	updateHorizontalHeader(column, column);
}
*/

//TODO: header update must be done by a signal connection 
/*
void TableModel::setColumnPlotDesignation(int column, SciDAVis::PlotDesignation pd)
{
	d_columns.at(column)->setPlotDesignation(pd);
	updateHorizontalHeader(column, d_column_count-1);
}
*/
	
void TableModel::showComments(bool on)
{
	if (d_show_comments == on)
		return;

	d_show_comments = on;
	updateHorizontalHeader(0, d_column_count-1);
}

bool TableModel::areCommentsShown()
{
	return d_show_comments;
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

// TODO: this must be done elsewhere
//void TableModel::handleUserInput(const QModelIndex& index)
//{
//		d_undo_stack->push(new TableUserInputCmd(this, index) );		
//}

