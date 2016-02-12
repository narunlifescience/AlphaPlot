/***************************************************************************
    File                 : Double2DateTimeFilter.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2009 Knut Franke
    Email (use @ for *)  : Knut.Franke*gmx.net
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

#include "Double2DateTimeFilter.h"

#include <math.h>

QDateTime Double2DateTimeFilter::dateTimeAt(int row) const {
	if (!d_inputs.value(0)) return QDateTime();
	double input_value = d_inputs.value(0)->valueAt(row);
	QDateTime result;
	// This gets a little messy, since QDate can't represent the reference date of JDN.
	// Thus, we manually interpret invalid dates as JDN reference.
	switch (m_unit_interval) {
		case Year:
			if (m_date_time_0.isValid())
				result = m_date_time_0.addYears(int(floor(input_value)));
			else {
				result.setDate(QDate(int(floor(input_value))+4713, 1, 1));
				result.setTime(QTime(12,0,0,0));
			}
			return result.addMSecs(qRound64((input_value - int(floor(input_value))) *
						result.date().daysInYear() * 86400000.0));
		case Month:
			if (m_date_time_0.isValid())
				result = m_date_time_0.addMonths(int(floor(input_value)));
			else {
				result.setDate(QDate(int(floor(input_value))/12+4713, int(floor(input_value))%12+1, 1));
				result.setTime(QTime(12,0,0,0));
			}
			return result.addMSecs(qRound64((input_value - int(floor(input_value))) *
						result.date().daysInMonth() * 86400000.0));
		case Day:
			if (m_date_time_0.isValid())
				result = m_date_time_0.addDays(int(floor(input_value)));
			else {
				result.setDate(QDate::fromJulianDay(int(floor(input_value))));
				result.setTime(QTime(12,0,0,0));
			}
			return result.addMSecs(qRound64((input_value - int(floor(input_value))) * 86400000.0));
		case Hour:
			if (m_date_time_0.isValid())
				return m_date_time_0.addMSecs(qRound64(input_value * 3600000.0));
			else {
				result.setDate(QDate::fromJulianDay(int(floor(input_value)) / 24));
				result.setTime(QTime(12,0,0,0));
				result.addSecs((int(floor(input_value)) % 24) * 3600);
				return result.addMSecs(qRound64((input_value - int(floor(input_value))) * 3600000.0));
			}
		case Minute:
			if (m_date_time_0.isValid())
				return m_date_time_0.addMSecs(qRound64(input_value * 60000.0));
			else {
				result.setDate(QDate::fromJulianDay(int(floor(input_value)) / (24 * 60)));
				result.setTime(QTime(12,0,0,0));
				result.addSecs((int(floor(input_value)) % (24 * 60)) * 60);
				return result.addMSecs(qRound64((input_value - int(floor(input_value))) * 60000.0));
			}
		case Second:
			if (m_date_time_0.isValid())
				return m_date_time_0.addMSecs(qRound64(input_value * 1000.0));
			else {
				result.setDate(QDate::fromJulianDay(int(floor(input_value)) / (24 * 60 * 60)));
				result.setTime(QTime(12,0,0,0));
				result.addSecs(int(floor(input_value)) % (24 * 60 * 60));
				return result.addMSecs(qRound64((input_value - int(floor(input_value))) * 1000.0));
			}
		case Millisecond:
			if (m_date_time_0.isValid())
				return m_date_time_0.addMSecs(qRound64(input_value));
			else {
				result.setDate(QDate::fromJulianDay(int(floor(input_value)) / (24 * 60 * 60)));
				result.setTime(QTime(12,0,0,0));
				result.addSecs(int(floor(input_value)) % (24 * 60 * 60));
				return result.addMSecs(qRound64((input_value - int(floor(input_value))) * 1000.0));
			}
	}
        return result;
}

