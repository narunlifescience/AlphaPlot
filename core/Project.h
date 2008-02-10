/***************************************************************************
    File                 : Project.h
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
#ifndef PROJECT_H
#define PROJECT_H

#include "Folder.h"

#ifndef _NO_TR1_
#include "tr1/memory"
using std::tr1::shared_ptr;
using std::tr1::enable_shared_from_this;
#else // if your compiler does not have TR1 support, you can use boost instead:
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
using boost::shared_ptr;
using boost::enable_shared_from_this;
#endif

#include <QHash>
#include <QKeySequence>
#include <QAction>
class QString;
class ProjectWindow;

//! Represents a SciDAVis project.
/**
 * Project manages an undo stack and is responsible for creating ProjectWindow instances
 * as views on itself.
 */
class Project : public Folder, public enable_shared_from_this<Project>
{
	Q_OBJECT

	public:
		enum MdiWindowControlPolicy 
		{
			folderOnly,
			folderAndSubfolders,
			manual
		};

	signals:
		void updateMdiWindows();
		void showAllMdiWindows();
		void hideAllMdiWindows();

	public:
		Project();
		~Project();

		virtual Project *project() const { return const_cast<Project*>(this); }
		virtual QUndoStack *undoStack() const;
		virtual QString path() const { return name(); }
		virtual AbstractAspect *parentAspect() const { return 0; }
		//! Query a keyboard shortcut for a given action
		/*
		 * This is used for application wide keyboard shortcuts.
		 */
		virtual QKeySequence queryShortcut(const QString& action_string);

		virtual AspectView *view();
		virtual ProjectWindow *projectWindow(QWidget *parent = 0);

		virtual QMenu *createContextMenu(QMenu * append_to = 0);

		void setMdiWindowControlPolicy(Project::MdiWindowControlPolicy policy);
		Project::MdiWindowControlPolicy mdiWindowControlPolicy() const;
	
	private slots:
		void setMdiWindowControlPolicy(QAction * action);

	private:
		class Private;
		Private *d;
};

#endif // ifndef PROJECT_H
