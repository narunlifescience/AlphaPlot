#include "TableFontSettings.h"

#include <QFontDialog>
#include <QSettings>

#include "core/IconLoader.h"
#include "ui_TableFontSettings.h"

TableFontSettings::TableFontSettings(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_TableFontSettings) {
  ui->setupUi(this);
  setWindowIcon(
      IconLoader::load("preferences-table-font", IconLoader::General));
  ui->defaultsPushButton->setIcon(
      IconLoader::load("edit-column-description", IconLoader::LightDark));
  ui->resetPushButton->setIcon(
      IconLoader::load("edit-undo", IconLoader::LightDark));
  ui->applyPushButton->setIcon(
      IconLoader::load("dialog-ok-apply", IconLoader::LightDark));
  setWindowTitle(tr("Fonts"));
  setTitle(ui->titleLabel, windowTitle());
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
  ui->scrollArea->setVerticalScrollBarPolicy(
      Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  connect(ui->applyPushButton, &QPushButton::clicked, this,
          &TableFontSettings::Save);
  connect(ui->resetPushButton, &QPushButton::clicked, this,
          &TableFontSettings::Load);
  connect(ui->defaultsPushButton, &QPushButton::clicked, this,
          &TableFontSettings::LoadDefault);
  connect(ui->textfontToolButton, &QToolButton::clicked, this,
          &TableFontSettings::pickTextFont);
  connect(ui->labelfontToolButton, &QToolButton::clicked, this,
          &TableFontSettings::pickLabelFont);
  Load();
}

TableFontSettings::~TableFontSettings() { delete ui; }

void TableFontSettings::Load() {
  loadQsettingsValues();
  ui->labelfontvalueLabel->setFont(labelfont_);
  ui->labelfontvalueLabel->setText(QString("%1 %2")
                                  .arg(labelfont_.family())
                                  .arg(labelfont_.pointSize()));
  ui->textfontvalueLabel->setFont(textfont_);
  ui->textfontvalueLabel->setText(QString("%1 %2")
                                  .arg(textfont_.family())
                                  .arg(textfont_.pointSize()));
}

void TableFontSettings::LoadDefault() {
  ui->labelfontvalueLabel->setFont(qApp->font());
  ui->labelfontvalueLabel->setText(QString("%1 %2")
                                  .arg(labelfont_.family())
                                  .arg(labelfont_.pointSize()));
  ui->textfontvalueLabel->setFont(qApp->font());
  ui->textfontvalueLabel->setText(QString("%1 %2")
                                  .arg(textfont_.family())
                                  .arg(textfont_.pointSize()));
}

void TableFontSettings::Save() {
  QSettings settings;
  settings.beginGroup("Tables");
  QStringList tableFonts;
  tableFonts << textfont_.family();
  tableFonts << QString::number(textfont_.pointSize());
  tableFonts << QString::number(textfont_.weight());
  tableFonts << QString::number(textfont_.italic());
  tableFonts << labelfont_.family();
  tableFonts << QString::number(labelfont_.pointSize());
  tableFonts << QString::number(labelfont_.weight());
  tableFonts << QString::number(labelfont_.italic());
  settings.setValue("Fonts", tableFonts);
  settings.endGroup();

  emit tablefontsettingsupdate();
}

bool TableFontSettings::settingsChangeCheck() {
  loadQsettingsValues();
  bool result = true;
  if (
      // these fonts donot match for some unknown reason so check each values
      textfont_.family() != ui->textfontvalueLabel->font().family() ||
      textfont_.pointSize() != ui->textfontvalueLabel->font().pointSize() ||
      textfont_.weight() != ui->textfontvalueLabel->font().weight() ||
      textfont_.italic() != ui->textfontvalueLabel->font().italic() ||
      labelfont_.family() != ui->labelfontvalueLabel->font().family() ||
      labelfont_.pointSize() != ui->labelfontvalueLabel->font().pointSize() ||
      labelfont_.weight() != ui->labelfontvalueLabel->font().weight() ||
      labelfont_.italic() != ui->labelfontvalueLabel->font().italic()) {
    result = settingsChanged();
  }
  return result;
}

void TableFontSettings::loadQsettingsValues() {
  QSettings settings;
  settings.beginGroup("Tables");
  QStringList tableFonts = settings.value("Fonts").toStringList();
  if (tableFonts.size() == 8) {
    textfont_ = QFont(tableFonts[0], tableFonts[1].toInt(),
                      tableFonts[2].toInt(), tableFonts[3].toInt());
    labelfont_ = QFont(tableFonts[4], tableFonts[5].toInt(),
                       tableFonts[6].toInt(), tableFonts[7].toInt());
  } else {
    textfont_ = qApp->font();
    labelfont_ = qApp->font();
  }
  settings.endGroup();
}

void TableFontSettings::pickTextFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, textfont_, this);
  if (ok)
    textfont_ = font;
  else
    return;
  ui->textfontvalueLabel->setFont(textfont_);
  ui->textfontvalueLabel->setText(
      QString("%1 %2").arg(textfont_.family()).arg(textfont_.pointSize()));
}

void TableFontSettings::pickLabelFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, labelfont_, this);
  if (ok)
    labelfont_ = font;
  else
    return;
  ui->labelfontvalueLabel->setFont(labelfont_);
  ui->labelfontvalueLabel->setText(
      QString("%1 %2").arg(labelfont_.family()).arg(labelfont_.pointSize()));
}
