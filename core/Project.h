/***************************************************************************
    File                 : Project.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
    Description          : Represents a SciDAVis project.

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

// TODO: replace old Folder.{h,cpp} in core
#include "../aspect-test/Folder.h"

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

//! Represents a SciDAVis project.
/**
 * Project manages an undo stack and is responsible for creating ProjectWindow instances
 * as views on itself.
 */
class Project : public Folder, public enable_shared_from_this<Project>
{
	Q_OBJECT

	public:
		Project();

		virtual Project *project() const { return const_cast<Project*>(this); }
		virtual QUndoStack *undoStack() const;
		virtual QString path() const { return "/"; }
		virtual AbstractAspect *parentAspect() const { return 0; }

		virtual QWidget *view(QWidget *parent = 0);

	private:
		class Private;
		Private *d;
};

#endif // ifndef PROJECT_H
