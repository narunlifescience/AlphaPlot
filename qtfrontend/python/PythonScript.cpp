/***************************************************************************
	File                 : PythonScript.cpp
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
// get rid of a compiler warning
#ifdef _POSIX_C_SOURCE
#undef _POSIX_C_SOURCE
#endif
#include <Python.h>

#include "PythonScript.h"
#include "PythonScriptingEngine.h"
#include "../core/ApplicationWindow.h"

#include <QObject>
#include <QVariant>

PythonScript::PythonScript(PythonScriptingEngine *engine, const QString &code, QObject *context, const QString &name)
: AbstractScript(engine, code, context, name)
{
	m_py_code = NULL;
	m_local_dict = PyDict_New();
	setQObject(m_context, "self");
}

PythonScript::~PythonScript()
{
	Py_DECREF(m_local_dict);
	Py_XDECREF(m_py_code);
}

void PythonScript::setContext(QObject *context)
{
	AbstractScript::setContext(context);
	setQObject(m_context, "self");
}

bool PythonScript::compile(bool for_eval)
{
	// Support for the convenient col() and cell() functions.
	// This can't be done anywhere else, because we need access to the local
	// variables self, i and j.
	if(m_context->inherits("Table")) {
		// A bit of a hack, but we need either IndexError or len() from __builtins__.
		PyDict_SetItemString(m_local_dict, "__builtins__",
				PyDict_GetItemString(engine()->globalDict(), "__builtins__"));
		PyObject *ret = PyRun_String(
				"def col(c,*arg):\n"
				"\ttry: return self.cell(c,arg[0])\n"
				"\texcept(IndexError): return self.cell(c,i)\n"
				"def cell(c,r):\n"
				"\treturn self.cell(c,r)\n"
				"def tablecol(t,c):\n"
				"\treturn self.folder().rootFolder().table(t,True).cell(c,i)\n"
				"def _meth_table_col_(t,c):\n"
				"\treturn t.cell(c,i)\n"
				"self.__class__.col = _meth_table_col_",
				Py_file_input, m_local_dict, m_local_dict);
		if (ret)
			Py_DECREF(ret);
		else
			PyErr_Print();
	} else if(m_context->inherits("Matrix")) {
		// A bit of a hack, but we need either IndexError or len() from __builtins__.
		PyDict_SetItemString(m_local_dict, "__builtins__",
				PyDict_GetItemString(engine()->globalDict(), "__builtins__"));
		PyObject *ret = PyRun_String(
				"def cell(*arg):\n"
				"\ttry: return self.cell(arg[0],arg[1])\n"
				"\texcept(IndexError): return self.cell(i,j)\n",
				Py_file_input, m_local_dict, m_local_dict);
		if (ret)
			Py_DECREF(ret);
		else
			PyErr_Print();
	}
	bool success=false;
	Py_XDECREF(m_py_code);
	// Simplest case: Code is a single expression
	m_py_code = Py_CompileString(m_code.toAscii(), m_name.toAscii(), Py_eval_input);
	if (m_py_code) {
		success = true;
	} else if (for_eval) {
		// Code contains statements (or errors) and we want to get a return
		// value from it.
		// So we wrap the code into a function definition,
		// execute that (as Py_file_input) and store the function object in m_py_code.
		// See http://mail.python.org/pipermail/python-bugs-list/2001-June/005534.html
		// and http://mail.python.org/pipermail/python-list/2001-June/087605.html
		// for why there isn't an easier way to do this in Python.
		PyErr_Clear(); // silently ignore errors
		PyObject *key, *value;
#if PY_VERSION_HEX >= 0x02050000
		Py_ssize_t i=0;
#else
		int i=0;
#endif
		QString signature = "";
		while(PyDict_Next(m_local_dict, &i, &key, &value))
			signature.append(PyString_AsString(key)).append(",");
		signature.truncate(signature.length()-1);
		QString fdef = "def __doit__("+signature+"):\n";
		fdef.append(m_code);
		fdef.replace('\n',"\n\t");
		m_py_code = Py_CompileString(fdef.toAscii(), m_name.toAscii(), Py_file_input);
		if (m_py_code)
		{
			PyObject *tmp = PyDict_New();
			Py_XDECREF(PyEval_EvalCode((PyCodeObject*)m_py_code, engine()->globalDict(), tmp));
			Py_DECREF(m_py_code);
			m_py_code = PyDict_GetItemString(tmp,"__doit__");
			Py_XINCREF(m_py_code);
			Py_DECREF(tmp);
		}
		success = m_py_code != NULL;
	} else {
		// Code contains statements (or errors), but we do not need to get
		// a return value.
		PyErr_Clear(); // silently ignore errors
		m_py_code = Py_CompileString(m_code.toAscii(), m_name.toAscii(), Py_file_input);
		success = m_py_code != NULL;
	}
	m_compiled_for_eval = for_eval;
	if (!success)
	{
		m_compiled = compileErr;
		emit_error(engine()->errorMsg(), 0);
	} else
		m_compiled = isCompiled;
	return success;
}

QVariant PythonScript::eval()
{
	if ((m_compiled != isCompiled || !m_compiled_for_eval) && !compile(true))
		return QVariant();
	PyObject *pyret;
	beginStdoutRedirect();
	if (PyCallable_Check(m_py_code))
	{
		PyObject *empty_tuple = PyTuple_New(0);
		pyret = PyObject_Call(m_py_code, empty_tuple, m_local_dict);
		Py_DECREF(empty_tuple);
	} else
		pyret = PyEval_EvalCode((PyCodeObject*)m_py_code, engine()->globalDict(), m_local_dict);
	endStdoutRedirect();
	if (!pyret)
	{
		emit_error(engine()->errorMsg(), 0);
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
		qret = QVariant(pyret==Py_True);
	// could handle advanced types (such as PyList->QList) here if needed
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
		emit_error(engine()->errorMsg(), 0);
		return QVariant();
	} else
		return qret;
}

bool PythonScript::exec()
{
	if ((m_compiled != isCompiled || m_compiled_for_eval) && !compile(false))
		return false;
	PyObject *pyret;
	beginStdoutRedirect();
	if (PyCallable_Check(m_py_code))
	{
		PyObject *empty_tuple = PyTuple_New(0);
		if (!empty_tuple) {
			emit_error(engine()->errorMsg(), 0);
			return false;
		}
		pyret = PyObject_Call(m_py_code,empty_tuple,m_local_dict);
		Py_DECREF(empty_tuple);
	} else
		pyret = PyEval_EvalCode((PyCodeObject*)m_py_code, engine()->globalDict(), m_local_dict);
	endStdoutRedirect();
	if (pyret) {
		Py_DECREF(pyret);
		return true;
	}
	emit_error(engine()->errorMsg(), 0);
	return false;
}

void PythonScript::beginStdoutRedirect()
{
	m_stdout_save = PyDict_GetItemString(engine()->sysDict(), "stdout");
	Py_XINCREF(m_stdout_save);
	m_stderr_save = PyDict_GetItemString(engine()->sysDict(), "stderr");
	Py_XINCREF(m_stderr_save);
	engine()->setQObject(this, "stdout", engine()->sysDict());
	engine()->setQObject(this, "stderr", engine()->sysDict());
}

void PythonScript::endStdoutRedirect()
{
	PyDict_SetItemString(engine()->sysDict(), "stdout", m_stdout_save);
	Py_XDECREF(m_stdout_save);
	PyDict_SetItemString(engine()->sysDict(), "stderr", m_stderr_save);
	Py_XDECREF(m_stderr_save);
}

bool PythonScript::setQObject(QObject *val, const char *name)
{
	if (!PyDict_Contains(m_local_dict, PyString_FromString(name)))
		m_compiled = notCompiled;
	return engine()->setQObject(val, name, m_local_dict);
}

bool PythonScript::setInt(int val, const char *name)
{
	if (!PyDict_Contains(m_local_dict, PyString_FromString(name)))
		m_compiled = notCompiled;
	return engine()->setInt(val, name, m_local_dict);
}

bool PythonScript::setDouble(double val, const char *name)
{
	if (!PyDict_Contains(m_local_dict, PyString_FromString(name)))
		m_compiled = notCompiled;
	return engine()->setDouble(val, name, m_local_dict);
}

PythonScriptingEngine* PythonScript::engine() {
	return static_cast<PythonScriptingEngine*>(m_engine);
}
