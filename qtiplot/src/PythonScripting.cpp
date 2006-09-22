/***************************************************************************
	File                 : PythonScripting.cpp
	Project              : QtiPlot
--------------------------------------------------------------------
	Copyright            : (C) 2006 by Knut Franke
	Email                : knut.franke@gmx.de
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
#include <compile.h>
#include <eval.h>
#include <frameobject.h>
#include <traceback.h>

#if PY_VERSION_HEX < 0x020400A1
typedef struct _traceback {
	PyObject_HEAD
		struct _traceback *tb_next;
	PyFrameObject *tb_frame;
	int tb_lasti;
	int tb_lineno;
} PyTracebackObject;
#endif

#include "PythonScripting.h"
#include "sipAPIqti.h"
extern "C" void initqti();
#include "application.h"

#include <qobject.h>
#include <qstringlist.h>
#include <qvariant.h>
#include <qdir.h>
#include <QDateTime>

const char* PythonScripting::langName = "Python";

QString PythonScripting::toString(PyObject *object, bool decref)
{
	QString ret;
	if (!object) return "";
	PyObject *repr = PyObject_Str(object);
	if (decref) Py_DECREF(object);
	if (!repr) return "";
	ret = PyString_AsString(repr);
	Py_DECREF(repr);
	return ret;
}

PyObject *PythonScripting::eval(const QString &code, PyObject *argDict, const char *name)
{
	PyObject *args;
	if (argDict)
	{
		Py_INCREF(argDict);
		args = argDict;
	} else
		args = PyDict_New();
	PyObject *ret=NULL;
	PyObject *co = Py_CompileString(code.ascii(), name, Py_eval_input);
	if (co)
	{
		ret = PyEval_EvalCode((PyCodeObject*)co, globals, args);
		Py_DECREF(co);
	}
	Py_DECREF(args);
	return ret;
}

bool PythonScripting::exec (const QString &code, PyObject *argDict, const char *name)
{
	PyObject *args;
	if (argDict)
	{
		Py_INCREF(argDict);
		args = argDict;
	} else
		args = PyDict_New();
	PyObject *tmp = NULL;
	PyObject *co = Py_CompileString(code.ascii(), name, Py_file_input);
	if (co)
	{
		tmp = PyEval_EvalCode((PyCodeObject*)co, globals, args);
		Py_DECREF(co);
	}
	Py_DECREF(args);
	if (!tmp) return false;
	Py_DECREF(tmp);
	return true;
}

QString PythonScripting::errorMsg()
{
	PyObject *exception=0, *value=0, *traceback=0;
	PyTracebackObject *excit=0;
	PyFrameObject *frame;
	char *fname;
	QString msg;
	if (!PyErr_Occurred()) return "";

	PyErr_Fetch(&exception, &value, &traceback);
	PyErr_NormalizeException(&exception, &value, &traceback);
	if(PyErr_GivenExceptionMatches(exception, PyExc_SyntaxError))
	{
		msg.append(toString(PyObject_GetAttrString(value, "text"), true) + "\n");
		PyObject *offset = PyObject_GetAttrString(value, "offset");
		for (int i=1; i<PyInt_AsLong(offset); i++)
			msg.append(" ");
		msg.append("^\n");
		Py_DECREF(offset);
		msg.append("SyntaxError: ");
		msg.append(toString(PyObject_GetAttrString(value, "msg"), true) + "\n");
		msg.append("at ").append(toString(PyObject_GetAttrString(value, "filename"), true));
		msg.append(":").append(toString(PyObject_GetAttrString(value, "lineno"), true));
		msg.append("\n");
		Py_DECREF(exception);
		Py_DECREF(value);
	} else {
		msg.append(toString(exception,true)).remove("exceptions.").append(": ");
		msg.append(toString(value,true));
		msg.append("\n");
	}

	if (traceback) {
		excit = (PyTracebackObject*)traceback;
		while (excit && (PyObject*)excit != Py_None)
		{
			frame = excit->tb_frame;
			msg.append("at ").append(PyString_AsString(frame->f_code->co_filename));
			msg.append(":").append(QString::number(excit->tb_lineno));
			if (frame->f_code->co_name && *(fname = PyString_AsString(frame->f_code->co_name)) != '?')
				msg.append(" in ").append(fname);
			msg.append("\n");
			excit = excit->tb_next;
		}
		Py_DECREF(traceback);
	}

	return msg;
}

	PythonScripting::PythonScripting(ApplicationWindow *parent)
: ScriptingEnv(parent, langName)
{
	PyObject *mainmod=NULL, *qtimod=NULL, *sysmod=NULL;
	math = NULL;
	sys = NULL;
	if (Py_IsInitialized())
	{
		PyEval_AcquireLock();
		mainmod = PyImport_ImportModule("__main__");
		if (!mainmod)
		{
			PyErr_Print();
			PyEval_ReleaseLock();
			return;
		}
		globals = PyModule_GetDict(mainmod);
		Py_DECREF(mainmod);
	} else {
		PyEval_InitThreads ();
		Py_Initialize ();
		if (!Py_IsInitialized ())
			return;
		initqti();

		mainmod = PyImport_AddModule("__main__");
		if (!mainmod)
		{
			PyEval_ReleaseLock();
			PyErr_Print();
			return;
		}
		globals = PyModule_GetDict(mainmod);
	}

	if (!globals)
	{
		PyErr_Print();
		PyEval_ReleaseLock();
		return;
	}
	Py_INCREF(globals);

	math = PyDict_New();
	if (!math)
		PyErr_Print();

	qtimod = PyImport_ImportModule("qti");
	if (qtimod)
	{
		PyDict_SetItemString(globals, "qti", qtimod);
		PyObject *qtiDict = PyModule_GetDict(qtimod);
		setQObject(Parent, "app", qtiDict);
		PyDict_SetItemString(qtiDict, "mathFunctions", math);
		Py_DECREF(qtimod);
	} else
		PyErr_Print();

	sysmod = PyImport_ImportModule("sys");
	if (sysmod)
	{
		sys = PyModule_GetDict(sysmod);
		Py_INCREF(sys);
	} else
		PyErr_Print();

	PyEval_ReleaseLock();

#ifdef Q_WS_WIN
	loadInitFile(QDir::homeDirPath()+"qtiplotrc") ||
		loadInitFile(qApp->applicationDirPath()+"qtiplotrc") ||
#else
		loadInitFile(QDir::homeDirPath()+"/.qtiplotrc") ||
		loadInitFile(QDir::rootDirPath()+"etc/qtiplotrc") ||
#endif
		loadInitFile("qtiplotrc");

	initialized=true;
}

PythonScripting::~PythonScripting()
{
	Py_XDECREF(globals);
	Py_XDECREF(math);
	Py_XDECREF(sys);
}

bool PythonScripting::loadInitFile(const QString &path)
{
	QFileInfo pyFile(path+".py"), pycFile(path+".pyc");
	if (pycFile.isReadable() && (pycFile.lastModified() >= pyFile.lastModified()))
		return PyRun_SimpleFileEx(fopen(pycFile.filePath(), "rb"), pycFile.filePath(), true) == 0;
	else if (pyFile.isReadable() && pyFile.exists()) {
		// try to compile pyFile to pycFile
		PyObject *compileModule = PyImport_ImportModule("py_compile");
		if (compileModule) {
			PyObject *compile = PyDict_GetItemString(PyModule_GetDict(compileModule),"compile");
			if (compile) {
				PyObject *tmp = PyObject_CallFunctionObjArgs(compile, PyString_FromString(pyFile.filePath()), PyString_FromString(pycFile.filePath()),NULL);
				if (tmp)
					Py_DECREF(tmp);
				else
					PyErr_Print();
			} else
				PyErr_Print();
			Py_DECREF(compileModule);
		} else
			PyErr_Print();
		if (pycFile.isReadable() && (pycFile.lastModified() >= pyFile.lastModified())) {
			// run the newly compiled pycFile
			return PyRun_SimpleFileEx(fopen(pycFile.filePath(), "rb"), pycFile.filePath(), true) == 0;
		} else {
			// fallback: just run pyFile
			return PyRun_SimpleFileEx(fopen(pyFile.filePath(), "r"), pyFile.filePath(), true) == 0;
		}
	}
	return false;
}

bool PythonScripting::isRunning() const
{
	return Py_IsInitialized();
}

bool PythonScripting::setQObject(QObject *val, const char *name, PyObject *dict)
{
	if(!val) return false;
	PyObject *pyobj=NULL;
	sipTypeDef *t;
#if SIP_VERSION >= 0x040301 && SIP_VERSION <= 0x040403
#error "SIP versions between 4.3.1 and 4.4.3 are currently not supported. Please install another version of SIP and try again."
#endif
#if SIP_VERSION >= 0x040400
	for (int i=0; i<sipModuleAPI_qti.em_nrtypes; i++)
#else
		for (int i=0; sipModuleAPI_qti.em_types[i] != 0; i++)
#endif
			// Note that the SIP API is a bit confusing here.
			// sipTypeDef.td_cname holds the C++ class name, but is NULL if that's the same as the Python class name.
			// sipTypeDef.td_name OTOH always holds the Python class name, but prepended by the module name ("qti.")
			if (((t=sipModuleAPI_qti.em_types[i]->type)->td_cname && !strcmp(val->className(),t->td_cname)) ||
					(!t->td_cname && !strcmp(val->className(),t->td_name+4)))
			{
#if SIP_VERSION >= 0x040400
				pyobj=sipConvertFromInstance(val,sipModuleAPI_qti.em_types[i],NULL);
#else
				pyobj=sipBuildResult(NULL, "M", val, sipModuleAPI_qti.em_types[i]);
#endif
				if (!pyobj) return false;
				break;
			}
	if (!pyobj) {
#if SIP_VERSION >= 0x040400
		for (int i=0; i<sipModuleAPI_qti_QtCore->em_nrtypes; i++)
#else
			for (int i=0; sipModuleAPI_qti_QtCore->em_types[i] != 0; i++)
#endif
				if (((t=sipModuleAPI_qti_QtCore->em_types[i]->type)->td_cname && !strcmp(val->className(),t->td_cname)) ||
						(!t->td_cname && !strcmp(val->className(),t->td_name+3)))
				{
#if SIP_VERSION >= 0x040400
					pyobj=sipConvertFromInstance(val,sipModuleAPI_qti_QtCore->em_types[i],NULL);
#else
					pyobj=sipBuildResult(NULL, "M", val, sipModuleAPI_qti_QtCore->em_types[i]);
#endif
					if (!pyobj) return false;
					break;
				}
	} 
	if (!pyobj) return false;

	if (dict)
		PyDict_SetItemString(dict,name,pyobj);
	else
		PyDict_SetItemString(globals,name,pyobj);
	Py_DECREF(pyobj);
	return true;
}

bool PythonScripting::setInt(int val, const char *name, PyObject *dict)
{
	PyObject *pyobj = Py_BuildValue("i",val);
	if (!pyobj) return false;
	if (dict)
		PyDict_SetItemString(dict,name,pyobj);
	else
		PyDict_SetItemString(globals,name,pyobj);
	Py_DECREF(pyobj);
	return true;
}

bool PythonScripting::setDouble(double val, const char *name, PyObject *dict)
{
	PyObject *pyobj = Py_BuildValue("d",val);
	if (!pyobj) return false;
	if (dict)
		PyDict_SetItemString(dict,name,pyobj);
	else
		PyDict_SetItemString(globals,name,pyobj);
	Py_DECREF(pyobj);
	return true;
}

const QStringList PythonScripting::mathFunctions() const
{
	QStringList flist;
	PyObject *key, *value;
	int i=0;
	while(PyDict_Next(math, &i, &key, &value))
		if (PyCallable_Check(value))
			flist << PyString_AsString(key);
	flist.sort();
	return flist;
}

const QString PythonScripting::mathFunctionDoc(const QString &name) const
{
	PyObject *mathf = PyDict_GetItemString(math,name); // borrowed
	if (!mathf) return "";
	PyObject *pydocstr = PyObject_GetAttrString(mathf, "__doc__"); // new
	QString qdocstr = PyString_AsString(pydocstr);
	Py_XDECREF(pydocstr);
	return qdocstr;
}

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
	if(Context->isA("Table")) {
		PyDict_SetItemString(localDict,"__builtins__",PyDict_GetItemString(env()->globalDict(),"__builtins__"));
		PyObject *ret = PyRun_String("def col(c,*arg):\n\ttry: return self.cell(c,arg[0])\n\texcept(IndexError): return self.cell(c,i)\n",Py_file_input,localDict,localDict);
		if (ret)
			Py_DECREF(ret);
		else
			PyErr_Print();
	} else if(Context->isA("Matrix")) {
		PyDict_SetItemString(localDict,"__builtins__",PyDict_GetItemString(env()->globalDict(),"__builtins__"));
		PyObject *ret = PyRun_String("def cell(*arg):\n\ttry: return self.cell(arg[0],arg[1])\n\texcept(IndexError): return self.cell(i,j)\n",Py_file_input,localDict,localDict);
		if (ret)
			Py_DECREF(ret);
		else
			PyErr_Print();
	}
	bool success=false;
	Py_XDECREF(PyCode);
	PyCode = Py_CompileString(Code.ascii(),Name,Py_eval_input);
	if (PyCode) { // code is a single expression
		success = true;
	} else if (for_eval) { // code contains statements
		PyErr_Clear();
		PyObject *key, *value;
		int i=0;
		QString signature = "";
		while(PyDict_Next(localDict, &i, &key, &value))
			signature.append(PyString_AsString(key)).append(",");
		signature.truncate(signature.length()-1);
		QString fdef = "def __doit__("+signature+"):\n";
		fdef.append(Code);
		fdef.replace('\n',"\n\t");
		PyCode = Py_CompileString(fdef,Name,Py_file_input);
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
		PyErr_Clear();
		PyCode = Py_CompileString(Code.ascii(),Name,Py_file_input);
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
		qret = QVariant((Q_LLONG)PyInt_AS_LONG(pyret));
	else if (PyLong_Check(pyret))
		qret = QVariant((Q_LLONG)PyLong_AsLongLong(pyret));
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
	PyDict_SetItemString(env()->sysDict(), "stderr", stderrSave);
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

