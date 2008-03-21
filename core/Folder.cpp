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
}

