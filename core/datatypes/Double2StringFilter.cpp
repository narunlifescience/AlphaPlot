/***************************************************************************
    File                 : Double2StringFilter.cpp
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

#include "Double2StringFilter.h"

void Double2StringFilter::save(QXmlStreamWriter * writer) const
{
	writer->writeStartElement("simple_filter");
	writer->writeAttribute("filter_name", "Double2StringFilter");
	writer->writeAttribute("format", QString(QChar(numericFormat())));
	writer->writeAttribute("digits", QString::number(numDigits()));
	writer->writeEndElement();
}

bool Double2StringFilter::load(QXmlStreamReader * reader)
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

void Double2StringFilter::setNumericFormat(char format) 
{ 
	Double2StringFilterSetFormatCmd * cmd = new Double2StringFilterSetFormatCmd(shared_from_this(), format);
	QUndoStack * stack;
	if(d_owner_aspect && (stack = d_owner_aspect->undoStack()) )
			stack->push(cmd);
	else 
	{
		cmd->redo();
		delete cmd;
	}
}

void Double2StringFilter::setNumDigits(int digits) 
{ 
	Double2StringFilterSetDigitsCmd * cmd = new Double2StringFilterSetDigitsCmd(shared_from_this(), digits);
	QUndoStack * stack;
	if(d_owner_aspect && (stack = d_owner_aspect->undoStack()) )
			stack->push(cmd);
	else 
	{
		cmd->redo();
		delete cmd;
	}
}

Double2StringFilterSetFormatCmd::Double2StringFilterSetFormatCmd(shared_ptr<Double2StringFilter> target, char new_format)
	: d_target(target), d_other_format(new_format) 
{
	if(d_target->ownerAspect())
		setText(QObject::tr("%1: set numeric format to '%2'").arg(d_target->ownerAspect()->name()).arg(new_format));
	else
		setText(QObject::tr("set numeric format to '%1'").arg(new_format));
}

void Double2StringFilterSetFormatCmd::redo() 
{
	char tmp = d_target->d_format;
	d_target->d_format = d_other_format;
	d_other_format = tmp;
	emit d_target->formatChanged();
}

void Double2StringFilterSetFormatCmd::undo() 
{ 
	redo(); 
}

Double2StringFilterSetDigitsCmd::Double2StringFilterSetDigitsCmd(shared_ptr<Double2StringFilter> target, int new_digits)
	: d_target(target), d_other_digits(new_digits) 
{
	if(d_target->ownerAspect())
		setText(QObject::tr("%1: set decimal digits to %2").arg(d_target->ownerAspect()->name()).arg(new_digits));
	else
		setText(QObject::tr("set decimal digits to %1").arg(new_digits));
}

void Double2StringFilterSetDigitsCmd::redo() 
{
	int tmp = d_target->d_digits;
	d_target->d_digits = d_other_digits;
	d_other_digits = tmp;
	emit d_target->formatChanged();
}

void Double2StringFilterSetDigitsCmd::undo() 
{ 
	redo(); 
}



