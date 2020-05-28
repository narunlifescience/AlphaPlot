/***************************************************************************
    File                 : ExpDecayDialog.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Fit exponential decay dialog

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
#include "ExpDecayDialog.h"
#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Plotcolumns.h"
#include "ApplicationWindow.h"
#include "ColorBox.h"
#include "ExponentialFit.h"
#include "Fit.h"

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

ExpDecayDialog::ExpDecayDialog(int type, QWidget *parent, Qt::WindowFlags fl)
    : QDialog(parent, fl), app_(qobject_cast<ApplicationWindow *>(parent)) {
  Q_ASSERT(app_);
  slopes = type;

  setWindowTitle(tr("Verify initial guesses"));

  QGroupBox *gb1 = new QGroupBox();
  QGridLayout *gl1 = new QGridLayout();
  gl1->addWidget(new QLabel(tr("Exponential Fit of")), 0, 0);

  boxName = new QComboBox();
  connect(boxName, SIGNAL(activated(const QString &)), this,
          SLOT(activateCurve(const QString &)));
  gl1->addWidget(boxName, 0, 1);

  if (type < 0)
    dampingLabel = new QLabel(tr("Growth time"));
  else if (type == 1)
    dampingLabel = new QLabel(tr("Decay time"));
  else
    dampingLabel = new QLabel(tr("First decay time (t1)"));
  gl1->addWidget(dampingLabel, 1, 0);

  boxFirst = new QLineEdit();
  boxFirst->setText(tr("1"));
  gl1->addWidget(boxFirst, 1, 1);

  if (type > 1) {
    gl1->addWidget(new QLabel(tr("Second decay time (t2)")), 2, 0);

    boxSecond = new QLineEdit();
    boxSecond->setText(tr("1"));
    gl1->addWidget(boxSecond, 2, 1);

    thirdLabel = new QLabel(tr("Third decay time (t3)"));
    gl1->addWidget(thirdLabel, 3, 0);

    boxThird = new QLineEdit();
    boxThird->setText(tr("1"));
    gl1->addWidget(boxThird, 3, 1);

    if (type < 3) {
      thirdLabel->hide();
      boxThird->hide();
    }
  }

  if (type <= 1) {
    gl1->addWidget(new QLabel(tr("Amplitude")), 2, 0);
    boxAmplitude = new QLineEdit();
    boxAmplitude->setText(tr("1"));
    gl1->addWidget(boxAmplitude, 2, 1);
  }

  gl1->addWidget(new QLabel(tr("Y Offset")), 4, 0);
  boxYOffset = new QLineEdit();
  boxYOffset->setText(tr("0"));
  gl1->addWidget(boxYOffset, 4, 1);

  gl1->addWidget(new QLabel(tr("Initial time")), 5, 0);

  boxStart = new QLineEdit();
  boxStart->setText(tr("0"));
  gl1->addWidget(boxStart, 5, 1);

  gl1->addWidget(new QLabel(tr("Color")), 6, 0);
  boxColor = new ColorBox();
  boxColor->setColor(QColor(Qt::red));
  gl1->addWidget(boxColor, 6, 1);

  gb1->setLayout(gl1);

  buttonFit = new QPushButton(tr("&Fit"));
  buttonFit->setDefault(true);

  buttonCancel = new QPushButton(tr("&Close"));

  QBoxLayout *bl1 = new QBoxLayout(QBoxLayout::TopToBottom);
  bl1->addWidget(buttonFit);
  bl1->addWidget(buttonCancel);
  bl1->addStretch();

  QHBoxLayout *hlayout = new QHBoxLayout();
  hlayout->addWidget(gb1);
  hlayout->addLayout(bl1);
  setLayout(hlayout);

  // signals and slots connections
  connect(buttonFit, SIGNAL(clicked()), this, SLOT(fit()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(close()));
}

void ExpDecayDialog::setAxisRect(AxisRect2D *axisrect) {
  if (!axisrect) return;

  fitter = nullptr;
  axisrect_ = axisrect;

  boxName->addItems(PlotColumns::getstringlistfromassociateddata(axisrect_));
  activateCurve(boxName->currentText());
}

void ExpDecayDialog::activateCurve(const QString &curveName) {
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

  Column *ycol = associateddata->ycol;
  double ymin = ycol->valueAt(associateddata->from);
  double ymax = ycol->valueAt(associateddata->from);
  for (int i = associateddata->from; i < associateddata->to + 1; i++) {
    double value = ycol->valueAt(i);
    if (ymin > value) ymin = value;
    if (ymax < value) ymax = value;
  }

  int precision = app_->fit_output_precision;
  double start = 0;
  double end = 0;
  boxStart->setText(QString::number(std::min(start, end)));
  boxYOffset->setText(QString::number(ymin, 'g', precision));
  if (slopes < 2)
    boxAmplitude->setText(QString::number(ymax - ymin, 'g', precision));
}

void ExpDecayDialog::changeDataRange() {
  double start = xmin_;
  double end = xmax_;
  boxStart->setText(QString::number(std::min(start, end), 'g', 15));
}

void ExpDecayDialog::fit() {
  QString curve = boxName->currentText();
  PlotData::AssociatedData *associateddata =
      PlotColumns::getassociateddatafromstring(axisrect_, curve);
  QStringList curvesList =
      PlotColumns::getstringlistfromassociateddata(axisrect_);
  if (!associateddata || !curvesList.contains(curve)) {
    QMessageBox::critical(
        this, tr("Warning"),
        tr("The curve <b> %1 </b> doesn't exist anymore! Operation aborted!")
            .arg(curve));
    boxName->clear();
    boxName->addItems(curvesList);
    return;
  }

  int precision = app_->fit_output_precision;

  if (fitter) delete fitter;

  if (slopes == 3) {
    double x_init[7] = {1.0,
                        boxFirst->text().toDouble(),
                        1.0,
                        boxSecond->text().toDouble(),
                        1.0,
                        boxThird->text().toDouble(),
                        boxYOffset->text().toDouble()};
    fitter = new ThreeExpFit(app_, axisrect_);
    fitter->setInitialGuesses(x_init);
  } else if (slopes == 2) {
    double x_init[5] = {1.0, boxFirst->text().toDouble(), 1.0,
                        boxSecond->text().toDouble(),
                        boxYOffset->text().toDouble()};
    fitter = new TwoExpFit(app_, axisrect_);
    fitter->setInitialGuesses(x_init);
  } else if (slopes == 1 || slopes == -1) {
    double x_init[3] = {boxAmplitude->text().toDouble(),
                        slopes / boxFirst->text().toDouble(),
                        boxYOffset->text().toDouble()};
    fitter = new ExponentialFit(app_, axisrect_, slopes == -1);
    fitter->setInitialGuesses(x_init);
  }

  if (fitter->setDataFromCurve(associateddata, boxStart->text().toDouble(),
                               xmax_)) {
    fitter->setColor(boxColor->currentIndex());
    fitter->scaleErrors(app_->fit_scale_errors);
    fitter->setOutputPrecision(app_->fit_output_precision);
    fitter->generateFunction(app_->generateUniformFitPoints, app_->fitPoints);
    fitter->fit();

    double *results = fitter->results();
    boxFirst->setText(QString::number(results[1], 'g', precision));
    if (slopes < 2) {
      boxAmplitude->setText(QString::number(results[0], 'g', precision));
      boxYOffset->setText(QString::number(results[2], 'g', precision));
    } else if (slopes == 2) {
      boxSecond->setText(QString::number(results[3], 'g', precision));
      boxYOffset->setText(QString::number(results[4], 'g', precision));
    } else if (slopes == 3) {
      boxSecond->setText(QString::number(results[3], 'g', precision));
      boxThird->setText(QString::number(results[5], 'g', precision));
      boxYOffset->setText(QString::number(results[6], 'g', precision));
    }
  }
}

void ExpDecayDialog::closeEvent(QCloseEvent *e) {
  if (fitter) {
    delete fitter;
  }

  e->accept();
}
