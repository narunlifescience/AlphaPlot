/***************************************************************************
    File                 : Double2DateTimeFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Benkert
    Email (use @ for *)  : knut.franke*gmx.de, thzs@gmx.net
    Description          : Conversion filter double -> QDateTime                           
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
#ifndef DOUBLE2DATE_TIME_FILTER_H
#define DOUBLE2DATE_TIME_FILTER_H

#include "../AbstractSimpleFilter.h"
#include <QDateTime>
#include <QDate>
#include <QTime>
#include "lib/XmlStreamReader.h"
#include <QXmlStreamWriter>

/**
 * \brief Conversion filter double -> QDateTime.
 *
 * The equivalence of one unit defaults to a day if nothing else is specified. The default reference
 * date/time if none is specified, or an invalid one is given, is the noon of January 1st, 4713 BC
 * as per Julian Day Number convention.
 */
class Double2DateTimeFilter : public AbstractSimpleFilter
{
	Q_OBJECT

	public:
		enum UnitInterval { Year, Month, Day, Hour, Minute, Second, Millisecond };

		Double2DateTimeFilter() : m_unit_interval(Day) {};
		Double2DateTimeFilter(UnitInterval unit, QDateTime date_time_0) :
			m_unit_interval(unit),
			m_date_time_0(date_time_0) {};

		virtual QDate dateAt(int row) const {
			return dateTimeAt(row).date();
		}
		virtual QTime timeAt(int row) const {
			return dateTimeAt(row).time();
		}
		virtual QDateTime dateTimeAt(int row) const;

		//! Return the data type of the column
		virtual SciDAVis::ColumnDataType dataType() const { return SciDAVis::TypeQDateTime; }

	protected:
		//! Using typed ports: only double inputs are accepted.
		virtual bool inputAcceptable(int, const AbstractColumn *source) {
			return source->dataType() == SciDAVis::TypeDouble;
		}

	private:
		UnitInterval m_unit_interval;
		QDateTime m_date_time_0;
};

#endif // ifndef DOUBLE2DATE_TIME_FILTER_H

