#include "FittingSettings.h"

#include <QFrame>

#include "../core/IconLoader.h"
#include "ui_FittingSettings.h"

FittingSettings::FittingSettings(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_FittingSettings) {
  ui->setupUi(this);
  setWindowIcon(IconLoader::load("preferences-fitting", IconLoader::General));
  ui->defaultsPushButton->setIcon(
      IconLoader::load("edit-column-description", IconLoader::LightDark));
  ui->resetPushButton->setIcon(
      IconLoader::load("edit-undo", IconLoader::LightDark));
  ui->applyPushButton->setIcon(
      IconLoader::load("dialog-ok-apply", IconLoader::LightDark));
  setWindowTitle(tr("Params"));
  setTitle(ui->titleLabel, windowTitle());
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
  ui->scrollArea->setVerticalScrollBarPolicy(
      Qt::ScrollBarPolicy::ScrollBarAsNeeded);
}

FittingSettings::~FittingSettings() { delete ui; }

void FittingSettings::Load() {}

void FittingSettings::LoadDefault() {}

void FittingSettings::Save() {}

bool FittingSettings::settingsChangeCheck() { return true; }
