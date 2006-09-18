/***************************************************************************
    File                 : Scripting.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Scripting classes
                           
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

#include <qvariant.h>
#include <qstring.h>
#include <q3valuelist.h>
#include <qobject.h>
#include <QStringList>

class ApplicationWindow;
class ScriptingEnv;

//! Roughly equivalent to QSScript
  /**
   * roughly equivalent to QSScript, but can execute itself (via Env in QSA)
   * this is so that you do not have to explicitly pass the ScriptingEnv 
   * to everyone who wants to execute a Script
   */
class Script : public QObject
{
  // roughly equivalent to QSScript, but can execute itself (via Env in QSA)
  // this is so that you do not have to explicitly pass the ScriptingEnv to everyone who wants to execute a Script
  Q_OBJECT

  public:
    Script(ScriptingEnv *env, const QString &code, QObject *context=0, const QString &name="<input>")
      : Env(env), Code(code), Name(name), compiled(notCompiled)
      { Context = context; EmitErrors=true; }

    const QString code() const { return Code; }
    const QObject* context() const { return Context; }
    const QString name() const { return Name; }
    const bool emitErrors() const { return EmitErrors; }
    virtual void addCode(const QString &code) { Code.append(code); compiled = notCompiled; emit codeChanged(); }
    virtual void setCode(const QString &code) { Code=code; compiled = notCompiled; emit codeChanged(); }
    virtual void setContext(QObject *context) { Context = context; compiled = notCompiled; }
    void setName(const QString &name) { Name = name; compiled = notCompiled; }
    void setEmitErrors(bool yes) { EmitErrors = yes; }

  public slots:
    virtual bool compile() { return false; }
    virtual QVariant eval() { return QVariant(); }
    virtual bool exec() { return false; }

    // local variables
    virtual bool setQObject(const QObject*, const char*) { return false; } // name should default to val->name()
    virtual bool setInt(int, const char*) { return false; }
    virtual bool setDouble(double, const char*) { return false; }

  signals:
    void codeChanged();
    void error(const QString & message, const QString & scriptName, int lineNumber);
    
  protected:
    ScriptingEnv *Env;
    QString Code, Name;
    QObject *Context;
    enum compileStatus { notCompiled, isCompiled, compileErr } compiled;
    bool EmitErrors;

    void emit_error(const QString & message, int lineNumber)
      { if(EmitErrors) emit error(message, Name, lineNumber); }
};

//! roughly equivalent to QSInterpreter
class ScriptingEnv : public QObject
{
  // roughly equivalent to QSInterpreter
  Q_OBJECT

  public:
    ScriptingEnv(ApplicationWindow *parent) : Parent(parent) { initialized=false; }
    // Python initialization may fail; or there could be other errors setting up the environment
    bool isInitialized() const { return initialized; }
    virtual bool isRunning() const { return false; }
    
    virtual Script *newScript(const QString&, QObject*, const QString&) { return 0; }
      
    // global variables
    virtual bool setQObject(QObject*, const char*) { return false; } // name should default to val->name()
    virtual bool setInt(int, const char*) { return false; }
    virtual bool setDouble(double, const char*) { return false; }

    virtual QString stackTraceString() { return QString::null; }

    virtual const QStringList mathFunctions() const { return QStringList(); }
    virtual const QString mathFunctionDoc(const QString&) const { return QString::null; }
//    virtual QSyntaxHighlighter syntaxHighlighter(QTextEdit *textEdit) const;

  public slots:
    virtual void clear() {}
    virtual void stopExecution() {}

  signals:
    void error(const QString & message, const QString & scriptName, int lineNumber);
    
  protected:
    bool initialized;
    ApplicationWindow *Parent;
};

#endif

