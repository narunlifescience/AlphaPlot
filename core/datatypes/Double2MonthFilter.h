/***************************************************************************
    File                 : Double2MonthFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Benkert
    Email (use @ for *)  : knut.franke*gmx.de, thzs@gmx.net
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

#include "../AbstractSimpleFilter.h"
#include <QDateTime>
#include <math.h>
#include "lib/XmlStreamReader.h"
#include <QXmlStreamWriter>

//! Conversion filter double -> QDateTime, interpreting the input numbers as months of the year.
class Double2MonthFilter : public AbstractSimpleFilter
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
			double input_value = d_inputs.value(0)->valueAt(row);
			// Don't use Julian days here since support for years < 1 is bad
			// Use 1900-01-01 instead
			QDate result_date = QDate(1900,1,1).addMonths(qRound(input_value - 1.0));
			QTime result_time = QTime(0,0,0,0);
			return QDateTime(result_date, result_time);
		}

		//! Return the data type of the column
		virtual SciDAVis::ColumnDataType dataType() const { return SciDAVis::TypeQDateTime; }

		//! \name XML related functions
		//@{
		//! Save the column as XML
		virtual void save(QXmlStreamWriter * writer) const
		{
			writer->writeStartElement("simple_filter");
			writer->writeAttribute("filter_name", "Double2MonthFilter");
			writer->writeEndElement();
		}
		//! Load the column from XML
		virtual bool load(XmlStreamReader * reader)
		{
			QString prefix(tr("XML read error: ","prefix for XML error messages"));
			QString postfix(tr(" (loading failed)", "postfix for XML error messages"));

			if(reader->isStartElement() && reader->name() == "simple_filter") 
			{
				QXmlStreamAttributes attribs = reader->attributes();
				QString str = attribs.value(reader->namespaceUri().toString(), "filter_name").toString();
				if(str != "Double2MonthFilter")
					reader->raiseError(prefix+tr("incompatible filter type")+postfix);
				reader->readNext(); // read the end element
			}
			else
				reader->raiseError(prefix+tr("no simple filter element found")+postfix);

			return !reader->error();
		}
		//@}

	protected:
		virtual bool inputAcceptable(int, AbstractColumn *source) {
			return source->dataType() == SciDAVis::TypeDouble;
		}
};

#endif // ifndef DOUBLE2MONTH_FILTER_H

