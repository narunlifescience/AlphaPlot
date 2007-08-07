/***************************************************************************
    File                 : AbstractScriptingEngine.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
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
#ifndef ABSTRACT_SCRIPTING_ENGINE_H
#define ABSTRACT_SCRIPTING_ENGINE_H

#include <QString>
#include <QStringList>
#include <QObject>
#include <QEvent>

#include "customevents.h"

class ApplicationWindow;
class AbstractScript;

//! An interpreter for evaluating scripting code.
  /**
   * AbstractScriptingEngine objects represent a running interpreter, possibly with global
   * variables, and are responsible for generating Script objects (which do
   * the actual evaluation of code).
	*
	* The class also keeps a static list of available interpreters and instantiates
	* them on demand.
   */
class AbstractScriptingEngine : public QObject
{
  Q_OBJECT

  public:
    AbstractScriptingEngine(ApplicationWindow *parent, const char *lang_name);
    //! Part of the initialization is deferred from the constructor until after the signals have been connected.
    virtual bool initialize() { return true; };
    //! initialization of the interpreter may fail; or there could be other errors setting up the environment
    bool initialized() const { return d_initialized; }
    //! whether asynchronuous execution is enabled (if supported by the implementation)
    virtual bool isRunning() const { return false; }
    
    //! Instantiate the AbstractScript subclass matching the AbstractScriptingEngine subclass.
    virtual AbstractScript *newScript(const QString&, QObject*, const QString&) { return 0; }
      
    //! If an exception / error occured, return a nicely formated stack backtrace.
    virtual QString stackTraceString() { return QString::null; }

    //! Return a list of supported mathematical functions. These should be imported into the global namespace.
    virtual const QStringList mathFunctions() const { return QStringList(); }
    //! Return a documentation string for the given mathematical function.
    virtual const QString mathFunctionDoc(const QString&) const { return QString::null; }
    //! Return a list of file extensions commonly used for this language.
    virtual const QStringList fileExtensions() const { return QStringList(); };
    //! Construct a filter expression from fileExtension(), suitable for QFileDialog.
    const QString fileFilter() const;

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
    //! If the implementation supports asynchronuos execution, activate it.
    virtual void startExecution() {}

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
    bool d_initialized;
    //! the context in which we are running
    ApplicationWindow *d_parent;

  private:
    //! the reference counter
    int d_refcount;

  public:
    //! Return an instance of the first implementation we can find.
    static AbstractScriptingEngine *create(ApplicationWindow *parent);
    //! Return an instance of the implementation specified by name, NULL on failure.
    static AbstractScriptingEngine *create(const char *name, ApplicationWindow *parent);
    //! Return the names of available implementations.
    static QStringList engineNames();
    //! Return the number of available implementations.
    static int engineCount();

  private:
    typedef AbstractScriptingEngine*(*ScriptingEngineConstructor)(ApplicationWindow*);
    typedef struct {
      const char *name;
      ScriptingEngineConstructor constructor;
    } ScriptingEngineEntry;
	 //! global registry of available interpreters
    static ScriptingEngineEntry g_engines[];
};

/******************************************************************************\
 *Helper classes for managing instances of AbstractScriptingEngine subclasses.*
\******************************************************************************/

//! notify an object that it should update its scripting environment (see class scripted)
class ScriptingChangeEvent : public QEvent
{
  public:
    ScriptingChangeEvent(AbstractScriptingEngine *engine) : QEvent(SCRIPTING_CHANGE_EVENT), d_scripting_engine(engine) {}
    AbstractScriptingEngine *scriptingEngine() const { return d_scripting_engine; }
    Type type() const { return SCRIPTING_CHANGE_EVENT; }
  private:
    AbstractScriptingEngine *d_scripting_engine;
};

//! Interface for maintaining a reference to the current AbstractScriptingEngine
  /**
   * Every class that wants to use a AbstractScriptingEngine should subclass this one and
   * implement slot customEvent(QEvent*) such that it forwards any
   * AbstractScriptingChangeEvents to scripted::scriptingChangeEvent.
   */
class scripted
{
  public:
   scripted(AbstractScriptingEngine* engine);
   ~scripted();
   void scriptingChangeEvent(ScriptingChangeEvent*);
  protected:
    AbstractScriptingEngine *d_scripting_engine;
};

#endif // ifndef ABSTRACT_SCRIPTING_ENGINE_H
