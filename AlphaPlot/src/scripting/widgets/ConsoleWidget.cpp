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
#include "../ScriptingFunctions.h"

ConsoleWidget::ConsoleWidget(QWidget *parent)
    : QDockWidget(parent),
      engine(new QScriptEngine(this)),
      debugger(new QScriptEngineDebugger(this)),
      ui_(new Ui_ConsoleWidget) {
  ui_->setupUi(this);
  setWindowTitle(tr("Scripting Console"));
  setWindowIcon(QIcon());
  ui_->gridLayout->setSpacing(0);
  ui_->gridLayout->setContentsMargins(0, 0, 0, 0);
  connect(ui_->console, SIGNAL(command(QString)), this,
          SLOT(evaluate(QString)));
  engine->setProcessEventsInterval(1000); // 1 sec process interval
  // print() function
  QScriptValue consoleObjectValue = engine->newQObject(ui_->console);
  QScriptValue consoleWidgetObjectValue = engine->newQObject(this);
  QScriptValue printFunction = engine->newFunction(&print);
  printFunction.setData(consoleObjectValue);
  engine->globalObject().setProperty("print", printFunction);
  // clear() function
  QScriptValue clearFunction = engine->newFunction(&clear);
  clearFunction.setData(consoleObjectValue);
  engine->globalObject().setProperty("clear", clearFunction);
  // collectGarbage() function
  QScriptValue garbageFunction = engine->newFunction(&collectGarbage);
  engine->globalObject().setProperty("collectGarbage", garbageFunction);
  // attachDebugger(bool) function
  QScriptValue debuggerFunction = engine->newFunction(&attachDebugger);
  debuggerFunction.setData(consoleWidgetObjectValue);
  engine->globalObject().setProperty("attachDebugger", debuggerFunction);
}

ConsoleWidget::~ConsoleWidget() {
  delete ui_;
  delete engine;
}

void ConsoleWidget::evaluate(QString line) {
  snippet.append(line);
  snippet += QLatin1Char('\n');
  if (engine->canEvaluate(snippet)) {
    QScriptValue result = engine->evaluate(snippet);
    if (!result.isUndefined()) {
      if (!result.isError())
        ui_->console->result(result.toString(), Console::Success);
      else
        ui_->console->result(result.toString(), Console::Error);
    } else {
      ui_->console->promptWithoutResult();
    }
    snippet.clear();
  } else {
    ui_->console->partialResult();
  }
}
