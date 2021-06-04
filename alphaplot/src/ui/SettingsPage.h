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

   Description : Class inherited by all settings pages
*/

#ifndef SETTINGSPAGE_H
#define SETTINGSPAGE_H

#include <QWidget>

class QLabel;
class SettingsDialog;

class SettingsPage : public QWidget {
  Q_OBJECT

 public:
  SettingsPage(SettingsDialog* dialog = nullptr);

  // Return false to grey out the page's item in the list.
  virtual bool IsEnabled() const { return true; }

  // Load is called when the dialog is shown, Save when the user clicks
  // Apply, & Cancel when the user clicks on Cancel
  virtual void Load() = 0;
  virtual void LoadDefault() = 0;
  virtual void Save() = 0;
  virtual void Cancel() {}
  virtual bool settingsChangeCheck() = 0;

  // The dialog that this page belongs to.
  SettingsDialog* dialog() const { return dialog_; }
  void setTitle(QLabel *titleLabel, QString title);
  QString getTitle();

 protected:
  bool settingsChanged();

 private:
  SettingsDialog* dialog_;
  QString title_;
};

#endif  // SETTINGSPAGE_H
