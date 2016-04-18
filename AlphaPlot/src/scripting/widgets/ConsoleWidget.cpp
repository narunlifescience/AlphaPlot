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
#include "ui_ConsoleWidget.h"

#include "scripting/widgets/Console.h"

ConsoleWidget::ConsoleWidget(QWidget *parent)
    : QDockWidget(parent),
      ui_(new Ui_ConsoleWidget),
      engine(new QScriptEngine(this)) {
  ui_->setupUi(this);
  setWindowTitle(tr("Scripting Console"));
  setWindowIcon(QIcon());
  ui_->gridLayout->setSpacing(0);
  ui_->gridLayout->setContentsMargins(0, 0, 0, 0);
  connect(ui_->console, SIGNAL(command(QString)), this,
          SLOT(evaluate(QString)));
}

ConsoleWidget::~ConsoleWidget() {
  delete ui_;
  delete engine;
}

void ConsoleWidget::evaluate(QString line) {
  if (line.contains("clear")) {
    QString clear = line.simplified();
    clear.replace(" ", "");
    if (clear == "clear" || clear == "clear;" || clear == "clear()" ||
        clear == "clear();") {
      ui_->console->clearConsole();
    } else {
      ui_->console->result(engine->evaluate(line).toString());
    }
  } else {
    ui_->console->result(engine->evaluate(line).toString());
  }
}
