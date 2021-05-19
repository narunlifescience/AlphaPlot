#include "GeneralConfirmationSettings.h"

#include "../core/IconLoader.h"
#include "ui_GeneralConfirmationSettings.h"

GeneralConfirmationSettings::GeneralConfirmationSettings(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_GeneralConfirmationSettings) {
  ui->setupUi(this);
  setWindowIcon(
      IconLoader::load("preferences-general-confirmation", IconLoader::General));
  setWindowTitle(tr("Confirmation"));
}

GeneralConfirmationSettings::~GeneralConfirmationSettings() { delete ui; }

void GeneralConfirmationSettings::Load() {}

void GeneralConfirmationSettings::Save() {}

void GeneralConfirmationSettings::setTitle(QString title) {
  QFont font = ui->label->font();

  font.setPointSize(font.pointSize() + 2);
  //font.setBold(true);
  font.setItalic(true);

  ui->label->setStyleSheet(
      "QLabel {padding-left: 5px;"
      " padding-right: 5px;"
      " padding-top: 5px;"
      " padding-bottom: 5px }");
  ui->label->setFont(font);
  ui->label->setText(title);
}
