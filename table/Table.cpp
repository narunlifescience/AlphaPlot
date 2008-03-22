/***************************************************************************
    File                 : Table.cpp
    Project              : SciDAVis
    Description          : Aspect providing a spreadsheet table with column logic
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2008 Tilman Hoener zu Siederdissen (thzs*gmx.net)
    Copyright            : (C) 2006-2008 Knut Franke (knut.franke*gmx.de)
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
#include "Table.h"
#include "Project.h"
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

#include "AbstractScript.h"
#include "AspectPrivate.h"
#include "TableModel.h"
#include "TableView.h"
#include "tablecommands.h"
#include "table/SortDialog.h"
#include "Column.h"
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

#define WAIT_CURSOR QApplication::setOverrideCursor(QCursor(Qt::WaitCursor))
#define RESET_CURSOR QApplication::restoreOverrideCursor()

bool Table::d_default_comment_visibility = false;

// TODO: move all selection related stuff to the primary view

Table::Table(AbstractScriptingEngine *engine, int rows, int columns, const QString& name)
	: AbstractPart(name), d_plot_menu(0), scripted(engine)
{
	d_table_private = new Private(this);

	// set initial number of rows and columns
	QList<Column*> cols;
	for(int i=0; i<columns; i++)
		cols << new Column(QString::number(i+1), SciDAVis::Numeric);
	appendColumns(cols);
	setRowCount(rows);

	d_view = new TableView(this); 
	createActions();
	connectActions();
	d_view->showComments(d_default_comment_visibility);
}

Table::Table()
	: AbstractPart("temp"), scripted(0)
{
	createActions();
}

Table::~Table()
{
}

Column * Table::column(int index) const
{ 
	return d_table_private->column(index); 
}

QWidget *Table::view()
{
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
	if( (new_size < 0) || (new_size == rowCount()) ) return;
	WAIT_CURSOR;
	exec(new TableSetNumberOfRowsCmd(d_table_private, new_size));
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
			cols << new Column(QString::number(i+1), SciDAVis::Numeric);
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
	WAIT_CURSOR;
	int count = d_view->selectedColumnCount(false);
	beginMacro(QObject::tr("%1: add %2 column(s)").arg(name()).arg(count));
	setColumnCount(columnCount() + count);
	endMacro();
	RESET_CURSOR;
}

void Table::cutSelection()
{
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(tr("%1: cut selected cell(s)").arg(name()));
	copySelection();
	QList<Column*> list = d_view->selectedColumns();
	Column * temp = new Column("temp", QStringList(QString()));
	foreach(Column * col_ptr, list)
	{
		AbstractSimpleFilter * in_fltr = col_ptr->inputFilter();
		in_fltr->input(0, temp);
		int col = columnIndex(col_ptr);
		for(int row=first; row<last; row++)
			if(d_view->isCellSelected(row, col)) col_ptr->copy(in_fltr->output(0), 0, row, 1);  
		in_fltr->input(0, 0);
	}
	delete temp;
	endMacro();
	RESET_CURSOR;
}

void Table::copySelection()
{
	int first_col = d_view->firstSelectedColumn(false);
	if(first_col == -1) return;
	int last_col = d_view->lastSelectedColumn(false);
	if(last_col == -1) return;
	int first_row = d_view->firstSelectedRow(false);
	if(first_row == -1)	return;
	int last_row = d_view->lastSelectedRow(false);
	if(last_row == -1) return;
	int cols = last_col - first_col +1;
	int rows = last_row - first_row +1;
	
	WAIT_CURSOR;
	QString output_str;

	for(int r=0; r<rows; r++)
	{
		for(int c=0; c<cols; c++)
		{
			if(d_view->isCellSelected(first_row + r, first_col + c))
				output_str += text(first_row + r, first_col + c);
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
	if(columnCount() < 1 || rowCount() < 1) return;

	WAIT_CURSOR;
	beginMacro(tr("%1: paste from clipboard").arg(name()));
	const QMimeData * mime_data = QApplication::clipboard()->mimeData();

	int first_col = d_view->firstSelectedColumn(false);
	int last_col = d_view->lastSelectedColumn(false);
	int first_row = d_view->firstSelectedRow(false);
	int last_row = d_view->lastSelectedRow(false);
	int input_row_count = 0;
	int input_col_count = 0;
	int rows, cols;

	if(mime_data->hasFormat("text/plain"))
	{
		QString input_str = QString(mime_data->data("text/plain"));
		QList< QStringList > cell_texts;
		QStringList input_rows(input_str.split("\n"));
		input_row_count = input_rows.count();
		input_col_count = 0;
		for(int i=0; i<input_row_count; i++)
		{
			cell_texts.append(input_rows.at(i).split("\t"));
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
					cols << new Column(QString::number(i+1), SciDAVis::Text);
				appendColumns(cols);
			}
			if(last_row >= rowCount())
				appendRows(last_row+1-rowCount());
			// select the rectangle to be pasted in
			d_view->setCellsSelected(first_row, first_col, last_row, last_col);
		}

		rows = last_row - first_row + 1;
		cols = last_col - first_col + 1;
		Column * temp = new Column("temp", QStringList(QString()));
		for(int r=0; r<rows && r<input_row_count; r++)
		{
			for(int c=0; c<cols && c<input_col_count; c++)
			{
				if(d_view->isCellSelected(first_row + r, first_col + c) && (c < cell_texts.at(r).count()) )
				{
					Column * col_ptr = d_table_private->column(first_col + c);
					AbstractSimpleFilter * in_fltr = col_ptr->inputFilter();
					temp->setTextAt(0, cell_texts.at(r).at(c));
					in_fltr->input(0, temp);
					col_ptr->copy(in_fltr->output(0), 0, first_row+c, 1);  
					in_fltr->input(0,0);
				}
			}
		}
		delete temp;
	}
	endMacro();
	RESET_CURSOR;
}

void Table::maskSelection()
{
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(tr("%1: mask selected cell(s)").arg(name()));
	QList<Column*> list = d_view->selectedColumns();
	foreach(Column * col_ptr, list)
	{
		int col = columnIndex(col_ptr);
		for(int row=first; row<last; row++)
			if(d_view->isCellSelected(row, col)) col_ptr->setMasked(row);  
	}
	endMacro();
	RESET_CURSOR;
}

void Table::unmaskSelection()
{
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(tr("%1: unmask selected cell(s)").arg(name()));
	QList<Column*> list = d_view->selectedColumns();
	foreach(Column * col_ptr, list)
	{
		int col = columnIndex(col_ptr);
		for(int row=first; row<last; row++)
			if(d_view->isCellSelected(row, col)) col_ptr->setMasked(row, false);  
	}
	endMacro();
	RESET_CURSOR;
}

void Table::setFormulaForSelection()
{
	d_view->showControlFormulaTab();
}

void Table::recalculateSelectedCells()
{
	// TODO
	QMessageBox::information(0, "info", "not yet implemented");
}

void Table::fillSelectedCellsWithRowNumbers()
{
	if(d_view->selectedColumnCount() < 1) return;
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;
	
	WAIT_CURSOR;
	beginMacro(tr("%1: fill cells with row numbers").arg(name()));
	QList<Column*> list = d_view->selectedColumns();
	Column * temp = new Column("temp", QStringList(QString()));
	foreach(Column * col_ptr, list)
	{
		AbstractSimpleFilter *in_fltr = col_ptr->inputFilter();
		in_fltr->input(0, temp);
		int col = columnIndex(col_ptr);
		for(int row=first; row<=last; row++)
			if(d_view->isCellSelected(row, col)) 
			{
				temp->setTextAt(0, QString::number(row+1));
				col_ptr->copy(in_fltr->output(0), 0, row, 1);  
			}
		in_fltr->input(0, 0);
	}
	delete temp;
	endMacro();
	RESET_CURSOR;
}

void Table::fillSelectedCellsWithRandomNumbers()
{
	if(d_view->selectedColumnCount() < 1) return;
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;
	
	WAIT_CURSOR;
	beginMacro(tr("%1: fill cells with random values").arg(name()));
	qsrand(QTime::currentTime().msec());
	QList<Column*> list = d_view->selectedColumns();
	foreach(Column * col_ptr, list)
	{
		int col = columnIndex(col_ptr);
		for(int row=first; row<last; row++)
			if(d_view->isCellSelected(row, col)) 
			{
				if (col_ptr->columnMode() == SciDAVis::Numeric)
					col_ptr->setValueAt(row, double(qrand())/double(RAND_MAX));
				else if (col_ptr->dataType() == SciDAVis::TypeQDateTime)
				{
					QDate date(1,1,1);
					QTime time(0,0,0,0);
					int days = (int)( (double)date.daysTo(QDate(2999,12,31)) * double(qrand())/double(RAND_MAX) );
					qint64 msecs = (qint64)(double(qrand())/double(RAND_MAX) * 1000.0 * 60.0 * 60.0 * 24.0);
					col_ptr->setDateTimeAt(row, QDateTime(date.addDays(days), time.addMSecs(msecs)));
				}
				else
					col_ptr->setTextAt(row, QString::number(double(qrand())/double(RAND_MAX)));
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
			cols << new Column(QString::number(i+1), SciDAVis::Numeric);
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
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: clear selected column(s)").arg(name()));

	QList< Column* > list = d_view->selectedColumns();
	foreach(Column* ptr, list)
		ptr->clear();

	endMacro();
	RESET_CURSOR;
}

void Table::setSelectionAs(SciDAVis::PlotDesignation pd)
{
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

void Table::normalizeSelectedColumns()
{
	// TODO
	QMessageBox::information(0, "info", "not yet implemented");
}

void Table::sortSelectedColumns()
{
	QList< Column* > cols = d_view->selectedColumns();
	sortDialog(cols);
}

void Table::statisticsOnSelectedColumns()
{
	// TODO
	QMessageBox::information(0, "info", "not yet implemented");
}

void Table::statisticsOnSelectedRows()
{
	// TODO
	QMessageBox::information(0, "info", "not yet implemented");
}

void Table::insertEmptyRows()
{
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
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: remove selected rows(s)").arg(name()));
	for(int i=last; i>=first; i--)
		if(d_view->isRowSelected(i, false)) removeRows(i, 1);
	endMacro();
	RESET_CURSOR;
}

void Table::clearSelectedRows()
{
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: clear selected rows(s)").arg(name()));
	QList<Column*> list = d_view->selectedColumns();
	Column * temp = new Column("temp", QStringList(QString()));
	foreach(Column * col_ptr, list)
	{
		AbstractSimpleFilter *in_fltr = col_ptr->inputFilter();
		in_fltr->input(0, temp);
		for(int row=last; row>=first; row--)
			if(d_view->isRowSelected(row, false))
			{
				if(row == (col_ptr->rowCount()-1) )
					col_ptr->removeRows(row,1);
				else if(row < col_ptr->rowCount())
					col_ptr->copy(in_fltr->output(0), 0, row, 1);  
			}
		in_fltr->input(0, 0);
	}
	delete temp;
	endMacro();
	RESET_CURSOR;
}

void Table::editTypeAndFormatOfSelectedColumns()
{
	d_view->showControlTypeTab();
}

void Table::editDescriptionOfCurrentColumn()
{
	d_view->showControlDescriptionTab();
}

void Table::addRows()
{
	WAIT_CURSOR;
	int count = d_view->selectedRowCount(false);
	beginMacro(QObject::tr("%1: add %2 rows(s)").arg(name()).arg(count));
	exec(new TableSetNumberOfRowsCmd(d_table_private, rowCount() + count));
	endMacro();
	RESET_CURSOR;
}

void Table::clearSelectedCells()
{
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(tr("%1: clear selected cell(s)").arg(name()));
	QList<Column*> list = d_view->selectedColumns();
	Column * temp = new Column("temp", QStringList(QString()));
	foreach(Column * col_ptr, list)
	{
		AbstractSimpleFilter *in_fltr = col_ptr->inputFilter();
		in_fltr->input(0, temp);
		int col = columnIndex(col_ptr);
		for(int row=last; row>=first; row--)
			if(d_view->isCellSelected(row, col))
			{
				if(row == (col_ptr->rowCount()-1) )
					col_ptr->removeRows(row,1);
				else if(row < col_ptr->rowCount())
					col_ptr->copy(in_fltr->output(0), 0, row, 1);  
			}
		in_fltr->input(0, 0);
	}
	delete temp;
	endMacro();
	RESET_CURSOR;
}

bool Table::fillProjectMenu(QMenu * menu)
{
	menu->setTitle(tr("&Table"));

	menu->addAction(action_toggle_comments);
	menu->addAction(action_toggle_tabbar);
	menu->addSeparator();
	menu->addAction(action_clear_table);
	menu->addAction(action_clear_masks);
	menu->addAction(action_sort_table);
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

QMenu *Table::createContextMenu() const
{
	QMenu *menu = AbstractPart::createContextMenu();
	Q_ASSERT(menu);
	menu->addSeparator();
	
	// TODO: add real actions here
	menu->addAction(new QAction(tr("E&xport to ASCII"), d_view));

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

	action_mask_selection = new QAction(QIcon(QPixmap(":/mask.xpm")), tr("&Mask","mask selection"), this);
	actionManager()->addAction(action_mask_selection, "mask_selection"); 

	action_unmask_selection = new QAction(QIcon(QPixmap(":/unmask.xpm")), tr("&Unmask","unmask selection"), this);
	actionManager()->addAction(action_unmask_selection, "unmask_selection"); 

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/fx.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/fx.png"));
	action_set_formula = new QAction(*icon_temp, tr("Assign &Formula"), this);
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
	actionManager()->addAction(action_toggle_tabbar, "toggle_tabbar"); 
	delete icon_temp;

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
	actionManager()->addAction(action_add_column, "add_column"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/clear_table.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/clear_table.png"));
	action_clear_table = new QAction(*icon_temp, tr("Clear Table"), this);
	actionManager()->addAction(action_clear_table, "clear_table"); 
	delete icon_temp;

	action_clear_masks = new QAction(QIcon(QPixmap(":/unmask.xpm")), tr("Clear Masks"), this);
	actionManager()->addAction(action_clear_masks, "clear_masks"); 

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
	actionManager()->addAction(action_go_to_cell, "go_to_cell"); 
	delete icon_temp;

	action_dimensions_dialog = new QAction(QIcon(QPixmap(":/resize.xpm")), tr("&Dimensions", "table size"), this);
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

	action_set_as_none = new QAction(QIcon(QPixmap()), tr("None","plot designation"), this);;
	actionManager()->addAction(action_set_as_none, "set_as_none"); 

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/normalize.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/normalize.png"));
	action_normalize_columns = new QAction(*icon_temp, tr("&Normalize Columns"), this);;
	actionManager()->addAction(action_normalize_columns, "normalize_columns"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/sort.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/sort.png"));
	action_sort_columns = new QAction(*icon_temp, tr("&Sort Columns"), this);;
	actionManager()->addAction(action_sort_columns, "sort_columns"); 
	delete icon_temp;

	action_statistics_columns = new QAction(QIcon(QPixmap(":/col_stat.xpm")), tr("Statisti&cs"), this);;
	actionManager()->addAction(action_statistics_columns, "statistics_columns"); 

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/column_format_type.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/column_format_type.png"));
	action_type_format = new QAction(*icon_temp, tr("Change &Type && Format"), this);;
	actionManager()->addAction(action_type_format, "type_format"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/column_description.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/column_description.png"));
	action_edit_description = new QAction(*icon_temp, tr("Edit Column &Description"), this);;
	actionManager()->addAction(action_edit_description, "edit_description"); 
	delete icon_temp;

	// row related actions
	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/insert_row.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/insert_row.png"));
	action_insert_rows = new QAction(*icon_temp ,tr("&Insert Empty Rows"), this);;
	actionManager()->addAction(action_insert_rows, "insert_rows"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/remove_row.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/remove_row.png"));
	action_remove_rows = new QAction(*icon_temp, tr("Remo&ve Rows"), this);;
	actionManager()->addAction(action_remove_rows, "remove_rows"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/clear_row.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/clear_row.png"));
	action_clear_rows = new QAction(*icon_temp, tr("Clea&r Rows"), this);;
	actionManager()->addAction(action_clear_rows, "clear_rows"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/add_rows.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/add_rows.png"));
	action_add_rows = new QAction(*icon_temp, tr("&Add Rows"), this);;
	actionManager()->addAction(action_add_rows, "add_rows"); 
	delete icon_temp;

	action_statistics_rows = new QAction(QIcon(QPixmap(":/stat_rows.xpm")), tr("Statisti&cs"), this);;
	actionManager()->addAction(action_statistics_rows, "statistics_rows"); 
}

void Table::connectActions()
{
	connect(action_cut_selection, SIGNAL(triggered()), this, SLOT(cutSelection()));
	d_view->addAction(action_cut_selection);
	connect(action_copy_selection, SIGNAL(triggered()), this, SLOT(copySelection()));
	d_view->addAction(action_copy_selection);
	connect(action_paste_into_selection, SIGNAL(triggered()), this, SLOT(pasteIntoSelection()));
	d_view->addAction(action_paste_into_selection);
	connect(action_mask_selection, SIGNAL(triggered()), this, SLOT(maskSelection()));
	d_view->addAction(action_mask_selection);
	connect(action_unmask_selection, SIGNAL(triggered()), this, SLOT(unmaskSelection()));
	d_view->addAction(action_unmask_selection);
	connect(action_set_formula, SIGNAL(triggered()), this, SLOT(setFormulaForSelection()));
	d_view->addAction(action_set_formula);
	connect(action_clear_selection, SIGNAL(triggered()), this, SLOT(clearSelectedCells()));
	d_view->addAction(action_clear_selection);
	connect(action_recalculate, SIGNAL(triggered()), this, SLOT(recalculateSelectedCells()));
	d_view->addAction(action_recalculate);
	connect(action_fill_row_numbers, SIGNAL(triggered()), this, SLOT(fillSelectedCellsWithRowNumbers()));
	d_view->addAction(action_fill_row_numbers);
	connect(action_fill_random, SIGNAL(triggered()), this, SLOT(fillSelectedCellsWithRandomNumbers()));
	d_view->addAction(action_fill_random);
	connect(action_toggle_comments, SIGNAL(triggered()), d_view, SLOT(toggleComments()));
	d_view->addAction(action_toggle_comments);
	connect(action_toggle_tabbar, SIGNAL(triggered()), d_view, SLOT(toggleControlTabBar()));
	d_view->addAction(action_toggle_tabbar);
	connect(action_select_all, SIGNAL(triggered()), this, SLOT(selectAll()));
	d_view->addAction(action_select_all);
	connect(action_add_column, SIGNAL(triggered()), this, SLOT(addColumn()));
	d_view->addAction(action_add_column);
	connect(action_clear_table, SIGNAL(triggered()), this, SLOT(clear()));
	d_view->addAction(action_clear_table);
	connect(action_clear_masks, SIGNAL(triggered()), this, SLOT(clearMasks()));
	d_view->addAction(action_clear_masks);
	connect(action_sort_table, SIGNAL(triggered()), this, SLOT(sortTable()));
	d_view->addAction(action_sort_table);
	connect(action_go_to_cell, SIGNAL(triggered()), this, SLOT(goToCell()));
	d_view->addAction(action_go_to_cell);
	connect(action_dimensions_dialog, SIGNAL(triggered()), this, SLOT(dimensionsDialog()));
	d_view->addAction(action_dimensions_dialog);
	connect(action_insert_columns, SIGNAL(triggered()), this, SLOT(insertEmptyColumns()));
	d_view->addAction(action_insert_columns);
	connect(action_remove_columns, SIGNAL(triggered()), this, SLOT(removeSelectedColumns()));
	d_view->addAction(action_remove_columns);
	connect(action_clear_columns, SIGNAL(triggered()), this, SLOT(clearSelectedColumns()));
	d_view->addAction(action_clear_columns);
	connect(action_add_columns, SIGNAL(triggered()), this, SLOT(addColumns()));
	d_view->addAction(action_add_columns);
	connect(action_set_as_x, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsX()));
	d_view->addAction(action_set_as_x);
	connect(action_set_as_y, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsY()));
	d_view->addAction(action_set_as_y);
	connect(action_set_as_z, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsZ()));
	d_view->addAction(action_set_as_z);
	connect(action_set_as_xerr, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsXError()));
	d_view->addAction(action_set_as_xerr);
	connect(action_set_as_yerr, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsYError()));
	d_view->addAction(action_set_as_yerr);
	connect(action_set_as_none, SIGNAL(triggered()), this, SLOT(setSelectedColumnsAsNone()));
	d_view->addAction(action_set_as_none);
	connect(action_normalize_columns, SIGNAL(triggered()), this, SLOT(normalizeSelectedColumns()));
	d_view->addAction(action_normalize_columns);
	connect(action_sort_columns, SIGNAL(triggered()), this, SLOT(sortSelectedColumns()));
	d_view->addAction(action_sort_columns);
	connect(action_statistics_columns, SIGNAL(triggered()), this, SLOT(statisticsOnSelectedColumns()));
	d_view->addAction(action_statistics_columns);
	connect(action_type_format, SIGNAL(triggered()), this, SLOT(editTypeAndFormatOfSelectedColumns()));
	d_view->addAction(action_type_format);
	connect(action_edit_description, SIGNAL(triggered()), this, SLOT(editDescriptionOfCurrentColumn()));
	d_view->addAction(action_edit_description);
	connect(action_insert_rows, SIGNAL(triggered()), this, SLOT(insertEmptyRows()));
	d_view->addAction(action_insert_rows);
	connect(action_remove_rows, SIGNAL(triggered()), this, SLOT(removeSelectedRows()));
	d_view->addAction(action_remove_rows);
	connect(action_clear_rows, SIGNAL(triggered()), this, SLOT(clearSelectedRows()));
	d_view->addAction(action_clear_rows);
	connect(action_add_rows, SIGNAL(triggered()), this, SLOT(addRows()));
	d_view->addAction(action_add_rows);
	connect(action_statistics_rows, SIGNAL(triggered()), this, SLOT(statisticsOnSelectedRows()));
	d_view->addAction(action_statistics_rows);
}

void Table::showTableViewContextMenu(const QPoint& pos)
{
	QMenu context_menu;
	
// TODO: Does undo/redo really be belong into a context menu?
//	context_menu.addAction(undoAction(&context_menu));
//	context_menu.addAction(redoAction(&context_menu));

	createSelectionMenu(&context_menu);
	context_menu.addSeparator();
	createTableMenu(&context_menu);
	context_menu.addSeparator();

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

	context_menu.exec(pos);
}

void Table::showTableViewColumnContextMenu(const QPoint& pos)
{
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
	QMenu context_menu;
	
// TODO: Does undo/redo really be belong into a context menu?
//	context_menu.addAction(undoAction(&context_menu));
//	context_menu.addAction(redoAction(&context_menu));

	createRowMenu(&context_menu);

	context_menu.exec(pos);
}

QMenu * Table::createSelectionMenu(QMenu * append_to)
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addAction(action_cut_selection);
	menu->addAction(action_copy_selection);
	menu->addAction(action_paste_into_selection);
	menu->addAction(action_clear_selection);
	menu->addSeparator();
	menu->addAction(action_mask_selection);
	menu->addAction(action_unmask_selection);
	menu->addSeparator();
	menu->addAction(action_set_formula);
	menu->addAction(action_recalculate);
	menu->addSeparator();

	QMenu * submenu = new QMenu("Fi&ll with");
	submenu->addAction(action_fill_row_numbers);
	submenu->addAction(action_fill_random);
	menu->addMenu(submenu);

	return menu;
}


QMenu * Table::createColumnMenu(QMenu * append_to)
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addAction(action_insert_columns);
	menu->addAction(action_remove_columns);
	menu->addAction(action_clear_columns);
	menu->addAction(action_add_columns);
	menu->addSeparator();
	
	QMenu * submenu = new QMenu("S&et As");

	submenu->addAction(action_set_as_x);
	submenu->addAction(action_set_as_y);
	submenu->addAction(action_set_as_z);
	submenu->addAction(action_set_as_xerr);
	submenu->addAction(action_set_as_yerr);
	submenu->addAction(action_set_as_none);
	menu->addMenu(submenu);
	menu->addSeparator();
	menu->addAction(action_edit_description);
	menu->addAction(action_type_format);
	menu->addSeparator();

	menu->addAction(action_normalize_columns);
	menu->addAction(action_sort_columns);
	menu->addSeparator();
	menu->addAction(action_statistics_columns);

	return menu;
}

QMenu * Table::createTableMenu(QMenu * append_to) 
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addAction(action_toggle_comments);
	menu->addAction(action_toggle_tabbar);
	menu->addSeparator();
	menu->addAction(action_select_all);
	menu->addAction(action_clear_table);
	menu->addAction(action_clear_masks);
	menu->addAction(action_sort_table);
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
	menu->addAction(action_statistics_rows);

	return menu;
}

void Table::goToCell()
{
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
	bool ok;

	int cols = QInputDialog::getInteger(0, tr("Set Table Dimensions"), tr("Enter number of columns"),
			columnCount(), 1, 1e9, 1, &ok);
	if ( !ok ) return;

	int rows = QInputDialog::getInteger(0, tr("Set Table Dimensions"), tr("Enter number of rows"),
			rowCount(), 1, 1e9, 1, &ok);
	if ( !ok ) return;
	
	setColumnCount(cols);
	setRowCount(rows);
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
	Q_UNUSED(other);
	// TODO
	QMessageBox::information(0, "info", "not yet implemented");
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
	// look to the right first
	for(int i=col+1; i<cols; i++)
	{
		if (column(i)->plotDesignation() == SciDAVis::Y)
			return i;
	}
	for(int i=col-1; i>=0; i--)
	{
		if (column(i)->plotDesignation() == SciDAVis::Y)
			return i;
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

	SortDialog *sortd = new SortDialog();
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
		emit dataChanged(0, index, col->rowCount()-1, index);	
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
	}
	d_column_count += count;
	updateHorizontalHeader(before, before+count-1);
	emit d_owner->columnsInserted(before, cols.count());
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
	}
	d_column_count -= count;
	updateHorizontalHeader(first, d_column_count);
	emit d_owner->columnsRemoved(first, count);
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
	updateHorizontalHeader(from, to);
	emit d_owner->dataChanged(0, from, d_row_count-1, from);
	emit d_owner->dataChanged(0, to, d_row_count-1, to);
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
		d_vertical_header_data.replace(i, QString::number(i+1));
	for(; i<d_row_count; i++)
		d_vertical_header_data << QString::number(i+1);
	emit d_owner->headerDataChanged(Qt::Vertical, start_row, d_row_count -1);	
}

void Table::Private::updateHorizontalHeader(int start_col, int end_col)
{
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
		case Qt::Vertical:
			switch(role) {
				case Qt::DisplayRole:
				case Qt::ToolTipRole:
					return d_vertical_header_data.at(section);
			}
	}
	return QVariant();
}

