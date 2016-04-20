/* This file is part of AlphaPlot.
   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : AlphaPlot Console dock widget
*/
#include <QDebug>
#include "ConsoleWidget.h"
#include "ui_ConsoleWidget.h"

#include "scripting/widgets/Console.h"

// ScriptingConsole print() function reimplimentation
QScriptValue print(QScriptContext *context, QScriptEngine *) {
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
  }
  return QObject::tr("Print done!");
}

// ScriptingConsole clear() function
QScriptValue clear(QScriptContext *context, QScriptEngine *)
{
  QScriptValue calleeData = context->callee().data();
  Console *console = qobject_cast<Console *>(calleeData.toQObject());
  if (console) {
    console->clearConsole();
  } else {
    qDebug() << "Scripting console clear() unable to access Console object";
  }
  return QObject::tr("");
}

ConsoleWidget::ConsoleWidget(QWidget *parent)
    : QDockWidget(parent),
      engine(new QScriptEngine(this)),
      ui_(new Ui_ConsoleWidget) {
  ui_->setupUi(this);
  setWindowTitle(tr("Scripting Console"));
  setWindowIcon(QIcon());
  ui_->gridLayout->setSpacing(0);
  ui_->gridLayout->setContentsMargins(0, 0, 0, 0);
  connect(ui_->console, SIGNAL(command(QString)), this,
          SLOT(evaluate(QString)));
  // print() function
  QScriptValue printFunction = engine->newFunction(&print);
  printFunction.setData(engine->newQObject(ui_->console));
  engine->globalObject().setProperty("print", printFunction);
  // clear() function
  QScriptValue clearFunction = engine->newFunction(&clear);
  clearFunction.setData(engine->newQObject(ui_->console));
  engine->globalObject().setProperty("clear", clearFunction);
}

ConsoleWidget::~ConsoleWidget() {
  delete ui_;
  delete engine;
}

void ConsoleWidget::evaluate(QString line) {
      ui_->console->result(engine->evaluate(line).toString());
}
