/***************************************************************************
    File                 : ScriptingEnv.cpp
    Project              : QtiPlot
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
#include "ScriptingEnv.h"
#include "Script.h"

#include <string.h>

#ifdef SCRIPTING_MUPARSER
#include "muParserScript.h"
#include "muParserScripting.h"
#endif
#ifdef SCRIPTING_PYTHON
#include "PythonScript.h"
#include "PythonScripting.h"
#endif

	ScriptingEnv::ScriptingEnv(ApplicationWindow *parent, const char *langName)
: QObject(0, langName), d_parent(parent)
{
	d_initialized=false;
	d_refcount=0;
}

const QString ScriptingEnv::fileFilter() const
{
	QStringList extensions = fileExtensions();
	if (extensions.isEmpty())
		return "";
	else
		return tr("%1 Source (*.%2);;").arg(name()).arg(extensions.join(" *."));
}

void ScriptingEnv::incref()
{
	d_refcount++;
}

void ScriptingEnv::decref()
{
	d_refcount--;
	if (d_refcount==0)
		delete this;
}

