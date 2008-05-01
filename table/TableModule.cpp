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

#include "table/Table.h"
#include "table/AsciiTableImportFilter.h"
#include "core/Project.h"
#include "core/ProjectWindow.h"
#include "lib/ActionManager.h"
#include <QAction>
#include <QPixmap>
#include <QtDebug>
#include <QSettings>
#include "ui_TableConfigPage.h"

TableConfigPage::TableConfigPage() 
{
	ui = new Ui_TableConfigPage();
	ui->setupUi(this);
}

TableConfigPage::~TableConfigPage() 
{
	delete ui;
}

void TableConfigPage::apply()
{
	// TODO: read setting from ui and change them in Table
}

AbstractPart * TableModule::makePart()
{
	return new Table(0, 30, 2, tr("Table %1").arg(1));
}

QAction * TableModule::makeAction(QObject *parent)
{
	QAction *new_table = new QAction(tr("New &Table"), parent);
	new_table->setShortcut(tr("Ctrl+T", "new table shortcut"));
	new_table->setIcon(QIcon(QPixmap(":/table.xpm")));
	Table::actionManager()->addAction(new_table, "new_table");
	return new_table;
}

AbstractImportFilter * TableModule::makeImportFilter()
{
	return new AsciiTableImportFilter();
}

AbstractExportFilter * TableModule::makeExportFilter()
{
	// TODO
	return 0;
}

void TableModule::initActionManager()
{
	Table::initActionManager();
}

ConfigPageWidget * TableModule::makeConfigPage()
{
	return new TableConfigPage();
}
		
QString TableModule::configPageLabel()
{
	return QObject::tr("Table");
}

void TableModule::loadSettings()
{
#ifdef Q_OS_MAC // Mac
	QSettings settings(QSettings::IniFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#else
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#endif

	settings.beginGroup("Table");
	settings.endGroup();
}

void TableModule::saveSettings()
{
#ifdef Q_OS_MAC // Mac
	QSettings settings(QSettings::IniFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#else
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#endif

	settings.beginGroup("Table");
	settings.endGroup();
}

Q_EXPORT_PLUGIN2(scidavis_table, TableModule)


