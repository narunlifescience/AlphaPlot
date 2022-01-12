/***************************************************************************
    File                 : PolynomFitDialog.h
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Fit polynomial dialog

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
#ifndef POLINOMFITDIALOG_H
#define POLINOMFITDIALOG_H

#include <QDialog>

class QCheckBox;
class QSpinBox;
class QPushButton;
class QLineEdit;
class QComboBox;
class ColorBox;
class AxisRect2D;
class ApplicationWindow;

//! Fit polynomial dialog
class PolynomFitDialog : public QDialog {
  Q_OBJECT

 public:
  PolynomFitDialog(QWidget* parent = nullptr, Qt::WindowFlags fl = Qt::Widget);

 public slots:
  void fit();
  void setAxisRect(AxisRect2D* axisrect);
  void activateCurve(const QString& curveName);

 private:
  AxisRect2D* axisrect_;
  ApplicationWindow *app_;
  double xmin_;
  double xmax_;

  QPushButton* buttonFit;
  QPushButton* buttonCancel;
  QCheckBox* boxShowFormula;
  QComboBox* boxName;
  QSpinBox* boxOrder;
  QLineEdit* boxStart;
  QLineEdit* boxEnd;
  ColorBox* boxColor;
};

#endif  // POLINOMFITDIALOG_H
