/***************************************************************************
    File                 : TableModule.cpp
    Project              : SciDAVis
    Description          : Module providing the table Part and support classes.
    --------------------------------------------------------------------
    Copyright            : (C) 2008 Knut Franke (knut.franke*gmx.de)
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
#include "TableModule.h"

#include "Table.h"
#include "Project.h"
#include "ProjectWindow.h"
#include <QAction>
#include <QPixmap>

AbstractPart * TableModule::makePart()
{
	return new Table(0, 20, 2, tr("Table %1").arg(1));
}

QAction * TableModule::makeAction(QObject *parent)
{
	QAction *new_table = new QAction(tr("New &Table"), parent);
	new_table->setIcon(QIcon(QPixmap(":/table.xpm")));
	return new_table;
}

QMenu * TableModule::makeProjectMenu(ProjectWindow *win)
{
	TableMenu *menu = new TableMenu(win);
	return menu;
}

TableMenu::TableMenu(ProjectWindow *win) : QMenu(tr("Table"))
{
	setEnabled(false);
	connect(win, SIGNAL(partActivated(AbstractPart*)),
			this, SLOT(handlePartActivated(AbstractPart*)));
}

void TableMenu::handlePartActivated(AbstractPart* part)
{
	setEnabled(part->inherits("Table"));
}


Q_EXPORT_PLUGIN2(scidavis_table, TableModule)
