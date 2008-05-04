/***************************************************************************
    File                 : Matrix.cpp
    Project              : SciDAVis
    Description          : Aspect providing a spreadsheet to manage MxN matrix data
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2008 Tilman Benkert (thzs*gmx.net)
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
#include "Matrix.h"
#include "core/AbstractScript.h"
#include "matrixcommands.h"
#include "lib/ActionManager.h"
#include "lib/XmlStreamReader.h"

#include <QtGlobal>
#include <QTextStream>
#include <QList>
#include <QEvent>
#include <QContextMenuEvent>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QApplication>
#include <QMessageBox>
#include <QClipboard>
#include <QShortcut>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QLocale>
#include <QMenu>
#include <QInputDialog>
#include <QXmlStreamWriter>
#include <QDateTime>
#include <QDebug>

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_math.h>


#define WAIT_CURSOR QApplication::setOverrideCursor(QCursor(Qt::WaitCursor))
#define RESET_CURSOR QApplication::restoreOverrideCursor()

Matrix::Matrix(AbstractScriptingEngine *engine, int rows, int cols, const QString& name)
	: AbstractPart(name), d_plot_menu(0), scripted(engine)
{
	d_matrix_private = new Private(this);

	// set initial number of rows and columns
	appendColumns(cols);
	appendRows(rows);

	d_view = new MatrixView(this); 
	createActions();
	connectActions();
}

Matrix::Matrix()
	: AbstractPart("temp"), scripted(0)
{
	createActions();
}

Matrix::~Matrix()
{
}

QWidget *Matrix::view()
{
	if (!d_view)
	{
		d_view = new MatrixView(this); 
		addActionsToView();
	}
	return d_view;
}

void Matrix::insertColumns(int before, int count)
{
	if( count < 1 || before < 0 || before > columnCount()) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: insert %2 column(s)").arg(name()).arg(count));
	exec(new MatrixInsertColumnsCmd(d_matrix_private, before, count));
	endMacro();
	RESET_CURSOR;
}

void Matrix::removeColumns(int first, int count)
{
	if( count < 1 || first < 0 || first+count > columnCount()) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: remove %2 column(s)").arg(name()).arg(count));
	exec(new MatrixRemoveColumnsCmd(d_matrix_private, first, count));
	endMacro();
	RESET_CURSOR;
}

void Matrix::removeRows(int first, int count)
{
	if( count < 1 || first < 0 || first+count > rowCount()) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: remove %2 row(s)").arg(name()).arg(count));
	exec(new MatrixRemoveRowsCmd(d_matrix_private, first, count));
	endMacro();
	RESET_CURSOR;
}

void Matrix::insertRows(int before, int count)
{
	if( count < 1 || before < 0 || before > rowCount()) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: insert %2 row(s)").arg(name()).arg(count));
	exec(new MatrixInsertRowsCmd(d_matrix_private, before, count));
	endMacro();
	RESET_CURSOR;
}

void Matrix::setDimensions(int rows, int cols)
{
	if( (rows < 0) || (cols < 0 ) || (rows == rowCount() && cols == columnCount()) ) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: set matrix size to %2x%3").arg(name()).arg(rows).arg(cols));
	int col_diff = cols - columnCount();
	int row_diff = rows - rowCount();
	if(col_diff > 0)
		exec(new MatrixInsertColumnsCmd(d_matrix_private, columnCount(), col_diff));
	else if(col_diff < 0)
		exec(new MatrixRemoveColumnsCmd(d_matrix_private, columnCount()+col_diff, -col_diff));
	if(row_diff > 0)
		exec(new MatrixInsertRowsCmd(d_matrix_private, rowCount(), row_diff));
	else if(row_diff < 0)
		exec(new MatrixRemoveRowsCmd(d_matrix_private, rowCount()+row_diff, -row_diff));
	endMacro();
	RESET_CURSOR;
}

int Matrix::columnCount() const
{
	return d_matrix_private->columnCount();
}

int Matrix::rowCount() const
{
	return d_matrix_private->rowCount();
}

void Matrix::clear()
{
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: clear").arg(name()));
	exec(new MatrixClearCmd(d_matrix_private));
	endMacro();
	RESET_CURSOR;
}

double Matrix::cell(int row, int col) const
{
	if(row < 0 || row >= rowCount() ||
	   col < 0 || col >= columnCount()) return 0.0;
	return d_matrix_private->cell(row, col);
}

void Matrix::cutSelection()
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

void Matrix::copySelection()
{
	if (!d_view) return;
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

void Matrix::pasteIntoSelection()
{
	if (!d_view) return;
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
			// resize the matrix if necessary
			if(last_col >= columnCount())
				appendColumns(last_col+1-columnCount());
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
					setCell(first_row + r, first_col + c, cell_texts.at(r).at(c).toDouble());
				}
			}
		}
	}
	endMacro();
	RESET_CURSOR;
}

void Matrix::insertEmptyColumns()
{
	if (!d_view) return;
	int first = d_view->firstSelectedColumn();
	int last = d_view->lastSelectedColumn();
	if( first < 0 ) return;
	int count, current = first;

	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: insert empty column(s)").arg(name()));
	while( current <= last )
	{
		current = first+1;
		while( current <= last && d_view->isColumnSelected(current) ) current++;
		count = current-first;
		insertColumns(first, count);
		current += count;
		last += count;
		while( current <= last && !d_view->isColumnSelected(current) ) current++;
		first = current;
	}
	endMacro();
	RESET_CURSOR;
}

void Matrix::removeSelectedColumns()
{
	if (!d_view) return;
	int first = d_view->firstSelectedColumn();
	int last = d_view->lastSelectedColumn();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: remove selected column(s)").arg(name()));
	for(int i=last; i>=first; i--)
		if(d_view->isColumnSelected(i, false)) removeColumns(i, 1);
	endMacro();
	RESET_CURSOR;
}

void Matrix::clearSelectedColumns()
{
	if (!d_view) return;
	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: clear selected column(s)").arg(name()));
	for(int i=0; i<columnCount(); i++)
		if(d_view->isColumnSelected(i, false))
			exec(new MatrixClearColumnCmd(d_matrix_private, i));
	endMacro();
	RESET_CURSOR;
}

void Matrix::insertEmptyRows()
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

void Matrix::removeSelectedRows()
{
	if (!d_view) return;
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

void Matrix::clearSelectedRows()
{
	if (!d_view) return;
	int first = d_view->firstSelectedRow();
	int last = d_view->lastSelectedRow();
	if( first < 0 ) return;

	WAIT_CURSOR;
	beginMacro(QObject::tr("%1: clear selected rows(s)").arg(name()));
	for(int i=first; i<=last; i++)
	{
		if(d_view->isRowSelected(i))
			for(int j=0; j<columnCount(); j++)
				exec(new MatrixSetCellValueCmd(d_matrix_private, i, j, 0.0));
	}
	endMacro();
	RESET_CURSOR;
}

void Matrix::clearSelectedCells()
{
	if (!d_view) return;
	int first_row = d_view->firstSelectedRow();
	int last_row = d_view->lastSelectedRow();
	if( first_row < 0 ) return;
	int first_col = d_view->firstSelectedColumn();
	int last_col = d_view->lastSelectedColumn();
	if( first_col < 0 ) return;

	WAIT_CURSOR;
	beginMacro(tr("%1: clear selected cell(s)").arg(name()));
	for(int i=first_row; i<=last_row; i++)
		for(int j=first_col; j<=last_col; j++)
			if(d_view->isCellSelected(i, j))
				exec(new MatrixSetCellValueCmd(d_matrix_private, i, j, 0.0));
	endMacro();
	RESET_CURSOR;
}

QMenu *Matrix::createContextMenu() const
{
	QMenu *menu = AbstractPart::createContextMenu();
	Q_ASSERT(menu);
	menu->addSeparator();
	
	new QAction(tr("E&xport to ASCII"), menu);
	// TODO menu->addAction( ....

	return menu;
}

QMenu * Matrix::createSelectionMenu(QMenu * append_to)
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addAction(action_cut_selection);
	menu->addAction(action_copy_selection);
	menu->addAction(action_paste_into_selection);
	menu->addAction(action_clear_selection);

	return menu;
}


QMenu * Matrix::createColumnMenu(QMenu * append_to)
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addAction(action_insert_columns);
	menu->addAction(action_remove_columns);
	menu->addAction(action_clear_columns);
	menu->addSeparator();
	menu->addAction(action_edit_coordinates);
	
	return menu;
}

QMenu * Matrix::createMatrixMenu(QMenu * append_to) 
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addAction(action_toggle_tabbar);
	menu->addSeparator();
	menu->addAction(action_select_all);
	menu->addAction(action_clear_matrix);
	menu->addSeparator();
	menu->addAction(action_set_formula);
	menu->addAction(action_recalculate);
	menu->addSeparator();
	menu->addAction(action_edit_format);
	menu->addSeparator();
	menu->addAction(action_go_to_cell);

	return menu;
}

QMenu * Matrix::createRowMenu(QMenu * append_to) 
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addAction(action_insert_rows);
	menu->addAction(action_remove_rows);
	menu->addAction(action_clear_rows);
	menu->addSeparator();
	menu->addAction(action_edit_coordinates);

	return menu;
}


void Matrix::createActions()
{
	QIcon * icon_temp;

	// selection related actions
	action_cut_selection = new QAction(QIcon(QPixmap(":/cut.xpm")), tr("Cu&t"), this);
	actionManager()->addAction(action_cut_selection, "cut_selection");

	action_copy_selection = new QAction(QIcon(QPixmap(":/copy.xpm")), tr("&Copy"), this);
	actionManager()->addAction(action_copy_selection, "copy_selection");

	action_paste_into_selection = new QAction(QIcon(QPixmap(":/paste.xpm")), tr("Past&e"), this);
	actionManager()->addAction(action_paste_into_selection, "paste_into_selection"); 

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/clear.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/clear.png"));
	action_clear_selection = new QAction(*icon_temp, tr("Clea&r","clear selection"), this);
	actionManager()->addAction(action_clear_selection, "clear_selection"); 
	delete icon_temp;

	// matrix related actions
	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/fx.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/fx.png"));
	action_set_formula = new QAction(*icon_temp, tr("Assign &Formula"), this);
	actionManager()->addAction(action_set_formula, "set_formula"); 
	delete icon_temp;
	
	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/recalculate.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/recalculate.png"));
	action_recalculate = new QAction(*icon_temp, tr("Recalculate"), this);
	actionManager()->addAction(action_recalculate, "recalculate"); 
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
	icon_temp->addPixmap(QPixmap(":/16x16/clear_table.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/clear_table.png"));
	action_clear_matrix = new QAction(*icon_temp, tr("Clear Matrix"), this);
	actionManager()->addAction(action_clear_matrix, "clear_matrix"); 
	delete icon_temp;

	icon_temp = new QIcon();
	icon_temp->addPixmap(QPixmap(":/16x16/go_to_cell.png"));
	icon_temp->addPixmap(QPixmap(":/32x32/go_to_cell.png"));
	action_go_to_cell = new QAction(*icon_temp, tr("&Go to Cell"), this);
	actionManager()->addAction(action_go_to_cell, "go_to_cell"); 
	delete icon_temp;

	action_dimensions_dialog = new QAction(QIcon(QPixmap(":/resize.xpm")), tr("&Dimensions", "matrix size"), this);
	actionManager()->addAction(action_dimensions_dialog, "dimensions_dialog"); 
	
	action_edit_coordinates = new QAction(tr("Set &Coordinates"), this);
	actionManager()->addAction(action_edit_coordinates, "edit_coordinates"); 
	
	action_edit_format = new QAction(tr("Set Display &Format"), this);
	actionManager()->addAction(action_edit_format, "edit_format"); 

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
}

void Matrix::connectActions()
{
	connect(action_cut_selection, SIGNAL(triggered()), this, SLOT(cutSelection()));
	connect(action_copy_selection, SIGNAL(triggered()), this, SLOT(copySelection()));
	connect(action_paste_into_selection, SIGNAL(triggered()), this, SLOT(pasteIntoSelection()));
	connect(action_set_formula, SIGNAL(triggered()), this, SLOT(editFormula()));
	connect(action_edit_coordinates, SIGNAL(triggered()), this, SLOT(editCoordinates()));
	connect(action_edit_format, SIGNAL(triggered()), this, SLOT(editFormat()));
	connect(action_clear_selection, SIGNAL(triggered()), this, SLOT(clearSelectedCells()));
// TODO: Formula support
//	connect(action_recalculate, SIGNAL(triggered()), this, SLOT(recalculate()));
	connect(action_select_all, SIGNAL(triggered()), this, SLOT(selectAll()));
	connect(action_clear_matrix, SIGNAL(triggered()), this, SLOT(clear()));
	connect(action_go_to_cell, SIGNAL(triggered()), this, SLOT(goToCell()));
	connect(action_dimensions_dialog, SIGNAL(triggered()), this, SLOT(dimensionsDialog()));
	connect(action_insert_columns, SIGNAL(triggered()), this, SLOT(insertEmptyColumns()));
	connect(action_remove_columns, SIGNAL(triggered()), this, SLOT(removeSelectedColumns()));
	connect(action_clear_columns, SIGNAL(triggered()), this, SLOT(clearSelectedColumns()));
	connect(action_insert_rows, SIGNAL(triggered()), this, SLOT(insertEmptyRows()));
	connect(action_remove_rows, SIGNAL(triggered()), this, SLOT(removeSelectedRows()));
	connect(action_clear_rows, SIGNAL(triggered()), this, SLOT(clearSelectedRows()));
	connect(action_add_columns, SIGNAL(triggered()), this, SLOT(addColumns()));
	connect(action_add_rows, SIGNAL(triggered()), this, SLOT(addRows()));
}

void Matrix::addActionsToView()
{
	connect(action_toggle_tabbar, SIGNAL(triggered()), d_view, SLOT(toggleControlTabBar()));

	d_view->addAction(action_cut_selection);
	d_view->addAction(action_copy_selection);
	d_view->addAction(action_paste_into_selection);
	d_view->addAction(action_set_formula);
	d_view->addAction(action_edit_coordinates);
	d_view->addAction(action_edit_format);
	d_view->addAction(action_clear_selection);
	// TODO: Formula support
	//		d_view->addAction(action_recalculate);
	d_view->addAction(action_toggle_tabbar);
	d_view->addAction(action_select_all);
	d_view->addAction(action_clear_matrix);
	d_view->addAction(action_go_to_cell);
	d_view->addAction(action_dimensions_dialog);
	d_view->addAction(action_insert_columns);
	d_view->addAction(action_remove_columns);
	d_view->addAction(action_clear_columns);
	d_view->addAction(action_insert_rows);
	d_view->addAction(action_remove_rows);
	d_view->addAction(action_clear_rows);
	d_view->addAction(action_add_columns);
	d_view->addAction(action_add_rows);
}


bool Matrix::fillProjectMenu(QMenu * menu)
{
	menu->setTitle(tr("&Matrix"));

	menu->addAction(action_toggle_tabbar);
	menu->addSeparator();
	menu->addAction(action_edit_coordinates);
	menu->addAction(action_dimensions_dialog);
	menu->addAction(action_edit_format);
	menu->addSeparator();
	menu->addAction(action_set_formula);
	menu->addAction(action_recalculate);
	menu->addSeparator();
	menu->addAction(action_clear_matrix);
	menu->addSeparator();
	menu->addAction(action_go_to_cell);

	return true;

	// TODO:
	// Convert to Table
	// Export 
}

void Matrix::showMatrixViewContextMenu(const QPoint& pos)
{
	if (!d_view) return;
	QMenu context_menu;
	
	createSelectionMenu(&context_menu);
	context_menu.addSeparator();
	createMatrixMenu(&context_menu);
	context_menu.addSeparator();

	QString action_name;
	if(d_view->isControlTabBarVisible()) 
		action_name = tr("Hide Controls");
	else
		action_name = tr("Show Controls");
	action_toggle_tabbar->setText(action_name);

	context_menu.exec(pos);
}

void Matrix::showMatrixViewColumnContextMenu(const QPoint& pos)
{
	QMenu context_menu;
	
	createColumnMenu(&context_menu);

	context_menu.exec(pos);
}

void Matrix::showMatrixViewRowContextMenu(const QPoint& pos)
{
	QMenu context_menu;
	
	createRowMenu(&context_menu);

	context_menu.exec(pos);
}

void Matrix::goToCell()
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

void Matrix::copy(Matrix * other)
{
	Q_UNUSED(other);
	// TODO
	QMessageBox::information(0, "info", "not yet implemented");
}

void Matrix::setPlotMenu(QMenu * menu)
{
	d_plot_menu = menu;
}

QIcon Matrix::icon() const
{
	QIcon ico;
	ico.addPixmap(QPixmap(":/16x16/matrix.png"));
	ico.addPixmap(QPixmap(":/24x24/matrix.png"));
	ico.addPixmap(QPixmap(":/32x32/matrix.png"));
	return ico;
}

QString Matrix::text(int row, int col)
{
	return QLocale().toString(cell(row,col), d_matrix_private->numericFormat(), d_matrix_private->displayedDigits());
}

void Matrix::selectAll()
{
	if (!d_view) return;
	d_view->selectAll();
}

void Matrix::setCell(int row, int col, double value)
{
	if(row < 0 || row >= rowCount()) return;
	if(col < 0 || col >= columnCount()) return;
	exec(new MatrixSetCellValueCmd(d_matrix_private, row, col, value));
}

void Matrix::dimensionsDialog()
{
	bool ok;

	int cols = QInputDialog::getInteger(0, tr("Set Matrix Dimensions"), tr("Enter number of columns"),
			columnCount(), 1, 1e9, 1, &ok);
	if ( !ok ) return;

	int rows = QInputDialog::getInteger(0, tr("Set Matrix Dimensions"), tr("Enter number of rows"),
			rowCount(), 1, 1e9, 1, &ok);
	if ( !ok ) return;
	
	setDimensions(rows, cols);
}

void Matrix::editFormat()
{
	if (!d_view) return;
	d_view->showControlFormatTab();
}

void Matrix::editCoordinates()
{
	if (!d_view) return;
	d_view->showControlCoordinatesTab();
}

void Matrix::editFormula()
{
	if (!d_view) return;
	d_view->showControlFormulaTab();
}

void Matrix::addRows()
{
	if (!d_view) return;
	WAIT_CURSOR;
	int count = d_view->selectedRowCount(false);
	beginMacro(QObject::tr("%1: add %2 rows(s)").arg(name()).arg(count));
	exec(new MatrixInsertRowsCmd(d_matrix_private, rowCount(), count));
	endMacro();
	RESET_CURSOR;
}

void Matrix::addColumns()
{
	if (!d_view) return;
	WAIT_CURSOR;
	int count = d_view->selectedRowCount(false);
	beginMacro(QObject::tr("%1: add %2 column(s)").arg(name()).arg(count));
	exec(new MatrixInsertColumnsCmd(d_matrix_private, columnCount(), count));
	endMacro();
	RESET_CURSOR;
}

void Matrix::setXStart(double x)
{
	WAIT_CURSOR;
	exec(new MatrixSetCoordinatesCmd(d_matrix_private, x, xEnd(), yStart(), yEnd()));
	RESET_CURSOR;
}

void Matrix::setXEnd(double x)
{
	WAIT_CURSOR;
	exec(new MatrixSetCoordinatesCmd(d_matrix_private, xStart(), x, yStart(), yEnd()));
	RESET_CURSOR;
}

void Matrix::setYStart(double y)
{
	WAIT_CURSOR;
	exec(new MatrixSetCoordinatesCmd(d_matrix_private, xStart(), xEnd(), y, yEnd()));
	RESET_CURSOR;
}

void Matrix::setYEnd(double y)
{
	WAIT_CURSOR;
	exec(new MatrixSetCoordinatesCmd(d_matrix_private, xStart(), xEnd(), yStart(), y));
	RESET_CURSOR;
}

void Matrix::setCoordinates(double x1, double x2, double y1, double y2)
{
	WAIT_CURSOR;
	exec(new MatrixSetCoordinatesCmd(d_matrix_private, x1, x2, y1, y2));
	RESET_CURSOR;
}

void Matrix::setNumericFormat(char format)
{
	WAIT_CURSOR;
	exec(new MatrixSetFormatCmd(d_matrix_private, format));
	RESET_CURSOR;
}

void Matrix::setDisplayedDigits(int digits)
{
	WAIT_CURSOR;
	exec(new MatrixSetDigitsCmd(d_matrix_private, digits));
	RESET_CURSOR;
}

double Matrix::xStart() const
{ 
	return d_matrix_private->xStart(); 
}

double Matrix::yStart() const
{ 
	return d_matrix_private->yStart(); 
}

double Matrix::xEnd() const
{ 
	return d_matrix_private->xEnd(); 
}

double Matrix::yEnd() const
{ 
	return d_matrix_private->yEnd(); 
}

QString Matrix::formula() const
{ 
	return d_matrix_private->formula(); 
}

void Matrix::setFormula(const QString & formula)
{
//	TODO
}

char Matrix::numericFormat() const 
{ 
	return d_matrix_private->numericFormat(); 
}

int Matrix::displayedDigits() const 
{ 
	return d_matrix_private->displayedDigits(); 
}

void Matrix::save(QXmlStreamWriter * writer) const
{
	int cols = columnCount();
	int rows = rowCount();
	writer->writeStartElement("matrix");
	writeBasicAttributes(writer);
	writer->writeAttribute("columns", QString::number(cols));
	writer->writeAttribute("rows", QString::number(rows));
	writeCommentElement(writer);
	writer->writeStartElement("formula");
	writer->writeCharacters(formula());
	writer->writeEndElement();
	writer->writeStartElement("display");
	writer->writeAttribute("numeric_format", QString(QChar(numericFormat())));
	writer->writeAttribute("displayed_digits", QString::number(displayedDigits()));
	writer->writeEndElement();
	writer->writeStartElement("coordinates");
	writer->writeAttribute("x_start", QString::number(xStart()));
	writer->writeAttribute("x_end", QString::number(xEnd()));
	writer->writeAttribute("y_start", QString::number(yStart()));
	writer->writeAttribute("y_end", QString::number(yEnd()));
	writer->writeEndElement();

	for (int col=0; col<cols; col++)
		for (int row=0; row<rows; row++)
		{
			writer->writeStartElement("cell");
			writer->writeAttribute("row", QString::number(row));
			writer->writeAttribute("column", QString::number(col));
			writer->writeCharacters(QString::number(cell(row, col), 'e', 16));
			writer->writeEndElement();
		}
	writer->writeEndElement(); // "matrix"
}


bool Matrix::load(XmlStreamReader * reader)
{
	if(reader->isStartElement() && reader->name() == "matrix") 
	{
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
		setDimensions(rows, cols);

		// read child elements
		while (!reader->atEnd()) 
		{
			reader->readNext();

			if (reader->isEndElement()) break;

			if (reader->isStartElement()) 
			{
				bool ret_val = true;
				if (reader->name() == "comment")
					ret_val = readCommentElement(reader);
				else if(reader->name() == "formula")
					ret_val = XmlReadFormula(reader);
				else if(reader->name() == "display")
					ret_val = XmlReadDisplay(reader);
				else if(reader->name() == "coordinates")
					ret_val = XmlReadCoordinates(reader);
				else if(reader->name() == "cell")
					ret_val = XmlReadCell(reader);
				else // unknown element
				{
					reader->raiseWarning(tr("unknown element '%1'").arg(reader->name().toString()));
					if (!reader->skipToEndElement()) return false;
				}
				if(!ret_val) return false;
			} 
		}
	}
	else // no matrix element
		reader->raiseError(tr("no matrix element found"));

	return !reader->hasError();
}

bool Matrix::XmlReadDisplay(XmlStreamReader * reader)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "display");
	QXmlStreamAttributes attribs = reader->attributes();

	QString str = attribs.value(reader->namespaceUri().toString(), "numeric_format").toString();
	if(str.isEmpty() || str.length() != 1)
	{
		reader->raiseError(tr("invalid or missing numeric format"));
		return false;
	}
	setNumericFormat(str.at(0).toAscii());
	
	bool ok;
	int digits = reader->readAttributeInt("displayed_digits", &ok);
	if(!ok)
	{
		reader->raiseError(tr("invalid or missing number of displayed digits"));
		return false;
	}
	setDisplayedDigits(digits);
	if (!reader->skipToEndElement()) return false;

	return true;
}

bool Matrix::XmlReadCoordinates(XmlStreamReader * reader)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "coordinates");

	bool ok;
	int val;

	val = reader->readAttributeInt("x_start", &ok);
	if(!ok)
	{
		reader->raiseError(tr("invalid x start value"));
		return false;
	}
	setXStart(val);

	val = reader->readAttributeInt("x_end", &ok);
	if(!ok)
	{
		reader->raiseError(tr("invalid x end value"));
		return false;
	}
	setXEnd(val);

	val = reader->readAttributeInt("y_start", &ok);
	if(!ok)
	{
		reader->raiseError(tr("invalid y start value"));
		return false;
	}
	setYStart(val);

	val = reader->readAttributeInt("y_end", &ok);
	if(!ok)
	{
		reader->raiseError(tr("invalid y end value"));
		return false;
	}
	setYEnd(val);
	if (!reader->skipToEndElement()) return false;

	return true;
}

bool Matrix::XmlReadFormula(XmlStreamReader * reader)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "formula");
	setFormula(reader->readElementText());
	return true;
}

bool Matrix::XmlReadCell(XmlStreamReader * reader)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "cell");
	
	QString str;
	int row, col;
	bool ok;

	QXmlStreamAttributes attribs = reader->attributes();
	row = reader->readAttributeInt("row", &ok);
	if(!ok)
	{
		reader->raiseError(tr("invalid or missing row index"));
		return false;
	}
	col = reader->readAttributeInt("column", &ok);
	if(!ok)
	{
		reader->raiseError(tr("invalid or missing column index"));
		return false;
	}

	str = reader->readElementText();
	double value = str.toDouble(&ok);
	if(!ok)
	{
		reader->raiseError(tr("invalid cell value"));
		return false;
	}
	setCell(row, col, value);

	return true;
}

/* ========================= static methods ======================= */
ActionManager * Matrix::action_manager = 0;

ActionManager * Matrix::actionManager()
{
	if (!action_manager)
		initActionManager();
	
	return action_manager;
}

void Matrix::initActionManager()
{
	if (!action_manager)
		action_manager = new ActionManager();

	action_manager->setTitle(tr("Matrix"));
	volatile Matrix * action_creator = new Matrix(); // initialize the action texts
	delete action_creator;
}

/* ========================== Matrix::Private ====================== */

Matrix::Private::Private(Matrix *owner) 
	: d_owner(owner), d_column_count(0), d_row_count(0) 
{
	d_numeric_format = 'f';
	d_displayed_digits = 6;
	d_x_start = 0.0;
	d_x_end = 1.0;  
	d_y_start = 0.0; 
	d_y_end = 1.0;
}

void Matrix::Private::insertColumns(int before, int count)
{
	Q_ASSERT(before >= 0);
	Q_ASSERT(before <= d_column_count);

	emit d_owner->columnsAboutToBeInserted(before, count);
	for(int i=0; i<count; i++)
		d_data.insert(before+i, QVector<double>(d_row_count));

	d_column_count += count;
	emit d_owner->columnsInserted(before, count);
}

void Matrix::Private::removeColumns(int first, int count)
{
	emit d_owner->columnsAboutToBeRemoved(first, count);
	Q_ASSERT(first >= 0);
	Q_ASSERT(first+count <= d_column_count);
	d_data.remove(first, count);
	d_column_count -= count;
	emit d_owner->columnsRemoved(first, count);
}

void Matrix::Private::insertRows(int before, int count)
{
	emit d_owner->rowsAboutToBeInserted(before, count);
	Q_ASSERT(before >= 0);
	Q_ASSERT(before <= d_row_count);
	for(int col=0; col<d_column_count; col++)
		for(int i=0; i<count; i++)
			d_data[col].insert(before+i, 0.0);

	d_row_count += count;
	emit d_owner->rowsInserted(before, count);
}

void Matrix::Private::removeRows(int first, int count)
{
	emit d_owner->rowsAboutToBeRemoved(first, count);
	Q_ASSERT(first >= 0);
	Q_ASSERT(first+count <= d_row_count);
	for(int col=0; col<d_column_count; col++)
		d_data[col].remove(first, count);

	d_row_count -= count;
	emit d_owner->rowsRemoved(first, count);
}

double Matrix::Private::cell(int row, int col) const
{
	Q_ASSERT(row >= 0 && row < d_row_count);
	Q_ASSERT(col >= 0 && col < d_column_count);
	return d_data.at(col).at(row);
}

void Matrix::Private::setCell(int row, int col, double value)
{
	Q_ASSERT(row >= 0 && row < d_row_count);
	Q_ASSERT(col >= 0 && col < d_column_count);
	d_data[col][row] = value;
	emit d_owner->dataChanged(row, col, row, col);
}

QVector<double> Matrix::Private::columnCells(int col, int first_row, int last_row)
{
	Q_ASSERT(first_row >= 0 && first_row < d_row_count);
	Q_ASSERT(last_row >= 0 && last_row < d_row_count);

	if(first_row == 0 && last_row == d_row_count-1)
		return d_data.at(col);

	QVector<double> result;
	for(int i=first_row; i<=last_row; i++)
		result.append(d_data.at(col).at(i));
	return result;
}

void Matrix::Private::setColumnCells(int col, int first_row, int last_row, QVector<double> values)
{
	Q_ASSERT(first_row >= 0 && first_row < d_row_count);
	Q_ASSERT(last_row >= 0 && last_row < d_row_count);
	Q_ASSERT(values.count() > last_row - first_row);

	if(first_row == 0 && last_row == d_row_count-1)
	{
		d_data[col] = values;
		d_data[col].resize(d_row_count);  // values may be larger
		return;
	}

	for(int i=first_row; i<=last_row; i++)
		d_data[col][i] = values.at(i-first_row);
	emit d_owner->dataChanged(first_row, col, last_row, col);
}

void Matrix::Private::clearColumn(int col)
{
	d_data[col].fill(0.0);
	emit d_owner->dataChanged(0, col, d_row_count-1, col);
}

