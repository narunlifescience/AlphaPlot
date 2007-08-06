/***************************************************************************
    File                 : Double2MonthFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Conversion filter double -> QDateTime, interpreting
                           the input numbers as months of the year.
                           
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
#ifndef DOUBLE2MONTH_FILTER_H
#define DOUBLE2MONTH_FILTER_H

#include "AbstractSimpleFilter.h"
#include <QDateTime>
#include <math.h>

//! Conversion filter double -> QDateTime, interpreting the input numbers as months of the year.
class Double2MonthFilter : public AbstractSimpleFilter<QDateTime>
{
	Q_OBJECT
	public:
		virtual QDate dateAt(int row) const {
			return dateTimeAt(row).date();
		}
		virtual QTime timeAt(int row) const {
			return dateTimeAt(row).time();
		}
		virtual QDateTime dateTimeAt(int row) const {
			if (!d_inputs.value(0)) return QDateTime();
			double input_value = doubleInput()->valueAt(row);
			// Don't use Julian days here since support for years < 1 is bad
			// Use 1900-01-01 instead
			QDate result_date = QDate(1900,1,1).addMonths(int(input_value) - 1);
			QTime result_time = QTime(0,0,0,0);
			return QDateTime(result_date, result_time);
		}

	protected:
		virtual bool inputAcceptable(int, AbstractDataSource *source) {
			return source->inherits("AbstractDoubleDataSource");
		}
};

#endif // ifndef DOUBLE2MONTH_FILTER_H

