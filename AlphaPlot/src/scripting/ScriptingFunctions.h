#ifndef SCRIPTINGFUNCTIONS_H
#define SCRIPTINGFUNCTIONS_H

#include <QScriptEngine>

class QScriptEngine;

QScriptValue print(QScriptContext *context, QScriptEngine *egne);
QScriptValue clear(QScriptContext *context, QScriptEngine *egne);
QScriptValue collectGarbage(QScriptContext *context, QScriptEngine *egne);
QScriptValue attachDebugger(QScriptContext *context, QScriptEngine *egne);

QScriptValue openProj(QScriptContext *context, QScriptEngine *egne);

#endif  // SCRIPTINGFUNCTIONS_H
