/***************************************************************************
    File                 : Project.cpp
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
#include "Project.h"
#include "ProjectWindow.h"
#include "ScriptingEngineManager.h"
#include "core/interfaces.h"
#include <QUndoStack>
#include <QString>
#include <QKeySequence>
#include <QMenu>
#include <QSettings>
#include <QPluginLoader>
#include <QComboBox>
#include "ui_ProjectConfigPage.h"

#define NOT_IMPL (QMessageBox::information(0, "info", "not yet implemented"))

class Project::Private
{
	public:
		Private() : mdi_window_visibility(folderOnly), primary_view(0), scripting_engine(0) {}
		~Private() {
			delete primary_view;
		}
		QUndoStack undo_stack;
		MdiWindowVisibility mdi_window_visibility;
		ProjectWindow * primary_view;
		AbstractScriptingEngine * scripting_engine;

		// global settings
		static Project::MdiWindowVisibility default_mdi_window_visibility;
		static bool auto_search_updates;
		static QString language;
		static QStringList recent_projects;
		static QString style;
		static bool auto_save;
		static int auto_save_interval;
		static QString default_scripting_language;
		static QLocale locale;
		static bool locale_use_group_separator;
		static QString help_path;
		static QString fit_plugins_path;
		static QString templates_path;
		static QString ascii_path;
		static QString images_path;
};

// global settings
Project::MdiWindowVisibility Project::Private::default_mdi_window_visibility = Project::folderOnly;
bool Project::Private::auto_search_updates = false;
QString Project::Private::language = QString("en");
QStringList Project::Private::recent_projects;
QString Project::Private::style;
bool Project::Private::auto_save = true;
int Project::Private::auto_save_interval = 15;
QString Project::Private::default_scripting_language = "muParser";
QLocale Project::Private::locale;
bool Project::Private::locale_use_group_separator = false;
QString Project::Private::help_path;
QString Project::Private::fit_plugins_path;
QString Project::Private::templates_path;
QString Project::Private::ascii_path;
QString Project::Private::images_path;

ProjectConfigPage::ProjectConfigPage() 
{
	ui = new Ui_ProjectConfigPage();
	ui->setupUi(this);
	ui->default_subwindow_visibility_combobox->setCurrentIndex((int)Project::Private::default_mdi_window_visibility);
	// TODO: set the ui according to the global settings in Project::Private
}

ProjectConfigPage::~ProjectConfigPage() 
{
	delete ui;
}

void ProjectConfigPage::apply() 
{
	int index = ui->default_subwindow_visibility_combobox->currentIndex();
	switch (index)
	{
		case 0:
		case 1:
		case 2: 
			Project::Private::default_mdi_window_visibility = (Project::MdiWindowVisibility)index;
			break;
	}
	// TODO: read settings from ui and change them in Project::Private
}

Project::Project()
	: Folder(tr("Unnamed")), d(new Private())
{
	// TODO: intelligent engine choosing
	Q_ASSERT(ScriptingEngineManager::instance()->engineNames().size() > 0);
	QString engine_name = ScriptingEngineManager::instance()->engineNames()[0];
	d->scripting_engine = ScriptingEngineManager::instance()->engine(engine_name);
}
	
Project::~Project()
{
	delete d;
}

QUndoStack *Project::undoStack() const
{
	return &d->undo_stack;
}

ProjectWindow *Project::view()
{
	if (!d->primary_view)
		d->primary_view = new ProjectWindow(this);
	return d->primary_view;
}

QMenu *Project::createContextMenu() const
{
	return const_cast<Project *>(this)->view()->createContextMenu();
}
		
QMenu *Project::createFolderContextMenu(const Folder * folder) const
{
	return const_cast<Project *>(this)->view()->createFolderContextMenu(folder);
}

void Project::setMdiWindowVisibility(MdiWindowVisibility visibility)
{ 
	d->mdi_window_visibility = visibility; 
	view()->updateMdiWindowVisibility();
}
		
Project::MdiWindowVisibility Project::mdiWindowVisibility() const 
{ 
	return d->mdi_window_visibility; 
}

AbstractScriptingEngine * Project::scriptingEngine() const
{
	return d->scripting_engine;
}

/* ================== static methods ======================= */
ConfigPageWidget * Project::makeConfigPage()
{
	 return new ProjectConfigPage();
}

QString Project::configPageLabel()
{
	return QObject::tr("Project");
}

void Project::loadSettings()
{
#ifdef Q_OS_MAC // Mac
	QSettings settings(QSettings::IniFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#else
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#endif

	settings.beginGroup("General");

	Private::default_mdi_window_visibility = static_cast<Project::MdiWindowVisibility>(settings.value("default_mdi_window_visibility", Project::folderOnly).toInt());
	Private::auto_search_updates = settings.value("auto_search_updates", false).toBool();
	Private::language = settings.value("language", QLocale::system().name().section('_', 0, 0)).toString();
	Private::recent_projects = settings.value("recent_projects").toStringList();
	Private::style = settings.value("/Style", qApp->style()->objectName()).toString();
	Private::auto_save = settings.value("auto_save", true).toBool();
	Private::auto_save_interval = settings.value("auto_save_interval", 15).toInt();
	Private::default_scripting_language = settings.value("default_scripting_language", "muParser").toString();

	QLocale temp_locale = QLocale(settings.value("locale", QLocale::system().name()).toString());
	Private::locale_use_group_separator = settings.value("locale_use_group_separator", false).toBool();
	if (Private::locale_use_group_separator)
		temp_locale.setNumberOptions(temp_locale.numberOptions() & ~QLocale::OmitGroupSeparator);
	else
		temp_locale.setNumberOptions(temp_locale.numberOptions() | QLocale::OmitGroupSeparator);
	Private::locale = temp_locale;
	QLocale::setDefault(Private::locale);

	// TODO: use changes from stable branch here; set resonable defaults 
	Private::help_path = settings.value("help_path", "").toString();
	Private::fit_plugins_path = settings.value("fit_plugins_path", "").toString();
	Private::templates_path = settings.value("templates_path", "").toString();
	Private::ascii_path = settings.value("ascii_path", "").toString();
	Private::images_path = settings.value("images_path", "").toString();

	settings.endGroup();
	
	// TODO: set language, style, scripting according to theses settings (in the project ctor probably)

	foreach(QObject * plugin, QPluginLoader::staticInstances()) 
	{
		ConfigPageMaker * ctm = qobject_cast<ConfigPageMaker*>(plugin);
		if (!ctm) continue;
		ctm->loadSettings();
	}
}

void Project::saveSettings()
{
#ifdef Q_OS_MAC // Mac
	QSettings settings(QSettings::IniFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#else
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope, "SciDAVis", "SciDAVis");
#endif

	settings.beginGroup("General");

	settings.setValue("default_mdi_window_visibility", Project::Private::default_mdi_window_visibility);
	settings.setValue("auto_search_updates", Private::auto_search_updates);
	settings.setValue("language", Private::language);
	settings.setValue("recent_projects", Private::recent_projects);
	settings.setValue("style", Private::style);
	settings.setValue("auto_save", Private::auto_save);
	settings.setValue("auto_save_interval", Private::auto_save_interval);
	settings.setValue("default_scripting_language", Private::default_scripting_language);
	settings.setValue("locale", QLocale().name());
	settings.setValue("locale_use_group_separator", bool(!(QLocale().numberOptions() & QLocale::OmitGroupSeparator)));

	settings.setValue("help_path", Private::help_path);
	settings.setValue("fit_plugins_path", Private::fit_plugins_path);
	settings.setValue("templates_path", Private::templates_path);
	settings.setValue("ascii_path", Private::ascii_path);
	settings.setValue("images_path", Private::images_path);

	settings.endGroup();

	foreach(QObject * plugin, QPluginLoader::staticInstances()) 
	{
		ConfigPageMaker * ctm = qobject_cast<ConfigPageMaker*>(plugin);
		if (!ctm) continue;
		ctm->saveSettings();
	}
}
