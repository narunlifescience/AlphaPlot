#ifndef APPLICATIONSETTINGSPAGE_H
#define APPLICATIONSETTINGSPAGE_H

#include "SettingsPage.h"

class Ui_ApplicationSettingsPage;

class ApplicationSettingsPage : public SettingsPage {
  Q_OBJECT

 public:
  explicit ApplicationSettingsPage(SettingsDialog *dialog);
  ~ApplicationSettingsPage();

  void Load();
  void LoadDefault();
  void Save();
  bool settingsChangeCheck();

 signals:
  void generalapplicationsettingsupdate();

 private:
  void loadQsettingsValues();
  void pickColor();
  void pickApplicationFont();
  void insertLanguagesList();
  Ui_ApplicationSettingsPage *ui;
  bool glowstatus_;
  QColor glowcolor_;
  int glowradius_;
  QString applanguage_;
  QString defaultscriptinglang_;
  bool autosave_;
  int autosavetime_;
  int undolimit_;
  QFont applicationfont_;
  bool autosearchupdates_;
};

#endif  // APPLICATIONSETTINGSPAGE_H
