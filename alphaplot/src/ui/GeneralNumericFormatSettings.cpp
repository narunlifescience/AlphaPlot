#include "GeneralNumericFormatSettings.h"

#include <QDebug>
#include <QSettings>

#include "../core/IconLoader.h"
#include "ui_GeneralNumericFormatSettings.h"

GeneralNumericFormatSettings::GeneralNumericFormatSettings(
    SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_GeneralNumericFormatSettings) {
  ui->setupUi(this);
  setWindowIcon(IconLoader::load("preferences-general-numericformat",
                                 IconLoader::General));
  ui->defaultsPushButton->setIcon(
      IconLoader::load("edit-column-description", IconLoader::LightDark));
  ui->resetPushButton->setIcon(
      IconLoader::load("edit-undo", IconLoader::LightDark));
  ui->applyPushButton->setIcon(
      IconLoader::load("dialog-ok-apply", IconLoader::LightDark));
  setWindowTitle(tr("Numeric Format"));
  setTitle(ui->titleLabel, windowTitle());
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
  ui->scrollArea->setVerticalScrollBarPolicy(
      Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  ui->groupSeparatorLineEdit->setReadOnly(true);
  ui->defaultNumberSpinBox->setRange(0, 16);
  // decimal separator
  ui->decimalSeparatorComboBox->clear();
  ui->decimalSeparatorComboBox->addItem(
      tr("default") + " (" + QLocale::system().toString(1000.0, 'f', 1) + ")");
  ui->decimalSeparatorComboBox->addItem(QLocale::c().toString(1000.0, 'f', 1));
  ui->decimalSeparatorComboBox->addItem(
      QLocale(QLocale::German).toString(1000.0, 'f', 1));
  ui->decimalSeparatorComboBox->addItem(
      QLocale(QLocale::French).toString(1000.0, 'f', 1));
  // decimal separator
  ui->numberFormatComboBox->clear();
  ui->numberFormatComboBox->addItem(tr("Decimal (f)"), QVariant('f'));
  ui->numberFormatComboBox->addItem(tr("Scientific (e)"), QVariant('e'));
  ui->numberFormatComboBox->addItem(tr("Scientific (E)"), QVariant('E'));
  ui->numberFormatComboBox->addItem(tr("Automatic (e)"), QVariant('g'));
  ui->numberFormatComboBox->addItem(tr("Automatic (E)"), QVariant('G'));
  // preview
  ui->groupSeparatorLineEdit->setText(
      tr("Preview:", "preview of the decimal separator") + " " +
      QLocale().toString(1000.1234567890123456, 'f',
                         ui->defaultNumberSpinBox->value()));
  connect(ui->applyPushButton, &QPushButton::clicked, this,
          &GeneralNumericFormatSettings::Save);
  connect(ui->resetPushButton, &QPushButton::clicked, this,
          &GeneralNumericFormatSettings::Load);
  connect(ui->defaultsPushButton, &QPushButton::clicked, this,
          &GeneralNumericFormatSettings::LoadDefault);
  connect(
      ui->decimalSeparatorComboBox,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &GeneralNumericFormatSettings::updatePreview);
  connect(
      ui->numberFormatComboBox,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &GeneralNumericFormatSettings::updatePreview);
  connect(ui->defaultNumberSpinBox,
          static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this,
          &GeneralNumericFormatSettings::updatePreview);
  connect(ui->groupSeparatorCheckBox,
          static_cast<void (QCheckBox::*)(bool)>(&QCheckBox::toggled), this,
          &GeneralNumericFormatSettings::updatePreview);
  Load();
}

GeneralNumericFormatSettings::~GeneralNumericFormatSettings() { delete ui; }

void GeneralNumericFormatSettings::Load() {
  loadQsettingsValues();

  ui->defaultNumberSpinBox->setValue(precision_);
  int index = ui->numberFormatComboBox->findData(defaultnumericformat_);
  ui->numberFormatComboBox->setCurrentIndex(index);
  if (localestring_ == QLocale::system().name()) {
    ui->decimalSeparatorComboBox->setCurrentIndex(0);
  } else if (localestring_ == QLocale::c().name())
    ui->decimalSeparatorComboBox->setCurrentIndex(1);
  else if (localestring_ == QLocale(QLocale::German).name())
    ui->decimalSeparatorComboBox->setCurrentIndex(2);
  else if (localestring_ == QLocale(QLocale::French).name())
    ui->decimalSeparatorComboBox->setCurrentIndex(3);
  ui->groupSeparatorCheckBox->setChecked(usegroupseperator_);
}

void GeneralNumericFormatSettings::LoadDefault() {
  ui->defaultNumberSpinBox->setValue(6);
  int index = ui->numberFormatComboBox->findData('g');
  ui->numberFormatComboBox->setCurrentIndex(index);
  ui->decimalSeparatorComboBox->setCurrentIndex(0);
  ui->groupSeparatorCheckBox->setChecked(true);
}

void GeneralNumericFormatSettings::Save() {
  QLocale locale;
  switch (ui->decimalSeparatorComboBox->currentIndex()) {
    case 0:
      locale = QLocale::system();
      break;
    case 1:
      locale = QLocale::c();
      break;
    case 2:
      locale = QLocale(QLocale::German);
      break;
    case 3:
      locale = QLocale(QLocale::French);
      break;
    default:
      locale = QLocale::system();
      qDebug() << "decimal separator index out of range";
  }
  QSettings settings;
  settings.beginGroup("General");
  settings.setValue("Locale", locale.name());
  settings.setValue("LocaleUseGroupSeparator",
                    ui->groupSeparatorCheckBox->isChecked());
  settings.setValue("DecimalDigits", ui->defaultNumberSpinBox->value());
  settings.setValue(
      "DefaultNumericFormat",
      ui->numberFormatComboBox->currentData().toChar().toLatin1());
  settings.endGroup();

  emit generalnumericformatsettingsupdate();
}

bool GeneralNumericFormatSettings::settingsChangeCheck() {
  loadQsettingsValues();
  bool result = true;
  int formatindex = ui->numberFormatComboBox->findData(defaultnumericformat_);
  int localeindex = 0;
  (QLocale::system().name() == localestring_)          ? localeindex = 0
  : (QLocale::c().name() == localestring_)             ? localeindex = 1
  : (QLocale(QLocale::German).name() == localestring_) ? localeindex = 2
  : (QLocale(QLocale::French).name() == localestring_) ? localeindex = 2
                                                       : localeindex = 0;
  if (localeindex != ui->decimalSeparatorComboBox->currentIndex() ||
      usegroupseperator_ != ui->groupSeparatorCheckBox->isChecked() ||
      precision_ != ui->defaultNumberSpinBox->value() ||
      formatindex != ui->numberFormatComboBox->currentIndex()) {
    result = settingsChanged();
  }
  return result;
}

void GeneralNumericFormatSettings::updatePreview() {
  QLocale locale;
  switch (ui->decimalSeparatorComboBox->currentIndex()) {
    case 0:
      locale = QLocale::system();
      break;
    case 1:
      locale = QLocale::c();
      break;
    case 2:
      locale = QLocale(QLocale::German);
      break;
    case 3:
      locale = QLocale(QLocale::French);
      break;
    default:
      locale = QLocale::system();
      qDebug() << "decimal separator index out of range";
  }

  (ui->groupSeparatorCheckBox->isChecked())
      ? locale.setNumberOptions(locale.numberOptions() &
                                ~QLocale::OmitGroupSeparator)
      : locale.setNumberOptions(locale.numberOptions() |
                                QLocale::OmitGroupSeparator);
  ui->groupSeparatorLineEdit->setText(
      tr("Preview:", "preview of the decimal separator") + " " +
      locale.toString(1000.1234567890123456, 'f',
                      ui->defaultNumberSpinBox->value()));
}

void GeneralNumericFormatSettings::loadQsettingsValues() {
  QSettings settings;
  settings.beginGroup("General");
  localestring_ = settings.value("Locale", QLocale::system().name()).toString();
  usegroupseperator_ = settings.value("LocaleUseGroupSeparator", true).toBool();
  precision_ = settings.value("DecimalDigits", 6).toInt();
  defaultnumericformat_ =
      settings.value("DefaultNumericFormat", 'g').toChar().toLatin1();
  settings.endGroup();
}
