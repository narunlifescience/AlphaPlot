#include "TableColorSettings.h"

#include <QColorDialog>
#include <QSettings>

#include "core/IconLoader.h"
#include "globals.h"
#include "ui_TableColorSettings.h"

const int TableColorSettings::btn_size = 24;
const int TableColorSettings::lbl_line_width = 1;

TableColorSettings::TableColorSettings(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_TableColorSettings) {
  ui->setupUi(this);
  setWindowIcon(
      IconLoader::load("preferences-desktop-color", IconLoader::General));
  ui->defaultsPushButton->setIcon(
      IconLoader::load("edit-column-description", IconLoader::LightDark));
  ui->resetPushButton->setIcon(
      IconLoader::load("edit-undo", IconLoader::LightDark));
  ui->applyPushButton->setIcon(
      IconLoader::load("dialog-ok-apply", IconLoader::LightDark));
  setWindowTitle(tr("Colors"));
  setTitle(ui->titleLabel, windowTitle());
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
  ui->scrollArea->setVerticalScrollBarPolicy(
      Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  ui->customColorGroupBox->setCheckable(true);
  setupColorLabel(ui->columnxColorLabel, ui->columnxColorButton);
  setupColorLabel(ui->columnyColorLabel, ui->columnyColorButton);
  setupColorLabel(ui->columnzColorLabel, ui->columnzColorButton);
  setupColorLabel(ui->columnxerrColorLabel, ui->columnxerrColorButton);
  setupColorLabel(ui->columnyerrColorLabel, ui->columnyerrColorButton);
  setupColorLabel(ui->noneColorLabel, ui->noneColorButton);
  setupColorLabel(ui->backgroundColorLabel, ui->backgroundColorButton);
  setupColorLabel(ui->textColorLabel, ui->textColorButton);
  setupColorLabel(ui->labelColorLabel, ui->labelColorButton);
  connect(ui->applyPushButton, &QPushButton::clicked, this,
          &TableColorSettings::Save);
  connect(ui->resetPushButton, &QPushButton::clicked, this,
          &TableColorSettings::Load);
  connect(ui->defaultsPushButton, &QPushButton::clicked, this,
          &TableColorSettings::LoadDefault);
  connect(ui->columnxColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->columnxColorLabel); });
  connect(ui->columnyColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->columnyColorLabel); });
  connect(ui->columnzColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->columnzColorLabel); });
  connect(ui->columnxerrColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->columnxerrColorLabel); });
  connect(ui->columnyerrColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->columnyerrColorLabel); });
  connect(ui->noneColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->noneColorLabel); });
  connect(ui->backgroundColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->backgroundColorLabel); });
  connect(ui->textColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->textColorLabel); });
  connect(ui->labelColorButton, &QToolButton::clicked,
          [&]() { pickColor(ui->labelColorLabel); });
  Load();
}

TableColorSettings::~TableColorSettings() { delete ui; }

void TableColorSettings::Load() {
  loadQsettingsValues();
  ui->columnxColorLabel->setStyleSheet(setStyleSheetString(xColorCode_));
  ui->columnyColorLabel->setStyleSheet(setStyleSheetString(yColorCode_));
  ui->columnzColorLabel->setStyleSheet(setStyleSheetString(zColorCode_));
  ui->columnxerrColorLabel->setStyleSheet(setStyleSheetString(xErrColorCode_));
  ui->columnyerrColorLabel->setStyleSheet(setStyleSheetString(yErrColorCode_));
  ui->noneColorLabel->setStyleSheet(setStyleSheetString(noneColorCode_));
  ui->customColorGroupBox->setChecked(customColor_);
  ui->backgroundColorLabel->setStyleSheet(setStyleSheetString(bkgdColor_));
  ui->textColorLabel->setStyleSheet(setStyleSheetString(textColor_));
  ui->labelColorLabel->setStyleSheet(setStyleSheetString(labelColor_));
}

void TableColorSettings::LoadDefault() {
  ui->columnxColorLabel->setStyleSheet(
      setStyleSheetString(QColor(0, 172, 109, 100)));
  ui->columnyColorLabel->setStyleSheet(
      setStyleSheetString(QColor(204, 140, 91, 100)));
  ui->columnzColorLabel->setStyleSheet(
      setStyleSheetString(QColor(174, 129, 255, 100)));
  ui->columnxerrColorLabel->setStyleSheet(
      setStyleSheetString(QColor(255, 0, 0, 100)));
  ui->columnyerrColorLabel->setStyleSheet(
      setStyleSheetString(QColor(255, 0, 0, 100)));
  ui->noneColorLabel->setStyleSheet(
      setStyleSheetString(QColor(150, 150, 150, 100)));
  ui->customColorGroupBox->setChecked(false);
  ui->backgroundColorLabel->setStyleSheet(
      setStyleSheetString(qApp->palette().color(QPalette::Base)));
  ui->textColorLabel->setStyleSheet(
      setStyleSheetString(qApp->palette().color(QPalette::Text)));
  ui->labelColorLabel->setStyleSheet(
      setStyleSheetString(qApp->palette().color(QPalette::Text)));
}

void TableColorSettings::Save() {
  QSettings settings;
  settings.beginGroup("Tables");
  settings.beginGroup("ColumnColorIndicator");
  settings.setValue("xColorCode",
                    ui->columnxColorLabel->palette().window().color());
  settings.setValue("yColorCode",
                    ui->columnyColorLabel->palette().window().color());
  settings.setValue("zColorCode",
                    ui->columnzColorLabel->palette().window().color());
  settings.setValue("xErrColorCode",
                    ui->columnxerrColorLabel->palette().window().color());
  settings.setValue("yErrColorCode",
                    ui->columnyerrColorLabel->palette().window().color());
  settings.setValue("noneColorCode",
                    ui->noneColorLabel->palette().window().color());
  settings.endGroup();  // ColumnColorIndicator
  settings.beginGroup("Colors");
  settings.setValue("Custom", ui->customColorGroupBox->isChecked());
  settings.setValue("Background",
                    ui->backgroundColorLabel->palette().window().color());
  settings.setValue("Text", ui->textColorLabel->palette().window().color());
  settings.setValue("Header", ui->labelColorLabel->palette().window().color());
  settings.endGroup();  // Colors
  settings.endGroup();

  emit tablecolorsettingsupdate();
}

bool TableColorSettings::settingsChangeCheck() {
  loadQsettingsValues();
  bool result = true;
  if (xColorCode_ != ui->columnxColorLabel->palette().window().color() ||
      yColorCode_ != ui->columnyColorLabel->palette().window().color() ||
      zColorCode_ != ui->columnzColorLabel->palette().window().color() ||
      xErrColorCode_ != ui->columnxerrColorLabel->palette().window().color() ||
      yErrColorCode_ != ui->columnyerrColorLabel->palette().window().color() ||
      noneColorCode_ != ui->noneColorLabel->palette().window().color() ||
      customColor_ != ui->customColorGroupBox->isChecked() ||
      bkgdColor_ != ui->backgroundColorLabel->palette().window().color() ||
      textColor_ != ui->textColorLabel->palette().window().color() ||
      labelColor_ != ui->labelColorLabel->palette().window().color()) {
    result = settingsChanged();
  }
  return result;
}

void TableColorSettings::setupColorLabel(QLabel *label, QToolButton *button) {
  label->setFixedSize(btn_size, btn_size);
  (label->height() > label->width()) ? label->setFixedWidth(label->height())
                                     : label->setFixedHeight(label->width());
  button->setIcon(IconLoader::load("color-management", IconLoader::General));
  button->setStyleSheet("QToolButton {border: 0px;}");
}

void TableColorSettings::pickColor(QLabel *label) {
  QPalette pal = label->palette();
  QColor color =
      QColorDialog::getColor(pal.window().color(), this, tr("Colors"),
                             QColorDialog::ColorDialogOption::ShowAlphaChannel);
  if (!color.isValid() || color == pal.window().color()) return;
  label->setStyleSheet(setStyleSheetString(color));
}

void TableColorSettings::loadQsettingsValues() {
  QSettings settings;
  settings.beginGroup("Tables");
  settings.beginGroup("ColumnColorIndicator");
  xColorCode_ =
      settings.value("xColorCode", QColor(0, 172, 109, 100)).value<QColor>();
  yColorCode_ =
      settings.value("yColorCode", QColor(204, 140, 91, 100)).value<QColor>();
  zColorCode_ =
      settings.value("zColorCode", QColor(174, 129, 255, 100)).value<QColor>();
  xErrColorCode_ =
      settings.value("xErrColorCode", QColor(255, 0, 0, 100)).value<QColor>();
  yErrColorCode_ =
      settings.value("yErrColorCode", QColor(255, 0, 0, 100)).value<QColor>();
  noneColorCode_ = settings.value("noneColorCode", QColor(150, 150, 150, 100))
                       .value<QColor>();
  settings.endGroup();
  settings.beginGroup("Colors");
  customColor_ = settings.value("Custom", false).toBool();
  bkgdColor_ =
      settings.value("Background", qApp->palette().color(QPalette::Base))
          .value<QColor>();
  textColor_ = settings.value("Text", qApp->palette().color(QPalette::Text))
                   .value<QColor>();
  labelColor_ = settings.value("Header", qApp->palette().color(QPalette::Text))
                    .value<QColor>();
  settings.endGroup();  // Colors
  settings.endGroup();
}

QString TableColorSettings::setStyleSheetString(const QColor &color) {
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
