/* This file is part of AlphaPlot.
   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : General appearance settings page.
*/

#ifndef GENERALAPPREANCESETTINGS_H
#define GENERALAPPREANCESETTINGS_H

#include "SettingsPage.h"

class Ui_GeneralAppreanceSettings;
class QLabel;
class QToolButton;
class ColorLabel;

class GeneralAppreanceSettings : public SettingsPage {
  Q_OBJECT

 public:
  explicit GeneralAppreanceSettings(SettingsDialog *dialog);
  ~GeneralAppreanceSettings();

  void Load();
  void LoadDefault();
  void Save();
  bool settingsChangeCheck();

 signals:
  void generalappreancesettingsupdate();

 private:
  void setupColorButton(QToolButton *button);
  void pickColor(ColorLabel *label);
  void loadQsettingsValues();
  Ui_GeneralAppreanceSettings *ui;
  QString appstyle_;
  int colorscheme_;
  bool customcolors_;
  QColor workspacecolor_;
  QColor panelcolor_;
  QColor paneltextcolor_;

 private slots:
  void stylePreview(const QString &style);
  void colorStylePreview(int index);
};

#endif  // GENERALAPPREANCESETTINGS_H
