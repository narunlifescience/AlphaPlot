/***************************************************************************
    File                 : NotesModule.h
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
#ifndef NOTES_MODULE_H
#define NOTES_MODULE_H

#include "core/interfaces.h"

class NotesModule : public QObject, public PartMaker
{
	Q_OBJECT
	Q_INTERFACES(PartMaker)

	public:
		virtual AbstractPart * makePart();
		virtual QAction * makeAction(QObject *parent);
};

#endif // ifndef NOTES_MODULE_H
