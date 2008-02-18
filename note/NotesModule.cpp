/***************************************************************************
    File                 : NotesModule.cpp
    Project              : SciDAVis
    Description          : Module providing the notes Part and support classes.
    --------------------------------------------------------------------
    Copyright            : (C) 2008 Knut Franke (knut.franke*gmx.de)
                           (replace * with @ in the email address)

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

#include "NotesModule.h"

#include "Notes.h"
#include <QAction>

AbstractPart * NotesModule::makePart()
{
	return new Notes(tr("Notes %1").arg(1));
}

QAction * NotesModule::makeAction(QObject *parent)
{
	QAction *new_notes = new QAction(tr("New &Notes"), parent);
	new_notes->setIcon(QIcon(QPixmap(":/new_note.xpm")));
	return new_notes;
}

Q_EXPORT_PLUGIN2(scidavis_notes, NotesModule)
