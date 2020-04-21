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

#include "core/IconLoader.h"
#include "core/Utilities.h"
#include "globals.h"

About::About(QWidget* parent) : QDialog(parent) {
  ui_.setupUi(this);
  setWindowIcon(IconLoader::load("help-about", IconLoader::LightDark));

  ui_.gridLayout->setContentsMargins(0, 0, 0, 0);
  ui_.verticalLayout_2->setSpacing(0);
  ui_.verticalLayout_2->setContentsMargins(0, 0, 0, 0);
  ui_.horizontalLayout->setContentsMargins(0, 10, 0, 10);
  ui_.horizontalLayout->setSpacing(10);
  ui_.verticalLayout->setContentsMargins(0, 0, 0, 0);
  ui_.verticalLayout->setSpacing(0);

  ui_.versionLabel->setText(AlphaPlot::versionString() +
                            AlphaPlot::extraVersion());
  ui_.releaseDateLabel->setText(AlphaPlot::releaseDateString());
  (Utilities::getWordSizeApp() == 0)
      ? ui_.buildLabel->setText(tr("Unknown"))
      : (Utilities::getWordSizeApp() == 32) ? ui_.buildLabel->setText("x86")
                                            : ui_.buildLabel->setText("x86_64");
  ui_.buildLabel->setVisible(false);
  ui_.buildLabelCaption->setVisible(false);
  ui_.originalAuthorLabel->setText(AlphaPlot::originalAuthor());
  ui_.osLabel->setText(QString("%1 %2-bit")
                           .arg(Utilities::getOperatingSystem())
                           .arg(Utilities::getWordSizeOfOS()));
}
