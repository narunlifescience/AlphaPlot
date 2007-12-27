/***************************************************************************
    File                 : Double2StringFilter.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs@gmx.net
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
#include <QChar>

//! Locale-aware conversion filter double -> QString.
class Double2StringFilter : public AbstractSimpleFilter
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

		//! Return the data type of the column
		virtual SciDAVis::ColumnDataType dataType() const { return SciDAVis::TypeQString; }

	private:
		//! Format character as in QString::number 
		char d_format;
		//! Display digits or precision as in QString::number  
		int d_digits;


		//! \name XML related functions
		//@{
		//! Save the column as XML
		virtual void save(QXmlStreamWriter * writer) const
		{
			writer->writeStartElement("simple_filter");
			writer->writeAttribute("filter_name", "Double2StringFilter");
			writer->writeAttribute("format", QString(QChar(numericFormat())));
			writer->writeAttribute("digits", QString::number(numDigits()));
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
				if(str != "Double2StringFilter")
					reader->raiseError(prefix+tr("incompatible filter type")+postfix);
				
				if(!reader->error())
				{
					QString format_str = attribs.value(reader->namespaceUri().toString(), "format").toString();
					QString digits_str = attribs.value(reader->namespaceUri().toString(), "digits").toString();
					bool ok;
					int digits = digits_str.toInt(&ok);
					if( (format_str.size() != 1) || !ok )
						reader->raiseError(prefix+tr("missing format attribute(s)")+postfix);
					else
					{
						setNumericFormat( format_str.at(0).toAscii() );
						setNumDigits( digits );
					}
				}
				reader->readNext(); // read the end element
			}
			else
				reader->raiseError(prefix+tr("no simple filter element found")+postfix);

			return !reader->error();
		}
		//@}

	public:
		virtual QString textAt(int row) const {
			if (!d_inputs.value(0)) return QString();
			if (d_inputs.value(0)->rowCount() <= row) return QString();
			return QLocale().toString(d_inputs.value(0)->valueAt(row), d_format, d_digits);
		}

	protected:
		//! Using typed ports: only double inputs are accepted.
		virtual bool inputAcceptable(int, AbstractColumn *source) {
			return source->dataType() == SciDAVis::TypeDouble;
		}
};

#endif // ifndef DOUBLE2STRING_FILTER_H

