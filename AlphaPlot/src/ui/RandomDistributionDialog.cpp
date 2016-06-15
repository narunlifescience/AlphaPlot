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
#include "ui_RandomDistributionDialog.h"

#include <QDebug>

RandomDistributionDialog::RandomDistributionDialog(QWidget *parent)
    : QDialog(parent), ui_(new Ui_RandomDistributionDialog) {
  ui_->setupUi(this);
  setModal(true);

  // Set layout margins
  ui_->horizontalLayout->setContentsMargins(3, 3, 3, 3);
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
  ui_->distComboBox->addItem(tr("F-distribution"), F);
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
  ui_->distComboBox->addItem(tr("Logarithmic Distribution"), Logarithmic);

  connect(ui_->distComboBox, SIGNAL(currentIndexChanged(int)),
          SLOT(distributionChanged(int)));
  connect(ui_->randomDistributionButtonBox, SIGNAL(accepted()), SLOT(ok()));
  connect(ui_->randomDistributionButtonBox, SIGNAL(rejected()), SLOT(cancel()));
}

RandomDistributionDialog::~RandomDistributionDialog() { delete ui_; }

void RandomDistributionDialog::distributionChanged(int index) {
  Distribution dist = static_cast<Distribution>(index);
  switch (dist) {
    case Gaussian: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("μ = "));
      ui_->parameter2Label->setText(QString::fromUtf8("σ = "));
      ui_->parameter1LineEdit->setText("0.0");
      ui_->parameter2LineEdit->setText("1.0");
    } break;
    case Exponential: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("λ = "));
      ui_->parameter1LineEdit->setText("1.0");
    } break;
    case Laplace: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("μ = "));
      ui_->parameter2Label->setText("a = ");
      ui_->parameter1LineEdit->setText("0.0");
      ui_->parameter2LineEdit->setText("1.0");
    } break;
    case ExponentialPower: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->show();
      ui_->parameter3Label->show();
      ui_->parameter1Label->setText(QString::fromUtf8("μ = "));
      ui_->parameter2Label->setText("a = ");
      ui_->parameter3Label->setText("b = ");
      ui_->parameter1LineEdit->setText("0.0");
      ui_->parameter2LineEdit->setText("1.0");
      ui_->parameter3LineEdit->setText("1.0");
    } break;
    case Cauchy: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("a = ");
      ui_->parameter1LineEdit->setText("1.0");
    } break;
    case Rayleigh: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("σ = "));
      ui_->parameter1LineEdit->setText("1.0");
    } break;
    case RayleighTail: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("σ = "));
      ui_->parameter2Label->setText("a = ");
      ui_->parameter1LineEdit->setText("1.0");
      ui_->parameter2LineEdit->setText("0.0");
    } break;
    case Landau: {
      ui_->parameter1LineEdit->hide();
      ui_->parameter1Label->hide();
      ui_->parameter2LineEdit->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
    } break;
    case LevyAlphaStable: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("c = ");
      ui_->parameter2Label->setText(QString::fromUtf8("α = "));
      ui_->parameter1LineEdit->setText("1.0");
      ui_->parameter2LineEdit->setText("1.0");
    } break;
    case LevySkewAlphaStable: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->show();
      ui_->parameter3Label->show();
      ui_->parameter1Label->setText("c = ");
      ui_->parameter2Label->setText(QString::fromUtf8("α = "));
      ui_->parameter3Label->setText(QString::fromUtf8("β = "));
      ui_->parameter1LineEdit->setText("1.0");
      ui_->parameter2LineEdit->setText("1.0");
      ui_->parameter3LineEdit->setText("1.0");
    } break;
    case Flat: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("a = ");
      ui_->parameter2Label->setText("b = ");
      ui_->parameter1LineEdit->setText("0.0");
      ui_->parameter2LineEdit->setText("1.0");
    } break;
    case Gamma:
    case Beta:
    case Pareto:
    case Weibull:
    case Gumbel1:
    case Gumbel2: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("a = ");
      ui_->parameter2Label->setText("b = ");
      ui_->parameter1LineEdit->setText("1.0");
      ui_->parameter2LineEdit->setText("1.0");
    } break;
    case Lognormal: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("σ = "));
      ui_->parameter2Label->setText(QString::fromUtf8("ζ = "));
      ui_->parameter1LineEdit->setText("1.0");
      ui_->parameter2LineEdit->setText("1.0");
    } break;
    case ChiSquared: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("ν = "));
      ui_->parameter1LineEdit->setText("1.0");
    } break;
    case F: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("ν1 = "));
      ui_->parameter2Label->setText(QString::fromUtf8("ν2 = "));
      ui_->parameter1LineEdit->setText("1.0");
      ui_->parameter2LineEdit->setText("1.0");
    } break;
    case t: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("ν = "));
      ui_->parameter1LineEdit->setText("1.0");
    } break;
    case Logistic: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("a = ");
      ui_->parameter1LineEdit->setText("1.0");
    } break;
    case Poisson: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText(QString::fromUtf8("μ = "));
      ui_->parameter1LineEdit->setText("0.0");
    } break;
    case Bernoulli:
    case Geometric:
    case Logarithmic: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->hide();
      ui_->parameter2Label->hide();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("p = ");
      ui_->parameter1LineEdit->setText("0.5");
    } break;
    case Binomial:
    case NegativeBinomial:
    case Pascal: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->hide();
      ui_->parameter3Label->hide();
      ui_->parameter1Label->setText("p = ");
      ui_->parameter2Label->setText("n = ");
      ui_->parameter1LineEdit->setText("0.5");
      ui_->parameter2LineEdit->setText("100");
    } break;
    case Hypergeometric: {
      ui_->parameter1LineEdit->show();
      ui_->parameter1Label->show();
      ui_->parameter2LineEdit->show();
      ui_->parameter2Label->show();
      ui_->parameter3LineEdit->show();
      ui_->parameter3Label->show();
      ui_->parameter1Label->setText("n1 = ");
      ui_->parameter2Label->setText("n2 = ");
      ui_->parameter3Label->setText("t = ");
      ui_->parameter1LineEdit->setText("1.0");
      ui_->parameter2LineEdit->setText("2.0");
      ui_->parameter3LineEdit->setText("3.0");
    } break;
  }
}

void RandomDistributionDialog::ok() { QDialog::accept(); }

void RandomDistributionDialog::cancel() { QDialog::reject(); }
