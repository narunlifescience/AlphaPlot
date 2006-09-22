/***************************************************************************
    File                 : Scripting.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Knut Franke
    Email                : knut.franke@gmx.de
    Description          : Implementations of generic scripting classes
                           
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
#include "Scripting.h"

#include <string.h>

#ifdef SCRIPTING_MUPARSER
#include "muParserScripting.h"
#endif
#ifdef SCRIPTING_PYTHON
#include "PythonScripting.h"
#endif

ScriptingLangManager::ScriptingLang ScriptingLangManager::langs[] = {
#ifdef SCRIPTING_MUPARSER
	{ muParserScripting::langName, muParserScripting::constructor },
#endif
#ifdef SCRIPTING_PYTHON
	{ PythonScripting::langName, PythonScripting::constructor },
#endif
	{ NULL, NULL }
};

ScriptingEnv *ScriptingLangManager::newEnv(ApplicationWindow *parent)
{
	if (!langs[0].constructor)
		return NULL;
	else
		return langs[0].constructor(parent);
}

ScriptingEnv *ScriptingLangManager::newEnv(const char *name, ApplicationWindow *parent)
{
	for (ScriptingLang *i = langs; i->constructor; i++)
		if (!strcmp(name, i->name))
			return i->constructor(parent);
	return NULL;
}

QStringList ScriptingLangManager::languages()
{
	QStringList l;
	for (ScriptingLang *i = langs; i->constructor; i++)
		l << i->name;
	return l;
}

	ScriptingEnv::ScriptingEnv(ApplicationWindow *parent, const char *langName)
: QObject(0, langName), Parent(parent)
{
	initialized=false;
	refcount=0;
}

void ScriptingEnv::incref()
{
	refcount++;
}

void ScriptingEnv::decref()
{
	refcount--;
	if (refcount==0)
		delete this;
}

bool Script::compile(bool for_eval)
{
	emit_error("Script::compile called!", 0);
	return false;
}

QVariant Script::eval()
{
	emit_error("Script::eval called!",0);
	return QVariant();
}

bool Script::exec()
{
	emit_error("Script::exec called!",0);
	return false;
}

scripted::scripted(ScriptingEnv *env)
{
	env->incref();
	scriptEnv = env;
}

scripted::~scripted()
{
	scriptEnv->decref();
}

void scripted::scriptingChangeEvent(ScriptingChangeEvent *sce)
{
	scriptEnv->decref();
	sce->scriptingEnv()->incref();
	scriptEnv = sce->scriptingEnv();
}
