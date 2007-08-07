/***************************************************************************
    File                 : Double2StringFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Locale-aware conversion filter double -> QString.
                           
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
#ifndef DOUBLE2STRING_FILTER_H
#define DOUBLE2STRING_FILTER_H

#include "../AbstractSimpleFilter.h"
#include <QLocale>

//! Locale-aware conversion filter double -> QString.
class Double2StringFilter : public AbstractSimpleFilter<QString>
{
	Q_OBJECT

	public:
		//! Standard constructor.
		explicit Double2StringFilter(char format='e', int digits=6) : d_format(format), d_digits(digits) {}
		//! Set format character as in QString::number
		void setNumericFormat(char format) { d_format = format; }
		//! Set number of displayed digits
		void setNumDigits(int digits) { d_digits = digits; }
		//! Get format character as in QString::number
		char numericFormat() const { return d_format; }
		//! Get number of displayed digits
		int numDigits() const { return d_digits; }

	private:
		//! Format character as in QString::number 
		char d_format;
		//! Display digits or precision as in QString::number  
		int d_digits;

// simplified filter interface
	public:
		virtual QString textAt(int row) const {
			if (!d_inputs.value(0)) return QString();
			if (doubleInput()->rowCount() <= row) return QString();
			return QLocale().toString(doubleInput()->valueAt(row), d_format, d_digits);
		}

	protected:
		//! Using typed ports: only double inputs are accepted.
		virtual bool inputAcceptable(int, AbstractDataSource *source) {
			return source->inherits("AbstractDoubleDataSource");
		}
};

#endif // ifndef DOUBLE2STRING_FILTER_H

