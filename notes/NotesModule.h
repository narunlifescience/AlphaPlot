/***************************************************************************
    File                 : NotesModule.h
    Project              : SciDAVis
    Description          : Module providing the notes Part and support classes.
    --------------------------------------------------------------------
    Copyright            : (C) 2008 Knut Franke (knut.franke*gmx.de)
    Copyright            : (C) 2008 Tilman Benkert (thzs*gmx.net)
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
#include "notes/Notes.h"

class NotesModule : public QObject, public PartMaker, public ActionManagerOwner, public ConfigPageMaker
{
	Q_OBJECT
	Q_INTERFACES(PartMaker ActionManagerOwner ConfigPageMaker)

	public:
		virtual AbstractPart * makePart();
		virtual QAction * makeAction(QObject *parent);
		virtual ActionManager * actionManager() { return Notes::actionManager(); }
		virtual void initActionManager();
		virtual ConfigPageWidget * makeConfigPage();
		virtual QString configPageLabel();
		virtual void loadSettings();
		virtual void saveSettings();
};

class Ui_NotesConfigPage;

//! Helper class for NotesModule
class NotesConfigPage : public ConfigPageWidget
{
	Q_OBJECT

	public:
		NotesConfigPage();
		~NotesConfigPage();

	public slots:
		virtual void apply();

	private:
		Ui_NotesConfigPage *ui;
};

#endif // ifndef NOTES_MODULE_H
