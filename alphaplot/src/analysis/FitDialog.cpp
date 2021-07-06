/***************************************************************************
    File                 : FitDialog.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Fit Wizard

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
#include "FitDialog.h"

#include <../3rdparty/muparser/muParserError.h>
#include <stdio.h>

#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QLayout>
#include <QLibrary>
#include <QLineEdit>
#include <QListWidget>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTableWidget>
#include <QWidget>
#include <QWidgetList>

#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Plotcolumns.h"
#include "ApplicationWindow.h"
#include "ColorBox.h"
#include "ExponentialFit.h"
#include "Fit.h"
#include "Matrix.h"
#include "MultiPeakFit.h"
#include "NonLinearFit.h"
#include "PluginFit.h"
#include "PolynomialFit.h"
#include "SigmoidalFit.h"
#include "scripting/MyParser.h"

#define CONFS(string) \
  QString::number(QLocale().toDouble(string), 'g', boxPrecision->value())

FitDialog::FitDialog(QWidget *parent, Qt::WindowFlags fl)
    : QDialog(parent, fl), app_(qobject_cast<ApplicationWindow *>(parent)) {
  Q_ASSERT(app_);
  setWindowTitle(tr("Fit Wizard"));
  setSizeGripEnabled(true);

  d_user_function_names = QStringList();
  d_user_functions = QStringList();
  d_user_function_params = QStringList();

  d_fitter = nullptr;

  tw = new QStackedWidget();

  initEditPage();
  initFitPage();
  initAdvancedPage();

  QVBoxLayout *vl = new QVBoxLayout();
  vl->addWidget(tw);
  setLayout(vl);

  setBuiltInFunctionNames();
  setBuiltInFunctions();

  categoryBox->setCurrentRow(2);
  funcBox->setCurrentRow(0);

  loadPlugins();
}

void FitDialog::initFitPage() {
  QGridLayout *gl1 = new QGridLayout();
  gl1->addWidget(new QLabel(tr("Curve")), 0, 0);
  boxCurve = new QComboBox();
  gl1->addWidget(boxCurve, 0, 1);
  gl1->addWidget(new QLabel(tr("Function")), 1, 0);
  lblFunction = new QLabel();
  gl1->addWidget(lblFunction, 1, 1);
  boxFunction = new QTextEdit();
  boxFunction->setReadOnly(true);
  QPalette palette = boxFunction->palette();
  palette.setColor(QPalette::Active, QPalette::Base, Qt::lightGray);
  boxFunction->setPalette(palette);
  boxFunction->setMaximumHeight(50);
  gl1->addWidget(boxFunction, 2, 1);
  gl1->addWidget(new QLabel(tr("Initial guesses")), 3, 0);

  boxParams = new QTableWidget();
  boxParams->setColumnCount(3);
  boxParams->horizontalHeader()->setSectionsClickable(false);
  boxParams->horizontalHeader()->setSectionResizeMode(
      0, QHeaderView::ResizeToContents);
  boxParams->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
  boxParams->horizontalHeader()->setSectionResizeMode(
      2, QHeaderView::ResizeToContents);
  boxParams->verticalHeader()->setSectionResizeMode(
      QHeaderView::ResizeToContents);
  QStringList header = QStringList()
                       << tr("Parameter") << tr("Value") << tr("Constant");
  boxParams->setHorizontalHeaderLabels(header);
  boxParams->verticalHeader()->hide();
  gl1->addWidget(boxParams, 3, 1);

  gl1->addWidget(new QLabel(tr("Algorithm")), 4, 0);
  boxAlgorithm = new QComboBox();
  boxAlgorithm->addItem(tr("Scaled Levenberg-Marquardt"));
  boxAlgorithm->addItem(tr("Unscaled Levenberg-Marquardt"));
  boxAlgorithm->addItem(tr("Nelder-Mead Simplex"));
  gl1->addWidget(boxAlgorithm, 4, 1);

  gl1->addWidget(new QLabel(tr("Color")), 5, 0);
  boxColor = new ColorBox();
  boxColor->setColor(QColor(Qt::red));
  gl1->addWidget(boxColor, 5, 1);

  QGroupBox *gb1 = new QGroupBox();
  gb1->setLayout(gl1);

  QGridLayout *gl2 = new QGridLayout();
  gl2->addWidget(new QLabel(tr("From x=")), 0, 0);
  boxFrom = new QLineEdit();
  gl2->addWidget(boxFrom, 0, 1);
  gl2->addWidget(new QLabel(tr("To x=")), 1, 0);
  boxTo = new QLineEdit();
  gl2->addWidget(boxTo, 1, 1);
  QGroupBox *gb2 = new QGroupBox();
  gb2->setLayout(gl2);

  QGridLayout *gl3 = new QGridLayout();
  gl3->addWidget(new QLabel(tr("Iterations")), 0, 0);
  boxPoints = new QSpinBox();
  boxPoints->setRange(10, 10000);
  boxPoints->setSingleStep(50);
  boxPoints->setValue(1000);
  gl3->addWidget(boxPoints, 0, 1);
  gl3->addWidget(new QLabel(tr("Tolerance")), 1, 0);
  boxTolerance = new QLineEdit("1e-4");
  gl3->addWidget(boxTolerance, 1, 1);
  QGroupBox *gb3 = new QGroupBox();
  gb3->setLayout(gl3);

  QHBoxLayout *hbox1 = new QHBoxLayout();
  hbox1->addWidget(gb2);
  hbox1->addWidget(gb3);

  QHBoxLayout *hbox2 = new QHBoxLayout();
  hbox2->addWidget(new QLabel(tr("Y Error Source")));
  boxYErrorSource = new QComboBox();
  boxYErrorSource->addItem(tr("Errors Unknown"));
  boxYErrorSource->addItem(tr("Associated"));
  boxYErrorSource->addItem(tr("Statistical (Poisson)"));
  boxYErrorSource->addItem(tr("Arbitrary Dataset"));
  hbox2->addWidget(boxYErrorSource);
  QGroupBox *gb4 = new QGroupBox();
  gb4->setLayout(hbox2);

  tableNamesBox = new QComboBox();
  tableNamesBox->setEnabled(false);
  hbox2->addWidget(tableNamesBox);
  colNamesBox = new QComboBox();
  colNamesBox->setEnabled(false);
  hbox2->addWidget(colNamesBox);

  QHBoxLayout *hbox3 = new QHBoxLayout();
  buttonEdit = new QPushButton(tr("<< &Edit function"));
  hbox3->addWidget(buttonEdit);
  btnDeleteFitCurves = new QPushButton(tr("&Delete Fit Curves"));
  hbox3->addWidget(btnDeleteFitCurves);
  buttonOk = new QPushButton(tr("&Fit"));
  buttonOk->setDefault(true);
  hbox3->addWidget(buttonOk);
  buttonCancel1 = new QPushButton(tr("&Close"));
  hbox3->addWidget(buttonCancel1);
  buttonAdvanced = new QPushButton(tr("Custom &Output >>"));
  hbox3->addWidget(buttonAdvanced);
  hbox3->addStretch();

  QVBoxLayout *vbox1 = new QVBoxLayout();
  vbox1->addWidget(gb1);
  vbox1->addLayout(hbox1);
  vbox1->addWidget(gb4);
  vbox1->addLayout(hbox3);

  fitPage = new QWidget();
  fitPage->setLayout(vbox1);
  tw->addWidget(fitPage);

  connect(boxCurve, SIGNAL(activated(const QString &)), this,
          SLOT(activateCurve(const QString &)));
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
  connect(buttonCancel1, SIGNAL(clicked()), this, SLOT(close()));
  connect(buttonEdit, SIGNAL(clicked()), this, SLOT(showEditPage()));
  connect(btnDeleteFitCurves, SIGNAL(clicked()), this, SLOT(deleteFitCurves()));
  connect(boxYErrorSource, SIGNAL(activated(int)), this,
          SLOT(yErrorSourceChanged(int)));
  connect(buttonAdvanced, SIGNAL(clicked()), this, SLOT(showAdvancedPage()));
  connect(tableNamesBox, SIGNAL(activated(int)), this,
          SLOT(selectSrcTable(int)));

  setFocusProxy(boxFunction);
}

void FitDialog::initEditPage() {
  QGridLayout *gl1 = new QGridLayout();
  gl1->addWidget(new QLabel(tr("Category")), 0, 0);
  gl1->addWidget(new QLabel(tr("Function")), 0, 1);
  gl1->addWidget(new QLabel(tr("Expression")), 0, 2);

  categoryBox = new QListWidget();
  categoryBox->addItem(tr("User defined"));
  categoryBox->addItem(tr("Built-in"));
  categoryBox->addItem(tr("Basic"));
  categoryBox->addItem(tr("Plugins"));

  gl1->addWidget(categoryBox, 1, 0);
  funcBox = new QListWidget();
  gl1->addWidget(funcBox, 1, 1);
  explainBox = new QTextEdit();
  explainBox->setReadOnly(true);
  gl1->addWidget(explainBox, 1, 2);

  boxUseBuiltIn = new QCheckBox();
  boxUseBuiltIn->setText(tr("Fit with &built-in function"));
  boxUseBuiltIn->hide();

  QHBoxLayout *hbox1 = new QHBoxLayout();
  hbox1->addWidget(boxUseBuiltIn);
  hbox1->addStretch();

  polynomOrderLabel = new QLabel(tr("Polynomial Order"));
  polynomOrderLabel->hide();
  hbox1->addWidget(polynomOrderLabel);

  polynomOrderBox = new QSpinBox();
  polynomOrderBox->setMinimum(1);
  polynomOrderBox->setValue(2);
  polynomOrderBox->hide();
  connect(polynomOrderBox, SIGNAL(valueChanged(int)), this,
          SLOT(showExpression(int)));
  hbox1->addWidget(polynomOrderBox);

  buttonPlugins = new QPushButton(tr("&Choose plugins folder..."));
  hbox1->addWidget(buttonPlugins);
  buttonPlugins->hide();

  buttonClearUsrList = new QPushButton(tr("Clear user &list"));
  hbox1->addWidget(buttonClearUsrList);
  buttonClearUsrList->hide();

  QGridLayout *gl2 = new QGridLayout();
  gl2->addWidget(new QLabel(tr("Name")), 0, 0);
  boxName = new QLineEdit(tr("user1"));
  gl2->addWidget(boxName, 0, 1);
  btnAddFunc = new QPushButton(tr("&Save"));
  gl2->addWidget(btnAddFunc, 0, 2);
  gl2->addWidget(new QLabel(tr("Parameters")), 1, 0);
  boxParam = new QLineEdit("a, b");
  gl2->addWidget(boxParam, 1, 1);
  btnDelFunc = new QPushButton(tr("&Remove"));
  gl2->addWidget(btnDelFunc, 1, 2);

  QGroupBox *gb = new QGroupBox();
  gb->setLayout(gl2);

  editBox = new QTextEdit();
  editBox->setAcceptRichText(false);
  editBox->setFocus();

  QVBoxLayout *vbox1 = new QVBoxLayout();
  btnAddTxt = new QPushButton(tr("Add &expression"));
  vbox1->addWidget(btnAddTxt);
  btnAddName = new QPushButton(tr("Add &name"));
  vbox1->addWidget(btnAddName);
  buttonClear = new QPushButton(tr("Rese&t"));
  vbox1->addWidget(buttonClear);
  buttonCancel2 = new QPushButton(tr("&Close"));
  vbox1->addWidget(buttonCancel2);
  btnContinue = new QPushButton(tr("&Fit >>"));
  vbox1->addWidget(btnContinue);
  vbox1->addStretch();

  QHBoxLayout *hbox2 = new QHBoxLayout();
  hbox2->addWidget(editBox);
  hbox2->addLayout(vbox1);

  QVBoxLayout *vbox2 = new QVBoxLayout();
  vbox2->addLayout(gl1);
  vbox2->addLayout(hbox1);
  vbox2->addWidget(gb);
  vbox2->addLayout(hbox2);

  editPage = new QWidget();
  editPage->setLayout(vbox2);
  tw->addWidget(editPage);

  connect(buttonPlugins, SIGNAL(clicked()), this, SLOT(choosePluginsFolder()));
  connect(buttonClear, SIGNAL(clicked()), this, SLOT(resetFunction()));
  connect(buttonClearUsrList, SIGNAL(clicked()), this,
          SLOT(clearUserFunctions()));
  connect(categoryBox, SIGNAL(currentRowChanged(int)), this,
          SLOT(showFunctionsList(int)));
  connect(funcBox, SIGNAL(currentRowChanged(int)), this,
          SLOT(showExpression(int)));
  connect(boxUseBuiltIn, SIGNAL(toggled(bool)), this, SLOT(setFunction(bool)));
  connect(btnAddName, SIGNAL(clicked()), this, SLOT(pasteFunctionName()));
  connect(btnAddTxt, SIGNAL(clicked()), this, SLOT(pasteExpression()));
  connect(btnContinue, SIGNAL(clicked()), this, SLOT(showFitPage()));
  connect(btnAddFunc, SIGNAL(clicked()), this, SLOT(saveUserFunction()));
  connect(btnDelFunc, SIGNAL(clicked()), this, SLOT(removeUserFunction()));
  connect(buttonCancel2, SIGNAL(clicked()), this, SLOT(close()));
}

void FitDialog::initAdvancedPage() {
  generatePointsBtn = new QRadioButton(tr("&Uniform X Function"));
  generatePointsBtn->setChecked(app_->generateUniformFitPoints);
  connect(generatePointsBtn, SIGNAL(clicked()), this,
          SLOT(enableApplyChanges()));

  QGridLayout *gl1 = new QGridLayout();
  gl1->addWidget(generatePointsBtn, 0, 0);

  lblPoints = new QLabel(tr("Points"));

  generatePointsBox = new QSpinBox();
  generatePointsBox->setRange(0, 1000000);
  generatePointsBox->setSingleStep(10);
  generatePointsBox->setValue(app_->fitPoints);
  connect(generatePointsBox, SIGNAL(valueChanged(int)), this,
          SLOT(enableApplyChanges(int)));
  showPointsBox(!app_->generateUniformFitPoints);

  QHBoxLayout *hb = new QHBoxLayout();
  hb->addStretch();
  hb->addWidget(lblPoints);
  hb->addWidget(generatePointsBox);
  gl1->addLayout(hb, 0, 1);

  samePointsBtn = new QRadioButton(tr("Same X as Fitting &Data"));
  gl1->addWidget(samePointsBtn, 1, 0);
  samePointsBtn->setChecked(!app_->generateUniformFitPoints);
  connect(samePointsBtn, SIGNAL(clicked()), this, SLOT(enableApplyChanges()));

  QGroupBox *gb1 = new QGroupBox(tr("Generated Fit Curve"));
  gb1->setLayout(gl1);

  QGridLayout *gl2 = new QGridLayout();
  gl2->addWidget(new QLabel(tr("Significant Digits")), 0, 1);
  boxPrecision = new QSpinBox();
  boxPrecision->setRange(0, 15);
  boxPrecision->setValue(app_->fit_output_precision);
  connect(boxPrecision, SIGNAL(valueChanged(int)), this,
          SLOT(enableApplyChanges(int)));
  gl2->addWidget(boxPrecision, 0, 2);
  btnParamTable = new QPushButton(tr("Parameters &Table"));
  gl2->addWidget(btnParamTable, 1, 0);
  gl2->addWidget(new QLabel(tr("Name: ")), 1, 1);
  paramTableName = new QLineEdit(tr("Parameters"));
  gl2->addWidget(paramTableName, 1, 2);
  btnCovMatrix = new QPushButton(tr("Covariance &Matrix"));
  gl2->addWidget(btnCovMatrix, 2, 0);
  gl2->addWidget(new QLabel(tr("Name: ")), 2, 1);
  covMatrixName = new QLineEdit(tr("CovMatrix"));
  gl2->addWidget(covMatrixName, 2, 2);

  scaleErrorsBox = new QCheckBox(tr("Scale Errors with sqrt(Chi^2/doF)"));
  scaleErrorsBox->setChecked(app_->fit_scale_errors);
  connect(scaleErrorsBox, SIGNAL(stateChanged(int)), this,
          SLOT(enableApplyChanges(int)));

  QGroupBox *gb2 = new QGroupBox(tr("Parameters Output"));
  gb2->setLayout(gl2);

  logBox = new QCheckBox(tr("&Write Parameters to Result Log"));
  logBox->setChecked(app_->writeFitResultsToLog);
  connect(logBox, SIGNAL(stateChanged(int)), this,
          SLOT(enableApplyChanges(int)));

  plotLabelBox = new QCheckBox(tr("&Paste Parameters to Plot"));
  plotLabelBox->setChecked(app_->pasteFitResultsToPlot);
  connect(plotLabelBox, SIGNAL(stateChanged(int)), this,
          SLOT(enableApplyChanges(int)));

  QHBoxLayout *hbox1 = new QHBoxLayout();

  btnBack = new QPushButton(tr("<< &Fit"));
  connect(btnBack, SIGNAL(clicked()), this, SLOT(showFitPage()));
  connect(btnBack, SIGNAL(clicked()), this, SLOT(applyChanges()));
  hbox1->addWidget(btnBack);

  btnApply = new QPushButton(tr("&Apply"));
  btnApply->setEnabled(false);
  connect(btnApply, SIGNAL(clicked()), this, SLOT(applyChanges()));
  hbox1->addWidget(btnApply);

  buttonCancel3 = new QPushButton(tr("&Close"));
  hbox1->addWidget(buttonCancel3);
  hbox1->addStretch();

  QVBoxLayout *vbox1 = new QVBoxLayout();
  vbox1->addWidget(gb1);
  vbox1->addWidget(gb2);
  vbox1->addWidget(scaleErrorsBox);
  vbox1->addWidget(logBox);
  vbox1->addWidget(plotLabelBox);
  vbox1->addStretch();
  vbox1->addLayout(hbox1);

  advancedPage = new QWidget();
  advancedPage->setLayout(vbox1);
  tw->addWidget(advancedPage);

  connect(btnParamTable, SIGNAL(clicked()), this, SLOT(showParametersTable()));
  connect(btnCovMatrix, SIGNAL(clicked()), this, SLOT(showCovarianceMatrix()));
  connect(samePointsBtn, SIGNAL(toggled(bool)), this,
          SLOT(showPointsBox(bool)));
  connect(generatePointsBtn, SIGNAL(toggled(bool)), this,
          SLOT(showPointsBox(bool)));
  connect(buttonCancel3, SIGNAL(clicked()), this, SLOT(close()));
}

void FitDialog::applyChanges() {
  app_->fit_output_precision = boxPrecision->value();
  app_->pasteFitResultsToPlot = plotLabelBox->isChecked();
  app_->writeFitResultsToLog = logBox->isChecked();
  app_->fitPoints = generatePointsBox->value();
  app_->generateUniformFitPoints = generatePointsBtn->isChecked();
  app_->fit_scale_errors = scaleErrorsBox->isChecked();
  app_->saveSettings();
  btnApply->setEnabled(false);
}

void FitDialog::showParametersTable() {
  QString tableName = paramTableName->text();
  if (tableName.isEmpty()) {
    QMessageBox::critical(
        this, tr("Error"),
        tr("Please enter a valid name for the parameters table."));
    return;
  }

  if (!d_fitter) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Please perform a fit first and try again."));
    return;
  }
  tableName = app_->generateUniqueName(tableName, false);
  d_fitter->parametersTable(tableName);
}

void FitDialog::showCovarianceMatrix() {
  QString matrixName = covMatrixName->text();
  if (matrixName.isEmpty()) {
    QMessageBox::critical(
        this, tr("Error"),
        tr("Please enter a valid name for the covariance matrix."));
    return;
  }

  if (!d_fitter) {
    QMessageBox::critical(this, tr("Error"),
                          tr("Please perform a fit first and try again."));
    return;
  }
  matrixName = app_->generateUniqueName(matrixName, false);
  d_fitter->covarianceMatrix(matrixName);
}

void FitDialog::showPointsBox(bool) {
  if (generatePointsBtn->isChecked()) {
    lblPoints->show();
    generatePointsBox->show();
  } else {
    lblPoints->hide();
    generatePointsBox->hide();
  }
}

void FitDialog::setAxisRect(AxisRect2D *axisrect) {
  if (!axisrect) return;

  axisrect_ = axisrect;
  boxCurve->clear();
  boxCurve->addItems(PlotColumns::getstringlistfromassociateddata(axisrect_));

  QString selectedCurve = boxCurve->currentText();
  if (!selectedCurve.isEmpty()) {
    int index = boxCurve->findText(selectedCurve);
    boxCurve->setCurrentIndex(index);
  }
  activateCurve(boxCurve->currentText());
}

void FitDialog::activateCurve(const QString &curveName) {
  if (!axisrect_) return;
  PlotData::AssociatedData *associateddata;
  associateddata =
      PlotColumns::getassociateddatafromstring(axisrect_, curveName);
  if (!associateddata) return;

  Column *col = associateddata->xcol;
  xmin_ = col->valueAt(associateddata->from);
  xmax_ = col->valueAt(associateddata->from);
  for (int i = associateddata->from; i < associateddata->to + 1; i++) {
    double value = col->valueAt(i);
    if (xmin_ > value) xmin_ = value;
    if (xmax_ < value) xmax_ = value;
  }

  double start = xmin_;
  double end = xmax_;
  boxFrom->setText(QLocale().toString(std::min(start, end), 'g', 15));
  boxTo->setText(QLocale().toString(std::max(start, end), 'g', 15));
}

void FitDialog::saveUserFunction() {
  if (editBox->toPlainText().isEmpty()) {
    QMessageBox::critical(this, tr("Input function error"),
                          tr("Please enter a valid function!"));
    editBox->setFocus();
    return;
  } else if (boxName->text().isEmpty()) {
    QMessageBox::critical(this, tr("Input function error"),
                          tr("Please enter a function name!"));
    boxName->setFocus();
    return;
  } else if (boxParam->text().remove(QRegExp("[,;\\s]")).isEmpty()) {
    QMessageBox::critical(this, tr("Input function error"),
                          tr("Please enter at least one parameter name!"));
    boxParam->setFocus();
    return;
  }

  if (d_built_in_function_names.contains(boxName->text())) {
    QMessageBox::critical(
        this, tr("Error: function name"),
        "<p><b>" + boxName->text() + "</b>" +
            tr(" is a built-in function name"
               "<p>You must choose another name for your function!"));
    editBox->setFocus();
    return;
  }
  if (editBox->toPlainText().contains(boxName->text())) {
    QMessageBox::critical(this, tr("Input function error"),
                          tr("You can't define functions recursevely!"));
    editBox->setFocus();
    return;
  }

  QString name = boxName->text();
  QString f = name + "(x, " + boxParam->text() +
              ")=" + editBox->toPlainText().remove("\n");

  if (d_user_function_names.contains(name)) {
    int index = d_user_function_names.indexOf(name);
    d_user_functions[index] = f;
    d_user_function_params[index] = boxParam->text();

    if (funcBox->currentItem()->text() == name) showExpression(index);
  } else {
    d_user_function_names << name;
    d_user_functions << f;
    d_user_function_params << boxParam->text();

    if (categoryBox->currentRow() == 0) {
      funcBox->addItem(name);
      funcBox->setCurrentRow(funcBox->count() - 1);
    }

    if (d_user_function_names.count() > 0 && !boxUseBuiltIn->isEnabled() &&
        categoryBox->currentRow() == 0)
      boxUseBuiltIn->setEnabled(true);
  }
  buttonClearUsrList->setEnabled(true);
  emit saveFunctionsList(d_user_functions);
}

void FitDialog::removeUserFunction() {
  if (!funcBox->currentItem()) return;
  QString name = funcBox->currentItem()->text();
  if (d_user_function_names.contains(name)) {
    explainBox->setText(QString());

    int index = d_user_function_names.indexOf(name);
    d_user_function_names.removeAll(name);

    QString f = d_user_functions[index];
    d_user_functions.removeAll(f);

    f = d_user_function_params[index];
    d_user_function_params.removeAll(f);

    funcBox->clear();
    funcBox->addItems(d_user_function_names);
    funcBox->setCurrentRow(0);

    if (!d_user_function_names.count()) {
      boxUseBuiltIn->setEnabled(false);
      buttonClearUsrList->setEnabled(false);
    }

    emit saveFunctionsList(d_user_functions);
  }
}

void FitDialog::showFitPage() {
  int param_table_rows = boxParams->rowCount();

  QString par = boxParam->text().simplified();
  QStringList paramList =
      par.split(QRegExp("[,;]+[\\s]*"), QString::SkipEmptyParts);
  int parameters = paramList.count();
  boxParams->setRowCount(parameters);
  boxParams->hideColumn(2);

  if (parameters > 7) parameters = 7;
  boxParams->setMinimumHeight(4 + (parameters + 1) *
                                      boxParams->horizontalHeader()->height());

  for (int i = param_table_rows; i < paramList.count(); i++) {
    QTableWidgetItem *it = new QTableWidgetItem(paramList[i]);
    it->setFlags(Qt::ItemFlags(!Qt::ItemIsEditable));
    it->setBackground(QBrush(Qt::lightGray));
    it->setForeground(QBrush(Qt::darkRed));
    QFont font = it->font();
    font.setItalic(true);
    it->setFont(font);
    boxParams->setItem(i, 0, it);

    it = new QTableWidgetItem(
        QLocale().toString(1.0, 'f', boxPrecision->value()));
    it->setTextAlignment(Qt::AlignRight);
    boxParams->setItem(i, 1, it);
  }
  for (int i = 0; i < paramList.count(); i++)
    boxParams->item(i, 0)->setText(paramList[i]);

  // FIXME: this check is pretty ugly, should be changed to a more elegant way
  // some time
  if (!boxUseBuiltIn->isChecked() ||
      (boxUseBuiltIn->isChecked() && categoryBox->currentRow() != 3 &&
       categoryBox->currentRow() != 1)) {
    boxParams->showColumn(2);

    for (int i = 0; i < boxParams->rowCount(); i++) {
      QTableWidgetItem *it = new QTableWidgetItem();
      it->setFlags(Qt::ItemFlags(Qt::ItemIsEditable ^ 2));
      it->setBackground(QBrush(Qt::lightGray));
      boxParams->setItem(i, 2, it);

      QCheckBox *cb = new QCheckBox();
      boxParams->setCellWidget(i, 2, cb);
    }
  }

  boxFunction->setText(editBox->toPlainText().simplified());
  lblFunction->setText(boxName->text() + " (x, " + par + ")");

  tw->setCurrentWidget(fitPage);
}

void FitDialog::showEditPage() { tw->setCurrentWidget(editPage); }

void FitDialog::showAdvancedPage() { tw->setCurrentWidget(advancedPage); }

void FitDialog::setFunction(bool ok) {
  editBox->setEnabled(!ok);
  boxParam->setEnabled(!ok);
  boxName->setEnabled(!ok);
  btnAddFunc->setEnabled(!ok);
  btnAddName->setEnabled(!ok);
  btnAddTxt->setEnabled(!ok);
  buttonClear->setEnabled(!ok);

  if (ok) {
    boxName->setText(funcBox->currentItem()->text());
    editBox->setText(explainBox->toPlainText());

    if (categoryBox->currentRow() == 0 && d_user_function_params.size() > 0)
      boxParam->setText(d_user_function_params[funcBox->currentRow()]);
    else if (categoryBox->currentRow() == 1) {
      QStringList lst;
      switch (funcBox->currentRow()) {
        case 0:
          lst << "A1"
              << "A2"
              << "x0"
              << "dx";
          break;
        case 1:
          lst << "A"
              << "t"
              << "y0";
          break;
        case 2:
          lst << "A"
              << "t"
              << "y0";
          break;
        case 3:
          lst << "A1"
              << "t1"
              << "A2"
              << "t2"
              << "y0";
          break;
        case 4:
          lst << "A1"
              << "t1"
              << "A2"
              << "t2"
              << "A3"
              << "t3"
              << "y0";
          break;
        case 5:
          lst << "y0"
              << "A"
              << "xc"
              << "w";
          break;
        case 6:
          lst = MultiPeakFit::generateParameterList(polynomOrderBox->value());
          break;
        case 7:
          lst = MultiPeakFit::generateParameterList(polynomOrderBox->value());
          break;
        case 8:
          lst = PolynomialFit::generateParameterList(polynomOrderBox->value());
          break;
      }
      boxParam->setText(lst.join(", "));
    } else if (categoryBox->currentRow() == 3 && d_plugin_params.size() > 0)
      boxParam->setText(d_plugin_params[funcBox->currentRow()]);
  }
}

void FitDialog::clearUserFunctions() {
  d_user_functions.clear();
  d_user_function_names.clear();
  if (categoryBox->currentRow() == 0) {
    funcBox->clear();
    explainBox->clear();
    boxUseBuiltIn->setEnabled(false);
    buttonClearUsrList->setEnabled(false);
  }
  emit clearFunctionsList();
}

void FitDialog::addUserFunctions(const QStringList &list) {
  if (!list.count()) {
    boxUseBuiltIn->setEnabled(false);
    return;
  }

  for (int i = 0; i < list.count(); i++) {
    QString s = list.at(i).simplified();
    if (!s.isEmpty()) {
      d_user_functions << s;

      int pos1 = s.indexOf("(", 0);
      d_user_function_names << s.left(pos1);

      int pos2 = s.indexOf(")", pos1);
      d_user_function_params << s.mid(pos1 + 4, pos2 - pos1 - 4);
    }
  }
}

void FitDialog::showFunctionsList(int category) {
  boxUseBuiltIn->setChecked(false);
  boxUseBuiltIn->setEnabled(false);
  boxUseBuiltIn->hide();
  buttonPlugins->hide();
  buttonClearUsrList->hide();
  buttonClearUsrList->setEnabled(false);
  btnDelFunc->setEnabled(false);
  funcBox->blockSignals(true);
  funcBox->clear();
  explainBox->clear();
  polynomOrderLabel->hide();
  polynomOrderBox->hide();

  switch (category) {
    case 0:
      if (d_user_function_names.count() > 0) {
        showUserFunctions();
        buttonClearUsrList->show();
        boxUseBuiltIn->setEnabled(true);
        buttonClearUsrList->setEnabled(true);
      }

      boxUseBuiltIn->setText(tr("Fit with selected &user function"));
      boxUseBuiltIn->show();
      buttonClearUsrList->show();
      btnDelFunc->setEnabled(true);
      break;

    case 1:
      boxUseBuiltIn->setText(tr("Fit using &built-in function"));
      boxUseBuiltIn->show();
      boxUseBuiltIn->setEnabled(true);
      funcBox->addItems(d_built_in_function_names);
      break;

    case 2:
      showParseFunctions();
      break;

    case 3:
      buttonPlugins->show();
      boxUseBuiltIn->setText(tr("Fit using &plugin function"));
      boxUseBuiltIn->show();
      if (d_plugin_function_names.size() > 0) {
        funcBox->addItems(d_plugin_function_names);
        boxUseBuiltIn->setEnabled(true);
      }
      break;
  }
  funcBox->blockSignals(false);
  funcBox->setCurrentRow(0);
}

void FitDialog::choosePluginsFolder() {
  QString dir = QFileDialog::getExistingDirectory(
      this, tr("Choose the plugins folder"), QDir::currentPath(),
      QFileDialog::ShowDirsOnly);
  if (!dir.isEmpty()) {
    d_plugin_files_list.clear();
    d_plugin_function_names.clear();
    d_plugin_functions.clear();
    d_plugin_params.clear();
    funcBox->clear();
    explainBox->clear();

    app_->fitPluginsPath = dir;
    loadPlugins();
    if (d_plugin_function_names.size() > 0) {
      funcBox->addItems(d_plugin_function_names);
      if (!boxUseBuiltIn->isEnabled()) boxUseBuiltIn->setEnabled(true);

      funcBox->setCurrentRow(0);
    } else
      boxUseBuiltIn->setEnabled(false);
  }
}

void FitDialog::loadPlugins() {
  typedef char *(*fitFunc)();
  QString path = app_->fitPluginsPath + "/";
  QDir dir(path);
  QStringList lst = dir.entryList(QDir::Files | QDir::NoSymLinks);

  for (int i = 0; i < lst.count(); i++) {
    QLibrary lib(path + lst[i]);

    fitFunc name = (fitFunc)lib.resolve("name");
    fitFunc function = (fitFunc)lib.resolve("function");
    fitFunc params = (fitFunc)lib.resolve("parameters");

    if (name && function && params) {
      d_plugin_function_names << QString(name());
      d_plugin_functions << QString(function());
      d_plugin_params << QString(params());
      d_plugin_files_list << lib.fileName();
    }
  }
}

void FitDialog::showUserFunctions() {
  funcBox->addItems(d_user_function_names);
}

void FitDialog::setBuiltInFunctionNames() {
  d_built_in_function_names << "Boltzmann"
                            << "ExpGrowth"
                            << "ExpDecay1"
                            << "ExpDecay2"
                            << "ExpDecay3"
                            << "GaussAmp"
                            << "Gauss"
                            << "Lorentz"
                            << "Polynomial";
}

void FitDialog::setBuiltInFunctions() {
  d_built_in_functions << "(A1-A2)/(1+exp((x-x0)/dx))+A2";
  d_built_in_functions << "y0+A*exp(x/t)";
  d_built_in_functions << "y0+A*exp(-x/t)";
  d_built_in_functions << "y0+A1*exp(-x/t1)+A2*exp(-x/t2)";
  d_built_in_functions << "y0+A1*exp(-x/t1)+A2*exp(-x/t2)+A3*exp(-x/t3)";
  d_built_in_functions << "y0+A*exp(-(x-xc)*(x-xc)/(2*w*w))";
}

void FitDialog::showParseFunctions() {
  funcBox->addItems(MyParser::functionsList());
}

void FitDialog::showExpression(int function) {
  if (function < 0) return;

  if (categoryBox->currentRow() == 2) {
    explainBox->setText(MyParser::explainFunction(function));
  } else if (categoryBox->currentRow() == 1) {
    polynomOrderLabel->show();
    polynomOrderBox->show();

    switch (funcBox->currentRow()) {
      case 6:  // Gauss
        polynomOrderLabel->setText(tr("Peaks"));
        explainBox->setText(MultiPeakFit::generateFormula(
            polynomOrderBox->value(), MultiPeakFit::Gauss));
        break;
      case 7:  // Lorentz
        polynomOrderLabel->setText(tr("Peaks"));
        explainBox->setText(MultiPeakFit::generateFormula(
            polynomOrderBox->value(), MultiPeakFit::Lorentz));
        break;
      case 8:  // Polynomial
        polynomOrderLabel->setText(tr("Polynomial Order"));
        explainBox->setText(
            PolynomialFit::generateFormula(polynomOrderBox->value()));
        break;
      default:
        polynomOrderLabel->hide();
        polynomOrderBox->hide();
        polynomOrderBox->setValue(1);
        explainBox->setText(d_built_in_functions[function]);
    }
    setFunction(boxUseBuiltIn->isChecked());
  } else if (categoryBox->currentRow() == 0) {
    if (d_user_functions.size() > function) {
      QStringList l = d_user_functions[function].split("=");
      explainBox->setText(l[1]);
    } else
      explainBox->clear();
    setFunction(boxUseBuiltIn->isChecked());
  } else if (categoryBox->currentRow() == 3) {
    if (d_plugin_functions.size() > 0) {
      explainBox->setText(d_plugin_functions[function]);
      setFunction(boxUseBuiltIn->isChecked());
    } else
      explainBox->clear();
  }
}

void FitDialog::pasteExpression() {
  QString f = explainBox->toPlainText();
  if (categoryBox->currentRow() == 2) {  // basic parser function
    f = f.left(f.indexOf("(", 0) + 1);
    if (editBox->textCursor().hasSelection()) {
      QString markedText = editBox->textCursor().selectedText();
      editBox->insertPlainText(f + markedText + ")");
    } else
      editBox->insertPlainText(f + ")");
  } else
    editBox->insertPlainText(f);

  editBox->setFocus();
}

void FitDialog::pasteFunctionName() {
  if (!funcBox->currentItem()) return;
  editBox->insertPlainText(funcBox->currentItem()->text());
  editBox->setFocus();
}

void FitDialog::accept() {
  QString curve = boxCurve->currentText();
  QStringList curvesList =
      PlotColumns::getstringlistfromassociateddata(axisrect_);
  if (curvesList.contains(curve) <= 0) {
    QMessageBox::critical(
        this, tr("Warning"),
        tr("The curve <b> %1 </b> doesn't exist anymore! Operation aborted!")
            .arg(curve));
    boxCurve->clear();
    boxCurve->addItems(curvesList);
    return;
  }

  if (!validInitialValues()) return;

  QString from = boxFrom->text().toLower();
  QString to = boxTo->text().toLower();
  QString tolerance = boxTolerance->text().toLower();
  double start, end, eps;
  try {
    MyParser parser;
    parser.SetExpr(CONFS(from).toUtf8().constData());
    start = parser.Eval();
  } catch (mu::ParserError &e) {
    QMessageBox::critical(this, tr("Start limit error"),
                          QString::fromStdString(e.GetMsg()));
    boxFrom->setFocus();
    return;
  }

  try {
    MyParser parser;
    parser.SetExpr(CONFS(to).toUtf8().constData());
    end = parser.Eval();
  } catch (mu::ParserError &e) {
    QMessageBox::critical(this, tr("End limit error"),
                          QString::fromStdString(e.GetMsg()));
    boxTo->setFocus();
    return;
  }

  if (start >= end) {
    QMessageBox::critical(
        app_, tr("Input error"),
        tr("Please enter x limits that satisfy: from < end!"));
    boxTo->setFocus();
    return;
  }

  try {
    MyParser parser;
    parser.SetExpr(CONFS(tolerance).toUtf8().constData());
    eps = parser.Eval();
  } catch (mu::ParserError &e) {
    QMessageBox::critical(0, tr("Tolerance input error"),
                          QString::fromStdString(e.GetMsg()));
    boxTolerance->setFocus();
    return;
  }

  if (eps < 0 || eps >= 1) {
    QMessageBox::critical(
        app_, tr("Tolerance input error"),
        tr("The tolerance value must be positive and less than 1!"));
    boxTolerance->setFocus();
    return;
  }

  int i, n = 0, rows = boxParams->rowCount();
  if (!boxParams->isColumnHidden(2)) {
    for (i = 0; i < rows; i++) {  // count the non-constant parameters
      QCheckBox *cb = qobject_cast<QCheckBox *>(boxParams->cellWidget(i, 2));
      if (!cb->isChecked()) n++;
    }
  } else
    n = rows;

  QStringList parameters;
  double *paramsInit = new double[static_cast<size_t>(n)];
  QString formula;

  // recursively define variables for user functions used in formula
  bool found_uf;
  do {
    found_uf = false;
    for (i = 0; i < d_user_function_names.count(); i++)
      if (boxFunction->toPlainText().contains(d_user_function_names[i])) {
        QStringList l = d_user_functions[i].split("=");
        formula += QString("%1=%2\n").arg(d_user_function_names[i]).arg(l[1]);
        found_uf = true;
      }
  } while (found_uf);
  formula += boxFunction->toPlainText();

  // define variables for builtin functions used in formula
  for (i = 0; i < d_built_in_function_names.count(); i++)
    if (formula.contains(d_built_in_function_names[i]))
      formula.prepend(QString("%1=%2\n")
                          .arg(d_built_in_function_names[i])
                          .arg(d_built_in_functions[i]));

  if (!boxParams->isColumnHidden(2)) {
    int j = 0;
    for (i = 0; i < rows; i++) {
      QCheckBox *cb = qobject_cast<QCheckBox *>(boxParams->cellWidget(i, 2));
      if (!cb->isChecked()) {
        paramsInit[j] = QLocale().toDouble(boxParams->item(i, 1)->text());
        parameters << boxParams->item(i, 0)->text();
        j++;
      } else
        formula.prepend(QString("%1=%2\n")
                            .arg(boxParams->item(i, 0)->text())
                            .arg(CONFS(boxParams->item(i, 1)->text())));
    }
  } else {
    for (i = 0; i < n; i++) {
      paramsInit[i] = QLocale().toDouble(boxParams->item(i, 1)->text());
      parameters << boxParams->item(i, 0)->text();
    }
  }

  if (d_fitter) {
    delete d_fitter;
    d_fitter = nullptr;
  }

  if (boxUseBuiltIn->isChecked() && categoryBox->currentRow() == 1)
    fitBuiltInFunction(funcBox->currentItem()->text(), paramsInit);
  else if (boxUseBuiltIn->isChecked() && categoryBox->currentRow() == 3) {
    d_fitter = new PluginFit(app_, axisrect_);
    if (!qobject_cast<PluginFit *>(d_fitter)->load(
            d_plugin_files_list[funcBox->currentRow()])) {
      d_fitter = nullptr;
      return;
    }
    d_fitter->setInitialGuesses(paramsInit);
  } else {
    d_fitter = new NonLinearFit(app_, axisrect_);
    ((NonLinearFit *)d_fitter)->setParametersList(parameters);
    ((NonLinearFit *)d_fitter)->setFormula(formula);
    d_fitter->setInitialGuesses(paramsInit);
  }
  delete[] paramsInit;

  PlotData::AssociatedData *associateddata;
  associateddata = PlotColumns::getassociateddatafromstring(axisrect_, curve);
  if (!associateddata) return;

  if (!d_fitter->setDataFromCurve(associateddata, start, end) ||
      !d_fitter->setYErrorSource(
          (Fit::ErrorSource)boxYErrorSource->currentIndex(),
          tableNamesBox->currentText() + "_" + colNamesBox->currentText())) {
    delete d_fitter;
    d_fitter = nullptr;
    return;
  }

  d_fitter->setTolerance(eps);
  d_fitter->setAlgorithm((Fit::Algorithm)boxAlgorithm->currentIndex());
  d_fitter->setColor(boxColor->currentIndex());
  d_fitter->generateFunction(generatePointsBtn->isChecked(),
                             generatePointsBox->value());
  d_fitter->setMaximumIterations(boxPoints->value());
  d_fitter->scaleErrors(scaleErrorsBox->isChecked());

  if (d_fitter->objectName() == tr("MultiPeak") &&
      qobject_cast<MultiPeakFit *>(d_fitter)->peaks() > 1) {
    qobject_cast<MultiPeakFit *>(d_fitter)->enablePeakCurves(
        app_->generatePeakCurves);
    qobject_cast<MultiPeakFit *>(d_fitter)->setPeakCurvesColor(
        app_->peakCurvesColor);
  }

  d_fitter->fit();
  double *res = d_fitter->results();
  if (!boxParams->isColumnHidden(2)) {
    int j = 0;
    for (i = 0; i < rows; i++) {
      QCheckBox *cb = qobject_cast<QCheckBox *>(boxParams->cellWidget(i, 2));
      if (!cb->isChecked())
        boxParams->item(i, 1)->setText(
            QLocale().toString(res[j++], 'g', boxPrecision->value()));
    }
  } else {
    for (i = 0; i < rows; i++)
      boxParams->item(i, 1)->setText(
          QLocale().toString(res[i], 'g', boxPrecision->value()));
  }
}

void FitDialog::fitBuiltInFunction(const QString &function, double *initVal) {
  if (function == "ExpDecay1") {
    initVal[1] = 1 / initVal[1];
    d_fitter = new ExponentialFit(app_, axisrect_);
  } else if (function == "ExpGrowth") {
    initVal[1] = -1 / initVal[1];
    d_fitter = new ExponentialFit(app_, axisrect_, true);
  } else if (function == "ExpDecay2") {
    initVal[1] = 1 / initVal[1];
    initVal[3] = 1 / initVal[3];
    d_fitter = new TwoExpFit(app_, axisrect_);
  } else if (function == "ExpDecay3") {
    initVal[1] = 1 / initVal[1];
    initVal[3] = 1 / initVal[3];
    initVal[5] = 1 / initVal[5];
    d_fitter = new ThreeExpFit(app_, axisrect_);
  } else if (function == "Boltzmann")
    d_fitter = new SigmoidalFit(app_, axisrect_);
  else if (function == "GaussAmp")
    d_fitter = new GaussAmpFit(app_, axisrect_);
  else if (function == "Gauss")
    d_fitter = new MultiPeakFit(app_, axisrect_, MultiPeakFit::Gauss,
                                polynomOrderBox->value());
  else if (function == "Lorentz")
    d_fitter = new MultiPeakFit(app_, axisrect_, MultiPeakFit::Lorentz,
                                polynomOrderBox->value());
  else if (function == "Polynomial")
    d_fitter = new PolynomialFit(app_, axisrect_, polynomOrderBox->value());

  if (function != "Polynomial") d_fitter->setInitialGuesses(initVal);
}

bool FitDialog::containsUserFunctionName(const QString &function) {
  foreach (QString fn, d_user_function_names)
    if (!fn.isEmpty() && function.contains(fn)) return true;

  return false;
}

bool FitDialog::validInitialValues() {
  for (int i = 0; i < boxParams->rowCount(); i++) {
    if (boxParams->item(i, 1)->text().isEmpty()) {
      QMessageBox::critical(
          app_, tr("Input error"),
          tr("Please enter initial guesses for your parameters!"));
      boxParams->setCurrentCell(i, 1);
      return false;
    }

    try {
      MyParser parser;
      parser.SetExpr(CONFS(boxParams->item(i, 1)->text()).toUtf8().constData());
      parser.Eval();
    } catch (mu::ParserError &e) {
      QMessageBox::critical(app_, tr("Start limit error"),
                            QString::fromStdString(e.GetMsg()));
      boxParams->setCurrentCell(i, 1);
      return false;
    }
  }
  return true;
}

void FitDialog::changeDataRange() {
  double start = xmin_;
  double end = xmax_;
  boxFrom->setText(QString::number(std::min(start, end), 'g', 15));
  boxTo->setText(QString::number(std::max(start, end), 'g', 15));
}

void FitDialog::setSrcTables(QList<QMdiSubWindow *> *tables) {
  if (tables->isEmpty()) {
    tableNamesBox->addItem(tr("No data tables"));
    colNamesBox->addItem(tr("No data tables"));
    return;
  }

  d_src_table = tables;
  tableNamesBox->clear();
  foreach (QWidget *i, *d_src_table)
    tableNamesBox->addItem(i->objectName());

  tableNamesBox->setCurrentIndex(tableNamesBox->findText(
      boxCurve->currentText().split("_", QString::SkipEmptyParts)[0]));
  selectSrcTable(tableNamesBox->currentIndex());
}

void FitDialog::selectSrcTable(int tabnr) {
  colNamesBox->clear();

  if (tabnr >= 0 && tabnr < d_src_table->count()) {
    Table *t = qobject_cast<Table *>(d_src_table->at(tabnr));
    if (t) colNamesBox->addItems(t->colNames());
  }
}

void FitDialog::yErrorSourceChanged(int index) {
  if (index == Fit::CustomErrors) {
    tableNamesBox->setEnabled(true);
    colNamesBox->setEnabled(true);
  } else {
    tableNamesBox->setEnabled(false);
    colNamesBox->setEnabled(false);
  }
}

FitDialog::~FitDialog() {}

void FitDialog::closeEvent(QCloseEvent *e) { e->accept(); }

void FitDialog::enableApplyChanges(int) { btnApply->setEnabled(true); }

void FitDialog::deleteFitCurves() {
  // d_graph->deleteFitCurves();
  boxCurve->clear();
  boxCurve->addItems(PlotColumns::getstringlistfromassociateddata(axisrect_));
}

void FitDialog::resetFunction() {
  boxName->clear();
  boxParam->clear();
  editBox->clear();
}
