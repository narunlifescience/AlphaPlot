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
  bool settingsChangeCheck() { return true; }

 private:
  Ui_ApplicationSettingsPage *ui;
};

#endif  // APPLICATIONSETTINGSPAGE_H
