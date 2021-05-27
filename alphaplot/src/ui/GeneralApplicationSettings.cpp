#include "GeneralApplicationSettings.h"

#include "../core/IconLoader.h"
#include "ui_GeneralApplicationSettings.h"
#include <QSettings>

ApplicationSettingsPage::ApplicationSettingsPage(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_ApplicationSettingsPage) {
  ui->setupUi(this);
  setWindowIcon(
      IconLoader::load("preferences-general", IconLoader::General));
  setWindowTitle(tr("Basic"));
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
}

ApplicationSettingsPage::~ApplicationSettingsPage() { delete ui; }

void ApplicationSettingsPage::Load() {}

void ApplicationSettingsPage::Save() {}

void ApplicationSettingsPage::setTitle(QString title) {
  ui->label->setText(title);
}
