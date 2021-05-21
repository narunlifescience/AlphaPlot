#include "GeneralConfirmationSettings.h"

#include <QDebug>
#include <QMessageBox>
#include <QSettings>

#include "../core/IconLoader.h"
#include "ui_GeneralConfirmationSettings.h"

GeneralConfirmationSettings::GeneralConfirmationSettings(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_GeneralConfirmationSettings) {
  ui->setupUi(this);
  setWindowIcon(IconLoader::load("preferences-general-confirmation",
                                 IconLoader::General));
  setWindowTitle(tr("Confirmation"));
  connect(ui->applyPushButton, &QPushButton::clicked, this,
          &GeneralConfirmationSettings::Save);
  connect(ui->defaultsPushButton, &QPushButton::clicked, this,
          &GeneralConfirmationSettings::Load);
  Load();
}

GeneralConfirmationSettings::~GeneralConfirmationSettings() { delete ui; }

void GeneralConfirmationSettings::Load() {
  loadQsettingsValues();

  ui->foldersCheckBox->setChecked(confirmCloseFolder_);
  ui->tablesCheckBox->setChecked(confirmCloseTable_);
  ui->matricesCheckBox->setChecked(confirmCloseMatrix_);
  ui->plot2dCheckBox->setChecked(confirmClosePlot2D_);
  ui->plot3dCheckBox->setChecked(confirmClosePlot3D_);
  ui->notesCheckBox->setChecked(confirmCloseNotes_);
}

void GeneralConfirmationSettings::Save() {
  QSettings settings;
  settings.beginGroup("Confirmations");
  settings.setValue("Folder", ui->foldersCheckBox->isChecked());
  settings.setValue("Table", ui->tablesCheckBox->isChecked());
  settings.setValue("Matrix", ui->matricesCheckBox->isChecked());
  settings.setValue("Plot2D", ui->plot2dCheckBox->isChecked());
  settings.setValue("Plot3D", ui->plot3dCheckBox->isChecked());
  settings.setValue("Note", ui->notesCheckBox->isChecked());
  settings.endGroup();  // Confirmations

  emit generalconfirmationsettingsupdate();
}

void GeneralConfirmationSettings::setTitle(QString title) {
  QFont font = ui->label->font();

  font.setPointSize(font.pointSize() + 2);
  // font.setBold(true);
  font.setItalic(true);

  ui->label->setStyleSheet(
      "QLabel {padding-left: 5px;"
      " padding-right: 5px;"
      " padding-top: 5px;"
      " padding-bottom: 5px }");
  ui->label->setFont(font);
  ui->label->setText(title);
}

bool GeneralConfirmationSettings::settingsChangeCheck() {
  loadQsettingsValues();
  bool result = true;
  if (confirmCloseFolder_ != ui->foldersCheckBox->isChecked() ||
      confirmCloseTable_ != ui->tablesCheckBox->isChecked() ||
      confirmCloseMatrix_ != ui->matricesCheckBox->isChecked() ||
      confirmClosePlot2D_ != ui->plot2dCheckBox->isChecked() ||
      confirmClosePlot3D_ != ui->plot3dCheckBox->isChecked() ||
      confirmCloseNotes_ != ui->notesCheckBox->isChecked()) {
    result = settingsChanged();
  }
  return result;
}

void GeneralConfirmationSettings::loadQsettingsValues() {
  QSettings settings;
  settings.beginGroup("Confirmations");
  confirmCloseFolder_ = settings.value("Folder", true).toBool();
  confirmCloseTable_ = settings.value("Table", true).toBool();
  confirmCloseMatrix_ = settings.value("Matrix", true).toBool();
  confirmClosePlot2D_ = settings.value("Plot2D", true).toBool();
  confirmClosePlot3D_ = settings.value("Plot3D", true).toBool();
  confirmCloseNotes_ = settings.value("Note", true).toBool();
  settings.endGroup();  // Confirmations
}
