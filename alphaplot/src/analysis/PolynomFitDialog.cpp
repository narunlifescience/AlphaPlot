/***************************************************************************
    File                 : PolynomFitDialog.cpp
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
#include "PolynomFitDialog.h"
#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Plotcolumns.h"
#include "ApplicationWindow.h"
#include "ColorBox.h"
#include "PolynomialFit.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>

PolynomFitDialog::PolynomFitDialog(QWidget *parent, Qt::WindowFlags fl)
    : QDialog(parent, fl), app_(qobject_cast<ApplicationWindow *>(parent)) {
  Q_ASSERT(app_);
  setWindowTitle(tr("Polynomial Fit Options"));
  setSizeGripEnabled(true);

  QGroupBox *gb1 = new QGroupBox();
  QGridLayout *gl1 = new QGridLayout(gb1);
  gl1->addWidget(new QLabel(tr("Polynomial Fit of")), 0, 0);

  boxName = new QComboBox();
  gl1->addWidget(boxName, 0, 1);

  gl1->addWidget(new QLabel(tr("Order (1 - 9, 1 = linear)")), 1, 0);
  boxOrder = new QSpinBox();
  boxOrder->setRange(1, 9);
  boxOrder->setValue(2);
  gl1->addWidget(boxOrder, 1, 1);

  gl1->addWidget(new QLabel(tr("Fit curve Xmin")), 2, 0);
  boxStart = new QLineEdit(tr("0"));
  gl1->addWidget(boxStart, 2, 1);

  gl1->addWidget(new QLabel(tr("Fit curve Xmax")), 3, 0);
  boxEnd = new QLineEdit();
  gl1->addWidget(boxEnd, 3, 1);

  gl1->addWidget(new QLabel(tr("Color")), 4, 0);
  boxColor = new ColorBox();
  boxColor->setColor(QColor(Qt::red));
  gl1->addWidget(boxColor, 4, 1);

  boxShowFormula = new QCheckBox(tr("Show Formula on Graph?"));
  boxShowFormula->setChecked(false);
  gl1->addWidget(boxShowFormula, 5, 1);
  gl1->setRowStretch(6, 1);

  buttonFit = new QPushButton(tr("&Fit"));
  buttonFit->setDefault(true);

  buttonCancel = new QPushButton(tr("&Close"));

  QVBoxLayout *vl = new QVBoxLayout();
  vl->addWidget(buttonFit);
  vl->addWidget(buttonCancel);
  vl->addStretch();

  QHBoxLayout *hlayout = new QHBoxLayout(this);
  hlayout->addWidget(gb1);
  hlayout->addLayout(vl);

  connect(buttonFit, SIGNAL(clicked()), this, SLOT(fit()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
  connect(boxName, SIGNAL(activated(const QString &)), this,
          SLOT(activateCurve(const QString &)));
}

void PolynomFitDialog::fit() {
  QString curveName = boxName->currentText();
  QStringList curvesList =
      PlotColumns::getstringlistfromassociateddata(axisrect_);
  if (!curvesList.contains(curveName)) {
    QMessageBox::critical(
        this, tr("Warning"),
        tr("The curve <b> %1 </b> doesn't exist anymore! Operation aborted!")
            .arg(curveName));
    boxName->clear();
    boxName->addItems(curvesList);
    return;
  }

  PolynomialFit *fitter = new PolynomialFit(app_, axisrect_, boxOrder->value());
  if (fitter->setDataFromCurve(
          PlotColumns::getassociateddatafromstring(axisrect_, curveName),
          boxStart->text().toDouble(), boxEnd->text().toDouble())) {
    fitter->setColor(boxColor->currentIndex());
    fitter->scaleErrors(app_->fit_scale_errors);
    fitter->setOutputPrecision(app_->fit_output_precision);
    fitter->generateFunction(app_->generateUniformFitPoints, app_->fitPoints);
    fitter->fit();
    delete fitter;
  }
}

void PolynomFitDialog::setAxisRect(AxisRect2D *axisrect) {
  axisrect_ = axisrect;
  boxName->addItems(PlotColumns::getstringlistfromassociateddata(axisrect_));
  activateCurve(boxName->currentText());
}

void PolynomFitDialog::activateCurve(const QString &curveName) {
  Q_UNUSED(curveName);
  double start = 0;
  double end = 0;
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

  boxStart->setText(QString::number(start, 'g', 15));
  boxEnd->setText(QString::number(end, 'g', 15));
}

void PolynomFitDialog::changeDataRange() {
  double start = xmin_;
  double end = xmax_;
  boxStart->setText(QString::number(std::min(start, end), 'g', 15));
  boxEnd->setText(QString::number(std::max(start, end), 'g', 15));
}
