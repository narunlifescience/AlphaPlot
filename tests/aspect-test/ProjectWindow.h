/***************************************************************************
    File                 : ProjectWindow.h
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
#ifndef PROJECT_WINDOW_H
#define PROJECT_WINDOW_H

#include <QMainWindow>
#include "AbstractAspect.h"

class Project;
class QMdiArea;

//! Standard view on a Project; main window.
class ProjectWindow : public QMainWindow
{
	Q_OBJECT

	public:
		ProjectWindow(shared_ptr<Project> project);
		~ProjectWindow();
	
	public slots:
		void aspectAdded(AbstractAspect *parent, int index);
		void aspectDescriptionChanged(AbstractAspect *aspect);

	private:
		shared_ptr<Project> d_project;
		QMdiArea * d_mdi;
};

#endif // ifndef PROJECT_WINDOW_H
