/***************************************************************************
	File                 : PythonScript.cpp
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
// get rid of a compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#include <Python.h>

#include "PythonScript.h"
#include "PythonScripting.h"
#include "ApplicationWindow.h"

#include <QObject>
#include <QVariant>

PythonScript::PythonScript(PythonScripting *env, const QString &code, QObject *context, const QString &name)
: Script(env, code, context, name)
{
	PyCode = NULL;
	localDict = PyDict_New();
	setQObject(Context, "self");
}

PythonScript::~PythonScript()
{
	Py_DECREF(localDict);
	Py_XDECREF(PyCode);
}

void PythonScript::setContext(QObject *context)
{
	Script::setContext(context);
	setQObject(Context, "self");
}

bool PythonScript::compile(bool for_eval)
{
	// Support for the convenient col() and cell() functions.
	// This can't be done anywhere else, because we need access to the local
	// variables self, i and j.
	if(Context->isA("Table")) {
		// A bit of a hack, but we need either IndexError or len() from __builtins__.
		PyDict_SetItemString(localDict, "__builtins__",
				PyDict_GetItemString(env()->globalDict(), "__builtins__"));
		PyObject *ret = PyRun_String(
				"def col(c,*arg):\n"
				"\ttry: return self.cell(c,arg[0])\n"
				"\texcept(IndexError): return self.cell(c,i)\n",
				Py_file_input, localDict, localDict);
		if (ret)
			Py_DECREF(ret);
		else
			PyErr_Print();
	} else if(Context->isA("Matrix")) {
		// A bit of a hack, but we need either IndexError or len() from __builtins__.
		PyDict_SetItemString(localDict, "__builtins__",
				PyDict_GetItemString(env()->globalDict(), "__builtins__"));
		PyObject *ret = PyRun_String(
				"def cell(*arg):\n"
				"\ttry: return self.cell(arg[0],arg[1])\n"
				"\texcept(IndexError): return self.cell(i,j)\n",
				Py_file_input, localDict, localDict);
		if (ret)
			Py_DECREF(ret);
		else
			PyErr_Print();
	}
	bool success=false;
	Py_XDECREF(PyCode);
	// Simplest case: Code is a single expression
	PyCode = Py_CompileString(Code.ascii(), Name, Py_eval_input);
	if (PyCode) {
		success = true;
	} else if (for_eval) {
		// Code contains statements (or errors) and we want to get a return
		// value from it.
		// So we wrap the code into a function definition,
		// execute that (as Py_file_input) and store the function object in PyCode.
		// See http://mail.python.org/pipermail/python-list/2001-June/046940.html
		// for why there isn't an easier way to do this in Python.
		PyErr_Clear(); // silently ignore errors
		PyObject *key, *value;
#if PY_VERSION_HEX >= 0x02050000
		Py_ssize_t i=0;
#else
		int i=0;
#endif
		QString signature = "";
		while(PyDict_Next(localDict, &i, &key, &value))
			signature.append(PyString_AsString(key)).append(",");
		signature.truncate(signature.length()-1);
		QString fdef = "def __doit__("+signature+"):\n";
		fdef.append(Code);
		fdef.replace('\n',"\n\t");
		PyCode = Py_CompileString(fdef, Name, Py_file_input);
		if (PyCode)
		{
			PyObject *tmp = PyDict_New();
			Py_XDECREF(PyEval_EvalCode((PyCodeObject*)PyCode, env()->globalDict(), tmp));
			Py_DECREF(PyCode);
			PyCode = PyDict_GetItemString(tmp,"__doit__");
			Py_XINCREF(PyCode);
			Py_DECREF(tmp);
		}
		success = PyCode != NULL;
	} else {
		// Code contains statements (or errors), but we do not need to get
		// a return value.
		PyErr_Clear(); // silently ignore errors
		PyCode = Py_CompileString(Code.ascii(), Name, Py_file_input);
		success = PyCode != NULL;
	}
	if (!success)
	{
		compiled = compileErr;
		emit_error(env()->errorMsg(), 0);
	} else
		compiled = isCompiled;
	return success;
}

QVariant PythonScript::eval()
{
	if (!isFunction) compiled = notCompiled;
	if (compiled != isCompiled && !compile(true))
		return QVariant();
	PyObject *pyret;
	beginStdoutRedirect();
	if (PyCallable_Check(PyCode))
	{
		PyObject *empty_tuple = PyTuple_New(0);
		pyret = PyObject_Call(PyCode, empty_tuple, localDict);
		Py_DECREF(empty_tuple);
	} else
		pyret = PyEval_EvalCode((PyCodeObject*)PyCode, env()->globalDict(), localDict);
	endStdoutRedirect();
	if (!pyret)
	{
		emit_error(env()->errorMsg(), 0);
		return QVariant();
	}

	QVariant qret = QVariant();
	/* None */
	if (pyret == Py_None)
		qret = QVariant("");
	/* numeric types */
	else if (PyFloat_Check(pyret))
		qret = QVariant(PyFloat_AS_DOUBLE(pyret));
	else if (PyInt_Check(pyret))
		qret = QVariant((qlonglong)PyInt_AS_LONG(pyret));
	else if (PyLong_Check(pyret))
		qret = QVariant((qlonglong)PyLong_AsLongLong(pyret));
	else if (PyNumber_Check(pyret))
	{
		PyObject *number = PyNumber_Float(pyret);
		if (number)
		{
			qret = QVariant(PyFloat_AS_DOUBLE(number));
			Py_DECREF(number);
		}
		/* bool */
	} else if (PyBool_Check(pyret))
		qret = QVariant(pyret==Py_True, 0);
	// could handle advanced types (such as PyList->QValueList) here if needed
	/* fallback: try to convert to (unicode) string */
	if(!qret.isValid()) {
		PyObject *pystring = PyObject_Unicode(pyret);
		if (pystring) {
			PyObject *asUTF8 = PyUnicode_EncodeUTF8(PyUnicode_AS_UNICODE(pystring), PyUnicode_GET_DATA_SIZE(pystring), 0);
			Py_DECREF(pystring);
			if (asUTF8) {
				qret = QVariant(QString::fromUtf8(PyString_AS_STRING(asUTF8)));
				Py_DECREF(asUTF8);
			} else if (pystring = PyObject_Str(pyret)) {
				qret = QVariant(QString(PyString_AS_STRING(pystring)));
				Py_DECREF(pystring);
			}
		}
	}

	Py_DECREF(pyret);
	if (PyErr_Occurred()) {
		emit_error(env()->errorMsg(), 0);
		return QVariant();
	} else
		return qret;
}

bool PythonScript::exec()
{
	if (isFunction) compiled = notCompiled;
	if (compiled != Script::isCompiled && !compile(false))
		return false;
	PyObject *pyret;
	beginStdoutRedirect();
	if (PyCallable_Check(PyCode))
	{
		PyObject *empty_tuple = PyTuple_New(0);
		if (!empty_tuple) {
			emit_error(env()->errorMsg(), 0);
			return false;
		}
		pyret = PyObject_Call(PyCode,empty_tuple,localDict);
		Py_DECREF(empty_tuple);
	} else
		pyret = PyEval_EvalCode((PyCodeObject*)PyCode, env()->globalDict(), localDict);
	endStdoutRedirect();
	if (pyret) {
		Py_DECREF(pyret);
		return true;
	}
	emit_error(env()->errorMsg(), 0);
	return false;
}

void PythonScript::beginStdoutRedirect()
{
	stdoutSave = PyDict_GetItemString(env()->sysDict(), "stdout");
	Py_XINCREF(stdoutSave);
	stderrSave = PyDict_GetItemString(env()->sysDict(), "stderr");
	Py_XINCREF(stderrSave);
	env()->setQObject(this, "stdout", env()->sysDict());
	env()->setQObject(this, "stderr", env()->sysDict());
}

void PythonScript::endStdoutRedirect()
{
	PyDict_SetItemString(env()->sysDict(), "stdout", stdoutSave);
	Py_XDECREF(stdoutSave);
	PyDict_SetItemString(env()->sysDict(), "stderr", stderrSave);
	Py_XDECREF(stderrSave);
}

bool PythonScript::setQObject(QObject *val, const char *name)
{
	if (!PyDict_Contains(localDict, PyString_FromString(name)))
		compiled = notCompiled;
	return env()->setQObject(val, name, localDict);
}

bool PythonScript::setInt(int val, const char *name)
{
	if (!PyDict_Contains(localDict, PyString_FromString(name)))
		compiled = notCompiled;
	return env()->setInt(val, name, localDict);
}

bool PythonScript::setDouble(double val, const char *name)
{
	if (!PyDict_Contains(localDict, PyString_FromString(name)))
		compiled = notCompiled;
	return env()->setDouble(val, name, localDict);
}

