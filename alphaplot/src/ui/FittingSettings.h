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

 private:
  Ui_FittingSettings *ui;
};

#endif  // FITTINGSETTINGS_H
