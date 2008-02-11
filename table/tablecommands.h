/***************************************************************************
    File                 : tablecommands.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Commands used in Table (part of the undo/redo framework)
                           
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

#ifndef TABLE_COMMANDS_H
#define TABLE_COMMANDS_H

#include <QUndoCommand>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QItemSelection>
class TableModel;
#include "Column.h"
#include "AbstractFilter.h"
#include "IntervalAttribute.h"

///////////////////////////////////////////////////////////////////////////
// class TableInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////
//! Insert columns
/**
 * The number of inserted columns is cols.size().
 */
class TableInsertColumnsCmd : public QUndoCommand
{
public:
	TableInsertColumnsCmd( TableModel * model, int before, QList< shared_ptr<Column> > cols, QUndoCommand * parent = 0 );
	~TableInsertColumnsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! Column to insert before
	int d_before;
	//! The new columns
	QList< shared_ptr<Column> > d_cols;
	//! Row count before the command
	int d_rows_before;

};

///////////////////////////////////////////////////////////////////////////
// end of class TableInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetNumberOfRowsCmd
///////////////////////////////////////////////////////////////////////////
//! Set the number of rows in the table
class TableSetNumberOfRowsCmd : public QUndoCommand
{
public:
	TableSetNumberOfRowsCmd( TableModel * model, int rows, QUndoCommand * parent = 0 );
	~TableSetNumberOfRowsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! Number of rows
	int d_rows;
	//! Number of rows before
	int d_old_rows;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableSetNumberOfRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////
//! Remove columns
class TableRemoveColumnsCmd : public QUndoCommand
{
public:
	TableRemoveColumnsCmd( TableModel * model, int first, int count, QList< shared_ptr<Column> > cols, QUndoCommand * parent = 0 );
	~TableRemoveColumnsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! The first column
	int d_first;
	//! The number of columns to be removed
	int d_count;
	//! The removed columns
	QList< shared_ptr<Column> > d_old_cols;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableMoveColumnCmd
///////////////////////////////////////////////////////////////////////////
//! Move a column
class TableMoveColumnCmd : public QUndoCommand
{
public:
	TableMoveColumnCmd( TableModel * model, int from, int to, QUndoCommand * parent = 0 );
	~TableMoveColumnCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! The old column index
	int d_from;
	//! The new column index
	int d_to;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableMoveColumnCmd
///////////////////////////////////////////////////////////////////////////

#if false

///////////////////////////////////////////////////////////////////////////
// class TableSetColumnValuesCmd
///////////////////////////////////////////////////////////////////////////
//! Set column values from a string list
/**
 * Make sure data contains at least one string. 
 * The input filter must be a filter that converts
 * from string to the type of the corresponding
 * column. The output filter must be a filter that 
 * converts from the type of the corresponding
 * column to string.
 */
class TableSetColumnValuesCmd : public QUndoCommand
{
public:
	TableSetColumnValuesCmd( TableModel * model, int col, const QStringList& data, QUndoCommand * parent = 0 );
	~TableSetColumnValuesCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! The changed column's index
	int d_col;
	//! Pointer to the column to be changed
	AbstractColumnData * d_col_ptr;
	//! The input filter
	AbstractFilter * d_in;
	//! The output filter
	AbstractFilter * d_out;
	//! The data as strings
	StringColumnData * d_data;	
	//! The data that gets replaced
	StringColumnData * d_backup;
	//! The selection
	IntervalAttribute<bool> * d_selection;

};

///////////////////////////////////////////////////////////////////////////
// end of class TableSetColumnValuesCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableReplaceColumnsCmd
///////////////////////////////////////////////////////////////////////////
//! Replace columns
/**
 * The number of replaced columns is cols.size(). You must
 * check that there are enough columns to be replaced 
 * before using this command.
 */
class TableReplaceColumnsCmd : public QUndoCommand
{
public:
	TableReplaceColumnsCmd( TableModel * model, int first, QList<AbstractColumnData *> cols,
		QList<AbstractFilter *> in_filters, QList<AbstractFilter *> out_filters, QUndoCommand * parent = 0 );
	~TableReplaceColumnsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! The first column to be replaced
	int d_first;
	//! The new columns
	QList<AbstractColumnData *> d_cols;
	//! The input filters
	QList<AbstractFilter *> d_in_filters;
	//! The output filters
	QList<AbstractFilter *> d_out_filters;
	//! The old columns
	QList<AbstractColumnData *> d_old_cols;
	//! The old input filters
	QList<AbstractFilter *> d_old_in_filters;
	//! The old output filters
	QList<AbstractFilter *> d_old_out_filters;
	//! Flag to determine whether the columns can be deleted in the dtor
	bool d_undone;
	//! Row count before the command
	int d_rows_before;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableReplaceColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableReplaceFilterCmd
///////////////////////////////////////////////////////////////////////////
//! Replace input and/or output filter of a column
/*
 * If one of the two filters shall not be replaced, set it to 0.
 */
class TableReplaceFilterCmd : public QUndoCommand
{
public:
	TableReplaceFilterCmd( TableModel * model, int col, AbstractFilter * in_filter, 
		AbstractFilter * out_filter, QUndoCommand * parent = 0 );
	~TableReplaceFilterCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! The column whose filters to replace
	int d_col;
	//! The new input filter
	AbstractFilter * d_in_filter;
	//! The new output filter
	AbstractFilter * d_out_filter;
	//! The old input filter
	AbstractFilter * d_old_in_filter;
	//! The old output filter
	AbstractFilter * d_old_out_filter;
	//! Flag to determine whether the columns can be deleted in the dtor
	bool d_undone;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableReplaceFilterCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetColumnNumericDisplayCmd
///////////////////////////////////////////////////////////////////////////
//! Sets a double columns numeric display parameters (format and displayed digits)
/*
 * \sa Double2StringFilter
 */
class TableSetColumnNumericDisplayCmd : public QUndoCommand
{
public:
	TableSetColumnNumericDisplayCmd(TableModel * model, int col, char format, int digits, QUndoCommand * parent = 0 );

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! The changed column's index
	int d_col;
	//! New numeric format
	char d_format;
	//! Old numeric format
	char d_old_format;
	//! New number of displayed digits
	int d_digits;
	//! Old number of displayed digits
	int d_old_digits;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableSetColumnNumericDisplayCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetFormulaCmd
///////////////////////////////////////////////////////////////////////////
//! Sets a formula for a given interval
class TableSetFormulaCmd : public QUndoCommand
{
public:
	TableSetFormulaCmd(TableModel * model, int col, Interval<int> interval, const QString& formula, QUndoCommand * parent = 0 );

	virtual void redo();
	virtual void undo();

private:
	//! The changed column's index
	int d_col;
	//! The interval
	Interval<int> d_interval;
	//! New formula
	QString d_formula;
	//! The old intervals
	QList< Interval<int> > d_old_intervals;
	//! Old formulas
	QList<QString> d_old_formulas;
	//! The model to modify
	TableModel * d_model;
	//! Flag to determine whether undo has be executed before
	bool d_undone;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableSetFormulaCmd
///////////////////////////////////////////////////////////////////////////

#endif
#endif // ifndef TABLE_COMMANDS_H
