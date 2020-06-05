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

#include "../core/IconLoader.h"
#include "GeneralAppreanceSettings.h"
#include "ui_GeneralAppreanceSettings.h"

#include <QStyleFactory>

GeneralAppreanceSettings::GeneralAppreanceSettings(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_GeneralAppreanceSettings) {
  ui->setupUi(this);
  setWindowIcon(IconLoader::load("preferences-general-apperance",
   IconLoader::General));
  setWindowTitle(tr("Apperance"));
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
  ui->gridLayout->setContentsMargins(3, 3, 3, 3);
  QStringList styles = QStyleFactory::keys();
  styles.sort();
  ui->styleComboBox->addItems(styles);
  QStringList colorSchemes;
  colorSchemes << "default"
               << "alpha dark"
               << "smooth dark blue"
               << "smooth dark green"
               << "smooth dark orange"
               << "smooth light blue"
               << "smooth light green"
               << "smooth light orange";
  ui->colorSchemeComboBox->addItems(colorSchemes);
  connect(ui->styleComboBox, SIGNAL(currentIndexChanged(QString)),
          SLOT(stylePreview(QString)));
  connect(ui->colorSchemeComboBox, &QComboBox::currentTextChanged, [&](QString index){

  });
}

GeneralAppreanceSettings::~GeneralAppreanceSettings() { delete ui; }

void GeneralAppreanceSettings::Load() {}

void GeneralAppreanceSettings::Save() {}

void GeneralAppreanceSettings::setTitle(QString title) {
  QFont font = ui->titleLabel->font();

  font.setPointSize(font.pointSize() + 2);
  font.setBold(true);
  font.setItalic(true);

  ui->titleLabel->setStyleSheet(
      "QLabel {padding-left: 5px;"
      " padding-right: 5px;"
      " padding-top: 5px;"
      " padding-bottom: 5px }");
  ui->titleLabel->setFont(font);
  ui->titleLabel->setText(title);
}

void GeneralAppreanceSettings::stylePreview(QString style) {
  QStyle *stylefinal = QStyleFactory::create(style.toLower());

  if (!stylefinal) {
    return;
  }

  ui->previewTabWidget->setStyle(stylefinal);
  ui->previewTab->setStyle(stylefinal);
  ui->previewGroupBox->setStyle(stylefinal);
  ui->previewRadioButton1->setStyle(stylefinal);
  ui->previewRadioButton2->setStyle(stylefinal);
  ui->previewLine->setStyle(stylefinal);
  ui->previewCheckBox->setStyle(stylefinal);
  ui->previewComboBox->setStyle(stylefinal);
  ui->previewProgressBar->setStyle(stylefinal);
  ui->previewSlider->setStyle(stylefinal);
  ui->previewSpinBox->setStyle(stylefinal);
  ui->previewPushButton->setStyle(stylefinal);
  ui->previewScrollBar->setStyle(stylefinal);
  ui->previewTableWidget->setStyle(stylefinal);
}
