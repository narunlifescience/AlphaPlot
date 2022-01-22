/***************************************************************************
    File                 : TextFormatButtons.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Widget with text format buttons (connected to a
 QTextEdit)

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

#include "TextFormatButtons.h"

#include <QHBoxLayout>
#include <QPushButton>
#include <QString>
#include <QTextEdit>

TextFormatButtons::TextFormatButtons(QTextEdit *textEdit, QWidget *parent)
    : QWidget(parent) {
  connectedTextEdit = textEdit;

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0);
  layout->setSpacing(0);
  layout->addStretch();

  QFont font = this->font();
  font.setPointSize(14);

  buttonCurve = new QPushButton(QPixmap(":/lineSymbol.xpm"), QString());
  buttonCurve->setMaximumWidth(40);
  buttonCurve->setMinimumHeight(35);
  buttonCurve->setFont(font);
  layout->addWidget(buttonCurve);

  buttonSubscript = new QPushButton(QPixmap(":/index.xpm"), QString());
  buttonSubscript->setMaximumWidth(40);
  buttonSubscript->setMinimumHeight(35);
  buttonSubscript->setFont(font);
  layout->addWidget(buttonSubscript);

  buttonSuperscript = new QPushButton(QPixmap(":/exp.xpm"), QString());
  buttonSuperscript->setMaximumWidth(40);
  buttonSuperscript->setMinimumHeight(35);
  buttonSuperscript->setFont(font);
  layout->addWidget(buttonSuperscript);

  buttonLowerGreek = new QPushButton(QString(QChar(0x3B1)));
  buttonLowerGreek->setFont(font);
  buttonLowerGreek->setMaximumWidth(40);
  layout->addWidget(buttonLowerGreek);

  buttonUpperGreek = new QPushButton(QString(QChar(0x393)));
  buttonUpperGreek->setFont(font);
  buttonUpperGreek->setMaximumWidth(40);
  layout->addWidget(buttonUpperGreek);

  buttonMathSymbols = new QPushButton(QString(QChar(0x222B)));
  buttonMathSymbols->setFont(font);
  buttonMathSymbols->setMaximumWidth(40);
  layout->addWidget(buttonMathSymbols);

  buttonArrowSymbols = new QPushButton(QString(QChar(0x2192)));
  buttonArrowSymbols->setFont(font);
  buttonArrowSymbols->setMaximumWidth(40);
  layout->addWidget(buttonArrowSymbols);

  font = this->font();
  font.setBold(true);
  font.setPointSize(14);

  buttonBold = new QPushButton(tr("B", "Button bold"));
  buttonBold->setFont(font);
  buttonBold->setMaximumWidth(40);
  layout->addWidget(buttonBold);

  font = this->font();
  font.setItalic(true);
  font.setPointSize(14);

  buttonItalics = new QPushButton(tr("It", "Button italics"));
  buttonItalics->setFont(font);
  buttonItalics->setMaximumWidth(40);
  layout->addWidget(buttonItalics);

  font = this->font();
  font.setUnderline(true);
  font.setPointSize(14);

  buttonUnderline = new QPushButton(tr("U", "Button underline"));
  buttonUnderline->setFont(font);
  buttonUnderline->setMaximumWidth(40);
  layout->addWidget(buttonUnderline);
  layout->addStretch();

  connect(buttonCurve, &QPushButton::clicked, this,
          &TextFormatButtons::addCurve);
  connect(buttonSuperscript, &QPushButton::clicked, this,
          &TextFormatButtons::addSuperscript);
  connect(buttonSubscript, &QPushButton::clicked, this,
          &TextFormatButtons::addSubscript);
  connect(buttonUnderline, &QPushButton::clicked, this,
          &TextFormatButtons::addUnderline);
  connect(buttonItalics, &QPushButton::clicked, this,
          &TextFormatButtons::addItalics);
  connect(buttonBold, &QPushButton::clicked, this, &TextFormatButtons::addBold);
  connect(buttonLowerGreek, &QPushButton::clicked, this,
          &TextFormatButtons::showLowerGreek);
  connect(buttonUpperGreek, &QPushButton::clicked, this,
          &TextFormatButtons::showUpperGreek);
  connect(buttonMathSymbols, &QPushButton::clicked, this,
          &TextFormatButtons::showMathSymbols);
  connect(buttonArrowSymbols, &QPushButton::clicked, this,
          &TextFormatButtons::showArrowSymbols);
}

void TextFormatButtons::showLowerGreek() {}

void TextFormatButtons::showUpperGreek() {}

void TextFormatButtons::showMathSymbols() {}

void TextFormatButtons::showArrowSymbols() {}

void TextFormatButtons::addSymbol(const QString &letter) {
  connectedTextEdit->textCursor().insertText(letter);
}

void TextFormatButtons::addCurve() { formatText("\\c{", "}"); }

void TextFormatButtons::addUnderline() { formatText("<u>", "</u>"); }

void TextFormatButtons::addItalics() { formatText("<i>", "</i>"); }

void TextFormatButtons::addBold() { formatText("<b>", "</b>"); }

void TextFormatButtons::addSubscript() { formatText("<sub>", "</sub>"); }

void TextFormatButtons::addSuperscript() { formatText("<sup>", "</sup>"); }

void TextFormatButtons::formatText(const QString &prefix,
                                   const QString &postfix) {
  QTextCursor cursor = connectedTextEdit->textCursor();
  QString markedText = cursor.selectedText();
  cursor.insertText(prefix + markedText + postfix);
  if (markedText.isEmpty()) {
    // if no text is marked, place cursor inside the <..></..> statement
    // instead of after it
    cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::MoveAnchor,
                        postfix.size());
    // the next line makes the selection visible to the user
    // (the line above only changes the selection in the
    // underlying QTextDocument)
    connectedTextEdit->setTextCursor(cursor);
  }
  // give focus back to text edit
  connectedTextEdit->setFocus();
}

void TextFormatButtons::toggleCurveButton(bool enable) {
  buttonCurve->setVisible(enable);
}
