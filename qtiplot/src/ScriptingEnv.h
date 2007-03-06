/***************************************************************************
    File                 : ScriptingEnv.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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
#ifndef SCRIPTINGENV_H
#define SCRIPTINGENV_H

#include <QVariant>
#include <QString>
#include <QStringList>
#include <QObject>
#include <QStringList>
#include <QEvent>

#include "customevents.h"

class ApplicationWindow;
class Script;

//! An interpreter for evaluating scripting code. Abstract.
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
    //! Part of the initialization is deferred from the constructor until after the signals have been connected.
    virtual bool initialize() { return true; };
    //! initialization of the interpreter may fail; or there could be other errors setting up the environment
    bool initialized() const { return d_initialized; }
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
};

