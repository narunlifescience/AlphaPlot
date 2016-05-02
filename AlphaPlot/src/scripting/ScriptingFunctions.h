#ifndef SCRIPTINGFUNCTIONS_H
#define SCRIPTINGFUNCTIONS_H

#include <QScriptEngine>
#include <../Table.h>
#include <../Note.h>
#include <../Matrix.h>
#include <../future/core/column/Column.h>

class QScriptEngine;
class Table;

// Register types
Q_DECLARE_METATYPE(Table *)
Q_DECLARE_METATYPE(Note *)
Q_DECLARE_METATYPE(Matrix *)
Q_DECLARE_METATYPE(Column *)
Q_DECLARE_METATYPE(QVector<int>)
Q_DECLARE_METATYPE(QVector<float>)
Q_DECLARE_METATYPE(QVector<double>)
Q_DECLARE_METATYPE(QVector<long>)
Q_DECLARE_METATYPE(QVector<QString>)
Q_DECLARE_METATYPE(QVector<QDate>)
Q_DECLARE_METATYPE(QVector<QDateTime>)

// Registered type to from conversion
template <class AlphaWindowObject>
QScriptValue tableObjectToScriptValue(QScriptEngine *engine,
                                      const AlphaWindowObject &in) {
  return engine->newQObject(in);
}

template <class AlphaWindowObject>
void tableObjectFromScriptValue(const QScriptValue &object,
                                AlphaWindowObject &out) {
  out = qobject_cast<AlphaWindowObject>(object.toQObject());
}

// Register QVector<>
template <class Container>
QScriptValue toScriptValue(QScriptEngine *eng, const Container &cont)
{
    QScriptValue a = eng->newArray();
    typename Container::const_iterator begin = cont.begin();
    typename Container::const_iterator end = cont.end();
    typename Container::const_iterator it;
    for (it = begin; it != end; ++it)
        a.setProperty(quint32(it - begin), qScriptValueFromValue(eng, *it));
    return a;
}

template <class Container>
void fromScriptValue(const QScriptValue &value, Container &cont)
{
    quint32 len = value.property("length").toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue item = value.property(i);
        typedef typename Container::value_type ContainerValue;
        cont.push_back(qscriptvalue_cast<ContainerValue>(item));
    }
}

// Console Basic Functions
QScriptValue print(QScriptContext *context, QScriptEngine *egne);
QScriptValue clear(QScriptContext *context, QScriptEngine *egne);
QScriptValue collectGarbage(QScriptContext *context, QScriptEngine *egne);
QScriptValue attachDebugger(QScriptContext *context, QScriptEngine *egne);
// Core functions
QScriptValue openProj(QScriptContext *context, QScriptEngine *egne);

#endif  // SCRIPTINGFUNCTIONS_H
