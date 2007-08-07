/***************************************************************************
    File                 : AbstractScript.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : A chunk of scripting code.
                           
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
#include "AbstractScript.h"
#include "AbstractScriptingEngine.h"

AbstractScript::AbstractScript(AbstractScriptingEngine *engine, const QString &code, QObject *context, const QString &name)
	: d_engine(engine), Code(code), Name(name), compiled(notCompiled)
{
	d_engine->incref();
	Context = context;
	EmitErrors=true;
}

AbstractScript::~AbstractScript()
{
	d_engine->decref();
}

bool AbstractScript::compile(bool for_eval)
{
	emit_error("AbstractScript::compile called!", 0);
	return false;
}

QVariant AbstractScript::eval()
{
	emit_error("AbstractScript::eval called!",0);
	return QVariant();
}

bool AbstractScript::exec()
{
	emit_error("AbstractScript::exec called!",0);
	return false;
}

