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

   Description : AlphaPlot scripting console widget
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QPlainTextEdit>
#include <QKeyEvent>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextDocumentFragment>
#include <QStack>

class Console : public QTextEdit {
  Q_OBJECT

 public:
  Console(QWidget *parent = nullptr);
  ~Console();
  QString prompt() const;
  void setPrompt(const QString &prompt);

  enum ResultType { Success, Error };

 protected:
  void insertFromMimeData(const QMimeData * source);
  void keyPressEvent(QKeyEvent *e);

  // Do not handle other events
  void mousePressEvent(QMouseEvent *) {}
  void mouseDoubleClickEvent(QMouseEvent *) {}
  void mouseMoveEvent(QMouseEvent *) {}
  void mouseReleaseEvent(QMouseEvent *) {}

 private:
  void handleLeft(QKeyEvent *event);
  void handleEnter();
  void handleHistoryUp();
  void handleHistoryDown();
  void handleHome();

  void moveToEndOfLine();
  void clearLine();
  QString getCommand() const;

  int getIndex(const QTextCursor &crQTextCursor);

  QString userPrompt;
  QString partialUserPrompt;
  QString activeUserPromptPrefix;
  QString partialUserPromptPrefix;
  QStack<QString> historyUp;
  QStack<QString> historyDown;
  bool partialPromptVisible;
  bool locked;
  bool historySkip;

  // The command signal is fired when a user input is entered
 signals:
  void command(QString command);

  // The result slot displays the result of a command in the terminal
 public slots:
  void result(QString result, ResultType type);
  void partialResult();
  void promptWithoutResult();
  void append(QString text);
  void clearConsole();
  void setConsoleFont(QFont font);
};

#endif  // CONSOLE_H
