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
  ui->defaultsPushButton->setIcon(
      IconLoader::load("edit-column-description", IconLoader::LightDark));
  ui->resetPushButton->setIcon(
      IconLoader::load("edit-undo", IconLoader::LightDark));
  ui->applyPushButton->setIcon(
      IconLoader::load("dialog-ok-apply", IconLoader::LightDark));
  setWindowTitle(tr("Confirmation"));
  setTitle(ui->titleLabel, windowTitle());
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
  ui->scrollArea->setVerticalScrollBarPolicy(
      Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  connect(ui->applyPushButton, &QPushButton::clicked, this,
          &GeneralConfirmationSettings::Save);
  connect(ui->resetPushButton, &QPushButton::clicked, this,
          &GeneralConfirmationSettings::Load);
  connect(ui->defaultsPushButton, &QPushButton::clicked, this,
          &GeneralConfirmationSettings::LoadDefault);
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

void GeneralConfirmationSettings::LoadDefault() {
  ui->foldersCheckBox->setChecked(true);
  ui->tablesCheckBox->setChecked(true);
  ui->matricesCheckBox->setChecked(true);
  ui->plot2dCheckBox->setChecked(true);
  ui->plot3dCheckBox->setChecked(true);
  ui->notesCheckBox->setChecked(true);
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
