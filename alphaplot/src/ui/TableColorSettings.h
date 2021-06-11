#ifndef TABLECOLORSETTINGS_H
#define TABLECOLORSETTINGS_H

#include "SettingsPage.h"

class Ui_TableColorSettings;
class QToolButton;
class ColorLabel;

class TableColorSettings : public SettingsPage {
  Q_OBJECT

 public:
  explicit TableColorSettings(SettingsDialog *dialog);
  ~TableColorSettings();

  void Load();
  void LoadDefault();
  void Save();
  bool settingsChangeCheck();

 signals:
  void tablecolorsettingsupdate();

 private:
  void setupColorButton(QToolButton *button);
  void pickColor(ColorLabel *label);
  void loadQsettingsValues();
  Ui_TableColorSettings *ui;
  QColor xColorCode_;
  QColor yColorCode_;
  QColor zColorCode_;
  QColor xErrColorCode_;
  QColor yErrColorCode_;
  QColor noneColorCode_;
  bool customColor_;
  QColor bkgdColor_;
  QColor textColor_;
  QColor labelColor_;
};

#endif  // TABLECOLORSETTINGS_H
