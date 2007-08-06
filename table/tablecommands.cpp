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
#include "StringColumnData.h"
#include "DoubleColumnData.h"
#include "DateTimeColumnData.h"
#include "Interval.h"
#include <QObject>
#include <QtDebug>

///////////////////////////////////////////////////////////////////////////
// class TableShowCommentsCmd
///////////////////////////////////////////////////////////////////////////
TableShowCommentsCmd::TableShowCommentsCmd( TableModel * model, bool show, QUndoCommand * parent )
 : QUndoCommand( parent ), d_new_state(show), d_model(model)
{
	if(show)
		setText(QObject::tr("show column comments"));
	else
		setText(QObject::tr("hide column comments"));
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
// class TableSetColumnPlotDesignationCmd
///////////////////////////////////////////////////////////////////////////
TableSetColumnPlotDesignationCmd::TableSetColumnPlotDesignationCmd( TableModel * model, int col, AbstractDataSource::PlotDesignation pd , QUndoCommand * parent )
 : QUndoCommand( parent ), d_col(col), d_new_pd(pd), d_model(model)
{
		setText(QObject::tr("set column plot designation"));
}

void TableSetColumnPlotDesignationCmd::redo()
{
	d_old_pd = d_model->columnPlotDesignation(d_col);
	d_model->setColumnPlotDesignation(d_col, d_new_pd);
}

void TableSetColumnPlotDesignationCmd::undo()
{
	d_model->setColumnPlotDesignation(d_col, d_old_pd);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableSetColumnPlotDesignationCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetColumnLabelCmd
///////////////////////////////////////////////////////////////////////////
TableSetColumnLabelCmd::TableSetColumnLabelCmd( TableModel * model, int col, const QString& label , QUndoCommand * parent )
 : QUndoCommand( parent ), d_col(col), d_new_label(label), d_model(model)
{
		setText(QObject::tr("set column label"));
}

void TableSetColumnLabelCmd::redo()
{
	d_old_label = d_model->columnLabel(d_col);
	d_model->setColumnLabel(d_col, d_new_label);
}

void TableSetColumnLabelCmd::undo()
{
	d_model->setColumnLabel(d_col, d_old_label);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableSetColumnLabelCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetColumnCommentCmd
///////////////////////////////////////////////////////////////////////////
TableSetColumnCommentCmd::TableSetColumnCommentCmd( TableModel * model, int col, const QString& comment , QUndoCommand * parent )
 : QUndoCommand( parent ), d_col(col), d_new_comment(comment), d_model(model)
{
		setText(QObject::tr("set column comment"));
}

void TableSetColumnCommentCmd::redo()
{
	d_old_comment = d_model->columnComment(d_col);
	d_model->setColumnComment(d_col, d_new_comment);
}

void TableSetColumnCommentCmd::undo()
{
	d_model->setColumnComment(d_col, d_old_comment);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableSetColumnCommentCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableClearColumnCmd
///////////////////////////////////////////////////////////////////////////
TableClearColumnCmd::TableClearColumnCmd( TableModel * model, int col, QUndoCommand * parent )
 : QUndoCommand( parent ), d_col(col), d_model(model)
{
	d_orig_col = 0;
	d_cleared_col = 0;
	setText(QObject::tr("clear column"));
}
TableClearColumnCmd::~TableClearColumnCmd()
{
	if(d_orig_col)
		delete d_orig_col;
	if(d_cleared_col)
		delete d_cleared_col;
}

void TableClearColumnCmd::redo()
{
	d_orig_col = d_model->columnPointer(d_col);

	if(!d_cleared_col) // no previous undo
	{
		// create a column of the correct type
		if(qobject_cast<StringColumnData *>(d_model->columnPointer(d_col)->asQObject()))
			d_cleared_col = new StringColumnData();
		else if(qobject_cast<DoubleColumnData *>(d_model->columnPointer(d_col)->asQObject()))
			d_cleared_col = new DoubleColumnData();
		else
			d_cleared_col = new DateTimeColumnData();

		// keep the designation, label and comment
		d_cleared_col->setPlotDesignation(d_orig_col->asDataSource()->plotDesignation());
		d_cleared_col->setLabel(d_orig_col->asDataSource()->label());
		d_cleared_col->setComment(d_orig_col->asDataSource()->comment());
		// keep the formulas
		QList< Interval<int> > formulas = d_orig_col->formulaIntervals();
		foreach(Interval<int> i, formulas)
			d_cleared_col->setFormula(i, d_orig_col->formula(i.start()));
	}
	// replace the column with the cleared one
	QList<AbstractColumnData *> list;
	list << d_cleared_col;
	d_model->replaceColumns(d_col, list);
	d_cleared_col = 0; // don't delete the active col in dtor
}

void TableClearColumnCmd::undo()
{
	d_cleared_col = d_model->columnPointer(d_col);
	QList<AbstractColumnData *> list;
	list << d_orig_col;
	d_model->replaceColumns(d_col, list);
	d_orig_col = 0; // don't delete the active col in dtor
}

///////////////////////////////////////////////////////////////////////////
// end of class TableClearColumnCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableUserInputCmd
///////////////////////////////////////////////////////////////////////////
TableUserInputCmd::TableUserInputCmd( TableModel * model, const QModelIndex& index, QUndoCommand * parent )
 : QUndoCommand( parent ), d_index(index), d_model(model)
{
	d_previous_undo = false;
	setText(QObject::tr("user input"));
}

TableUserInputCmd::~TableUserInputCmd()
{
}

void TableUserInputCmd::redo()
{
	if(d_previous_undo) 
	{
		d_model->setData(d_index, d_new_data, Qt::EditRole); 
		d_model->columnPointer(d_index.column())->setInvalid(d_index.row(), d_invalid_after);
	}
	else
	{
		d_old_data = d_model->data(d_index, Qt::EditRole);
		d_invalid_before = d_model->output(d_index.column())->isInvalid(d_index.row());
	}
}

void TableUserInputCmd::undo()
{
		d_new_data = d_model->data(d_index, Qt::EditRole);
		d_invalid_after = d_model->output(d_index.column())->isInvalid(d_index.row());
		d_model->setData(d_index, d_old_data, Qt::EditRole); 
		d_model->columnPointer(d_index.column())->setInvalid(d_index.row(), d_invalid_before);
		d_previous_undo = true;
}

///////////////////////////////////////////////////////////////////////////
// end of class TableUserInputCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableAppendRowsCmd
///////////////////////////////////////////////////////////////////////////
TableAppendRowsCmd::TableAppendRowsCmd( TableModel * model, int count, QUndoCommand * parent )
 : QUndoCommand( parent ), d_model(model), d_count(count)
{
	setText(QObject::tr("append rows"));
}

TableAppendRowsCmd::~TableAppendRowsCmd()
{
}

void TableAppendRowsCmd::redo()
{
	d_model->appendRows(d_count);
}

void TableAppendRowsCmd::undo()
{
	d_model->removeRows(d_model->rowCount()-d_count, d_count);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableAppendRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////
TableRemoveRowsCmd::TableRemoveRowsCmd( TableModel * model, int first, int count, QUndoCommand * parent )
 : QUndoCommand( parent ), d_model(model), d_first(first), d_count(count)
{
	setText(QObject::tr("remove rows"));
}

TableRemoveRowsCmd::~TableRemoveRowsCmd()
{
	for(int i=0; i<d_old_cols.size(); i++)
		delete d_old_cols.at(i);
}

void TableRemoveRowsCmd::redo()
{
	int cols = d_model->columnCount();
	if(d_old_cols.size() == 0) // no previous undo
		for(int i=0; i<cols; i++)
		{
			AbstractDataSource * src = d_model->output(i);
			if(qobject_cast<AbstractDoubleDataSource *>(src))
				d_old_cols.append(new DoubleColumnData());
			else if(qobject_cast<AbstractStringDataSource *>(src))
				d_old_cols.append(new StringColumnData());
			else
				d_old_cols.append(new DateTimeColumnData());
			d_old_cols.at(i)->copy(src, d_first, 0, d_count);
			
			// copy masking 
			QList< Interval<int> > masking = src->maskedIntervals();
			Interval<int>::restrictList(&masking, Interval<int>(d_first, d_first + d_count -1));
			foreach(Interval<int> mask_iv, masking)
			{	
				mask_iv.translate(-d_first);
				d_old_cols.at(i)->setMasked( mask_iv );
			}

			// copy formulas
			AbstractColumnData * fsrc = d_model->columnPointer(i);
			QList< Interval<int> > formula_ivs = fsrc->formulaIntervals();
			Interval<int>::restrictList(&formula_ivs, Interval<int>(d_first, d_first + d_count -1));
	 		foreach(Interval<int> fiv, formula_ivs)
			{
				QString formula = fsrc->formula(fiv.start());
				fiv.translate(-d_first);
				d_old_cols.at(i)->setFormula( fiv, formula );
			}
		} // end for all cols

	d_model->removeRows(d_first, d_count);
}

void TableRemoveRowsCmd::undo()
{
	d_model->insertRows(d_first, d_count);

	int cols = d_model->columnCount();
	for(int i=0; i<cols; i++)
	{
		AbstractDataSource * src = d_old_cols.at(i)->asDataSource();
		AbstractColumnData * dest = d_model->columnPointer(i);
		dest->copy(src, 0, d_first, d_count);

		// copy masking 
		QList< Interval<int> > masking = src->maskedIntervals();
		foreach(Interval<int> mask_iv, masking)
		{	
			mask_iv.translate(d_first);
			dest->setMasked( mask_iv );
		}

		// copy formulas
		AbstractColumnData * fsrc = d_old_cols.at(i);
		QList< Interval<int> > formula_ivs = fsrc->formulaIntervals();
		foreach(Interval<int> fiv, formula_ivs)
		{
			QString formula = fsrc->formula(fiv.start());
			fiv.translate(d_first);
			dest->setFormula( fiv, formula );
		}
	}
}

///////////////////////////////////////////////////////////////////////////
// end of class TableRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////
TableRemoveColumnsCmd::TableRemoveColumnsCmd( TableModel * model, int first, int count, QUndoCommand * parent )
 : QUndoCommand( parent ), d_model(model), d_first(first), d_count(count)
{
	setText(QObject::tr("remove columns"));
}

TableRemoveColumnsCmd::~TableRemoveColumnsCmd()
{
	for(int i=0; i<d_old_cols.size(); i++)
	{
		delete d_old_cols.at(i);
		delete d_in_filters.at(i);
		delete d_out_filters.at(i);
	}
}

void TableRemoveColumnsCmd::redo()
{
	for(int i=d_first; i<(d_first+d_count); i++)
	{
		d_old_cols.append(d_model->columnPointer(i));
		d_in_filters.append(d_model->inputFilter(i));
		d_out_filters.append(d_model->outputFilter(i));
	}

	d_model->removeColumns(d_first, d_count);
}

void TableRemoveColumnsCmd::undo()
{
	d_model->insertColumns(d_first, d_old_cols, d_in_filters, d_out_filters);
	d_old_cols.clear();
	d_in_filters.clear();
	d_out_filters.clear();
}

///////////////////////////////////////////////////////////////////////////
// end of class TableRemoveColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableInsertRowsCmd
///////////////////////////////////////////////////////////////////////////
TableInsertRowsCmd::TableInsertRowsCmd( TableModel * model, int before, int count, QUndoCommand * parent )
 : QUndoCommand( parent ), d_model(model), d_before(before), d_count(count)
{
	setText(QObject::tr("insert rows"));
}

TableInsertRowsCmd::~TableInsertRowsCmd()
{
}

void TableInsertRowsCmd::redo()
{
	d_model->insertRows(d_before, d_count);
}

void TableInsertRowsCmd::undo()
{
	d_model->removeRows(d_before, d_count);
}

///////////////////////////////////////////////////////////////////////////
// end of class TableInsertRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableAppendColumnsCmd
///////////////////////////////////////////////////////////////////////////
TableAppendColumnsCmd::TableAppendColumnsCmd( TableModel * model, QList<AbstractColumnData *> cols,
		QList<AbstractFilter *> in_filters, QList<AbstractFilter *> out_filters, QUndoCommand * parent)
 : QUndoCommand( parent ), d_model(model), d_cols(cols), d_in_filters(in_filters), d_out_filters(out_filters)
{
	setText(QObject::tr("append columns"));
	d_undone = false;
}

TableAppendColumnsCmd::~TableAppendColumnsCmd()
{
	if(d_undone) // if the columns are not needed anymore, delete them
		for(int i=0; i<d_cols.size(); i++)
		{
			delete d_cols.at(i);
			delete d_in_filters.at(i);
			delete d_out_filters.at(i);
		}
}

void TableAppendColumnsCmd::redo()
{
	d_model->appendColumns(d_cols, d_in_filters, d_out_filters);
	d_undone = false;
}

void TableAppendColumnsCmd::undo()
{
	d_model->removeColumns(d_model->columnCount()-d_cols.size(), d_cols.size());
	d_undone = true;
}

///////////////////////////////////////////////////////////////////////////
// end of class TableAppendColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////
TableInsertColumnsCmd::TableInsertColumnsCmd( TableModel * model, int before, QList<AbstractColumnData *> cols,
		QList<AbstractFilter *> in_filters, QList<AbstractFilter *> out_filters, QUndoCommand * parent)
 : QUndoCommand( parent ), d_model(model), d_before(before), d_cols(cols), d_in_filters(in_filters), d_out_filters(out_filters)
{
	setText(QObject::tr("insert columns"));
	d_undone = false;
}

TableInsertColumnsCmd::~TableInsertColumnsCmd()
{
	if(d_undone) // if the columns are not needed anymore, delete them
		for(int i=0; i<d_cols.size(); i++)
		{
			delete d_cols.at(i);
			delete d_in_filters.at(i);
			delete d_out_filters.at(i);
		}
}

void TableInsertColumnsCmd::redo()
{
	d_rows_before = d_model->rowCount();
	d_model->insertColumns(d_before, d_cols, d_in_filters, d_out_filters);
	d_undone = false;
}

void TableInsertColumnsCmd::undo()
{
	d_model->removeColumns(d_before, d_cols.size());
	if(d_rows_before < d_model->rowCount())
		d_model->removeRows(d_rows_before, d_model->rowCount() - d_rows_before);
	d_undone = true;
}

///////////////////////////////////////////////////////////////////////////
// end of class TableInsertColumnsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class TableSetColumnValuesCmd
///////////////////////////////////////////////////////////////////////////
TableSetColumnValuesCmd::TableSetColumnValuesCmd( TableModel * model, int col, 
	const QStringList& data, QUndoCommand * parent )
 : QUndoCommand( parent ), d_model(model), d_col(col)
{
	d_data = new StringColumnData(data);
	d_backup = new StringColumnData();
	setText(QObject::tr("set column values"));
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
	setText(QObject::tr("replace columns"));
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

