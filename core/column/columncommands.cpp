/***************************************************************************
    File                 : columncommands.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Commands to change columns
                           
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

#include "columncommands.h"

///////////////////////////////////////////////////////////////////////////
// class ColumnSetModeCmd
///////////////////////////////////////////////////////////////////////////
	ColumnSetModeCmd::ColumnSetModeCmd(Column * col, SciDAVis::ColumnMode mode, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_mode(mode)
{
	setText(QObject::tr("change mode of column %1").arg(col->d->columnLabel()));
	d_undone = false;
	d_executed = false;
}

ColumnSetModeCmd::~ColumnSetModeCmd()
{
	if(d_undone)
	{
		if(d_new_in_filter != d_old_in_filter)	
			delete d_new_in_filter;
		if(d_new_out_filter != d_old_out_filter)	
			delete d_new_out_filter;
		if(d_new_data != d_old_data)
		{
			if(d_new_type == SciDAVis::TypeDouble)
				delete static_cast< QVector<double>* >(d_new_data);
			else if(d_new_type == SciDAVis::TypeQString)
				delete static_cast< QStringList* >(d_new_data);
			else if(d_new_type == SciDAVis::TypeQDateTime)
				delete static_cast< QList<QDateTime>* >(d_new_data);
		}
	}
	else
	{
		if(d_new_in_filter != d_old_in_filter)	
			delete d_old_in_filter;
		if(d_new_out_filter != d_old_out_filter)	
			delete d_old_out_filter;
		if(d_new_data != d_old_data)
		{
			{
				if(d_old_type == SciDAVis::TypeDouble)
					delete static_cast< QVector<double>* >(d_old_data);
				else if(d_old_type == SciDAVis::TypeQString)
					delete static_cast< QStringList* >(d_old_data);
				else if(d_old_type == SciDAVis::TypeQDateTime)
					delete static_cast< QList<QDateTime>* >(d_old_data);
			}
		}
	}

}

void ColumnSetModeCmd::redo()
{
	if(!d_executed)
	{
		// save old values
		d_old_mode = d_col->d->columnMode();	
		d_old_type = d_col->d->dataType();
		d_old_data = d_col->d->dataPointer();
		d_old_in_filter = d_col->d->inputFilter();
		d_old_out_filter = d_col->d->outputFilter();
		d_old_validity = d_col->d->validityAttribute();

		// do conversion
		d_col->d->setColumnMode(d_mode);

		// save new values
		d_new_type = d_col->d->dataType();
		d_new_data = d_col->d->dataPointer();
		d_new_in_filter = d_col->d->inputFilter();
		d_new_out_filter = d_col->d->outputFilter();
		d_new_validity = d_col->d->validityAttribute();
		d_executed = true;
	}
	else
	{
		// set to saved new values
		d_col->d->replaceModeData(d_mode, d_new_type, d_new_data, d_new_in_filter, d_new_out_filter, d_new_validity);
	}
	d_undone = false;
}

void ColumnSetModeCmd::undo()
{
	// reset to old values
	d_col->d->replaceModeData(d_old_mode, d_old_type, d_old_data, d_old_in_filter, d_old_out_filter, d_old_validity);

	d_undone = true;
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetModeCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnFullCopyCmd
///////////////////////////////////////////////////////////////////////////
	ColumnFullCopyCmd::ColumnFullCopyCmd(Column * col, const AbstractColumn * src, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_src(src)
{
	setText(QObject::tr("copy values into column %1").arg(col->d->columnLabel()));
	d_backup = 0;
}

ColumnFullCopyCmd::~ColumnFullCopyCmd()
{
	delete d_backup;
}

void ColumnFullCopyCmd::redo()
{
	if(!d_backup)
	{
		d_backup = new Column("backup", d_src->columnMode());
		d_backup->d->copy(d_col);
		d_col->d->copy(d_src);
	}
	else
	{
		// swap data + validity of orig. column and backup
		IntervalAttribute<bool> val_temp = d_col->d->validityAttribute();
		void * data_temp = d_col->d->dataPointer();
		d_col->d->replaceData(d_backup->d->dataPointer(), d_backup->d->validityAttribute());
		d_backup->d->replaceData(data_temp, val_temp);
	}
}

void ColumnFullCopyCmd::undo()
{
	// swap data + validity of orig. column and backup
	IntervalAttribute<bool> val_temp = d_col->d->validityAttribute();
	void * data_temp = d_col->d->dataPointer();
	d_col->d->replaceData(d_backup->d->dataPointer(), d_backup->d->validityAttribute());
	d_backup->d->replaceData(data_temp, val_temp);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnFullCopyCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnPartialCopyCmd
///////////////////////////////////////////////////////////////////////////
	ColumnPartialCopyCmd::ColumnPartialCopyCmd(Column * col, const AbstractColumn * src, int src_start, int dest_start, int num_rows, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_src(src), d_src_start(src_start), d_dest_start(dest_start), d_num_rows(num_rows)
{
	setText(QObject::tr("copy values into column %1").arg(col->d->columnLabel()));
	d_col_backup = 0;
	d_src_backup = 0;
}

ColumnPartialCopyCmd::~ColumnPartialCopyCmd()
{
	delete d_col_backup;
	delete d_src_backup;
}

void ColumnPartialCopyCmd::redo()
{
	if(!d_src_backup)
	{
		// copy the relevant rows of source and destination column into backup columns
		d_src_backup = new Column("d_src_backup", d_col->d->columnMode());
		d_src_backup->d->copy(d_src, d_src_start, 0, d_num_rows);
		d_col_backup = new Column("d_col_backup", d_col->d->columnMode());
		d_col_backup->d->copy(d_col, d_dest_start, 0, d_num_rows);
	}
	d_col->d->copy(d_src_backup, 0, d_dest_start, d_num_rows);
}

void ColumnPartialCopyCmd::undo()
{
	d_col->d->copy(d_col_backup, 0, d_dest_start, d_num_rows);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnPartialCopyCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnInsertEmptyRowsCmd
///////////////////////////////////////////////////////////////////////////
	ColumnInsertEmptyRowsCmd::ColumnInsertEmptyRowsCmd(Column * col, int before, int count, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_before(before), d_count(count)
{
	setText(QObject::tr("insert rows into column %1").arg(col->d->columnLabel()));
}

ColumnInsertEmptyRowsCmd::~ColumnInsertEmptyRowsCmd()
{
}

void ColumnInsertEmptyRowsCmd::redo()
{
	d_col->d->insertEmptyRows(d_before, d_count);
}

void ColumnInsertEmptyRowsCmd::undo()
{
	d_col->d->removeRows(d_before, d_count);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnInsertEmptyRowsCmd
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
// class ColumnRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////
	ColumnRemoveRowsCmd::ColumnRemoveRowsCmd(Column * col, int first, int count, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_first(first), d_count(count)
{
	setText(QObject::tr("remove rows from column %1").arg(col->d->columnLabel()));
	d_backup = 0;
}

ColumnRemoveRowsCmd::~ColumnRemoveRowsCmd()
{
	delete d_backup;
}

void ColumnRemoveRowsCmd::redo()
{
	if(!d_backup)
	{
		if(d_first + d_count > d_col->d->rowCount()) 
			d_data_row_count = d_col->d->rowCount() - d_first;
		else
			d_data_row_count = d_count;

		d_backup = new Column("d_backup", d_col->d->columnMode());
		d_backup->d->copy(d_col, d_first, 0, d_data_row_count);
		d_masking = d_col->d->maskingAttribute();
		d_formulas = d_col->d->formulaAttribute();
	}
	d_col->d->removeRows(d_first, d_count);
}

void ColumnRemoveRowsCmd::undo()
{
	d_col->d->insertEmptyRows(d_first, d_count);
	d_col->d->copy(d_backup, 0, d_first, d_data_row_count);
	if(d_data_row_count < d_count)
		d_col->d->resizeTo(d_col->d->rowCount() - (d_count - d_data_row_count));
	d_col->d->replaceMasking(d_masking);
	d_col->d->replaceFormulas(d_formulas);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnRemoveRowsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetPlotDesignationCmd
///////////////////////////////////////////////////////////////////////////
	ColumnSetPlotDesignationCmd::ColumnSetPlotDesignationCmd( Column * col, SciDAVis::PlotDesignation pd , QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_new_pd(pd)
{
	setText(QObject::tr("set plot designation for column %1").arg(col->d->columnLabel()));
}

ColumnSetPlotDesignationCmd::~ColumnSetPlotDesignationCmd()
{
}

void ColumnSetPlotDesignationCmd::redo()
{
	d_old_pd = d_col->d->plotDesignation();
	d_col->d->setPlotDesignation(d_new_pd);
}

void ColumnSetPlotDesignationCmd::undo()
{
	d_col->d->setPlotDesignation(d_old_pd);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetPlotDesignationCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnClearCmd
///////////////////////////////////////////////////////////////////////////
	ColumnClearCmd::ColumnClearCmd(Column * col, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col)
{
	setText(QObject::tr("clear column %1").arg(col->d->columnLabel()));
	d_empty_data = 0;
	d_undone = false;
}

ColumnClearCmd::~ColumnClearCmd()
{
	if(d_undone)
	{
		if(d_type == SciDAVis::TypeDouble)
			delete static_cast< QVector<double>* >(d_empty_data);
		else if(d_type == SciDAVis::TypeQString)
			delete static_cast< QStringList* >(d_empty_data);
		else if(d_type == SciDAVis::TypeQDateTime)
			delete static_cast< QList<QDateTime>* >(d_empty_data);
	}
	else
	{
		if(d_type == SciDAVis::TypeDouble)
			delete static_cast< QVector<double>* >(d_data);
		else if(d_type == SciDAVis::TypeQString)
			delete static_cast< QStringList* >(d_data);
		else if(d_type == SciDAVis::TypeQDateTime)
			delete static_cast< QList<QDateTime>* >(d_data);
	}
}

void ColumnClearCmd::redo()
{
	if(!d_empty_data)
	{
		d_type = d_col->d->dataType();
		switch(d_type)
		{
			case SciDAVis::TypeDouble:
				d_empty_data = new QVector<double>();
				break;
			case SciDAVis::TypeQDateTime:
				d_empty_data = new QList<QDateTime>();
				break;
			case SciDAVis::TypeQString:
				d_empty_data = new QStringList();
				break;
		}
		d_data = d_col->d->dataPointer();
		d_validity = d_col->d->validityAttribute();
	}
	d_col->d->replaceData(d_empty_data, IntervalAttribute<bool>());
	d_undone = false;
}

void ColumnClearCmd::undo()
{
	d_col->d->replaceData(d_data, d_validity);
	d_undone = true;
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnClearCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnClearValidityCmd
///////////////////////////////////////////////////////////////////////////
	ColumnClearValidityCmd::ColumnClearValidityCmd(Column * col, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col)
{
	setText(QObject::tr("set column %1 valid").arg(col->d->columnLabel()));
	d_copied = false;
}

ColumnClearValidityCmd::~ColumnClearValidityCmd()
{
}

void ColumnClearValidityCmd::redo()
{
	if(!d_copied)
	{
		d_validity = d_col->d->validityAttribute();
		d_copied = true;
	}
	d_col->d->clearValidity();
}

void ColumnClearValidityCmd::undo()
{
	d_col->d->replaceData(d_col->d->dataPointer(), d_validity);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnClearValidityCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnClearMasksCmd
///////////////////////////////////////////////////////////////////////////
	ColumnClearMasksCmd::ColumnClearMasksCmd(Column * col, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col)
{
	setText(QObject::tr("clear masks of column %1").arg(col->d->columnLabel()));
	d_copied = false;
}

ColumnClearMasksCmd::~ColumnClearMasksCmd()
{
}

void ColumnClearMasksCmd::redo()
{
	if(!d_copied)
	{
		d_masking = d_col->d->maskingAttribute();
		d_copied = true;
	}
	d_col->d->clearMasks();
}

void ColumnClearMasksCmd::undo()
{
	d_col->d->replaceMasking(d_masking);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnClearMasksCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetInvalidCmd
///////////////////////////////////////////////////////////////////////////
	ColumnSetInvalidCmd::ColumnSetInvalidCmd(Column * col, Interval<int> interval, bool invalid, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_interval(interval), d_invalid(invalid)
{
	if(invalid)
		setText(QObject::tr("set cells invalid"));
	else
		setText(QObject::tr("set cells valid"));
	d_copied = false;
}

ColumnSetInvalidCmd::~ColumnSetInvalidCmd()
{
}

void ColumnSetInvalidCmd::redo()
{
	if(!d_copied)
	{
		d_validity = d_col->d->validityAttribute();
		d_copied = true;
	}
	d_col->d->setInvalid(d_interval, d_invalid);
}

void ColumnSetInvalidCmd::undo()
{
	d_col->d->replaceData(d_col->d->dataPointer(), d_validity);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetInvalidCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetMaskedCmd
///////////////////////////////////////////////////////////////////////////
	ColumnSetMaskedCmd::ColumnSetMaskedCmd(Column * col, Interval<int> interval, bool masked, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_interval(interval), d_masked(masked)
{
	if(masked)
		setText(QObject::tr("mask cells"));
	else
		setText(QObject::tr("unmask cells"));
	d_copied = false;
}

ColumnSetMaskedCmd::~ColumnSetMaskedCmd()
{
}

void ColumnSetMaskedCmd::redo()
{
	if(!d_copied)
	{
		d_masking = d_col->d->maskingAttribute();
		d_copied = true;
	}
	d_col->d->setMasked(d_interval, d_masked);
}

void ColumnSetMaskedCmd::undo()
{
	d_col->d->replaceMasking(d_masking);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetMaskedCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetFormulaCmd
///////////////////////////////////////////////////////////////////////////
	ColumnSetFormulaCmd::ColumnSetFormulaCmd(Column * col, Interval<int> interval, QString formula, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_interval(interval), d_formula(formula)
{
	setText(QObject::tr("set the formula for cell(s)"));
	d_copied = false;
}

ColumnSetFormulaCmd::~ColumnSetFormulaCmd()
{
}

void ColumnSetFormulaCmd::redo()
{
	if(!d_copied)
	{
		d_formulas = d_col->d->formulaAttribute();
		d_copied = true;
	}
	d_col->d->setFormula(d_interval, d_formula);
}

void ColumnSetFormulaCmd::undo()
{
	d_col->d->replaceFormulas(d_formulas);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetFormulaCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnClearFormulasCmd
///////////////////////////////////////////////////////////////////////////
	ColumnClearFormulasCmd::ColumnClearFormulasCmd(Column * col, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col)
{
	setText(QObject::tr("clear all formulas of column %1").arg(col->d->columnLabel()));
	d_copied = false;
}

ColumnClearFormulasCmd::~ColumnClearFormulasCmd()
{
}

void ColumnClearFormulasCmd::redo()
{
	if(!d_copied)
	{
		d_formulas = d_col->d->formulaAttribute();
		d_copied = true;
	}
	d_col->d->clearFormulas();
}

void ColumnClearFormulasCmd::undo()
{
	d_col->d->replaceFormulas(d_formulas);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnClearFormulasCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetTextCmd
///////////////////////////////////////////////////////////////////////////
	ColumnSetTextCmd::ColumnSetTextCmd(Column * col, int row, const QString& new_value, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_row(row), d_new_value(new_value)
{
	setText(QObject::tr("set text for row %1 in column %2").arg(row).arg(col->d->columnLabel()));
}

ColumnSetTextCmd::~ColumnSetTextCmd()
{
}

void ColumnSetTextCmd::redo()
{
	d_old_value = d_col->d->textAt(d_row);
	d_row_count = d_col->d->rowCount();
	d_col->d->setTextAt(d_row, d_new_value);
}

void ColumnSetTextCmd::undo()
{
	d_col->d->setTextAt(d_row, d_old_value);
	d_col->d->resizeTo(d_row_count);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetTextCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetValueCmd
///////////////////////////////////////////////////////////////////////////
	ColumnSetValueCmd::ColumnSetValueCmd(Column * col, int row, double new_value, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_row(row), d_new_value(new_value)
{
	setText(QObject::tr("set value for row %1 in column %2").arg(row).arg(col->d->columnLabel()));
}

ColumnSetValueCmd::~ColumnSetValueCmd()
{
}

void ColumnSetValueCmd::redo()
{
	d_old_value = d_col->d->valueAt(d_row);
	d_row_count = d_col->d->rowCount();
	d_col->d->setValueAt(d_row, d_new_value);
}

void ColumnSetValueCmd::undo()
{
	d_col->d->setValueAt(d_row, d_old_value);
	d_col->d->resizeTo(d_row_count);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetValueCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnSetDateTimeCmd
///////////////////////////////////////////////////////////////////////////
	ColumnSetDateTimeCmd::ColumnSetDateTimeCmd(Column * col, int row, const QDateTime& new_value, QUndoCommand * parent )
: QUndoCommand( parent ), d_col(col), d_row(row), d_new_value(new_value)
{
	setText(QObject::tr("set value for row %1 in column %2").arg(row).arg(col->d->columnLabel()));
}

ColumnSetDateTimeCmd::~ColumnSetDateTimeCmd()
{
}

void ColumnSetDateTimeCmd::redo()
{
	d_old_value = d_col->d->dateTimeAt(d_row);
	d_row_count = d_col->d->rowCount();
	d_col->d->setDateTimeAt(d_row, d_new_value);
}

void ColumnSetDateTimeCmd::undo()
{
	d_col->d->setDateTimeAt(d_row, d_old_value);
	d_col->d->resizeTo(d_row_count);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnSetDateTimeCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnReplaceTextsCmd
///////////////////////////////////////////////////////////////////////////
ColumnReplaceTextsCmd::ColumnReplaceTextsCmd(Column * col, int first, const QStringList& new_values, QUndoCommand * parent )
 : QUndoCommand( parent ), d_col(col), d_first(first), d_new_values(new_values)
{
	setText(QObject::tr("replace the texts for rows %1 to %2 in column %3").arg(first).arg(first + new_values.count() -1).arg(col->d->columnLabel()));
	d_copied = false;
}

ColumnReplaceTextsCmd::~ColumnReplaceTextsCmd()
{
}

void ColumnReplaceTextsCmd::redo()
{
	if(!d_copied)
	{
		d_old_values = static_cast< QStringList* >(d_col->d->dataPointer())->mid(d_first, d_new_values.count());
		d_row_count = d_col->d->rowCount();
		d_copied = true;
	}
	d_col->d->replaceTexts(d_first, d_new_values);
}

void ColumnReplaceTextsCmd::undo()
{
	d_col->d->replaceTexts(d_first, d_old_values);
	d_col->d->resizeTo(d_row_count);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnReplaceTextsCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnReplaceValuesCmd
///////////////////////////////////////////////////////////////////////////
ColumnReplaceValuesCmd::ColumnReplaceValuesCmd(Column * col, int first, const QVector<double>& new_values, QUndoCommand * parent )
 : QUndoCommand( parent ), d_col(col), d_first(first), d_new_values(new_values)
{
	setText(QObject::tr("replace the values for rows %1 to %2 in column %3").arg(first).arg(first + new_values.count() -1).arg(col->d->columnLabel()));
	d_copied = false;
}

ColumnReplaceValuesCmd::~ColumnReplaceValuesCmd()
{
}

void ColumnReplaceValuesCmd::redo()
{
	if(!d_copied)
	{
		d_old_values = static_cast< QVector<double>* >(d_col->d->dataPointer())->mid(d_first, d_new_values.count());
		d_row_count = d_col->d->rowCount();
		d_copied = true;
	}
	d_col->d->replaceValues(d_first, d_new_values);
}

void ColumnReplaceValuesCmd::undo()
{
	d_col->d->replaceValues(d_first, d_old_values);
	d_col->d->resizeTo(d_row_count);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnReplaceValuesCmd
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// class ColumnReplaceDateTimesCmd
///////////////////////////////////////////////////////////////////////////
ColumnReplaceDateTimesCmd::ColumnReplaceDateTimesCmd(Column * col, int first, const QList<QDateTime>& new_values, QUndoCommand * parent )
 : QUndoCommand( parent ), d_col(col), d_first(first), d_new_values(new_values)
{
	setText(QObject::tr("replace the values for rows %1 to %2 in column %3").arg(first).arg(first + new_values.count() -1).arg(col->d->columnLabel()));
	d_copied = false;
}

ColumnReplaceDateTimesCmd::~ColumnReplaceDateTimesCmd()
{
}

void ColumnReplaceDateTimesCmd::redo()
{
	if(!d_copied)
	{
		d_old_values = static_cast< QList<QDateTime>* >(d_col->d->dataPointer())->mid(d_first, d_new_values.count());
		d_row_count = d_col->d->rowCount();
		d_copied = true;
	}
	d_col->d->replaceDateTimes(d_first, d_new_values);
}

void ColumnReplaceDateTimesCmd::undo()
{
	d_col->d->replaceDateTimes(d_first, d_old_values);
	d_col->d->resizeTo(d_row_count);
}

///////////////////////////////////////////////////////////////////////////
// end of class ColumnReplaceDateTimesCmd
///////////////////////////////////////////////////////////////////////////



