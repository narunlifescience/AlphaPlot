/***************************************************************************
    File                 : IntDialog.h
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Vasileios Gkanis, Tilman
 Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Integration options dialog

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
#ifndef INTDIALOG_H
#define INTDIALOG_H

#include <QDialog>

class QPushButton;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QSpinBox;
class ApplicationWindow;
class AxisRect2D;

//! Integration options dialog
class IntDialog : public QDialog {
  Q_OBJECT

 public:
  IntDialog(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::Widget);
  ~IntDialog() {}

  QPushButton* buttonOk;
  QPushButton* buttonCancel;
  QPushButton* buttonHelp;
  QCheckBox* boxShowFormula;
  QComboBox* boxName;
  QComboBox* boxMethod;
  QLineEdit* boxStart;
  QLineEdit* boxEnd;

 public slots:
  void accept();
  void setAxisrect(AxisRect2D* axisrect);
  void activateCurve(const QString& curveName);
  void help();
  void changeDataRange();

 signals:
  void integrate(int, int, int, double, double, double);

 private:
  AxisRect2D* axisrect_;
  ApplicationWindow *app_;
  double xmin_;
  double xmax_;
};

#endif  // INTDIALOG_H
