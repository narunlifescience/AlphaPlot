#include "Function2DDialog.h"
#include "scripting/MyParser.h"
#include "ui_Function2DDialog.h"
#include "../AxisRect2D.h"
#include "ApplicationWindow.h"

#include <QMessageBox>

Function2DDialog::Function2DDialog(QWidget *parent, AxisRect2D *axisrect)
    : QDialog(parent), ui_(new Ui_Function2DDialog), axisrect_(axisrect) {
  ui_->setupUi(this);
  setSizeGripEnabled(true);

  // set ranges
  ui_->normpointsSpinBox->setRange(2, 1000000);
  ui_->normpointsSpinBox->setSingleStep(100);
  ui_->parampointsSpinBox->setRange(2, 1000000);
  ui_->parampointsSpinBox->setSingleStep(100);
  ui_->polarpointsSpinBox->setRange(2, 1000000);
  ui_->polarpointsSpinBox->setSingleStep(100);

  // set default values
  ui_->normfofxTextEdit->setMinimumWidth(350);
  ui_->normfromxLineEdit->setText("0");
  ui_->normtoxLineEdit->setText("1");
  ui_->normpointsSpinBox->setValue(100);
  ui_->paramparameterLineEdit->setText("m");
  ui_->paramfromLineEdit->setText("0");
  ui_->paramtoLineEdit->setText("1");
  ui_->parampointsSpinBox->setValue(100);
  ui_->polarparameterLineEdit->setText("t");
  ui_->polarfromLineEdit->setText("0");
  ui_->polartoLineEdit->setText("pi");
  ui_->polarpointsSpinBox->setValue(100);
  ui_->dialogButtonBox->button(QDialogButtonBox::Ok)->setDefault(true);
  setFocusProxy(ui_->normfofxTextEdit);

  connect(ui_->functionComboBox, SIGNAL(activated(int)), this,
          SLOT(raiseWidget(int)));
  connect(ui_->dialogButtonBox->button(QDialogButtonBox::Apply),
          SIGNAL(clicked()), this, SLOT(apply()));
  connect(ui_->dialogButtonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()),
          this, SLOT(accept()));
  connect(ui_->dialogButtonBox->button(QDialogButtonBox::Cancel),
          SIGNAL(clicked()), this, SLOT(reject()));
  connect(ui_->clearfunction_pushButton, SIGNAL(clicked()), this,
          SLOT(clearList()));
}

Function2DDialog::~Function2DDialog() { delete ui_; }

void Function2DDialog::raiseWidget(const int index) const {
  (index == 0) ? ui_->clearfunction_pushButton->setText(tr("Clear list"))
               : ui_->clearfunction_pushButton->setText(tr("Clear Function"));
  ui_->functionStackedWidget->setCurrentIndex(index);
}

bool Function2DDialog::apply() {
  bool result = true;
  switch (ui_->functionComboBox->currentIndex()) {
    case 0:
      result = acceptFunction();
      break;

    case 1:
      result = acceptParametric();
      break;

    case 2:
      // result = acceptPolar();
      break;
  }
  return result;
}

void Function2DDialog::accept() {
  if (apply()) {
    close();
  }
}

bool Function2DDialog::acceptFunction() {
  QString from = ui_->normfromxLineEdit->text().toLower();
  QString to = ui_->normtoxLineEdit->text().toLower();
  QString points = ui_->normpointsSpinBox->text().toLower();

  double start, end;
  try {
    MyParser parser;
    parser.SetExpr(from.toAscii().constData());
    start = parser.Eval();
  } catch (mu::ParserError &error) {
    QMessageBox::critical(0, tr("Start limit error"),
                          QString::fromStdString(error.GetMsg()));
    ui_->normfromxLineEdit->setFocus();
    return false;
  }
  try {
    MyParser parser;
    parser.SetExpr(to.toAscii().constData());
    end = parser.Eval();
  } catch (mu::ParserError &error) {
    QMessageBox::critical(0, tr("End limit error"),
                          QString::fromStdString(error.GetMsg()));
    ui_->normtoxLineEdit->setFocus();
    return false;
  }

  if (start >= end) {
    QMessageBox::critical(
        0, tr("Input error"),
        tr("Please enter x limits that satisfy: from < end!"));
    ui_->normtoxLineEdit->setFocus();
    return false;
  }

  QString formula =
      ui_->normfofxTextEdit->text().replace(QChar::ParagraphSeparator, "\n");

  // Collecting all the information
  int type = ui_->functionComboBox->currentItem();
  QStringList formulas;
  QList<double> ranges;
  formulas += formula;
  ranges += start;
  ranges += end;

  bool result;
  emit updateFunctionLists(type, formulas);
  /*if(axisrect_) {
      ApplicationWindow *app = (ApplicationWindow *)this->parent();
      app->updateFunctionLists(type, formulas);
      app->newFunctionPlot(type, formulas, "", ranges, points.toInt());
    }*/
  /*if (!graph)
    result =
        app->newFunctionPlot(type, formulas, "x", ranges, boxPoints->value());
  else {
    if (curveID >= 0)
      result = graph->modifyFunctionCurve(app, curveID, type, formulas, "x",
                                          ranges, boxPoints->value());
    else
      result = graph->addFunctionCurve(app, type, formulas, "x", ranges,
                                       boxPoints->value());
  }*/
  if (!result) ui_->normfofxTextEdit->setFocus();
  return result;
}

bool Function2DDialog::acceptParametric() {
  QString from = ui_->paramfromLineEdit->text().toLower();
  QString to = ui_->paramtoLineEdit->text().toLower();
  QString points = ui_->parampointsSpinBox->text().toLower();

  double start, end;
  try {
    MyParser parser;
    parser.SetExpr(from.toAscii().constData());
    start = parser.Eval();
  } catch (mu::ParserError &error) {
    QMessageBox::critical(0, tr("Start limit error"),
                          QString::fromStdString(error.GetMsg()));
    ui_->paramfromLineEdit->setFocus();
    return false;
  }

  try {
    MyParser parser;
    parser.SetExpr(to.toAscii().constData());
    end = parser.Eval();
  } catch (mu::ParserError &error) {
    QMessageBox::critical(0, tr("End limit error"),
                          QString::fromStdString(error.GetMsg()));
    ui_->paramtoLineEdit->setFocus();
    return false;
  }

  if (start >= end) {
    QMessageBox::critical(
        0, tr("Input error"),
        tr("Please enter parameter limits that satisfy: from < end!"));
    ui_->paramtoLineEdit->setFocus();
    return false;
  }

  QString xformula =
      ui_->paramxLineEdit->text().replace(QChar::ParagraphSeparator, "\n");
  QString yformula =
      ui_->paramyLineEdit->text().replace(QChar::ParagraphSeparator, "\n");

  // Collecting all the information
  int type = ui_->functionComboBox->currentItem();
  QStringList formulas;
  QList<double> ranges;
  formulas += xformula;
  formulas += yformula;
  ranges += start;
  ranges += end;

  bool result;
  emit updateFunctionLists(type, formulas);
  /*if (!graph)
    result = app->newFunctionPlot(type, formulas, boxParameter->text(), ranges,
                                  boxParPoints->value());
  else {
    if (curveID >= 0)
      result = graph->modifyFunctionCurve(app, curveID, type, formulas,
                                          boxParameter->text(), ranges,
                                          boxParPoints->value());
    else
      result =
          graph->addFunctionCurve(app, type, formulas, boxParameter->text(),
                                  ranges, boxParPoints->value());
  }*/
  if (!result) ui_->paramxLineEdit->setFocus();
  return result;
}

void Function2DDialog::clearList() {
  switch (ui_->functionComboBox->currentIndex()) {
    case 0:
      ui_->normfofxTextEdit->clear();
      break;

    case 1:
      ui_->paramxLineEdit->clear();
      ui_->paramyLineEdit->clear();
      emit clearParamFunctionsList();
      break;

    case 2:
      ui_->polarxLineEdit->clear();
      ui_->polaryLineEdit->clear();
      emit clearPolarFunctionsList();
      break;
  }
}
