/***************************************************************************
	File                 : PythonScriptingEngine.h
	Project              : SciDAVis
--------------------------------------------------------------------
	Copyright            : (C) 2006,2008 by Knut Franke
	Email (use @ for *)  : knut.franke*gmx.de
	Description          : Execute Python code from within SciDAVis

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
#ifndef PYTHON_SCRIPTING_H
#define PYTHON_SCRIPTING_H

#include "../core/AbstractScriptingEngine.h"
#include "PythonScript.h"

class QObject;
class QString;

typedef struct _object PyObject;

class PythonScriptingEngine: public AbstractScriptingEngine
{
	Q_OBJECT
	Q_INTERFACES(AbstractScriptingEngine)

	public:
		static const char *g_lang_name;
		PythonScriptingEngine();
		~PythonScriptingEngine();
		void initialize();

		void write(const QString &text) { emit print(text); }

		//! like str(object) in Python
		/**
		 * Convert object to a string.
		 * Steals a reference to object if decref is true; borrows otherwise.
		 */
		QString toString(PyObject *object, bool decref=false);
		//! evaluate a Python expression
		/**
		 * Evaluates code, using argDict (borrowed reference) as local dictionary
		 * or an empty one if argDict==NULL. name is the filename Python uses when
		 * reporting errors. Returns a new reference; NULL means caller has to do
		 * exception handling.
		 */
		PyObject* eval(const QString &code, PyObject *argDict=NULL, const char *name="<scidavis>");
		//! execute a sequence of Python statements
		/**
		 * Executes code, using argDict (borrowed reference) as local dictionary
		 * or an empty one if argDict==NULL. name is the filename Python uses when
		 * reporting errors. A false return value means caller has to do exception
		 * handling.
		 */
		bool exec(const QString &code, PyObject *argDict=NULL, const char *name="<scidavis>");
		QString errorMsg();

		bool isRunning() const;
		AbstractScript *makeScript(const QString &code, QObject *context, const QString &name="<input>")
		{
			return new PythonScript(this, code, context, name);
		}

		bool setQObject(QObject*, const char*, PyObject *dict);
		bool setQObject(QObject *val, const char *name) { return setQObject(val,name,NULL); }
		bool setInt(int, const char*, PyObject *dict=NULL);
		bool setDouble(double, const char*, PyObject *dict=NULL);

		const QStringList mathFunctions() const;
		const QString mathFunctionDoc (const QString &name) const;
		const QStringList fileExtensions() const;

		PyObject *globalDict() { return m_globals; }
		PyObject *sysDict() { return m_sys; }

	private:
		bool loadInitFile(const QString &path);

		PyObject *m_globals;  // PyDict of global environment
		PyObject *m_math;     // PyDict of math functions
		PyObject *m_sys;      // PyDict of sys module
};

#endif
