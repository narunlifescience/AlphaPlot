/***************************************************************************
    File                 : Table.cpp
    Project              : SciDAVis
    Description          : Aspect providing a spreadsheet table with column logic
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2009 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2006-2009 Knut Franke (knut.franke*gmx.de)
    Copyright            : (C) 2006-2007 Ion Vasilief (ion_vasilief*yahoo.fr)
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
#include "table/future_Table.h"
#include "core/Project.h"
#include "lib/ActionManager.h"

#include <QItemSelectionModel>
#include <QTime>
#include <QtGlobal>
#include <QHBoxLayout>
#include <QShortcut>
#include <QApplication>
#include <QContextMenuEvent>
#include <climits> // for RAND_MAX
#include <QMenu>
#include <QItemSelection>
#include <QModelIndex>
#include <QModelIndexList>
#include <QInputDialog>
#include <QMapIterator>
#include <QDialog>
#include <QMenuBar>
#include <QClipboard>
#include <QToolBar>
#include <QtDebug>

#include "table/TableModel.h"
#include "table/TableView.h"
#include "table/tablecommands.h"
#include "table/future_SortDialog.h"
#include "core/column/Column.h"
#include "core/AbstractFilter.h"
#include "core/datatypes/String2DoubleFilter.h"
#include "core/datatypes/Double2StringFilter.h"
#include "core/datatypes/DateTime2StringFilter.h"
#include "core/datatypes/String2DayOfWeekFilter.h"
#include "core/datatypes/String2MonthFilter.h"
#include "core/datatypes/Double2DateTimeFilter.h"
#include "core/datatypes/Double2MonthFilter.h"
#include "core/datatypes/Double2DayOfWeekFilter.h"
#include "core/datatypes/String2DateTimeFilter.h"
#include "core/datatypes/DateTime2DoubleFilter.h"
#include "core/datatypes/SimpleCopyThroughFilter.h"
#include "TeXTableExportDialog.h"
#include "TeXTableSettings.h" 


#include "ui_DimensionsDialog.h"

#define WAIT_CURSOR QApplication::setOverrideCursor(QCursor(Qt::WaitCursor))
#define RESET_CURSOR QApplication::restoreOverrideCursor()

namespace future
{

bool Table::d_default_comment_visibility = false;
int Table::default_column_width = 120;

// TODO: move all selection related stuff to the primary view ?

#ifndef LEGACY_CODE_0_2_x
Table::Table(AbstractScriptingEngine *engine, int rows, int columns, const QString& name)
	: AbstractPart(name), d_plot_menu(0), scripted(engine)
#else
Table::Table(void *engine, int rows, int columns, const QString& name)
	: AbstractPart(name), d_plot_menu(0)
#endif
{
	d_table_private = new Private(this);

	// set initial number of rows and columns
	QList<Column*> cols;
	for(int i=0; i<columns; i++)
	{
		Column * new_col = new Column(QString::number(i+1), SciDAVis::Numeric);
		new_col->setPlotDesignation(i == 0 ? SciDAVis::X : SciDAVis::Y);
		cols << new_col;
	}
	appendColumns(cols);
	setRowCount(rows);

	d_view = NULL; 
	createActions();
	connectActions();
}

#ifndef LEGACY_CODE_0_2_x
Table::Table()
	: AbstractPart("temp"), scripted(0)
#else
Table::Table()
	: AbstractPart("temp")
#endif
{
	d_view = NULL;
	d_table_private = NULL;
	createActions();
}

Table::~Table()
{
	delete d_table_private;
	delete d_view;
}

Column * Table::column(int index) const
{ 
	return d_table_private->column(index); 
}

Column * Table::column(const QString & name, bool legacy_kludge) const
{ 
	// TODO for 0.3.0: remove all name concatenation with _ in favor of Column * pointers
	int pos=name.find("_",false);
	QString label=name.right(name.length()-pos-1);
	for (int i=0; i<columnCount(); i++)
	{
		Column *col = d_table_private->column(i);
		if (col->name() == name || (legacy_kludge && col->name() == label))
			return col;
	}

	return NULL;
}

void Table::setView(TableView * view)
{
	d_view = view;
	addActionsToView();
	d_view->showComments(d_default_comment_visibility);
}

QWidget *Table::view()
{
#ifndef LEGACY_CODE_0_2_x
	if (!d_view)
	{
		d_view = new TableView(this); 
		addActionsToView();
		d_view->showComments(d_default_comment_visibility);
	}
#else
	Q_ASSERT(d_view != NULL);
#endif
	return d_view;
}

void Table::insertColumns(int before, QList<Column*> new_cols)
{
	if( new_cols.size() < 1 || before < 0 || before > columnCount()) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: insert %2 column(s)").arg(name()).arg(new_cols.size()));
	int pos=before;
	foreach(Column* col, new_cols)
		insertChild(col, pos++);
	// remark: the TableInsertColumnsCmd will be created in completeAspectInsertion()
	endMacro();
	RESET_CURSOR;
}

void Table::removeColumns(int first, int count)
{
	if( count < 1 || first < 0 || first+count > columnCount()) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: remove %2 column(s)").arg(name()).arg(count));
	QList<Column*> cols;
	for(int i=first; i<(first+count); i++)
		cols.append(d_table_private->column(i));
	// remark:  the TableRemoveColumnsCmd will be created in prepareAspectRemoval()
	foreach(Column* col, cols)
		removeChild(col);
	endMacro();
	RESET_CURSOR;
}

void Table::removeColumn(Column * col)
{
	removeColumns(columnIndex(col), 1);
}

void Table::removeRows(int first, int count)
{
	if( count < 1 || first < 0 || first+count > rowCount()) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: remove %2 row(s)").arg(name()).arg(count));
	int end = d_table_private->columnCount();
	for(int col=0; col<end; col++)
		d_table_private->column(col)->removeRows(first, count);
	exec(new TableSetNumberOfRowsCmd(d_table_private, d_table_private->rowCount()-count));
	endMacro();
	RESET_CURSOR;
}

void Table::insertRows(int before, int count)
{
	if( count < 1 || before < 0 || before > rowCount()) return;
	WAIT_CURSOR;
	int new_row_count = rowCount() + count;
	beginMacro(QObject::tr("%1: insert %2 row(s)").arg(name()).arg(count));
	int end = d_table_private->columnCount();
	for(int col=0; col<end; col++)
		d_table_private->column(col)->insertRows(before, count);
	setRowCount(new_row_count);
	endMacro();
	RESET_CURSOR;
}

void Table::setRowCount(int new_size)
{
	if( (new_size < 0) || (new_size == d_table_private->rowCount()) ) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: set the number of rows to %2").arg(name()).arg(new_size));
	if (new_size < d_table_private->rowCount())
	{
		int end = d_table_private->columnCount();
		for(int col=0; col<end; col++)
		{	
			Column *col_ptr = d_table_private->column(col);
			if (col_ptr->rowCount() > new_size)
				col_ptr->removeRows(new_size, col_ptr->rowCount() - new_size);
		}
	}
	exec(new TableSetNumberOfRowsCmd(d_table_private, new_size));
	endMacro();
	RESET_CURSOR;
}

int Table::columnCount() const
{
	return d_table_private->columnCount();
}

int Table::rowCount() const
{
	return d_table_private->rowCount();
}

int Table::columnCount(SciDAVis::PlotDesignation pd) const
{
	int count = 0;
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		if(column(i)->plotDesignation() == pd) count++;
	
	return count;
}

void Table::setColumnCount(int new_size)
{
	int old_size = columnCount();
	if ( old_size == new_size || new_size < 0 )
		return;

	WAIT_CURSOR;
	if (new_size < old_size)
		removeColumns(new_size, old_size-new_size);
	else
	{
		QList<Column*> cols;
		for(int i=0; i<new_size-old_size; i++)
		{
			Column * new_col = new Column(QString::number(i+1), SciDAVis::Numeric);
			new_col->setPlotDesignation(SciDAVis::Y);
			cols << new_col;
		}
		appendColumns(cols);
	}
	RESET_CURSOR;
}
		
int Table::columnIndex(const Column * col) const 
{ 
	return d_table_private->columnIndex(col); 
}

void Table::clear()
{
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: clear").arg(name()));
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		column(i)->clear();
	endMacro();
	RESET_CURSOR;
}

#ifndef LEGACY_CODE_0_2_x
void Table::clearMasks()
{
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: clear all masks").arg(name()));
	int cols = columnCount();
	for(int i=0; i<cols; i++)
		column(i)->clearMasks();
	endMacro();
	RESET_CURSOR;
}
#endif

void Table::addColumn()
{
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: add column").arg(name()));
	setColumnCount(columnCount()+1);
	endMacro();
	RESET_CURSOR;
}

void Table::addColumns()
{
	if (!d_view) return;
	WAIT_CURSOR;
	int count = d_view->selectedColumnCount(false);
	beginMacro(QObject::tr("%1: add %2 column(s)").arg(name()).arg(count));
	setColumnCount(columnCount() + count);
	endMacro();
	RESET_CURSOR;
}

void Table::cutSelection()
{
	if (!d_view) return;
	int first = d_view->firstSelectedRow();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(tr("%1: cut selected cell(s)").arg(name()));
	copySelection();
	clearSelectedCells();
	endMacro();
	RESET_CURSOR;
}

void Table::copySelection()
{
	if (!d_view) return;
	int first_col = d_view->firstSelectedColumn(false);
	if(first_col == -1) return;
	int last_col = d_view->lastSelectedColumn(false);
	if(last_col == -2) return;
	int first_row = d_view->firstSelectedRow(false);
	if(first_row == -1)	return;
	int last_row = d_view->lastSelectedRow(false);
	if(last_row == -2) return;
	int cols = last_col - first_col +1;
	int rows = last_row - first_row +1;
	
	WAIT_CURSOR;
	QString output_str;

	for(int r=0; r<rows; r++)
	{
		for(int c=0; c<cols; c++)
		{	
			Column *col_ptr = column(first_col + c);
			if(d_view->isCellSelected(first_row + r, first_col + c))
			{
				if (d_view->formulaModeActive())
				{
					output_str += col_ptr->formula(first_row + r);
				}
				else if (col_ptr->dataType() == SciDAVis::TypeDouble)
				{
					Double2StringFilter * out_fltr = static_cast<Double2StringFilter *>(col_ptr->outputFilter());
					output_str += QLocale().toString(col_ptr->valueAt(first_row + r), 
							out_fltr->numericFormat(), 16); // copy with max. precision
				}
				else
				{
					output_str += text(first_row + r, first_col + c);
				}
			}
			if(c < cols-1)
				output_str += "\t";
		}
		if(r < rows-1)
			output_str += "\n";
	}
	QApplication::clipboard()->setText(output_str);
	RESET_CURSOR;
}

void Table::pasteIntoSelection()
{
	if (!d_view) return;
	if(columnCount() < 1 || rowCount() < 1) return;

	WAIT_CURSOR;
	beginMacro(tr("%1: paste from clipboard").arg(name()));

	int first_col = d_view->firstSelectedColumn(false);
	int last_col = d_view->lastSelectedColumn(false);
	int first_row = d_view->firstSelectedRow(false);
	int last_row = d_view->lastSelectedRow(false);
	int input_row_count = 0;
	int input_col_count = 0;
	int rows, cols;

	const QClipboard *clipboard = QApplication::clipboard();
	const QMimeData *mimeData = clipboard->mimeData();;
	if(mimeData->hasText())
	{
		QString input_str = clipboard->text().trimmed();
		QList< QStringList > cell_texts;
		QStringList input_rows(input_str.split(QRegExp("\\n|\\r\\n|\\r")));
		input_row_count = input_rows.count();
		input_col_count = 0;
		for(int i=0; i<input_row_count; i++)
		{
			cell_texts.append(input_rows.at(i).trimmed().split(QRegExp("\\s+")));
			if(cell_texts.at(i).count() > input_col_count) input_col_count = cell_texts.at(i).count();
		}

		if( (first_col == -1 || first_row == -1) ||
			(last_row == first_row && last_col == first_col) )
		// if the is no selection or only one cell selected, the
		// selection will be expanded to the needed size from the current cell
		{
			int current_row, current_col;
			d_view->getCurrentCell(&current_row, &current_col);
			if(current_row == -1) current_row = 0;
			if(current_col == -1) current_col = 0;
			d_view->setCellSelected(current_row, current_col);
			first_col = current_col;
			first_row = current_row;
			last_row = first_row + input_row_count -1;
			last_col = first_col + input_col_count -1;
			// resize the table if necessary
			if(last_col >= columnCount())
			{
				QList<Column*> cols;
				for(int i=0; i<last_col+1-columnCount(); i++)
				{
					Column * new_col = new Column(QString::number(i+1), SciDAVis::Text);
					new_col->setPlotDesignation(SciDAVis::Y);
					cols << new_col;
				}
				appendColumns(cols);
			}
			if(last_row >= rowCount())
				appendRows(last_row+1-rowCount());
			// select the rectangle to be pasted in
			d_view->setCellsSelected(first_row, first_col, last_row, last_col);
		}

		rows = last_row - first_row + 1;
		cols = last_col - first_col + 1;
		for(int r=0; r<rows && r<input_row_count; r++)
		{
			for(int c=0; c<cols && c<input_col_count; c++)
			{
				if(d_view->isCellSelected(first_row + r, first_col + c) && (c < cell_texts.at(r).count()) )
				{
					Column * col_ptr = d_table_private->column(first_col + c);
					if (d_view->formulaModeActive())
					{
						col_ptr->setFormula(first_row + r, cell_texts.at(r).at(c));  
						col_ptr->setInvalid(first_row + r, false);
					}
					else
						col_ptr->asStringColumn()->setTextAt(first_row+r, cell_texts.at(r).at(c));
				}
			}
		}
		recalculateSelectedCells();
	}
	endMacro();
	RESET_CURSOR;
}

#ifndef LEGACY_CODE_0_2_x
void Table::maskSelection()
{
	if (!d_view) return;
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(tr("%1: mask selected cell(s)").arg(name()));
	QList<Column*> list = d_view->selectedColumns();
	foreach(Column * col_ptr, list)
	{
		int col = columnIndex(col_ptr);
		for(int row=first; row<=last; row++)
			if(d_view->isCellSelected(row, col)) col_ptr->setMasked(row);  
	}
	endMacro();
	RESET_CURSOR;
}

void Table::unmaskSelection()
{
	if (!d_view) return;
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(tr("%1: unmask selected cell(s)").arg(name()));
	QList<Column*> list = d_view->selectedColumns();
	foreach(Column * col_ptr, list)
	{
		int col = columnIndex(col_ptr);
		for(int row=first; row<=last; row++)
			if(d_view->isCellSelected(row, col)) col_ptr->setMasked(row, false);  
	}
	endMacro();
	RESET_CURSOR;
}
#endif

void Table::setFormulaForSelection()
{
	if (!d_view) return;
	d_view->showControlFormulaTab();
}

void Table::recalculateSelectedCells()
{
	if (!d_view) return;
#ifdef LEGACY_CODE_0_2_x
	WAIT_CURSOR;
	beginMacro(tr("%1: apply formula to selection").arg(name()));
	emit recalculate();
	endMacro();
	RESET_CURSOR;
#endif
}

void Table::fillSelectedCellsWithRowNumbers()
{
	if (!d_view) return;
	if(d_view->selectedColumnCount() < 1) return;
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;
	
	WAIT_CURSOR;
	beginMacro(tr("%1: fill cells with row numbers").arg(name()));
	foreach(Column *col_ptr, d_view->selectedColumns()) {
		int col = columnIndex(col_ptr);
		switch (col_ptr->columnMode()) {
			case SciDAVis::Numeric:
				{
					QVector<qreal> results(last-first+1);
					for (int row=first; row<=last; row++)
						if(d_view->isCellSelected(row, col)) 
							results[row-first] = row+1;
						else
							results[row-first] = col_ptr->valueAt(row);
					col_ptr->replaceValues(first, results);
					break;
				}
			case SciDAVis::Text:
				{
					QStringList results;
					for (int row=first; row<=last; row++)
						if (d_view->isCellSelected(row, col))
							results << QString::number(row+1);
						else
							results << col_ptr->textAt(row);
					col_ptr->replaceTexts(first, results);
					break;
				}
			default:
				break;
		}
	}
	endMacro();
	RESET_CURSOR;
}

void Table::fillSelectedCellsWithRandomNumbers()
{
	if (!d_view) return;
	if(d_view->selectedColumnCount() < 1) return;
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;
	
	WAIT_CURSOR;
	beginMacro(tr("%1: fill cells with random values").arg(name()));
	qsrand(QTime::currentTime().msec());
	foreach(Column *col_ptr, d_view->selectedColumns()) {
		int col = columnIndex(col_ptr);
		switch (col_ptr->columnMode()) {
			case SciDAVis::Numeric:
				{
					QVector<qreal> results(last-first+1);
					for (int row=first; row<=last; row++)
						if (d_view->isCellSelected(row, col))
							results[row-first] = double(qrand())/double(RAND_MAX);
						else
							results[row-first] = col_ptr->valueAt(row);
					col_ptr->replaceValues(first, results);
					break;
				}
			case SciDAVis::Text:
				{
					QStringList results;
					for (int row=first; row<=last; row++)
						if (d_view->isCellSelected(row, col))
							results << QString::number(double(qrand())/double(RAND_MAX));
						else
							results << col_ptr->textAt(row);
					col_ptr->replaceTexts(first, results);
					break;
				}
			case SciDAVis::DateTime:
			case SciDAVis::Month:
			case SciDAVis::Day:
				{
					QList<QDateTime> results;
					QDate earliestDate(1,1,1);
					QDate latestDate(2999,12,31);
					QTime midnight(0,0,0,0);
					for (int row=first; row<=last; row++)
						if (d_view->isCellSelected(row, col))
							results << QDateTime(
									earliestDate.addDays(((double)qrand())*((double)earliestDate.daysTo(latestDate))/((double)RAND_MAX)),
									midnight.addMSecs(((qint64)qrand())*1000*60*60*24/RAND_MAX));
						else
							results << col_ptr->dateTimeAt(row);
					col_ptr->replaceDateTimes(first, results);
					break;
				}
		}
	}
	endMacro();
	RESET_CURSOR;
}

void Table::sortTable()
{
	QList<Column*> cols;
	
	for(int i=0; i<columnCount(); i++)
		cols.append(column(i));

	sortDialog(cols);
}

void Table::insertEmptyColumns()
{
	if (!d_view) return;
	int first = d_view->firstSelectedColumn();
	int last = d_view->lastSelectedColumn();
	if( first < 0 ) return;
	int count, current = first;
	QList<Column*> cols;

	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: insert empty column(s)").arg(name()));
	while( current <= last )
	{
		current = first+1;
		while( current <= last && d_view->isColumnSelected(current) ) current++;
		count = current-first;
		for(int i=0; i<count; i++)
		{
			Column * new_col = new Column(QString::number(i+1), SciDAVis::Numeric);
			new_col->setPlotDesignation(SciDAVis::Y);
			cols << new_col;
		}
		insertColumns(first, cols);
		cols.clear();
		current += count;
		last += count;
		while( current <= last && !d_view->isColumnSelected(current) ) current++;
		first = current;
	}
	endMacro();
	RESET_CURSOR;
}

void Table::removeSelectedColumns()
{
	if (!d_view) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: remove selected column(s)").arg(name()));

	QList< Column* > list = d_view->selectedColumns();
	foreach(Column* ptr, list)
		removeColumn(ptr);

	endMacro();
	RESET_CURSOR;
}

void Table::clearSelectedColumns()
{
	if (!d_view) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: clear selected column(s)").arg(name()));

	QList< Column* > list = d_view->selectedColumns();
	if (d_view->formulaModeActive())
	{
		foreach(Column* ptr, list)
			ptr->clearFormulas();
	}
	else
	{
		foreach(Column* ptr, list)
			ptr->clear();
	}

	endMacro();
	RESET_CURSOR;
}

void Table::setSelectionAs(SciDAVis::PlotDesignation pd)
{
	if (!d_view) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: set plot designation(s)").arg(name()));

	QList< Column* > list = d_view->selectedColumns();
	foreach(Column* ptr, list)
		ptr->setPlotDesignation(pd);

	endMacro();
	RESET_CURSOR;
}

void Table::setSelectedColumnsAsX()
{
	setSelectionAs(SciDAVis::X);
}

void Table::setSelectedColumnsAsY()
{
	setSelectionAs(SciDAVis::Y);
}

void Table::setSelectedColumnsAsZ()
{
	setSelectionAs(SciDAVis::Z);
}

void Table::setSelectedColumnsAsYError()
{
	setSelectionAs(SciDAVis::yErr);
}

void Table::setSelectedColumnsAsXError()
{
	setSelectionAs(SciDAVis::xErr);
}

void Table::setSelectedColumnsAsNone()
{
	setSelectionAs(SciDAVis::noDesignation);
}

void Table::normalizeColumns(QList< Column* > cols)
{
	if (!d_view) return;

	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: normalize column(s)").arg(name()));
	foreach(Column * col, cols)
	{
		if (col->dataType() == SciDAVis::TypeDouble)
		{
			double max = 0.0;
			for (int row=0; row<col->rowCount(); row++)
			{
				if (col->valueAt(row) > max)
					max = col->valueAt(row);
			}
			QVector<qreal> results(col->rowCount());
			if (max != 0.0) // avoid division by zero
				for (int row=0; row<col->rowCount(); row++)
					results[row] = col->valueAt(row) / max;
			col->replaceValues(0, results);
		}
	}
	endMacro();
	RESET_CURSOR;
}

void Table::normalizeSelectedColumns()
{
	normalizeColumns(d_view->selectedColumns());
}

void Table::normalizeSelection()
{
	if (!d_view) return;

	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: normalize selection").arg(name()));
	double max = 0.0;
	foreach(Column *col_ptr, d_view->selectedColumns()) {
		int col = columnIndex(col_ptr);
		if (col_ptr->dataType() == SciDAVis::TypeDouble)
			for (int row=0; row<col_ptr->rowCount(); row++)
				if (d_view->isCellSelected(row, col) && col_ptr->valueAt(row) > max)
					max = col_ptr->valueAt(row);
	}

	if (max != 0.0) // avoid division by zero
	{
		foreach(Column *col_ptr, d_view->selectedColumns()) {
			int col = columnIndex(col_ptr);
			if (col_ptr->dataType() == SciDAVis::TypeDouble) {
				QVector<qreal> results(rowCount());
				for (int row=0; row<col_ptr->rowCount(); row++)
					if (d_view->isCellSelected(row, col))
						results[row] = col_ptr->valueAt(row) / max;
					else
						results[row] = col_ptr->valueAt(row);
				col_ptr->replaceValues(0, results);
			}
		}
	}
	endMacro();
	RESET_CURSOR;
}

void Table::sortSelectedColumns()
{
	if (!d_view) return;
	QList< Column* > cols = d_view->selectedColumns();
	sortDialog(cols);
}

void Table::statisticsOnSelectedColumns()
{
#ifdef LEGACY_CODE_0_2_x
	// TODO: this is only an ugly hack for 0.2.0
	emit requestColumnStatistics();
#endif
}

void Table::statisticsOnSelectedRows()
{
#ifdef LEGACY_CODE_0_2_x
	// TODO: this is only an ugly hack for 0.2.0
	emit requestRowStatistics();
#endif
}

void Table::insertEmptyRows()
{
	if (!d_view) return;
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	int count, current = first;

	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: insert empty rows(s)").arg(name()));
	while( current <= last )
	{
		current = first+1;
		while( current <= last && d_view->isRowSelected(current) ) current++;
		count = current-first;
		insertRows(first, count);
		current += count;
		last += count;
		while( current <= last && !d_view->isRowSelected(current) ) current++;
		first = current;
	}
	endMacro();
	RESET_CURSOR;
}

void Table::removeSelectedRows()
{
	if (!d_view) return;

	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: remove selected rows(s)").arg(name()));
	foreach(Interval<int> i, d_view->selectedRows().intervals())
		removeRows(i.start(), i.size());
	endMacro();
	RESET_CURSOR;
}

void Table::clearSelectedCells()
{
	if (!d_view) return;
	int first = d_view->firstSelectedRow();
	// int last = d_view->lastSelectedRow();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: clear selected cell(s)").arg(name()));
	QList<Column*> list = d_view->selectedColumns();
	foreach(Column * col_ptr, list)
	{
		if (d_view->formulaModeActive())
			foreach(Interval<int> i, d_view->selectedRows().intervals())
				col_ptr->setFormula(i, "");
		else
			foreach(Interval<int> i, d_view->selectedRows().intervals())
				if (i.end() == col_ptr->rowCount()-1)
					col_ptr->removeRows(i.start(), i.size());
				else {
					QStringList empties;
					for (int j=0; j<i.size(); j++)
						empties << QString();
					col_ptr->asStringColumn()->replaceTexts(i.start(), empties);
				}
	}
	endMacro();
	RESET_CURSOR;
}

void Table::editTypeAndFormatOfSelectedColumns()
{
	if (!d_view) return;
	d_view->showControlTypeTab();
}

void Table::editDescriptionOfCurrentColumn()
{
	if (!d_view) return;
	d_view->showControlDescriptionTab();
}

void Table::addRows()
{
	if (!d_view) return;
	WAIT_CURSOR;
	int count = d_view->selectedRowCount(false);
	beginMacro(QObject::tr("%1: add %2 rows(s)").arg(name()).arg(count));
	exec(new TableSetNumberOfRowsCmd(d_table_private, rowCount() + count));
	endMacro();
	RESET_CURSOR;
}

bool Table::fillProjectMenu(QMenu * menu)
{
	menu->setTitle(tr("&Table"));

	QMenu * submenu = new QMenu(tr("S&et Column(s) As"));
	submenu->addAction(action_set_as_x);
	submenu->addAction(action_set_as_y);
	submenu->addAction(action_set_as_z);
	submenu->addSeparator();
	submenu->addAction(action_set_as_xerr);
	submenu->addAction(action_set_as_yerr);
	submenu->addSeparator();
	submenu->addAction(action_set_as_none);
	menu->addMenu(submenu);
	menu->addSeparator();

	submenu = new QMenu(tr("Fi&ll Selection with"));
	submenu->addAction(action_fill_row_numbers);
	submenu->addAction(action_fill_random);
	menu->addMenu(submenu);
	menu->addSeparator();

	connect(menu, SIGNAL(aboutToShow()), this, SLOT(adjustActionNames()));
	menu->addAction(action_toggle_comments);
	menu->addAction(action_toggle_tabbar);
	menu->addAction(action_formula_mode);
	menu->addAction(action_edit_description);
	menu->addAction(action_type_format);
	menu->addSeparator();
	menu->addAction(action_clear_table);
#ifndef LEGACY_CODE_0_2_x
	menu->addAction(action_clear_masks);
#endif
	menu->addAction(action_sort_table);
	menu->addSeparator();
	menu->addAction(action_set_formula);
	menu->addAction(action_recalculate);
	menu->addSeparator();
	menu->addAction(action_add_column);
	menu->addAction(action_dimensions_dialog);
	menu->addSeparator();
	menu->addAction(action_go_to_cell);

	return true;

	// TODO:
	// Convert to Matrix
	// Export 
}
		
bool Table::fillProjectToolBar(QToolBar * bar)
{
	bar->addAction(action_dimensions_dialog);
	bar->addAction(action_add_column);
	bar->addAction(action_statistics_columns);
	bar->addAction(action_statistics_rows);

	return true;
}

QMenu *Table::createContextMenu() const
{
	QMenu *menu = AbstractPart::createContextMenu();
	Q_ASSERT(menu);
	menu->addSeparator();
	
	// TODO
	// Export to ASCII
	// Print --> maybe should go to AbstractPart::createContextMenu()
	// ----
	// Rename --> AbstractAspect::createContextMenu(); maybe call this "Properties" and include changing comment/caption spec
	// Duplicate --> AbstractPart::createContextMenu()
	// Hide/Show --> Do we need hiding of views (in addition to minimizing)? How do we avoid confusion with hiding of Aspects?
	// Activate ?
	// Resize --> AbstractPart::createContextMenu()
	
	return menu;
}
		

void Table::createActions()
{
	QIcon * icon_temp;

	// selection related actions
	action_cut_selection = new QAction(QIcon(QPixmap(":/cut.xpm")), tr("Cu&t"), this);
	actionManager()->addAction(action_cut_selection, "cut_selection");

	action_copy_selection = new QAction(QIcon(QPixmap(":/copy.xpm")), tr("&Copy"), this);
	actionManager()->addAction(action_copy_selection, "copy_selection");

	action_paste_into_selection = new QAction(QIcon(QPixmap(":/paste.xpm")), tr("Past&e"), this);
	actionManager()->addAction(action_paste_into_selection, "paste_into_selection"); 

#ifndef LEGACY_CODE_0_2_x
	action_mask_selection = new QAction(QIcon(QPixmap(":/mask.xpm")), tr("&Mask","mask selection"), this);
	actionManager()->addAction(action_mask_selection, "mask_selection"); 

	action_unmask_selection = new QAction(QIcon(QPixmap(":/unmask.xpm")), tr("&Unmask","unmask selection"), this);
	actionManager()->addAction(action_unmask_selection, "unmask_selection"); 
#endif

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/fx.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/fx.png"));
	action_set_formula = new QAction(*icon_temp, tr("Assign &Formula"), this);
	action_set_formula->setShortcut(tr("Alt+Q"));
	actionManager()->addAction(action_set_formula, "set_formula"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/clear.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/clear.png"));
	action_clear_selection = new QAction(*icon_temp, tr("Clea&r","clear selection"), this);
	actionManager()->addAction(action_clear_selection, "clear_selection"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/recalculate.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/recalculate.png"));
	action_recalculate = new QAction(*icon_temp, tr("Recalculate"), this);
	action_recalculate->setShortcut(tr("Ctrl+Return"));
	actionManager()->addAction(action_recalculate, "recalculate"); 
	delete icon_temp;

	action_fill_row_numbers = new QAction(QIcon(QPixmap(":/rowNumbers.xpm")), tr("Row Numbers"), this);
	actionManager()->addAction(action_fill_row_numbers, "fill_row_numbers"); 

	action_fill_random = new QAction(QIcon(QPixmap(":/randomNumbers.xpm")), tr("Random Values"), this);
	actionManager()->addAction(action_fill_random, "fill_random"); 
	
	//table related actions
	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/table_header.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/table_header.png"));
	action_toggle_comments = new QAction(*icon_temp, QString("Show/Hide comments"), this); // show/hide column comments
	actionManager()->addAction(action_toggle_comments, "toggle_comments"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/table_options.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/table_options.png"));
	action_toggle_tabbar = new QAction(*icon_temp, QString("Show/Hide Controls"), this); // show/hide control tabs
	action_toggle_tabbar->setShortcut(tr("F12"));
	actionManager()->addAction(action_toggle_tabbar, "toggle_tabbar"); 
	delete icon_temp;

	action_formula_mode = new QAction(tr("Formula Edit Mode"), this);
	action_formula_mode->setCheckable(true);
	actionManager()->addAction(action_formula_mode, "formula_mode"); 

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/select_all.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/select_all.png"));
	action_select_all = new QAction(*icon_temp, tr("Select All"), this);
	actionManager()->addAction(action_select_all, "select_all"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/add_column.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/add_column.png"));
	action_add_column = new QAction(*icon_temp, tr("&Add Column"), this);
	action_add_column->setToolTip(tr("append a new column to the table"));
	actionManager()->addAction(action_add_column, "add_column"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/clear_table.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/clear_table.png"));
	action_clear_table = new QAction(*icon_temp, tr("Clear Table"), this);
	actionManager()->addAction(action_clear_table, "clear_table"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/TeX.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/TeX.png"));
	action_export_to_TeX = new QAction(*icon_temp, tr("Export to TeX..."), this);
	actionManager()->addAction(action_export_to_TeX, "export_to_TeX"); 
	delete icon_temp;
        

#ifndef LEGACY_CODE_0_2_x
	action_clear_masks = new QAction(QIcon(QPixmap(":/unmask.xpm")), tr("Clear Masks"), this);
	actionManager()->addAction(action_clear_masks, "clear_masks"); 
#endif

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/sort.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/sort.png"));
	action_sort_table = new QAction(*icon_temp, tr("&Sort Table"), this);
	actionManager()->addAction(action_sort_table, "sort_table"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/go_to_cell.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/go_to_cell.png"));
	action_go_to_cell = new QAction(*icon_temp, tr("&Go to Cell"), this);
	action_go_to_cell->setShortcut(tr("Ctrl+Alt+G"));
	actionManager()->addAction(action_go_to_cell, "go_to_cell"); 
	delete icon_temp;

	action_dimensions_dialog = new QAction(QIcon(QPixmap(":/resize.xpm")), tr("&Dimensions", "table size"), this);
	action_dimensions_dialog->setToolTip(tr("change the table size"));
	actionManager()->addAction(action_dimensions_dialog, "dimensions_dialog"); 

	// column related actions
	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/insert_column.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/insert_column.png"));
	action_insert_columns = new QAction(*icon_temp, tr("&Insert Empty Columns"), this);
	actionManager()->addAction(action_insert_columns, "insert_columns"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/remove_column.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/remove_column.png"));
	action_remove_columns = new QAction(*icon_temp, tr("Remo&ve Columns"), this);
	actionManager()->addAction(action_remove_columns, "remove_columns"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/clear_column.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/clear_column.png"));
	action_clear_columns = new QAction(*icon_temp, tr("Clea&r Columns"), this);
	actionManager()->addAction(action_clear_columns, "clear_columns"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/add_columns.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/add_columns.png"));
	action_add_columns = new QAction(*icon_temp, tr("&Add Columns"), this);
	actionManager()->addAction(action_add_columns, "add_columns"); 
	delete icon_temp;

	action_set_as_x = new QAction(QIcon(QPixmap()), tr("X","plot designation"), this);
	actionManager()->addAction(action_set_as_x, "set_as_x"); 

	action_set_as_y = new QAction(QIcon(QPixmap()), tr("Y","plot designation"), this);
	actionManager()->addAction(action_set_as_y, "set_as_y"); 

	action_set_as_z = new QAction(QIcon(QPixmap()), tr("Z","plot designation"), this);
	actionManager()->addAction(action_set_as_z, "set_as_z"); 

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/x_error.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/x_error.png"));
	action_set_as_xerr = new QAction(*icon_temp, tr("X Error","plot designation"), this);
	actionManager()->addAction(action_set_as_xerr, "set_as_xerr"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/y_error.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/y_error.png"));
	action_set_as_yerr = new QAction(*icon_temp, tr("Y Error","plot designation"), this);
	actionManager()->addAction(action_set_as_yerr, "set_as_yerr"); 
	delete icon_temp;

	action_set_as_none = new QAction(QIcon(QPixmap()), tr("None","plot designation"), this);
	actionManager()->addAction(action_set_as_none, "set_as_none"); 

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/normalize.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/normalize.png"));
	action_normalize_columns = new QAction(*icon_temp, tr("&Normalize Columns"), this);
	actionManager()->addAction(action_normalize_columns, "normalize_columns"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/normalize.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/normalize.png"));
	action_normalize_selection = new QAction(*icon_temp, tr("&Normalize Selection"), this);
	actionManager()->addAction(action_normalize_selection, "normalize_selection"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/sort.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/sort.png"));
	action_sort_columns = new QAction(*icon_temp, tr("&Sort Columns"), this);
	actionManager()->addAction(action_sort_columns, "sort_columns"); 
	delete icon_temp;

	action_statistics_columns = new QAction(QIcon(QPixmap(":/col_stat.xpm")), tr("Column Statisti&cs"), this);
	action_statistics_columns->setToolTip(tr("statistics on columns"));
	actionManager()->addAction(action_statistics_columns, "statistics_columns"); 

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/column_format_type.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/column_format_type.png"));
	action_type_format = new QAction(*icon_temp, tr("Change &Type && Format"), this);
	action_type_format->setShortcut(tr("Ctrl+Alt+O"));
	actionManager()->addAction(action_type_format, "type_format"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/column_description.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/column_description.png"));
	action_edit_description = new QAction(*icon_temp, tr("Edit Column &Description"), this);
	actionManager()->addAction(action_edit_description, "edit_description"); 
	delete icon_temp;

	// row related actions
	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/insert_row.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/insert_row.png"));
	action_insert_rows = new QAction(*icon_temp ,tr("&Insert Empty Rows"), this);
	actionManager()->addAction(action_insert_rows, "insert_rows"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/remove_row.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/remove_row.png"));
	action_remove_rows = new QAction(*icon_temp, tr("Remo&ve Rows"), this);
	actionManager()->addAction(action_remove_rows, "remove_rows"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/clear_row.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/clear_row.png"));
	action_clear_rows = new QAction(*icon_temp, tr("Clea&r Rows"), this);
	actionManager()->addAction(action_clear_rows, "clear_rows"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/add_rows.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/add_rows.png"));
	action_add_rows = new QAction(*icon_temp, tr("&Add Rows"), this);
	actionManager()->addAction(action_add_rows, "add_rows"); 
	delete icon_temp;

	action_statistics_rows = new QAction(QIcon(QPixmap(":/stat_rows.xpm")), tr("Row Statisti&cs"), this);
	action_statistics_rows->setToolTip(tr("statistics on rows"));
	actionManager()->addAction(action_statistics_rows, "statistics_rows"); 
}

void Table::connectActions()
{
	connect(action_cut_selection, SIGNAL(triggered()), this, SLOT(cutSelection()));
	connect(action_copy_selection, SIGNAL(triggered()), this, SLOT(copySelection()));
	connect(action_paste_into_selection, SIGNAL(triggered()), this, SLOT(pasteIntoSelection()));
#ifndef LEGACY_CODE_0_2_x
	connect(action_mask_selection, SIGNAL(triggered()), this, SLOT(maskSelection()));
	connect(action_unmask_selection, SIGNAL(triggered()), this, SLOT(unmaskSelection()));
#endif
	connect(action_set_formula, SIGNAL(triggered()), this, SLOT(setFormulaForSelection()));
	connect(action_clear_selection, SIGNAL(triggered()), this, SLOT(clearSelectedCells()));
	connect(action_recalculate, SIGNAL(triggered()), this, SLOT(recalculateSelectedCells()));
	connect(action_fill_row_numbers, SIGNAL(triggered()), this, SLOT(fillSelectedCellsWithRowNumbers()));
	connect(action_fill_random, SIGNAL(triggered()), this, SLOT(fillSelectedCellsWithRandomNumbers()));
	connect(action_select_all, SIGNAL(triggered()), this, SLOT(selectAll()));
	connect(action_add_column, SIGNAL(triggered()), this, SLOT(addColumn()));
	connect(action_clear_table, SIGNAL(triggered()), this, SLOT(clear()));

        //Export to TeX
	connect(action_export_to_TeX, SIGNAL( triggered() ), this, SLOT( showTeXTableExportDialog() ) );

#ifndef LEGACY_CODE_0_2_x
	connect(action_clear_masks, SIGNAL(triggered()), this, SLOT(clearMasks()));
#endif
	connect(action_sort_table, SIGNAL(triggered()), this, SLOT(sortTable()));
	connect(action_go_to_cell, SIGNAL(triggered()), this, SLOT(goToCell()));
	connect(action_dimensions_dialog, SIGNAL(triggered()), this, SLOT(dimensionsDialog()));
	connect(action_insert_columns, SIGNAL(triggered()), this, SLOT(insertEmptyColumns()));
	connect(action_remove_columns, SIGNAL(triggered()), this, SLOT(removeSelectedColumns()));
	connect(action_clear_columns, SIGNAL(triggered()), this, SLOT(clearSelectedColumns()));
	connect(action_add_columns, SIGNAL(triggered()), this, SLOT(addColumns()));
	connect(action_set_as_x, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsX()));
	connect(action_set_as_y, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsY()));
	connect(action_set_as_z, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsZ()));
	connect(action_set_as_xerr, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsXError()));
	connect(action_set_as_yerr, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsYError()));
	connect(action_set_as_none, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsNone()));
	connect(action_normalize_columns, SIGNAL(triggered()), this, SLOT(normalizeSelectedColumns()));
	connect(action_normalize_selection, SIGNAL(triggered()), this, SLOT(normalizeSelection()));
	connect(action_sort_columns, SIGNAL(triggered()), this, SLOT(sortSelectedColumns()));
	connect(action_statistics_columns, SIGNAL(triggered()), this, SLOT(statisticsOnSelectedColumns()));
	connect(action_type_format, SIGNAL(triggered()), this, SLOT(editTypeAndFormatOfSelectedColumns()));
	connect(action_edit_description, SIGNAL(triggered()), this, SLOT(editDescriptionOfCurrentColumn()));
	connect(action_insert_rows, SIGNAL(triggered()), this, SLOT(insertEmptyRows()));
	connect(action_remove_rows, SIGNAL(triggered()), this, SLOT(removeSelectedRows()));
	connect(action_clear_rows, SIGNAL(triggered()), this, SLOT(clearSelectedCells()));
	connect(action_add_rows, SIGNAL(triggered()), this, SLOT(addRows()));
	connect(action_statistics_rows, SIGNAL(triggered()), this, SLOT(statisticsOnSelectedRows()));
}

void Table::addActionsToView()
{
	connect(action_toggle_comments, SIGNAL(triggered()), d_view, SLOT(toggleComments()));
	connect(action_toggle_tabbar, SIGNAL(triggered()), d_view, SLOT(toggleControlTabBar()));
	connect(action_formula_mode, SIGNAL(toggled(bool)), d_view, SLOT(activateFormulaMode(bool)));

	d_view->addAction(action_cut_selection);
	d_view->addAction(action_copy_selection);
	d_view->addAction(action_paste_into_selection);
#ifndef LEGACY_CODE_0_2_x
	d_view->addAction(action_mask_selection);
	d_view->addAction(action_unmask_selection);
#endif
	d_view->addAction(action_set_formula);
	d_view->addAction(action_clear_selection);
	d_view->addAction(action_recalculate);
	d_view->addAction(action_fill_row_numbers);
	d_view->addAction(action_fill_random);
	d_view->addAction(action_toggle_comments);
	d_view->addAction(action_toggle_tabbar);
	d_view->addAction(action_formula_mode);
	d_view->addAction(action_select_all);
	d_view->addAction(action_add_column);
	d_view->addAction(action_clear_table);
	d_view->addAction(action_export_to_TeX);
#ifndef LEGACY_CODE_0_2_x
	d_view->addAction(action_clear_masks);
#endif
	d_view->addAction(action_sort_table);
	d_view->addAction(action_go_to_cell);
	d_view->addAction(action_dimensions_dialog);
	d_view->addAction(action_insert_columns);
	d_view->addAction(action_remove_columns);
	d_view->addAction(action_clear_columns);
	d_view->addAction(action_add_columns);
	d_view->addAction(action_set_as_x);
	d_view->addAction(action_set_as_y);
	d_view->addAction(action_set_as_z);
	d_view->addAction(action_set_as_xerr);
	d_view->addAction(action_set_as_yerr);
	d_view->addAction(action_set_as_none);
	d_view->addAction(action_normalize_columns);
	d_view->addAction(action_normalize_selection);
	d_view->addAction(action_sort_columns);
	d_view->addAction(action_statistics_columns);
	d_view->addAction(action_type_format);
	d_view->addAction(action_edit_description);
	d_view->addAction(action_insert_rows);
	d_view->addAction(action_remove_rows);
	d_view->addAction(action_clear_rows);
	d_view->addAction(action_add_rows);
	d_view->addAction(action_statistics_rows);
}

void Table::showTableViewContextMenu(const QPoint& pos)
{
	if (!d_view) return;
	QMenu context_menu;
	
// TODO: Does undo/redo really be belong into a context menu?
//	context_menu.addAction(undoAction(&context_menu));
//	context_menu.addAction(redoAction(&context_menu));

	if(d_plot_menu)
	{
		context_menu.addMenu(d_plot_menu);
		context_menu.addSeparator();
	}

	createSelectionMenu(&context_menu);
	context_menu.addSeparator();
	createTableMenu(&context_menu);
	context_menu.addSeparator();

	context_menu.exec(pos);
}

void Table::showTableViewColumnContextMenu(const QPoint& pos)
{
	if (!d_view) return;
	QMenu context_menu;
	
// TODO: Does undo/redo really be belong into a context menu?
//	context_menu.addAction(undoAction(&context_menu));
//	context_menu.addAction(redoAction(&context_menu));

	if(d_plot_menu)
	{
		context_menu.addMenu(d_plot_menu);
		context_menu.addSeparator();
	}

	createColumnMenu(&context_menu);
	context_menu.addSeparator();

	context_menu.exec(pos);
}

void Table::showTableViewRowContextMenu(const QPoint& pos)
{
	if (!d_view) return;
	QMenu context_menu;
	
// TODO: Does undo/redo really be belong into a context menu?
//	context_menu.addAction(undoAction(&context_menu));
//	context_menu.addAction(redoAction(&context_menu));

	createRowMenu(&context_menu);

	context_menu.exec(pos);
}

void Table::showTeXTableExportDialog()
{
        TeXTableExportDialog export_Dialog;

        export_Dialog.setFileMode(QFileDialog::AnyFile);
        export_Dialog.setNameFilter("*.tex");

        //Set the default file name by the name of the table
        export_Dialog.selectFile( name() );
        
        export_Dialog.exec();

        if (export_Dialog.result() == QDialog::Accepted)
         {
            // Get file name 
            QString fileName = export_Dialog.selectedFiles().first(); 

            //Add  file extention
            fileName += export_Dialog.selectedNameFilter().remove( 0, 1 );
             
            //Get TeX table settings
            TeXTableSettings tex_settings = export_Dialog.tex_TableSettings();

            //Export to TeX table
            export_to_TeX( fileName, tex_settings );
         }

}

bool Table::export_to_TeX( QString fileName, TeXTableSettings& tex_settings )
{

    WAIT_CURSOR;

    QFile file( fileName );

	 if (!file.open( QIODevice::WriteOnly ))
	 {
		 QApplication::restoreOverrideCursor();
		 QMessageBox::critical(0, tr("TeX Export Error"),
				 tr("Could not write to file: <br><h4>%1</h4><p>Please verify that you have the right to write to this location!").arg(fileName));
		 return false;
	 }

   
   QList< Column* > columns_list;  
   int first_row_index = 0;
   int last_row_index = 0;


   if (d_view->selectedColumnCount() != 0)
     {
       //Get selected columns
       for (int i = 0; i < columnCount(); i++)
         {
            if (d_view->isColumnSelected( i )) 
                columns_list << column( i );  
         }

       // Get the first and last selected row index 
       first_row_index = d_view->firstSelectedRow();
       last_row_index = d_view->lastSelectedRow();
     }
   else
     {
       //Get all columns 
       for( int i = 0; i < columnCount(); i++ ) 
           columns_list << column( i );

       // Get the first and last row index
       first_row_index = 0;
       if( rowCount() > 0 ) last_row_index = rowCount()-1;
       else last_row_index = 0;
     }     

   // Get the TeX column alignment
   char cl_alignment = 'c'; 
   if (tex_settings.columnsAlignment() == ALIGN_LEFT) cl_alignment = 'l';
   else if (tex_settings.columnsAlignment() == ALIGN_RIGHT) cl_alignment = 'r';

   QTextStream out(&file);

   //Check whether the TeX table should have caption 
   if (tex_settings.with_caption()) 
       out << "\\begin{table} \n \\caption{" << name() << "}\n"; 

   //begin tabular with all parameters
   out << QString("\\begin{tabular}{|*{") + QString().setNum( columns_list.count() ) 
          + ("}{") + QString(cl_alignment) + QString("|}}\n");
   out << "\\hline \n";

   //Check if export with table labels
   if (tex_settings.with_labels())
     {
         //Get the columns labes        
         QStringList columns_labels; 
         foreach (Column* col, columns_list) columns_labels << col->name();
         out << columns_labels.join(" & ")  << "\\\\ \\hline \n";
     }

         
    //Export TeX table content
    QStringList str_row;
    for ( int row_index = first_row_index; row_index < last_row_index+1; row_index++)
      {
         str_row.clear(); 
         foreach( Column* col, columns_list ) 
           str_row << col->asStringColumn()->textAt( row_index );
         out << str_row.join(" & ") << " \\\\ \\hline \n "; 
     }

   out << "\\end{tabular} \n";

   if( tex_settings.with_caption() ) out << "\\end{table} \n";
   
   RESET_CURSOR;   

   return true;        
}




QMenu * Table::createSelectionMenu(QMenu * append_to)
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	QMenu * submenu = new QMenu(tr("Fi&ll Selection with"));
	submenu->addAction(action_fill_row_numbers);
	submenu->addAction(action_fill_random);
	menu->addMenu(submenu);
	menu->addSeparator();

	menu->addAction(action_cut_selection);
	menu->addAction(action_copy_selection);
	menu->addAction(action_paste_into_selection);
	menu->addAction(action_clear_selection);
	menu->addSeparator();
#ifndef LEGACY_CODE_0_2_x
	menu->addAction(action_mask_selection);
	menu->addAction(action_unmask_selection);
	menu->addSeparator();
#endif
	menu->addAction(action_normalize_selection);
	menu->addSeparator();
	menu->addAction(action_set_formula);
	menu->addAction(action_recalculate);
	menu->addSeparator();

	return menu;
}


QMenu * Table::createColumnMenu(QMenu * append_to)
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	QMenu * submenu = new QMenu(tr("S&et Column(s) As"));
	submenu->addAction(action_set_as_x);
	submenu->addAction(action_set_as_y);
	submenu->addAction(action_set_as_z);
	submenu->addSeparator();
	submenu->addAction(action_set_as_xerr);
	submenu->addAction(action_set_as_yerr);
	submenu->addSeparator();
	submenu->addAction(action_set_as_none);
	menu->addMenu(submenu);
	menu->addSeparator();

	submenu = new QMenu(tr("Fi&ll Selection with"));
	submenu->addAction(action_fill_row_numbers);
	submenu->addAction(action_fill_random);
	menu->addMenu(submenu);
	menu->addSeparator();

	menu->addAction(action_insert_columns);
	menu->addAction(action_remove_columns);
	menu->addAction(action_clear_columns);
	menu->addAction(action_add_columns);
	menu->addSeparator();
	
	menu->addAction(action_normalize_columns);
	menu->addAction(action_sort_columns);
	menu->addSeparator();

	menu->addAction(action_edit_description);
	menu->addAction(action_type_format);
	connect(menu, SIGNAL(aboutToShow()), this, SLOT(adjustActionNames()));
	menu->addAction(action_toggle_comments);
	menu->addSeparator();

	menu->addAction(action_statistics_columns);

	return menu;
}

QMenu * Table::createTableMenu(QMenu * append_to) 
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	connect(menu, SIGNAL(aboutToShow()), this, SLOT(adjustActionNames()));
	menu->addAction(action_toggle_comments);
	menu->addAction(action_toggle_tabbar);
	menu->addAction(action_formula_mode);
	menu->addSeparator();
	menu->addAction(action_select_all);
	menu->addAction(action_clear_table);
        menu->addAction(action_export_to_TeX);
#ifndef LEGACY_CODE_0_2_x
	menu->addAction(action_clear_masks);
	menu->addAction(action_sort_table);
#endif
	menu->addSeparator();
	menu->addAction(action_add_column);
	menu->addSeparator();
	menu->addAction(action_go_to_cell);

	return menu;
}

QMenu * Table::createRowMenu(QMenu * append_to) 
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addAction(action_insert_rows);
	menu->addAction(action_remove_rows);
	menu->addAction(action_clear_rows);
	menu->addAction(action_add_rows);
	menu->addSeparator();
	QMenu *submenu = new QMenu(tr("Fi&ll Selection with"));
	submenu->addAction(action_fill_row_numbers);
	submenu->addAction(action_fill_random);
	menu->addMenu(submenu);
	menu->addSeparator();
	menu->addAction(action_statistics_rows);

	return menu;
}

void Table::goToCell()
{
	if (!d_view) return;
	bool ok;

	int col = QInputDialog::getInteger(0, tr("Go to Cell"), tr("Enter column"),
			1, 1, columnCount(), 1, &ok);
	if ( !ok ) return;

	int row = QInputDialog::getInteger(0, tr("Go to Cell"), tr("Enter row"),
			1, 1, rowCount(), 1, &ok);
	if ( !ok ) return;

	d_view->goToCell(row-1, col-1);
}

void Table::dimensionsDialog()
{
	Ui::DimensionsDialog ui;
	QDialog dialog;
	ui.setupUi(&dialog);
	dialog.setWindowTitle(tr("Set Table Dimensions"));
	ui.columnsSpinBox->setValue(columnCount());
	ui.rowsSpinBox->setValue(rowCount());
	connect(ui.buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));
	connect(ui.buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));

	if (dialog.exec())
	{
		setColumnCount(ui.columnsSpinBox->value());
		setRowCount(ui.rowsSpinBox->value());
	}
}

void Table::moveColumn(int from, int to)
{
	beginMacro(tr("%1: move column %2 from position %3 to %4.").arg(name()).arg(d_table_private->column(from)->name()).arg(from+1).arg(to+1));
	moveChild(from, to);
	exec(new TableMoveColumnCmd(d_table_private, from, to));	
	endMacro();
}

void Table::copy(Table * other)
{
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: copy %2").arg(name()).arg(other->name()));
	
	removeColumns(0, columnCount());
	QList<Column *> columns;
	for (int i=0; i<other->columnCount(); i++)
	{
		Column * src_col = other->column(i);
		Column * new_col = new Column(src_col->name(), src_col->columnMode());
		new_col->copy(src_col);
		new_col->setPlotDesignation(src_col->plotDesignation());
		QList< Interval<int> > masks = src_col->maskedIntervals();
		foreach(Interval<int> iv, masks)
			new_col->setMasked(iv);
		QList< Interval<int> > formulas = src_col->formulaIntervals();
		foreach(Interval<int> iv, formulas)
			new_col->setFormula(iv, src_col->formula(iv.start()));
		columns.append(new_col);
	}
	appendColumns(columns);
	setCaptionSpec(other->captionSpec());
	setComment(other->comment());
	for (int i=0; i<columnCount(); i++)
		setColumnWidth(i, other->columnWidth(i));
	if (d_view) d_view->rereadSectionSizes();

	endMacro();
	RESET_CURSOR;
}

int Table::colX(int col)
{
	for(int i=col-1; i>=0; i--)
	{
		if (column(i)->plotDesignation() == SciDAVis::X)
			return i;
	}
	int cols = columnCount();
	for(int i=col+1; i<cols; i++)
	{
		if (column(i)->plotDesignation() == SciDAVis::X)
			return i;
	}
	return -1;
}

int Table::colY(int col)
{
	int cols = columnCount();

	if (column(col)->plotDesignation() == SciDAVis::xErr || 
			column(col)->plotDesignation() == SciDAVis::yErr) {
		// look to the left first
		for(int i=col-1; i>=0; i--) {
			if (column(i)->plotDesignation() == SciDAVis::Y)
				return i;
		}
		for(int i=col+1; i<cols; i++) {
			if (column(i)->plotDesignation() == SciDAVis::Y)
				return i;
		}
	} else {
		// look to the right first
		for(int i=col+1; i<cols; i++) {
			if (column(i)->plotDesignation() == SciDAVis::Y)
				return i;
		}
		for(int i=col-1; i>=0; i--) {
			if (column(i)->plotDesignation() == SciDAVis::Y)
				return i;
		}
	}
	return -1;
}

void Table::setPlotMenu(QMenu * menu)
{
	d_plot_menu = menu;
}

void Table::sortDialog(QList<Column*> cols)
{
	if(cols.isEmpty()) return;

	SortDialog *sortd = new future::SortDialog();
	sortd->setAttribute(Qt::WA_DeleteOnClose);
	connect(sortd, SIGNAL(sort(Column*,QList<Column*>,bool)), this, SLOT(sortColumns(Column*,QList<Column*>,bool)));
	sortd->setColumnsList(cols);
	sortd->exec();
}


void Table::sortColumns(Column *leading, QList<Column*> cols, bool ascending)
{
	if(cols.isEmpty()) return;

	// the normal QPair comparison does not work properly with descending sorting
	// thefore we use our own compare functions
	class CompareFunctions{ 
		public:
			static bool doubleLess(const QPair<double, int>& a, const QPair<double, int>& b)
			{
				return a.first < b.first;
			}
			static bool doubleGreater(const QPair<double, int>& a, const QPair<double, int>& b)
			{
				return a.first > b.first;
			}
			static bool QStringLess(const QPair<QString, int>& a, const QPair<QString, int>& b)
			{
				return a < b;
			}
			static bool QStringGreater(const QPair<QString, int>& a, const QPair<QString, int>& b)
			{
				return a > b;
			}
			static bool QDateTimeLess(const QPair<QDateTime, int>& a, const QPair<QDateTime, int>& b)
			{
				return a < b;
			}
			static bool QDateTimeGreater(const QPair<QDateTime, int>& a, const QPair<QDateTime, int>& b)
			{
				return a > b;
			}
	};

	WAIT_CURSOR;
	beginMacro(tr("%1: sort column(s)").arg(name()));

	if(leading == 0) // sort separately
	{
		for(int i=0; i<cols.size(); i++)
		{
			Column* col = cols.at(i);

			if(col->dataType() == SciDAVis::TypeDouble)
			{
				int rows = col->rowCount();
				QList< QPair<double, int> > map;

				for(int j=0; j<rows; j++)
					map.append(QPair<double, int>(col->valueAt(j), j));
	
				if(ascending)
					qStableSort(map.begin(), map.end(), CompareFunctions::doubleLess);
				else
					qStableSort(map.begin(), map.end(), CompareFunctions::doubleGreater);

				QListIterator< QPair<double, int> > it(map);
				Column *temp_col = new Column("temp", col->columnMode());
				
				int k=0;
				// put the values in the right order into temp_col
				while(it.hasNext())
				{
					temp_col->copy(col, it.peekNext().second, k, 1);
					temp_col->setMasked(col->isMasked(it.next().second));
					k++;
				}
				// copy the sorted column
				col->copy(temp_col, 0, 0, rows);
				delete temp_col;
			}
			else if(col->dataType() == SciDAVis::TypeQString)
			{
				int rows = col->rowCount();
				QList< QPair<QString, int> > map;

				for(int j=0; j<rows; j++)
					map.append(QPair<QString, int>(col->textAt(j), j));
	
				if(ascending)
					qStableSort(map.begin(), map.end(), CompareFunctions::QStringLess);
				else
					qStableSort(map.begin(), map.end(), CompareFunctions::QStringGreater);

				QListIterator< QPair<QString, int> > it(map);
				Column *temp_col = new Column("temp", col->columnMode());
				
				int k=0;
				// put the values in the right order into temp_col
				while(it.hasNext())
				{
					temp_col->copy(col, it.peekNext().second, k, 1);
					temp_col->setMasked(col->isMasked(it.next().second));
					k++;
				}
				// copy the sorted column
				col->copy(temp_col, 0, 0, rows);
				delete temp_col;
			}
			else if(col->dataType() == SciDAVis::TypeQDateTime)
			{
				int rows = col->rowCount();
				QList< QPair<QDateTime, int> > map;

				for(int j=0; j<rows; j++)
					map.append(QPair<QDateTime, int>(col->dateTimeAt(j), j));
	
				if(ascending)
					qStableSort(map.begin(), map.end(), CompareFunctions::QDateTimeLess);
				else
					qStableSort(map.begin(), map.end(), CompareFunctions::QDateTimeGreater);

				QListIterator< QPair<QDateTime, int> > it(map);
				Column *temp_col = new Column("temp", col->columnMode());
				
				int k=0;
				// put the values in the right order into temp_col
				while(it.hasNext())
				{
					temp_col->copy(col, it.peekNext().second, k, 1);
					temp_col->setMasked(col->isMasked(it.next().second));
					k++;
				}
				// copy the sorted column
				col->copy(temp_col, 0, 0, rows);
				delete temp_col;
			}
		}
		
	}
	else // sort with leading column
	{
		if(leading->dataType() == SciDAVis::TypeDouble)
		{
			QList< QPair<double, int> > map;
			int rows = leading->rowCount();

			for(int i=0; i<rows; i++)
				map.append(QPair<double, int>(leading->valueAt(i), i));

			if(ascending)
				qStableSort(map.begin(), map.end(), CompareFunctions::doubleLess);
			else
				qStableSort(map.begin(), map.end(), CompareFunctions::doubleGreater);
			QListIterator< QPair<double, int> > it(map);

			for(int i=0; i<cols.size(); i++) 
			{
				Column *temp_col = new Column("temp", cols.at(i)->columnMode());
				it.toFront();
				int j=0;
				// put the values in the right order into temp_col
				while(it.hasNext())
				{
					temp_col->copy(cols.at(i), it.peekNext().second, j, 1);
					temp_col->setMasked(cols.at(i)->isMasked(it.next().second));
					j++;
				}
				// copy the sorted column
				cols.at(i)->copy(temp_col, 0, 0, rows);
				delete temp_col;
			}
		}
		else if(leading->dataType() == SciDAVis::TypeQString)
		{
			QList< QPair<QString, int> > map;
			int rows = leading->rowCount();

			for(int i=0; i<rows; i++)
				map.append(QPair<QString, int>(leading->textAt(i), i));

			if(ascending)
				qStableSort(map.begin(), map.end(), CompareFunctions::QStringLess);
			else
				qStableSort(map.begin(), map.end(), CompareFunctions::QStringGreater);
			QListIterator< QPair<QString, int> > it(map);

			for(int i=0; i<cols.size(); i++) 
			{
				Column *temp_col = new Column("temp", cols.at(i)->columnMode());
				it.toFront();
				int j=0;
				// put the values in the right order into temp_col
				while(it.hasNext())
				{
					temp_col->copy(cols.at(i), it.peekNext().second, j, 1);
					temp_col->setMasked(cols.at(i)->isMasked(it.next().second));
					j++;
				}
				// copy the sorted column
				cols.at(i)->copy(temp_col, 0, 0, rows);
				delete temp_col;
			}
		}
		else if(leading->dataType() == SciDAVis::TypeQDateTime)
		{
			QList< QPair<QDateTime, int> > map;
			int rows = leading->rowCount();

			for(int i=0; i<rows; i++)
				map.append(QPair<QDateTime, int>(leading->dateTimeAt(i), i));

			if(ascending)
				qStableSort(map.begin(), map.end(), CompareFunctions::QDateTimeLess);
			else
				qStableSort(map.begin(), map.end(), CompareFunctions::QDateTimeGreater);
			QListIterator< QPair<QDateTime, int> > it(map);

			for(int i=0; i<cols.size(); i++) 
			{
				Column *temp_col = new Column("temp", cols.at(i)->columnMode());
				it.toFront();
				int j=0;
				// put the values in the right order into temp_col
				while(it.hasNext())
				{
					temp_col->copy(cols.at(i), it.peekNext().second, j, 1);
					temp_col->setMasked(cols.at(i)->isMasked(it.next().second));
					j++;
				}
				// copy the sorted column
				cols.at(i)->copy(temp_col, 0, 0, rows);
				delete temp_col;
			}
		}
	}
	endMacro();
	RESET_CURSOR;
} // end of sortColumns()


QIcon Table::icon() const
{
	QIcon ico;
	ico.addPixmap(QPixmap(":/16x16/table.png"));
	ico.addPixmap(QPixmap(":/24x24/table.png"));
	ico.addPixmap(QPixmap(":/32x32/table.png"));
	return ico;
}

QString Table::text(int row, int col)
{
	Column * col_ptr = column(col);
	if(!col_ptr)
		return QString();
	if(col_ptr->isInvalid(row))
		return QString();

	AbstractSimpleFilter * out_fltr = col_ptr->outputFilter();
	out_fltr->input(0, col_ptr);
	return out_fltr->output(0)->textAt(row);
}

void Table::selectAll()
{
	if (!d_view) return;
	d_view->selectAll();
}

void Table::handleModeChange(const AbstractColumn * col)
{
	int index = columnIndex(static_cast<const Column *>(col));
	if(index != -1)
		d_table_private->updateHorizontalHeader(index, index);
}

void Table::handleDescriptionChange(const AbstractAspect * aspect)
{
	int index = columnIndex(static_cast<const Column *>(aspect));
	if(index != -1)
		d_table_private->updateHorizontalHeader(index, index);
}

void Table::handlePlotDesignationChange(const AbstractColumn * col)
{
	int index = columnIndex(static_cast<const Column *>(col));
	if(index != -1)
		d_table_private->updateHorizontalHeader(index, columnCount()-1);
}

void Table::handleDataChange(const AbstractColumn * col)
{
	int index = columnIndex(static_cast<const Column *>(col));
	if(index != -1)
	{
		if (col->rowCount() > rowCount())
			setRowCount(col->rowCount());
		emit dataChanged(0, index, col->rowCount()-1, index);	
	}
}

void Table::handleRowsAboutToBeInserted(const AbstractColumn * col, int before, int count)
{
	int new_size = col->rowCount() + count; 
	if(before <= col->rowCount() && new_size > rowCount())
		setRowCount(new_size);
}

void Table::handleRowsInserted(const AbstractColumn * col, int before, int count)
{
	Q_UNUSED(count);
	int index = columnIndex(static_cast<const Column *>(col));
	if(index != -1 && before <= col->rowCount())
		emit dataChanged(before, index, col->rowCount()-1, index);
}


void Table::handleRowsAboutToBeRemoved(const AbstractColumn * col, int first, int count)
{
	Q_UNUSED(col);
	Q_UNUSED(first);
	Q_UNUSED(count);
}

void Table::handleRowsRemoved(const AbstractColumn * col, int first, int count)
{
	Q_UNUSED(count);
	int index = columnIndex(static_cast<const Column *>(col));
	if(index != -1)
		emit dataChanged(first, index, col->rowCount()-1, index);
}

void Table::connectColumn(const Column* col)
{
	connect(col, SIGNAL(aspectDescriptionChanged(const AbstractAspect *)), this, 
			SLOT(handleDescriptionChange(const AbstractAspect *)));
	connect(col, SIGNAL(plotDesignationChanged(const AbstractColumn *)), this, 
			SLOT(handlePlotDesignationChange(const AbstractColumn *)));
	connect(col, SIGNAL(modeChanged(const AbstractColumn *)), this, 
			SLOT(handleDataChange(const AbstractColumn *)));
	connect(col, SIGNAL(dataChanged(const AbstractColumn *)), this, 
			SLOT(handleDataChange(const AbstractColumn *)));
	connect(col, SIGNAL(modeChanged(const AbstractColumn *)), this, 
			SLOT(handleModeChange(const AbstractColumn *)));
	connect(col, SIGNAL(rowsAboutToBeInserted(const AbstractColumn *, int, int)), this, 
			SLOT(handleRowsAboutToBeInserted(const AbstractColumn *,int,int)));
	connect(col, SIGNAL(rowsInserted(const AbstractColumn *, int, int)), this, 
			SLOT(handleRowsInserted(const AbstractColumn *,int,int))); 
	connect(col, SIGNAL(rowsAboutToBeRemoved(const AbstractColumn *, int, int)), this, 
			SLOT(handleRowsAboutToBeRemoved(const AbstractColumn *,int,int))); 
	connect(col, SIGNAL(rowsRemoved(const AbstractColumn *, int, int)), this, 
			SLOT(handleRowsRemoved(const AbstractColumn *,int,int))); 
	connect(col, SIGNAL(maskingChanged(const AbstractColumn *)), this, 
			SLOT(handleDataChange(const AbstractColumn *))); 
}

void Table::disconnectColumn(const Column* col)
{
	disconnect(col, 0, this, 0);
}

QVariant Table::headerData(int section, Qt::Orientation orientation,int role) const
{
	return d_table_private->headerData(section, orientation, role);
}

void Table::completeAspectInsertion(AbstractAspect * aspect, int index)
{
	Column * column = qobject_cast<Column *>(aspect);
	if (!column) return;
	QList<Column*> cols;
	cols.append(column);
	exec(new TableInsertColumnsCmd(d_table_private, index, cols));
}

void Table::prepareAspectRemoval(AbstractAspect * aspect)
{
	Column * column = qobject_cast<Column *>(aspect);
	if (!column) return;
	int first = columnIndex(column);
	QList<Column*> cols;
	cols.append(column);
	exec(new TableRemoveColumnsCmd(d_table_private, first, 1, cols));
}

void Table::save(QXmlStreamWriter * writer) const
{
	int cols = columnCount();
	int rows = rowCount();
	writer->writeStartElement("table");
	writeBasicAttributes(writer);
	writer->writeAttribute("columns", QString::number(cols));
	writer->writeAttribute("rows", QString::number(rows));
	writeCommentElement(writer);

	for (int col=0; col<cols; col++)
		column(col)->save(writer);
	for (int col=0; col<cols; col++)
	{
		writer->writeStartElement("column_width");
		writer->writeAttribute("column", QString::number(col));
		writer->writeCharacters(QString::number(columnWidth(col)));
		writer->writeEndElement();
	}
	writer->writeEndElement(); // "table"
}

bool Table::load(XmlStreamReader * reader)
{
	if(reader->isStartElement() && reader->name() == "table") 
	{
		setColumnCount(0);
		setRowCount(0);
		setComment("");

		if (!readBasicAttributes(reader)) return false;

		// read dimensions
		bool ok1, ok2;
		int rows, cols;
		rows = reader->readAttributeInt("rows", &ok1);
		cols = reader->readAttributeInt("columns", &ok2);
		if(!ok1 || !ok2) 
		{
			reader->raiseError(tr("invalid row or column count"));
			return false;
		}

		setRowCount(rows);
		// read child elements
		while (!reader->atEnd()) 
		{
			reader->readNext();

			if (reader->isEndElement()) break;

			if (reader->isStartElement()) 
			{
				if (reader->name() == "comment")
				{
					if (!readCommentElement(reader)) return false;
				}
				else if(reader->name() == "column")
				{
					Column * column = new Column(tr("Column %1").arg(1), SciDAVis::Text);
					if (!column->load(reader))
					{
						setColumnCount(0);
						return false;
					}
					QList<Column *> columns;
					columns.append(column);
					appendColumns(columns);
				}
				else if(reader->name() == "column_width")
				{
					if (!readColumnWidthElement(reader)) return false;
				}
				else // unknown element
				{
					reader->raiseWarning(tr("unknown element '%1'").arg(reader->name().toString()));
					if (!reader->skipToEndElement()) return false;
				}
			} 
		}
		if (cols != columnCount())
			reader->raiseWarning(tr("columns attribute and number of read columns do not match"));
	}
	else // no table element
		reader->raiseError(tr("no table element found"));

	return !reader->hasError();
}

void Table::adjustActionNames()
{
	if (!d_view) return;

	QString action_name;
	if(d_view->areCommentsShown()) 
		action_name = tr("Hide Comments");
	else
		action_name = tr("Show Comments");
	action_toggle_comments->setText(action_name);

	if(d_view->isControlTabBarVisible()) 
		action_name = tr("Hide Controls");
	else
		action_name = tr("Show Controls");
	action_toggle_tabbar->setText(action_name);
}

bool Table::readColumnWidthElement(XmlStreamReader * reader)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "column_width");
	bool ok;
	int col = reader->readAttributeInt("column", &ok);
	if(!ok)
	{
		reader->raiseError(tr("invalid or missing column index"));
		return false;
	}
	QString str = reader->readElementText();
	int value = str.toInt(&ok);
	if(!ok)
	{
		reader->raiseError(tr("invalid column width"));
		return false;
	}
	if (d_view)
		d_view->setColumnWidth(col, value);
	else
		setColumnWidth(col, value);
	return true;
}

void Table::setColumnWidth(int col, int width) 
{ 
	d_table_private->setColumnWidth(col, width); 
}

int Table::columnWidth(int col) const 
{ 
	return d_table_private->columnWidth(col); 
}


/* ========================= static methods ======================= */
ActionManager * Table::action_manager = 0;

ActionManager * Table::actionManager()
{
	if (!action_manager)
		initActionManager();
	
	return action_manager;
}

void Table::initActionManager()
{
	if (!action_manager)
		action_manager = new ActionManager();

	action_manager->setTitle(tr("Table"));
	volatile Table * action_creator = new Table(); // initialize the action texts
	delete action_creator;
}

/* ========================== Table::Private ====================== */

Column * Table::Private::column(int index) const		
{ 
	return d_columns.value(index); 
}

void Table::Private::replaceColumns(int first, QList<Column*> new_cols)
{
	if( (first < 0) || (first + new_cols.size() > d_column_count) )
		return;

	int count = new_cols.size();
	emit d_owner->columnsAboutToBeReplaced(first, new_cols.count());
	for(int i=0; i<count; i++)
	{
		int rows = new_cols.at(i)->rowCount();
		if(rows > d_row_count)
			setRowCount(rows); 

		if(d_columns.at(first+i))
			d_columns.at(first+i)->notifyReplacement(new_cols.at(i));

		d_columns[first+i] = new_cols.at(i);
		d_owner->connectColumn(new_cols.at(i));
	}
	updateHorizontalHeader(first, first+count-1);
	emit d_owner->columnsReplaced(first, new_cols.count());
	emit d_owner->dataChanged(0, first, d_row_count-1, first+count-1);
}

void Table::Private::insertColumns(int before, QList<Column*> cols)
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

	emit d_owner->columnsAboutToBeInserted(before, cols);
	for(int i=count-1; i>=0; i--)
	{
		d_columns.insert(before, cols.at(i));
		d_owner->connectColumn(cols.at(i));
		d_column_widths.insert(before, Table::defaultColumnWidth());
	}
	d_column_count += count;
	emit d_owner->columnsInserted(before, cols.count());
	updateHorizontalHeader(before, before+count-1);
}

void Table::Private::removeColumns(int first, int count)
{
	if( (count < 1) || (first >= d_column_count) )
		return;

	Q_ASSERT(first >= 0);

	Q_ASSERT(first+count <= d_column_count);

	emit d_owner->columnsAboutToBeRemoved(first, count);
	for(int i=count-1; i>=0; i--)
	{
		d_owner->disconnectColumn(d_columns.at(first));
		d_columns.removeAt(first);
		d_column_widths.removeAt(first);
	}
	d_column_count -= count;
	emit d_owner->columnsRemoved(first, count);
	updateHorizontalHeader(first, d_column_count-1);
}

void Table::Private::appendColumns(QList<Column*> cols)
{
	insertColumns(d_column_count, cols);
}

void Table::Private::moveColumn(int from, int to)
{
	if( from < 0 || from >= d_column_count) return;
	if( to < 0 || to >= d_column_count) return;
	
	d_columns.move(from, to);
	d_owner->connectColumn(d_columns.at(to));
	d_column_widths.move(from, to);
	updateHorizontalHeader(qMin(from, to), qMax(from, to));
	emit d_owner->dataChanged(0, from, d_row_count-1, from);
	emit d_owner->dataChanged(0, to, d_row_count-1, to);
	if (d_owner->d_view) d_owner->d_view->rereadSectionSizes();
}

void Table::Private::setRowCount(int rows)
{
	int diff = rows - d_row_count;
	int old_row_count = d_row_count;
	if(diff == 0) 
		return;

	if(diff > 0)
	{
		emit d_owner->rowsAboutToBeInserted(d_row_count, diff);
		d_row_count = rows;
		updateVerticalHeader(d_row_count - diff);
		emit d_owner->rowsInserted(old_row_count, diff);
	}
	else
	{
		emit d_owner->rowsAboutToBeRemoved(rows, -diff);
		d_row_count = rows;
		emit d_owner->rowsRemoved(rows, -diff);
	}
}

QString Table::Private::columnHeader(int col)
{
	return headerData(col, Qt::Horizontal, Qt::DisplayRole).toString();
}

int Table::Private::numColsWithPD(SciDAVis::PlotDesignation pd)
{
	int count = 0;
	
	for (int i=0; i<d_column_count; i++)
		if(d_columns.at(i)->plotDesignation() == pd)
			count++;
	
	return count;
}

void Table::Private::updateVerticalHeader(int start_row)
{
	int current_size = d_vertical_header_data.size(), i;
	for(i=start_row; i<current_size; i++)
		d_vertical_header_data.replace(i, i+1);
	for(; i<d_row_count; i++)
		d_vertical_header_data << i+1;
	emit d_owner->headerDataChanged(Qt::Vertical, start_row, d_row_count -1);	
}

void Table::Private::updateHorizontalHeader(int start_col, int end_col)
{
	if (start_col > end_col) return;

	while(d_horizontal_header_data.size() < d_column_count)
		d_horizontal_header_data << QString();

	if(numColsWithPD(SciDAVis::X)>1)
	{
		int x_cols = 0;
		for (int i=0; i<d_column_count; i++)
		{
			if (d_columns.at(i)->plotDesignation() == SciDAVis::X)
				composeColumnHeader(i, d_columns.at(i)->name()+"[X" + QString::number(++x_cols) +"]");
			else if (d_columns.at(i)->plotDesignation() == SciDAVis::Y)
			{
				if(x_cols>0)
					composeColumnHeader(i, d_columns.at(i)->name()+"[Y"+ QString::number(x_cols) +"]");
				else
					composeColumnHeader(i, d_columns.at(i)->name()+"[Y]");
			}
			else if (d_columns.at(i)->plotDesignation() == SciDAVis::Z)
			{
				if(x_cols>0)
					composeColumnHeader(i, d_columns.at(i)->name()+"[Z"+ QString::number(x_cols) +"]");
				else
					composeColumnHeader(i, d_columns.at(i)->name()+"[Z]");
			}
			else if (d_columns.at(i)->plotDesignation() == SciDAVis::xErr)
			{
				if(x_cols>0)
					composeColumnHeader(i, d_columns.at(i)->name()+"[xEr"+ QString::number(x_cols) +"]");
				else
					composeColumnHeader(i, d_columns.at(i)->name()+"[xEr]");
			}
			else if (d_columns.at(i)->plotDesignation() == SciDAVis::yErr)
			{
				if(x_cols>0)
					composeColumnHeader(i, d_columns.at(i)->name()+"[yEr"+ QString::number(x_cols) +"]");
				else
					composeColumnHeader(i, d_columns.at(i)->name()+"[yEr]");
			}
			else
				composeColumnHeader(i, d_columns.at(i)->name());
		}
	}
	else
	{
		for (int i=0; i<d_column_count; i++)
		{
			if (d_columns.at(i)->plotDesignation() == SciDAVis::X)
				composeColumnHeader(i, d_columns.at(i)->name()+"[X]");
			else if(d_columns.at(i)->plotDesignation() == SciDAVis::Y)
				composeColumnHeader(i, d_columns.at(i)->name()+"[Y]");
			else if(d_columns.at(i)->plotDesignation() == SciDAVis::Z)
				composeColumnHeader(i, d_columns.at(i)->name()+"[Z]");
			else if(d_columns.at(i)->plotDesignation() == SciDAVis::xErr)
				composeColumnHeader(i, d_columns.at(i)->name()+"[xEr]");
			else if(d_columns.at(i)->plotDesignation() == SciDAVis::yErr)
				composeColumnHeader(i, d_columns.at(i)->name()+"[yEr]");
			else
				composeColumnHeader(i, d_columns.at(i)->name());
		}
	}
	emit d_owner->headerDataChanged(Qt::Horizontal, start_col, end_col);	
}

void Table::Private::composeColumnHeader(int col, const QString& label)
{
	if (col >= d_horizontal_header_data.size())
		d_horizontal_header_data << label;
	else
		d_horizontal_header_data.replace(col, label);
}

QVariant Table::Private::headerData(int section, Qt::Orientation orientation, int role) const
{
	switch(orientation) {
		case Qt::Horizontal:
			if (section >= d_horizontal_header_data.size())
				return QVariant();
			switch(role) {
				case Qt::DisplayRole:
				case Qt::ToolTipRole:
				case Qt::EditRole:
					return d_horizontal_header_data.at(section);
				case Qt::DecorationRole:
					return d_columns.at(section)->icon();
				case TableModel::CommentRole:
					return d_columns.at(section)->comment();
			}
			break;
		case Qt::Vertical:
			if (section >= d_vertical_header_data.size())
				return QVariant();
			switch(role) {
				case Qt::DisplayRole:
				case Qt::ToolTipRole:
					return d_vertical_header_data.at(section);
			}
			break;
	}
	return QVariant();
}

} // namespace
