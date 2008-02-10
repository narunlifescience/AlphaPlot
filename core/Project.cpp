/***************************************************************************
    File                 : Project.cpp
    Project              : SciDAVis
    Description          : Represents a SciDAVis project.
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
#include "ProjectWindow.h"
#include <QUndoStack>
#include <QString>
#include <QKeySequence>
#include <QMenu>

#define NOT_IMPL (QMessageBox::information(0, "info", "not yet implemented"))

class Project::Private
{
	public:
		QUndoStack undo_stack;
		//! Applicationwide keyboard shortcuts
		QHash<QString, QKeySequence> keyboard_shortcuts;

		//! Settings global in the project
		struct {
			Project::MdiWindowControlPolicy mdi_policy;
		} global_settings;
};

Project::Project()
	: Folder(tr("Unnamed")), d(new Private())
{
	setMdiWindowControlPolicy(Project::folderOnly);
}
	
Project::~Project()
{
	delete d;
}

QUndoStack *Project::undoStack() const
{
	return &d->undo_stack;
}

AspectView *Project::view()
{
	return 0;
}

ProjectWindow *Project::projectWindow(QWidget *parent)
{
	Q_UNUSED(parent);
	return new ProjectWindow(shared_from_this());
}

QKeySequence Project::queryShortcut(const QString& action_string)
{
	QString str = action_string.toLower();
	// TODO: implement a customization dialog for this

	d->keyboard_shortcuts.insert("undo", QKeySequence(QObject::tr("Ctrl+Z")));
	d->keyboard_shortcuts.insert("redo", QKeySequence(QObject::tr("Ctrl+Y")));
	
	return d->keyboard_shortcuts.value(str, QKeySequence());
	
}

QMenu *Project::createContextMenu(QMenu * append_to)
{
	QMenu * menu = append_to;
	if(!menu)
		menu = new QMenu();

	menu->addSeparator();
	// Find
	// ----
	// Append Project
	// Save Project As
	// ----
	
	menu->addAction(tr("&Show All Windows"), this, SIGNAL(showAllMdiWindows()));
	menu->addAction(tr("&Hide All Windows"), this, SIGNAL(hideAllMdiWindows()));

	QMenu * win_policy_menu = new QMenu(tr("Show &Windows"));
	QActionGroup * policy_action_group = new QActionGroup(menu);
	policy_action_group->setExclusive(true);

	QAction * action = new QAction(tr("Current &Folder Only"), policy_action_group);
	action->setCheckable(true);
	action->setData(Project::folderOnly);
	if(d->global_settings.mdi_policy == Project::folderOnly) action->setChecked(true);
	action = new QAction(tr("Current Folder and &Subfolders"), policy_action_group);
	action->setCheckable(true);
	action->setData(Project::folderAndSubfolders);
	if(d->global_settings.mdi_policy == Project::folderAndSubfolders) action->setChecked(true);
	action = new QAction(tr("&Manual showing/hiding"), policy_action_group);
	action->setCheckable(true);
	action->setData(Project::manual);
	if(d->global_settings.mdi_policy == Project::manual) action->setChecked(true);
	connect(policy_action_group, SIGNAL(triggered(QAction*)), this, SLOT(setMdiWindowControlPolicy(QAction*)));
	win_policy_menu->addActions(policy_action_group->actions());
	menu->addMenu(win_policy_menu);
	menu->addSeparator();
	
	// --- 
	// New Aspect ->
	// ----
	//
	menu->addAction(QPixmap(), QObject::tr("&Properties"), d_aspect_wrapper, SLOT(showProperties()) );

	return menu;
}

void Project::setMdiWindowControlPolicy(QAction * action) 
{
	setMdiWindowControlPolicy((Project::MdiWindowControlPolicy)(action->data().toInt()));
}
		
void Project::setMdiWindowControlPolicy(Project::MdiWindowControlPolicy policy)
{ 
	d->global_settings.mdi_policy = policy; 
	emit updateMdiWindows();
}
		
Project::MdiWindowControlPolicy Project::mdiWindowControlPolicy() const 
{ 
	return d->global_settings.mdi_policy; 
}


