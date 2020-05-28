#include "GeneralApplicationSettings.h"
#include "ui_GeneralApplicationSettings.h"

ApplicationSettingsPage::ApplicationSettingsPage(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_ApplicationSettingsPage)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/data/document-open-remote.png"));
    setWindowTitle(tr("Application"));
    ui->scrollArea->setFrameShape(QFrame::NoFrame);
}

ApplicationSettingsPage::~ApplicationSettingsPage()
{
    delete ui;
}

void ApplicationSettingsPage::Load() { }

void ApplicationSettingsPage::Save() { }

void ApplicationSettingsPage::setTitle(QString title)
{
    ui->label->setText(title);
}
