#include "TableBasicSettings.h"

#include <QDebug>
#include <QSettings>

#include "core/IconLoader.h"
#include "ui_TableBasicSettings.h"

TableBasicSettings::TableBasicSettings(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_TableBasicSettings) {
  ui->setupUi(this);
  setWindowIcon(IconLoader::load("table-properties", IconLoader::General));
  ui->defaultsPushButton->setIcon(
      IconLoader::load("edit-column-description", IconLoader::LightDark));
  ui->resetPushButton->setIcon(
      IconLoader::load("edit-undo", IconLoader::LightDark));
  ui->applyPushButton->setIcon(
      IconLoader::load("dialog-ok-apply", IconLoader::LightDark));
  setWindowTitle(tr("Preference"));
  setTitle(ui->titleLabel, windowTitle());
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
  ui->scrollArea->setVerticalScrollBarPolicy(
      Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  ui->rowHeightSpinBox->setRange(3, 200);
  ui->columnSeparatorComboBox->clear();
  ui->columnSeparatorComboBox->addItem(tr("TAB"));
  ui->columnSeparatorComboBox->addItem(tr("SPACE"));
  ui->columnSeparatorComboBox->addItem(";" + tr("TAB"));
  ui->columnSeparatorComboBox->addItem("," + tr("TAB"));
  ui->columnSeparatorComboBox->addItem(";" + tr("SPACE"));
  ui->columnSeparatorComboBox->addItem("," + tr("SPACE"));
  ui->columnSeparatorComboBox->addItem(";");
  ui->columnSeparatorComboBox->addItem(",");
  connect(ui->applyPushButton, &QPushButton::clicked, this,
          &TableBasicSettings::Save);
  connect(ui->resetPushButton, &QPushButton::clicked, this,
          &TableBasicSettings::Load);
  connect(ui->defaultsPushButton, &QPushButton::clicked, this,
          &TableBasicSettings::LoadDefault);
  Load();
}

TableBasicSettings::~TableBasicSettings() { delete ui; }

void TableBasicSettings::Load() {
  loadQsettingsValues();
  ui->commentsCheckBox->setChecked(show_table_comments_);
  setColumnSeparator(columnseparator_);
}

void TableBasicSettings::LoadDefault() {
  ui->commentsCheckBox->setChecked(false);
  setColumnSeparator("\t");
}

void TableBasicSettings::Save() {
  QSettings settings;
  settings.beginGroup("Tables");
  settings.setValue("DisplayComments", ui->commentsCheckBox->isChecked());
  settings.endGroup();
  settings.beginGroup("ImportASCII");
  settings.setValue("ColumnSeparator",
                    ui->columnSeparatorComboBox->currentText()
                        .replace(tr("TAB"), "\t")
                        .replace(tr("SPACE"), " "));
  settings.endGroup();

  emit tablebasicsettingsupdate();
}

bool TableBasicSettings::settingsChangeCheck() {
  loadQsettingsValues();
  bool result = true;
  if (show_table_comments_ != ui->commentsCheckBox->isChecked() ||
      columnseparator_ != ui->columnSeparatorComboBox->currentText()
                              .replace(tr("TAB"), "\t")
                              .replace(tr("SPACE"), " ")) {
    result = settingsChanged();
  }
  return result;
}

void TableBasicSettings::loadQsettingsValues() {
  QSettings settings;
  settings.beginGroup("Tables");
  show_table_comments_ = settings.value("DisplayComments", false).toBool();
  settings.endGroup();
  settings.beginGroup("ImportASCII");
  columnseparator_ = settings.value("ColumnSeparator", "\\t").toString();
  columnseparator_.replace(tr("TAB"), "\t").replace(tr("SPACE"), " ");
  settings.endGroup();
}

void TableBasicSettings::setColumnSeparator(const QString &sep) {
  if (sep == "\t")
    ui->columnSeparatorComboBox->setCurrentIndex(0);
  else if (sep == " ")
    ui->columnSeparatorComboBox->setCurrentIndex(1);
  else if (sep == ";\t")
    ui->columnSeparatorComboBox->setCurrentIndex(2);
  else if (sep == ",\t")
    ui->columnSeparatorComboBox->setCurrentIndex(3);
  else if (sep == "; ")
    ui->columnSeparatorComboBox->setCurrentIndex(4);
  else if (sep == ", ")
    ui->columnSeparatorComboBox->setCurrentIndex(5);
  else if (sep == ";")
    ui->columnSeparatorComboBox->setCurrentIndex(6);
  else if (sep == ",")
    ui->columnSeparatorComboBox->setCurrentIndex(7);
  else {
    QString separator = sep;
    ui->columnSeparatorComboBox->setEditText(
        separator.replace(" ", "\\s").replace("\t", "\\t"));
  }
}
