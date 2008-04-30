/***************************************************************************
    File                 : Folder.cpp
    Project              : SciDAVis
    Description          : Folder in a project
    --------------------------------------------------------------------
    Copyright            : (C) 2007 Tilman Hoener zu Siederdissen (thzs*gmx.net)
    Copyright            : (C) 2007 Knut Franke (knut.franke*gmx.de)
                           (replace * with @ in the email addresses) 

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
#include "Project.h"
#include "Folder.h"

#include <QIcon>
#include <QApplication>
#include <QStyle>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QPluginLoader>

Folder::Folder(const QString &name)
	: AbstractAspect(name)
{
}

Folder::~Folder()
{
}

QIcon Folder::icon() const
{
	QIcon result;
	result.addFile(":/folder_closed.xpm", QSize(), QIcon::Normal, QIcon::Off);
	result.addFile(":/folder_open.xpm", QSize(), QIcon::Normal, QIcon::On);	
	return result;
}

QMenu *Folder::createContextMenu() const
{
	if (project())
		return project()->createFolderContextMenu(this);
	return 0;
}

void Folder::resetToDefaultValues()
{
	AbstractAspect::resetToDefaultValues();
	for (int i=childCount()-1; i >= 0; i--) 
		removeChild(i);
}

void Folder::save(QXmlStreamWriter * writer) const
{
	writer->writeStartElement("folder");
	writeBasicAttributes(writer);
	writeCommentElement(writer);

	int child_count = childCount();
	for (int i=0; i<child_count; i++)
	{
		writer->writeStartElement("child_aspect");
		writer->writeAttribute("index", QString::number(i));
		child(i)->save(writer);
		writer->writeEndElement(); // "child_aspect"
	}
	writer->writeEndElement(); // "folder"
}

bool Folder::load(QXmlStreamReader * reader)
{
	QString prefix(tr("XML read error: ","prefix for XML error messages"));
	QString postfix(tr(" (loading failed)", "postfix for XML error messages"));

	if(reader->isStartElement() && reader->name() == "folder") 
	{
		resetToDefaultValues();

		if (!readBasicAttributes(reader)) return false;

		QXmlStreamAttributes attribs = reader->attributes();
		QString str;

		// read child elements
		while (!reader->atEnd()) 
		{
			reader->readNext();

			if (reader->isEndElement()) break;

			if (reader->isStartElement()) 
			{
				bool ret_val = true;
				if (reader->name() == "comment")
					ret_val = readCommentElement(reader);
				else if(reader->name() == "child_aspect")
					ret_val = readChildAspectElement(reader);
				else
					reader->readElementText(); // unknown element
				if(!ret_val)
					return false;
			} 
		}
	}
	else // no folder element
		reader->raiseError(prefix+tr("no folder element found")+postfix);

	return !reader->error();
}

bool Folder::readChildAspectElement(QXmlStreamReader * reader)
{
	QString prefix(tr("XML read error: ","prefix for XML error messages"));
	QString postfix(tr(" (loading failed)", "postfix for XML error messages"));

	bool result = false, ok;
	Q_ASSERT(reader->isStartElement() && reader->name() == "child_aspect");
	QXmlStreamAttributes attribs = reader->attributes();
	QString str;

	str = attribs.value(reader->namespaceUri().toString(), "index").toString();
	int index = str.toInt(&ok);
	if(str.isEmpty() || !ok)
	{
		reader->raiseError(prefix+tr("invalid or missing child index")+postfix);
		return false;
	}

	reader->readNext();
	if (reader->name() == "folder")
	{
		Folder * folder = new Folder(tr("Folder 1"));
		folder->load(reader);
		insertChild(folder, index);
		result = true;
	}
	else
		foreach(QObject * plugin, QPluginLoader::staticInstances()) 
		{
			XmlElementAspectMaker * maker = qobject_cast<XmlElementAspectMaker *>(plugin);
			if (maker && maker->canCreate(reader->name().toString()))
			{
				AbstractAspect * aspect = maker->createAspectFromXml(reader);
				if (aspect)
				{
					insertChild(aspect, index);
					result = true;
				}
				break;
			}
		}
	if (!result)
		reader->raiseError(prefix+tr("no plugin for aspect '%1' found").arg(reader->name().toString())+postfix);
	reader->readNext();
	Q_ASSERT(reader->isEndElement() && reader->name() == "child_element");
	return result;
}

