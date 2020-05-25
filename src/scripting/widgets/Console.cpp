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

#include "Console.h"
#include "../SyntaxHighlighter.h"

#include <iostream>
#include <QDebug>
#include <QMimeData>

Console::Console(QWidget *parent)
    : QTextEdit(parent),
      userPrompt(QString(" >> ")),
      partialUserPrompt(QString("... ")),
      activeUserPromptPrefix(QString("<font color=\"orange\">&alpha;</font>")),
      partialUserPromptPrefix(QString(" ")),
      partialPromptVisible(false),
      locked(false),
      historySkip(false) {
  historyUp.clear();
  historyDown.clear();
  insertHtml(activeUserPromptPrefix + userPrompt);
  new SyntaxHighlighter(document());
  document()->setMaximumBlockCount(3000);
  QFont consoleFont = QFont("Monospace", 10, QFont::Normal, false);
  setConsoleFont(consoleFont);
}

Console::~Console() {}

/** Filter all key events. The keys are filtered and handled manually
  * in order to create a typical shell-like behaviour. For example
  * Up and Down arrows don't move the cursor, but allow the user to
  * browse the last commands that there launched.
  */

void Console::keyPressEvent(QKeyEvent *e) {
  // locked State: a command has been submitted but no result
  // has been received yet.
  if (locked) return;

  switch (e->key()) {
    case Qt::Key_Return:
      handleEnter();
      break;
    case Qt::Key_Backspace:
      handleLeft(e);
      break;
    case Qt::Key_Up:
      handleHistoryUp();
      break;
    case Qt::Key_Down:
      handleHistoryDown();
      break;
    case Qt::Key_Left:
      handleLeft(e);
      break;
    case Qt::Key_Home:
      handleHome();
      break;
    default:
      QTextEdit::keyPressEvent(e);
      break;
  }
}

// Enter key pressed
void Console::handleEnter() {
  QString cmd = getCommand();

  if (0 < cmd.length()) {
    while (historyDown.count() > 0) {
      historyUp.push(historyDown.pop());
    }

    historyUp.push(cmd);
  }

  moveToEndOfLine();

  if (cmd.length() > 0) {
    locked = true;
    setFocus();
    insertPlainText("\n");
    emit command(cmd);
  } else {
    insertPlainText("\n");
    if (!partialPromptVisible)
      insertHtml(activeUserPromptPrefix + userPrompt);
    else
      insertPlainText(partialUserPromptPrefix + partialUserPrompt);
    ensureCursorVisible();
  }
}

// Result received
void Console::result(QString result, ResultType type) {
  partialPromptVisible = false;
  if (result.trimmed().length() != 0) {
    if (type == Console::Success)
      insertPlainText("  " + result);
    else
      insertHtml(QString("<font color=\"red\">") + QString("&nbsp;&nbsp;") +
                 QString(result) + QString("</ font>"));
    insertPlainText("\n");
    insertHtml(activeUserPromptPrefix + userPrompt);
    ensureCursorVisible();
    locked = false;
  }
}

void Console::partialResult() {
  partialPromptVisible = true;
  insertPlainText(partialUserPromptPrefix + partialUserPrompt);
  ensureCursorVisible();
  locked = false;
}

void Console::promptWithoutResult() {
  partialPromptVisible = false;
  insertHtml(activeUserPromptPrefix + userPrompt);
  ensureCursorVisible();
  locked = false;
}

// Append line but do not display prompt afterwards
void Console::append(QString text) {
  if (text.trimmed().length() != 0) {
    insertPlainText("  " + text);
    insertPlainText("\n");
    ensureCursorVisible();
  }
}

void Console::clearConsole() {
  setPlainText("");
  ensureCursorVisible();
  locked = false;
}

void Console::setConsoleFont(QFont font) { setFont(font); }

// Arrow up pressed
void Console::handleHistoryUp() {
  if (0 < historyUp.count()) {
    QString cmd = historyUp.pop();
    historyDown.push(cmd);
    clearLine();
    insertPlainText(cmd);
  }

  historySkip = true;
}

// Arrow down pressed
void Console::handleHistoryDown() {
  if (0 < historyDown.count() && historySkip) {
    historyUp.push(historyDown.pop());
    historySkip = false;
  }

  if (0 < historyDown.count()) {
    QString cmd = historyDown.pop();
    historyUp.push(cmd);

    clearLine();
    insertPlainText(cmd);
  } else {
    clearLine();
  }
}

void Console::clearLine() {
  QTextCursor c = this->textCursor();
  c.select(QTextCursor::LineUnderCursor);
  c.removeSelectedText();
  if (!partialPromptVisible)
    this->insertHtml(activeUserPromptPrefix + userPrompt);
  else
    this->insertPlainText(partialUserPromptPrefix + partialUserPrompt);
}

// Select and return the user-input (exclude the prompt)
QString Console::getCommand() const {
  QTextCursor c = this->textCursor();
  c.select(QTextCursor::LineUnderCursor);

  QString text = c.selectedText();
  text.remove(0, userPrompt.length() + 1);

  return text;
}

void Console::moveToEndOfLine() {
  QTextEdit::moveCursor(QTextCursor::EndOfLine);
}

// The text cursor is not allowed to move beyond the
// prompt
void Console::handleLeft(QKeyEvent *event) {
  if (getIndex(textCursor()) > userPrompt.length() + 1) {
    QTextEdit::keyPressEvent(event);
  }
}

// Home (pos1) key pressed
void Console::handleHome() {
  QTextCursor c = textCursor();
  c.movePosition(QTextCursor::StartOfLine);
  c.movePosition(QTextCursor::Right, QTextCursor::MoveAnchor,
                 userPrompt.length() + 1);
  setTextCursor(c);
}

// Solution for getting x and y position of the cursor. Found
// them in the Qt mailing list
int Console::getIndex(const QTextCursor &crQTextCursor) {
  QTextBlock b;
  int column = 1;
  b = crQTextCursor.block();
  column = crQTextCursor.position() - b.position();
  return column;
}

void Console::setPrompt(const QString &prompt) {
  userPrompt = prompt;
  clearLine();
}

void Console::insertFromMimeData(const QMimeData *source) {
  QString text = source->text();
  QStringList line = text.split("\n");

  if (line.isEmpty()) return;
  if (line.length() == 1) {
    insertPlainText(line[0]);
    return;
  }

  append(line[0]);
  for (int i = 1; i < line.length(); ++i) {
    partialResult();
    append(line[i]);
  }
  locked = true;
  emit(command(text));
}

QString Console::prompt() const { return userPrompt; }
