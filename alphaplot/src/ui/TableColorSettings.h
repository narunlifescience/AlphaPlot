#ifndef TABLECOLORSETTINGS_H
#define TABLECOLORSETTINGS_H

#include "SettingsPage.h"

class Ui_TableColorSettings;
class QToolButton;

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
  void setupColorLabel(QLabel *label, QToolButton *button);
  void pickColor(QLabel *label);
  void loadQsettingsValues();
  QString setStyleSheetString(const QColor &color);
  Ui_TableColorSettings *ui;
  static const int btn_size;
  static const int lbl_line_width;
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
