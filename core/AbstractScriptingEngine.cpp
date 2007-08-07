/***************************************************************************
    File                 : AbstractScriptingEngine.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
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
#include "AbstractScriptingEngine.h"

#include <string>

/******************************************************************************\
 * Static part: implementation registry                                       *
\******************************************************************************/

#ifdef SCRIPTING_MUPARSER
#include "muparser/MuParserScriptingEngine.h"
#endif
#ifdef SCRIPTING_PYTHON
#include "python/PythonScriptingEngine.h"
#endif

AbstractScriptingEngine::ScriptingEngineEntry AbstractScriptingEngine::g_engines[] = {
#ifdef SCRIPTING_MUPARSER
	{ MuParserScriptingEngine::g_lang_name, MuParserScriptingEngine::constructor },
#endif
#ifdef SCRIPTING_PYTHON
	{ PythonScriptingEngine::g_lang_name, PythonScriptingEngine::constructor },
#endif
	{ NULL, NULL }
};

AbstractScriptingEngine* AbstractScriptingEngine::create(ApplicationWindow *parent)
{
	if(g_engines[0].constructor)
		return g_engines[0].constructor(parent);
	else
		return 0;
}

AbstractScriptingEngine* AbstractScriptingEngine::create(const char *name, ApplicationWindow *parent)
{
	for(ScriptingEngineEntry *i = g_engines; i->constructor; i++)
		if (!strcmp(name, i->name))
			return i->constructor(parent);
	return 0;
}

QStringList AbstractScriptingEngine::engineNames()
{
	QStringList result;
	for (ScriptingEngineEntry *i = g_engines; i->constructor; i++)
		result << i->name;
	return result;
}

/******************************************************************************\
 * Non-static part: generic engine methods.                                   *
\******************************************************************************/

AbstractScriptingEngine::AbstractScriptingEngine(ApplicationWindow *parent, const char *lang_name)
	: QObject(0), d_parent(parent)
{
	setObjectName(lang_name);
	d_initialized=false;
	d_refcount=0;
}

const QString AbstractScriptingEngine::fileFilter() const
{
	QStringList extensions = fileExtensions();
	if (extensions.isEmpty())
		return "";
	else
		return tr("%1 Source (*.%2);;").arg(objectName()).arg(extensions.join(" *."));
}

void AbstractScriptingEngine::incref()
{
	d_refcount++;
}

void AbstractScriptingEngine::decref()
{
	d_refcount--;
	if (d_refcount==0)
		delete this;
}

/******************************************************************************\
 *Helper classes for managing instances of AbstractScriptingEngine subclasses.*
\******************************************************************************/

scripted::scripted(AbstractScriptingEngine *engine)
{
	if (engine)
		engine->incref();
	d_scripting_engine = engine;
}

scripted::~scripted()
{
	if (d_scripting_engine)
		d_scripting_engine->decref();
}

void scripted::scriptingChangeEvent(ScriptingChangeEvent *sce)
{
	d_scripting_engine->decref();
	sce->scriptingEngine()->incref();
	d_scripting_engine = sce->scriptingEngine();
}
