/***************************************************************************
    File                 : Project.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
    Description          : Represents a SciDAVis project.

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
#include "ProjectWindow.h"
#include <QUndoStack>
#include <QString>
#include <QKeySequence>
#include <QMenu>

class Project::Private
{
	public:
		QUndoStack undo_stack;
};

Project::Project()
	: Folder(tr("Unnamed")), d(new Private())
{
}

QUndoStack *Project::undoStack() const
{
	return &d->undo_stack;
}

QWidget *Project::view(QWidget *parent)
{
	Q_UNUSED(parent);
	return new ProjectWindow(shared_from_this());
}

QKeySequence Project::queryShortcut(const QString& action_string)
{
	QString str = action_string.toLower();
	// TODO: implement a customization dialog for this

	keyboard_shortcuts.insert("undo", QKeySequence(QObject::tr("Ctrl+Z")));
	keyboard_shortcuts.insert("redo", QKeySequence(QObject::tr("Ctrl+Y")));
	
	return keyboard_shortcuts.value(str, QKeySequence());
	
}

QMenu *Project::createContextMenu(QMenu * append_to)
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addSeparator();
	menu->addAction(QPixmap(), QObject::tr("&Properties"), d_aspect_wrapper, SLOT(showProperties()) );

	return menu;
}

