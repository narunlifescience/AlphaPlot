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

#ifndef CHARACTERWIDGET_H
#define CHARACTERWIDGET_H

#include <QFont>
#include <QWidget>

class CharacterWidget : public QWidget {
  Q_OBJECT

 public:
  CharacterWidget(QWidget *parent = nullptr);
  QSize sizeHint() const override;
  void resized(int widgetwidth);
  enum class Chars : int {
    All = 0,
    GreekSymbol= 1,
    NumberSymbol = 2,
    SubSuperSymbol = 3,
    MathSymbol = 4,
    ArrowSymbol = 5,
    OtherSymbol = 6
  };

 public slots:
  void updateFont(const QFont &font);
  void updateSize(const int fontSize);
  void updateStyle(const QString &fontStyle);
  void updateFontMerging(bool enable);
  void updateChars(Chars chars);

 signals:
  void characterSelected(const QString &character);

 protected:
  void mouseMoveEvent(QMouseEvent *event) override;
  void mousePressEvent(QMouseEvent *event) override;
  void paintEvent(QPaintEvent *event) override;

 private:
  void calculateSquareSize();
  void loadCharsList();

  QFont displayFont_;
  int columns_ = 16;
  int lastKey_ = -1;
  int squareSize_ = 0;
  QList<int> charsList_;
  bool greeksymbolchar_;
  bool numbersymbolchar_;
  bool subsupersymbolchar_;
  bool mathsymbolchar_;
  bool arrowsymbolchar_;
  bool othersymbolchar_;
};

#endif  // CHARACTERWIDGET_H
