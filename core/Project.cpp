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
#include "ScriptingEngineManager.h"
#include <QUndoStack>
#include <QString>
#include <QKeySequence>
#include <QMenu>

#define NOT_IMPL (QMessageBox::information(0, "info", "not yet implemented"))

class Project::Private
{
	public:
		Private() : mdi_window_visibility(folderOnly), primary_view(0), scripting_engine(0) {}
		~Private() {
			if (primary_view) delete primary_view;
		}
		QUndoStack undo_stack;
		MdiWindowVisibility mdi_window_visibility;
		ProjectWindow * primary_view;
		AbstractScriptingEngine * scripting_engine;
};

Project::Project()
	: Folder(tr("Unnamed")), d(new Private())
{
	// TODO: intelligent engine choosing
	Q_ASSERT(ScriptingEngineManager::instance()->engineNames().size() > 0);
	QString engine_name = ScriptingEngineManager::instance()->engineNames()[0];
	d->scripting_engine = ScriptingEngineManager::instance()->engine(engine_name);
}
	
Project::~Project()
{
	delete d;
}

QUndoStack *Project::undoStack() const
{
	return &d->undo_stack;
}

ProjectWindow *Project::view()
{
	if (!d->primary_view)
		d->primary_view = new ProjectWindow(this);
	return d->primary_view;
}

QMenu *Project::createContextMenu() const
{
	QMenu * menu = AbstractAspect::createContextMenu();
	Q_ASSERT(menu);
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
	if(d->mdi_window_visibility == Project::folderOnly) action->setChecked(true);
	action = new QAction(tr("Current Folder and &Subfolders"), policy_action_group);
	action->setCheckable(true);
	action->setData(Project::folderAndSubfolders);
	if(d->mdi_window_visibility == Project::folderAndSubfolders) action->setChecked(true);
	action = new QAction(tr("&Manual showing/hiding"), policy_action_group);
	action->setCheckable(true);
	action->setData(Project::manual);
	if(d->mdi_window_visibility == Project::manual) action->setChecked(true);
	connect(policy_action_group, SIGNAL(triggered(QAction*)), this, SLOT(setMdiWindowVisibility(QAction*)));
	win_policy_menu->addActions(policy_action_group->actions());
	menu->addMenu(win_policy_menu);
	menu->addSeparator();
	
	// --- 
	// New Aspect ->
	// ----
	//
	menu->addAction(QPixmap(), QObject::tr("&Properties"), this, SLOT(showProperties()) );

	return menu;
}

void Project::setMdiWindowVisibility(QAction * action) 
{
	setMdiWindowVisibility((MdiWindowVisibility)(action->data().toInt()));
}
		
void Project::setMdiWindowVisibility(MdiWindowVisibility visibility)
{ 
	d->mdi_window_visibility = visibility; 
	view()->updateMdiWindowVisibility();
}
		
Project::MdiWindowVisibility Project::mdiWindowVisibility() const 
{ 
	return d->mdi_window_visibility; 
}

AbstractScriptingEngine * Project::scriptingEngine() const
{
	return d->scripting_engine;
}
