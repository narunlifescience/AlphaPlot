/***************************************************************************
    File                 : columncommands.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Commands to be called by Column to modify ColumnPrivate

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

#ifndef COLUMNCOMMANDS_H
#define COLUMNCOMMANDS_H

#include <QUndoCommand>
#include "Column.h"
#include "ColumnPrivate.h"
#include "core/AbstractSimpleFilter.h"
#include "lib/IntervalAttribute.h"

///////////////////////////////////////////////////////////////////////////
// class ColumnSetModeCmd
///////////////////////////////////////////////////////////////////////////
//! Set the column mode 
class ColumnSetModeCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnSetModeCmd(shared_ptr<ColumnPrivate> col, SciDAVis::ColumnMode mode, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnSetModeCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The previous mode
	SciDAVis::ColumnMode d_old_mode;	
	//! The new mode
	SciDAVis::ColumnMode d_mode;
	//! The old data type
	SciDAVis::ColumnDataType d_old_type;
	//! The new data type
	SciDAVis::ColumnDataType d_new_type;
	//! Pointer to old data
	void * d_old_data;
	//! Pointer to new data
	void * d_new_data;
	//! The new input filter
	shared_ptr<AbstractSimpleFilter> d_new_in_filter;
	//! The new output filter
	shared_ptr<AbstractSimpleFilter> d_new_out_filter;
	//! The old input filter
	shared_ptr<AbstractSimpleFilter> d_old_in_filter;
	//! The old output filter
	shared_ptr<AbstractSimpleFilter> d_old_out_filter;
	//! The old validity information
	IntervalAttribute<bool> d_old_validity;
	//! The new validity information
	IntervalAttribute<bool> d_new_validity;
	//! A status flag
	bool d_undone;
	//! A status flag
	bool d_executed;
};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetModeCmd
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// class ColumnFullCopyCmd
///////////////////////////////////////////////////////////////////////////
//! Copy a complete column 
class ColumnFullCopyCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnFullCopyCmd(shared_ptr<ColumnPrivate> col, const AbstractColumn * src, QUndoCommand * parent = 0 );
	//! Ctor
	ColumnFullCopyCmd(shared_ptr<ColumnPrivate> col, shared_ptr<AbstractColumn> src, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnFullCopyCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The column to copy
	const AbstractColumn * d_src;
	//! A backup column
	shared_ptr<ColumnPrivate> d_backup;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnFullCopyCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnPartialCopyCmd
///////////////////////////////////////////////////////////////////////////
//! Copy parts of a column
class ColumnPartialCopyCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnPartialCopyCmd(shared_ptr<ColumnPrivate> col, const AbstractColumn * src, int src_start, int dest_start, int num_rows, QUndoCommand * parent = 0 );
	//! Ctor
	ColumnPartialCopyCmd(shared_ptr<ColumnPrivate> col, shared_ptr<AbstractColumn> src, int src_start, int dest_start, int num_rows, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnPartialCopyCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The column to copy
	const AbstractColumn * d_src;
	//! A backup of the orig. column
	shared_ptr<ColumnPrivate> d_col_backup;
	//! A backup of the source column
	shared_ptr<ColumnPrivate> d_src_backup;
	//! Start index in source column
	int d_src_start;
	//! Start index in destination column
	int d_dest_start;
	//! Number of rows to copy
	int d_num_rows;
	//! Previous number of rows in the destination column
	int d_old_row_count;
	

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnPartialCopyCmd
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// class ColumnInsertEmptyRowsCmd
///////////////////////////////////////////////////////////////////////////
//! Insert empty rows 
class ColumnInsertEmptyRowsCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnInsertEmptyRowsCmd(shared_ptr<ColumnPrivate> col, int before, int count, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnInsertEmptyRowsCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! Row to insert before
	int d_before;
	//! Number of rows
	int d_count;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnInsertEmptyRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////
//!  
class ColumnRemoveRowsCmd : public QUndoCommand 
{
public:
	//! Ctor
	ColumnRemoveRowsCmd(shared_ptr<ColumnPrivate> col, int first, int count, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnRemoveRowsCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The first row
	int d_first;
	//! The number of rows to be removed
	int d_count;
	//! Number of removed rows actually containing data
	int d_data_row_count;
	//! The number of rows before the removal
	int d_old_size;
	//! Column saving the removed rows
	shared_ptr<ColumnPrivate> d_backup;
	//! Backup of the masking attribute
	IntervalAttribute<bool> d_masking;
	//! Backup of the formula attribute
	IntervalAttribute<QString> d_formulas;
};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetPlotDesignationCmd
///////////////////////////////////////////////////////////////////////////
//! Sets a column's plot designation
class ColumnSetPlotDesignationCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnSetPlotDesignationCmd(shared_ptr<ColumnPrivate> col, SciDAVis::PlotDesignation pd, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnSetPlotDesignationCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! New plot designation
	SciDAVis::PlotDesignation d_new_pd;
	//! Old plot designation
	SciDAVis::PlotDesignation d_old_pd;
};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetPlotDesignationCmd
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// class ColumnClearCmd
///////////////////////////////////////////////////////////////////////////
//! Clear the column 
class ColumnClearCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnClearCmd(shared_ptr<ColumnPrivate> col, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnClearCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The column's data type
	SciDAVis::ColumnDataType d_type;
	//! Pointer to the old data pointer
	void * d_data;
	//! Pointer to an empty data vector
	void * d_empty_data;
	//! The old validity
	IntervalAttribute<bool> d_validity;
	//! Status flag
	bool d_undone;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnClearCmd
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// class ColumnClearValidityCmd
///////////////////////////////////////////////////////////////////////////
//! Clear validity information 
class ColumnClearValidityCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnClearValidityCmd(shared_ptr<ColumnPrivate> col, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnClearValidityCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The old validity
	IntervalAttribute<bool> d_validity;
	//! A status flag
	bool d_copied;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnClearValidityCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnClearMasksCmd
///////////////////////////////////////////////////////////////////////////
//! Clear validity information 
class ColumnClearMasksCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnClearMasksCmd(shared_ptr<ColumnPrivate> col, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnClearMasksCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The old masks
	IntervalAttribute<bool> d_masking;
	//! A status flag
	bool d_copied;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnClearMasksCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetInvalidCmd
///////////////////////////////////////////////////////////////////////////
//! Mark an interval of rows as invalid 
class ColumnSetInvalidCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnSetInvalidCmd(shared_ptr<ColumnPrivate> col, Interval<int> interval, bool invalid, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnSetInvalidCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The interval
	Interval<int> d_interval;
	//! Valid/invalid flag
	bool d_invalid;
	//! Interval attribute backup
	IntervalAttribute<bool> d_validity;
	//! A status flag
	bool d_copied;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetInvalidCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetMaskedCmd
///////////////////////////////////////////////////////////////////////////
//! Mark an interval of rows as masked
class ColumnSetMaskedCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnSetMaskedCmd(shared_ptr<ColumnPrivate> col, Interval<int> interval, bool masked, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnSetMaskedCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The interval
	Interval<int> d_interval;
	//! Mask/unmask flag
	bool d_masked;
	//! Interval attribute backup
	IntervalAttribute<bool> d_masking;
	//! A status flag
	bool d_copied;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetMaskedCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetFormulaCmd
///////////////////////////////////////////////////////////////////////////
//! Set the formula for a given interval
class ColumnSetFormulaCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnSetFormulaCmd(shared_ptr<ColumnPrivate> col, Interval<int> interval, const QString& formula, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnSetFormulaCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The interval
	Interval<int> d_interval;
	//! The new formula
	QString d_formula;
	//! Interval attribute backup
	IntervalAttribute<QString> d_formulas;
	//! A status flag
	bool d_copied;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetFormulaCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnClearFormulasCmd
///////////////////////////////////////////////////////////////////////////
//! Clear all associated formulas 
class ColumnClearFormulasCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnClearFormulasCmd(shared_ptr<ColumnPrivate> col, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnClearFormulasCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The old formulas
	IntervalAttribute<QString> d_formulas;
	//! A status flag
	bool d_copied;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnClearFormulasCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetTextCmd
///////////////////////////////////////////////////////////////////////////
//! Set the text for a string cell 
class ColumnSetTextCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnSetTextCmd(shared_ptr<ColumnPrivate> col, int row, const QString& new_value, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnSetTextCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The row to modify
	int d_row;
	//! The new value
	QString d_new_value;
	//! The old value
	QString d_old_value;
	//! The old number of rows
	int d_row_count;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetTextCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetValueCmd
///////////////////////////////////////////////////////////////////////////
//! Set the value for a double cell 
class ColumnSetValueCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnSetValueCmd(shared_ptr<ColumnPrivate> col, int row, double new_value, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnSetValueCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The row to modify
	int d_row;
	//! The new value
	double d_new_value;
	//! The old value
	double d_old_value;
	//! The old number of rows
	int d_row_count;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetValueCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetDateTimeCmd
///////////////////////////////////////////////////////////////////////////
//! Set the value of a date-time cell 
class ColumnSetDateTimeCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnSetDateTimeCmd(shared_ptr<ColumnPrivate> col, int row, const QDateTime& new_value, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnSetDateTimeCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The row to modify
	int d_row;
	//! The new value
	QDateTime d_new_value;
	//! The old value
	QDateTime d_old_value;
	//! The old number of rows
	int d_row_count;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetDateTimeCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnReplaceTextsCmd
///////////////////////////////////////////////////////////////////////////
//! Replace a range of strings in a string column 
class ColumnReplaceTextsCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnReplaceTextsCmd(shared_ptr<ColumnPrivate> col, int first, const QStringList& new_values, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnReplaceTextsCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The first row to replace
	int d_first;
	//! The new values
	QStringList d_new_values;
	//! The old values
	QStringList d_old_values;
	//! Status flag
	bool d_copied;
	//! The old number of rows
	int d_row_count;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnReplaceTextsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnReplaceValuesCmd
///////////////////////////////////////////////////////////////////////////
//! Replace a range of doubles in a double column 
class ColumnReplaceValuesCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnReplaceValuesCmd(shared_ptr<ColumnPrivate> col, int first, const QVector<double>& new_values, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnReplaceValuesCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The first row to replace
	int d_first;
	//! The new values
	QVector<double> d_new_values;
	//! The old values
	QVector<double> d_old_values;
	//! Status flag
	bool d_copied;
	//! The old number of rows
	int d_row_count;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnReplaceValuesCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnReplaceDateTimesCmd
///////////////////////////////////////////////////////////////////////////
//! Replace a range of date-times in a date-time column 
class ColumnReplaceDateTimesCmd : public QUndoCommand
{
public:
	//! Ctor
	ColumnReplaceDateTimesCmd(shared_ptr<ColumnPrivate> col, int first, const QList<QDateTime>& new_values, QUndoCommand * parent = 0 );
	//! Dtor
	~ColumnReplaceDateTimesCmd();

	//! Execute the command
	virtual void redo();
	//! Undo the command
	virtual void undo();

private:
	//! The column to modify
	shared_ptr<ColumnPrivate> d_col;
	//! The first row to replace
	int d_first;
	//! The new values
	QList<QDateTime> d_new_values;
	//! The old values
	QList<QDateTime> d_old_values;
	//! Status flag
	bool d_copied;
	//! The old number of rows
	int d_row_count;

};
///////////////////////////////////////////////////////////////////////////
// end of class ColumnReplaceDateTimesCmd
///////////////////////////////////////////////////////////////////////////




#endif
