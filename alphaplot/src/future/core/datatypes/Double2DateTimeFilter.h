/***************************************************************************
    File                 : Double2DateTimeFilter.h
    Project              : AlphaPlot
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

#include "NumericDateTimeBaseFilter.h"

/**
 * \brief Conversion filter double -> QDateTime.
 *
 * The equivalence of one unit defaults to a day if nothing else is specified.
 * The default reference date/time if none is specified, or an invalid one is
 * given, is the noon of January 1st, 4713 BC as per Julian Day Number
 * convention.
 */
class Double2DateTimeFilter : public NumericDateTimeBaseFilter {
  Q_OBJECT

 public:
  // The equivalence of one unit defaults to a day if nothing else is specified.
  // Default offset date is the noon of January 1st, 4713 BC as per Julian Day
  // Number convention. Double2DateTimeFilter(const UnitInterval unit =
  // UnitInterval::Day, const QDateTime& date_time_0 = zeroOffsetDate) :
  Double2DateTimeFilter(const UnitInterval unit, const QDateTime &date_time_0)
      : NumericDateTimeBaseFilter(unit, date_time_0){};

  virtual QDate dateAt(int row) const { return dateTimeAt(row).date(); }
  virtual QTime timeAt(int row) const { return dateTimeAt(row).time(); }
  virtual QDateTime dateTimeAt(int row) const {
    if (!d_inputs.value(0)) return QDateTime();
    double input_value = d_inputs.value(0)->valueAt(row);
    return makeDateTime(input_value);
  }

  //! Return the data type of the column
  virtual AlphaPlot::ColumnDataType dataType() const {
    return AlphaPlot::TypeDateTime;
  }

  //! Explicit conversion from base class using conversion ctor
  explicit Double2DateTimeFilter(const NumericDateTimeBaseFilter &numeric)
      : NumericDateTimeBaseFilter(numeric){};

 protected:
  //! Using typed ports: only double inputs are accepted.
  virtual bool inputAcceptable(int, const AbstractColumn *source) {
    return source->dataType() == AlphaPlot::TypeDouble;
  }
};

#endif  // ifndef DOUBLE2DATE_TIME_FILTER_H
