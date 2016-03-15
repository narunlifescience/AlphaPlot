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

   Description : about box.
*/

#include "About.h"
#include "globals.h"

About::About(QWidget* parent) : QDialog(parent) {
  ui_.setupUi(this);
  ui_.versionLabel->setText("AlpaPlot " + AlphaPlot::versionString() + AlphaPlot::extraVersion());
  ui_.releaseDateLabel->setText(QObject::tr("Released") + ": " +
                                 AlphaPlot::releaseDateString());
}
