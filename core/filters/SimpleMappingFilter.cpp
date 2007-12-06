/***************************************************************************
    File                 : SimpleMappingFilter.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen
    Email                : thzs@gmx.net
    Description          : Filter that maps rows indices of a column
                           
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

#include "SimpleMappingFilter.h"
#include "Interval.h"
#include "IntervalAttribute.h"

void SimpleMappingFilter::addMapping(int src_row, int dest_row)
{
	int dest_pos = d_dest_rows.indexOf(dest_row);

	emit abstractColumnSignalEmitter()->dataAboutToChange(this);
	if(dest_pos != -1)
	{
		d_source_rows.removeAt(dest_pos);
		d_dest_rows.removeAt(dest_pos);
	}

	int src_pos = d_source_rows.indexOf(src_row);
	if(src_pos != -1)
	{
		d_source_rows.removeAt(src_pos);
		d_dest_rows.removeAt(src_pos);
	}
	d_source_rows.append(src_row);
	d_dest_rows.append(dest_row);
	emit abstractColumnSignalEmitter()->dataChanged(this);
}

void SimpleMappingFilter::removeMappingTo(int dest_row)
{
	emit abstractColumnSignalEmitter()->dataAboutToChange(this);
	int dest_pos = d_dest_rows.indexOf(dest_row);

	if(dest_pos != -1)
	{
		d_source_rows.removeAt(dest_pos);
		d_dest_rows.removeAt(dest_pos);
	}
	emit abstractColumnSignalEmitter()->dataChanged(this);
}

void SimpleMappingFilter::removeMappingFrom(int src_row)
{
	emit abstractColumnSignalEmitter()->dataAboutToChange(this);
	int src_pos = d_source_rows.indexOf(src_row);

	if(src_pos != -1)
	{
		d_source_rows.removeAt(src_pos);
		d_dest_rows.removeAt(src_pos);
	}
	emit abstractColumnSignalEmitter()->dataChanged(this);
}

void SimpleMappingFilter::clearMappings() 
{ 
	d_source_rows.clear(); 
	d_dest_rows.clear(); 
}

bool SimpleMappingFilter::isReadOnly() const 
{ 
	return d_inputs.value(0) ? d_inputs.at(0)->isReadOnly() : true; 
}

void SimpleMappingFilter::setColumnMode(SciDAVis::ColumnMode mode)
{
	if(d_inputs.value(0) && !isReadOnly()) d_inputs.at(0)->setColumnMode(mode); 
}


bool SimpleMappingFilter::copy(const AbstractColumn * source, int source_start, int dest_start, int num_rows)
{
	if(!d_inputs.value(0)) return false;
	if(isReadOnly()) return false;
	for(int i=0; i<num_rows; i++)
	{
		int index = d_dest_rows.indexOf(dest_start+i);
		if(index == -1) continue;
		if(!d_inputs.at(0)->copy(source, source_start+i, d_source_rows.at(index), 1)) return false;
	}		

	return true;
};

bool SimpleMappingFilter::copy(const AbstractColumn * other)
{
	if(!d_inputs.value(0)) return false;
	if(isReadOnly()) return false;
	return copy(other, 0, 0, other->rowCount());
}

void SimpleMappingFilter::inputRowsAboutToBeInserted(AbstractColumn * source, int before, int count)
{
	Q_UNUSED(source);
	Q_UNUSED(before);
	Q_UNUSED(count);
}

void SimpleMappingFilter::inputRowsInserted(AbstractColumn * source, int before, int count)
{
	Q_UNUSED(source);
	for(int i=0; i<d_source_rows.count(); i++)
	{
		int src_row = d_source_rows.at(i);
		if(src_row >= before) 
			d_source_rows.replace(i, src_row + count);
	}
}

void SimpleMappingFilter::inputRowsAboutToBeRemoved(AbstractColumn * source, int first, int count)
{
	Q_UNUSED(source);
	for(int i=0; i<d_source_rows.count(); i++)
	{
		int src_row = d_source_rows.at(i);
		if(src_row >= first && src_row < first+count) 
		{
			removeMappingFrom(src_row);
			i--;
		}
	}
	for(int i=0; i<d_source_rows.count(); i++)
	{
		int src_row = d_source_rows.at(i);
		if(src_row >= first) 
			d_source_rows.replace(i, src_row - count);
	}
}

void SimpleMappingFilter::inputRowsRemoved(AbstractColumn * source, int first, int count)
{
	Q_UNUSED(source);
	Q_UNUSED(first);
	Q_UNUSED(count);
}

void SimpleMappingFilter::insertRows(int before, int count)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int index = d_dest_rows.indexOf(before), src_row;
	if(index == -1 && before != rowCount()) return;

	if(before == rowCount())
	{
		index = d_dest_rows.indexOf(before-1);
		if(index == -1) return; // this happens when no mappings are defined
		src_row = d_source_rows.at(index)+1;
	}
	else
		src_row = d_source_rows.at(index);

	emit abstractColumnSignalEmitter()->rowsAboutToBeInserted(this, before, count);
	d_inputs.at(0)->insertRows(src_row, count);

	for(int i=0; i<d_dest_rows.count(); i++)
	{
		int dest_row = d_dest_rows.at(i);
		if(dest_row >= before) 
			d_dest_rows.replace(i, dest_row + count);
	}

	for(int i=0; i<count; i++)
		addMapping(src_row+i, before+i);
	emit abstractColumnSignalEmitter()->rowsInserted(this, before, count);
}

void SimpleMappingFilter::removeRows(int first, int count)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	emit abstractColumnSignalEmitter()->rowsAboutToBeRemoved(this, first, count);
	for(int i=0; i<count; i++)
	{
		int index = d_dest_rows.indexOf(first+i);
		if(index == -1) continue;
		d_inputs.at(0)->removeRows(d_source_rows.at(index), 1);
	}		
	for(int i=0; i<d_dest_rows.count(); i++)
	{
		int dest_row = d_dest_rows.at(i);
		if(dest_row >= first) 
			d_dest_rows.replace(i, dest_row - count);
	}
	emit abstractColumnSignalEmitter()->rowsRemoved(this, first, count);
}
void SimpleMappingFilter::setColumnLabel(const QString& label)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	d_inputs.at(0)->setColumnLabel(label);	
}

void SimpleMappingFilter::setColumnComment(const QString& comment)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	d_inputs.at(0)->setColumnComment(comment);	
}

void SimpleMappingFilter::setPlotDesignation(SciDAVis::PlotDesignation pd)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	d_inputs.at(0)->setPlotDesignation(pd);	
}

void SimpleMappingFilter::clear() 
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	while(rowCount() > 0)
		removeRows(d_dest_rows.at(0), 1);
}

void SimpleMappingFilter::clearValidity() 
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	for(int i=0; i<d_source_rows.count(); i++)
		d_inputs.at(0)->setInvalid(d_source_rows.at(i), false);
}

void SimpleMappingFilter::clearMasks() 
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	for(int i=0; i<d_source_rows.count(); i++)
		d_inputs.at(0)->setMasked(d_source_rows.at(i), false);
}

void SimpleMappingFilter::setInvalid(Interval<int> i, bool invalid)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	for(int c=i.start(); c<=i.end(); c++)
		setInvalid(c, invalid);
}

void SimpleMappingFilter::setInvalid(int row, bool invalid)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return;
	d_inputs.at(0)->setInvalid(d_source_rows.at(index), invalid);
}

void SimpleMappingFilter::setMasked(Interval<int> i, bool mask)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	for(int c=i.start(); c<=i.end(); c++)
		setMasked(c, mask);
}

void SimpleMappingFilter::setMasked(int row, bool mask) 
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return;
	d_inputs.at(0)->setMasked(d_source_rows.at(index), mask);
}

int SimpleMappingFilter::rowCount() const
{
	int max = -1;
	for(int i=0; i<d_dest_rows.count(); i++)
		if(d_dest_rows.at(i) > max) max = d_dest_rows.at(i);
	return max+1;
}

bool SimpleMappingFilter::isInvalid(int row) const
{
	if(!d_inputs.value(0)) return false;
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return false;
	return d_inputs.at(0)->isInvalid(d_source_rows.at(index));
}

bool SimpleMappingFilter::isInvalid(Interval<int> i) const
{
	if(!d_inputs.value(0)) return false;
	for(int c=i.start(); c<=i.end(); c++)
		if(!isInvalid(c)) return false;
	return true;
}

QList< Interval<int> > SimpleMappingFilter::invalidIntervals() const
{
	IntervalAttribute<bool> attrib;	
	for(int i=0; i<d_dest_rows.count(); i++)
		attrib.setValue(Interval<int>(d_dest_rows.at(i),d_dest_rows.at(i)), isInvalid(d_dest_rows.at(i)));
	return attrib.intervals();
}

bool SimpleMappingFilter::isMasked(int row) const
{
	if(!d_inputs.value(0)) return false;
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return false;
	return d_inputs.at(0)->isMasked(d_source_rows.at(index));
}

bool SimpleMappingFilter::isMasked(Interval<int> i) const
{
	if(!d_inputs.value(0)) return false;
	for(int c=i.start(); c<=i.end(); c++)
		if(!isMasked(c)) return false;
	return true;
}

QList< Interval<int> > SimpleMappingFilter::maskedIntervals() const
{
	IntervalAttribute<bool> attrib;	
	for(int i=0; i<d_dest_rows.count(); i++)
		attrib.setValue(Interval<int>(d_dest_rows.at(i),d_dest_rows.at(i)), isMasked(d_dest_rows.at(i)));
	return attrib.intervals();
}

QString SimpleMappingFilter::formula(int row) const
{
	if(!d_inputs.value(0)) QString();
	int index = d_dest_rows.indexOf(row);
	if(index == -1) QString();
	return d_inputs.at(0)->formula(d_source_rows.at(index));
}

QList< Interval<int> > SimpleMappingFilter::formulaIntervals() const
{
	IntervalAttribute<QString> attrib;	
	for(int i=0; i<d_dest_rows.count(); i++)
		attrib.setValue(Interval<int>(d_dest_rows.at(i),d_dest_rows.at(i)), formula(d_dest_rows.at(i)));
	return attrib.intervals();
}

void SimpleMappingFilter::setFormula(Interval<int> i, QString formula)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	for(int c=i.start(); c<=i.end(); c++)
		setFormula(c, formula);
}

void SimpleMappingFilter::setFormula(int row, QString formula)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return;
	d_inputs.at(0)->setFormula(d_source_rows.at(index), formula);
}

void SimpleMappingFilter::clearFormulas()
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	for(int i=0; i<d_source_rows.count(); i++)
		d_inputs.at(0)->setFormula(d_source_rows.at(i), QString());

}

QString SimpleMappingFilter::textAt(int row) const
{
	if(!d_inputs.value(0)) return QString();
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return QString();
	return d_inputs.at(0)->textAt(d_source_rows.at(index));
}

void SimpleMappingFilter::setTextAt(int row, const QString& new_value)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return;
	d_inputs.at(0)->setTextAt(d_source_rows.at(index), new_value);
}

void SimpleMappingFilter::replaceTexts(int first, const QStringList& new_values)
{ 
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int count = new_values.count();
	for(int i=0; i<count; i++)
		setTextAt(first+i, new_values.at(i));
}

QDate SimpleMappingFilter::dateAt(int row) const
{
	if(!d_inputs.value(0)) return QDate();
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return QDate();
	return d_inputs.at(0)->dateAt(d_source_rows.at(index));
}

void SimpleMappingFilter::setDateAt(int row, const QDate& new_value) 
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return;
	d_inputs.at(0)->setDateAt(d_source_rows.at(index), new_value);
}

QTime SimpleMappingFilter::timeAt(int row) const
{
	if(!d_inputs.value(0)) return QTime();
	int index = d_dest_rows.indexOf(row);
	if(index == -1) QTime();
	return d_inputs.at(0)->timeAt(d_source_rows.at(index));
}

void SimpleMappingFilter::setTimeAt(int row, const QTime& new_value)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return;
	d_inputs.at(0)->setTimeAt(d_source_rows.at(index), new_value);
}

QDateTime SimpleMappingFilter::dateTimeAt(int row) const
{
	if(!d_inputs.value(0)) return QDateTime();
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return QDateTime();
	return d_inputs.at(0)->dateTimeAt(d_source_rows.at(index));
}

void SimpleMappingFilter::setDateTimeAt(int row, const QDateTime& new_value)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return;
	d_inputs.at(0)->setDateTimeAt(d_source_rows.at(index), new_value);
}

void SimpleMappingFilter::replaceDateTimes(int first, const QList<QDateTime>& new_values)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int count = new_values.count();
	for(int i=0; i<count; i++)
		setDateTimeAt(first+i, new_values.at(i));
}

double SimpleMappingFilter::valueAt(int row) const 
{
	if(!d_inputs.value(0)) return 0.0;
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return 0.0;
	return d_inputs.at(0)->valueAt(d_source_rows.at(index));
}

void SimpleMappingFilter::setValueAt(int row, double new_value) 
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int index = d_dest_rows.indexOf(row);
	if(index == -1) return;
	d_inputs.at(0)->setValueAt(d_source_rows.at(index), new_value);
}

void SimpleMappingFilter::replaceValues(int first, const QVector<double>& new_values)
{
	if(!d_inputs.value(0)) return;
	if(isReadOnly()) return;
	int count = new_values.count();
	for(int i=0; i<count; i++)
		setValueAt(first+i, new_values.at(i));
}

