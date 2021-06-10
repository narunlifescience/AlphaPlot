#ifndef TABLEBASICSETTINGS_H
#define TABLEBASICSETTINGS_H

#include "SettingsPage.h"

class Ui_TableBasicSettings;

class TableBasicSettings : public SettingsPage
{
  Q_OBJECT

public:
  explicit TableBasicSettings(SettingsDialog *dialog);
  ~TableBasicSettings();

  void Load();
  void LoadDefault();
  void Save();
  bool settingsChangeCheck();

signals:
 void tablebasicsettingsupdate();

private:
 void loadQsettingsValues();
 void setColumnSeparator(const QString &sep);
  Ui_TableBasicSettings *ui;
  QString columnseparator_;
  bool show_table_comments_;
};

#endif // TABLEBASICSETTINGS_H
