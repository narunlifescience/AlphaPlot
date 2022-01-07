/***************************************************************************
    File                 : String2DoubleFilter.h
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Locale-aware conversion filter QString -> double.

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
#ifndef STRING2DOUBLE_FILTER_H
#define STRING2DOUBLE_FILTER_H

#include <QLocale>
#include <QXmlStreamWriter>
#include <QtDebug>
#include <QSettings>

#include "../AbstractSimpleFilter.h"
#include "lib/XmlStreamReader.h"

//! Locale-aware conversion filter QString -> double.
class String2DoubleFilter : public AbstractSimpleFilter {
  Q_OBJECT

 public:
  String2DoubleFilter() {}
  virtual double valueAt(int row) const {
    if (!d_inputs.value(0)) return 0;
    double val = std::numeric_limits<double>::quiet_NaN();
    convertToDouble(d_inputs.value(0)->textAt(row), val);
    return val;
  }
  virtual bool isInvalid(int row) const {
    if (!d_inputs.value(0)) return false;
    double val = std::numeric_limits<double>::quiet_NaN();
    return !convertToDouble(d_inputs.value(0)->textAt(row), val);
  }
  virtual bool isInvalid(Interval<int> i) const {
    if (!d_inputs.value(0)) return false;
    double val = std::numeric_limits<double>::quiet_NaN();
    QLocale locale = getLocale();
    bool allowForeignSeparator = isAnyDecimalSeparatorAllowed();
    for (int row = i.start(); row <= i.end(); row++) {
      if (convertToDouble(d_inputs.value(0)->textAt(row), val, locale,
                          allowForeignSeparator))
        return false;
    }
    return true;
  }
  virtual QList<Interval<int>> invalidIntervals() const {
    IntervalAttribute<bool> validity;
    if (d_inputs.value(0)) {
      int rows = d_inputs.value(0)->rowCount();
      for (int i = 0; i < rows; i++) validity.setValue(i, isInvalid(i));
    }
    return validity.intervals();
  }

  //! Checks if it is possible to convert an input QString to number
  bool isInvalid(const QString &str) const {
    double val = std::numeric_limits<double>::quiet_NaN();
    return !convertToDouble(str, val);
  }

  //! Return the data type of the column
  virtual AlphaPlot::ColumnDataType dataType() const {
    return AlphaPlot::TypeDouble;
  }

 protected:
  //! Using typed ports: only string inputs are accepted.
  virtual bool inputAcceptable(int, const AbstractColumn *source) {
    return source->dataType() == AlphaPlot::TypeString;
  }

 private:
  QLocale getLocale() const {
    return QLocale();  // new QLocale instance in case it was changed between
                       // calls
  }

  bool isAnyDecimalSeparatorAllowed() const {
    QSettings settings;
    settings.beginGroup("General");
    return settings.value("LocaleUseGroupSeparator").toBool();
  }

  // convenience overload
  bool convertToDouble(const QString &str, double &value) const {
    return convertToDouble(str, value, getLocale(),
                           isAnyDecimalSeparatorAllowed());
  }

  bool convertToDouble(const QString &str, double &value, const QLocale &locale,
                       const bool accept_any_decimal_separator) const {
    bool ok;
    auto tstr = QString(str);
    if (accept_any_decimal_separator) {
      QChar decimalSeparator =
          locale.decimalPoint();  // get the decimal separator for this locale
      QChar foreignSeparator =
          decimalSeparator;  // safeguard initialization just in case
                             // there are other decimal separators.
      if ('.' == decimalSeparator) foreignSeparator = ',';
      if (',' == decimalSeparator) foreignSeparator = '.';

      tstr.replace(foreignSeparator, decimalSeparator);
    }
    value = locale.toDouble(tstr, &ok);

    return ok;
  }
};

#endif  // STRING2DOUBLE_FILTER_H
