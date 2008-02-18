/***************************************************************************
    File                 : ProjectWindow.h
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
#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <QMainWindow>

class AbstractAspect;
class Project;
class QMdiArea;
class QToolBar;
class QMenu;
class ProjectExplorer;
class QUndoView;
class QToolButton;
class QMdiSubWindow;
class Folder;
class QSignalMapper;
class AbstractPart;

//! Standard view on a Project; main window.
class ProjectWindow : public QMainWindow
{
	Q_OBJECT

	public:
		ProjectWindow(Project* project);
		~ProjectWindow();

	public slots:
		//! Add a new Part in the current folder.
		/**
		 * The argument object can be either a PartMaker or an AbstractAspect.
		 */
		void addNewAspect(QObject *obj);

	protected:
		//! \name Initialization
		//@{
		void init();
		void initDockWidgets();
		void initToolBars();
		void initMenus();
		void initActions();
		//@}
	
		QMenu * createToolbarsMenu();
		QMenu * createDockWidgetsMenu();
	
	public slots:
		void addNewFolder();
		//! Show hide mdi windows depending on the currend folder
		void updateMdiWindowVisibility();
		void hideAllMdiWindows();
		void showAllMdiWindows();

	signals:
		void partActivated(AbstractPart*);
	
	private slots:
		void handleAspectAdded(AbstractAspect *parent, int index);
		void handleAspectRemoved(AbstractAspect *parent, int index);
		void handleAspectDescriptionChanged(AbstractAspect *aspect);
		void handleCurrentAspectChanged(AbstractAspect *aspect);
		void handleCurrentSubWindowChanged(QMdiSubWindow*);

	private:
		Project* d_project;

		struct {
		QToolBar 
			*file,
			*edit;
		} d_toolbars;

		struct {
		QMenu 
			*file,
			*edit,
			*view,
			*new_aspect,
			*toolbars,
			*dockwidgets;
		} d_menus;
		
		struct {
		QAction 
			*quit,
			*new_folder;
		} d_actions;

		QList<QAction*> d_part_makers;
		QSignalMapper *d_part_maker_map;

		struct {
		QToolButton 
			*new_aspect;
		} d_buttons;
		
		QDockWidget * d_project_explorer_dock;
		ProjectExplorer * d_project_explorer;
		QDockWidget * d_history_dock;
		QUndoView * d_undo_view;
		QMdiArea * d_mdi_area;
		AbstractAspect * d_current_aspect;
		Folder * d_current_folder;
};

#endif // ifndef PROJECT_WINDOW_H
