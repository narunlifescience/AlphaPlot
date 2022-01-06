/***************************************************************************
   File                 : NumericDateTimeBaseFilter.cpp
       Project              : AlphaPlot
       --------------------------------------------------------------------
       Copyright            : (C) 2021 by Suthiro
       Email (use @ for *)  : soviet_man*list.ru
       Description          : Base for conversion filters double <-> QDateTime
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

#include "NumericDateTimeBaseFilter.h"

#include <math.h>

const QDateTime NumericDateTimeBaseFilter::zeroOffsetDate =
    QDateTime::fromMSecsSinceEpoch(0);
static const double milliSecondsInDay = 86400000.0;

void NumericDateTimeBaseFilter::writeExtraAttributes(
    QXmlStreamWriter *writer) const {
  writer->writeAttribute("base_datetime",
                         QString::number(m_date_time_0.toMSecsSinceEpoch()));
  writer->writeAttribute("unit",
                         QString::number(static_cast<int>(m_unit_interval)));
}

bool NumericDateTimeBaseFilter::load(XmlStreamReader *reader) {
  QXmlStreamAttributes attribs = reader->attributes();
  QString base_datetimeStr =
      attribs.value(reader->namespaceUri().toString(), "base_datetime")
          .toString();
  QString unitStr =
      attribs.value(reader->namespaceUri().toString(), "unit").toString();
  if (AbstractSimpleFilter::load(reader)) {
    bool ok;
    UnitInterval unit = static_cast<UnitInterval>(unitStr.toInt(&ok));
    if (!ok) {
      reader->raiseError(
          tr("NumericDateTimeBaseFilter: invalid unit, defaulting to years"));
      unit = UnitInterval::Year;
    }
    setUnitInterval(unit);
    qint64 msecs = base_datetimeStr.toLongLong(&ok);
    if (!ok) {
      reader->raiseError(
          tr("NumericDateTimeBaseFilter: invalid offset, defaulting to zero"));
      msecs = 0;
    }
    QDateTime base_datetime = QDateTime::fromMSecsSinceEpoch(msecs);
    setBaseDateTime(base_datetime);
  } else
    return false;

  return !reader->hasError();
}

void NumericDateTimeBaseFilter::setUnitInterval(const UnitInterval unit) {
  // if unit is valid, assign and return
  switch (unit) {
    case UnitInterval::Day:
    case UnitInterval::Hour:
    case UnitInterval::Millisecond:
    case UnitInterval::Minute:
    case UnitInterval::Month:
    case UnitInterval::Second:
    case UnitInterval::Year:
      m_unit_interval = unit;
      return;
  }
  // if not, assign default value
  m_unit_interval = UnitInterval::Day;
}

void NumericDateTimeBaseFilter::setBaseDateTime(const QDateTime &date) {
  if (date.isValid())
    m_date_time_0 = date;
  else
    m_date_time_0 = zeroOffsetDate;
}

double NumericDateTimeBaseFilter::offsetToDouble(
    const QDateTime &tDateTime) const {
  double retVal = 0.0;
  switch (m_unit_interval) {
    case UnitInterval::Year: {
      // brutto
      int years = tDateTime.date().year() - m_date_time_0.date().year();
      // consider base date 2020/12/15 and tDateTime 2022/01/01, got 2 years in
      // the line above but in fact it is only one full year + some days,
      // therefore value is to be corrected
      if (tDateTime < m_date_time_0.addYears(years)) years -= 1;
      retVal += years;
      auto previousYearOffset = m_date_time_0.addYears(years);
      auto daysToNextYear =
          previousYearOffset.daysTo(previousYearOffset.addYears(1));
      retVal += previousYearOffset.msecsTo(tDateTime) / daysToNextYear /
                milliSecondsInDay;
      return retVal;
    }
    case UnitInterval::Month: {
      int months = tDateTime.date().month() - m_date_time_0.date().month();
      if (tDateTime < m_date_time_0.addMonths(months)) months -= 1;
      retVal += months;
      auto previousMonthsOffset = m_date_time_0.addMonths(months);
      auto daysToNextMonth =
          previousMonthsOffset.daysTo(previousMonthsOffset.addMonths(1));
      retVal += previousMonthsOffset.msecsTo(tDateTime) / daysToNextMonth /
                milliSecondsInDay;
      return retVal;
    }
    case UnitInterval::Day: {
      int days = tDateTime.date().day() - m_date_time_0.date().day();
      if (tDateTime < m_date_time_0.addDays(days)) days -= 1;
      retVal += days;
      retVal +=
          m_date_time_0.addDays(days).msecsTo(tDateTime) / milliSecondsInDay;
      return retVal;
    }
    case UnitInterval::Hour:
      return m_date_time_0.msecsTo(tDateTime) / 1000.0 / 3600.0;
    case UnitInterval::Minute:
      return m_date_time_0.msecsTo(tDateTime) / 1000.0 / 60.0;
    case UnitInterval::Second:
      return m_date_time_0.msecsTo(tDateTime) / 1000.0;
    case UnitInterval::Millisecond:
      return m_date_time_0.msecsTo(tDateTime);
  }
  // return default value
  return retVal;
}

QDateTime NumericDateTimeBaseFilter::makeDateTime(double input_value) const {
  QDateTime dateTime = m_date_time_0;
  qint64 fullUnits(floor(input_value));
  double residual{0.0};
  switch (m_unit_interval) {
    // add full number of units to dateTime
    case UnitInterval::Year: {
      dateTime = dateTime.addYears(fullUnits);
      auto daysToNextYear = dateTime.daysTo(dateTime.addYears(1));
      residual = qRound64((input_value - fullUnits) * daysToNextYear *
                          milliSecondsInDay);
      break;
    }
    case UnitInterval::Month: {
      dateTime = dateTime.addMonths(fullUnits);
      auto daysToNextMonth = dateTime.daysTo(dateTime.addMonths(1));
      residual = qRound64((input_value - fullUnits) * daysToNextMonth *
                          milliSecondsInDay);
      break;
    }
    case UnitInterval::Day: {
      dateTime = dateTime.addDays(fullUnits);
      residual = qRound64((input_value - fullUnits) * milliSecondsInDay);
      break;
    }
    case UnitInterval::Hour: {
      residual = qRound64(input_value * 3600 * 1000.0);
      break;
    }
    case UnitInterval::Minute: {
      residual = qRound64(input_value * 60 * 1000.0);
      break;
    }
    case UnitInterval::Second: {
      residual = qRound64(input_value * 1000.0);
      break;
    }
    case UnitInterval::Millisecond: {
      residual = qRound64(input_value);
      break;
    }
  }
  // add the residual difference in dates as milliseconds
  return dateTime.addMSecs(residual);
}
