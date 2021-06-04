#ifndef GENERALCONFIRMATIONSETTINGS_H
#define GENERALCONFIRMATIONSETTINGS_H

#include "SettingsPage.h"

class Ui_GeneralConfirmationSettings;

class GeneralConfirmationSettings : public SettingsPage {
  Q_OBJECT

 public:
  explicit GeneralConfirmationSettings(SettingsDialog *dialog);
  ~GeneralConfirmationSettings();

  void Load();
  void LoadDefault();
  void Save();
  bool settingsChangeCheck();

 signals:
  void generalconfirmationsettingsupdate();

 private:
  void loadQsettingsValues();
  Ui_GeneralConfirmationSettings *ui;
  bool confirmCloseFolder_;
  bool confirmCloseTable_;
  bool confirmCloseMatrix_;
  bool confirmClosePlot2D_;
  bool confirmClosePlot3D_;
  bool confirmCloseNotes_;
};

#endif  // GENERALCONFIRMATIONSETTINGS_H
