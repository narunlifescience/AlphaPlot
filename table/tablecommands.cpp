/***************************************************************************
    File                 : tablecommands.cpp
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

#include "tablecommands.h"
#include "TableModel.h"
#include "Column.h"
#include "Interval.h"
#include "Double2StringFilter.h"
#include <QObject>
#include <QtDebug>

///////////////////////////////////////////////////////////////////////////
// class TableShowCommentsCmd
///////////////////////////////////////////////////////////////////////////
TableShowCommentsCmd::TableShowCommentsCmd( TableModel * model, bool show, QUndoCommand * parent )
 : QUndoCommand( parent ), d_new_state(show), d_model(model)
{
	if(show)
		setText(QObject::tr("%1: show column comments").arg(d_model->name()));
	else
		setText(QObject::tr("%1: hide column comments").arg(d_model->name()));
}

void TableShowCommentsCmd::redo()
{
	d_old_state = d_model->areCommentsShown();
	d_model->showComments(d_new_state);
}

void TableShowCommentsCmd::undo()
{
	d_model->showComments(d_old_state);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableShowCommentsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////
TableInsertColumnsCmd::TableInsertColumnsCmd( TableModel * model, int before, QList< shared_ptr<Column> > cols, QUndoCommand * parent)
 : QUndoCommand( parent ), d_model(model), d_before(before), d_cols(cols)
{
	setText(QObject::tr("%1: insert %2 column(s)").arg(d_model->name()).arg(d_cols.size()));
}

TableInsertColumnsCmd::~TableInsertColumnsCmd()
{
}

void TableInsertColumnsCmd::redo()
{
	d_rows_before = d_model->rowCount();
	d_model->insertColumns(d_before, d_cols);
}

void TableInsertColumnsCmd::undo()
{
	d_model->removeColumns(d_before, d_cols.size());
	d_model->setRowCount(d_rows_before);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetNumberOfRowsCmd
///////////////////////////////////////////////////////////////////////////
TableSetNumberOfRowsCmd::TableSetNumberOfRowsCmd( TableModel * model, int rows, QUndoCommand * parent )
 : QUndoCommand( parent ), d_model(model), d_rows(rows)
{
	setText(QObject::tr("%1: set the number of rows to %2").arg(d_model->name()).arg(rows));
}

TableSetNumberOfRowsCmd::~TableSetNumberOfRowsCmd()
{
}

void TableSetNumberOfRowsCmd::redo()
{
	d_old_rows = d_model->rowCount();
	d_model->setRowCount(d_rows);
}

void TableSetNumberOfRowsCmd::undo()
{
	d_model->setRowCount(d_old_rows);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableSetNumberOfRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////
TableRemoveColumnsCmd::TableRemoveColumnsCmd( TableModel * model, int first, int count, QUndoCommand * parent )
 : QUndoCommand( parent ), d_model(model), d_first(first), d_count(count)
{
	setText(QObject::tr("%1: remove %2 column(s)").arg(d_model->name()).arg(count));
}

TableRemoveColumnsCmd::~TableRemoveColumnsCmd()
{
}

void TableRemoveColumnsCmd::redo()
{
	if(d_old_cols.isEmpty())
		for(int i=d_first; i<(d_first+d_count); i++)
			d_old_cols.append(dynamic_pointer_cast<Column>(d_model->output(i)));

	d_model->removeColumns(d_first, d_count);
}

void TableRemoveColumnsCmd::undo()
{
	d_model->insertColumns(d_first, d_old_cols);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////

#if false

///////////////////////////////////////////////////////////////////////////
// class TableSetColumnValuesCmd
///////////////////////////////////////////////////////////////////////////
TableSetColumnValuesCmd::TableSetColumnValuesCmd( TableModel * model, int col, 
	const QStringList& data, QUndoCommand * parent )
 : QUndoCommand( parent ), d_model(model), d_col(col)
{
	d_data = new StringColumnData(data);
	d_backup = new StringColumnData();
	setText(QObject::tr("%1: set column values").arg(d_model->name()));
}

TableSetColumnValuesCmd::~TableSetColumnValuesCmd()
{
	delete d_data;
	delete d_backup;
	delete d_selection;
}

void TableSetColumnValuesCmd::redo()
{
	int strings = d_data->rowCount();
	int index = 0; // index in the string list
	int rows = d_model->rowCount();

	if(d_backup->rowCount() > 0)  // after previous undo
	{
		// prepare input filter
		d_in->input(0, d_data);
		AbstractDataSource * src = d_in->output(0);

		for(int i=0; i<rows; i++)
		{
			if(d_selection->isSet(i))
			{
				d_col_ptr->copy(src, index, i, 1);  
				index++;
				if(index >= strings) index = 0; // wrap around in the string array
			}
		}
	}
	else
	{
		d_col_ptr = d_model->columnPointer(d_col);
		d_in = d_model->inputFilter(d_col);
		d_out = d_model->outputFilter(d_col);

		// store the selection intervals in a internal interval attribute
		d_selection = new IntervalAttribute<bool>(d_col_ptr->asDataSource()->selectedIntervals());

		// prepare output filter
		d_out->input(0, d_col_ptr->asDataSource());
		AbstractStringDataSource * sds = static_cast<AbstractStringDataSource *>(d_out->output(0));

		// prepare input filter
		d_in->input(0, d_data);
		AbstractDataSource * src = d_in->output(0);

		for(int i=0; i<rows; i++)
		{
			if(d_selection->isSet(i))
			{
				d_backup->append(sds->textAt(i));
				d_col_ptr->copy(src, index, i, 1);  									
				index++;
				if(index >= strings) index = 0; // wrap around in the string array
			}
		}
	}

	// notify the view of the data change
	QList< Interval<int> > intervals = d_selection->intervals();
	foreach(Interval<int> iv, intervals)
		d_model->emitDataChanged(iv.start(), d_col, iv.end(), d_col);
}

void TableSetColumnValuesCmd::undo()
{
	int rows = d_col_ptr->asDataSource()->rowCount();
	int index = 0; // index in the string list

	// prepare input filter
	d_in->input(0, d_backup);
	AbstractDataSource * src = d_in->output(0);

	for(int i=0; i<rows; i++)
	{
		if(d_selection->isSet(i))
		{
			d_col_ptr->copy(src, index, i, 1);  
			index++;
		}
	}

	// notify the view of the data change
	QList< Interval<int> > intervals = d_selection->intervals();
	foreach(Interval<int> iv, intervals)
		d_model->emitDataChanged(iv.start(), d_col, iv.end(), d_col);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableSetColumnValuesCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableReplaceColumnsCmd
///////////////////////////////////////////////////////////////////////////
TableReplaceColumnsCmd::TableReplaceColumnsCmd( TableModel * model, int first, QList<AbstractColumnData *> cols,
		QList<AbstractFilter *> in_filters, QList<AbstractFilter *> out_filters, QUndoCommand * parent)
 : QUndoCommand( parent ), d_model(model), d_first(first), d_cols(cols), d_in_filters(in_filters), d_out_filters(out_filters)
{
	setText(QObject::tr("%1: replace columns").arg(d_model->name()));
	d_undone = false;
}

TableReplaceColumnsCmd::~TableReplaceColumnsCmd()
{
	if(d_undone) 
		for(int i=0; i<d_cols.size(); i++)
		{
			delete d_cols.at(i);
			delete d_in_filters.at(i);
			delete d_out_filters.at(i);
		}
	else
		for(int i=0; i<d_old_cols.size(); i++)
		{
			delete d_old_cols.at(i);
			delete d_old_in_filters.at(i);
			delete d_old_out_filters.at(i);
		}
}

void TableReplaceColumnsCmd::redo()
{
	d_rows_before = d_model->rowCount();
	if(d_old_cols.isEmpty())
	{
		for(int i=0; i<d_cols.size(); i++)
		{
			d_old_cols.append(d_model->columnPointer(d_first+i));
			d_old_in_filters.append(d_model->inputFilter(d_first+i));
			d_old_out_filters.append(d_model->outputFilter(d_first+i));
		}
		
	}
	d_model->replaceColumns(d_first, d_cols, d_in_filters, d_out_filters);
	d_undone = false;
}

void TableReplaceColumnsCmd::undo()
{
	d_model->replaceColumns(d_first, d_old_cols, d_old_in_filters, d_old_out_filters);
	if(d_rows_before < d_model->rowCount())
		d_model->removeRows(d_rows_before, d_model->rowCount() - d_rows_before);
	d_undone = true;
}

///////////////////////////////////////////////////////////////////////////
// end of class TableReplaceColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableReplaceFilterCmd
///////////////////////////////////////////////////////////////////////////
TableReplaceFilterCmd::TableReplaceFilterCmd( TableModel * model, int col, AbstractFilter * in_filter, 
		AbstractFilter * out_filter, QUndoCommand * parent)
 : QUndoCommand( parent ), d_model(model), d_col(col), d_in_filter(in_filter), d_out_filter(out_filter)
{
	setText(QObject::tr("%1: change column type mode").arg(d_model->name()));
	d_old_in_filter = 0;
	d_old_out_filter = 0;
	d_undone = false;
}

TableReplaceFilterCmd::~TableReplaceFilterCmd()
{
	if(d_undone)
	{
		if(d_in_filter) delete d_in_filter;
		if(d_out_filter) delete d_out_filter;
	}
	else
	{
		if(d_in_filter) delete d_old_in_filter;
		if(d_out_filter) delete d_old_out_filter;
	}
}

void TableReplaceFilterCmd::redo()
{
	if(!d_undone)
	{
		d_old_in_filter = d_model->inputFilter(d_col);
		d_old_out_filter = d_model->outputFilter(d_col);
	}
	if(d_in_filter)	d_model->setInputFilter(d_col, d_in_filter);
	if(d_out_filter) d_model->setOutputFilter(d_col, d_out_filter);
	d_undone = false;
}

void TableReplaceFilterCmd::undo()
{
	if(d_in_filter)	d_model->setInputFilter(d_col, d_old_in_filter);
	if(d_out_filter) d_model->setOutputFilter(d_col, d_old_out_filter);
	d_undone = true;
}

///////////////////////////////////////////////////////////////////////////
// end of class TableReplaceFilterCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetColumnNumericDisplayCmd
///////////////////////////////////////////////////////////////////////////
TableSetColumnNumericDisplayCmd::TableSetColumnNumericDisplayCmd(TableModel * model, int col, char format, int digits, QUndoCommand * parent)
 : QUndoCommand( parent ), d_model(model), d_col(col), d_format(format), d_digits(digits)
{
	setText(QObject::tr("%1: set column numeric display").arg(d_model->name()));
}

void TableSetColumnNumericDisplayCmd::redo()
{
	d_old_format = static_cast<Double2StringFilter *>(d_model->outputFilter(d_col))->numericFormat();
	d_old_digits = static_cast<Double2StringFilter *>(d_model->outputFilter(d_col))->numDigits();
	static_cast<Double2StringFilter *>(d_model->outputFilter(d_col))->setNumericFormat(d_format);
	static_cast<Double2StringFilter *>(d_model->outputFilter(d_col))->setNumDigits(d_digits);
}

void TableSetColumnNumericDisplayCmd::undo()
{
	static_cast<Double2StringFilter *>(d_model->outputFilter(d_col))->setNumericFormat(d_old_format);
	static_cast<Double2StringFilter *>(d_model->outputFilter(d_col))->setNumDigits(d_old_digits);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableSetColumnNumericDisplayCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetFormulaCmd
///////////////////////////////////////////////////////////////////////////
TableSetFormulaCmd::TableSetFormulaCmd(TableModel * model, int col, Interval<int> interval, const QString& formula, QUndoCommand * parent)
: QUndoCommand( parent ), d_col(col), d_interval(interval), d_formula(formula)
{
	setText(QObject::tr("%1: set formula").arg(d_model->name()));
	d_undone = false;
}

void TableSetFormulaCmd::redo()
{
	AbstractColumnData * ptr = d_model->columnPointer(d_col);
	if(!d_undone)
	{
		d_old_intervals = ptr->asDataSource()->formulaIntervals();
		for(int i=0; i<d_old_intervals.size(); i++)
		{
			if(!d_old_intervals.at(i).intersects(d_interval))
				d_old_intervals.removeAt(i--);
		}
		foreach(Interval<int> i, d_old_intervals)
			d_old_formulas.append(ptr->asDataSource()->formula(i.start()));
	}
	ptr->setFormula(d_interval, d_formula);
}

void TableSetFormulaCmd::undo()
{
	AbstractColumnData * ptr = d_model->columnPointer(d_col);
	for(int i=0; i<d_old_intervals.size(); i++)
	{
		ptr->setFormula(d_old_intervals.at(i), d_old_formulas.at(i));
	}
	d_undone = true;
}

///////////////////////////////////////////////////////////////////////////
// end of class TableSetFormulaCmd
///////////////////////////////////////////////////////////////////////////

#endif
