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
#include <QSettings>
#include "ui_NotesConfigPage.h"

NotesConfigPage::NotesConfigPage() 
{
	ui = new Ui_NotesConfigPage();
	ui->setupUi(this);
}

NotesConfigPage::~NotesConfigPage() 
{
	delete ui;
}

void NotesConfigPage::apply()
{
	// TODO: read setting from ui and change them in Notes
}

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

void NotesModule::initActionManager()
{
	Notes::initActionManager();
}

ConfigPageWidget * NotesModule::makeConfigPage()
{
	return new NotesConfigPage();
}
		
QString NotesModule::configPageLabel()
{
	return QObject::tr("Notes");
}

void NotesModule::loadSettings()
{
#ifdef Q_OS_MAC // Mac
	QSettings settings(QSettings::IniFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#else
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#endif

	settings.beginGroup("Notes");
	settings.endGroup();
}

void NotesModule::saveSettings()
{
#ifdef Q_OS_MAC // Mac
	QSettings settings(QSettings::IniFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#else
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#endif

	settings.beginGroup("Notes");
	settings.endGroup();
}

Q_EXPORT_PLUGIN2(scidavis_notes, NotesModule)
