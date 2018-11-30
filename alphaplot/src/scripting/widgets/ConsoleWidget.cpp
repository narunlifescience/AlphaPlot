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
#include "ConsoleWidget.h"
#include <QDebug>
#include <QPainter>
#include <QStandardItem>
#include <QStandardItemModel>
#include "ui_ConsoleWidget.h"

#include "../ScriptingFunctions.h"
#include "scripting/widgets/Console.h"

ConsoleWidget::ConsoleWidget(QWidget *parent)
    : QDockWidget(parent),
      engine(new QScriptEngine(this)),
      debugger(new QScriptEngineDebugger(this)),
      ui_(new Ui_ConsoleWidget),
      scriptGlobalObjectsModel(new QStandardItemModel(this)) {
  ui_->setupUi(this);
  setWindowTitle(tr("Scripting Console"));
  setWindowIcon(QIcon());
  ui_->gridLayout->setSpacing(0);
  ui_->gridLayout->setContentsMargins(0, 0, 0, 0);
  ui_->console->setFrameShape(QFrame::NoFrame);
  ui_->tableView->setFrameShape(QFrame::NoFrame);
  ui_->splitter->setSizes(QList<int>() << 70 << 30);
  // ui_->tableView->setShowGrid(false);
  ui_->tableView->verticalHeader()->setVisible(false);
  ui_->tableView->horizontalHeader()->setSelectionMode(
      QAbstractItemView::SingleSelection);
  ui_->tableView->horizontalHeader()->setSectionResizeMode(
      (0, QHeaderView::Stretch));
  scriptGlobalObjectsModel->setColumnCount(2);
  ui_->tableView->setModel(scriptGlobalObjectsModel);
  ui_->tableView->setItemDelegate(new Delegate(this));
  ui_->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui_->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
  ui_->tableView->setAlternatingRowColors(true);
  addScriptGlobalsToTableView();

  connect(ui_->console, SIGNAL(command(QString)), this,
          SLOT(evaluate(QString)));

  engine->setProcessEventsInterval(50);  // 1 sec process interval
  // Basic console functions
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
  if (engine) delete engine;
  if (debugger) delete debugger;
}

void ConsoleWidget::printError(QString err) {
  ui_->console->result("muParser error: " + err, Console::Error);
}

void ConsoleWidget::setSplitterPosition(QByteArray state) {
  ui_->splitter->restoreState(state);
}

QByteArray ConsoleWidget::getSplitterPosition() {
  return ui_->splitter->saveState();
}

void ConsoleWidget::addScriptGlobalsToTableView() {
  QPair<QString, QString> rowPair;
  scriptGlobalObjectsModel->clear();
  scriptGlobalObjectsModel->setHorizontalHeaderLabels(QStringList()
                                                      << "variables"
                                                      << "values");
  QScriptValueIterator it(engine->globalObject());
  while (it.hasNext()) {
    it.next();
    if (it.value().isArray()) {
      // Array variables
      rowPair.first = it.name() + QString("[%0]").arg(
                                      it.value().property("length").toString());
      QString arrayValue;
      double arrayLength = it.value().property("length").toInteger();
      for (quint32 i = 0; i < 3; i++) {
        if (i < arrayLength)
          arrayValue += it.value().property(i).toString() + " ,";
      }
      if (arrayLength > 3) {
        arrayValue += "...";
      }
      rowPair.second = arrayValue;
      appendRowToTableView(rowPair);

    } else if (!it.value().isFunction() && !it.value().isObject()) {
      if (it.name() != "NaN" && it.name() != "Infinity" &&
          it.name() != "undefined") {
        // Other variables
        rowPair.first = it.name();
        rowPair.second = it.value().toString();
        appendRowToTableView(rowPair);
      }
    } else if (it.value().isObject() && !it.value().isFunction()) {
      rowPair.first = it.name();
      rowPair.second = it.value().toString();
      appendRowToTableView(rowPair);
    }
  }
}

void ConsoleWidget::appendRowToTableView(QPair<QString, QString> rowPair) {
  scriptGlobalObjectsModel->appendRow(QList<QStandardItem *>()
                                      << new QStandardItem(rowPair.first)
                                      << new QStandardItem(rowPair.second));
}

void ConsoleWidget::evaluate(QString line) {
  snippet.append(line);
  snippet += QLatin1Char('\n');
  if (engine->canEvaluate(snippet)) {
    QString syntaxError;
    // Check syntax errors
    QScriptSyntaxCheckResult error = engine->checkSyntax(snippet);
    (error.state() != QScriptSyntaxCheckResult::Valid)
        ? syntaxError += error.errorMessage() + " "
        : syntaxError = "";
    QScriptValue result = engine->evaluate(snippet, "line", 1);
    snippet.clear();
    if (!result.isUndefined()) {
      if (!result.isError()) {
        ui_->console->result(result.toString(), Console::Success);
      } else {
        if (engine->hasUncaughtException()) {
          QStringList backtrace = engine->uncaughtExceptionBacktrace();
          ui_->console->result(
              syntaxError + result.toString() + " | " + backtrace.join("\n"),
              Console::Error);
        } else {
          ui_->console->result(syntaxError + result.toString(), Console::Error);
        }
      }

    } else {
      ui_->console->promptWithoutResult();
    }
    addScriptGlobalsToTableView();
  } else {
    ui_->console->partialResult();
  }
}

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                     const QModelIndex &index) const {
  QString val;
  QStyleOptionViewItem opt(option);
  /*if (qVariantCanConvert<QString>(index.data()))
    val = qVariantValue<QString>(index.data());
  if (val == "variable") {
    QColor fillColor = option.palette.highlight();
    fillColor.setAlpha(50);
    painter->fillRect(option.rect,fillColor);
    }*/
  QItemDelegate::paint(painter, opt, index);
}
