/***************************************************************************
    File                 : DateTime2StringFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : thzs*gmx.net, knut.franke*gmx.de
    Description          : Conversion filter QDateTime -> QString.
                           
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
#ifndef DATE_TIME2STRING_FILTER_H
#define DATE_TIME2STRING_FILTER_H

#include "../AbstractSimpleFilter.h"
#include <QDateTime>
#include <QRegExp>

//! Conversion filter QDateTime -> QString.
class DateTime2StringFilter : public AbstractSimpleFilter
{
	Q_OBJECT

	public:
		//! Standard constructor.
		explicit DateTime2StringFilter(QString format="yyyy-MM-dd hh:mm:ss.zzz") : d_format(format) {}
		//! Set the format string to be used for conversion.
		void setFormat(QString format) { d_format = format; }
		//! Return the format string
		/**
		 * The default format string is "yyyy-MM-dd hh:mm:ss.zzz".
		 * \sa QDate::toString()
		 */
		QString format() const { return d_format; }

		//! Return the data type of the column
		virtual SciDAVis::ColumnDataType dataType() const { return SciDAVis::TypeQString; }

	private:
		//! The format string.
		QString d_format;

	public:
		virtual QString textAt(int row) const {
			if (!d_inputs.value(0)) return QString();
			QDateTime input_value = d_inputs.value(0)->dateTimeAt(row);
			if(!input_value.date().isValid() && input_value.time().isValid())
				input_value.setDate(QDate(1900,1,1));
#if QT_VERSION < 0x040302 // the bug seems to be fixed in Qt 4.3.2
			// QDate::toString produces shortened year numbers for "yyyy"
			// in violation of ISO 8601 and ambiguous with respect to "yy" format
			QString format(d_format);
			format.replace("yyyy","YYYYyyyyYYYY");
			QString result = input_value.toString(format);
			result.replace(QRegExp("YYYY(-)?(\\d\\d\\d\\d)YYYY"), "\\1\\2");
			result.replace(QRegExp("YYYY(-)?(\\d\\d\\d)YYYY"), "\\10\\2");
			result.replace(QRegExp("YYYY(-)?(\\d\\d)YYYY"), "\\100\\2");
			result.replace(QRegExp("YYYY(-)?(\\d)YYYY"), "\\1000\\2");
			return result;
#else
			return input_value.toString(d_format);
#endif
		}

		//! \name XML related functions
		//@{
		//! Save the column as XML
		virtual void save(QXmlStreamWriter * writer) const
		{
			writer->writeStartElement("simple_filter");
			writer->writeAttribute("filter_name", "DateTime2StringFilter");
			writer->writeAttribute("format", format());
			writer->writeEndElement();
		}
		//! Load the column from XML
		virtual bool load(QXmlStreamReader * reader)
		{
			QString prefix(tr("XML read error: ","prefix for XML error messages"));
			QString postfix(tr(" (loading failed)", "postfix for XML error messages"));

			if(reader->isStartElement() && reader->name() == "simple_filter") 
			{
				QXmlStreamAttributes attribs = reader->attributes();
				QString str = attribs.value(reader->namespaceUri().toString(), "filter_name").toString();
				if(str != "DateTime2StringFilter")
					reader->raiseError(prefix+tr("incompatible filter type")+postfix);
				else
					setFormat(attribs.value(reader->namespaceUri().toString(), "format").toString());
				reader->readNext(); // read the end element
			}
			else
				reader->raiseError(prefix+tr("no simple filter element found")+postfix);

			return !reader->error();
		}
		//@}

	protected:
		//! Using typed ports: only DateTime inputs are accepted.
		virtual bool inputAcceptable(int, AbstractColumn *source) {
			return source->dataType() == SciDAVis::TypeQDateTime;
		}
};

#endif // ifndef DATE_TIME2STRING_FILTER_H

