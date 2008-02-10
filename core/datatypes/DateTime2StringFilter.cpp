/***************************************************************************
    File                 : DateTime2StringFilter.cpp
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

#include "DateTime2StringFilter.h"

void DateTime2StringFilter::setFormat(const QString& format) 
{ 
	DateTime2StringFilterSetFormatCmd * cmd = new DateTime2StringFilterSetFormatCmd(
			static_pointer_cast<DateTime2StringFilter>(shared_from_this()), format);
	QUndoStack * stack;
	if(d_owner_aspect && (stack = d_owner_aspect->undoStack()) )
			stack->push(cmd);
	else 
	{
		cmd->redo();
		delete cmd;
	}
}

DateTime2StringFilterSetFormatCmd::DateTime2StringFilterSetFormatCmd(shared_ptr<DateTime2StringFilter> target, const QString &new_format)
	: d_target(target), d_other_format(new_format) 
{
	if(d_target->ownerAspect())
		setText(QObject::tr("%1: set date-time format to %2").arg(d_target->ownerAspect()->name()).arg(new_format));
	else
		setText(QObject::tr("set date-time format to %1").arg(new_format));
}

void DateTime2StringFilterSetFormatCmd::redo() 
{
	QString tmp = d_target->d_format;
	d_target->d_format = d_other_format;
	d_other_format = tmp;
	emit d_target->formatChanged();
}

void DateTime2StringFilterSetFormatCmd::undo() 
{ 
	redo(); 
}

void DateTime2StringFilter::save(QXmlStreamWriter * writer) const
{
	writer->writeStartElement("simple_filter");
	writer->writeAttribute("filter_name", "DateTime2StringFilter");
	writer->writeAttribute("format", format());
	writer->writeEndElement();
}

bool DateTime2StringFilter::load(QXmlStreamReader * reader)
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
