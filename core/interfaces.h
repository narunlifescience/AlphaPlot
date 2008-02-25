/***************************************************************************
    File                 : interfaces.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2008 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Interfaces the kernel uses to talk to modules

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
#ifndef INTERFACES_H
#define INTERFACES_H

#include <QtPlugin>

class AbstractPart;
class QAction;
class QMenu;
class ProjectWindow;
class AbstractFilter;
class AbstractImportFilter;
class AbstractExportFilter;

//! Factory for AbstractPart objects.
class PartMaker
{
	public:
		virtual ~PartMaker() {}
		//! The factory method.
		virtual AbstractPart *makePart() = 0;
		//! The action to be used for making new parts.
		/**
		 * The caller takes care of connecting the action. If the parent argument is zero, it
		 * also recieves ownership of the action.
		 * Implementations should only set things like name and icon.
		 */
		virtual QAction *makeAction(QObject *parent) = 0;
};

Q_DECLARE_INTERFACE(PartMaker, "net.sf.scidavis.partmaker/0.1")

//! Factory for Menus to be added to a ProjectWindow.
class ProjectMenuMaker
{
	public:
		virtual ~ProjectMenuMaker() {}
		//! The factory method.
		virtual QMenu *makeProjectMenu(ProjectWindow *) = 0;
};

Q_DECLARE_INTERFACE(ProjectMenuMaker, "net.sf.scidavis.projectmenumaker/0.1")

//! Factory for filters.
/**
 * A FilterMaker introduces one or more filters to the kernel.
 */
class FilterMaker
{
	public:
		virtual ~FilterMaker() {}
		virtual AbstractFilter * makeFilter(int id=0) = 0;
		virtual int filterCount() const { return 1; }
		virtual QAction *makeAction(QObject *parent, int id=0) = 0;
};

Q_DECLARE_INTERFACE(FilterMaker, "net.sf.scidavis.filtermaker/0.1")

//! Factory for import/export filters.
class FileFormat
{
	public:
		virtual ~FileFormat() {}
		virtual AbstractImportFilter * makeImportFilter() = 0;
		virtual AbstractExportFilter * makeExportFilter() = 0;
};

Q_DECLARE_INTERFACE(FileFormat, "net.sf.scidavis.fileformat/0.1")

#endif // ifndef INTERFACES_H
