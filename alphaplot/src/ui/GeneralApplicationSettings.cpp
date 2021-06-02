#include "GeneralApplicationSettings.h"

#include <QSettings>

#include "../core/IconLoader.h"
#include "ui_GeneralApplicationSettings.h"

ApplicationSettingsPage::ApplicationSettingsPage(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_ApplicationSettingsPage) {
  ui->setupUi(this);
  setWindowIcon(IconLoader::load("preferences-general", IconLoader::General));
  setWindowTitle(tr("Basic"));
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
  ui->scrollArea->setVerticalScrollBarPolicy(
      Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  ui->activeWindowGroupBox->setCheckable(true);
  ui->activeWindowGroupBox->setAlignment(Qt::AlignLeft);
}

ApplicationSettingsPage::~ApplicationSettingsPage() { delete ui; }

void ApplicationSettingsPage::Load() {}

void ApplicationSettingsPage::Save() {}

void ApplicationSettingsPage::setTitle(QString title) {
  QFont font = ui->titleLabel->font();
  font.setItalic(true);

  ui->titleLabel->setStyleSheet(
      "QLabel {padding-left: 5px;"
      " padding-right: 5px;"
      " padding-top: 5px;"
      " padding-bottom: 5px }");
  ui->titleLabel->setFont(font);
  ui->titleLabel->setText(title);
}
