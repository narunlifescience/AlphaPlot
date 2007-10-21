/***************************************************************************
    File                 : AspectModel.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
    Description          : Private model data managed by AbstractAspect.

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
#include "AspectModel.h"
#include <QRegExp>

AspectModel::AspectModel(const QString& name)
	: d_name(name), d_caption_spec("%n%C{ - }%c")
{
	d_creation_time = QDateTime::currentDateTime();
}

void AspectModel::addChild(shared_ptr<AbstractAspect> child)
{
	d_children << child;
}

void AspectModel::insertChild(int index, shared_ptr<AbstractAspect> child)
{
	d_children.insert(index, child);
}

int AspectModel::indexOfChild(const AbstractAspect *child) const
{
	for(int i=0; i<d_children.size(); i++)
		if(d_children.at(i).get() == child) return i;
	return -1;
}

void AspectModel::removeChild(shared_ptr<AbstractAspect> child)
{
	d_children.removeAll(child);
}

int AspectModel::childCount() const
{
	return d_children.count();
}

shared_ptr<AbstractAspect> AspectModel::child(int index)
{
	Q_ASSERT(index >= 0 && index <= childCount());
	return d_children.at(index);
}

QString AspectModel::name() const
{
	return d_name;
}

void AspectModel::setName(const QString &value)
{
	d_name = value;
}

QString AspectModel::comment() const
{
	return d_comment;
}

void AspectModel::setComment(const QString &value)
{
	d_comment = value;
}

QString AspectModel::captionSpec() const
{
	return d_caption_spec;
}

void AspectModel::setCaptionSpec(const QString &value)
{
	d_caption_spec = value;
}

int AspectModel::indexOfMatchingBrace(const QString &str, int start)
{
	int result = str.indexOf('}', start);
	if (result < 0)
		result = start;
	return result;
}

QString AspectModel::caption() const
{
	QString result = d_caption_spec;
	QRegExp magic("%(.)");
	for(int pos=magic.indexIn(result, 0); pos >= 0; pos=magic.indexIn(result, pos)) {
		QString replacement;
		int length;
		switch(magic.cap(1).at(0).toAscii()) {
			case '%': replacement = "%"; length=2; break;
			case 'n': replacement = d_name; length=2; break;
			case 'c': replacement = d_comment; length=2; break;
			case 't': replacement = d_creation_time.toString(); length=2; break;
			case 'C':
						 length = indexOfMatchingBrace(result, pos) - pos + 1;
						 replacement = d_comment.isEmpty() ? "" : result.mid(pos+3, length-4);
						 break;
		}
		result.replace(pos, length, replacement);
		pos += replacement.size();
	}
	return result;
}

QDateTime AspectModel::creationTime() const
{
	return d_creation_time;
}

