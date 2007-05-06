/***************************************************************************
	File                 : PythonScript.h
	Project              : QtiPlot
--------------------------------------------------------------------
	Copyright            : (C) 2006 by Knut Franke
	Email (use @ for *)  : knut.franke*gmx.de
	Description          : Execute Python code from within QtiPlot

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
#ifndef PYTHON_SCRIPT_H
#define PYTHON_SCRIPT_H

#include "ScriptingEnv.h" 
#include "Script.h"

#include <qobject.h>
#include <qstring.h>

typedef struct _object PyObject;
class PythonScripting;

class PythonScript : public Script
{
	Q_OBJECT

	public:
		PythonScript(PythonScripting *env, const QString &code, QObject *context=0, const QString &name="<input>");
		~PythonScript();

		void write(const QString &text) { emit print(text); }

		public slots:
		bool compile(bool for_eval=true);
		QVariant eval();
		bool exec();
		bool setQObject(QObject *val, const char *name);
		bool setInt(int val, const char* name);
		bool setDouble(double val, const char* name);
		void setContext(QObject *context);

	private:
		PythonScripting *env() { return (PythonScripting*)Env; }
		void beginStdoutRedirect();
		void endStdoutRedirect();

		PyObject *PyCode, *localDict, *stdoutSave, *stderrSave;
		bool isFunction;
};


#endif
