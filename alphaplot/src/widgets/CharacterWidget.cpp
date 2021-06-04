/* This file is part of AlphaPlot.
   Copyright 2016 - 2021, Arun Narayanankutty <n.arun.lifescience@gmail.com>

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

   Description : Symbol browser widget */

#include "CharacterWidget.h"

#include <QDebug>
#include <QFontDatabase>
#include <QMouseEvent>
#include <QPainter>
#include <QToolTip>

CharacterWidget::CharacterWidget(QWidget *parent)
    : QWidget(parent),
      greeksymbolchar_(true),
      numbersymbolchar_(true),
      subsupersymbolchar_(true),
      mathsymbolchar_(true),
      arrowsymbolchar_(true),
      othersymbolchar_(true) {
  updateChars(Chars::All);
  calculateSquareSize();
  setMouseTracking(true);
}

void CharacterWidget::updateFont(const QFont &font) {
  displayFont_.setFamily(font.family());
  calculateSquareSize();
  adjustSize();
  update();
}

void CharacterWidget::updateSize(const int fontSize) {
  displayFont_.setPointSize(fontSize);
  calculateSquareSize();
  adjustSize();
  update();
}

void CharacterWidget::updateStyle(const QString &fontStyle) {
  QFontDatabase fontDatabase;
  const QFont::StyleStrategy oldStrategy = displayFont_.styleStrategy();
  displayFont_ = fontDatabase.font(displayFont_.family(), fontStyle,
                                   displayFont_.pointSize());
  displayFont_.setStyleStrategy(oldStrategy);
  calculateSquareSize();
  adjustSize();
  update();
}

void CharacterWidget::updateFontMerging(bool enable) {
  if (enable)
    displayFont_.setStyleStrategy(QFont::PreferDefault);
  else
    displayFont_.setStyleStrategy(QFont::NoFontMerging);
  adjustSize();
  update();
}

void CharacterWidget::updateChars(CharacterWidget::Chars chars) {
  switch (chars) {
    case Chars::All:
      greeksymbolchar_ = true;
      numbersymbolchar_ = true;
      subsupersymbolchar_ = true;
      mathsymbolchar_ = true;
      arrowsymbolchar_ = true;
      othersymbolchar_ = true;
      break;
    case Chars::GreekSymbol:
      greeksymbolchar_ = true;
      numbersymbolchar_ = false;
      subsupersymbolchar_ = false;
      mathsymbolchar_ = false;
      arrowsymbolchar_ = false;
      othersymbolchar_ = false;
      break;
    case Chars::NumberSymbol:
      greeksymbolchar_ = false;
      numbersymbolchar_ = true;
      subsupersymbolchar_ = false;
      mathsymbolchar_ = false;
      arrowsymbolchar_ = false;
      othersymbolchar_ = false;
      break;
    case Chars::SubSuperSymbol:
      greeksymbolchar_ = false;
      numbersymbolchar_ = false;
      subsupersymbolchar_ = true;
      mathsymbolchar_ = false;
      arrowsymbolchar_ = false;
      othersymbolchar_ = false;
      break;
    case Chars::MathSymbol:
      greeksymbolchar_ = false;
      numbersymbolchar_ = false;
      subsupersymbolchar_ = false;
      mathsymbolchar_ = true;
      arrowsymbolchar_ = false;
      othersymbolchar_ = false;
      break;
    case Chars::ArrowSymbol:
      greeksymbolchar_ = false;
      numbersymbolchar_ = false;
      subsupersymbolchar_ = false;
      mathsymbolchar_ = false;
      arrowsymbolchar_ = true;
      othersymbolchar_ = false;
      break;
    case Chars::OtherSymbol:
      greeksymbolchar_ = false;
      numbersymbolchar_ = false;
      subsupersymbolchar_ = false;
      mathsymbolchar_ = false;
      arrowsymbolchar_ = false;
      othersymbolchar_ = true;
      break;
  }
  loadCharsList();
  calculateSquareSize();
  adjustSize();
  update();
}

void CharacterWidget::resized(int widgetwidth) {
  if (widgetwidth <= 0) return;
  int newColumns = widgetwidth / squareSize_;
  newColumns--;

  if (newColumns != columns_) {
    columns_ = newColumns;
    adjustSize();
    update();
  }
}

void CharacterWidget::calculateSquareSize() {
  squareSize_ = qMax(16, 4 + QFontMetrics(displayFont_, this).height());
}

void CharacterWidget::loadCharsList() {
  charsList_.clear();
  int i = 0;
  // Greek Symbol
  if (greeksymbolchar_) {
    // Upper greek chars
    for (i = 0; i <= (0x3A1 - 0x391); i++) charsList_.append(i + 0x391);
    for (i = 0; i <= (0x3A9 - 0x3A3); i++) charsList_.append(i + 0x3A3);
    // Lower greek chars
    for (i = 0; i <= (0x3C9 - 0x3B1); i++) charsList_.append(i + 0x3B1);
    // Greek punctuations and accents
    charsList_ << 0x374 << 0x375 << 0x37A << 0x37E;
    for (i = 0; i <= (0x389 - 0x384); i++) charsList_.append(i + 0x384);
    charsList_ << 0x38A << 0x38C << 0x38E << 0x38F << 0x3AC << 0x3AD << 0x3AE
               << 0x3CC << 0x3AF << 0x3CD << 0x3CE << 0x3AA << 0x3AB << 0x3CA
               << 0x3CB << 0x3B0 << 0x390;
    // Greek Cursives and Archaic Letters
    for (i = 0; i <= (0x3D7 - 0x3D0); i++) charsList_.append(i + 0x3D0);
    for (i = 0; i <= (0x3E1 - 0x3DA); i++) charsList_.append(i + 0x3DA);
    // other
    for (i = 0; i <= (0x3F1 - 0x3F0); i++) charsList_.append(i + 0x3F0);
  }
  // Number Symbols
  if (numbersymbolchar_) {
    charsList_ << 0xBC << 0xBD << 0xBE;
    for (i = 0; i <= (0x216B - 0x2153); i++) charsList_.append(i + 0x2153);
    for (i = 0; i <= (0x217B - 0x2170); i++) charsList_.append(i + 0x2170);
  }
  // Subscript and Super Script
  if (subsupersymbolchar_) {
    charsList_ << 0x2071 << 0x2070 << 0x00B9 << 0x00B2 << 0x00B3;
    for (i = 0; i <= (0x207F - 0x2074); i++) charsList_.append(i + 0x2074);
    for (i = 0; i <= (0x208E - 0x2080); i++) charsList_.append(i + 0x2080);
    for (i = 0; i <= (0x2093 - 0x2090); i++) charsList_.append(i + 0x2090);
    for (i = 0; i <= (0x1D6A - 0x1D62); i++) charsList_.append(i + 0x1D62);
  }
  // Math symbols
  if (mathsymbolchar_) {
    // Common Math Symbols
    charsList_ << 0x3C << 0x3E << 0x2260 << 0xF7 << 0x00D7 << 0x2212 << 0x2215
               << 0x2216 << 0x2044 << 0x192 << 0x2329 << 0x232A;
    for (i = 0; i <= (0x230B - 0x2308); i++) charsList_.append(i + 0x2308);
    // h bar
    for (i = 0; i <= (0x210F - 0x210F); i++) charsList_.append(i + 0x210F);
    // angstrom
    for (i = 0; i <= (0x212B - 0x212B); i++) charsList_.append(i + 0x212B);
    // Codes for Statistics Symbols
    for (i = 0; i <= (0x00B1 - 0x00B1); i++) charsList_.append(i + 0x00B1);
    for (i = 0; i <= (0x221C - 0x221a); i++) charsList_.append(i + 0x221a);
    // Statistics Symbols using Combining Diacritics
    charsList_ << 0x2211 << 0x220F << 0x2210;
    // Measurement Symbols
    charsList_ << 0xB0 << 0xB5;
    for (i = 0; i <= (0x2034 - 0x2030); i++) charsList_.append(i + 0x2030);
    // Calculus Symbols
    charsList_ << 0x2206 << 0x2207;
    for (i = 0; i <= (0x2233 - 0x222B); i++) charsList_.append(i + 0x222B);
    charsList_ << 0x2A11;
    // Common Letter Symbols
    charsList_ << 0x221E << 0x2135 << 0x2118 << 0x2111 << 0x211C << 0x211D
               << 0x2102 << 0x2115 << 0x2119 << 0x211A << 0x2124;
    // Logic and Set Theory Symbols
    for (i = 0; i <= (0x2205 - 0x2200); i++) charsList_.append(i + 0x2200);
    charsList_ << 0xAC << 0x2227 << 0x2228 << 0x22BB << 0x22BC << 0x22BD
               << 0x222A << 0x2229;
    for (i = 0; i <= (0x220D - 0x2208); i++) charsList_.append(i + 0x2208);
    for (i = 0; i <= (0x228B - 0x2282); i++) charsList_.append(i + 0x2282);
    for (i = 0; i <= (0x228E - 0x228C); i++) charsList_.append(i + 0x228C);
    for (i = 0; i <= (0x22D3 - 0x22D0); i++) charsList_.append(i + 0x22D0);
    for (i = 0; i <= (0x22C3 - 0x22C0); i++) charsList_.append(i + 0x22C0);
    charsList_ << 0x22CE << 0x22CF;
    // Variants of Element of Symbols
    for (i = 0; i <= (0x22FF - 0x22F2); i++) charsList_.append(i + 0x22F2);
    // Angle or Line symbols
    for (i = 0; i <= (0x2226 - 0x221F); i++) charsList_.append(i + 0x221F);
    charsList_ << 0x22BE << 0x22BF;
    // Symbols in proof
    charsList_ << 0x2234 << 0x2235 << 0x220E;
    // Equalance math symbols
    for (i = 0; i <= (0x2263 - 0x2261); i++) charsList_.append(i + 0x2261);
    for (i = 0; i <= (0x2279 - 0x2264); i++) charsList_.append(i + 0x2264);
    for (i = 0; i <= (0x224C - 0x2241); i++) charsList_.append(i + 0x2241);
    // Other math symbols
    charsList_ << 0x2213 << 0x2214 << 0x2217 << 0x2218 << 0x2219 << 0x221D;
    for (i = 0; i <= (0x2240 - 0x2236); i++) charsList_.append(i + 0x2236);
    for (i = 0; i <= (0x225F - 0x224D); i++) charsList_.append(i + 0x224D);
    for (i = 0; i <= (0x2281 - 0x227A); i++) charsList_.append(i + 0x227A);
    for (i = 0; i <= (0x22A5 - 0x228F); i++) charsList_.append(i + 0x228F);
    for (i = 0; i <= (0x22BA - 0x22A6); i++) charsList_.append(i + 0x22A6);
    for (i = 0; i <= (0x22CD - 0x22C4); i++) charsList_.append(i + 0x22C4);
    for (i = 0; i <= (0x22F1 - 0x22D4); i++) charsList_.append(i + 0x22D4);
  }
  if (arrowsymbolchar_) {
    for (i = 0; i <= (0x219B - 0x2190); i++) charsList_.append(i + 0x2190);
    for (i = 0; i <= (0x21A7 - 0x21A4); i++) charsList_.append(i + 0x21A4);
    for (i = 0; i <= (0x21D5 - 0x21CD); i++) charsList_.append(i + 0x21CD);
    for (i = 0; i <= (0x21E9 - 0x21E6); i++) charsList_.append(i + 0x21E6);
  }
  if (othersymbolchar_) {
    charsList_ << 0xa9 << 0xae << 0x2122 << 0x2713 << 0x2103 << 0x2109 << 0x24
               << 0xa2 << 0xa3 << 0x20AC << 0xA5 << 0x20B9 << 0x20BD << 0x5143
               << 0x20BF;
  }

  calculateSquareSize();
  adjustSize();
  update();
}

QSize CharacterWidget::sizeHint() const {
  if (charsList_.isEmpty()) return QSize();

  int addcol;
  (charsList_.size() % columns_) ? addcol = 1 : addcol = 0;
  return QSize(columns_ * squareSize_,
               ((charsList_.size() / columns_) + addcol) * squareSize_);
}

void CharacterWidget::mouseMoveEvent(QMouseEvent *event) {
  QPoint widgetPosition = mapFromGlobal(event->globalPos());
  uint key = (widgetPosition.y() / squareSize_) * columns_ +
             widgetPosition.x() / squareSize_;

  if (key < static_cast<uint>(charsList_.size())) {
    key = charsList_.at(key);
    QString text =
        QString::fromLatin1(
            "<p>Character: <span style=\"font-size: 24pt; font-family: %1\">")
            .arg(displayFont_.family()) +
        QChar(key) + QString::fromLatin1("</span><p>Value: 0x") +
        QString::number(key, 16);
    QToolTip::showText(event->globalPos(), text, this);
  }
}

void CharacterWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    lastKey_ = (event->y() / squareSize_) * columns_ + event->x() / squareSize_;
    if (lastKey_ < charsList_.size()) {
      lastKey_ = charsList_.at(lastKey_);
      if (QChar(lastKey_).category() != QChar::Other_NotAssigned)
        emit characterSelected(QString(QChar(lastKey_)));
      update();
    } else {
      qDebug() << lastKey_ << "Symbol/ Glyph out of range";
    }
  } else
    QWidget::mousePressEvent(event);
}

void CharacterWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.fillRect(event->rect(), QBrush(Qt::white));
  painter.setFont(displayFont_);

  QRect redrawRect = event->rect();
  int beginRow = redrawRect.top() / squareSize_;
  int endRow = redrawRect.bottom() / squareSize_;
  int beginColumn = redrawRect.left() / squareSize_;
  int endColumn = redrawRect.right() / squareSize_;
  int charsize = charsList_.size();

  painter.setPen(QPen(Qt::gray));
  for (int row = beginRow; row <= endRow; ++row) {
    for (int column = beginColumn; column <= endColumn; ++column) {
      if (charsize <= (row * columns_ + column)) {
        break;
      }
      painter.drawRect(column * squareSize_, row * squareSize_, squareSize_,
                       squareSize_);
    }
  }

  QFontMetrics fontMetrics(displayFont_);
  painter.setPen(QPen(Qt::black));
  for (int row = beginRow; row <= endRow; ++row) {
    for (int column = beginColumn; column <= endColumn; ++column) {
      if (charsize <= (row * columns_ + column)) {
        break;
      }
      int key = charsList_.at(row * columns_ + column);
      painter.setClipRect(column * squareSize_, row * squareSize_, squareSize_,
                          squareSize_);

      if (key == lastKey_)
        painter.fillRect(column * squareSize_ + 1, row * squareSize_ + 1,
                         squareSize_, squareSize_, QBrush(Qt::green));

      painter.drawText(column * squareSize_ + (squareSize_ / 2) -
                           fontMetrics.horizontalAdvance(QChar(key)) / 2,
                       row * squareSize_ + 4 + fontMetrics.ascent(),
                       QString(QChar(key)));
    }
  }
}
