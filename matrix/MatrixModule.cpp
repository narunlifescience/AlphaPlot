/***************************************************************************
    File                 : MatrixModule.cpp
    Project              : SciDAVis
    Description          : Module providing the matrix Part and support classes.
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
#include "MatrixModule.h"

#include "Matrix.h"
#include "Project.h"
#include "ProjectWindow.h"
#include <QAction>
#include <QPixmap>

AbstractPart * MatrixModule::makePart()
{
	return new Matrix(0, 20, 2, tr("Matrix %1").arg(1));
}

QAction * MatrixModule::makeAction(QObject *parent)
{
	QAction *new_matrix = new QAction(tr("New &Matrix"), parent);
	new_matrix->setIcon(QIcon(QPixmap(":/new_matrix.xpm")));
	return new_matrix;
}

QMenu * MatrixModule::makeProjectMenu(ProjectWindow *win)
{
	MatrixMenu *menu = new MatrixMenu(win);
	return menu;
}

MatrixMenu::MatrixMenu(ProjectWindow *win) : QMenu(tr("Matrix"))
{
	setEnabled(false);
	connect(win, SIGNAL(partActivated(AbstractPart*)),
			this, SLOT(handlePartActivated(AbstractPart*)));
}

void MatrixMenu::handlePartActivated(AbstractPart* part)
{
	setEnabled(part->inherits("Matrix"));
}


Q_EXPORT_PLUGIN2(scidavis_matrix, MatrixModule)

