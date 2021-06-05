#ifndef GENERALNUMERICFORMATSETTINGS_H
#define GENERALNUMERICFORMATSETTINGS_H

#include <QWidget>

#include "SettingsPage.h"

class Ui_GeneralNumericFormatSettings;
class QLocale;

class GeneralNumericFormatSettings : public SettingsPage {
  Q_OBJECT

 public:
  explicit GeneralNumericFormatSettings(SettingsDialog *dialog = nullptr);
  ~GeneralNumericFormatSettings();

  void Load();
  void LoadDefault();
  void Save();
  bool settingsChangeCheck();

 signals:
  void generalnumericformatsettingsupdate();

 private slots:
  void updatePreview();

 private:
  void loadQsettingsValues();
  Ui_GeneralNumericFormatSettings *ui;
  int precision_;
  QString localestring_;
  bool usegroupseperator_;
  char defaultnumericformat_;
};

#endif  // GENERALNUMERICFORMATSETTINGS_H
