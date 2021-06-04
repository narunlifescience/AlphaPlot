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

#include "SettingsPage.h"

#include <QLabel>
#include <QMessageBox>

#include "SettingsDialog.h"

SettingsPage::SettingsPage(SettingsDialog* dialog)
    : QWidget(dialog), dialog_(dialog) {}

void SettingsPage::setTitle(QLabel* titleLabel, QString title) {
  if (!titleLabel) return;

  title_ = title;
  QFont font = titleLabel->font();
  font.setItalic(true);

  titleLabel->setStyleSheet(
      "QLabel {padding-left: 5px;"
      " padding-right: 5px;"
      " padding-top: 5px;"
      " padding-bottom: 5px }");
  titleLabel->setFont(font);
  titleLabel->setText(title_);
}

bool SettingsPage::settingsChanged() {
  bool result = true;
  int ret = QMessageBox::warning(
      this, tr("Apply Sttings"),
      tr("Settings of current module have changed. Do you want to apply the "
         "changes or discard them?"),
      QMessageBox::Apply | QMessageBox::Discard | QMessageBox::Cancel);
  switch (ret) {
    case QMessageBox::Apply:
      Save();
      break;
    case QMessageBox::Discard:
      Load();
      break;
    case QMessageBox::Cancel:
      result = false;
      break;
    default:
      Load();
      break;
  }
  return result;
}
