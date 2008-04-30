/***************************************************************************
    File                 : ProjectWindow.h
    Project              : SciDAVis
    Description          : Standard view on a Project; main window.
    --------------------------------------------------------------------
    Copyright            : (C) 2007-2008 Knut Franke (knut.franke*gmx.de)
    Copyright            : (C) 2007-2008 Tilman Hoener zu Siederdissen (thzs*gmx.net)
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
                           (some parts taken from former ApplicationWindow class
						    (C) 2004-2007 by Ion Vasilief (ion_vasilief*yahoo.fr))
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
class ActionManager;
#include "core/PartMdiView.h"

//! Standard view on a Project; main window.
class ProjectWindow : public QMainWindow
{
	Q_OBJECT

	public:
		ProjectWindow(Project* project);
		~ProjectWindow();

		QMenu *createContextMenu() const;
		QMenu *createFolderContextMenu(const Folder * folder) const;

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
		void importAspect();
		void showKeyboardShortcutsDialog();
		void showPreferencesDialog();
		void openProject();
		void saveProject();
		void saveProjectAs();

	signals:
		void partActivated(AbstractPart*);
	
	private slots:
		void handleAspectAdded(const AbstractAspect *parent, int index);
		void handleAspectAboutToBeRemoved(const AbstractAspect *parent, int index);
		void handleAspectRemoved(const AbstractAspect *parent, int index);
		void handleAspectDescriptionChanged(const AbstractAspect *aspect);
		void handleCurrentAspectChanged(AbstractAspect *aspect);
		void handleCurrentSubWindowChanged(QMdiSubWindow*);
		void handleSubWindowStatusChange(PartMdiView * view, PartMdiView::SubWindowStatus from, PartMdiView::SubWindowStatus to);
		void handleWindowsMenuAboutToShow();
		void handleWindowsPolicyMenuAboutToShow();
		void setMdiWindowVisibility(QAction * action);


	private:
		Project * d_project;

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
			*dockwidgets,
			*part,
			*windows,
			*win_policy_menu;
		} d_menus;
		
		struct {
		QAction 
			*quit,
			*import_aspect,
			*open_project,
			*save_project,
			*save_project_as,
			*new_folder,
			*keyboard_shortcuts_dialog,
			*preferences_dialog,
			*next_subwindow,
			*previous_subwindow,
			*cascade_windows,
			*close_current_window,
			*close_all_windows,
			*tile_windows,
			*visibility_folder,
			*visibility_subfolders,
			*visibility_all;
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

		static ActionManager * action_manager;
};

#endif // ifndef PROJECT_WINDOW_H
