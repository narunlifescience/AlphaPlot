/* This file is part of LabPlot & AlphaPlot.
   Copyright 2014, Alexander Semke <alexander.semke@web.de>
   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>

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

   Description : Random distribution dialog for selection
*/

#include "RandomDistributionDialog.h"

#include <QDebug>
#include <QSettings>
#include <cmath>

#include "../core/IconLoader.h"
#include "ui_RandomDistributionDialog.h"

RandomDistributionDialog::Distribution RandomDistributionDialog::distribution;
QVector<double> RandomDistributionDialog::parameters;
// QString RandomDistributionDialog::distFormulaPath;

RandomDistributionDialog::RandomDistributionDialog(QWidget *parent)
    : QDialog(parent), ui_(new Ui_RandomDistributionDialog) {
  ui_->setupUi(this);
  setWindowIcon(IconLoader::load("edit-random-dist", IconLoader::LightDark));
  ui_->distFormulaLabel->setAlignment(Qt::AlignCenter);
  setModal(true);

  // Theaming based pixmap loading
  (IconLoader::lumen_ < 100)
      ? distFormulaPath = QString(":icons/dark/distribution/")
      : distFormulaPath = QString(":icons/light/distribution/");

  // Set layout margins
  ui_->verticalLayout->setContentsMargins(3, 3, 3, 3);
  ui_->gridLayout->setContentsMargins(0, 0, 0, 0);

  // Add distributions
  ui_->distComboBox->addItem(tr("Gaussian Distribution"), Gaussian);
  // ui_->distComboBox->addItem(tr("Gaussian Tail Distribution"));
  ui_->distComboBox->addItem(tr("Exponential Distribution"), Exponential);
  ui_->distComboBox->addItem(tr("Laplace Distribution"), Laplace);
  ui_->distComboBox->addItem(tr("Exponential Power Distribution"),
                             ExponentialPower);
  ui_->distComboBox->addItem(tr("Cauchy Distribution"), Cauchy);
  ui_->distComboBox->addItem(tr("Rayleigh Distribution"), Rayleigh);
  ui_->distComboBox->addItem(tr("Rayleigh Tail Distribution"), RayleighTail);
  ui_->distComboBox->addItem(tr("Landau Distribution"), Landau);
  ui_->distComboBox->addItem(tr("Levy alpha-stable Distribution"),
                             LevyAlphaStable);
  ui_->distComboBox->addItem(tr("Levy skew alpha-stable Distribution"),
                             LevySkewAlphaStable);
  ui_->distComboBox->addItem(tr("Gamma Distribution"), Gamma);
  ui_->distComboBox->addItem(tr("Flat (Uniform) Distribution"), Flat);
  ui_->distComboBox->addItem(tr("Lognormal Distribution"), Lognormal);
  ui_->distComboBox->addItem(tr("Chi-squared Distribution"), ChiSquared);
  /*ui_->distComboBox->addItem(tr("F-distribution"), F);
  ui_->distComboBox->addItem(tr("t-distribution"), t);
  ui_->distComboBox->addItem(tr("Beta Distribution"), Beta);
  ui_->distComboBox->addItem(tr("Logistic Distribution"), Logistic);
  ui_->distComboBox->addItem(tr("Pareto Distribution"), Pareto);
  // ui_->distComboBox->addItem(tr("Spherical Vector Distributions"));
  ui_->distComboBox->addItem(tr("Weibull Distribution"), Weibull);
  ui_->distComboBox->addItem(tr("Type-1 Gumbel Distribution"), Gumbel1);
  ui_->distComboBox->addItem(tr("Type-2 Gumbel Distribution"), Gumbel2);
  // ui_->distComboBox->addItem(tr("Dirichlet Distribution"));
  // ui_->distComboBox->addItem(tr("General Discrete Distributions"));
  ui_->distComboBox->addItem(tr("Poisson Distribution"), Poisson);
  ui_->distComboBox->addItem(tr("Bernoulli Distribution"), Bernoulli);
  ui_->distComboBox->addItem(tr("Binomial Distribution"), Binomial);
  // ui_->distComboBox->addItem(tr("Multinomial Distribution"));
  ui_->distComboBox->addItem(tr("Negative Binomial Distribution"),
                             NegativeBinomial);
  ui_->distComboBox->addItem(tr("Pascal Distribution"), Pascal);
  ui_->distComboBox->addItem(tr("Geometric Distribution"), Geometric);
  ui_->distComboBox->addItem(tr("Hypergeometric Distribution"), Hypergeometric);
  ui_->distComboBox->addItem(tr("Logarithmic Distribution"), Logarithmic);*/

  // Load settings
  QSettings settings;
  settings.beginGroup("General");
  int distIndex = settings.value("RandomDistribution", 0).toInt();
  ui_->distComboBox->setCurrentIndex(distIndex);
  distributionChanged(distIndex);
  ui_->parameter1DbleSpinBox->setValue(
      settings.value("RandomDistributionParam-1").toDouble());
  ui_->parameter2DbleSpinBox->setValue(
      settings.value("RandomDistributionParam-2").toDouble());
  ui_->parameter3DbleSpinBox->setValue(
      settings.value("RandomDistributionParam-3").toDouble());
  settings.endGroup();

  // set parameter bounds (move to switch case for more control)
  ui_->parameter1DbleSpinBox->setMinimum(-INFINITY);
  ui_->parameter1DbleSpinBox->setMaximum(std::numeric_limits<double>::max());
  ui_->parameter2DbleSpinBox->setMinimum(-INFINITY);
  ui_->parameter2DbleSpinBox->setMaximum(std::numeric_limits<double>::max());
  ui_->parameter3DbleSpinBox->setMinimum(-INFINITY);
  ui_->parameter3DbleSpinBox->setMaximum(std::numeric_limits<double>::max());

  // Slot connections
  connect(ui_->distComboBox, qOverload<int>(&QComboBox::currentIndexChanged),
          this, &RandomDistributionDialog::distributionChanged);
  connect(ui_->randomDistributionButtonBox, &QDialogButtonBox::accepted, this,
          &RandomDistributionDialog::ok);
  connect(ui_->randomDistributionButtonBox, &QDialogButtonBox::rejected, this,
          &RandomDistributionDialog::cancel);
}

RandomDistributionDialog::~RandomDistributionDialog() { delete ui_; }

void RandomDistributionDialog::distributionChanged(int index) {
  Distribution dist = static_cast<Distribution>(index);
  distribution = dist;
  switch (dist) {
    case Gaussian: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-gaussian.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("μ = "));
      ui_->parameter2Label->setText(QString::fromUtf8("σ = "));
      ui_->parameter1DbleSpinBox->setValue(0.0);
      ui_->parameter2DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 0.0 << 1.0 << nan("null");
    } break;
    case Exponential: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-exponential.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("λ = "));
      ui_->parameter1DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 1.0 << nan("null") << nan("null");
    } break;
    case Laplace: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-laplace.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("μ = "));
      ui_->parameter2Label->setText("a = ");
      ui_->parameter1DbleSpinBox->setValue(0.0);
      ui_->parameter2DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 0.0 << 1.0 << nan("null");
    } break;
    case ExponentialPower: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-exponential-power.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->show();
      ui_->parameter3Label->show();
      ui_->parameter1Label->setText(QString::fromUtf8("μ = "));
      ui_->parameter2Label->setText("a = ");
      ui_->parameter3Label->setText("b = ");
      ui_->parameter1DbleSpinBox->setValue(0.0);
      ui_->parameter2DbleSpinBox->setValue(1.0);
      ui_->parameter3DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 0.0 << 1.0 << 1.0;
    } break;
    case Cauchy: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-cauchy.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("a = ");
      ui_->parameter1DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 1.0 << nan("null") << nan("null");
    } break;
    case Rayleigh: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-rayleigh.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("σ = "));
      ui_->parameter1DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 1.0 << nan("null") << nan("null");
    } break;
    case RayleighTail: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-rayleigh-tail.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("σ = "));
      ui_->parameter2Label->setText("a = ");
      ui_->parameter1DbleSpinBox->setValue(1.0);
      ui_->parameter2DbleSpinBox->setValue(0.0);
      parameters.clear();
      parameters << 1.0 << 0.0 << nan("null");
    } break;
    case Landau: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-landau.png"));
      ui_->parameter1DbleSpinBox->hide();
      ui_->parameter1Label->hide();
      ui_->parameter2DbleSpinBox->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      parameters.clear();
      parameters << nan("null") << nan("null") << nan("null");
    } break;
    case LevyAlphaStable: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-levy-alpha-stable.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("c = ");
      ui_->parameter2Label->setText(QString::fromUtf8("α = "));
      ui_->parameter1DbleSpinBox->setValue(1.0);
      ui_->parameter2DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 1.0 << 1.0 << nan("null");
    } break;
    case LevySkewAlphaStable: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-levy-skew-alpha-stable.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->show();
      ui_->parameter3Label->show();
      ui_->parameter1Label->setText("c = ");
      ui_->parameter2Label->setText(QString::fromUtf8("α = "));
      ui_->parameter3Label->setText(QString::fromUtf8("β = "));
      ui_->parameter1DbleSpinBox->setValue(1.0);
      ui_->parameter2DbleSpinBox->setValue(1.0);
      ui_->parameter3DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 1.0 << 1.0 << 1.0;
    } break;
    case Flat: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-flat.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("a = ");
      ui_->parameter2Label->setText("b = ");
      ui_->parameter1DbleSpinBox->setValue(0.0);
      ui_->parameter2DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 0.0 << 1.0 << nan("null");
    } break;
    case Gamma:
      // deliberate fall through
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-gamma.png"));
    case Beta:
    case Pareto:
    case Weibull:
    case Gumbel1:
    case Gumbel2: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-gamma.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("a = ");
      ui_->parameter2Label->setText("b = ");
      ui_->parameter1DbleSpinBox->setValue(1.0);
      ui_->parameter2DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 1.0 << 1.0 << nan("null");
    } break;
    case Lognormal: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-lognormal.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("σ = "));
      ui_->parameter2Label->setText(QString::fromUtf8("ζ = "));
      ui_->parameter1DbleSpinBox->setValue(1.0);
      ui_->parameter2DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 1.0 << 1.0 << nan("null");
    } break;
    case ChiSquared: {
      ui_->distFormulaLabel->setPixmap(
          QPixmap(distFormulaPath + "dist-chisquared.png"));
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("ν = "));
      ui_->parameter1DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 1.0 << nan("null") << nan("null");
    } break;
    case F: {
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("ν1 = "));
      ui_->parameter2Label->setText(QString::fromUtf8("ν2 = "));
      ui_->parameter1DbleSpinBox->setValue(1.0);
      ui_->parameter2DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 1.0 << 1.0 << nan("null");
    } break;
    case t: {
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("ν = "));
      ui_->parameter1DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 1.0 << nan("null") << nan("null");
    } break;
    case Logistic: {
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("a = ");
      ui_->parameter1DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 1.0 << nan("null") << nan("null");
    } break;
    case Poisson: {
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("μ = "));
      ui_->parameter1DbleSpinBox->setValue(0.0);
      parameters.clear();
      parameters << 0.0 << nan("null") << nan("null");
    } break;
    case Bernoulli:
    case Geometric:
    case Logarithmic: {
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("p = ");
      ui_->parameter1DbleSpinBox->setValue(0.5);
      parameters.clear();
      parameters << 0.5 << nan("null") << nan("null");
    } break;
    case Binomial:
    case NegativeBinomial:
    case Pascal: {
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("p = ");
      ui_->parameter2Label->setText("n = ");
      ui_->parameter1DbleSpinBox->setValue(0.5);
      ui_->parameter2DbleSpinBox->setValue(100);
      parameters.clear();
      parameters << 0.5 << 100 << nan("null");
    } break;
    case Hypergeometric: {
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->show();
      ui_->parameter3Label->show();
      ui_->parameter1Label->setText("n1 = ");
      ui_->parameter2Label->setText("n2 = ");
      ui_->parameter3Label->setText("t = ");
      ui_->parameter1DbleSpinBox->setValue(1.0);
      ui_->parameter2DbleSpinBox->setValue(2.0);
      ui_->parameter3DbleSpinBox->setValue(3.0);
      parameters.clear();
      parameters << 1.0 << 2.0 << 3.0;
    } break;
  }
}

void RandomDistributionDialog::ok() {
  // set qvector params
  switch (distribution) {
    case Gaussian: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value()
                 << ui_->parameter2DbleSpinBox->value() << nan("null");
    } break;
    case Exponential: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value() << nan("null")
                 << nan("null");
    } break;
    case Laplace: {
      ui_->parameter1DbleSpinBox->show();
      ui_->parameter1Label->show();
      ui_->parameter2DbleSpinBox->show();
      ui_->parameter2Label->show();
      ui_->parameter3DbleSpinBox->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("μ = "));
      ui_->parameter2Label->setText("a = ");
      ui_->parameter1DbleSpinBox->setValue(0.0);
      ui_->parameter2DbleSpinBox->setValue(1.0);
      parameters.clear();
      parameters << 0.0 << 1.0 << nan("null");
    } break;
    case ExponentialPower: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value()
                 << ui_->parameter2DbleSpinBox->value()
                 << ui_->parameter3DbleSpinBox->value();
    } break;
    case Cauchy: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value() << nan("null")
                 << nan("null");
    } break;
    case Rayleigh: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value() << nan("null")
                 << nan("null");
    } break;
    case RayleighTail: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value()
                 << ui_->parameter2DbleSpinBox->value() << nan("null");
    } break;
    case Landau: {
      parameters.clear();
      parameters << nan("null") << nan("null") << nan("null");
    } break;
    case LevyAlphaStable: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value()
                 << ui_->parameter2DbleSpinBox->value() << nan("null");
    } break;
    case LevySkewAlphaStable: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value()
                 << ui_->parameter2DbleSpinBox->value()
                 << ui_->parameter3DbleSpinBox->value();
    } break;
    case Flat: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value()
                 << ui_->parameter2DbleSpinBox->value() << nan("null");
    } break;
    case Gamma:
    case Beta:
    case Pareto:
    case Weibull:
    case Gumbel1:
    case Gumbel2: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value()
                 << ui_->parameter2DbleSpinBox->value() << nan("null");
    } break;
    case Lognormal: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value()
                 << ui_->parameter2DbleSpinBox->value() << nan("null");
    } break;
    case ChiSquared: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value() << nan("null")
                 << nan("null");
    } break;
    case F: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value()
                 << ui_->parameter2DbleSpinBox->value() << nan("null");
    } break;
    case t: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value() << nan("null")
                 << nan("null");
    } break;
    case Logistic: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value() << nan("null")
                 << nan("null");
    } break;
    case Poisson: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value() << nan("null")
                 << nan("null");
    } break;
    case Bernoulli:
    case Geometric:
    case Logarithmic: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value() << nan("null")
                 << nan("null");
    } break;
    case Binomial:
    case NegativeBinomial:
    case Pascal: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value()
                 << ui_->parameter2DbleSpinBox->value() << nan("null");
    } break;
    case Hypergeometric: {
      parameters.clear();
      parameters << ui_->parameter1DbleSpinBox->value()
                 << ui_->parameter2DbleSpinBox->value()
                 << ui_->parameter3DbleSpinBox->value();
    } break;
  }

  emit randomDistribution(distribution, parameters);
  // Save settings
  QSettings settings;
  settings.beginGroup("General");
  settings.setValue("RandomDistribution", ui_->distComboBox->currentIndex());
  settings.setValue("RandomDistributionParam-1", parameters.at(0));
  settings.setValue("RandomDistributionParam-2", parameters.at(1));
  settings.setValue("RandomDistributionParam-3", parameters.at(2));
  settings.endGroup();
  QDialog::accept();
}

void RandomDistributionDialog::cancel() { QDialog::reject(); }
