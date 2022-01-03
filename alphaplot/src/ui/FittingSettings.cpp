#include "FittingSettings.h"

#include <QFrame>
#include <QSettings>

#include "../core/IconLoader.h"
#include "ui_FittingSettings.h"

FittingSettings::FittingSettings(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_FittingSettings) {
  ui->setupUi(this);
  setWindowIcon(IconLoader::load("preferences-fitting", IconLoader::General));
  ui->defaultsPushButton->setIcon(
      IconLoader::load("edit-column-description", IconLoader::LightDark));
  ui->resetPushButton->setIcon(
      IconLoader::load("edit-undo", IconLoader::LightDark));
  ui->applyPushButton->setIcon(
      IconLoader::load("dialog-ok-apply", IconLoader::LightDark));
  setWindowTitle(tr("Fitting"));
  setTitle(ui->titleLabel, windowTitle());
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
  ui->scrollArea->setVerticalScrollBarPolicy(
      Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  connect(ui->uniformXRadioButton, &QRadioButton::toggled, this,
          [&](bool checked) {
            if (checked) {
              ui->pointsLabel->setVisible(true);
              ui->pointsSpinBox->setVisible(true);
              ui->linearFitCheckBox->setVisible(true);
            } else {
              ui->pointsLabel->setVisible(false);
              ui->pointsSpinBox->setVisible(false);
              ui->linearFitCheckBox->setVisible(false);
            }
          });
  connect(ui->applyPushButton, &QPushButton::clicked, this,
          &FittingSettings::Save);
  connect(ui->resetPushButton, &QPushButton::clicked, this,
          &FittingSettings::Load);
  connect(ui->defaultsPushButton, &QPushButton::clicked, this,
          &FittingSettings::LoadDefault);
  Load();
}

FittingSettings::~FittingSettings() { delete ui; }

void FittingSettings::Load() {
  loadQsettingsValues();
  ui->uniformXRadioButton->setChecked(generateUniformFitPoints_);
  ui->pointsSpinBox->setValue(fitPoints_);
  ui->linearFitCheckBox->setChecked(d_2_linear_fit_points_);
  ui->multiPeakGroupBox->setChecked(generatePeakCurves_);
  ui->peakColorComboBox->setCurrentIndex(peakCurvesColor_);
  ui->significantDigitsSpinBox->setValue(fit_output_precision_);
  ui->writetoResultCheckBox->setChecked(writeFitResultsToLog_);
  ui->pastetoPlotCheckBox->setChecked(pasteFitResultsToPlot_);
  ui->scaleCheckBox->setChecked(fit_scale_errors_);
}

void FittingSettings::LoadDefault() {
  ui->uniformXRadioButton->setChecked(true);
  ui->pointsSpinBox->setValue(100);
  ui->linearFitCheckBox->setChecked(true);
  ui->multiPeakGroupBox->setChecked(true);
  ui->peakColorComboBox->setCurrentIndex(2);
  ui->significantDigitsSpinBox->setValue(15);
  ui->writetoResultCheckBox->setChecked(true);
  ui->pastetoPlotCheckBox->setChecked(false);
  ui->scaleCheckBox->setChecked(false);
}

void FittingSettings::Save() {
  QSettings settings;
  settings.beginGroup("Fitting");
  settings.setValue("OutputPrecision", ui->significantDigitsSpinBox->value());
  settings.setValue("PasteResultsToPlot", ui->pastetoPlotCheckBox->isChecked());
  settings.setValue("WriteResultsToLog",
                    ui->writetoResultCheckBox->isChecked());
  settings.setValue("GenerateFunction", ui->uniformXRadioButton->isChecked());
  settings.setValue("Points", ui->pointsSpinBox->value());
  settings.setValue("GeneratePeakCurves", ui->multiPeakGroupBox->isChecked());
  settings.setValue("PeaksColor", ui->peakColorComboBox->currentIndex());
  settings.setValue("ScaleErrors", ui->scaleCheckBox->isChecked());
  settings.setValue("TwoPointsLinearFit", ui->linearFitCheckBox->isChecked());
  settings.endGroup();  // Fitting
}

bool FittingSettings::settingsChangeCheck() {
  loadQsettingsValues();
  bool result = true;
  if (generateUniformFitPoints_ != ui->uniformXRadioButton->isChecked() ||
      fitPoints_ != ui->pointsSpinBox->value() ||
      d_2_linear_fit_points_ != ui->linearFitCheckBox->isChecked() ||
      generatePeakCurves_ != ui->multiPeakGroupBox->isChecked() ||
      peakCurvesColor_ != ui->peakColorComboBox->currentIndex() ||
      fit_output_precision_ != ui->significantDigitsSpinBox->value() ||

      writeFitResultsToLog_ != ui->writetoResultCheckBox->isChecked() ||
      pasteFitResultsToPlot_ != ui->pastetoPlotCheckBox->isChecked() ||
      fit_scale_errors_ != ui->scaleCheckBox->isChecked()) {
    result = settingsChanged();
  }
  return result;
}

void FittingSettings::loadQsettingsValues() {
  QSettings settings;
  settings.beginGroup("Fitting");
  fit_output_precision_ = settings.value("OutputPrecision", 15).toInt();
  pasteFitResultsToPlot_ = settings.value("PasteResultsToPlot", false).toBool();
  writeFitResultsToLog_ = settings.value("WriteResultsToLog", true).toBool();
  generateUniformFitPoints_ = settings.value("GenerateFunction", true).toBool();
  fitPoints_ = settings.value("Points", 100).toInt();
  generatePeakCurves_ = settings.value("GeneratePeakCurves", true).toBool();
  peakCurvesColor_ = settings.value("PeaksColor", 2).toInt();  // green color
  fit_scale_errors_ = settings.value("ScaleErrors", false).toBool();
  d_2_linear_fit_points_ = settings.value("TwoPointsLinearFit", true).toBool();
  settings.endGroup();  // Fitting
}
