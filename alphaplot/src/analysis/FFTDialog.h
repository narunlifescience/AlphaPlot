/***************************************************************************
    File                 : FFTDialog.h
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Fast Fourier transform options dialog

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
#ifndef FFTDIALOG_H
#define FFTDIALOG_H

#include <QDialog>
#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Graph2DCommon.h"

class QPushButton;
class QRadioButton;
class QLineEdit;
class QComboBox;
class QCheckBox;
class Table;

//! Fast Fourier transform options dialog
class FFTDialog : public QDialog {
  Q_OBJECT

 public:
  enum DataType { onGraph = 0, onTable = 1 };

  FFTDialog(int type, QWidget* parent = nullptr,
            Qt::WindowFlags flag = Qt::Widget);
  ~FFTDialog() {}

  QPushButton* buttonOK;
  QPushButton* buttonCancel;
  QRadioButton *forwardBtn, *backwardBtn;
  QComboBox *boxName, *boxReal, *boxImaginary;
  QLineEdit* boxSampling;
  QCheckBox *boxNormalize, *boxOrder;

 public slots:
  void setAxisrect(AxisRect2D* axisrect);
  void setTable(Table* table);
  void activateCurve(const QString& curveName);
  void accept();

 private:
  AxisRect2D* axisrect_;
  Table* d_table;
  int d_type;
};

#endif  // FFTDIALOG_H
