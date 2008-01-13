/***************************************************************************
    File                 : ProjectWindow.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
                           some parts written 2004-2007 by Ion Vasilief
                           (from former ApplicationWindow class)
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
#include "Table.h"

#include <QAction>
#include <QMenuBar>
#include <QMenu>
#include <QMdiArea>
#include <QDockWidget>
#include <QToolBar>
#include <QApplication>
#include <QUndoStack>
#include <QUndoView>
#include <QToolButton>

ProjectWindow::ProjectWindow(shared_ptr<Project> project)
	: d_project(project)
{
	init();
}

void ProjectWindow::init()
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowIcon(QIcon(":/appicon"));

	d_mdi_area = new QMdiArea(this);
	setCentralWidget(d_mdi_area);

	initDockWidgets();
	// initActions() needs to be called after initialization of dock windows,
	// because we now use QDockWidget::toggleViewAction()
	initActions();
	initMenus();
	initToolBars();
	
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
	d_mdi_area->addSubWindow(new MdiSubWindow(aspect, view));
	view->show();
}

void ProjectWindow::initDockWidgets()
{
	// project explorer
	d_project_explorer_dock = new QDockWidget(this);
	d_project_explorer_dock->setWindowTitle(tr("Project Explorer"));
	d_project_explorer = new ProjectExplorer(d_project_explorer_dock);
	d_project_explorer->setModel(new AspectTreeModel(d_project, this));
	d_project_explorer->setAnimated(true);
	d_project_explorer->setAlternatingRowColors(true);
	d_project_explorer_dock->setWidget(d_project_explorer);
	addDockWidget(Qt::BottomDockWidgetArea, d_project_explorer_dock);

	// undo history
	d_history_dock = new QDockWidget(this);
	d_history_dock->setWindowTitle(tr("History"));
	d_undo_view = new QUndoView(d_project->undoStack());
	d_history_dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	d_history_dock->setWidget(d_undo_view);
	addDockWidget(Qt::RightDockWidgetArea, d_history_dock);
}

void ProjectWindow::initActions()
{
	d_actions.quit = new QAction(tr("&Quit"), this);
	d_actions.quit->setShortcut(tr("Ctrl+Q"));
	connect(d_actions.quit, SIGNAL(triggered(bool)), qApp, SLOT(closeAllWindows()));
		
	d_actions.new_table = new QAction(tr("New &Table"), this);
	d_actions.new_table->setIcon(QIcon(QPixmap(":/table.xpm")));
//	d_actions.new_table->setShortcut(d_project->queryShortcut("new table"));
	connect(d_actions.new_table, SIGNAL(triggered(bool)), this, SLOT(addNewTable()));

	d_actions.undo = d_project->undoStack()->createUndoAction(this);
	d_actions.undo->setIcon(QIcon(QPixmap(":/undo.xpm")));
	d_actions.undo->setShortcut(d_project->queryShortcut("undo"));

	d_actions.redo = d_project->undoStack()->createRedoAction(this);
	d_actions.redo->setIcon(QIcon(QPixmap(":/redo.xpm")));
	d_actions.redo->setShortcut(d_project->queryShortcut("redo"));
}

void ProjectWindow::initMenus()
{
	d_menus.file = menuBar()->addMenu(tr("&File"));
	d_menus.new_aspect = d_menus.file->addMenu(tr("&New"));
	d_menus.new_aspect->addAction(d_actions.new_table);

	d_menus.file->addAction(d_actions.quit);

	d_menus.edit = menuBar()->addMenu(tr("&Edit"));
	d_menus.edit->addAction(d_actions.undo);
	d_menus.edit->addAction(d_actions.redo);
	d_menus.edit->addSeparator();

}

void ProjectWindow::initToolBars()
{
	d_toolbars.file = new QToolBar( tr( "File" ), this );
	d_toolbars.file->setObjectName("file_toolbar"); // this is needed for QMainWindow::restoreState()
	addToolBar( Qt::TopToolBarArea, d_toolbars.file );
	
	d_buttons.new_aspect = new QToolButton(this);
	d_buttons.new_aspect->setMenu(d_menus.new_aspect);
	d_buttons.new_aspect->setPopupMode(QToolButton::InstantPopup);
	d_buttons.new_aspect->setIcon(QPixmap(":/new_aspect.xpm"));
	d_buttons.new_aspect->setToolTip(tr("New Aspect"));
	d_toolbars.file->addWidget(d_buttons.new_aspect);
}

void ProjectWindow::addNewTable()
{
	d_project->addChild(shared_ptr<Table>(new Table(0, 20, 2, "table1")));
}


