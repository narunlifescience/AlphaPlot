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
#include "AbstractPart.h"
#include "PartMdiView.h"
#include "ProjectExplorer.h"
#include "interfaces.h"

#include <QMenuBar>
#include <QMenu>
#include <QMdiArea>
#include <QDockWidget>
#include <QToolBar>
#include <QApplication>
#include <QUndoStack>
#include <QUndoView>
#include <QToolButton>
#include <QPluginLoader>
#include <QSignalMapper>

ProjectWindow::ProjectWindow(Project* project)
	: d_project(project)
{
	init();
}

void ProjectWindow::init()
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowIcon(QIcon(":/appicon"));

	d_mdi_area = new QMdiArea();
	setCentralWidget(d_mdi_area);
	connect(d_mdi_area, SIGNAL(subWindowActivated(QMdiSubWindow*)),
			this, SLOT(handleCurrentSubWindowChanged(QMdiSubWindow*)));
	d_current_aspect = d_project;
	d_current_folder = d_project;

	initDockWidgets();
	initActions();
	initToolBars();
	initMenus();
	d_buttons.new_aspect->setMenu(d_menus.new_aspect);
	// TODO: move all strings to one method to be called on a language change
	
	connect(d_project, SIGNAL(aspectDescriptionChanged(AbstractAspect *)), 
		this, SLOT(handleAspectDescriptionChanged(AbstractAspect *)));
	connect(d_project, SIGNAL(aspectAdded(AbstractAspect *, int)), 
		this, SLOT(handleAspectAdded(AbstractAspect *, int)));
	connect(d_project, SIGNAL(aspectRemoved(AbstractAspect *, int)), 
		this, SLOT(handleAspectRemoved(AbstractAspect *, int)));

	handleAspectDescriptionChanged(d_project);
}

ProjectWindow::~ProjectWindow()
{
	disconnect(d_project, 0, this, 0);
}

void ProjectWindow::handleAspectDescriptionChanged(AbstractAspect *aspect)
{
	if (aspect != static_cast<AbstractAspect *>(d_project)) return;
	setWindowTitle(d_project->caption() + " - SciDAVis");
}

void ProjectWindow::handleAspectAdded(AbstractAspect *parent, int index)
{
	AbstractPart *part = qobject_cast<AbstractPart*>(parent->child(index));
	if (!part) return;
	QMdiSubWindow *win = part->mdiSubWindow();
	Q_ASSERT(win);
	d_mdi_area->addSubWindow(win);
	win->show();
	updateMdiWindowVisibility();
}

void ProjectWindow::handleAspectRemoved(AbstractAspect *parent, int index)
{
	Q_UNUSED(index);
	d_project_explorer->setCurrentAspect(parent);
}

void ProjectWindow::initDockWidgets()
{
	// project explorer
	d_project_explorer_dock = new QDockWidget(this);
	d_project_explorer_dock->setWindowTitle(tr("Project Explorer"));
	d_project_explorer = new ProjectExplorer(d_project_explorer_dock);
	d_project_explorer->setModel(new AspectTreeModel(d_project, this));
	d_project_explorer_dock->setWidget(d_project_explorer);
	addDockWidget(Qt::BottomDockWidgetArea, d_project_explorer_dock);
	connect(d_project_explorer, SIGNAL(currentAspectChanged(AbstractAspect *)),
		this, SLOT(handleCurrentAspectChanged(AbstractAspect *)));
	d_project_explorer->setCurrentAspect(d_project);

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
		
	d_actions.new_folder = new QAction(tr("New F&older"), this);
	d_actions.new_folder->setIcon(QIcon(QPixmap(":/folder_closed.xpm")));
	connect(d_actions.new_folder, SIGNAL(triggered(bool)), this, SLOT(addNewFolder()));

	d_part_maker_map = new QSignalMapper(this);
	connect(d_part_maker_map, SIGNAL(mapped(QObject*)), this, SLOT(addNewAspect(QObject*)));
	foreach(QObject *plugin, QPluginLoader::staticInstances()) {
		PartMaker *maker = qobject_cast<PartMaker*>(plugin);
		if (maker) {
			QAction *make = maker->makeAction(this);
			connect(make, SIGNAL(triggered()), d_part_maker_map, SLOT(map()));
			d_part_maker_map->setMapping(make, plugin);
			d_part_makers << make;
		}
	}
}

void ProjectWindow::initMenus()
{
	d_menus.file = menuBar()->addMenu(tr("&File"));
	d_menus.new_aspect = d_menus.file->addMenu(tr("&New"));
	d_menus.new_aspect->addAction(d_actions.new_folder);
	foreach(QAction *a, d_part_makers)
		d_menus.new_aspect->addAction(a);

	d_menus.file->addAction(d_actions.quit);

	d_menus.edit = menuBar()->addMenu(tr("&Edit"));
	d_menus.edit->addAction(d_project->undoAction(d_menus.edit));
	d_menus.edit->addAction(d_project->redoAction(d_menus.edit));
	d_menus.edit->addSeparator();

	d_menus.view = menuBar()->addMenu(tr("&View"));

	d_menus.toolbars = createToolbarsMenu();
	if(!d_menus.toolbars) d_menus.toolbars = new QMenu(this);
	d_menus.toolbars->setTitle(tr("Toolbars"));
	
	d_menus.dockwidgets = createDockWidgetsMenu();
	if(!d_menus.dockwidgets) d_menus.dockwidgets = new QMenu(this);
	d_menus.dockwidgets->setTitle(tr("Dock Widgets"));

	d_menus.view->addMenu(d_menus.toolbars);
	d_menus.view->addMenu(d_menus.dockwidgets);
	d_menus.view->addSeparator();

	foreach(QObject *plugin, QPluginLoader::staticInstances()) {
		ProjectMenuMaker *maker = qobject_cast<ProjectMenuMaker*>(plugin);
		if(maker)
			menuBar()->addMenu(maker->makeProjectMenu(this));
	}
}

void ProjectWindow::initToolBars()
{
	d_toolbars.file = new QToolBar( tr( "File" ), this );
	d_toolbars.file->setObjectName("file_toolbar"); // this is needed for QMainWindow::restoreState()
	addToolBar( Qt::TopToolBarArea, d_toolbars.file );
	
	d_buttons.new_aspect = new QToolButton(this);
	d_buttons.new_aspect->setPopupMode(QToolButton::InstantPopup);
	d_buttons.new_aspect->setIcon(QPixmap(":/new_aspect.xpm"));
	d_buttons.new_aspect->setToolTip(tr("New Aspect"));
	d_toolbars.file->addWidget(d_buttons.new_aspect);

	d_toolbars.edit = new QToolBar( tr("Edit"), this);
	d_toolbars.edit->setObjectName("edit_toolbar");
	addToolBar(Qt::TopToolBarArea, d_toolbars.edit);

	d_toolbars.edit->addAction(d_project->undoAction(d_toolbars.edit));
	d_toolbars.edit->addAction(d_project->redoAction(d_toolbars.edit));
}

void ProjectWindow::addNewFolder()
{
	addNewAspect(new Folder("Folder1"));
}

QMenu * ProjectWindow::createToolbarsMenu()
{
    QMenu *menu = 0;
    QList<QToolBar *> toolbars = qFindChildren<QToolBar *>(this);
    if (toolbars.size())
	{
        menu = new QMenu(this);
		foreach(QToolBar *toolbar, toolbars)
		{
            if (toolbar->parentWidget() == this)
                menu->addAction(toolbar->toggleViewAction());
        }
    }
    return menu;
}

QMenu * ProjectWindow::createDockWidgetsMenu()
{
    QMenu *menu = 0;
    QList<QDockWidget *> dockwidgets = qFindChildren<QDockWidget *>(this);
    if (dockwidgets.size())
	{
        menu = new QMenu(this);
		foreach(QDockWidget *widget, dockwidgets)
		{
            if (widget->parentWidget() == this)
                menu->addAction(widget->toggleViewAction());
        }
    }
    return menu;
}

void ProjectWindow::addNewAspect(QObject* obj)
{
	AbstractAspect *aspect = qobject_cast<AbstractAspect*>(obj);
	if (!aspect) {
		PartMaker *maker = qobject_cast<PartMaker*>(obj);
		if (!maker) return;
		aspect = maker->makePart();
	}
	QModelIndex index = d_project_explorer->currentIndex();

	if(!index.isValid()) 
		d_project->addChild(aspect);
	else
	{
		AbstractAspect * parent_aspect = static_cast<AbstractAspect *>(index.internalPointer());
		Q_ASSERT(parent_aspect->folder()); // every aspect contained in the project should have a folder
		parent_aspect->folder()->addChild(aspect);
	}
}

void ProjectWindow::handleCurrentAspectChanged(AbstractAspect *aspect)
{
	if(!aspect) aspect = d_project; // should never happen, just in case
	if(aspect->folder() != d_current_folder)
	{
		d_current_folder = aspect->folder();
		updateMdiWindowVisibility();
	}
	if(aspect != d_current_aspect)
	{
		AbstractPart * part = qobject_cast<AbstractPart*>(aspect);
		if (part)
			d_mdi_area->setActiveSubWindow(part->mdiSubWindow());
	}
	d_current_aspect = aspect;
}

void ProjectWindow::handleCurrentSubWindowChanged(QMdiSubWindow* win) {
	PartMdiView *view = qobject_cast<PartMdiView*>(win);
	if (!view) return;
	emit partActivated(view->part());
}

void ProjectWindow::updateMdiWindowVisibility()
{
	QList<QMdiSubWindow *> windows = d_mdi_area->subWindowList();
	PartMdiView * part_view;
	switch(d_project->mdiWindowControlPolicy()) {
		case Project::folderOnly:
			foreach(QMdiSubWindow *window, windows) {
				part_view = qobject_cast<PartMdiView *>(window);
				Q_ASSERT(part_view);
				if(part_view->part()->folder() == d_current_folder)
					part_view->show();
				else
					part_view->hide();
			}
		case Project::folderAndSubfolders:
			foreach(QMdiSubWindow *window, windows) {
				part_view = qobject_cast<PartMdiView *>(window);
				if(part_view->part()->isDescendantOf(d_current_folder))
					part_view->show();
				else
					part_view->hide();
			}
	}
}

void ProjectWindow::hideAllMdiWindows()
{
	QList<QMdiSubWindow *> windows = d_mdi_area->subWindowList();
	foreach(QMdiSubWindow *window, windows)
		window->hide();
}

void ProjectWindow::showAllMdiWindows()
{
	QList<QMdiSubWindow *> windows = d_mdi_area->subWindowList();
	foreach(QMdiSubWindow *window, windows)
		window->show();
}

