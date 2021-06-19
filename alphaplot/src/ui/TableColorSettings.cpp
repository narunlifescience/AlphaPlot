#include "TableColorSettings.h"

#include <QColorDialog>
#include <QMessageBox>
#include <QSettings>

#include "core/IconLoader.h"
#include "globals.h"
#include "ui_TableColorSettings.h"
#include "widgets/ColorLabel.h"

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
  setupColorButton(ui->columnxColorButton);
  setupColorButton(ui->columnyColorButton);
  setupColorButton(ui->columnzColorButton);
  setupColorButton(ui->columnxerrColorButton);
  setupColorButton(ui->columnyerrColorButton);
  setupColorButton(ui->noneColorButton);
  setupColorButton(ui->backgroundColorButton);
  setupColorButton(ui->textColorButton);
  setupColorButton(ui->labelColorButton);
  connect(ui->customColorGroupBox, &QGroupBox::toggled, this,
          [=](const bool status) {
            if (status) {
              QMessageBox::StandardButton reply;
              reply = QMessageBox::warning(
                  this, tr("Table custom color setting"),
                  tr("This feature is highly experimental and can result in "
                     "unintended table colors. If you wish to proceed with the "
                     "same and apply the settings, make sure that you close "
                     "alphaplot and reopen again to reflect the changes.") +
                      "\n\n" + tr("Do you wish to continue ?"),
              QMessageBox::Yes | QMessageBox::No);
              (reply == QMessageBox::Yes)
                  ? ui->customColorGroupBox->setChecked(true)
                  : ui->customColorGroupBox->setChecked(false);
            }
          });
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
  ui->columnxColorLabel->setColor(xColorCode_);
  ui->columnyColorLabel->setColor(yColorCode_);
  ui->columnzColorLabel->setColor(zColorCode_);
  ui->columnxerrColorLabel->setColor(xErrColorCode_);
  ui->columnyerrColorLabel->setColor(yErrColorCode_);
  ui->noneColorLabel->setColor(noneColorCode_);
  ui->customColorGroupBox->setChecked(customColor_);
  ui->backgroundColorLabel->setColor(bkgdColor_);
  ui->textColorLabel->setColor(textColor_);
  ui->labelColorLabel->setColor(labelColor_);
}

void TableColorSettings::LoadDefault() {
  ui->columnxColorLabel->setColor(QColor(0, 172, 109, 100));
  ui->columnyColorLabel->setColor(QColor(204, 140, 91, 100));
  ui->columnzColorLabel->setColor(QColor(174, 129, 255, 100));
  ui->columnxerrColorLabel->setColor(QColor(255, 0, 0, 100));
  ui->columnyerrColorLabel->setColor(QColor(255, 0, 0, 100));
  ui->noneColorLabel->setColor(QColor(150, 150, 150, 100));
  ui->customColorGroupBox->setChecked(false);
  ui->backgroundColorLabel->setColor(qApp->palette().color(QPalette::Base));
  ui->textColorLabel->setColor(qApp->palette().color(QPalette::Text));
  ui->labelColorLabel->setColor(qApp->palette().color(QPalette::Text));
}

void TableColorSettings::Save() {
  QSettings settings;
  settings.beginGroup("Tables");
  settings.beginGroup("ColumnColorIndicator");
  settings.setValue("xColorCode", ui->columnxColorLabel->getColor());
  settings.setValue("yColorCode", ui->columnyColorLabel->getColor());
  settings.setValue("zColorCode", ui->columnzColorLabel->getColor());
  settings.setValue("xErrColorCode", ui->columnxerrColorLabel->getColor());
  settings.setValue("yErrColorCode", ui->columnyerrColorLabel->getColor());
  settings.setValue("noneColorCode", ui->noneColorLabel->getColor());
  settings.endGroup();  // ColumnColorIndicator
  settings.beginGroup("Colors");
  settings.setValue("Custom", ui->customColorGroupBox->isChecked());
  settings.setValue("Background", ui->backgroundColorLabel->getColor());
  settings.setValue("Text", ui->textColorLabel->getColor());
  settings.setValue("Header", ui->labelColorLabel->getColor());
  settings.endGroup();  // Colors
  settings.endGroup();

  emit tablecolorsettingsupdate();
}

bool TableColorSettings::settingsChangeCheck() {
  loadQsettingsValues();
  bool result = true;
  if (xColorCode_ != ui->columnxColorLabel->getColor() ||
      yColorCode_ != ui->columnyColorLabel->getColor() ||
      zColorCode_ != ui->columnzColorLabel->getColor() ||
      xErrColorCode_ != ui->columnxerrColorLabel->getColor() ||
      yErrColorCode_ != ui->columnyerrColorLabel->getColor() ||
      noneColorCode_ != ui->noneColorLabel->getColor() ||
      customColor_ != ui->customColorGroupBox->isChecked() ||
      bkgdColor_ != ui->backgroundColorLabel->getColor() ||
      textColor_ != ui->textColorLabel->getColor() ||
      labelColor_ != ui->labelColorLabel->getColor()) {
    result = settingsChanged();
  }
  return result;
}

void TableColorSettings::setupColorButton(QToolButton *button) {
  button->setIcon(IconLoader::load("color-management", IconLoader::General));
  button->setStyleSheet("QToolButton {border: 0px;}");
}

void TableColorSettings::pickColor(ColorLabel *label) {
  QColor color =
      QColorDialog::getColor(label->getColor(), this, tr("Colors"),
                             QColorDialog::ColorDialogOption::ShowAlphaChannel);
  if (!color.isValid() || color == label->getColor()) return;
  label->setColor(color);
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
