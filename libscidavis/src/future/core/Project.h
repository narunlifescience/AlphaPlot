/***************************************************************************
    File                 : Project.h
    Project              : SciDAVis
    Description          : Represents a SciDAVis project.
    --------------------------------------------------------------------
    Copyright            : (C) 2007 Tilman Benkert (thzs*gmx.net)
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

#include "core/future_Folder.h"
#include "core/interfaces.h"

class QString;
class ProjectWindow;
class QAction;
class AbstractScriptingEngine;

//! Represents a SciDAVis project.
/**
 * Project manages an undo stack and is responsible for creating ProjectWindow instances
 * as views on itself.
 */
class Project : public future::Folder
{
	Q_OBJECT

	public:
		//! MDI subwindow visibility setting
		enum MdiWindowVisibility
		{
			folderOnly,
			folderAndSubfolders,
			allMdiWindows
		};

	public:
		Project();
		~Project();

		//!\name Reimplemented from AbstractAspect
		//@{
		virtual const Project *project() const { return this; }
		virtual Project *project() { return this; }
		virtual QUndoStack *undoStack() const;
		virtual QString path() const { return ""; }
#ifndef LEGACY_CODE_0_2_x
		virtual ProjectWindow *view();
#else
		virtual void *view();
#endif
		virtual QMenu *createContextMenu() const;
		//@}
		virtual QMenu *createFolderContextMenu(const future::Folder * folder) const;

		AbstractScriptingEngine * scriptingEngine() const;

		void setMdiWindowVisibility(MdiWindowVisibility visibility);
		MdiWindowVisibility mdiWindowVisibility() const;
		void setFileName(const QString & file_name);
		QString fileName() const;
	
		static ConfigPageWidget * makeConfigPage();
		static QString configPageLabel();
	
		//! \name serialize/deserialize
		//@{
		//! Save as XML
		virtual void save(QXmlStreamWriter *) const;
		//! Load from XML
		virtual bool load(XmlStreamReader *);
		//@}

	private:
		class Private;
		Private *d;

		friend class ProjectConfigPage;
};

#endif // ifndef PROJECT_H
