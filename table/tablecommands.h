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
#include "core/AbstractDataSource.h"
#include "AbstractColumnData.h"
#include "StringColumnData.h"
#include "core/AbstractFilter.h"
#include "lib/IntervalAttribute.h"

///////////////////////////////////////////////////////////////////////////
// class TableShowCommentsCmd
///////////////////////////////////////////////////////////////////////////
//! Toggles showing of column comments
class TableShowCommentsCmd : public QUndoCommand
{
public:
	TableShowCommentsCmd(TableModel * model, bool show, QUndoCommand * parent = 0 );

	virtual void redo();
	virtual void undo();

private:
	//! New flag state
	bool d_new_state;
	//! Old flag state
	bool d_old_state;
	//! The model to modify
	TableModel * d_model;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableShowCommentsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetColumnPlotDesignationCmd
///////////////////////////////////////////////////////////////////////////
//! Sets a column's plot designation
class TableSetColumnPlotDesignationCmd : public QUndoCommand
{
public:
	TableSetColumnPlotDesignationCmd(TableModel * model, int col, SciDAVis::PlotDesignation pd, QUndoCommand * parent = 0 );

	virtual void redo();
	virtual void undo();

private:
	//! The changed column's index
	int d_col;
	//! New plot designation
	SciDAVis::PlotDesignation d_new_pd;
	//! Old plot designation
	SciDAVis::PlotDesignation d_old_pd;
	//! The model to modify
	TableModel * d_model;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableSetColumnPlotDesignationCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetColumnLabelCmd
///////////////////////////////////////////////////////////////////////////
//! Sets a column's label
class TableSetColumnLabelCmd : public QUndoCommand
{
public:
	TableSetColumnLabelCmd(TableModel * model, int col, const QString& label, QUndoCommand * parent = 0 );

	virtual void redo();
	virtual void undo();

private:
	//! The changed column's index
	int d_col;
	//! New column label
	QString d_new_label;
	//! Old column label
	QString d_old_label;
	//! The model to modify
	TableModel * d_model;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableSetColumnLabelCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetColumnCommentCmd
///////////////////////////////////////////////////////////////////////////
//! Sets a column's comment
class TableSetColumnCommentCmd : public QUndoCommand
{
public:
	TableSetColumnCommentCmd(TableModel * model, int col, const QString& comment, QUndoCommand * parent = 0 );

	virtual void redo();
	virtual void undo();

private:
	//! The changed column's index
	int d_col;
	//! New column comment
	QString d_new_comment;
	//! Old column comment
	QString d_old_comment;
	//! The model to modify
	TableModel * d_model;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableSetColumnCommentCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableClearColumnCmd
///////////////////////////////////////////////////////////////////////////
//! Removes all data from a column
/**
 * This command removes all data from a column except the label, the comment,
 * the plot designation and the formulas. It replaces the column with an
 * empty one, therefore the old column never needs to be copied. So this command
 * should be pretty fast in redo() as well as undo().
 */
class TableClearColumnCmd : public QUndoCommand
{
public:
	TableClearColumnCmd( TableModel * model, int col, QUndoCommand * parent = 0 );
	~TableClearColumnCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The cleared column's index
	int d_col;
	//! The original column
	AbstractColumnData * d_orig_col;
	//! The cleared column
	AbstractColumnData * d_cleared_col;
	//! The model to modify
	TableModel * d_model;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableClearColumnCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableUserInputCmd
///////////////////////////////////////////////////////////////////////////
//! Handles user input
class TableUserInputCmd : public QUndoCommand
{
public:
	TableUserInputCmd( TableModel * model, const QModelIndex& index, QUndoCommand * parent = 0 );
	~TableUserInputCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model index of the edited cell
	QModelIndex d_index;
	//! The previous data
	QVariant d_old_data;
	//! The previous data
	QVariant d_new_data;
	//! Validity flag for undo
	bool d_invalid_before;
	//! Validity flag for redo
	bool d_invalid_after;
	//! The state flag 
	/**
	 * true means that his command has been undone at least once
	 */
	bool d_previous_undo;
	//! The model to modify
	TableModel * d_model;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableUserInputCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableAppendRowsCmd
///////////////////////////////////////////////////////////////////////////
//! Append rows
class TableAppendRowsCmd : public QUndoCommand
{
public:
	TableAppendRowsCmd( TableModel * model, int count, QUndoCommand * parent = 0 );
	~TableAppendRowsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! Number of rows
	int d_count;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableAppendRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////
//! Remove rows
class TableRemoveRowsCmd : public QUndoCommand
{
public:
	TableRemoveRowsCmd( TableModel * model, int first, int count, QUndoCommand * parent = 0 );
	~TableRemoveRowsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! The first row
	int d_first;
	//! The number of rows to be removed
	int d_count;
	//! Columns saving the removed rows
	QList<AbstractColumnData *> d_old_cols;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////
//! Remove columns
class TableRemoveColumnsCmd : public QUndoCommand
{
public:
	TableRemoveColumnsCmd( TableModel * model, int first, int count, QUndoCommand * parent = 0 );
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
	QList<AbstractColumnData *> d_old_cols;
	//! The removed input filters
	QList<AbstractFilter *> d_in_filters;
	//! The removed output filters
	QList<AbstractFilter *> d_out_filters;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableInsertRowsCmd
///////////////////////////////////////////////////////////////////////////
//! Insert rows
class TableInsertRowsCmd : public QUndoCommand
{
public:
	TableInsertRowsCmd( TableModel * model, int before, int count, QUndoCommand * parent = 0 );
	~TableInsertRowsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! Row to insert before
	int d_before;
	//! Number of rows
	int d_count;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableInsertRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableAppendColumnsCmd
///////////////////////////////////////////////////////////////////////////
//! Append columns
class TableAppendColumnsCmd : public QUndoCommand
{
public:
	TableAppendColumnsCmd( TableModel * model, QList<AbstractColumnData *> cols,
		QList<AbstractFilter *> in_filters, QList<AbstractFilter *> out_filters, QUndoCommand * parent = 0 );
	~TableAppendColumnsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! The new columns
	QList<AbstractColumnData *> d_cols;
	//! The input filters
	QList<AbstractFilter *> d_in_filters;
	//! The output filters
	QList<AbstractFilter *> d_out_filters;
	//! Flag to determine whether the columns can be deleted in the dtor
	bool d_undone;
};

///////////////////////////////////////////////////////////////////////////
// end of class TableAppendColumnsCmd
///////////////////////////////////////////////////////////////////////////

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
	TableInsertColumnsCmd( TableModel * model, int before, QList<AbstractColumnData *> cols,
		QList<AbstractFilter *> in_filters, QList<AbstractFilter *> out_filters, QUndoCommand * parent = 0 );
	~TableInsertColumnsCmd();

	virtual void redo();
	virtual void undo();

private:
	//! The model to modify
	TableModel * d_model;
	//! Column to insert before
	int d_before;
	//! The new columns
	QList<AbstractColumnData *> d_cols;
	//! The input filters
	QList<AbstractFilter *> d_in_filters;
	//! The output filters
	QList<AbstractFilter *> d_out_filters;
	//! Flag to determine whether the columns can be deleted in the dtor
	bool d_undone;
	//! Row count before the command
	int d_rows_before;

};

///////////////////////////////////////////////////////////////////////////
// end of class TableInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////

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

#endif // ifndef TABLE_COMMANDS_H
