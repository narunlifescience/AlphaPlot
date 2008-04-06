/***************************************************************************
    File                 : Graph3DModule.h
    Project              : SciDAVis
    Description          : Module providing the 3D graph Part and support classes.
    --------------------------------------------------------------------
    Copyright            : (C) 2008 Knut Franke (knut.franke*gmx.de)
    Copyright            : (C) 2008 Tilman Hoener zu Siederdissen (thzs*gmx.net)
                           (replace * with @ in the email address)

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

#include "Graph3DModule.h"

#include "Graph3D.h"
#include "Project.h"
#include "ProjectWindow.h"
#include <QAction>
#include <QPixmap>
#include <QtDebug>

AbstractPart * Graph3DModule::makePart()
{
	return new Graph3D(tr("Graph3D %1").arg(1));
}

QAction * Graph3DModule::makeAction(QObject *parent)
{
	QAction *new_graph3D = new QAction(tr("New &Graph3D"), parent);
	new_graph3D->setIcon(QIcon(QPixmap(":/grid_poly.xpm")));  // TODO: make a better icon
	return new_graph3D;
}

void Graph3DModule::initActionManager()
{
	Graph3D::initActionManager();
}

Q_EXPORT_PLUGIN2(scidavis_graph3D, Graph3DModule)
