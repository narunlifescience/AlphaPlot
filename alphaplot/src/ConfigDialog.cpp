/***************************************************************************
    File                 : ConfigDialog.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Preferences dialog

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#include "ConfigDialog.h"
#include "ApplicationWindow.h"
#include "ColorBox.h"
#include "ColorButton.h"
#include "Matrix.h"
#include "core/IconLoader.h"

#include <QApplication>
#include <QColorDialog>
#include <QComboBox>
#include <QDir>
#include <QFont>
#include <QFontDialog>
#include <QFontMetrics>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QLocale>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QRadioButton>
#include <QRegExp>
#include <QSpinBox>
#include <QStackedWidget>
#include <QStyleFactory>
#include <QTabWidget>
#include <QTranslator>
#include <QVBoxLayout>
#include <QWidget>

ConfigDialog::ConfigDialog(QWidget *parent, Qt::WindowFlags fl)
    : QDialog(parent, fl), app_(qobject_cast<ApplicationWindow *>(parent)) {
  Q_ASSERT(app_);
  setWindowIcon(IconLoader::load("edit-preference", IconLoader::LightDark));
  setModal(true);
  // get current values from app window
  plot3DColors = app_->plot3DColors;
  plot3DTitleFont = app_->plot3DTitleFont;
  plot3DNumbersFont = app_->plot3DNumbersFont;
  plot3DAxesFont = app_->plot3DAxesFont;
  textFont = app_->tableTextFont;
  headerFont = app_->tableHeaderFont;
  appFont = app_->appFont;
  axesFont = app_->plotAxesFont;
  numbersFont = app_->plotNumbersFont;
  legendFont = app_->plotLegendFont;
  titleFont = app_->plotTitleFont;

  // create the GUI
  generalDialog = new QStackedWidget();
  itemsList = new QListWidget();
  itemsList->setSpacing(10);

  initAppPage();
  initTablesPage();
  initPlotsPage();
  initPlots3DPage();
  initFittingPage();

  generalDialog->addWidget(appTabWidget);
  generalDialog->addWidget(tables);
  generalDialog->addWidget(plotsTabWidget);
  generalDialog->addWidget(plots3D);
  generalDialog->addWidget(fitPage);

  QVBoxLayout *rightLayout = new QVBoxLayout();
  lblPageHeader = new QLabel();
  QFont fnt = this->font();
  fnt.setPointSize(fnt.pointSize() + 3);
  lblPageHeader->setFont(fnt);

  rightLayout->setSpacing(10);
  rightLayout->addWidget(lblPageHeader);
  rightLayout->addWidget(generalDialog);

  QHBoxLayout *topLayout = new QHBoxLayout();
  topLayout->setSpacing(5);
  topLayout->setMargin(5);
  topLayout->addWidget(itemsList);
  topLayout->addLayout(rightLayout);

  QHBoxLayout *bottomButtons = new QHBoxLayout();
  bottomButtons->addStretch();
  buttonApply = new QPushButton();
  buttonApply->setAutoDefault(true);
  bottomButtons->addWidget(buttonApply);

  buttonOk = new QPushButton();
  buttonOk->setAutoDefault(true);
  buttonOk->setDefault(true);
  bottomButtons->addWidget(buttonOk);

  buttonCancel = new QPushButton();
  buttonCancel->setAutoDefault(true);
  bottomButtons->addWidget(buttonCancel);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addLayout(topLayout);
  mainLayout->addLayout(bottomButtons);

  languageChange();

  // signals and slots connections
  connect(itemsList, SIGNAL(currentRowChanged(int)), this,
          SLOT(setCurrentPage(int)));
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
  connect(buttonApply, SIGNAL(clicked()), this, SLOT(apply()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
  connect(buttonBackground, SIGNAL(clicked()), this, SLOT(pickBgColor()));
  connect(buttonText, SIGNAL(clicked()), this, SLOT(pickTextColor()));
  connect(buttonHeader, SIGNAL(clicked()), this, SLOT(pickHeaderColor()));
  connect(buttonTextFont, SIGNAL(clicked()), this, SLOT(pickTextFont()));
  connect(buttonHeaderFont, SIGNAL(clicked()), this, SLOT(pickHeaderFont()));

  setCurrentPage(0);
}

void ConfigDialog::setCurrentPage(int index) {
  generalDialog->setCurrentIndex(index);
  if (itemsList->currentItem())
    lblPageHeader->setText(itemsList->currentItem()->text());
}

void ConfigDialog::initTablesPage() {
  tables = new QWidget();

  QHBoxLayout *topLayout = new QHBoxLayout();
  topLayout->setSpacing(5);

  lblSeparator = new QLabel();
  topLayout->addWidget(lblSeparator);
  boxSeparator = new QComboBox();
  boxSeparator->setEditable(true);
  topLayout->addWidget(boxSeparator);

  QString help =
      tr("The column separator can be customized. \nThe following special "
         "codes can be used:\n\\t for a TAB character \n\\s for a SPACE");
  help +=
      "\n" +
      tr("The separator must not contain the following characters: \n0-9eE.+-");

  boxSeparator->setWhatsThis(help);
  boxSeparator->setToolTip(help);
  lblSeparator->setWhatsThis(help);
  lblSeparator->setToolTip(help);

  groupBoxTableCol = new QGroupBox();
  QGridLayout *colorsLayout = new QGridLayout(groupBoxTableCol);

  lblTableBackground = new QLabel();
  colorsLayout->addWidget(lblTableBackground, 0, 0);
  buttonBackground = new ColorButton();
  buttonBackground->setColor(app_->tableBkgdColor);
  colorsLayout->addWidget(buttonBackground, 0, 1);

  lblTextColor = new QLabel();
  colorsLayout->addWidget(lblTextColor, 1, 0);
  buttonText = new ColorButton();
  buttonText->setColor(app_->tableTextColor);
  colorsLayout->addWidget(buttonText, 1, 1);

  lblHeaderColor = new QLabel();
  colorsLayout->addWidget(lblHeaderColor, 2, 0);
  buttonHeader = new ColorButton();
  buttonHeader->setColor(app_->tableHeaderColor);
  colorsLayout->addWidget(buttonHeader, 2, 1);

  groupBoxTableFonts = new QGroupBox();
  QHBoxLayout *bottomLayout = new QHBoxLayout(groupBoxTableFonts);

  buttonTextFont = new QPushButton();
  bottomLayout->addWidget(buttonTextFont);
  buttonHeaderFont = new QPushButton();
  bottomLayout->addWidget(buttonHeaderFont);

  boxTableComments = new QCheckBox();
  boxTableComments->setChecked(app_->d_show_table_comments);

  QVBoxLayout *tablesPageLayout = new QVBoxLayout(tables);
  tablesPageLayout->addWidget(boxTableComments);
  tablesPageLayout->addLayout(topLayout, 1);
  tablesPageLayout->addWidget(groupBoxTableCol);
  tablesPageLayout->addWidget(groupBoxTableFonts);
  tablesPageLayout->addStretch();
}

void ConfigDialog::initPlotsPage() {
  plotsTabWidget = new QTabWidget();
  plotOptions = new QWidget();

  QVBoxLayout *optionsTabLayout = new QVBoxLayout(plotOptions);
  optionsTabLayout->setSpacing(5);

  QGroupBox *groupBoxOptions = new QGroupBox();
  optionsTabLayout->addWidget(groupBoxOptions);

  QGridLayout *optionsLayout = new QGridLayout(groupBoxOptions);

  boxAutoscaling = new QCheckBox();
  boxAutoscaling->setChecked(app_->autoscale2DPlots);
  optionsLayout->addWidget(boxAutoscaling, 0, 0);

  boxScaleFonts = new QCheckBox();
  boxScaleFonts->setChecked(app_->autoScaleFonts);
  optionsLayout->addWidget(boxScaleFonts, 0, 1);

  boxTitle = new QCheckBox();
  boxTitle->setChecked(app_->titleOn);
  optionsLayout->addWidget(boxTitle, 1, 0);

  boxAllAxes = new QCheckBox();
  boxAllAxes->setChecked(app_->allAxesOn);
  optionsLayout->addWidget(boxAllAxes, 1, 1);

  boxAntialiasing = new QCheckBox();
  boxAntialiasing->setChecked(app_->antialiasing2DPlots);
  optionsLayout->addWidget(boxAntialiasing, 2, 0);

  boxBackbones = new QCheckBox();
  boxBackbones->setChecked(app_->drawBackbones);
  optionsLayout->addWidget(boxBackbones, 2, 1);

  boxFrame = new QCheckBox();
  boxFrame->setChecked(app_->canvasFrameOn);
  optionsLayout->addWidget(boxFrame, 3, 0);

  labelFrameWidth = new QLabel();
  optionsLayout->addWidget(labelFrameWidth, 4, 0);
  boxFrameWidth = new QSpinBox();
  optionsLayout->addWidget(boxFrameWidth, 4, 1);
  boxFrameWidth->setRange(1, 100);
  boxFrameWidth->setValue(app_->canvasFrameWidth);
  if (!app_->canvasFrameOn) {
    labelFrameWidth->hide();
    boxFrameWidth->hide();
  }

  lblAxesLineWidth = new QLabel();
  optionsLayout->addWidget(lblAxesLineWidth, 5, 0);
  boxLineWidth = new QSpinBox();
  boxLineWidth->setRange(0, 100);
  boxLineWidth->setValue(app_->axesLineWidth);
  optionsLayout->addWidget(boxLineWidth, 5, 1);

  lblMargin = new QLabel();
  optionsLayout->addWidget(lblMargin, 6, 0);
  boxMargin = new QSpinBox();
  boxMargin->setRange(0, 1000);
  boxMargin->setSingleStep(5);
  boxMargin->setValue(app_->defaultPlotMargin);
  optionsLayout->addWidget(boxMargin, 6, 1);

  optionsLayout->setRowStretch(7, 1);

  boxResize = new QCheckBox();
  boxResize->setChecked(!app_->autoResizeLayers);
  if (boxResize->isChecked()) boxScaleFonts->setEnabled(false);

  optionsTabLayout->addWidget(boxResize);

  plotsTabWidget->addTab(plotOptions, QString());

  initCurvesPage();

  plotsTabWidget->addTab(curves, QString());

  plotTicks = new QWidget();
  QVBoxLayout *plotTicksLayout = new QVBoxLayout(plotTicks);

  QGroupBox *ticksGroupBox = new QGroupBox();
  QGridLayout *ticksLayout = new QGridLayout(ticksGroupBox);
  plotTicksLayout->addWidget(ticksGroupBox);

  lblMajTicks = new QLabel();
  ticksLayout->addWidget(lblMajTicks, 0, 0);
  boxMajTicks = new QComboBox();
  ticksLayout->addWidget(boxMajTicks, 0, 1);

  lblMajTicksLength = new QLabel();
  ticksLayout->addWidget(lblMajTicksLength, 0, 2);
  boxMajTicksLength = new QSpinBox();
  boxMajTicksLength->setRange(0, 100);
  boxMajTicksLength->setValue(app_->majTicksLength);
  ticksLayout->addWidget(boxMajTicksLength, 0, 3);

  lblMinTicks = new QLabel();
  ticksLayout->addWidget(lblMinTicks, 1, 0);
  boxMinTicks = new QComboBox();
  ticksLayout->addWidget(boxMinTicks, 1, 1);

  lblMinTicksLength = new QLabel();
  ticksLayout->addWidget(lblMinTicksLength, 1, 2);
  boxMinTicksLength = new QSpinBox();
  boxMinTicksLength->setRange(0, 100);
  boxMinTicksLength->setValue(app_->minTicksLength);
  ticksLayout->addWidget(boxMinTicksLength, 1, 3);

  ticksLayout->setRowStretch(4, 1);

  plotsTabWidget->addTab(plotTicks, QString());

  plotFonts = new QWidget();
  QVBoxLayout *plotFontsLayout = new QVBoxLayout(plotFonts);

  QGroupBox *groupBox2DFonts = new QGroupBox();
  plotFontsLayout->addWidget(groupBox2DFonts);
  QVBoxLayout *fontsLayout = new QVBoxLayout(groupBox2DFonts);
  buttonTitleFont = new QPushButton();
  fontsLayout->addWidget(buttonTitleFont);
  buttonLegendFont = new QPushButton();
  fontsLayout->addWidget(buttonLegendFont);
  buttonAxesFont = new QPushButton();
  fontsLayout->addWidget(buttonAxesFont);
  buttonNumbersFont = new QPushButton();
  fontsLayout->addWidget(buttonNumbersFont);
  fontsLayout->addStretch();

  plotsTabWidget->addTab(plotFonts, QString());

  plotPrint = new QWidget();
  QVBoxLayout *printLayout = new QVBoxLayout(plotPrint);

  boxScaleLayersOnPrint = new QCheckBox();
  boxScaleLayersOnPrint->setChecked(app_->d_scale_plots_on_print);
  printLayout->addWidget(boxScaleLayersOnPrint);

  boxPrintCropmarks = new QCheckBox();
  boxPrintCropmarks->setChecked(app_->d_print_cropmarks);
  printLayout->addWidget(boxPrintCropmarks);
  printLayout->addStretch();
  plotsTabWidget->addTab(plotPrint, QString());

  connect(boxResize, SIGNAL(clicked()), this, SLOT(enableScaleFonts()));
  connect(boxFrame, SIGNAL(toggled(bool)), this, SLOT(showFrameWidth(bool)));
  connect(buttonAxesFont, SIGNAL(clicked()), this, SLOT(pickAxesFont()));
  connect(buttonNumbersFont, SIGNAL(clicked()), this, SLOT(pickNumbersFont()));
  connect(buttonLegendFont, SIGNAL(clicked()), this, SLOT(pickLegendFont()));
  connect(buttonTitleFont, SIGNAL(clicked()), this, SLOT(pickTitleFont()));
}

void ConfigDialog::enableScaleFonts() {
  if (boxResize->isChecked())
    boxScaleFonts->setEnabled(false);
  else
    boxScaleFonts->setEnabled(true);
}

void ConfigDialog::showFrameWidth(bool ok) {
  if (!ok) {
    boxFrameWidth->hide();
    labelFrameWidth->hide();
  } else {
    boxFrameWidth->show();
    labelFrameWidth->show();
  }
}

void ConfigDialog::initPlots3DPage() {
  plots3D = new QWidget();

  QGroupBox *topBox = new QGroupBox();
  QGridLayout *topLayout = new QGridLayout(topBox);
  topLayout->setSpacing(5);

  lblResolution = new QLabel();
  topLayout->addWidget(lblResolution, 0, 0);
  boxResolution = new QSpinBox();
  boxResolution->setRange(1, 100);
  boxResolution->setValue(app_->plot3DResolution);
  topLayout->addWidget(boxResolution, 0, 1);

  boxShowLegend = new QCheckBox();
  boxShowLegend->setChecked(app_->showPlot3DLegend);
  topLayout->addWidget(boxShowLegend, 1, 0);

  boxShowProjection = new QCheckBox();
  boxShowProjection->setChecked(app_->showPlot3DProjection);
  topLayout->addWidget(boxShowProjection, 1, 1);

  boxSmoothMesh = new QCheckBox();
  boxSmoothMesh->setChecked(app_->smooth3DMesh);
  topLayout->addWidget(boxSmoothMesh, 2, 0);

  boxOrthogonal = new QCheckBox();
  boxOrthogonal->setChecked(app_->orthogonal3DPlots);
  topLayout->addWidget(boxOrthogonal, 2, 1);

  boxAutoscale3DPlots = new QCheckBox();
  boxAutoscale3DPlots->setChecked(app_->autoscale3DPlots);
  topLayout->addWidget(boxAutoscale3DPlots, 3, 0);

  groupBox3DCol = new QGroupBox();
  QGridLayout *middleLayout = new QGridLayout(groupBox3DCol);

  btnFromColor = new QPushButton();
  middleLayout->addWidget(btnFromColor, 0, 0);
  btnLabels = new QPushButton();
  middleLayout->addWidget(btnLabels, 0, 1);
  btnMesh = new QPushButton();
  middleLayout->addWidget(btnMesh, 0, 2);
  btnGrid = new QPushButton();
  middleLayout->addWidget(btnGrid, 0, 3);
  btnToColor = new QPushButton();
  middleLayout->addWidget(btnToColor, 1, 0);
  btnNumbers = new QPushButton();
  middleLayout->addWidget(btnNumbers, 1, 1);
  btnAxes = new QPushButton();
  middleLayout->addWidget(btnAxes, 1, 2);
  btnBackground3D = new QPushButton();
  middleLayout->addWidget(btnBackground3D, 1, 3);

  groupBox3DFonts = new QGroupBox();
  QHBoxLayout *bottomLayout = new QHBoxLayout(groupBox3DFonts);

  btnTitleFnt = new QPushButton();
  bottomLayout->addWidget(btnTitleFnt);
  btnLabelsFnt = new QPushButton();
  bottomLayout->addWidget(btnLabelsFnt);
  btnNumFnt = new QPushButton();
  bottomLayout->addWidget(btnNumFnt);

  QVBoxLayout *plots3DPageLayout = new QVBoxLayout(plots3D);
  plots3DPageLayout->addWidget(topBox);
  plots3DPageLayout->addWidget(groupBox3DCol);
  plots3DPageLayout->addWidget(groupBox3DFonts);
  plots3DPageLayout->addStretch();

  connect(btnAxes, SIGNAL(clicked()), this, SLOT(pick3DAxesColor()));
  connect(btnLabels, SIGNAL(clicked()), this, SLOT(pick3DLabelsColor()));
  connect(btnNumbers, SIGNAL(clicked()), this, SLOT(pick3DNumbersColor()));
  connect(btnBackground3D, SIGNAL(clicked()), this,
          SLOT(pick3DBackgroundColor()));
  connect(btnFromColor, SIGNAL(clicked()), this, SLOT(pickDataMinColor()));
  connect(btnToColor, SIGNAL(clicked()), this, SLOT(pickDataMaxColor()));
  connect(btnGrid, SIGNAL(clicked()), this, SLOT(pickGridColor()));
  connect(btnMesh, SIGNAL(clicked()), this, SLOT(pickMeshColor()));

  connect(btnNumFnt, SIGNAL(clicked()), this, SLOT(pick3DNumbersFont()));
  connect(btnTitleFnt, SIGNAL(clicked()), this, SLOT(pick3DTitleFont()));
  connect(btnLabelsFnt, SIGNAL(clicked()), this, SLOT(pick3DAxesFont()));
}

void ConfigDialog::initAppPage() {
  appTabWidget = new QTabWidget(generalDialog);

  application = new QWidget();
  QVBoxLayout *applicationLayout = new QVBoxLayout(application);
  QGroupBox *groupBoxApp = new QGroupBox();
  applicationLayout->addWidget(groupBoxApp);
  QGridLayout *topBoxLayout = new QGridLayout(groupBoxApp);

  lblLanguage = new QLabel();
  topBoxLayout->addWidget(lblLanguage, 0, 0);
  boxLanguage = new QComboBox();
  insertLanguagesList();
  //boxLanguage->addItem("English");
  topBoxLayout->addWidget(boxLanguage, 0, 1);

  lblStyle = new QLabel();
  topBoxLayout->addWidget(lblStyle, 1, 0);
  boxStyle = new QComboBox();
  topBoxLayout->addWidget(boxStyle, 1, 1);
  QStringList styles = QStyleFactory::keys();
  styles.sort();
  boxStyle->addItems(styles);
  boxStyle->setCurrentIndex(
      boxStyle->findText(app_->appStyle, Qt::MatchWildcard));

  lblColorScheme = new QLabel();
  lblColorScheme->setText("Color scheme");
  topBoxLayout->addWidget(lblColorScheme, 2, 0);
  boxColorScheme = new QComboBox();
  topBoxLayout->addWidget(boxColorScheme, 2, 1);
  QStringList colorSchemes;
  colorSchemes << "default"
               << "alpha dark"
               << "smooth dark blue"
               << "smooth dark green"
               << "smooth dark orange"
               << "smooth light blue"
               << "smooth light green"
               << "smooth light orange";
  boxColorScheme->addItems(colorSchemes);
  boxColorScheme->setCurrentIndex(app_->appColorScheme);

  lblFonts = new QLabel();
  topBoxLayout->addWidget(lblFonts, 3, 0);
  fontsBtn = new QPushButton();
  topBoxLayout->addWidget(fontsBtn, 3, 1);

  lblScriptingLanguage = new QLabel();
  topBoxLayout->addWidget(lblScriptingLanguage, 4, 0);
  boxScriptingLanguage = new QComboBox();
  QStringList llist = ScriptingLangManager::languages();
  boxScriptingLanguage->addItems(llist);
  boxScriptingLanguage->setCurrentIndex(
      llist.indexOf(app_->defaultScriptingLang));
  topBoxLayout->addWidget(boxScriptingLanguage, 4, 1);

  boxSave = new QCheckBox();
  boxSave->setChecked(app_->autoSave);
  topBoxLayout->addWidget(boxSave, 5, 0);

  boxMinutes = new QSpinBox();
  boxMinutes->setRange(1, 100);
  boxMinutes->setValue(app_->autoSaveTime);
  boxMinutes->setEnabled(app_->autoSave);
  topBoxLayout->addWidget(boxMinutes, 5, 1);

  lblUndoLimit = new QLabel();
  topBoxLayout->addWidget(lblUndoLimit, 6, 0);
  boxUndoLimit = new QSpinBox();
  boxUndoLimit->setRange(1, 10000);
  boxUndoLimit->setValue(app_->undoLimit);
  topBoxLayout->addWidget(boxUndoLimit, 6, 1);

#ifdef SEARCH_FOR_UPDATES
  boxSearchUpdates = new QCheckBox();
  boxSearchUpdates->setChecked(app_->autoSearchUpdates);
  topBoxLayout->addWidget(boxSearchUpdates, 7, 0, 1, 2);
#endif

  topBoxLayout->setRowStretch(8, 1);

  appTabWidget->addTab(application, QString());

  initConfirmationsPage();

  appTabWidget->addTab(confirm, QString());

  appColors = new QWidget();
  QVBoxLayout *appColorsLayout = new QVBoxLayout(appColors);
  QGroupBox *groupBoxAppCol = new QGroupBox();
  appColorsLayout->addWidget(groupBoxAppCol);
  QGridLayout *colorsBoxLayout = new QGridLayout(groupBoxAppCol);

  lblWorkspace = new QLabel();
  colorsBoxLayout->addWidget(lblWorkspace, 0, 0);
  btnWorkspace = new ColorButton();
  btnWorkspace->setColor(app_->workspaceColor);
  colorsBoxLayout->addWidget(btnWorkspace, 0, 1);

  lblPanels = new QLabel();
  colorsBoxLayout->addWidget(lblPanels, 1, 0);
  btnPanels = new ColorButton();
  colorsBoxLayout->addWidget(btnPanels, 1, 1);
  btnPanels->setColor(app_->panelsColor);

  lblPanelsText = new QLabel();
  colorsBoxLayout->addWidget(lblPanelsText, 2, 0);
  btnPanelsText = new ColorButton();
  colorsBoxLayout->addWidget(btnPanelsText, 2, 1);
  btnPanelsText->setColor(app_->panelsTextColor);

  colorsBoxLayout->setRowStretch(3, 1);

  appTabWidget->addTab(appColors, QString());

  numericFormatPage = new QWidget();
  QVBoxLayout *numLayout = new QVBoxLayout(numericFormatPage);
  QGroupBox *numericFormatBox = new QGroupBox();
  numLayout->addWidget(numericFormatBox);
  QGridLayout *numericFormatLayout = new QGridLayout(numericFormatBox);

  lblAppPrecision = new QLabel();
  numericFormatLayout->addWidget(lblAppPrecision, 0, 0);
  boxAppPrecision = new QSpinBox();
  boxAppPrecision->setRange(0, 16);
  boxAppPrecision->setValue(app_->d_decimal_digits);
  numericFormatLayout->addWidget(boxAppPrecision, 0, 1);

  lblDecimalSeparator = new QLabel();
  numericFormatLayout->addWidget(lblDecimalSeparator, 1, 0);
  boxDecimalSeparator = new QComboBox();

  numericFormatLayout->addWidget(boxDecimalSeparator, 1, 1);

  boxUseGroupSeparator = new QCheckBox();
  boxUseGroupSeparator->setChecked(
      !(QLocale().numberOptions() & QLocale::OmitGroupSeparator));

  numericFormatLayout->addWidget(boxUseGroupSeparator, 2, 0);

  boxSeparatorPreview = new QLabel();
  boxSeparatorPreview->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  numericFormatLayout->addWidget(boxSeparatorPreview, 2, 1);

  lblDefaultNumericFormat = new QLabel();
  boxDefaultNumericFormat = new QComboBox();

  numericFormatLayout->addWidget(lblDefaultNumericFormat, 3, 0);
  numericFormatLayout->addWidget(boxDefaultNumericFormat, 3, 1);

  numericFormatLayout->setRowStretch(4, 1);

  appTabWidget->addTab(numericFormatPage, QString());

  connect(boxLanguage, SIGNAL(activated(int)), this,
          SLOT(switchToLanguage(int)));
  connect(fontsBtn, SIGNAL(clicked()), this, SLOT(pickApplicationFont()));
  connect(boxSave, SIGNAL(toggled(bool)), boxMinutes, SLOT(setEnabled(bool)));
  connect(btnWorkspace, SIGNAL(clicked()), this, SLOT(pickWorkspaceColor()));
  connect(btnPanels, SIGNAL(clicked()), this, SLOT(pickPanelsColor()));
  connect(btnPanelsText, SIGNAL(clicked()), this, SLOT(pickPanelsTextColor()));
  connect(boxUseGroupSeparator, SIGNAL(toggled(bool)), this,
          SLOT(updateDecSepPreview()));
  connect(boxDecimalSeparator, SIGNAL(currentIndexChanged(int)), this,
          SLOT(updateDecSepPreview()));
  connect(boxAppPrecision, SIGNAL(valueChanged(int)), this,
          SLOT(updateDecSepPreview()));
}

void ConfigDialog::initFittingPage() {
  fitPage = new QWidget();

  groupBoxFittingCurve = new QGroupBox();
  QGridLayout *fittingCurveLayout = new QGridLayout(groupBoxFittingCurve);
  fittingCurveLayout->setSpacing(5);

  generatePointsBtn = new QRadioButton();
  generatePointsBtn->setChecked(app_->generateUniformFitPoints);
  fittingCurveLayout->addWidget(generatePointsBtn, 0, 0);

  lblPoints = new QLabel();
  fittingCurveLayout->addWidget(lblPoints, 0, 1);
  generatePointsBox = new QSpinBox();
  generatePointsBox->setRange(0, 1000000);
  generatePointsBox->setSingleStep(10);
  generatePointsBox->setValue(app_->fitPoints);
  fittingCurveLayout->addWidget(generatePointsBox, 0, 2);

  linearFit2PointsBox = new QCheckBox();
  linearFit2PointsBox->setChecked(app_->d_2_linear_fit_points);
  fittingCurveLayout->addWidget(linearFit2PointsBox, 0, 3);

  showPointsBox(!app_->generateUniformFitPoints);

  samePointsBtn = new QRadioButton();
  samePointsBtn->setChecked(!app_->generateUniformFitPoints);
  fittingCurveLayout->addWidget(samePointsBtn, 1, 0);

  groupBoxMultiPeak = new QGroupBox();
  groupBoxMultiPeak->setCheckable(true);
  groupBoxMultiPeak->setChecked(app_->generatePeakCurves);

  QHBoxLayout *multiPeakLayout = new QHBoxLayout(groupBoxMultiPeak);

  lblPeaksColor = new QLabel();
  multiPeakLayout->addWidget(lblPeaksColor);
  boxPeaksColor = new ColorBox(nullptr);
  boxPeaksColor->setCurrentIndex(app_->peakCurvesColor);
  multiPeakLayout->addWidget(boxPeaksColor);

  groupBoxFitParameters = new QGroupBox();
  QGridLayout *fitParamsLayout = new QGridLayout(groupBoxFitParameters);

  lblPrecision = new QLabel();
  fitParamsLayout->addWidget(lblPrecision, 0, 0);
  boxPrecision = new QSpinBox();
  fitParamsLayout->addWidget(boxPrecision, 0, 1);
  boxPrecision->setValue(app_->fit_output_precision);

  logBox = new QCheckBox();
  logBox->setChecked(app_->writeFitResultsToLog);
  fitParamsLayout->addWidget(logBox, 1, 0);

  plotLabelBox = new QCheckBox();
  plotLabelBox->setChecked(app_->pasteFitResultsToPlot);
  fitParamsLayout->addWidget(plotLabelBox, 2, 0);

  scaleErrorsBox = new QCheckBox();
  fitParamsLayout->addWidget(scaleErrorsBox, 3, 0);
  scaleErrorsBox->setChecked(app_->fit_scale_errors);

  QVBoxLayout *fitPageLayout = new QVBoxLayout(fitPage);
  fitPageLayout->addWidget(groupBoxFittingCurve);
  fitPageLayout->addWidget(groupBoxMultiPeak);
  fitPageLayout->addWidget(groupBoxFitParameters);
  fitPageLayout->addStretch();

  connect(samePointsBtn, SIGNAL(toggled(bool)), this,
          SLOT(showPointsBox(bool)));
  connect(generatePointsBtn, SIGNAL(toggled(bool)), this,
          SLOT(showPointsBox(bool)));
}

void ConfigDialog::initCurvesPage() {
  curves = new QWidget();

  QGroupBox *curvesGroupBox = new QGroupBox();
  QGridLayout *curvesBoxLayout = new QGridLayout(curvesGroupBox);

  lblCurveStyle = new QLabel();
  curvesBoxLayout->addWidget(lblCurveStyle, 0, 0);
  boxCurveStyle = new QComboBox();
  curvesBoxLayout->addWidget(boxCurveStyle, 0, 1);

  lblLineWidth = new QLabel();
  curvesBoxLayout->addWidget(lblLineWidth, 1, 0);
  boxCurveLineWidth = new QSpinBox();
  boxCurveLineWidth->setRange(1, 100);
  boxCurveLineWidth->setValue(app_->defaultCurveLineWidth);
  curvesBoxLayout->addWidget(boxCurveLineWidth, 1, 1);

  lblSymbSize = new QLabel();
  curvesBoxLayout->addWidget(lblSymbSize, 2, 0);
  boxSymbolSize = new QSpinBox();
  boxSymbolSize->setRange(1, 100);
  boxSymbolSize->setValue(app_->defaultSymbolSize / 2);
  curvesBoxLayout->addWidget(boxSymbolSize, 2, 1);

  curvesBoxLayout->setRowStretch(3, 1);

  QHBoxLayout *curvesPageLayout = new QHBoxLayout(curves);
  curvesPageLayout->addWidget(curvesGroupBox);
}

void ConfigDialog::initConfirmationsPage() {
  confirm = new QWidget();

  groupBoxConfirm = new QGroupBox();
  QVBoxLayout *layout = new QVBoxLayout(groupBoxConfirm);

  boxFolders = new QCheckBox();
  boxFolders->setChecked(app_->confirmCloseFolder);
  layout->addWidget(boxFolders);

  boxTables = new QCheckBox();
  boxTables->setChecked(app_->confirmCloseTable);
  layout->addWidget(boxTables);

  boxMatrices = new QCheckBox();
  boxMatrices->setChecked(app_->confirmCloseMatrix);
  layout->addWidget(boxMatrices);

  boxPlots2D = new QCheckBox();
  boxPlots2D->setChecked(app_->confirmClosePlot2D);
  layout->addWidget(boxPlots2D);

  boxPlots3D = new QCheckBox();
  boxPlots3D->setChecked(app_->confirmClosePlot3D);
  layout->addWidget(boxPlots3D);

  boxNotes = new QCheckBox();
  boxNotes->setChecked(app_->confirmCloseNotes);
  layout->addWidget(boxNotes);

  layout->addStretch();

  QHBoxLayout *confirmPageLayout = new QHBoxLayout(confirm);
  confirmPageLayout->addWidget(groupBoxConfirm);
}

void ConfigDialog::languageChange() {
  setWindowTitle(tr("Preferences"));
  // pages list
  itemsList->clear();
  itemsList->addItem(tr("General"));
  itemsList->addItem(tr("Tables"));
  itemsList->addItem(tr("2D Plots"));
  itemsList->addItem(tr("3D Plots"));
  itemsList->addItem(tr("Fitting"));
  itemsList->setCurrentRow(0);
  itemsList->item(0)->setIcon(QIcon(QPixmap(":/config_general.xpm")));
  itemsList->item(1)->setIcon(QIcon(QPixmap(":/config_table.xpm")));
  itemsList->item(2)->setIcon(QIcon(QPixmap(":/config_plot2d.xpm")));
  itemsList->item(3)->setIcon(QIcon(QPixmap(":/config_plot3d.xpm")));
  itemsList->item(4)->setIcon(QIcon(QPixmap(":/config_fit.xpm")));
  itemsList->setIconSize(QSize(32, 32));
  // calculate a sensible width for the items list
  // (default QListWidget size is 256 which looks too big)
  QFontMetrics fm(itemsList->font());
  int width = 32, i;
  for (i = 0; i < itemsList->count(); i++)
    if (fm.width(itemsList->item(i)->text()) > width)
      width = fm.width(itemsList->item(i)->text());
  itemsList->setMaximumWidth(itemsList->iconSize().width() + width + 50);
  // resize the list to the maximum width
  itemsList->resize(itemsList->maximumWidth(), itemsList->height());

  // plots 2D page
  plotsTabWidget->setTabText(plotsTabWidget->indexOf(plotOptions),
                             tr("Options"));
  plotsTabWidget->setTabText(plotsTabWidget->indexOf(curves), tr("Curves"));
  plotsTabWidget->setTabText(plotsTabWidget->indexOf(plotTicks), tr("Ticks"));
  plotsTabWidget->setTabText(plotsTabWidget->indexOf(plotFonts), tr("Fonts"));

  boxResize->setText(tr("Do not &resize layers when window size changes"));

  lblMinTicksLength->setText(tr("Length"));

  lblAxesLineWidth->setText(tr("Axes linewidth"));
  lblMajTicksLength->setText(tr("Length"));
  lblMajTicks->setText(tr("Major Ticks"));
  lblMinTicks->setText(tr("Minor Ticks"));

  lblMargin->setText(tr("Margin"));
  labelFrameWidth->setText(tr("Frame width"));
  boxBackbones->setText(tr("Axes &backbones"));
  boxFrame->setText(tr("Canvas Fra&me"));
  boxAllAxes->setText(tr("Sho&w all axes"));
  boxTitle->setText(tr("Show &Title"));
  boxScaleFonts->setText(tr("Scale &Fonts"));
  boxAutoscaling->setText(tr("Auto&scaling"));
  boxAntialiasing->setText(tr("Antia&liasing"));

  boxMajTicks->clear();
  boxMajTicks->addItem(tr("None"));
  boxMajTicks->addItem(tr("Out"));
  boxMajTicks->addItem(tr("In & Out"));
  boxMajTicks->addItem(tr("In"));

  boxMinTicks->clear();
  boxMinTicks->addItem(tr("None"));
  boxMinTicks->addItem(tr("Out"));
  boxMinTicks->addItem(tr("In & Out"));
  boxMinTicks->addItem(tr("In"));

  boxMajTicks->setCurrentIndex(app_->majTicksStyle);
  boxMinTicks->setCurrentIndex(app_->minTicksStyle);

  plotsTabWidget->setTabText(plotsTabWidget->indexOf(plotPrint), tr("Print"));
  boxPrintCropmarks->setText(tr("Print Crop &Marks"));
  boxScaleLayersOnPrint->setText(tr("&Scale layers to paper size"));

  // confirmations page
  groupBoxConfirm->setTitle(tr("Prompt on closing"));
  boxFolders->setText(tr("Folders"));
  boxTables->setText(tr("Tables"));
  boxPlots3D->setText(tr("3D Plots"));
  boxPlots2D->setText(tr("2D Plots"));
  boxMatrices->setText(tr("Matrices"));
  boxNotes->setText(tr("&Notes"));

  buttonOk->setText(tr("&OK"));
  buttonCancel->setText(tr("&Cancel"));
  buttonApply->setText(tr("&Apply"));
  buttonTextFont->setText(tr("&Text Font"));
  buttonHeaderFont->setText(tr("&Labels Font"));
  buttonAxesFont->setText(tr("A&xes Labels"));
  buttonNumbersFont->setText(tr("Axes &Numbers"));
  buttonLegendFont->setText(tr("&Legend"));
  buttonTitleFont->setText(tr("T&itle"));

  // application page
  appTabWidget->setTabText(appTabWidget->indexOf(application),
                           tr("Application"));
  appTabWidget->setTabText(appTabWidget->indexOf(confirm), tr("Confirmations"));
  appTabWidget->setTabText(appTabWidget->indexOf(appColors), tr("Colors"));
  appTabWidget->setTabText(appTabWidget->indexOf(numericFormatPage),
                           tr("Numeric Format"));

  lblLanguage->setText(tr("Language"));
  lblStyle->setText(tr("Style"));
  lblFonts->setText(tr("Main Font"));
  fontsBtn->setText(tr("Choose &font"));
  lblWorkspace->setText(tr("Workspace"));
  lblPanelsText->setText(tr("Panels text"));
  lblPanels->setText(tr("Panels"));
  lblUndoLimit->setText(tr("Undo/Redo History limit"));
  boxSave->setText(tr("Save every"));
#ifdef SEARCH_FOR_UPDATES
  boxSearchUpdates->setText(tr("Check for new versions at startup"));
#endif
  boxMinutes->setSuffix(tr(" minutes"));
  lblScriptingLanguage->setText(tr("Default scripting language"));

  lblDefaultNumericFormat->setText(tr("Default numeric format"));
  boxDefaultNumericFormat->clear();
  boxDefaultNumericFormat->addItem(tr("Decimal"), QVariant('f'));
  boxDefaultNumericFormat->addItem(tr("Scientific (e)"), QVariant('e'));
  boxDefaultNumericFormat->addItem(tr("Scientific (E)"), QVariant('E'));
  boxDefaultNumericFormat->addItem(tr("Automatic (e)"), QVariant('g'));
  boxDefaultNumericFormat->addItem(tr("Automatic (E)"), QVariant('G'));
  int format_index =
      boxDefaultNumericFormat->findData(app_->d_default_numeric_format);
  boxDefaultNumericFormat->setCurrentIndex(format_index);

  boxUseGroupSeparator->setText(
      tr("Use group separator", "option: use separator every 3 digits"));
  lblAppPrecision->setText(tr("Default Number of Decimal Digits"));
  lblDecimalSeparator->setText(tr("Decimal Separators"));
  boxDecimalSeparator->clear();
  boxDecimalSeparator->addItem(
      tr("default") + " (" + QLocale::system().toString(1000.0, 'f', 1) + ")");
  boxDecimalSeparator->addItem(QLocale::c().toString(1000.0, 'f', 1));
  boxDecimalSeparator->addItem(
      QLocale(QLocale::German).toString(1000.0, 'f', 1));
  boxDecimalSeparator->addItem(
      QLocale(QLocale::French).toString(1000.0, 'f', 1));

  if (QLocale().name() == QLocale::c().name())
    boxDecimalSeparator->setCurrentIndex(1);
  else if (QLocale().name() == QLocale(QLocale::German).name())
    boxDecimalSeparator->setCurrentIndex(2);
  else if (QLocale().name() == QLocale(QLocale::French).name())
    boxDecimalSeparator->setCurrentIndex(3);

  boxSeparatorPreview->setText(
      tr("Preview:", "preview of the decimal separator") + " " +
      QLocale().toString(1000.1234567890123456, 'f', boxAppPrecision->value()));

  // tables page
  boxTableComments->setText(tr("&Display Comments in Header"));
  groupBoxTableCol->setTitle(tr("Colors"));
  lblSeparator->setText(tr("Default Column Separator"));
  boxSeparator->clear();
  boxSeparator->addItem(tr("TAB"));
  boxSeparator->addItem(tr("SPACE"));
  boxSeparator->addItem(";" + tr("TAB"));
  boxSeparator->addItem("," + tr("TAB"));
  boxSeparator->addItem(";" + tr("SPACE"));
  boxSeparator->addItem("," + tr("SPACE"));
  boxSeparator->addItem(";");
  boxSeparator->addItem(",");
  setColumnSeparator(app_->columnSeparator);

  lblTableBackground->setText(tr("Background"));
  lblTextColor->setText(tr("Text"));
  lblHeaderColor->setText(tr("Labels"));
  groupBoxTableFonts->setTitle(tr("Fonts"));

  // curves page
  lblCurveStyle->setText(tr("Default curve style"));
  lblLineWidth->setText(tr("Line width"));
  lblSymbSize->setText(tr("Symbol size"));

  boxCurveStyle->clear();
  boxCurveStyle->addItem(
      IconLoader::load("graph2d-line", IconLoader::LightDark), tr(" Line"));
  boxCurveStyle->addItem(
      IconLoader::load("graph2d-scatter", IconLoader::LightDark),
      tr(" Scatter"));
  boxCurveStyle->addItem(
      IconLoader::load("graph2d-line-scatter", IconLoader::LightDark),
      tr(" Line + Symbol"));
  boxCurveStyle->addItem(
      IconLoader::load("graph2d-vertical-drop", IconLoader::LightDark),
      tr(" Vertical drop lines"));
  boxCurveStyle->addItem(
      IconLoader::load("graph2d-spline", IconLoader::LightDark), tr(" Spline"));
  boxCurveStyle->addItem(
      IconLoader::load("graph2d-vertical-step", IconLoader::LightDark),
      tr(" Vertical steps"));
  boxCurveStyle->addItem(
      IconLoader::load("graph2d-horizontal-step", IconLoader::LightDark),
      tr(" Horizontal steps"));
  boxCurveStyle->addItem(
      IconLoader::load("graph2d-area", IconLoader::LightDark), tr(" Area"));
  boxCurveStyle->addItem(
      IconLoader::load("graph2d-vertical-bar", IconLoader::LightDark),
      tr(" Vertical Bars"));
  boxCurveStyle->addItem(
      IconLoader::load("graph2d-horizontal-bar", IconLoader::LightDark),
      tr(" Horizontal Bars"));

  // plots 3D
  lblResolution->setText(tr("Resolution"));
  boxResolution->setSpecialValueText("1 " + tr("(all data shown)"));
  boxShowLegend->setText(tr("&Show Legend"));
  boxShowProjection->setText(tr("Show &Projection"));
  btnFromColor->setText(tr("&Data Max"));
  boxSmoothMesh->setText(tr("Smoot&h Line"));
  boxOrthogonal->setText(tr("O&rthogonal"));
  btnLabels->setText(tr("Lab&els"));
  btnMesh->setText(tr("Mesh &Line"));
  btnGrid->setText(tr("&Grid"));
  btnToColor->setText(tr("Data &Min"));
  btnNumbers->setText(tr("&Numbers"));
  btnAxes->setText(tr("A&xes"));
  btnBackground3D->setText(tr("&Background"));
  groupBox3DCol->setTitle(tr("Colors"));
  groupBox3DFonts->setTitle(tr("Fonts"));
  btnTitleFnt->setText(tr("&Title"));
  btnLabelsFnt->setText(tr("&Axes Labels"));
  btnNumFnt->setText(tr("&Numbers"));
  boxAutoscale3DPlots->setText(tr("Autosca&ling"));

  // Fitting page
  groupBoxFittingCurve->setTitle(tr("Generated Fit Curve"));
  generatePointsBtn->setText(tr("Uniform X Function"));
  lblPoints->setText(tr("Points"));
  samePointsBtn->setText(tr("Same X as Fitting Data"));
  linearFit2PointsBox->setText(tr("2 points for linear fits"));

  groupBoxMultiPeak->setTitle(tr("Display Peak Curves for Multi-peak Fits"));

  groupBoxFitParameters->setTitle(tr("Parameters Output"));
  lblPrecision->setText(tr("Significant Digits"));
  logBox->setText(tr("Write Parameters to Result Log"));
  plotLabelBox->setText(tr("Paste Parameters to Plot"));
  scaleErrorsBox->setText(tr("Scale Errors with sqrt(Chi^2/doF)"));
  groupBoxMultiPeak->setTitle(tr("Display Peak Curves for Multi-peak Fits"));
  lblPeaksColor->setText(tr("Peaks Color"));
}

void ConfigDialog::accept() {
  apply();
  close();
}

void ConfigDialog::apply() {
  // tables page
  QString sep = boxSeparator->currentText();
  sep.replace(tr("TAB"), "\t", Qt::CaseInsensitive);
  sep.replace("\\t", "\t");
  sep.replace(tr("SPACE"), " ");
  sep.replace("\\s", " ");

  if (sep.contains(QRegExp("[0-9.eE+-]")) != 0) {
    QMessageBox::warning(app_, tr("Import options error"),
                         tr("The separator must not contain the following "
                            "characters: 0-9eE.+-"));
    return;
  }

  app_->columnSeparator = sep;
  /*app_->customizeTables(buttonBackground->color(), buttonText->color(),
                        buttonHeader->color(), textFont, headerFont,
                        boxTableComments->isChecked());*/
  // 2D plots page: options tab
  app_->titleOn = boxTitle->isChecked();
  app_->allAxesOn = boxAllAxes->isChecked();
  app_->canvasFrameOn = boxFrame->isChecked();
  app_->canvasFrameWidth = boxFrameWidth->value();
  app_->drawBackbones = boxBackbones->isChecked();
  app_->axesLineWidth = boxLineWidth->value();
  app_->defaultPlotMargin = boxMargin->value();
  // 2D plots page: curves tab
  app_->defaultCurveStyle = curveStyle();
  app_->defaultCurveLineWidth = boxCurveLineWidth->value();
  app_->defaultSymbolSize = 2 * boxSymbolSize->value() + 1;
  // 2D plots page: ticks tab
  app_->majTicksLength = boxMajTicksLength->value();
  app_->minTicksLength = boxMinTicksLength->value();
  app_->majTicksStyle = boxMajTicks->currentIndex();
  app_->minTicksStyle = boxMinTicks->currentIndex();
  // 2D plots page: fonts tab
  app_->plotAxesFont = axesFont;
  app_->plotNumbersFont = numbersFont;
  app_->plotLegendFont = legendFont;
  app_->plotTitleFont = titleFont;
  // 2D plots page: print tab
  app_->d_print_cropmarks = boxPrintCropmarks->isChecked();
  app_->d_scale_plots_on_print = boxScaleLayersOnPrint->isChecked();
  QList<QMdiSubWindow *> windows = app_->subWindowsList();
  // general page: application tab
  app_->changeAppFont(appFont);
  setFont(appFont);
  //app_->changeAppStyle(boxStyle->currentText());
  //app_->changeAppColorScheme(boxColorScheme->currentIndex());
#ifdef SEARCH_FOR_UPDATES
  app_->autoSearchUpdates = boxSearchUpdates->isChecked();
#endif
  app_->setSaveSettings(boxSave->isChecked(), boxMinutes->value());
  app_->defaultScriptingLang = boxScriptingLanguage->currentText();

  app_->undoLimit =
      boxUndoLimit->value();  // FIXME: can apply only after restart

  // general page: numeric format tab
  app_->d_decimal_digits = boxAppPrecision->value();
  QLocale locale;
  switch (boxDecimalSeparator->currentIndex()) {
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
  }

  int currentBoxIndex = boxDefaultNumericFormat->currentIndex();
  if (currentBoxIndex > -1) {
    app_->d_default_numeric_format =
        boxDefaultNumericFormat->itemData(currentBoxIndex).toChar().toLatin1();
  }

  if (boxUseGroupSeparator->isChecked())
    locale.setNumberOptions(locale.numberOptions() &
                            ~QLocale::OmitGroupSeparator);
  else
    locale.setNumberOptions(locale.numberOptions() |
                            QLocale::OmitGroupSeparator);

  if (QLocale() != locale) {
    QLocale::setDefault(locale);
  }

  // general page: confirmations tab
  app_->confirmCloseFolder = boxFolders->isChecked();
  //app_->updateConfirmOptions(boxTables->isChecked(), boxMatrices->isChecked(),
  //                           boxPlots2D->isChecked(), boxPlots3D->isChecked(),
  //                           boxNotes->isChecked());
  // general page: colors tab
  // app->setAppColors(btnWorkspace->color(), btnPanels->color(),
  //                  btnPanelsText->color());
  // 3D plots page
  app_->plot3DColors = plot3DColors;
  app_->showPlot3DLegend = boxShowLegend->isChecked();
  app_->showPlot3DProjection = boxShowProjection->isChecked();
  app_->plot3DResolution = boxResolution->value();
  app_->plot3DTitleFont = plot3DTitleFont;
  app_->plot3DNumbersFont = plot3DNumbersFont;
  app_->plot3DAxesFont = plot3DAxesFont;
  app_->orthogonal3DPlots = boxOrthogonal->isChecked();
  app_->smooth3DMesh = boxSmoothMesh->isChecked();
  app_->autoscale3DPlots = boxAutoscale3DPlots->isChecked();
  //app_->setPlot3DOptions();

  // fitting page
  app_->fit_output_precision = boxPrecision->value();
  app_->pasteFitResultsToPlot = plotLabelBox->isChecked();
  app_->writeFitResultsToLog = logBox->isChecked();
  app_->fitPoints = generatePointsBox->value();
  app_->generateUniformFitPoints = generatePointsBtn->isChecked();
  app_->generatePeakCurves = groupBoxMultiPeak->isChecked();
  app_->peakCurvesColor = boxPeaksColor->currentIndex();
  app_->fit_scale_errors = scaleErrorsBox->isChecked();
  app_->d_2_linear_fit_points = linearFit2PointsBox->isChecked();
  app_->saveSettings();

  // calculate a sensible width for the items list
  // (default QListWidget size is 256 which looks too big)
  QFontMetrics fm(itemsList->font());
  int width = 32, i;
  for (i = 0; i < itemsList->count(); i++)
    if (fm.width(itemsList->item(i)->text()) > width)
      width = fm.width(itemsList->item(i)->text());
  itemsList->setMaximumWidth(itemsList->iconSize().width() + width + 50);
  // resize the list to the maximum width
  itemsList->resize(itemsList->maximumWidth(), itemsList->height());
}

int ConfigDialog::curveStyle() {
  int style = 0;
  return style;
}

void ConfigDialog::pickBgColor() {
  QColor c = QColorDialog::getColor(buttonBackground->color(), this);
  if (!c.isValid() || c == buttonBackground->color()) return;

  buttonBackground->setColor(c);
}

void ConfigDialog::pickTextColor() {
  QColor c = QColorDialog::getColor(buttonText->color(), this);
  if (!c.isValid() || c == buttonText->color()) return;

  buttonText->setColor(c);
}

void ConfigDialog::pickHeaderColor() {
  QColor c = QColorDialog::getColor(buttonHeader->color(), this);
  if (!c.isValid() || c == buttonHeader->color()) return;

  buttonHeader->setColor(c);
}

void ConfigDialog::pickTextFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, textFont, this);
  if (ok) {
    textFont = font;
  } else {
    return;
  }
}

void ConfigDialog::pickHeaderFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, headerFont, this);
  if (ok) {
    headerFont = font;
  } else {
    return;
  }
}

void ConfigDialog::pickLegendFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, legendFont, this);
  if (ok) {
    legendFont = font;
  } else {
    return;
  }
}

void ConfigDialog::pickAxesFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, axesFont, this);
  if (ok) {
    axesFont = font;
  } else {
    return;
  }
}

void ConfigDialog::pickNumbersFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, numbersFont, this);
  if (ok) {
    numbersFont = font;
  } else {
    return;
  }
}

void ConfigDialog::pickTitleFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, titleFont, this);
  if (ok)
    titleFont = font;
  else
    return;
}

void ConfigDialog::pickApplicationFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, appFont, this);
  if (ok)
    appFont = font;
  else
    return;
  fontsBtn->setFont(appFont);
}

void ConfigDialog::pickPanelsTextColor() {
  QColor c = QColorDialog::getColor(btnPanelsText->color(), this);
  if (!c.isValid() || c == btnPanelsText->color()) return;

  btnPanelsText->setColor(c);
}

void ConfigDialog::pickPanelsColor() {
  QColor c = QColorDialog::getColor(btnPanels->color(), this);
  if (!c.isValid() || c == btnPanels->color()) return;

  btnPanels->setColor(c);
}

void ConfigDialog::pickWorkspaceColor() {
  QColor c = QColorDialog::getColor(btnWorkspace->color(), this);
  if (!c.isValid() || c == btnWorkspace->color()) return;

  btnWorkspace->setColor(c);
}

void ConfigDialog::pickDataMaxColor() {
  QColor c = QColorDialog::getColor(QColor(plot3DColors[0]), this);
  if (!c.isValid()) return;

  plot3DColors[0] = c.name();
}

void ConfigDialog::pickDataMinColor() {
  QColor c = QColorDialog::getColor(QColor(plot3DColors[4]), this);
  if (!c.isValid()) return;

  plot3DColors[4] = c.name();
}

void ConfigDialog::pick3DBackgroundColor() {
  QColor c = QColorDialog::getColor(QColor(plot3DColors[7]), this);
  if (!c.isValid()) return;

  plot3DColors[7] = c.name();
}

void ConfigDialog::pickMeshColor() {
  QColor c = QColorDialog::getColor(QColor(plot3DColors[2]), this);
  if (!c.isValid()) return;

  plot3DColors[2] = c.name();
}

void ConfigDialog::pickGridColor() {
  QColor c = QColorDialog::getColor(QColor(plot3DColors[3]), this);
  if (!c.isValid()) return;

  plot3DColors[3] = c.name();
}

void ConfigDialog::pick3DAxesColor() {
  QColor c = QColorDialog::getColor(QColor(plot3DColors[6]), this);
  if (!c.isValid()) return;

  plot3DColors[6] = c.name();
}

void ConfigDialog::pick3DNumbersColor() {
  QColor c = QColorDialog::getColor(QColor(plot3DColors[5]), this);
  if (!c.isValid()) return;

  plot3DColors[5] = c.name();
}

void ConfigDialog::pick3DLabelsColor() {
  QColor c = QColorDialog::getColor(QColor(plot3DColors[1]), this);
  if (!c.isValid()) return;

  plot3DColors[1] = c.name();
}

void ConfigDialog::pick3DTitleFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, plot3DTitleFont, this);
  if (ok)
    plot3DTitleFont = font;
  else
    return;
}

void ConfigDialog::pick3DNumbersFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, plot3DNumbersFont, this);
  if (ok)
    plot3DNumbersFont = font;
  else
    return;
}

void ConfigDialog::pick3DAxesFont() {
  bool ok;
  QFont font = QFontDialog::getFont(&ok, plot3DAxesFont, this);
  if (ok)
    plot3DAxesFont = font;
  else
    return;
}

void ConfigDialog::setColumnSeparator(const QString &sep) {
  if (sep == "\t")
    boxSeparator->setCurrentIndex(0);
  else if (sep == " ")
    boxSeparator->setCurrentIndex(1);
  else if (sep == ";\t")
    boxSeparator->setCurrentIndex(2);
  else if (sep == ",\t")
    boxSeparator->setCurrentIndex(3);
  else if (sep == "; ")
    boxSeparator->setCurrentIndex(4);
  else if (sep == ", ")
    boxSeparator->setCurrentIndex(5);
  else if (sep == ";")
    boxSeparator->setCurrentIndex(6);
  else if (sep == ",")
    boxSeparator->setCurrentIndex(7);
  else {
    QString separator = sep;
    boxSeparator->setEditText(
        separator.replace(" ", "\\s").replace("\t", "\\t"));
  }
}

void ConfigDialog::switchToLanguage(int param) {
  app_->switchToLanguage(param);
  languageChange();
}

void ConfigDialog::insertLanguagesList() {
  QDir dir(app_->qmPath);
  QStringList locales = app_->locales;
  QStringList languages;
  int lang = 0;
  for (int i = 0; i < locales.size(); i++) {
    if (locales[i] == "en")
      languages.push_back("English");
    else {
      QTranslator translator;
      translator.load("alphaplot_" + locales[i], app_->qmPath);

      QString language = translator.translate("ApplicationWindow", "English",
                                              "translate this to the language "
                                              "of the translation file, NOT to "
                                              "the meaning of English!");
      if (!language.isEmpty())
        languages.push_back(language);
      else
        languages.push_back(locales[i]);
    }

    if (locales[i] == app_->appLanguage) lang = i;
  }
  boxLanguage->addItems(languages);
  boxLanguage->setCurrentIndex(lang);
}

void ConfigDialog::showPointsBox(bool) {
  if (generatePointsBtn->isChecked()) {
    lblPoints->show();
    generatePointsBox->show();
    linearFit2PointsBox->show();
  } else {
    lblPoints->hide();
    generatePointsBox->hide();
    linearFit2PointsBox->hide();
  }
}

void ConfigDialog::updateDecSepPreview() {
  QLocale locale;
  switch (boxDecimalSeparator->currentIndex()) {
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
  }

  if (boxUseGroupSeparator->isChecked())
    locale.setNumberOptions(locale.numberOptions() &
                            ~QLocale::OmitGroupSeparator);
  else
    locale.setNumberOptions(locale.numberOptions() |
                            QLocale::OmitGroupSeparator);

  boxSeparatorPreview->setText(
      tr("Preview:", "preview of the decimal separator") + " " +
      locale.toString(1000.1234567890123456, 'f', boxAppPrecision->value()));
}
