#ifndef TABLEFONTSETTINGS_H
#define TABLEFONTSETTINGS_H

#include "SettingsPage.h"

class Ui_TableFontSettings;

class TableFontSettings : public SettingsPage
{
  Q_OBJECT

public:
  explicit TableFontSettings(SettingsDialog *dialog);
  ~TableFontSettings();

  void Load();
  void LoadDefault();
  void Save();
  bool settingsChangeCheck();

signals:
 void tablefontsettingsupdate();

private:
 void loadQsettingsValues();
 void pickTextFont();
 void pickLabelFont();
  Ui_TableFontSettings *ui;
  QFont textfont_;
  QFont labelfont_;
};

#endif // TABLEFONTSETTINGS_H
