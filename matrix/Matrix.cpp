/***************************************************************************
    File                 : Matrix.cpp
    Project              : SciDAVis
    Description          : Aspect providing a spreadsheet to manage MxN matrix data
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
#include "Matrix.h"
#include "core/AbstractScript.h"
#include "matrixcommands.h"

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
}

Matrix::~Matrix()
{
}

QWidget *Matrix::view()
{
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
	int col_diff = columnCount() - cols;
	int row_diff = rowCount() - rows;
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

double Matrix::cell(int row, int col)
{
	return d_matrix_private->cell(row, col);
}

void Matrix::cutSelection()
{
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
					setCell(first_row + c, first_col + c, cell_texts.at(r).at(c).toDouble());
				}
			}
		}
	}
	endMacro();
	RESET_CURSOR;
}

void Matrix::insertEmptyColumns()
{
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

void Matrix::createActions()
{
	//TODO
}

void Matrix::showMatrixViewContextMenu(const QPoint& pos)
{
	QMenu context_menu;
	
	context_menu.addAction(undoAction(&context_menu));
	context_menu.addAction(redoAction(&context_menu));

	//TODO
}

void Matrix::showMatrixViewColumnContextMenu(const QPoint& pos)
{
	QMenu context_menu;
	
	context_menu.addAction(undoAction(&context_menu));
	context_menu.addAction(redoAction(&context_menu));

	//TODO
}

void Matrix::showMatrixViewRowContextMenu(const QPoint& pos)
{
	QMenu context_menu;
	
	context_menu.addAction(undoAction(&context_menu));
	context_menu.addAction(redoAction(&context_menu));

	//TODO
}

// This probably needs to be moved to the main window
QMenu * Matrix::createApplicationWindowMenu()
{
	QMenu * menu = new QMenu(tr("Matrix"));

	//TODO
	// [Context Menu]
	// Show/Hide Matrix Controls (Dock)
	// Set Dimensions
	// Convert to Table
	return menu;
}

void Matrix::goToCell()
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
	return QString::number(cell(row,col), d_matrix_private->numericFormat(), d_matrix_private->displayedDigits());
}

void Matrix::selectAll()
{
	d_view->selectAll();
}


/* ========================== Matrix::Private ====================== */

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
			d_data.at(col).insert(before+i, 0.0);

	d_row_count += count;
	emit d_owner->rowsInserted(before, count);
}

void Matrix::Private::removeRows(int first, int count)
{
	emit d_owner->rowsAboutToBeRemoved(first, count);
	Q_ASSERT(first >= 0);
	Q_ASSERT(first+count <= d_row_count);
	for(int col=0; col<d_column_count; col++)
		d_data.at(col).remove(first, count);

	d_row_count -= count;
	emit d_owner->rowsRemoved(first, count);
}

double Matrix::Private::cell(int row, int col)
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

