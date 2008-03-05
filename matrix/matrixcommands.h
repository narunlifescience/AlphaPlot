/***************************************************************************
    File                 : matrixcommands.h
    Project              : SciDAVis
    Description          : Commands used in Matrix (part of the undo/redo framework)
    --------------------------------------------------------------------
    Copyright            : (C) 2008 Tilman Hoener zu Siederdissen (thzs*gmx.net)
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

#ifndef MATRIX_COMMANDS_H
#define MATRIX_COMMANDS_H

#include <QUndoCommand>
#include "Matrix.h"

///////////////////////////////////////////////////////////////////////////
// class MatrixInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////
//! Insert columns
class MatrixInsertColumnsCmd : public QUndoCommand
{
public:
	MatrixInsertColumnsCmd( Matrix::Private * private_obj, int before, int count, QUndoCommand * parent = 0 );
	~MatrixInsertColumnsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	Matrix::Private * d_private_obj;
	//! Column to insert before
	int d_before;
	//! The number of new columns
	int d_count;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixInsertRowsCmd
///////////////////////////////////////////////////////////////////////////
//! Insert rows
class MatrixInsertRowsCmd : public QUndoCommand
{
public:
	MatrixInsertRowsCmd( Matrix::Private * private_obj, int before, int count, QUndoCommand * parent = 0 );
	~MatrixInsertRowsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	Matrix::Private * d_private_obj;
	//! Row to insert before
	int d_before;
	//! The number of new rows
	int d_count;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixInsertRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////
//! Remove columns
class MatrixRemoveColumnsCmd : public QUndoCommand
{
public:
	MatrixRemoveColumnsCmd( Matrix::Private * private_obj, int first, int count, QUndoCommand * parent = 0 );
	~MatrixRemoveColumnsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	Matrix::Private * d_private_obj;
	//! First column to remove
	int d_first;
	//! The number of columns to remove
	int d_count;
	//! Backups of the removed columns
	QVector< QVector<double> > d_backups;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////
//! Remove rows
class MatrixRemoveRowsCmd : public QUndoCommand
{
public:
	MatrixRemoveRowsCmd( Matrix::Private * private_obj, int first, int count, QUndoCommand * parent = 0 );
	~MatrixRemoveRowsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	Matrix::Private * d_private_obj;
	//! First row to remove
	int d_first;
	//! The number of rows to remove
	int d_count;
	//! Backups of the removed rows
	QVector< QVector<double> > d_backups;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixClearCmd
///////////////////////////////////////////////////////////////////////////
//! Clear matrix
class MatrixClearCmd : public QUndoCommand
{
public:
	MatrixClearCmd( Matrix::Private * private_obj, QUndoCommand * parent = 0 );
	~MatrixClearCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	Matrix::Private * d_private_obj;
	//! Backups of the cleared cells
	QVector< QVector<double> > d_backups;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixClearCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixClearColumnCmd
///////////////////////////////////////////////////////////////////////////
//! Clear matrix column
class MatrixClearColumnCmd : public QUndoCommand
{
public:
	MatrixClearColumnCmd( Matrix::Private * private_obj, int col, QUndoCommand * parent = 0 );
	~MatrixClearColumnCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	Matrix::Private * d_private_obj;
	//! The index of the column
	int d_col;
	//! Backup of the cleared column
	QVector<double> d_backup;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixClearColumnCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class MatrixSetCellValueCmd
///////////////////////////////////////////////////////////////////////////
//! Set cell value
class MatrixSetCellValueCmd : public QUndoCommand
{
public:
	MatrixSetCellValueCmd( Matrix::Private * private_obj, int row, int col, double value, QUndoCommand * parent = 0 );
	~MatrixSetCellValueCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The private object to modify
	Matrix::Private * d_private_obj;
	//! The index of the row
	int d_row;
	//! The index of the column
	int d_col;
	//! New cell value
	double d_value;
	//! Backup of the changed value
	double d_old_value;
};

///////////////////////////////////////////////////////////////////////////
// end of class MatrixClearCmd
///////////////////////////////////////////////////////////////////////////



#endif // MATRIX_COMMANDS_H
