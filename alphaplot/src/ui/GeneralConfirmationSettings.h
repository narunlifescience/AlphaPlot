#ifndef GENERALCONFIRMATIONSETTINGS_H
#define GENERALCONFIRMATIONSETTINGS_H

#include "SettingsPage.h"

class Ui_GeneralConfirmationSettings;

class GeneralConfirmationSettings : public SettingsPage
{
  Q_OBJECT

public:
  explicit GeneralConfirmationSettings(SettingsDialog *dialog);
  ~GeneralConfirmationSettings();

  void Load();
  void Save();
  void setTitle(QString title);

private:
  Ui_GeneralConfirmationSettings *ui;
};

#endif // GENERALCONFIRMATIONSETTINGS_H
