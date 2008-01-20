/***************************************************************************
    File                 : ProjectWindow.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
                           some parts written 2004-2007 by Ion Vasilief
                           (from former ProjectWindow class)
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
#include <QtDebug>

ProjectWindow::ProjectWindow(shared_ptr<Project> project)
	: d_project(project)
{
	init();
}

void ProjectWindow::init()
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowIcon(QIcon(":/appicon"));

//	setCentralWidget(d_mdi_area);

	initDockWidgets();
	initActions();
	initToolBars();
	initMenus();
	d_buttons.new_aspect->setMenu(d_menus.new_aspect);
	// TODO: move all strings to one method to be called on a language change
	
	connect(d_project->abstractAspectSignalEmitter(), SIGNAL(aspectDescriptionChanged(AbstractAspect *)), 
		this, SLOT(handleAspectDescriptionChanged(AbstractAspect *)));
	connect(d_project->abstractAspectSignalEmitter(), SIGNAL(aspectAdded(AbstractAspect *, int)), 
		this, SLOT(handleAspectAdded(AbstractAspect *, int)));
	connect(d_project->abstractAspectSignalEmitter(), SIGNAL(aspectRemoved(AbstractAspect *, int)), 
		this, SLOT(handleAspectRemoved(AbstractAspect *, int)));
//	connect(d_mdi_area, SIGNAL(subWindowActivated(QMdiSubWindow *)),
//		this, SLOT(handleMdiSubWindowActivated(QMdiSubWindow *)));

	handleAspectDescriptionChanged(d_project.get());
}

ProjectWindow::~ProjectWindow()
{
	setCentralWidget(0); // the mdi area objects are deleted in the folder dtor
	disconnect(d_project->abstractAspectSignalEmitter(), 0, this, 0);
}

void ProjectWindow::handleAspectDescriptionChanged(AbstractAspect *aspect)
{
	if (aspect != static_cast<AbstractAspect *>(d_project.get())) return;
	setWindowTitle(d_project->caption() + " - SciDAVis");
}

void ProjectWindow::handleAspectAdded(AbstractAspect *parent, int index)
{
	// TODO: this goes to folder
	shared_ptr<AbstractAspect> aspect = parent->child(index);
	QWidget *view = aspect->view();
	if (!view) return;
	
	QModelIndex current_index = d_project_explorer->currentIndex();
	if(!current_index.isValid()) return;

	Folder * folder = static_cast<AbstractAspect *>(current_index.internalPointer())->folder();
	folder->mdiArea()->addSubWindow(new MdiSubWindow(aspect, view));
	view->show();
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
	d_project_explorer->setAnimated(true);
	d_project_explorer->setAlternatingRowColors(true);
	d_project_explorer_dock->setWidget(d_project_explorer);
	addDockWidget(Qt::BottomDockWidgetArea, d_project_explorer_dock);
	connect(d_project_explorer, SIGNAL(currentAspectChanged(AbstractAspect *)),
		this, SLOT(handleCurrentAspectChanged(AbstractAspect *)));
	d_project_explorer->setCurrentAspect(d_project.get());

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

	d_actions.new_folder = new QAction(tr("New F&older"), this);
	d_actions.new_folder->setIcon(QIcon(QPixmap(":/folder_closed.xpm")));
	connect(d_actions.new_folder, SIGNAL(triggered(bool)), this, SLOT(addNewFolder()));

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
	d_menus.new_aspect->addAction(d_actions.new_folder);

	d_menus.file->addAction(d_actions.quit);

	d_menus.edit = menuBar()->addMenu(tr("&Edit"));
	d_menus.edit->addAction(d_actions.undo);
	d_menus.edit->addAction(d_actions.redo);
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
}

void ProjectWindow::addNewTable()
{
	addNewAspect(shared_ptr<Table>(new Table(0, 20, 2, "Table1")));
}

void ProjectWindow::addNewFolder()
{
	addNewAspect(shared_ptr<Folder>(new Folder("Folder1")));
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

void ProjectWindow::handleMdiSubWindowActivated(QMdiSubWindow *window)
{
	if(!window) return;
	MdiSubWindow * mdi_win = static_cast<MdiSubWindow *>(window);
	shared_ptr<AbstractAspect> aspect = mdi_win->aspect();
	d_project_explorer->setCurrentAspect(aspect.get());
}

void ProjectWindow::addNewAspect(shared_ptr<AbstractAspect> aspect)
{
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
	QMdiArea * mdi_area = aspect->folder()->mdiArea();
	if(static_cast<QWidget *>(mdi_area) != centralWidget())
	{
		QWidget * widget = centralWidget();
		if(widget) widget->hide();
		setCentralWidget(mdi_area);
		mdi_area->show();
	}
}

