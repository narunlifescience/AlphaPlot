/***************************************************************************
    File                 : IntDialog.cpp
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
#include "IntDialog.h"
#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Plotcolumns.h"
#include "ApplicationWindow.h"
#include "Differentiation.h"
#include "Integration.h"
#include "scripting/MyParser.h"

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

IntDialog::IntDialog(QWidget *parent, Qt::WindowFlags fl)
    : QDialog(parent, fl), app_(qobject_cast<ApplicationWindow *>(parent)) {
  Q_ASSERT(app_);
  setWindowTitle(tr("Integration Options"));
  setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

  QGroupBox *gb1 = new QGroupBox();
  QGridLayout *gl1 = new QGridLayout(gb1);
  gl1->addWidget(new QLabel(tr("Integration of")), 0, 0);
  boxName = new QComboBox();
  gl1->addWidget(boxName, 0, 1);

  gl1->addWidget(new QLabel(tr("Interpolation")), 1, 0);
  boxMethod = new QComboBox();
  boxMethod->addItem(tr("Linear"));
  boxMethod->addItem(tr("Cubic"));
  boxMethod->addItem(tr("Non-rounded Akima"));
  gl1->addWidget(boxMethod, 1, 1);

  gl1->addWidget(new QLabel(tr("Lower limit")), 4, 0);
  boxStart = new QLineEdit();
  gl1->addWidget(boxStart, 4, 1);

  gl1->addWidget(new QLabel(tr("Upper limit")), 5, 0);
  boxEnd = new QLineEdit();
  gl1->addWidget(boxEnd, 5, 1);
  gl1->setRowStretch(6, 1);

  buttonOk = new QPushButton(tr("&Integrate"));
  buttonOk->setDefault(true);
  buttonHelp = new QPushButton(tr("&Help"));
  buttonCancel = new QPushButton(tr("&Close"));

  QVBoxLayout *vl = new QVBoxLayout();
  vl->addWidget(buttonOk);
  vl->addWidget(buttonHelp);
  vl->addWidget(buttonCancel);
  vl->addStretch();

  QHBoxLayout *hb = new QHBoxLayout(this);
  hb->addWidget(gb1);
  hb->addLayout(vl);

  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
  connect(buttonHelp, SIGNAL(clicked()), this, SLOT(help()));
  connect(boxName, SIGNAL(activated(const QString &)), this,
          SLOT(activateCurve(const QString &)));
}

void IntDialog::accept() {
  if (!axisrect_) return;
  QString curve = boxName->currentText();
  QStringList curvesList =
      PlotColumns::getstringlistfromassociateddata(axisrect_);
  if (!curvesList.contains(curve)) {
    QMessageBox::critical(
        this, tr("Warning"),
        tr("The curve <b> %1 </b> doesn't exist anymore! Operation aborted!")
            .arg(curve));
    boxName->clear();
    boxName->addItems(curvesList);
    return;
  }

  PlotData::AssociatedData *associateddata =
      PlotColumns::getassociateddatafromstring(axisrect_,
                                               boxName->currentText());
  Column *col = associateddata->xcol;
  double minx = col->valueAt(associateddata->from);
  double maxx = col->valueAt(associateddata->from);
  for (int i = associateddata->from; i < associateddata->to + 1; i++) {
    double value = col->valueAt(i);
    if (minx > value) minx = value;
    if (maxx < value) maxx = value;
  }
  double start = 0;
  double stop = 0;

  // Check the Xmin
  QString from = boxStart->text().toLower();
  if (from == "min") {
    boxStart->setText(QString::number(minx));
    return;
  } else if (from == "max") {
    boxStart->setText(QString::number(maxx));
    return;
  } else {
    try {
      MyParser parser;
      parser.SetExpr((boxStart->text()).toUtf8().constData());
      start = parser.Eval();

      if (start < minx) {
        QMessageBox::warning(
            app_, tr("Input error"),
            tr("Please give a number larger or equal to the minimum value of "
               "X, for the lower limit.\n If you do not know that value, type "
               "min in the box."));
        boxStart->clear();
        boxStart->setFocus();
        return;
      }
      if (start > maxx) {
        QMessageBox::warning(
            app_, tr("Input error"),
            tr("Please give a number smaller or equal to the maximum value of "
               "X, for the lower limit.\n If you do not know that value, type "
               "max in the box."));
        boxStart->clear();
        boxStart->setFocus();
        return;
      }
    } catch (mu::ParserError &e) {
      QMessageBox::critical(app_, tr("Start limit error"),
                            QString::fromStdString(e.GetMsg()));
      boxStart->clear();
      boxStart->setFocus();
      return;
    }
  }

  // Check Xmax
  QString end = boxEnd->text().toLower();
  if (end == "min") {
    boxEnd->setText(QString::number(minx));
    return;
  } else if (end == "max") {
    boxEnd->setText(QString::number(maxx));
    return;
  } else {
    try {
      MyParser parser;
      parser.SetExpr((boxEnd->text()).toUtf8().constData());
      stop = parser.Eval();
      if (stop > maxx) {
        // FIXME: I don't understand why this doesn't work for
        // FunctionCurves!!(Ion)
        /*QMessageBox::warning((ApplicationWindow *)parent(), tr("Input error"),
                tr("Please give a number smaller or equal to the maximum value
        of X, for the upper limit.\n If you do not know that value, type max in
        the box."));
        boxEnd->clear();
        boxEnd->setFocus();
        return;
        */
        boxEnd->setText(QString::number(maxx));
      }
      if (stop < minx) {
        QMessageBox::warning(
            app_, tr("Input error"),
            tr("Please give a number larger or equal to the minimum value of "
               "X, for the upper limit.\n If you do not know that value, type "
               "min in the box."));
        boxEnd->clear();
        boxEnd->setFocus();
        return;
      }
    } catch (mu::ParserError &e) {
      QMessageBox::critical(app_, tr("End limit error"),
                            QString::fromStdString(e.GetMsg()));
      boxEnd->clear();
      boxEnd->setFocus();
      return;
    }
  }

  Integration *i =
      new Integration(app_, axisrect_, associateddata,
                      boxStart->text().toDouble(), boxEnd->text().toDouble());
  i->setMethod(
      static_cast<Integration::InterpolationMethod>(boxMethod->currentIndex()));
  i->run();
  delete i;
}

void IntDialog::setAxisrect(AxisRect2D *axisrect) {
  if (!axisrect) return;
  axisrect_ = axisrect;
  boxName->addItems(PlotColumns::getstringlistfromassociateddata(axisrect_));
  activateCurve(boxName->currentText());
}

void IntDialog::activateCurve(const QString &curveName) {
  if (!axisrect_) return;
  PlotData::AssociatedData *associateddata;
  associateddata =
      PlotColumns::getassociateddatafromstring(axisrect_, curveName);
  if (!associateddata) return;

  Column *col = associateddata->xcol;
  xmin_ = col->valueAt(associateddata->from);
  xmax_ = col->valueAt(associateddata->from);
  for (int i = associateddata->from; i < associateddata->to + 1; i++) {
    double value = col->valueAt(i);
    if (xmin_ > value) xmin_ = value;
    if (xmax_ < value) xmax_ = value;
  }
  boxStart->setText(
      QString::number(std::min(xmin_, xmax_), 'g', app_->d_decimal_digits));
  boxEnd->setText(
      QString::number(std::max(xmin_, xmax_), 'g', app_->d_decimal_digits));
}

void IntDialog::changeDataRange() {
  double start = xmin_;
  double end = xmax_;
  boxStart->setText(
      QString::number(std::min(start, end), 'g', app_->d_decimal_digits));
  boxEnd->setText(
      QString::number(std::max(start, end), 'g', app_->d_decimal_digits));
}

void IntDialog::help() {
  QMessageBox::about(
      this, tr("Help for Integration"),
      tr("The integration of a curve consists of the following five steps:\n "
         "1) Choose which curve you want to integrate\n 2) Set the order of "
         "the integration. The higher it is the more accurate the calculation "
         "is\n 3) Choose the number of iterations \n 4) Choose the tolerance "
         "\n 5) Choose the lower and the upper limit.\n The code integrates "
         "the curve with an iterative algorithm. The tolerance determines the "
         "termination criteria for the solver.\n Because, sometimes we ask for "
         "too much accuracy, the number of iterations makes sure that the "
         "solver will not work for ever.\n IMPORTANT \nThe limits must be "
         "within the range of x; If you do not know the maximum (minimum) "
         "value of x, type max (min) in the boxes."));
}
