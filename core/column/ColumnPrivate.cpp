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
#include "core/AbstractSimpleFilter.h"
#include "core/datatypes/SimpleCopyThroughFilter.h"
#include "core/datatypes/String2DoubleFilter.h"
#include "core/datatypes/Double2StringFilter.h"
#include "core/datatypes/Double2DateTimeFilter.h"
#include "core/datatypes/Double2MonthFilter.h"
#include "core/datatypes/Double2DayOfWeekFilter.h"
#include "core/datatypes/String2DateTimeFilter.h"
#include "core/datatypes/DateTime2StringFilter.h"
#include "core/datatypes/String2MonthFilter.h"
#include "core/datatypes/String2DayOfWeekFilter.h"
#include "core/datatypes/DateTime2DoubleFilter.h"
#include "core/datatypes/DayOfWeek2DoubleFilter.h"
#include "core/datatypes/Month2DoubleFilter.h"
#include <QString>
#include <QtDebug>


ColumnPrivate::ColumnPrivate(Column * owner, SciDAVis::ColumnMode mode)
 : AbstractColumn(owner->name()), d_owner(owner)
{
	Q_ASSERT(owner != 0); // a ColumnPrivate without owner is not allowed 
					      // because the owner must become the parent aspect of the input and output filters
	d_column_mode = mode;
	switch(mode)
	{		
		case SciDAVis::Numeric:
			d_input_filter = new String2DoubleFilter();
			d_output_filter = new Double2StringFilter();
			connect(static_cast<Double2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			d_data_type = SciDAVis::TypeDouble;
			d_data = new QVector<double>();
			break;
		case SciDAVis::Text:
			d_input_filter = new SimpleCopyThroughFilter();
			d_output_filter = new SimpleCopyThroughFilter();
			d_data_type = SciDAVis::TypeQString;
			d_data = new QStringList();
			break;
		case SciDAVis::Date:
		case SciDAVis::Time:
			d_column_mode = SciDAVis::DateTime;
		case SciDAVis::DateTime:
			d_input_filter = new String2DateTimeFilter();
			d_output_filter = new DateTime2StringFilter();
			connect(static_cast<DateTime2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			d_data_type = SciDAVis::TypeQDateTime;
			d_data = new QList<QDateTime>();
			break;
		case SciDAVis::Month:
			d_input_filter = new String2MonthFilter();
			d_output_filter = new DateTime2StringFilter();
			static_cast<DateTime2StringFilter *>(d_output_filter)->setFormat("MMMM");
			connect(static_cast<DateTime2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			d_data_type = SciDAVis::TypeQDateTime;
			d_data = new QList<QDateTime>();
			break;
		case SciDAVis::Day:
			d_input_filter = new String2DayOfWeekFilter();
			d_output_filter = new DateTime2StringFilter();
			static_cast<DateTime2StringFilter *>(d_output_filter)->setFormat("dddd");
			connect(static_cast<DateTime2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			d_data_type = SciDAVis::TypeQDateTime;
			d_data = new QList<QDateTime>();
			break;
	} // switch(mode)

	d_plot_designation = SciDAVis::noDesignation;
	d_input_filter->setName("InputFilter");
	d_output_filter->setName("OutputFilter");
	d_owner->addChild(d_input_filter);
	d_owner->addChild(d_output_filter);
}

ColumnPrivate::ColumnPrivate(Column * owner, SciDAVis::ColumnDataType type, SciDAVis::ColumnMode mode, 
	void * data, IntervalAttribute<bool> validity) : AbstractColumn(owner->name()), d_owner(owner)
{
	d_data_type = type;
	d_column_mode = mode;
	d_data = data;
	d_validity = validity;

	switch(mode)
	{		
		case SciDAVis::Numeric:
			d_input_filter = new String2DoubleFilter();
			d_output_filter = new Double2StringFilter();
			connect(static_cast<Double2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			break;
		case SciDAVis::Text:
			d_input_filter = new SimpleCopyThroughFilter();
			d_output_filter = new SimpleCopyThroughFilter();
			break;
		case SciDAVis::Date:
		case SciDAVis::Time:
			d_column_mode = SciDAVis::DateTime;
		case SciDAVis::DateTime:
			d_input_filter = new String2DateTimeFilter();
			d_output_filter = new DateTime2StringFilter();
			connect(static_cast<DateTime2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			break;
		case SciDAVis::Month:
			d_input_filter = new String2MonthFilter();
			d_output_filter = new DateTime2StringFilter();
			static_cast<DateTime2StringFilter *>(d_output_filter)->setFormat("MMMM");
			connect(static_cast<DateTime2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			break;
		case SciDAVis::Day:
			d_input_filter = new String2DayOfWeekFilter();
			d_output_filter = new DateTime2StringFilter();
			static_cast<DateTime2StringFilter *>(d_output_filter)->setFormat("dddd");
			connect(static_cast<DateTime2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			break;
	} // switch(mode)

	d_plot_designation = SciDAVis::noDesignation;
	d_input_filter->setName("InputFilter");
	d_output_filter->setName("OutputFilter");
	d_owner->addChild(d_input_filter);
	d_owner->addChild(d_output_filter);
}

ColumnPrivate::~ColumnPrivate()
{
	if(!d_data) return;

	switch(d_data_type)
	{
		case SciDAVis::TypeDouble:
			delete static_cast< QVector<double>* >(d_data);
			break;

		case SciDAVis::TypeQString:
			delete static_cast< QStringList* >(d_data);
			break;
			
		case SciDAVis::TypeQDateTime:
			delete static_cast< QList<QDateTime>* >(d_data);
			break;
	} // switch(d_data_type)
}

void ColumnPrivate::setColumnMode(SciDAVis::ColumnMode mode)
{
	if(mode == d_column_mode) return;

	void * old_data = d_data;
	// remark: the deletion of the old data will be done in the dtor of a command

	AbstractSimpleFilter *filter, *new_in_filter, *new_out_filter;
	bool filter_is_temporary; // it can also become outputFilter(), which we may not delete here
	Column* temp_col = 0;

	emit d_owner->modeAboutToChange(d_owner);

	// determine the conversion filter and allocate the new data vector
	switch(d_column_mode)
	{
		case SciDAVis::Numeric:
			disconnect(static_cast<Double2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			switch(mode)
			{		
				case SciDAVis::Numeric:
					break;
				case SciDAVis::Text:
					filter = outputFilter(); filter_is_temporary = false;
					temp_col = new Column("temp_col", *(static_cast< QVector<double>* >(old_data)), d_validity);
					d_data = new QStringList();
					d_data_type = SciDAVis::TypeQString;
					break;
				case SciDAVis::Date:
				case SciDAVis::Time:
				case SciDAVis::DateTime:
					filter = new Double2DateTimeFilter(); filter_is_temporary = true;
					temp_col = new Column("temp_col", *(static_cast< QVector<double>* >(old_data)), d_validity);
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					break;
				case SciDAVis::Month:
					filter = new Double2MonthFilter(); filter_is_temporary = true;
					temp_col = new Column("temp_col", *(static_cast< QVector<double>* >(old_data)), d_validity);
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					break;
				case SciDAVis::Day:
					filter = new Double2DayOfWeekFilter(); filter_is_temporary = true;
					temp_col = new Column("temp_col", *(static_cast< QVector<double>* >(old_data)), d_validity);
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					break;
			} // switch(mode)
			break;

		case SciDAVis::Text:
			switch(mode)
			{		
				case SciDAVis::Text:
					break;
				case SciDAVis::Numeric:
					filter = new String2DoubleFilter(); filter_is_temporary = true;
					temp_col = new Column("temp_col", *(static_cast< QStringList* >(old_data)), d_validity);
					d_data = new QVector<double>();
					d_data_type = SciDAVis::TypeDouble;
					break;
				case SciDAVis::Date:
				case SciDAVis::Time:
				case SciDAVis::DateTime:
					filter = new String2DateTimeFilter(); filter_is_temporary = true;
					temp_col = new Column("temp_col", *(static_cast< QStringList* >(old_data)), d_validity);
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					break;
				case SciDAVis::Month:
					filter = new String2MonthFilter(); filter_is_temporary = true;
					temp_col = new Column("temp_col", *(static_cast< QStringList* >(old_data)), d_validity);
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					break;
				case SciDAVis::Day:
					filter = new String2DayOfWeekFilter(); filter_is_temporary = true;
					temp_col = new Column("temp_col", *(static_cast< QStringList* >(old_data)), d_validity);
					d_data = new QList<QDateTime>();
					d_data_type = SciDAVis::TypeQDateTime;
					break;
			} // switch(mode)
			break;

		case SciDAVis::Date:
		case SciDAVis::Time:
		case SciDAVis::DateTime:
		case SciDAVis::Month:
		case SciDAVis::Day:
			disconnect(static_cast<DateTime2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			switch(mode)
			{		
				case SciDAVis::Date:
				case SciDAVis::Time:
				case SciDAVis::DateTime:
					break;
				case SciDAVis::Text:
					filter = outputFilter(); filter_is_temporary = false;
					temp_col = new Column("temp_col", *(static_cast< QList<QDateTime>* >(old_data)), d_validity);
					d_data = new QStringList();
					d_data_type = SciDAVis::TypeQString;
					break;
				case SciDAVis::Numeric:
					if(d_column_mode == SciDAVis::Month)
						filter = new Month2DoubleFilter();
					else if(d_column_mode == SciDAVis::Day)
						filter = new DayOfWeek2DoubleFilter();
					else
						filter = new DateTime2DoubleFilter();
					filter_is_temporary = true;
					temp_col = new Column("temp_col", *(static_cast< QList<QDateTime>* >(old_data)), d_validity);
					d_data = new QVector<double>();
					d_data_type = SciDAVis::TypeDouble;
					break;
				case SciDAVis::Month:
				case SciDAVis::Day:
					break;
			} // switch(mode)
			break;

	}

	// determine the new input and output filters
	switch(mode)
	{		
		case SciDAVis::Numeric:
			new_in_filter = new String2DoubleFilter();
			new_out_filter = new Double2StringFilter();
			connect(static_cast<Double2StringFilter *>(new_out_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			break;
		case SciDAVis::Text:
			new_in_filter = new SimpleCopyThroughFilter();
			new_out_filter = new SimpleCopyThroughFilter();
			break;
		case SciDAVis::Date:
		case SciDAVis::Time:
		case SciDAVis::DateTime:
			new_in_filter = new String2DateTimeFilter();
			new_out_filter = new DateTime2StringFilter();
			connect(static_cast<DateTime2StringFilter *>(new_out_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			break;
		case SciDAVis::Month:
			new_in_filter = new String2MonthFilter();
			new_out_filter = new DateTime2StringFilter();
			static_cast<DateTime2StringFilter *>(new_out_filter)->setFormat("MMMM");
			connect(static_cast<DateTime2StringFilter *>(new_out_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			break;
		case SciDAVis::Day:
			new_in_filter = new String2DayOfWeekFilter();
			new_out_filter = new DateTime2StringFilter();
			static_cast<DateTime2StringFilter *>(new_out_filter)->setFormat("dddd");
			connect(static_cast<DateTime2StringFilter *>(new_out_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			break;
	} // switch(mode)

	d_column_mode = mode;
	if(mode == SciDAVis::Date || mode == SciDAVis::Time) // these values are obsolete	
		d_column_mode = SciDAVis::DateTime;

	new_in_filter->setName("InputFilter");
	new_out_filter->setName("OutputFilter");
	d_owner->removeChild(d_input_filter);
	d_owner->removeChild(d_output_filter);
	d_input_filter = new_in_filter;
	d_output_filter = new_out_filter;
	d_owner->addChild(d_input_filter);
	d_owner->addChild(d_output_filter);

	if(temp_col) // if temp_col == 0, only the input/output filters need to be changed
	{
		// copy the filtered, i.e. converted, column
		filter->input(0, temp_col);
		copy(filter->output(0));
		delete temp_col;
	}

	if (filter_is_temporary) delete filter;

	emit d_owner->modeChanged(d_owner);
}

void ColumnPrivate::replaceModeData(SciDAVis::ColumnMode mode, SciDAVis::ColumnDataType type, void * data, 
	AbstractSimpleFilter * in_filter, AbstractSimpleFilter * out_filter, IntervalAttribute<bool> validity)
{
	emit d_owner->modeAboutToChange(d_owner);
	// disconnect formatChanged()
	switch(d_column_mode)
	{
		case SciDAVis::Numeric:
			disconnect(static_cast<Double2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
		case SciDAVis::Date:
		case SciDAVis::Time:
		case SciDAVis::DateTime:
		case SciDAVis::Month:
		case SciDAVis::Day:
			disconnect(static_cast<DateTime2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
	}

	d_column_mode = mode;
	if(mode == SciDAVis::Date || mode == SciDAVis::Time) // these values are obsolete	
		d_column_mode = SciDAVis::DateTime;
	d_data_type = type;
	d_data = data;

	in_filter->setName("InputFilter");
	out_filter->setName("OutputFilter");
	d_owner->removeChild(d_input_filter);
	d_owner->removeChild(d_output_filter);
	d_input_filter = in_filter;
	d_output_filter = out_filter;
	d_owner->addChild(d_input_filter);
	d_owner->addChild(d_output_filter);

	// connect formatChanged()
	switch(d_column_mode)
	{		
		case SciDAVis::Numeric:
			connect(static_cast<Double2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			break;
		case SciDAVis::Date:
		case SciDAVis::Time:
		case SciDAVis::DateTime:
		case SciDAVis::Month:
		case SciDAVis::Day:
			connect(static_cast<DateTime2StringFilter *>(d_output_filter), SIGNAL(formatChanged()),
				this, SLOT(notifyDisplayChange()));
			break;
	} 

	d_validity = validity;
	emit d_owner->modeChanged(d_owner);
}

void ColumnPrivate::replaceData(void * data, IntervalAttribute<bool> validity)
{
	emit d_owner->dataAboutToChange(d_owner);
	d_data = data;
	d_validity = validity;
	emit d_owner->dataChanged(d_owner);
}

bool ColumnPrivate::copy(const AbstractColumn * other)
{
	if(other->dataType() != dataType()) return false;
	int num_rows = other->rowCount();

	emit d_owner->dataAboutToChange(d_owner);
	resizeTo(num_rows); 

	// copy the data
	switch(d_data_type)
	{
		case SciDAVis::TypeDouble:
			{
				double * ptr = static_cast< QVector<double>* >(d_data)->data();
				for(int i=0; i<num_rows; i++)
					ptr[i] = other->valueAt(i);
				break;
			}
		case SciDAVis::TypeQString:
			{
				for(int i=0; i<num_rows; i++)
					static_cast< QStringList* >(d_data)->replace(i, other->textAt(i));
				break;
			}
		case SciDAVis::TypeQDateTime:
			{
				for(int i=0; i<num_rows; i++)
					static_cast< QList<QDateTime>* >(d_data)->replace(i, other->dateTimeAt(i));
				break;
			}
	}
	// copy the validity information
	d_validity = other->invalidIntervals();

	emit d_owner->dataChanged(d_owner);

	return true;
}

bool ColumnPrivate::copy(const AbstractColumn * source, int source_start, int dest_start, int num_rows)
{
	if(source->dataType() != dataType()) return false;
	if(num_rows == 0) return true;

	emit d_owner->dataAboutToChange(d_owner);
	if(dest_start + num_rows > rowCount())
		resizeTo(dest_start + num_rows); 

	// copy the data
	switch(d_data_type)
	{
		case SciDAVis::TypeDouble:
			{
				double * ptr = static_cast< QVector<double>* >(d_data)->data();
				for(int i=0; i<num_rows; i++)
					ptr[dest_start+i] = source->valueAt(source_start + i);
				break;
			}
		case SciDAVis::TypeQString:
				for(int i=0; i<num_rows; i++)
					static_cast< QStringList* >(d_data)->replace(dest_start+i, source->textAt(source_start + i));
				break;
		case SciDAVis::TypeQDateTime:
				for(int i=0; i<num_rows; i++)
					static_cast< QList<QDateTime>* >(d_data)->replace(dest_start+i, source->dateTimeAt(source_start + i));
				break;
	}
	// copy the validity information
	for(int i=0; i<num_rows; i++)
		setInvalid(dest_start+i, source->isInvalid(source_start+i));

	emit d_owner->dataChanged(d_owner);

	return true;
}

int ColumnPrivate::rowCount() const
{
	switch(d_data_type)
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

void ColumnPrivate::resizeTo(int new_size)
{
	if(new_size == rowCount()) return;

	switch(d_data_type)
	{
		case SciDAVis::TypeDouble:
			static_cast< QVector<double>* >(d_data)->resize(new_size);
			break;
		case SciDAVis::TypeQDateTime:
			{
				int new_rows = new_size - rowCount();
				if(new_rows > 0)
				{
					for(int i=0; i<new_rows; i++)
						static_cast< QList<QDateTime>* >(d_data)->append(QDateTime());
				}
				else
				{
					for(int i=0; i<-new_rows; i++)
						static_cast< QList<QDateTime>* >(d_data)->removeLast();
				}
				break;
			}
		case SciDAVis::TypeQString:
			{
				int new_rows = new_size - rowCount();
				if(new_rows > 0)
				{
					for(int i=0; i<new_rows; i++)
						static_cast< QStringList* >(d_data)->append(QString());
				}
				else
				{
					for(int i=0; i<-new_rows; i++)
						static_cast< QStringList* >(d_data)->removeLast();
				}
				break;
			}
	}
}

void ColumnPrivate::insertRows(int before, int count)
{
	if(count == 0) return;

	emit d_owner->rowsAboutToBeInserted(d_owner, before, count);
	d_validity.insertRows(before, count);
	d_masking.insertRows(before, count);
	d_formulas.insertRows(before, count);

	if(before <= rowCount()) 
	{
		switch(d_data_type)
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
	}
	emit d_owner->rowsInserted(d_owner, before, count);
}

void ColumnPrivate::removeRows(int first, int count)
{
	if(count == 0) return;

	emit d_owner->rowsAboutToBeRemoved(d_owner, first, count);
	d_validity.removeRows(first, count);
	d_masking.removeRows(first, count);
	d_formulas.removeRows(first, count);

	if(first < rowCount()) 
	{
		int corrected_count = count;
		if(first + count > rowCount()) 
			corrected_count = rowCount() - first;

		switch(d_data_type)
		{
			case SciDAVis::TypeDouble:
				static_cast< QVector<double>* >(d_data)->remove(first, corrected_count);
				break;
			case SciDAVis::TypeQDateTime:
				for(int i=0; i<corrected_count; i++)
					static_cast< QList<QDateTime>* >(d_data)->removeAt(first);
				break;
			case SciDAVis::TypeQString:
				for(int i=0; i<corrected_count; i++)
					static_cast< QStringList* >(d_data)->removeAt(first);
				break;
		}
	}
	emit d_owner->rowsRemoved(d_owner, first, count);
}

void ColumnPrivate::setPlotDesignation(SciDAVis::PlotDesignation pd)
{
	emit d_owner->plotDesignationAboutToChange(d_owner);
	d_plot_designation = pd; 
	emit d_owner->plotDesignationChanged(d_owner);
}

void ColumnPrivate::clear()
{
	removeRows(0, rowCount());
}

void ColumnPrivate::clearValidity()
{
	emit d_owner->dataAboutToChange(d_owner);	
	d_validity.clear();
	emit d_owner->dataChanged(d_owner);	
}

void ColumnPrivate::clearMasks()
{
	emit d_owner->maskingAboutToChange(d_owner);	
	d_masking.clear();
	emit d_owner->maskingChanged(d_owner);	
}

void ColumnPrivate::setInvalid(Interval<int> i, bool invalid)
{
	emit d_owner->dataAboutToChange(d_owner);	
	d_validity.setValue(i, invalid);
	emit d_owner->dataChanged(d_owner);	
}

void ColumnPrivate::setInvalid(int row, bool invalid)
{
	setInvalid(Interval<int>(row,row), invalid);
}

void ColumnPrivate::setMasked(Interval<int> i, bool mask)
{
		emit d_owner->maskingAboutToChange(d_owner);	
		d_masking.setValue(i, mask);
		emit d_owner->maskingChanged(d_owner);	
}

void ColumnPrivate::setMasked(int row, bool mask)
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

QString ColumnPrivate::textAt(int row) const
{
	if(d_data_type != SciDAVis::TypeQString) return QString();
	return static_cast< QStringList* >(d_data)->value(row);
}

QDate ColumnPrivate::dateAt(int row) const
{
	return dateTimeAt(row).date();
}

QTime ColumnPrivate::timeAt(int row) const
{
	return dateTimeAt(row).time();
}

QDateTime ColumnPrivate::dateTimeAt(int row) const
{
	if(d_data_type != SciDAVis::TypeQDateTime) return QDateTime();
	return static_cast< QList<QDateTime>* >(d_data)->value(row);
}

double ColumnPrivate::valueAt(int row) const
{
	if(d_data_type != SciDAVis::TypeDouble) return 0.0;
	return static_cast< QVector<double>* >(d_data)->value(row);
}

void ColumnPrivate::setTextAt(int row, const QString& new_value)
{
	if(d_data_type != SciDAVis::TypeQString) return;

	emit d_owner->dataAboutToChange(d_owner);
	if(row >= rowCount())
		resizeTo(row+1); 

	static_cast< QStringList* >(d_data)->replace(row, new_value);
	emit d_owner->dataChanged(d_owner);
}

void ColumnPrivate::replaceTexts(int first, const QStringList& new_values)
{
	if(d_data_type != SciDAVis::TypeQString) return;
	
	emit d_owner->dataAboutToChange(d_owner);
	int num_rows = new_values.size();
	if (first + num_rows > rowCount())
		resizeTo(first + num_rows);

	for(int i=0; i<num_rows; i++)
		static_cast< QStringList* >(d_data)->replace(first+i, new_values.at(i));
	emit d_owner->dataChanged(d_owner);
}

void ColumnPrivate::setDateAt(int row, const QDate& new_value)
{
	if(d_data_type != SciDAVis::TypeQDateTime) return;

	setDateTimeAt(row, QDateTime(new_value, timeAt(row)));
}

void ColumnPrivate::setTimeAt(int row, const QTime& new_value)
{
	if(d_data_type != SciDAVis::TypeQDateTime) return;
	
	setDateTimeAt(row, QDateTime(dateAt(row), new_value));
}

void ColumnPrivate::setDateTimeAt(int row, const QDateTime& new_value)
{
	if(d_data_type != SciDAVis::TypeQDateTime) return;

	emit d_owner->dataAboutToChange(d_owner);
	if(row >= rowCount())
		resizeTo(row+1); 

	static_cast< QList<QDateTime>* >(d_data)->replace(row, new_value);
	emit d_owner->dataChanged(d_owner);
}

void ColumnPrivate::replaceDateTimes(int first, const QList<QDateTime>& new_values)
{
	if(d_data_type != SciDAVis::TypeQDateTime) return;
	
	emit d_owner->dataAboutToChange(d_owner);
	int num_rows = new_values.size();
	if (first + num_rows > rowCount())
		resizeTo(first + num_rows);

	for(int i=0; i<num_rows; i++)
		static_cast< QList<QDateTime>* >(d_data)->replace(first+i, new_values.at(i));
	emit d_owner->dataChanged(d_owner);
}

void ColumnPrivate::setValueAt(int row, double new_value)
{
	if(d_data_type != SciDAVis::TypeDouble) return;

	emit d_owner->dataAboutToChange(d_owner);
	if(row >= rowCount())
		resizeTo(row+1); 

	static_cast< QVector<double>* >(d_data)->replace(row, new_value);
	emit d_owner->dataChanged(d_owner);
}

void ColumnPrivate::replaceValues(int first, const QVector<double>& new_values)
{
	if(d_data_type != SciDAVis::TypeDouble) return;
	
	emit d_owner->dataAboutToChange(d_owner);
	int num_rows = new_values.size();
	if (first + num_rows > rowCount())
		resizeTo(first + num_rows);

	double * ptr = static_cast< QVector<double>* >(d_data)->data();
	for(int i=0; i<num_rows; i++)
		ptr[first+i] = new_values.at(i);
	emit d_owner->dataChanged(d_owner);
}

void ColumnPrivate::replaceMasking(IntervalAttribute<bool> masking)
{
	emit d_owner->maskingAboutToChange(d_owner);
	d_masking = masking;
	emit d_owner->maskingChanged(d_owner);
}

void ColumnPrivate::replaceFormulas(IntervalAttribute<QString> formulas)
{
	d_formulas = formulas;
}


QString ColumnPrivate::columnLabel() const
{ 
	return d_owner->name();
}

QString ColumnPrivate::columnComment() const
{
	return d_owner->comment();
}

void ColumnPrivate::setColumnLabel(const QString& label)
{ 
	d_owner->setName(label);
}

void ColumnPrivate::setColumnComment(const QString& comment)
{ 
	d_owner->setComment(comment);
}

void ColumnPrivate::notifyDisplayChange()
{
	emit d_owner->dataChanged(d_owner); // all cells must be repainted
	emit d_owner->descriptionChanged(d_owner); // the icon for the type changed
}
