#ifndef FITTINGSETTINGS_H
#define FITTINGSETTINGS_H

#include "SettingsPage.h"

class Ui_FittingSettings;

class FittingSettings : public SettingsPage {
  Q_OBJECT

 public:
  explicit FittingSettings(SettingsDialog *dialog = nullptr);
  ~FittingSettings();

  void Load();
  void LoadDefault();
  void Save();
  bool settingsChangeCheck();

 signals:
  void fittingsettingsupdate();

 private:
  void loadQsettingsValues();
  Ui_FittingSettings *ui;
  bool generateUniformFitPoints_;
  int fitPoints_;
  bool generatePeakCurves_;
  int peakCurvesColor_;
  int fit_output_precision_;
  bool writeFitResultsToLog_;
  bool pasteFitResultsToPlot_;
  bool fit_scale_errors_;
  bool d_2_linear_fit_points_;
};

#endif  // FITTINGSETTINGS_H
