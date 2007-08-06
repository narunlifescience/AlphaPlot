/***************************************************************************
    File                 : StringColumnData.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Data source that stores a list of strings (implementation)

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

#include "StringColumnData.h"
#include <QLocale>

StringColumnData::StringColumnData()
{
}

StringColumnData::StringColumnData(const QStringList& list)
{ 
	*(static_cast< QStringList* >(this)) = list; 
}

bool StringColumnData::copy(const AbstractDataSource * other)
{
	const AbstractStringDataSource *other_as_string = qobject_cast<const AbstractStringDataSource*>(other);
	if (!other_as_string) return false;

	int count = other_as_string->rowCount();

	if(count > rowCount())
	{
		emit dataAboutToChange(this);
		expand(count - rowCount()); 
	}
	else
	{
		removeRows(count, rowCount() - count);
		emit dataAboutToChange(this);
	}

	for(int i=0; i<count; i++)
	{
		replace(i, other_as_string->textAt(i));
		setInvalid(i, other_as_string->isInvalid(i));
	}
	emit dataChanged(this);
	return true;
}

bool StringColumnData::copy(const AbstractDataSource * source, int source_start, int dest_start, int num_rows)
{
	const AbstractStringDataSource *source_as_string = qobject_cast<const AbstractStringDataSource*>(source);
	if (!source_as_string) return false;

	emit dataAboutToChange(this);
	if (dest_start + num_rows > rowCount())
		expand(dest_start + num_rows - rowCount());

	for(int i=0; i<num_rows; i++)
	{
		replace(dest_start+i, source_as_string->textAt(source_start+i));
		setInvalid(dest_start+i, source_as_string->isInvalid(source_start+i));
	}
	emit dataChanged(this);
	return true;
}

int StringColumnData::rowCount() const 
{ 
	return size(); 
}

QString StringColumnData::label() const
{ 
	return d_label;
}

QString StringColumnData::comment() const
{ 
	return d_comment;
}

AbstractDataSource::PlotDesignation StringColumnData::plotDesignation() const
{ 
	return d_plot_designation;
}

void StringColumnData::setLabel(const QString& label) 
{ 
	emit descriptionAboutToChange(this);
	d_label = label; 
	emit descriptionChanged(this);
}

void StringColumnData::setComment(const QString& comment) 
{ 
	emit descriptionAboutToChange(this);
	d_comment = comment; 
	emit descriptionChanged(this);
}

void StringColumnData::setPlotDesignation(AbstractDataSource::PlotDesignation pd) 
{ 
	emit plotDesignationAboutToChange(this);
	d_plot_designation = pd; 
	emit plotDesignationChanged(this);
}

void StringColumnData::notifyReplacement(AbstractDataSource * replacement)
{
	emit aboutToBeReplaced(this, replacement); 
}

void StringColumnData::expand(int new_rows)
{
	for(int i=0; i<new_rows; i++)
		*this << QString();
	// Remark: this does not change the logical
	// number of rows, therefore no changes
	// to the interval attributes here
}

void StringColumnData::insertEmptyRows(int before, int count)
{
	emit rowsAboutToBeInserted(this, before, count);
	for(int i=0; i<count; i++)
		insert(before, QString());
	d_validity.insertRows(before, count);
	d_selection.insertRows(before, count);
	d_masking.insertRows(before, count);
	d_formulas.insertRows(before, count);
	emit rowsInserted(this, before, count);
}

void StringColumnData::removeRows(int first, int count)
{
	emit rowsAboutToBeDeleted(this, first, count);
	for(int i=0; i<count; i++)
		removeAt(first);
	d_validity.removeRows(first, count);
	d_selection.removeRows(first, count);
	d_masking.removeRows(first, count);
	d_formulas.removeRows(first, count);
	emit rowsDeleted(this, first, count);
}

QString StringColumnData::textAt(int row) const 
{ 
		return value(row);
}

void StringColumnData::clear()
{
	removeRows(0, size());
}

