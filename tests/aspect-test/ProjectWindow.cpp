/***************************************************************************
    File                 : ProjectWindow.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
    Description          : Standard view on a Project; main window.

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
#include "ProjectWindow.h"

#include "Project.h"
#include "AspectTreeModel.h"
#include "MdiSubWindow.h"
#include "ProjectExplorer.h"

#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QMdiArea>
#include <QDockWidget>
#include <QApplication>
#include <QUndoStack>
#include <QUndoView>

ProjectWindow::ProjectWindow(shared_ptr<Project> project)
	: d_project(project)
{
	setAttribute(Qt::WA_DeleteOnClose);

	QAction *quit_action = new QAction(tr("&Quit"), this);
	quit_action->setShortcut(tr("Ctrl+Q"));
	connect(quit_action, SIGNAL(triggered(bool)), qApp, SLOT(closeAllWindows()));

	QMenu *file_menu = menuBar()->addMenu(tr("&File"));
	file_menu->addAction(quit_action);

	QMenu *undo_menu = menuBar()->addMenu(tr("&Undo/Redo test"));
	undo_menu->addAction(d_project->undoStack()->createUndoAction(this));
	undo_menu->addAction(d_project->undoStack()->createRedoAction(this));

	d_mdi = new QMdiArea(this);
	d_mdi->setProperty("scrollBarsEnabled", true);
	setCentralWidget(d_mdi);

	QDockWidget *explorer_win = new QDockWidget(this);
	explorer_win->setWindowTitle(tr("Project Explorer"));
	ProjectExplorer *explorer = new ProjectExplorer(explorer_win);
	explorer->setModel(new AspectTreeModel(d_project, this));
	explorer_win->setWidget(explorer);
	addDockWidget(Qt::BottomDockWidgetArea, explorer_win);

	// experimenting with some QTreeView features
	explorer->setAnimated(true);
	explorer->setAlternatingRowColors(true);

	d_mdi->addSubWindow(new QUndoView(d_project->undoStack()))->setWindowTitle(tr("History"));

	connect(d_project->abstractAspectSignalEmitter(), SIGNAL(aspectDescriptionChanged(AbstractAspect *)), 
		this, SLOT(aspectDescriptionChanged(AbstractAspect *)));
	connect(d_project->abstractAspectSignalEmitter(), SIGNAL(aspectAdded(AbstractAspect *, int)), 
		this, SLOT(aspectAdded(AbstractAspect *, int)));

	aspectDescriptionChanged(d_project.get());
}

ProjectWindow::~ProjectWindow()
{
	disconnect(d_project->abstractAspectSignalEmitter(), 0, this, 0);
}

void ProjectWindow::aspectDescriptionChanged(AbstractAspect *aspect)
{
	if (aspect != static_cast<AbstractAspect *>(d_project.get())) return;
	setWindowTitle(d_project->caption() + " - SciDAVis");
}

void ProjectWindow::aspectAdded(AbstractAspect *parent, int index)
{
	shared_ptr<AbstractAspect> aspect = parent->child(index);
	QWidget *view = aspect->view();
	if (!view) return;
	d_mdi->addSubWindow(new MdiSubWindow(aspect, view));
	view->show();
}

