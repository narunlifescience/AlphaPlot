/***************************************************************************
    File                 : ColumnPrivate.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Private members for Class Column

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

#include "ColumnPrivate.h"
#include "Column.h"
#include "AbstractFilter.h"
#include <QString>

ColumnPrivate::ColumnPrivate()
{
	d_data_type = SciDAVis::UndefinedType;
	d_column_mode = SciDAVis::UndefinedMode;
	d_data = 0;
}

ColumnPrivate::~ColumnPrivate()
{
	if(d_data) delete d_data;
}

void ColumnPrivate::setColumnMode(SciDAVis::ColumnMode mode)
{
	if(mode == column_mode) return;

	void * old_data = d_data;
	// remark: the deletion of the old data will be done in the dtor of a command

	AbstractFilter *filter, *new_in_filter, *new_out_filter;
	Column * temp_col = 0;

	// determine the conversion filter and allocate the new data vector
	switch(d_data_type)
	{
		case SciDAVis::TypeDouble:
			switch(mode)
			{		
				case SciDAVis::Numeric:
					break;
				case SciDAVis::Text:
					filter = new Double2StringFilter();
					d_data = new QStringList();
					d_data_type = SciDAVis::TypeQString;
					temp_col = new Column(*(static_cast< QStringList* >(old_data)), d_validity);
					break;
				case SciDAVis::Date:
				case SciDAVis::Time:
				case SciDAVis::DateTime:
					filter = new Double2DateTimeFilter();
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					temp_col = new Column(*(static_cast< QList<QDatetime>* >(old_data)), d_validity);
					break;
				case SciDAVis::Month:
					filter = new Double2MonthFilter();
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					temp_col = new Column(*(static_cast< QList<QDatetime>* >(old_data)), d_validity);
					break;
				case SciDAVis::Day:
					filter = new Double2DayOfWeekFilter();
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					temp_col = new Column(*(static_cast< QList<QDatetime>* >(old_data)), d_validity);
					break;
			} // switch(mode)
			break;

		case SciDAVis::TypeQString:
			switch(mode)
			{		
				case SciDAVis::Text:
					break;
				case SciDAVis::Numeric:
					filter = new String2DoubleFilter();
					d_data = new QVector<double>();
					d_data_type = SciDAVis::TypeQDouble;
					temp_col = new Column(*(static_cast< QVector<double>* >(old_data)), d_validity);
					break;
				case SciDAVis::Date:
				case SciDAVis::Time:
				case SciDAVis::DateTime:
					filter = new String2DateTimeFilter();
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					temp_col = new Column(*(static_cast< QList<QDatetime>* >(old_data)), d_validity);
					break;
				case SciDAVis::Month:
					filter = new String2MonthFilter();
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					temp_col = new Column(*(static_cast< QList<QDatetime>* >(old_data)), d_validity);
					break;
				case SciDAVis::Day:
					filter = new String2DayOfWeekFilter();
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					temp_col = new Column(*(static_cast< QList<QDatetime>* >(old_data)), d_validity);
					break;
			} // switch(mode)
			break;

		case SciDAVis::TypeQDateTime:
			switch(mode)
			{		
				case SciDAVis::Date:
				case SciDAVis::Time:
				case SciDAVis::DateTime:
					break;
				case SciDAVis::Text:
					filter = new DateTime2StringFilter();
					d_data = new QStringList();
					d_data_type = SciDAVis::TypeQString;
					temp_col = new Column(*(static_cast< QStringList* >(old_data)), d_validity);
					break;
				case SciDAVis::Numeric:
					filter = new DateTime2DoubleFilter();
					d_data = new QVector<double>();
					d_data_type = SciDAVis::TypeQDouble;
					temp_col = new Column(*(static_cast< QVector<double>* >(old_data)), d_validity);
					break;
				case SciDAVis::Month:
				case SciDAVis::Day:
					break;
			} // switch(mode)
	}

	// determine the new input and output filters
	switch(mode)
	{		
		case SciDAVis::Numeric:
			new_in_filter = new String2DoubleFilter();
			new_out_filter = new Double2StringFilter();
			break;
		case SciDAVis::Text:
			new_in_filter = new CopyThroughFilter();
			new_out_filter = new CopyThroughFilter();
			break;
		case SciDAVis::Date:
		case SciDAVis::Time:
		case SciDAVis::DateTime:
			new_in_filter = new String2DateTimeFilter();
			new_out_filter = new DateTime2StringFilter();
			break;
		case SciDAVis::Month:
			new_in_filter = new String2MonthFilter();
			new_out_filter = new DateTime2StringFilter();
			static_cast<DateTime2StringFilter *>(new_out_filter)->setFormat("MMMM");
			break;
		case SciDAVis::Day:
			new_in_filter = new String2DayOfWeekFilter();
			new_out_filter = new DateTime2StringFilter();
			static_cast<DateTime2StringFilter *>(new_out_filter)->setFormat("dddd");
			break;
	} // switch(mode)


	if(d_column_mode != SciDAVis::UndefinedMode)
		emit modeAboutToChange();
	
	if(temp_col) // if temp_col == 0, only the input/output filters need to be changed
	{
		// copy the filtered, i.e. converted, column
		filter->input(0, temp_col);
		copy(filter->output(0));
		delete temp_col;
	}

	d_in_filter = new_in_filter;
	d_out_filter = new_out_filter;
	// remark: the deletion of the old filters will be done in the dtor of a command

	if(d_column_mode != SciDAVis::UndefinedMode)
	{
		d_column_mode = mode;
		emit modeChanged();
	}
	else
		d_column_mode = mode;
}

bool ColumnPrivate::copy(const AbstractColumn * other)
{
	int num_rows = other->rowCount();

	if(num_rows > rowCount())
	{
		emit dataAboutToChange(this);
		expand(num_rows - rowCount()); 
	}
	else
	{
		removeRows(num_rows, rowCount() - num_rows);
		emit dataAboutToChange(this);
	}

	// copy the data
	switch(d_data_type)
	{
		case SciDAVis::TypeDouble:
			double * ptr = static_cast< QVector<double>* >(d_data)->data();
			for(int i=0; i<num_rows; i++)
				ptr[i] = other->valueAt(i);
			break;
		case SciDAVis::TypeQString:
			for(int i=0; i<num_rows; i++)
				static_cast< QStringList >(d_data)->replace(i, other->textAt(i));
			break;
		case SciDAVis::TypeQDateTime:
			for(int i=0; i<num_rows; i++)
				static_cast< QList<QDateTime>* >(d_data)->replace(i, other->dateTimeAt(i));
			break;
	}
	// copy the validity information
	d_validity = other->invalidIntervals();

	emit dataChanged(this);

	return true;
}

bool ColumnPrivate::copy(const AbstractColumn * source, int source_start, int dest_start, int num_rows)
{
	if(num_rows == 0) return;

	emit dataAboutToChange(this);
	if(dest_start + num_rows > rowCount())
		expand(dest_start + num_rows - rowCount()); 

	// copy the data
	switch(d_data_type)
	{
		case SciDAVis::TypeDouble:
			double * ptr = static_cast< QVector<double>* >(d_data)->data();
			for(int i=0; i<num_rows; i++)
				ptr[dest_start+i] = source->valueAt(source_start + i);
			break;
		case SciDAVis::TypeQString:
			for(int i=0; i<num_rows; i++)
				static_cast< QStringList >(d_data)->replace(dest_start+i, source->textAt(source_start + i));
			break;
		case SciDAVis::TypeQDateTime:
			for(int i=0; i<num_rows; i++)
				static_cast< QList<QDateTime>* >(d_data)->replace(dest_start+i, source->dateTimeAt(source_start + i));
			break;
	}
	// copy the validity information
	for(int i=0; i<num_rows; i++)
		setInvalid(dest_start+i, source->isInvalid(source_start+i));

	emit dataChanged(this);

	return true;
}

int ColumnPrivate::rowCount() const
{
	switch(data_type)
	{
		case SciDAVis::TypeDouble:
			return static_cast< QVector<double>* >(d_data)->size();
		case SciDAVis::TypeQDateTime:
			return static_cast< QList<QDateTime>* >(d_data)->size();
		case SciDAVis::TypeQString:
			return static_cast< QStringList* >(d_data)->size();
	}

	return 0;
}

void ColumnPrivate::expand(int new_rows)
{
	if(new_rows < 1) return;

	switch(data_type)
	{
		case SciDAVis::TypeDouble:
			static_cast< QVector<double>* >(d_data)->resize(rowCount() + new_rows);
			break;
		case SciDAVis::TypeQDateTime:
			for(int i=0; i<new_rows; i++)
				static_cast< QList<QDateTime>* >(d_data)->append(QDateTime());
			break;
		case SciDAVis::TypeQString:
			for(int i=0; i<new_rows; i++)
				static_cast< QStringList* >(d_data)->append(QString());
			break;
	}
	// Remark: this does not change the logical
	// number of rows, therefore no changes
	// to the interval attributes here
}

void ColumnPrivate::insertEmptyRows(int before, int count)
{
	if(data_type == SciDAVis::UndefinedType) return;

	emit rowsAboutToBeInserted(this, before, count);
	switch(data_type)
	{
		case SciDAVis::TypeDouble:
			static_cast< QVector<double>* >(d_data)->insert(before, count, 0.0);
			break;
		case SciDAVis::TypeQDateTime:
			for(int i=0; i<count; i++)
				static_cast< QList<QDateTime>* >(d_data)->insert(before, QDateTime());
			break;
		case SciDAVis::TypeQString:
			for(int i=0; i<count; i++)
				static_cast< QStringList* >(d_data)->insert(before, QString());
			break;
	}
	d_validity.insertRows(before, count);
	d_selection.insertRows(before, count);
	d_masking.insertRows(before, count);
	d_formulas.insertRows(before, count);
	emit rowsInserted(this, before, count);
}

void ColumnPrivate::removeRows(int first, int count)
{
	if(data_type == SciDAVis::UndefinedType) return;

	emit rowsAboutToBeDeleted(this, first, count);
	switch(data_type)
	{
		case SciDAVis::TypeDouble:
			static_cast< QVector<double>* >(d_data)->remove(first, count);
			break;
		case SciDAVis::TypeQDateTime:
			for(int i=0; i<count; i++)
				static_cast< QList<QDateTime>* >(d_data)->removeAt(first);
			break;
		case SciDAVis::TypeQString:
			for(int i=0; i<count; i++)
				static_cast< QStringList* >(d_data)->removeAt(first);
			break;
	}
	d_validity.removeRows(first, count);
	d_selection.removeRows(first, count);
	d_masking.removeRows(first, count);
	d_formulas.removeRows(first, count);
	emit rowsDeleted(this, first, count);
}

void ColumnPrivate::setPlotDesignation(SciDAVis::PlotDesignation pd)
{
	emit plotDesignationAboutToChange(this);
	d_plot_designation = pd; 
	emit plotDesignationChanged(this);
}

void ColumnPrivate::clear()
{
	removeRows(0, size());
}

void ColumnPrivate::clearValidity()
{
	emit dataAboutToChange(this);	
	d_validity.clear();
	emit dataChanged(this);	
}

void ColumnPrivate::clearMasks()
{
	emit maskingAboutToChange(this);	
	d_masking.clear();
	emit maskingChanged(this);	
}

void ColumnPrivate::setInvalid(Interval<int> i, bool invalid = true)
{
	emit dataAboutToChange(this);	
	d_validity.setValue(i, invalid);
	emit dataChanged(this);	
}

void ColumnPrivate::setInvalid(int row, bool invalid = true)
{
	setInvalid(Interval<int>(row,row), invalid);
}

void ColumnPrivate::setMasked(Interval<int> i, bool mask = true)
{
		emit selectionAboutToChange(this);	
		d_selection.setValue(i, select);
		emit selectionChanged(this);	
}

void ColumnPrivate::setMasked(int row, bool mask = true)
{
	setMasked(Interval<int>(row,row), mask);
}

void ColumnPrivate::setFormula(Interval<int> i, QString formula)
{
	d_formulas.setValue(i, formula);
}

void ColumnPrivate::setFormula(int row, QString formula)
{
	setFormula(Interval<int>(row,row), formula);
}

void ColumnPrivate::clearFormulas()
{
	d_formulas.clear();
}


QString Column::textAt(int row) const
{
	if(d_data_type != SciDAVis::TypeQString) return QString();
	return static_cast< QStringList* >(d_data)->value(row);
}

QDate Column::dateAt(int row) const
{
	return dateTimeAt(row).date();
}

QTime Column::timeAt(int row) const
{
	return dateTimeAt(row).time();
}

QDateTime Column::dateTimeAt(int row) const
{
	if(d_data_type != SciDAVis::TypeQDateTime) return QDateTime();
	return static_cast< QList<QDateTime>* >(d_data)->value(row);
}

double Column::valueAt(int row) const
{
	if(d_data_type != SciDAVis::TypeDouble) return 0.0;
	return static_cast< QVector<double>* >(d_data)->value(row);
}

void ColumnPrivate::setTextAt(int row, QString new_value)
{
	if(d_data_type != SciDAVis::TypeQString) return;

	emit dataAboutToChange(this);
	if(row => rowCount())
		expand(row+1 - rowCount()); 

	static_cast< QStringList* >(d_data)->replace(i, new_value);
	emit dataChanged(this);
}

void ColumnPrivate::replaceTexts(int first, QStringList new_values)
{
	if(data_type != SciDAVis::TypeQString) return;
	
	emit dataAboutToChange(this);
	num_rows = new_values.size();
	if (first + num_rows > rowCount())
		expand(first + num_rows - rowCount());

	for(int i=0; i<num_rows; i++)
		static_cast< QStringList* >(d_data)->replace(first+i, new_values.at(i));
	emit dataChanged(this);
}

void ColumnPrivate::setDateAt(int row, QDate new_value)
{
	if(data_type != SciDAVis::TypeQDateTime) return;

	setDateTimeAt(QDateTime(new_value, timeAt(row)));
}

void ColumnPrivate::setTimeAt(int row, QTime new_value)
{
	if(data_type != SciDAVis::TypeQDateTime) return;
	
	setDateTimeAt(QDateTime(dateAt(row), new_value));
}

void ColumnPrivate::setDateTimeAt(int row, QDateTime new_value)
{
	if(data_type != SciDAVis::TypeQDateTime) return;

	emit dataAboutToChange(this);
	if(row => rowCount())
		expand(row+1 - rowCount()); 

	static_cast< QList<QDateTime>* >(d_data)->replace(i, new_value);
	emit dataChanged(this);
}

void ColumnPrivate::replaceDateTimes(int first, QList<QDateTime> new_values)
{
	if(data_type != SciDAVis::TypeQDateTime) return;
	
	emit dataAboutToChange(this);
	num_rows = new_values.size();
	if (first + num_rows > rowCount())
		expand(first + num_rows - rowCount());

	for(int i=0; i<num_rows; i++)
		static_cast< QList<QDateTime>* >(d_data)->replace(first+i, new_values.at(i));
	emit dataChanged(this);
}

void ColumnPrivate::setValueAt(int row, double new_value)
{
	if(data_type != SciDAVis::TypeDouble) return;

	emit dataAboutToChange(this);
	if(row => rowCount())
		expand(row+1 - rowCount()); 

	static_cast< QVector<double>* >(d_data)->replace(i, new_value);
	emit dataChanged(this);
}

void ColumnPrivate::replaceValues(int first, int num_rows, const double * new_values)
{
	if(data_type != SciDAVis::TypeDouble) return;
	
	emit dataAboutToChange(this);
	if (first + num_rows > rowCount())
		expand(first + num_rows - rowCount());

	double * ptr = static_cast< QVector<double>* >(d_data)->data();
	for(int i=0; i<num_rows; i++)
		ptr[first+i] = new_values[i];
	emit dataChanged(this);
}

