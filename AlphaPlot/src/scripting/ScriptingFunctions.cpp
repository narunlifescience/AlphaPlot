#include <QScriptEngine>
#include <QScriptContext>
#include <QFile>
#include <QFileInfo>
#include <QtDebug>

#include "../ApplicationWindow.h"
#include "../Table.h"
#include "scripting/widgets/Console.h"
#include "widgets/ConsoleWidget.h"
#include "ScriptingFunctions.h"

// ScriptingConsole print() function reimplimentation
QScriptValue print(QScriptContext *context, QScriptEngine *egne) {
  if (context->argumentCount() == 0) {
    context->throwError(
        QObject::tr("print() should have atleast one argument"));
    return false;
  }
  QScriptValue result;
  for (int i = 0; i < context->argumentCount(); i++) {
    result = result.toString() + " " + context->argument(i).toString();
    if (context->state() == QScriptContext::ExceptionState) {
      result = result.toString() + " Unhandled Exception";
      break;
    }
  }
  QScriptValue calleeData = context->callee().data();
  Console *console = qobject_cast<Console *>(calleeData.toQObject());
  if (console) {
    console->append(result.toString());
  } else {
    qDebug() << "Scripting console print() unable to access Console object";
    return false;
  }
  return egne->undefinedValue();
}

// ScriptingConsole clear() function
QScriptValue clear(QScriptContext *context, QScriptEngine *egne) {
  if (context->argumentCount() != 0) {
    context->throwError(QObject::tr("clear() take no arguments!"));
    return false;
  }
  QScriptValue calleeData = context->callee().data();
  Console *console = qobject_cast<Console *>(calleeData.toQObject());
  if (console) {
    console->clearConsole();
  } else {
    qDebug() << "Scripting console clear() unable to access Console object";
    return false;
  }
  return egne->undefinedValue();
}

// ScriptingConsole openAproj() function
QScriptValue openProj(QScriptContext *context, QScriptEngine *egne) {
  if (context->argumentCount() > 1) {
    context->throwError(QObject::tr("openAproj(string) take one argument!"));
    return false;
  }
  QScriptValue calleeData = context->callee().data();
  ApplicationWindow *app =
      qobject_cast<ApplicationWindow *>(calleeData.toQObject());
  if (app) {
    if (context->argumentCount() == 0) {
      app->openAproj();
    } else {
      QFileInfo fileInfo(context->argument(0).toString());
      if (fileInfo.exists()) {
        app->openAproj(fileInfo.absoluteFilePath());
      } else {
        context->throwError(QString(fileInfo.absoluteFilePath()) +
                            QObject::tr(" file not found!"));
        return false;
      }
    }
  } else {
    qDebug() << "Scripting console clear() unable to access Console object";
    return false;
  }
  return egne->undefinedValue();
}

QScriptValue collectGarbage(QScriptContext *context, QScriptEngine *egne) {
  if (context->argumentCount() != 0) {
    context->throwError(QObject::tr("collectGarbage() take no arguments!"));
    return false;
  }
  egne->collectGarbage();
  return egne->undefinedValue();
}

QScriptValue attachDebugger(QScriptContext *context, QScriptEngine *egne) {
  if (context->argumentCount() != 1) {
    context->throwError(QObject::tr("attachDebugger(bool) take one argument!"));
    return false;
  }
  if (!context->argument(0).isBool()) {
    context->throwError(
        QObject::tr("argument should be a bool attachDebugger(bool)"));
    return false;
  }
  QScriptValue calleeData = context->callee().data();
  ConsoleWidget *consoleWidget =
      qobject_cast<ConsoleWidget *>(calleeData.toQObject());
  if (consoleWidget) {
    if (context->argument(0).toBool()) {
      consoleWidget->debugger->attachTo(egne);
    } else {
      consoleWidget->debugger->detach();
    }
  } else {
    qDebug() << "Scripting console attachDebugger(bool) unable to access "
                "Console object";
    return false;
  }
  return egne->undefinedValue();
}
