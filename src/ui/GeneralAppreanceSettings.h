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

class GeneralAppreanceSettings : public SettingsPage
{
    Q_OBJECT

public:
    explicit GeneralAppreanceSettings(SettingsDialog *dialog);
    ~GeneralAppreanceSettings();

    void Load();
    void Save();
    void setTitle(QString title);

private:
    Ui_GeneralAppreanceSettings *ui;

private slots:
    void stylePreview(QString style);
};

#endif // GENERALAPPREANCESETTINGS_H
