/***************************************************************************
    File                 : String2DoubleFilter.h
    Project              : SciDAVis
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

#include "../AbstractSimpleFilter.h"
#include <QLocale>
#include "lib/XmlStreamReader.h"
#include <QXmlStreamWriter>

//! Locale-aware conversion filter QString -> double.
class String2DoubleFilter : public AbstractSimpleFilter
{
	Q_OBJECT

	public:
		virtual double valueAt(int row) const {
			if (!d_inputs.value(0)) return 0;
			return QLocale().toDouble(d_inputs.value(0)->textAt(row));
		}

		//! Return the data type of the column
		virtual SciDAVis::ColumnDataType dataType() const { return SciDAVis::TypeDouble; }

		//! \name XML related functions
		//@{
		//! Save the column as XML
		virtual void save(QXmlStreamWriter * writer) const
		{
			writer->writeStartElement("simple_filter");
			writer->writeAttribute("filter_name", "String2DoubleFilter");
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
				if(str != "String2DoubleFilter")
					reader->raiseError(prefix+tr("incompatible filter type")+postfix);
				reader->readNext(); // read the end element
			}
			else
				reader->raiseError(prefix+tr("no simple filter element found")+postfix);

			return !reader->error();
		}
		//@}

	protected:
		//! Using typed ports: only string inputs are accepted.
		virtual bool inputAcceptable(int, AbstractColumn *source) {
			return source->dataType() == SciDAVis::TypeQString;
		}
};

#endif // ifndef STRING2DOUBLE_FILTER_H

