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

   Description : Symbol browser dialog */

#ifndef CHARACTERMAPWIDGET_H
#define CHARACTERMAPWIDGET_H

#include <QDialog>

class QFontComboBox;
class QComboBox;
class QLineEdit;
class QScrollArea;
class CharacterWidget;

class CharacterMapWidget : public QDialog {
  Q_OBJECT

 public:
  CharacterMapWidget(QDialog *parent = nullptr);

 public slots:
  void insertCharacter(const QString &character);
  void updateClipboard();

 protected:
  void resizeEvent(QResizeEvent *event) override;

 private:
  static const int glyphSize_;
  CharacterWidget *characterWidget_;
  QFontComboBox *fontCombo_;
  QComboBox *charlist_;
  QLineEdit *lineEdit_;
  QScrollArea *scrollArea_;
};

#endif  // CHARACTERMAPWIDGET_H
