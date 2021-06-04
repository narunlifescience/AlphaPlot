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

#include "GeneralAppreanceSettings.h"

#include <QColorDialog>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QStyleFactory>

#include "../core/IconLoader.h"
#include "globals.h"
#include "ui_GeneralAppreanceSettings.h"

const int GeneralAppreanceSettings::btn_size = 24;
const int GeneralAppreanceSettings::lbl_line_width = 1;

GeneralAppreanceSettings::GeneralAppreanceSettings(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_GeneralAppreanceSettings) {
  ui->setupUi(this);
  setWindowIcon(
      IconLoader::load("preferences-general-apperance", IconLoader::General));
  ui->defaultsPushButton->setIcon(
      IconLoader::load("edit-column-description", IconLoader::LightDark));
  ui->resetPushButton->setIcon(
      IconLoader::load("edit-undo", IconLoader::LightDark));
  ui->applyPushButton->setIcon(
      IconLoader::load("dialog-ok-apply", IconLoader::LightDark));
  setWindowTitle(tr("Apperance"));
  setTitle(ui->titleLabel, windowTitle());
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
  ui->scrollArea->setVerticalScrollBarPolicy(
      Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  ui->gridLayout->setContentsMargins(3, 3, 3, 3);
  QStringList styles = QStyleFactory::keys();
  styles.sort();
  ui->styleComboBox->addItems(styles);
  ui->colorSchemeComboBox->addItems(AlphaPlot::appColorSchemeName());
  ui->customColorGroupBox->setCheckable(true);
  ui->customColorGroupBox->setAlignment(Qt::AlignLeft);
  setupColorLabel(ui->panelColorLabel, ui->panelColorButton);
  setupColorLabel(ui->paneltextColorLabel, ui->paneltextColorButton);
  setupColorLabel(ui->workspaceColorLabel, ui->workspaceColorButton);
  connect(ui->applyPushButton, &QPushButton::clicked, this,
          &GeneralAppreanceSettings::Save);
  connect(ui->resetPushButton, &QPushButton::clicked, this,
          &GeneralAppreanceSettings::Load);
  connect(ui->defaultsPushButton, &QPushButton::clicked, this,
          &GeneralAppreanceSettings::LoadDefault);
  connect(ui->colorSchemeComboBox,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &GeneralAppreanceSettings::colorStylePreview);
  connect(ui->styleComboBox, &QComboBox::currentTextChanged, this,
          &GeneralAppreanceSettings::stylePreview);
  connect(ui->panelColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->panelColorLabel); });
  connect(ui->paneltextColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->paneltextColorLabel); });
  connect(ui->workspaceColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->workspaceColorLabel); });
  Load();
}

GeneralAppreanceSettings::~GeneralAppreanceSettings() { delete ui; }

void GeneralAppreanceSettings::Load() {
  loadQsettingsValues();
  ui->styleComboBox->setCurrentIndex(
      ui->styleComboBox->findText(appstyle_, Qt::MatchWildcard));
  ui->colorSchemeComboBox->setCurrentIndex(colorscheme_);
  ui->customColorGroupBox->setChecked(customcolors_);
  ui->workspaceColorLabel->setStyleSheet(setStyleSheetString(workspacecolor_));
  ui->panelColorLabel->setStyleSheet(setStyleSheetString(panelcolor_));
  ui->paneltextColorLabel->setStyleSheet(setStyleSheetString(paneltextcolor_));
}

void GeneralAppreanceSettings::LoadDefault() {
  ui->styleComboBox->setCurrentIndex(0);
  ui->colorSchemeComboBox->setCurrentIndex(0);
  ui->customColorGroupBox->setChecked(false);
}

void GeneralAppreanceSettings::Save() {
  QSettings settings;
  settings.beginGroup("General");
  settings.setValue("Style", ui->styleComboBox->currentText());
  settings.setValue("ColorScheme", ui->colorSchemeComboBox->currentIndex());
  settings.beginGroup("Colors");
  settings.setValue("Custom", ui->customColorGroupBox->isChecked());
  settings.setValue("Workspace",
                    ui->workspaceColorLabel->palette().window().color());
  settings.setValue("Panels", ui->panelColorLabel->palette().window().color());
  settings.setValue("PanelsText",
                    ui->paneltextColorLabel->palette().window().color());
  settings.endGroup();
  settings.endGroup();

  emit generalappreancesettingsupdate();
}

bool GeneralAppreanceSettings::settingsChangeCheck() {
  loadQsettingsValues();
  bool result = true;
  if (appstyle_.toLower() != ui->styleComboBox->currentText().toLower() ||
      colorscheme_ != ui->colorSchemeComboBox->currentIndex() ||
      customcolors_ != ui->customColorGroupBox->isChecked() ||
      workspacecolor_ != ui->workspaceColorLabel->palette().window().color() ||
      panelcolor_ != ui->panelColorLabel->palette().window().color() ||
      paneltextcolor_ != ui->paneltextColorLabel->palette().window().color()) {
    result = settingsChanged();
  }
  return result;
}

void GeneralAppreanceSettings::setupColorLabel(QLabel *label,
                                               QToolButton *button) {
  label->setFixedSize(btn_size, btn_size);
  (label->height() > label->width()) ? label->setFixedWidth(label->height())
                                     : label->setFixedHeight(label->width());
  button->setIcon(IconLoader::load("color-management", IconLoader::General));
  button->setStyleSheet("QToolButton {border: 0px;}");
}

void GeneralAppreanceSettings::pickColor(QLabel *label) {
  QPalette pal = label->palette();
  QColor color = QColorDialog::getColor(pal.window().color(), this);
  if (!color.isValid() || color == pal.window().color()) return;
  label->setStyleSheet(setStyleSheetString(color));
}

void GeneralAppreanceSettings::loadQsettingsValues() {
  QSettings settings;
  settings.beginGroup("General");
  appstyle_ = settings.value("Style", qApp->style()->objectName()).toString();
  colorscheme_ = settings.value("ColorScheme", 0).toInt();
  settings.beginGroup("Colors");
  customcolors_ = settings.value("Custom", false).toBool();
  workspacecolor_ = settings.value("Workspace", "darkGray").value<QColor>();
  panelcolor_ =
      settings.value("Panels", palette().window().color()).value<QColor>();
  paneltextcolor_ = settings.value("PanelsText", palette().windowText().color())
                        .value<QColor>();
  settings.endGroup();
  settings.endGroup();
}

QString GeneralAppreanceSettings::setStyleSheetString(const QColor &color) {
  QString stylesheetstring =
      "QLabel:!disabled{background : rgba(%1,%2, %3, %4); border: 1px solid "
      "rgba(%5, %6, %7, %8);}";
  QColor bordercolor = qApp->palette().windowText().color();
  return stylesheetstring.arg(color.red())
      .arg(color.green())
      .arg(color.blue())
      .arg(color.alpha())
      .arg(bordercolor.red())
      .arg(bordercolor.green())
      .arg(bordercolor.blue())
      .arg(bordercolor.alpha());
}

void GeneralAppreanceSettings::stylePreview(const QString &style) {
  QStyle *stylefinal = QStyleFactory::create(style.toLower());

  if (!stylefinal) {
    return;
  }

  ui->groupBox->setStyle(stylefinal);
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

void GeneralAppreanceSettings::colorStylePreview(int index) {
  switch (index) {
    case 0: {
      ui->groupBox->setStyleSheet(styleSheet());
    } break;
    case 1: {
      QFile schemefile(":style/alpha/dark.qss");
      schemefile.open(QFile::ReadOnly | QFile::Text);
      QTextStream schemeFileStream(&schemefile);
      ui->groupBox->setStyleSheet(schemeFileStream.readAll());
    } break;
    case 2: {
      QFile schemefile(":style/smooth/dark-blue.qss");
      schemefile.open(QFile::ReadOnly | QFile::Text);
      QTextStream schemeFileStream(&schemefile);
      ui->groupBox->setStyleSheet(schemeFileStream.readAll());
    } break;
    case 3: {
      QFile schemefile(":style/smooth/dark-green.qss");
      schemefile.open(QFile::ReadOnly | QFile::Text);
      QTextStream schemeFileStream(&schemefile);
      ui->groupBox->setStyleSheet(schemeFileStream.readAll());
    } break;
    case 4: {
      QFile schemefile(":style/smooth/dark-orange.qss");
      schemefile.open(QFile::ReadOnly | QFile::Text);
      QTextStream schemeFileStream(&schemefile);
      ui->groupBox->setStyleSheet(schemeFileStream.readAll());
    } break;
    case 5: {
      QFile schemefile(":style/smooth/light-blue.qss");
      schemefile.open(QFile::ReadOnly | QFile::Text);
      QTextStream schemeFileStream(&schemefile);
      ui->groupBox->setStyleSheet(schemeFileStream.readAll());
    } break;
    case 6: {
      QFile schemefile(":style/smooth/light-green.qss");
      schemefile.open(QFile::ReadOnly | QFile::Text);
      QTextStream schemeFileStream(&schemefile);
      ui->groupBox->setStyleSheet(schemeFileStream.readAll());
    } break;
    case 7: {
      QFile schemefile(":style/smooth/light-orange.qss");
      schemefile.open(QFile::ReadOnly | QFile::Text);
      QTextStream schemeFileStream(&schemefile);
      ui->groupBox->setStyleSheet(schemeFileStream.readAll());
    } break;
    default:
      // should not reach
      qDebug() << "color scheme index out of range";
      break;
  }
}
