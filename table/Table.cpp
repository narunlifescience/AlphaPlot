/***************************************************************************
    File                 : Table.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : Table worksheet class

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
#include "Table.h"

#include <QItemSelectionModel>
#include <QTime>
#include <QtGlobal>
#include <QHBoxLayout>
#include <QShortcut>
#include <QApplication>
#include <stdlib.h> // for RAND_MAX

#include "TableModel.h"
#include "TableView.h"
#include "tablecommands.h"
#include "core/AbstractDataSource.h"
#include "AbstractColumnData.h"
#include "DoubleColumnData.h"
#include "StringColumnData.h"
#include "DateTimeColumnData.h"
#include "core/AbstractFilter.h"
#include "core/datatypes/String2DoubleFilter.h"
#include "core/datatypes/Double2StringFilter.h"
#include "core/datatypes/DateTime2StringFilter.h"

#define OBSOLETE qDebug("obsolete Table function called");

Table::Table(AbstractScriptingEngine *engine, int rows, int cols, const QString& label, QWidget* parent, const char* name, Qt::WFlags f)
: MyWidget(label,parent,name,f), scripted(engine)
{
//###	d_selected_col = -1;
//###	d_saved_cells = 0;

	QDateTime dt = QDateTime::currentDateTime();
	setBirthDate(dt.toString(Qt::LocalDate));

	// create model and view
	d_table_model = new TableModel(this);
	d_table_view = new TableView(this, d_table_model, rows, cols);
	d_table_view->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	d_table_view->setFocusPolicy(Qt::StrongFocus);
	d_table_view->setFocus();

	d_table_view->setSelectionMode(QAbstractItemView::ExtendedSelection);

	// header stuff
	QHeaderView * v_header = d_table_view->verticalHeader();
	QHeaderView * h_header = d_table_view->horizontalHeader();

//###	connect(h_header, SIGNAL(sectionDoubleClicked(int)),
//###			this, SLOT(headerDoubleClickedHandler(int)));

//###	connect(h_header, SIGNAL(sectionResized(int, int, int)),
//###			this, SLOT(colWidthModified(int, int, int)));

	// main layout
	QHBoxLayout* main_layout = new QHBoxLayout(this);
	main_layout->setMargin(0);
	main_layout->addWidget(d_table_view);

	// initialized the columns
	for (int i=0; i<cols; i++)
	{
		d_table_model->setColumnLabel(i, QString::number(i+1));
		d_table_model->setColumnPlotDesignation(i, AbstractDataSource::Y);
	}
	if(cols > 0)
		d_table_model->setColumnPlotDesignation(0, AbstractDataSource::X);

	// calculate initial geometry
	int w=4*h_header->sectionSize(0);
	int h;
	if (rows>11)
		h=11*v_header->sectionSize(0);
	else
		h=(rows+1)*v_header->sectionSize(0);
	setGeometry(50, 50, w + 45, h);

	// keyboard shortcuts
	QShortcut * sel_all = new QShortcut(QKeySequence(tr("Ctrl+A", "Table: select all")), this);
	connect(sel_all, SIGNAL(activated()), d_table_view, SLOT(selectAll()));
}

QUndoStack *Table::undoStack() const
{
	return d_table_model->undoStack();
}

QList<AbstractDataSource *> Table::selectedColumns(bool full)
{
	QList<AbstractDataSource *> list;
	int cols = columnCount();
	for (int i=0; i<cols; i++)
		if(isColumnSelected(i, full)) list << d_table_model->output(i);

	return list;
}

int Table::selectedColumnCount(bool full)
{
	int count = 0;
	int cols = columnCount();
	for (int i=0; i<cols; i++)
		if(isColumnSelected(i, full)) count++;
	return count;
}

int Table::selectedColumnCount(AbstractDataSource::PlotDesignation pd)
{
	int count = 0;
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		if( (columnPlotDesignation(i) == pd) && (isColumnSelected(i, false)) ) count++;
	
	return count;
}


bool Table::isColumnSelected(int col, bool full)
{
	QItemSelectionModel * sel_model = d_table_view->selectionModel();
	if(full)
		return sel_model->isColumnSelected(col, QModelIndex());
	else
		return sel_model->columnIntersectsSelection(col, QModelIndex());
}

QString Table::columnLabel(int col)
{
	return d_table_model->columnLabel(col);
}

void Table::setColumnLabel(int col, const QString& label)
{
	undoStack()->push( new TableSetColumnLabelCmd(d_table_model, col, label) );
}

QString Table::columnComment(int col)
{
	return d_table_model->columnComment(col);
}

void Table::setColumnComment(int col, const QString& comment)
{
	undoStack()->push( new TableSetColumnCommentCmd(d_table_model, col, comment) );
}

AbstractDataSource::PlotDesignation Table::columnPlotDesignation(int col)
{
	return d_table_model->output(col)->plotDesignation();
}

void Table::setPlotDesignation(int col, AbstractDataSource::PlotDesignation pd)
{
	undoStack()->push( new TableSetColumnPlotDesignationCmd( d_table_model, col, pd) );
}

int Table::columnCount(AbstractDataSource::PlotDesignation pd)
{
	int count = 0;
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		if(columnPlotDesignation(i) == pd) count++;
	
	return count;
}

int Table::columnCount()
{
	return d_table_model->columnCount();
}

int Table::rowCount()
{
	return d_table_model->rowCount();
}

void Table::setBackgroundColor(const QColor& col)
{
	QPalette pal = d_table_view->palette();
	pal.setColor(QPalette::Base, col);
	d_table_view->setPalette(pal);
}

void Table::setTextColor(const QColor& col)
{
	QPalette pal = d_table_view->palette();
	pal.setColor(QPalette::Text, col);
	d_table_view->setPalette(pal);
}

void Table::setTextFont(const QFont& fnt)
{
	d_table_view->setFont(fnt);
}

void Table::setHeaderColor(const QColor& col)
{
	QPalette pal = d_table_view->horizontalHeader()->palette();
	pal.setColor(QPalette::ButtonText, col);
	d_table_view->horizontalHeader()->setPalette(pal);

	pal = d_table_view->verticalHeader()->palette();
	pal.setColor(QPalette::ButtonText, col);
	d_table_view->verticalHeader()->setPalette(pal);
}

void Table::setHeaderFont(const QFont& fnt)
{
	d_table_view->horizontalHeader()->setFont(fnt);
}

void Table::showComments(bool on)
{
	undoStack()->push(new TableShowCommentsCmd(d_table_model, on));
}

QStringList Table::columnLabels()
{
	QStringList list;
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		list << d_table_model->columnLabel(i);

	return list;
}

void Table::setRowCount(int new_size)
{
	int rows = rowCount();
	if (rows == new_size)
		return;

	if (new_size < rows)
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		undoStack()->push(new TableRemoveRowsCmd(d_table_model, new_size, rows-new_size));
		QApplication::restoreOverrideCursor();
	}
	else
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		undoStack()->push(new TableAppendRowsCmd(d_table_model, new_size-rows) );
		QApplication::restoreOverrideCursor();
	}
}

void Table::setColumnCount(int new_size)
{
	int old_size = columnCount();
	if (old_size == new_size)
		return;

	if (new_size < old_size)
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		undoStack()->push(new TableRemoveColumnsCmd(d_table_model, new_size, old_size-new_size));
		QApplication::restoreOverrideCursor();
	}
	else
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		// determine the highest number used as a column name
		int index, max=0;
		for (int i=0; i<old_size; i++)
		{
			if (!columnLabel(i).contains(QRegExp("\\D")))
			{
				index = columnLabel(i).toInt();
				if (index > max)
					max = index;
			}
		}
		max++;

		QList<AbstractColumnData *> cols;
		QList<AbstractFilter *> in_filters;
		QList<AbstractFilter *> out_filters;

		for(int i=0; i<new_size-old_size; i++)
		{
			cols << new DoubleColumnData();
			cols.at(i)->setLabel(QString::number(max++));
			in_filters << new String2DoubleFilter();
			out_filters << new Double2StringFilter();
		}
		undoStack()->push(new TableAppendColumnsCmd(d_table_model, cols, in_filters, out_filters) );
		QApplication::restoreOverrideCursor();
	}
}

void Table::copy(Table * other)
{
	// TODO
}

int Table::selectedRowCount(bool full)
{
	int count = 0;
	int rows = columnCount();
	for (int i=0; i<rows; i++)
		if(isRowSelected(i, full)) count++;
	return count;
}

bool Table::isRowSelected(int row, bool full)
{
	QItemSelectionModel * sel_model = d_table_view->selectionModel();
	if(full)
		return sel_model->isRowSelected(row, QModelIndex());
	else
		return sel_model->rowIntersectsSelection(row, QModelIndex());
}

void Table::setAscendingValues()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int rows = rowCount();

	QStringList data;

	int end = lastSelectedColumn();
	for(int i=firstSelectedColumn(); i<end; i++)
	{
		if(isColumnSelected(i))
		{
			data.clear();
			for(int j=0; j<rows; j++)
			{
				if(isCellSelected(j, i))
					data << QString::number(j+1);
			}
			undoStack()->push(new TableSetColumnValuesCmd(d_table_model, i, data) );
		}
	}

	QApplication::restoreOverrideCursor();
}

void Table::setRandomValues()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int rows = rowCount();

	qsrand(QTime::currentTime().msec());

	QStringList data;

	int end = lastSelectedColumn();
	for(int i=firstSelectedColumn(); i<end; i++)
	{
		if(isColumnSelected(i))
		{
			data.clear();
			for(int j=0; j<rows; j++)
			{
				if(isCellSelected(j, i))
					data << QString::number(double(qrand())/double(RAND_MAX));
			}
			undoStack()->push(new TableSetColumnValuesCmd(d_table_model, i, data) );
		}
	}

	QApplication::restoreOverrideCursor();
}

int Table::firstSelectedColumn(bool full)
{
	int cols = columnCount();
	for (int i=0; i<cols; i++)
	{
		if(isColumnSelected(i, full))
			return i;
	}
	return -1;
}

int Table::lastSelectedColumn(bool full)
{
	int cols = columnCount();
	for(int i=cols-1; i>=0; i--)
		if(isColumnSelected(i, full)) return i;

	return -1;
}


bool Table::isCellSelected(int row, int col)
{
	if(row < 0 || col < 0 || row >= rowCount() || col >= columnCount()) return false;

	return d_table_model->output(col)->isSelected(row);
	// Remark: This could also be done like this:
	// <code>
	// return d_table_view->selectionModel()->isSelected( d_table_model->index(row, col, QModelIndex()) );
	// </code>
	// But since the selection is synchronized between columns and selection
	// model there should be at most a slight speed difference, if any. - thzs
}

void Table::setPlotDesignation(AbstractDataSource::PlotDesignation pd, int col)
{
	if( col >= 0 && col <= columnCount() )
		undoStack()->push(new TableSetColumnPlotDesignationCmd(d_table_model, col, pd));
	else
		for(int i=0;i<columnCount(); i++)
			if(isColumnSelected(i, false))
				undoStack()->push(new TableSetColumnPlotDesignationCmd(d_table_model, i, pd));
}

void Table::clearColumn(int col)
{
	if( col >= 0 && col <= columnCount() )
		undoStack()->push(new TableClearColumnCmd(d_table_model, col));
}

void Table::clear()
{
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		clearColumn(i);
}

AbstractDataSource::PlotDesignation Table::plotDesignation(int col)
{
	return d_table_model->columnPlotDesignation(col);
}

void Table::goToCell(int row, int col)
{
	if( (row < 0) || (row >= rowCount()) ) return;
	if( (col < 0) || (col >= columnCount()) ) return;

	d_table_view->scrollTo(d_table_model->index(row, col, QModelIndex()), QAbstractItemView::PositionAtCenter);
}

int Table::colX(int col)
{
	for(int i=col-1; i>=0; i--)
	{
		if (d_table_model->columnPlotDesignation(i) == AbstractDataSource::X)
			return i;
	}
	int cols = columnCount();
	for(int i=col+1; i<cols; i++)
	{
		if (d_table_model->columnPlotDesignation(i) == AbstractDataSource::X)
			return i;
	}
	return -1;
}

int Table::colY(int col)
{
	int cols = columnCount();
	// look to the right first
	for(int i=col+1; i<cols; i++)
	{
		if (d_table_model->columnPlotDesignation(i) == AbstractDataSource::Y)
			return i;
	}
	for(int i=col-1; i>=0; i--)
	{
		if (d_table_model->columnPlotDesignation(i) == AbstractDataSource::Y)
			return i;
	}
	return -1;
}

Table::ColumnMode Table::columnMode(int col)
{
	// TODO
}

int Table::columnWidth(int col)
{
	return d_table_view->columnWidth(col);
}


// -------------------------------------------------------------------------
// transition functions
// -------------------------------------------------------------------------
int Table::colIndex(const QString& name)
{
	OBSOLETE
	int pos=name.find("_",false);
	QString label=name.right(name.length()-pos-1);
	for(int i=0; i<columnCount(); i++)
		if(d_table_model->columnLabel(i) == label)
			return i;
}

QString Table::text(int row, int col)
{
	OBSOLETE
	return d_table_model->data(d_table_model->index(row, col, QModelIndex()), Qt::EditRole).toString();
}

QString Table::colName(int col)
{ //returns the table name + underscore + column label
	OBSOLETE
	if (col<0 || col >= columnCount())
		return QString();

	return QString(this->name())+"_"+d_table_model->columnLabel(col);
}

int Table::columnType(int col)
{
	OBSOLETE
	AbstractDataSource * ptr = d_table_model->output(col);
	if(ptr->inherits("DoubleDataSource"))
		return Table::Numeric;
	if(ptr->inherits("StringDataSource"))
		return Table::Text;
	if(ptr->inherits("DateTimeDataSource"))
		return Table::Date;
}

QString Table::columnFormat(int col)
{
	OBSOLETE
	AbstractFilter * fltr = d_table_model->outputFilter(col);
	QObject * ptr = dynamic_cast<QObject *>(fltr);
	if(ptr && ptr->inherits("DateTime2StringFilter"))
		return static_cast<DateTime2StringFilter *>(ptr)->format();
	else
		return QString();
}

QStringList Table::selectedYLabels()
{
	OBSOLETE
	QStringList names;
	for (int i=0; i<columnCount(); i++)
	{
		if(isColumnSelected (i) && plotDesignation(i) == AbstractDataSource::Y)
			names << columnLabel(i);
	}
	return names;
}

double Table::cell(int row, int col)
{
	OBSOLETE
	return d_table_model->data(d_table_model->index(row, col, QModelIndex()), Qt::EditRole).toDouble();
}

int Table::selectedColumn()
{
	OBSOLETE
	return firstSelectedColumn();
}


void Table::setSelectedCol(int col)
{
	OBSOLETE
	Q_UNUSED(col)
}

QStringList Table::colNames()
{
	OBSOLETE
	QStringList names;
	for (int i=0; i<columnCount(); i++)
	{
		names << columnLabel(i);
	}
	return names;
}

void Table::setText(int row, int col, QString text)
{
	OBSOLETE
	QModelIndex index = d_table_model->index(row, col, QModelIndex());
	d_table_model->setData(index, text, Qt::EditRole);
	undoStack()->push(new TableUserInputCmd(d_table_model, index) );		
}

void Table::setHeader(QStringList header)
{
	OBSOLETE
	for(int i=0; i<header.size(); i++)
		undoStack()->push(new TableSetColumnLabelCmd(d_table_model, i, header.at(i)) );		
}

int Table::colPlotDesignation(int col)
{
	OBSOLETE
	return int(plotDesignation(col));
}

void Table::setColPlotDesignation(int col, AbstractDataSource::PlotDesignation pd)
{
	OBSOLETE
	setPlotDesignation(col, pd);
}

QString Table::colLabel(int col)
{
	OBSOLETE
	return columnLabel(col);
}

QString Table::colComment(int col)
{
	OBSOLETE
	return columnComment(col);
}

void Table::columnNumericFormat(int col, char *f, int *precision)
{
	OBSOLETE
	AbstractFilter * fltr = d_table_model->outputFilter(col);
	QObject * ptr = dynamic_cast<QObject *>(fltr);

	if( ptr && ptr->inherits("Double2StringFilter"))
	{
		Double2StringFilter * d2sf = static_cast<Double2StringFilter *>(ptr);
		*f = d2sf->numericFormat();
		*precision = d2sf->numDigits();
	}
	else
	{
		*f = 0;
		*precision = 0;
	}
}

void Table::columnNumericFormat(int col, int *f, int *precision)
{
	OBSOLETE
	char format;
	columnNumericFormat(col, &format, precision);

	if( !format )
		*f = 0;
	else
		switch(format)
		{
			case 'g':
				*f = 0;
				break;

			case 'f':
				*f = 1;
				break;

			case 'e':
				*f = 2;
				break;
		}
}

void Table::changeColWidth(int width, bool all)
{
	OBSOLETE
	int cols = columnCount();
	if (all)
	{
		for (int i=0;i<cols;i++)
			d_table_view->setColumnWidth(i, width);
	}
	else
	{
		d_table_view->setColumnWidth(firstSelectedColumn(), width);
	}
}

void Table::enumerateRightCols(bool checked)
{
	OBSOLETE
}


void Table::setColComment(int col, const QString& s)
{
	OBSOLETE
	setColumnComment(col, s);
}

void Table::changeColName(const QString& new_name)
{
	OBSOLETE
	setColumnLabel(firstSelectedColumn(), new_name);
}

void Table::setColName(int col,const QString& new_name)
{
	OBSOLETE
	setColumnLabel(col, new_name);
}

void Table::setCommand(int col, const QString& com)
{
	OBSOLETE
}

void Table::setColNumericFormat(int f, int prec, int col)
{
	OBSOLETE
}

void Table::setTextFormat(int col)
{
	OBSOLETE
}

void Table::setDateFormat(const QString& format, int col)
{
	OBSOLETE
}

void Table::setTimeFormat(const QString& format, int col)
{
	OBSOLETE
}

void Table::setMonthFormat(const QString& format, int col)
{
	OBSOLETE
}

void Table::setDayFormat(const QString& format, int col)
{
	OBSOLETE
}

bool Table::setDateTimeFormat(int col, int f, const QString& format)
{
	OBSOLETE
}

int Table::verticalHeaderWidth()
{
	OBSOLETE
	return d_table_view->verticalHeader()->width();
}

QStringList Table::columnsList()
{
	OBSOLETE
	QStringList names;
	for (int i=0;i<columnCount();i++)
		names << QString(name())+"_"+columnLabel(i);

	return names;
}

int Table::firstXCol()
{
	OBSOLETE
	for (int j=0; j<columnCount(); j++)
	{
		if (plotDesignation(j) == AbstractDataSource::X)
			return j;
	}
	return -1;
}

QStringList Table::selectedColumnsOld()
{
	OBSOLETE
	QStringList list;
	int cols = columnCount();
	for (int i=0; i<cols; i++)
		if(isColumnSelected(i)) list << columnLabel(i);

	return list;
}

void Table::addCol()
{
	OBSOLETE
	setColumnCount(columnCount() + 1);
}

