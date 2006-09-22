/***************************************************************************
    File                 : Scripting.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Scripting abstraction layer
                           
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
#ifndef SCRIPTING_H
#define SCRIPTING_H

#include <QVariant>
#include <QString>
#include <q3valuelist.h>
#include <QStringList>
#include <QObject>
#include <QStringList>
#include <QEvent>

#include "customEvents.h"

class ApplicationWindow;
class Script;

//! An interpreter for evaluting scripting code. Abstract.
  /**
   * ScriptingEnv objects represent a running interpreter, possibly with global
   * variables, and are responsible for generating Script objects (which do
   * the actual evaluation of code).
   */
class ScriptingEnv : public QObject
{
  Q_OBJECT

  public:
    ScriptingEnv(ApplicationWindow *parent, const char *langName);
    //! initialization of the interpreter may fail; or there could be other errors setting up the environment
    bool isInitialized() const { return initialized; }
    //! whether asynchronuous execution is enabled (if supported by the implementation)
    virtual bool isRunning() const { return false; }
    
    //! Instantiate the Script subclass matching the ScriptEnv subclass.
    virtual Script *newScript(const QString&, QObject*, const QString&) { return 0; }
      
    //! If an exception / error occured, return a nicely formated stack backtrace.
    virtual QString stackTraceString() { return QString::null; }

    //! Return a list of supported mathematical functions. These should be imported into the global namespace.
    virtual const QStringList mathFunctions() const { return QStringList(); }
    //! Return a documentation string for the given mathematical function.
    virtual const QString mathFunctionDoc(const QString&) const { return QString::null; }

//    virtual QSyntaxHighlighter syntaxHighlighter(QTextEdit *textEdit) const;

  public slots:
    // global variables
    virtual bool setQObject(QObject*, const char*) { return false; }
    virtual bool setInt(int, const char*) { return false; }
    virtual bool setDouble(double, const char*) { return false; }

    //! Clear the global environment. What exactly happens depends on the implementation.
    virtual void clear() {}
    //! If the implementation supports asynchronuos execution, deactivate it.
    virtual void stopExecution() {}

    //! Increase the reference count. This should only be called by scripted and Script to avoid memory leaks.
    void incref();
    //! Decrease the reference count. This should only be called by scripted and Script to avoid segfaults.
    void decref();

  signals:
    //! signal an error condition / exception
    void error(const QString & message, const QString & scriptName, int lineNumber);
    //! output that is not handled by a Script
    void print(const QString & output);
    
  protected:
    //! whether the interpreter has been successfully initialized
    bool initialized;
    //! the context in which we are running
    ApplicationWindow *Parent;

  private:
    //! the reference counter
    int refcount;
};

//! A chunk of scripting code. Abstract.
  /**
   * Script objects represent a chunk of code, possibly together with local
   * variables. The code may be changed and executed multiple times during the
   * lifetime of an object.
   */
class Script : public QObject
{
  Q_OBJECT

  public:
    Script(ScriptingEnv *env, const QString &code, QObject *context=0, const QString &name="<input>")
      : Env(env), Code(code), Name(name), compiled(notCompiled)
      { Env->incref(); Context = context; EmitErrors=true; }
    ~Script() { Env->decref(); }

    //! Return the code that will be executed when calling exec() or eval()
    const QString code() const { return Code; }
    //! Return the context in which the code is to be executed.
    const QObject* context() const { return Context; }
    //! Like QObject::name, but with unicode support.
    const QString name() const { return Name; }
    //! Return whether errors / exceptions are to be emitted or silently ignored
    const bool emitErrors() const { return EmitErrors; }
    //! Append to the code that will be executed when calling exec() or eval()
    virtual void addCode(const QString &code) { Code.append(code); compiled = notCompiled; emit codeChanged(); }
    //! Set the code that will be executed when calling exec() or eval()
    virtual void setCode(const QString &code) { Code=code; compiled = notCompiled; emit codeChanged(); }
    //! Set the context in which the code is to be executed.
    virtual void setContext(QObject *context) { Context = context; compiled = notCompiled; }
    //! Like QObject::setName, but with unicode support.
    void setName(const QString &name) { Name = name; compiled = notCompiled; }
    //! Set whether errors / exceptions are to be emitted or silently ignored
    void setEmitErrors(bool yes) { EmitErrors = yes; }

  public slots:
    //! Compile the Code. Return true if the implementation doesn't support compilation.
    virtual bool compile(bool for_eval=true);
    //! Evaluate the Code, returning QVariant() on an error / exception.
    virtual QVariant eval();
    //! Execute the Code, returning false on an error / exception.
    virtual bool exec();

    // local variables
    virtual bool setQObject(const QObject*, const char*) { return false; }
    virtual bool setInt(int, const char*) { return false; }
    virtual bool setDouble(double, const char*) { return false; }

  signals:
    //! This is emitted whenever the code to be executed by exec() and eval() is changed.
    void codeChanged();
    //! signal an error condition / exception
    void error(const QString & message, const QString & scriptName, int lineNumber);
    //! output generated by the code
    void print(const QString & output);
    
  protected:
    ScriptingEnv *Env;
    QString Code, Name;
    QObject *Context;
    enum compileStatus { notCompiled, isCompiled, compileErr } compiled;
    bool EmitErrors;

    void emit_error(const QString & message, int lineNumber)
      { if(EmitErrors) emit error(message, Name, lineNumber); }
};

//! keeps a static list of available interpreters and instantiates them on demand
class ScriptingLangManager
{
  public:
    //! Return an instance of the first implementation we can find.
    static ScriptingEnv *newEnv(ApplicationWindow *parent);
    //! Return an instance of the implementation specified by name, NULL on failure.
    static ScriptingEnv *newEnv(const char *name, ApplicationWindow *parent);
    //! Return the names of available implementations.
    static QStringList languages();
    //! Return the number of available implementations.
    static int numLanguages();

  private:
    typedef ScriptingEnv*(*ScriptingEnvConstructor)(ApplicationWindow*);
    typedef struct {
      const char *name;
      ScriptingEnvConstructor constructor;
    } ScriptingLang;
//! global registry of available languages
    static ScriptingLang langs[];
};

//! notify an object that it should update its scripting environment (see class scripted)
class ScriptingChangeEvent : public QEvent
{
  public:
    ScriptingChangeEvent(ScriptingEnv *e) : QEvent(SCRIPTING_CHANGE_EVENT), env(e) {}
    ScriptingEnv *scriptingEnv() const { return env; }
    Type type() const { return SCRIPTING_CHANGE_EVENT; }
  private:
    ScriptingEnv *env;
};

//! Interface for maintaining a reference to the current ScriptingEnv
  /**
   * Every class that wants to use a ScriptingEnv should subclass this one and
   * implement slot customEvent(QEvent*) that forwards any
   * ScriptingChangeEvents to scripted::scriptingChangeEvent.
   */
class scripted
{
  public:
   scripted(ScriptingEnv* env);
   ~scripted();
   void scriptingChangeEvent(ScriptingChangeEvent*);
  protected:
    ScriptingEnv *scriptEnv;
};

#endif

