/***************************************************************************
    File                 : matrixcommands.cpp
    Project              : SciDAVis
    Description          : Commands used in Matrix (part of the undo/redo framework)
    --------------------------------------------------------------------
    Copyright            : (C) 2008-2009 Tilman Benkert (thzs*gmx.net)
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

#include "matrixcommands.h"

///////////////////////////////////////////////////////////////////////////
// class MatrixInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////
MatrixInsertColumnsCmd::MatrixInsertColumnsCmd( future::Matrix::Private * private_obj, int before, int count, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj), d_before(before), d_count(count)
{
	setText(QObject::tr("%1: insert %2 column(s)").arg(d_private_obj->name()).arg(d_count));
}

MatrixInsertColumnsCmd::~MatrixInsertColumnsCmd()
{
}

void MatrixInsertColumnsCmd::redo()
{
	d_private_obj->insertColumns(d_before, d_count);
}

void MatrixInsertColumnsCmd::undo()
{
	d_private_obj->removeColumns(d_before, d_count);
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixInsertRowsCmd
///////////////////////////////////////////////////////////////////////////
MatrixInsertRowsCmd::MatrixInsertRowsCmd( future::Matrix::Private * private_obj, int before, int count, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj), d_before(before), d_count(count)
{
	setText(QObject::tr("%1: insert %2 row(s)").arg(d_private_obj->name()).arg(d_count));
}

MatrixInsertRowsCmd::~MatrixInsertRowsCmd()
{
}

void MatrixInsertRowsCmd::redo()
{
	d_private_obj->insertRows(d_before, d_count);
}

void MatrixInsertRowsCmd::undo()
{
	d_private_obj->removeRows(d_before, d_count);
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixInsertRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////
MatrixRemoveColumnsCmd::MatrixRemoveColumnsCmd( future::Matrix::Private * private_obj, int first, int count, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj), d_first(first), d_count(count)
{
	setText(QObject::tr("%1: remove %2 column(s)").arg(d_private_obj->name()).arg(d_count));
}

MatrixRemoveColumnsCmd::~MatrixRemoveColumnsCmd()
{
}

void MatrixRemoveColumnsCmd::redo()
{
	if(d_backups.isEmpty())
	{
		int last_row = d_private_obj->rowCount()-1;
		for(int i=0; i<d_count; i++)
			d_backups.append(d_private_obj->columnCells(d_first+i, 0, last_row));
	}
	d_private_obj->removeColumns(d_first, d_count);
}

void MatrixRemoveColumnsCmd::undo()
{
	d_private_obj->insertColumns(d_first, d_count);
	int last_row = d_private_obj->rowCount()-1;
	for(int i=0; i<d_count; i++)
		d_private_obj->setColumnCells(d_first+i, 0, last_row, d_backups.at(i));
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////
MatrixRemoveRowsCmd::MatrixRemoveRowsCmd( future::Matrix::Private * private_obj, int first, int count, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj), d_first(first), d_count(count)
{
	setText(QObject::tr("%1: remove %2 row(s)").arg(d_private_obj->name()).arg(d_count));
}

MatrixRemoveRowsCmd::~MatrixRemoveRowsCmd()
{
}

void MatrixRemoveRowsCmd::redo()
{
	if(d_backups.isEmpty())
	{
		int last_row = d_first+d_count-1;
		for(int col=0; col<d_private_obj->columnCount(); col++)
			d_backups.append(d_private_obj->columnCells(col, d_first, last_row));
	}
	d_private_obj->removeRows(d_first, d_count);
}

void MatrixRemoveRowsCmd::undo()
{
	d_private_obj->insertRows(d_first, d_count);
	int last_row = d_first+d_count-1;
	for(int col=0; col<d_private_obj->columnCount(); col++)
		d_private_obj->setColumnCells(col, d_first, last_row, d_backups.at(col));
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixClearCmd
///////////////////////////////////////////////////////////////////////////
MatrixClearCmd::MatrixClearCmd( future::Matrix::Private * private_obj, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj)
{
	setText(QObject::tr("%1: clear").arg(d_private_obj->name()));
}

MatrixClearCmd::~MatrixClearCmd()
{
}

void MatrixClearCmd::redo()
{
	if(d_backups.isEmpty())
	{
		int last_row = d_private_obj->rowCount()-1;
		for(int i=0; i<d_private_obj->columnCount(); i++)
			d_backups.append(d_private_obj->columnCells(i, 0, last_row));
	}
	for(int i=0; i<d_private_obj->columnCount(); i++)
		d_private_obj->clearColumn(i);
}

void MatrixClearCmd::undo()
{
	int last_row = d_private_obj->rowCount()-1;
	for(int i=0; i<d_private_obj->columnCount(); i++)
		d_private_obj->setColumnCells(i, 0, last_row, d_backups.at(i));
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixClearCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixClearColumnCmd
///////////////////////////////////////////////////////////////////////////
MatrixClearColumnCmd::MatrixClearColumnCmd( future::Matrix::Private * private_obj, int col, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj), d_col(col)
{
	setText(QObject::tr("%1: clear column %2").arg(d_private_obj->name()).arg(d_col+1));
}

MatrixClearColumnCmd::~MatrixClearColumnCmd()
{
}

void MatrixClearColumnCmd::redo()
{
	if(d_backup.isEmpty())
		d_backup = d_private_obj->columnCells(d_col, 0, d_private_obj->rowCount()-1);
	d_private_obj->clearColumn(d_col);
}

void MatrixClearColumnCmd::undo()
{
	d_private_obj->setColumnCells(d_col, 0, d_private_obj->rowCount()-1, d_backup);
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixClearColumnCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetCellValueCmd
///////////////////////////////////////////////////////////////////////////
MatrixSetCellValueCmd::MatrixSetCellValueCmd( future::Matrix::Private * private_obj, int row, int col, double value, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj), d_row(row), d_col(col), d_value(value)
{
	// remark: don't use many QString::arg() calls in ctors of commands that might be called often,
	// they use a lot of execution time
	setText(QObject::tr("%1: set cell value").arg(d_private_obj->name()));
}

MatrixSetCellValueCmd::~MatrixSetCellValueCmd()
{
}

void MatrixSetCellValueCmd::redo()
{
	d_old_value = d_private_obj->cell(d_row, d_col);
	d_private_obj->setCell(d_row, d_col, d_value);
}

void MatrixSetCellValueCmd::undo()
{
	d_private_obj->setCell(d_row, d_col, d_old_value);
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetCellValueCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetCoordinatesCmd
///////////////////////////////////////////////////////////////////////////
MatrixSetCoordinatesCmd::MatrixSetCoordinatesCmd( future::Matrix::Private * private_obj, double x1, double x2, double y1, double y2, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj), d_new_x1(x1), d_new_x2(x2), d_new_y1(y1), d_new_y2(y2)
{
	setText(QObject::tr("%1: set matrix coordinates").arg(d_private_obj->name()));
}

MatrixSetCoordinatesCmd::~MatrixSetCoordinatesCmd()
{
}

void MatrixSetCoordinatesCmd::redo()
{
	d_old_x1 = d_private_obj->xStart();
	d_old_x2 = d_private_obj->xEnd();
	d_old_y1 = d_private_obj->yStart();
	d_old_y2 = d_private_obj->yEnd();
	d_private_obj->setXStart(d_new_x1);
	d_private_obj->setXEnd(d_new_x2);
	d_private_obj->setYStart(d_new_y1);
	d_private_obj->setYEnd(d_new_y2);
}

void MatrixSetCoordinatesCmd::undo()
{
	d_private_obj->setXStart(d_old_x1);
	d_private_obj->setXEnd(d_old_x2);
	d_private_obj->setYStart(d_old_y1);
	d_private_obj->setYEnd(d_old_y2);
}

///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetCoordinatesCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetFormatCmd
///////////////////////////////////////////////////////////////////////////
MatrixSetFormatCmd::MatrixSetFormatCmd(future::Matrix::Private * private_obj, char new_format)
	: d_private_obj(private_obj), d_other_format(new_format) 
{
	setText(QObject::tr("%1: set numeric format to '%2'").arg(d_private_obj->name()).arg(new_format));
}

void MatrixSetFormatCmd::redo() 
{
	char tmp = d_private_obj->numericFormat();
	d_private_obj->setNumericFormat(d_other_format);
	d_other_format = tmp;
}

void MatrixSetFormatCmd::undo() 
{ 
	redo(); 
}

///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetFormatCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetDigitsCmd
///////////////////////////////////////////////////////////////////////////
MatrixSetDigitsCmd::MatrixSetDigitsCmd(future::Matrix::Private * private_obj, int new_digits)
	: d_private_obj(private_obj), d_other_digits(new_digits) 
{
	setText(QObject::tr("%1: set decimal digits to %2").arg(d_private_obj->name()).arg(new_digits));
}

void MatrixSetDigitsCmd::redo() 
{
	int tmp = d_private_obj->displayedDigits();
	d_private_obj->setDisplayedDigits(d_other_digits);
	d_other_digits = tmp;
}

void MatrixSetDigitsCmd::undo() 
{ 
	redo(); 
}

///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetDigitsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetFormulaCmd
///////////////////////////////////////////////////////////////////////////
MatrixSetFormulaCmd::MatrixSetFormulaCmd(future::Matrix::Private * private_obj, QString formula)
	: d_private_obj(private_obj), d_other_formula(formula) 
{
	setText(QObject::tr("%1: set formula").arg(d_private_obj->name()));
}

void MatrixSetFormulaCmd::redo() 
{
	QString tmp = d_private_obj->formula();
	d_private_obj->setFormula(d_other_formula);
	d_other_formula = tmp;
}

void MatrixSetFormulaCmd::undo() 
{ 
	redo(); 
}

///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetFormulaCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetColumnCellsCmd
///////////////////////////////////////////////////////////////////////////
MatrixSetColumnCellsCmd::MatrixSetColumnCellsCmd( future::Matrix::Private * private_obj, int col, int first_row, 
		int last_row, const QVector<qreal> & values, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj), d_col(col), d_first_row(first_row), 
 		d_last_row(last_row), d_values(values)
{
	setText(QObject::tr("%1: set cell values").arg(d_private_obj->name()));
}

MatrixSetColumnCellsCmd::~MatrixSetColumnCellsCmd()
{
}

void MatrixSetColumnCellsCmd::redo()
{
	if (d_old_values.isEmpty())
		d_old_values = d_private_obj->columnCells(d_col, d_first_row, d_last_row);
	d_private_obj->setColumnCells(d_col, d_first_row, d_last_row, d_values);
}

void MatrixSetColumnCellsCmd::undo()
{
	d_private_obj->setColumnCells(d_col, d_first_row, d_last_row, d_old_values);
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetColumnCellsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetRowCellsCmd
///////////////////////////////////////////////////////////////////////////
MatrixSetRowCellsCmd::MatrixSetRowCellsCmd( future::Matrix::Private * private_obj, int row, int first_column, 
		int last_column, const QVector<qreal> & values, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj), d_row(row), d_first_column(first_column), 
 		d_last_column(last_column), d_values(values)
{
	setText(QObject::tr("%1: set cell values").arg(d_private_obj->name()));
}

MatrixSetRowCellsCmd::~MatrixSetRowCellsCmd()
{
}

void MatrixSetRowCellsCmd::redo()
{
	if (d_old_values.isEmpty())
		d_old_values = d_private_obj->rowCells(d_row, d_first_column, d_last_column);
	d_private_obj->setRowCells(d_row, d_first_column, d_last_column, d_values);
}

void MatrixSetRowCellsCmd::undo()
{
	d_private_obj->setRowCells(d_row, d_first_column, d_last_column, d_old_values);
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixSetRowCellsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixTransposeCmd
///////////////////////////////////////////////////////////////////////////
MatrixTransposeCmd::MatrixTransposeCmd( future::Matrix::Private * private_obj, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj)
{
	setText(QObject::tr("%1: transpose").arg(d_private_obj->name()));
}

MatrixTransposeCmd::~MatrixTransposeCmd()
{
}

void MatrixTransposeCmd::redo()
{
	int rows = d_private_obj->rowCount();
	int cols = d_private_obj->columnCount();
	int temp_size = qMax(rows, cols);
	d_private_obj->blockChangeSignals(true);
	if (cols < rows)
		d_private_obj->insertColumns(cols, temp_size - cols);
	else if (cols > rows)
		d_private_obj->insertRows(rows, temp_size - rows);
	for(int i = 1; i<temp_size; i++)
	{
		QVector<qreal> row = d_private_obj->rowCells(i, 0, i-1);
		QVector<qreal> col = d_private_obj->columnCells(i, 0, i-1);
		d_private_obj->setRowCells(i, 0, i-1, col);
		d_private_obj->setColumnCells(i, 0, i-1, row);
	}
	if (cols < rows)
		d_private_obj->removeRows(cols, temp_size - cols);
	else if (cols > rows)
		d_private_obj->removeColumns(rows, temp_size - rows);
	d_private_obj->blockChangeSignals(false);
	d_private_obj->emitDataChanged(0, 0, d_private_obj->rowCount()-1, d_private_obj->columnCount()-1);
}

void MatrixTransposeCmd::undo()
{
	redo();
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixTransposeCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixMirrorHorizontallyCmd
///////////////////////////////////////////////////////////////////////////
MatrixMirrorHorizontallyCmd::MatrixMirrorHorizontallyCmd( future::Matrix::Private * private_obj, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj)
{
	setText(QObject::tr("%1: mirror horizontally").arg(d_private_obj->name()));
}

MatrixMirrorHorizontallyCmd::~MatrixMirrorHorizontallyCmd()
{
}

void MatrixMirrorHorizontallyCmd::redo()
{
	int rows = d_private_obj->rowCount();
	int cols = d_private_obj->columnCount();
	int middle = cols/2;
	d_private_obj->blockChangeSignals(true);
	for(int i = 0; i<middle; i++)
	{
		QVector<qreal> temp = d_private_obj->columnCells(i, 0, rows-1);
		d_private_obj->setColumnCells(i, 0, rows-1, d_private_obj->columnCells(cols-i-1, 0, rows-1));
		d_private_obj->setColumnCells(cols-i-1, 0, rows-1, temp);
	}
	d_private_obj->blockChangeSignals(false);
	d_private_obj->emitDataChanged(0, 0, rows-1, cols-1);
}

void MatrixMirrorHorizontallyCmd::undo()
{
	redo();
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixMirrorHorizontallyCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixMirrorVerticallyCmd
///////////////////////////////////////////////////////////////////////////
MatrixMirrorVerticallyCmd::MatrixMirrorVerticallyCmd( future::Matrix::Private * private_obj, QUndoCommand * parent)
 : QUndoCommand( parent ), d_private_obj(private_obj)
{
	setText(QObject::tr("%1: mirror vertically").arg(d_private_obj->name()));
}

MatrixMirrorVerticallyCmd::~MatrixMirrorVerticallyCmd()
{
}

void MatrixMirrorVerticallyCmd::redo()
{
	int rows = d_private_obj->rowCount();
	int cols = d_private_obj->columnCount();
	int middle = rows/2;
	d_private_obj->blockChangeSignals(true);
	for(int i = 0; i<middle; i++)
	{
		QVector<qreal> temp = d_private_obj->rowCells(i, 0, cols-1);
		d_private_obj->setRowCells(i, 0, cols-1, d_private_obj->rowCells(rows-i-1, 0, cols-1));
		d_private_obj->setRowCells(rows-i-1, 0, cols-1, temp);
	}
	d_private_obj->blockChangeSignals(false);
	d_private_obj->emitDataChanged(0, 0, rows-1, cols-1);
}

void MatrixMirrorVerticallyCmd::undo()
{
	redo();
}
///////////////////////////////////////////////////////////////////////////
// end of class MatrixMirrorVerticallyCmd
///////////////////////////////////////////////////////////////////////////

