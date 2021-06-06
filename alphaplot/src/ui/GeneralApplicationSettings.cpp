#include "GeneralApplicationSettings.h"

#include <QColorDialog>
#include <QDir>
#include <QFontDialog>
#include <QSettings>
#include <QTranslator>

#include "../core/IconLoader.h"
#include "globals.h"
#include "scripting/Script.h"
#include "scripting/ScriptingEnv.h"
#include "ui_GeneralApplicationSettings.h"

const int ApplicationSettingsPage::btn_size = 24;

ApplicationSettingsPage::ApplicationSettingsPage(SettingsDialog *dialog)
    : SettingsPage(dialog), ui(new Ui_ApplicationSettingsPage) {
  ui->setupUi(this);
  setWindowIcon(IconLoader::load("preferences-general", IconLoader::General));
  ui->defaultsPushButton->setIcon(
      IconLoader::load("edit-column-description", IconLoader::LightDark));
  ui->resetPushButton->setIcon(
      IconLoader::load("edit-undo", IconLoader::LightDark));
  ui->applyPushButton->setIcon(
      IconLoader::load("dialog-ok-apply", IconLoader::LightDark));
  setWindowTitle(tr("Basic"));
  setTitle(ui->titleLabel, windowTitle());
  ui->scrollArea->setFrameShape(QFrame::NoFrame);
  ui->scrollArea->setVerticalScrollBarPolicy(
      Qt::ScrollBarPolicy::ScrollBarAsNeeded);
  ui->glowIndicatorGroupBox->setCheckable(true);
  ui->glowIndicatorGroupBox->setAlignment(Qt::AlignLeft);
  // seup glow setting stylesheet
  ui->glowColorLabel->setFixedSize(btn_size, btn_size);
  (ui->glowColorLabel->height() > ui->glowColorLabel->width())
      ? ui->glowColorLabel->setFixedWidth(ui->glowColorLabel->height())
      : ui->glowColorLabel->setFixedHeight(ui->glowColorLabel->width());
  ui->glowColorButton->setIcon(
      IconLoader::load("color-management", IconLoader::General));
  ui->glowColorButton->setStyleSheet("QToolButton {border: 0px;}");
  ui->scriptingComboBox->addItems(ScriptingLangManager::languages());
  ui->versionCheckBox->hide();
#ifdef SEARCH_FOR_UPDATES
  ui->versionCheckBox->show();
#endif
  connect(ui->applyPushButton, &QPushButton::clicked, this,
          &ApplicationSettingsPage::Save);
  connect(ui->resetPushButton, &QPushButton::clicked, this,
          &ApplicationSettingsPage::Load);
  connect(ui->defaultsPushButton, &QPushButton::clicked, this,
          &ApplicationSettingsPage::LoadDefault);
  connect(ui->fontToolButton, &QToolButton::clicked, this,
          &ApplicationSettingsPage::pickApplicationFont);
  connect(ui->glowColorButton, &QToolButton::clicked, this,
          &ApplicationSettingsPage::pickColor);

  insertLanguagesList();
  Load();
}

ApplicationSettingsPage::~ApplicationSettingsPage() { delete ui; }

void ApplicationSettingsPage::Load() {
  loadQsettingsValues();

  ui->fontvalueLabel->setFont(applicationfont_);
  ui->fontvalueLabel->setText(QString("%1 %2")
                                  .arg(applicationfont_.family())
                                  .arg(applicationfont_.pointSize()));
  ui->glowIndicatorGroupBox->setChecked(glowstatus_);
  ui->glowThicknessSpinBox->setValue(glowradius_);
  ui->glowColorLabel->setStyleSheet(setStyleSheetString(glowcolor_));
  // ToDo: language
  ui->scriptingComboBox->setCurrentIndex(
      ScriptingLangManager::languages().indexOf(defaultscriptinglang_));
  ui->saveCheckBox->setChecked(autosave_);
  ui->saveSpinBox->setValue(autosavetime_);
  ui->undoSpinBox->setValue(undolimit_);
#ifdef SEARCH_FOR_UPDATES
  ui->versionCheckBox->setChecked(autosearchupdates_);
#endif
}

void ApplicationSettingsPage::LoadDefault() {
  ui->fontvalueLabel->setFont(QFont());
  applicationfont_ = QFont();
  ui->fontvalueLabel->setText(QString("%1 %2")
                                  .arg(applicationfont_.family())
                                  .arg(applicationfont_.pointSize()));
  ui->glowIndicatorGroupBox->setChecked(false);
  ui->glowThicknessSpinBox->setValue(8);
  ui->glowColorLabel->setStyleSheet(setStyleSheetString(Qt::red));
  ui->scriptingComboBox->setCurrentIndex(
      ScriptingLangManager::languages().indexOf("muParser"));
  ui->saveCheckBox->setChecked(true);
  ui->saveSpinBox->setValue(15);
  ui->undoSpinBox->setValue(10);
#ifdef SEARCH_FOR_UPDATES
  ui->versionCheckBox->setChecked(false);
#endif
}

void ApplicationSettingsPage::Save() {
  QSettings settings;
  settings.beginGroup("General");
  settings.beginGroup("GlowIndicator");
  settings.setValue("Show", ui->glowIndicatorGroupBox->isChecked());
  settings.setValue("Color", ui->glowColorLabel->palette().window().color());
  settings.setValue("Radius", ui->glowThicknessSpinBox->value());
  settings.endGroup();
  QStringList applicationFont;
  applicationFont << ui->fontvalueLabel->font().family();
  applicationFont << QString::number(ui->fontvalueLabel->font().pointSize());
  applicationFont << QString::number(ui->fontvalueLabel->font().weight());
  applicationFont << QString::number(ui->fontvalueLabel->font().italic());
  settings.setValue("Font", applicationFont);
  settings.setValue("Language", ui->scriptingComboBox->currentText());
  settings.setValue("AutoSave", ui->saveCheckBox->isChecked());
  settings.setValue("AutoSaveTime", ui->saveSpinBox->value());
  settings.setValue("UndoLimit", ui->undoSpinBox->value());
  settings.setValue("ScriptingLang", defaultscriptinglang_);
#ifdef SEARCH_FOR_UPDATES
  settings.setValue("AutoSearchUpdates", ui->versionCheckBox->isChecked());
#endif
  settings.endGroup();

  emit generalapplicationsettingsupdate();
}

bool ApplicationSettingsPage::settingsChangeCheck() {
  loadQsettingsValues();
  bool result = true;
  if (glowstatus_ != ui->glowIndicatorGroupBox->isChecked() ||
      glowcolor_ != ui->glowColorLabel->palette().window().color() ||
      glowradius_ != ui->glowThicknessSpinBox->value() ||
      applicationfont_ != ui->fontvalueLabel->font() ||
      defaultscriptinglang_.toLower() !=
          ui->scriptingComboBox->currentText().toLower() ||
      autosave_ != ui->saveCheckBox->isChecked() ||
      autosavetime_ != ui->saveSpinBox->value() ||
      undolimit_ != ui->undoSpinBox->value() ||
      autosearchupdates_ != ui->versionCheckBox->isChecked()) {
    result = settingsChanged();
  }
  return result;
}

void ApplicationSettingsPage::loadQsettingsValues() {
  QSettings settings;
  settings.beginGroup("General");
  settings.beginGroup("GlowIndicator");
  glowstatus_ = settings.value("Show", false).toBool();
  glowcolor_ = settings.value("Color", "red").value<QColor>();
  glowradius_ = settings.value("Radius", 8).toDouble();
  settings.endGroup();
  applanguage_ =
      settings.value("Language", QLocale::system().name().section('_', 0, 0))
          .toString();
  defaultscriptinglang_ =
      settings.value("ScriptingLang", "muParser").toString();
  autosave_ = settings.value("AutoSave", true).toBool();
  autosavetime_ = settings.value("AutoSaveTime", 15).toInt();
  undolimit_ = settings.value("UndoLimit", 10).toInt();
  QStringList applicationFont = settings.value("Font").toStringList();
  if (applicationFont.size() == 4)
    applicationfont_ =
        QFont(applicationFont.at(0), applicationFont.at(1).toInt(),
              applicationFont.at(2).toInt(), applicationFont.at(3).toInt());
#ifdef SEARCH_FOR_UPDATES
  autosearchupdates_ = settings.value("AutoSearchUpdates", false).toBool();
#endif
  settings.endGroup();
}

void ApplicationSettingsPage::pickColor() {
  QPalette pal = ui->glowColorLabel->palette();
  QColor color = QColorDialog::getColor(pal.window().color(), this);
  if (!color.isValid() || color == pal.window().color()) return;
  ui->glowColorLabel->setStyleSheet(setStyleSheetString(color));
}

void ApplicationSettingsPage::pickApplicationFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, applicationfont_, this);
  if (ok)
    applicationfont_ = font;
  else
    return;
  ui->fontvalueLabel->setFont(applicationfont_);
  ui->fontvalueLabel->setText(QString("%1 %2")
                                  .arg(applicationfont_.family())
                                  .arg(applicationfont_.pointSize()));
}

QString ApplicationSettingsPage::setStyleSheetString(const QColor &color) {
  QString stylesheetstring =
      "QLabel:!disabled{background : rgba(%1,%2, %3, %4); border: 1px solid "
      "rgba(%5, %6, %7, %8);}";
  QColor bordercolor = qApp->palette().windowText().color();
  return stylesheetstring.arg(color.red())
      .arg(color.green())
      .arg(color.blue())
      .arg(color.alpha())
      .arg(bordercolor.red())
      .arg(bordercolor.green())
      .arg(bordercolor.blue())
      .arg(bordercolor.alpha());
}

void ApplicationSettingsPage::insertLanguagesList() {
  ui->languageComboBox->clear();
  QDir dir(TS_PATH);
  QStringList locales = AlphaPlot::getLocales();
  QStringList languages;
  int lang = 0;
  QSettings settings;
  for (int i = 0; i < locales.size(); i++) {
    if (locales.at(i) == "en")
      languages.push_back("English");
    else {
      QTranslator translator;
      translator.load("alphaplot_" + locales.at(i), TS_PATH);

      QString language = translator.translate("ApplicationWindow", "English",
                                              "translate this to the language "
                                              "of the translation file, NOT to "
                                              "the meaning of English!");
      if (!language.isEmpty())
        languages.push_back(language);
      else
        languages.push_back(locales.at(i));
    }

    if (locales[i] ==
        settings.value("Language", QLocale::system().name().section('_', 0, 0))
            .toString())
      lang = i;
  }
  ui->languageComboBox->addItems(languages);
  ui->languageComboBox->setCurrentIndex(lang);
}
