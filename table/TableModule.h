/***************************************************************************
    File                 : TableModule.h
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
#ifndef TABLE_MODULE_H
#define TABLE_MODULE_H

#include "core/interfaces.h"
#include "table/Table.h"
#include <QMenu>

class TableModule : public QObject, public PartMaker, public ProjectMenuMaker, public FileFormat, public ActionManagerOwner
{
	Q_OBJECT
	Q_INTERFACES(PartMaker ProjectMenuMaker FileFormat ActionManagerOwner)

	public:
		virtual AbstractPart * makePart();
		virtual QAction * makeAction(QObject *parent);
		virtual QMenu * makeProjectMenu(ProjectWindow *win);
		virtual AbstractImportFilter * makeImportFilter();
		virtual AbstractExportFilter * makeExportFilter();
		virtual ActionManager * actionManager() { return Table::actionManager(); }
		virtual void initActionManager();
};

class TableMenu : public QMenu
{
	Q_OBJECT
	
	public:
		TableMenu(ProjectWindow *win);

	private slots:
		void handlePartActivated(AbstractPart *part);
};

#endif // ifndef TABLE_MODULE_H

