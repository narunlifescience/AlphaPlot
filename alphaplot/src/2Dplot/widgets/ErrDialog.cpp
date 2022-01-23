/***************************************************************************
    File                 : ErrDialog.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Add error bars dialog

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
#include "ErrDialog.h"

#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMdiSubWindow>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>

#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Bar2D.h"
#include "2Dplot/Curve2D.h"
#include "2Dplot/DataManager2D.h"
#include "2Dplot/ErrorBar2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "ApplicationWindow.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "core/column/Column.h"

ErrDialog::ErrDialog(QWidget *parent, AxisRect2D *axisrect, Qt::WindowFlags fl)
    : QDialog(parent, fl),
      axisrect_(axisrect),
      app_(qobject_cast<ApplicationWindow *>(parent->parent())) {
  Q_ASSERT(axisrect);
  Q_ASSERT(app_);
  setWindowIcon(IconLoader::load("graph-y-error", IconLoader::LightDark));
  setFocusPolicy(Qt::StrongFocus);
  setSizeGripEnabled(true);

  QVBoxLayout *vbox1 = new QVBoxLayout();
  vbox1->setSpacing(5);

  QHBoxLayout *hbox1 = new QHBoxLayout();
  vbox1->addLayout(hbox1);

  textLabel1 = new QLabel();
  hbox1->addWidget(textLabel1);

  nameLabel = new QComboBox();
  hbox1->addWidget(nameLabel);

  groupBox1 = new QGroupBox(QString(tr("Source of errors")));
  QGridLayout *gridLayout = new QGridLayout(groupBox1);
  vbox1->addWidget(groupBox1);

  buttonGroup1 = new QButtonGroup();
  buttonGroup1->setExclusive(true);

  columnBox = new QRadioButton();
  columnBox->setChecked(true);
  buttonGroup1->addButton(columnBox);
  gridLayout->addWidget(columnBox, 0, 0);

  tableNamesBox = new QComboBox();

  QHBoxLayout *comboBoxes = new QHBoxLayout();
  comboBoxes->addWidget(tableNamesBox);

  gridLayout->addLayout(comboBoxes, 0, 1);

  percentBox = new QRadioButton();
  buttonGroup1->addButton(percentBox);
  gridLayout->addWidget(percentBox, 1, 0);

  valueBox = new QLineEdit();
  valueBox->setAlignment(Qt::AlignHCenter);
  valueBox->setEnabled(false);
  gridLayout->addWidget(valueBox, 1, 1);

  standardBox = new QRadioButton();
  buttonGroup1->addButton(standardBox);
  gridLayout->addWidget(standardBox, 2, 0);

  groupBox3 = new QGroupBox(QString());
  vbox1->addWidget(groupBox3);
  QHBoxLayout *hbox2 = new QHBoxLayout(groupBox3);

  buttonGroup2 = new QButtonGroup();
  buttonGroup2->setExclusive(true);

  xErrBox = new QRadioButton();
  buttonGroup2->addButton(xErrBox);
  hbox2->addWidget(xErrBox);

  yErrBox = new QRadioButton();
  buttonGroup2->addButton(yErrBox);
  hbox2->addWidget(yErrBox);
  yErrBox->setChecked(true);

  QVBoxLayout *vbox2 = new QVBoxLayout();
  buttonAdd = new QPushButton();
  buttonAdd->setDefault(true);
  vbox2->addWidget(buttonAdd);

  buttonCancel = new QPushButton();
  vbox2->addWidget(buttonCancel);

  vbox2->addStretch(1);

  QHBoxLayout *hlayout1 = new QHBoxLayout(this);
  hlayout1->addLayout(vbox1);
  hlayout1->addLayout(vbox2);

  setWindowTitle(tr("Error Bars"));
  xErrBox->setText(tr("&X Error Bars"));
  buttonAdd->setText(tr("&Add"));
  textLabel1->setText(tr("Add Error Bars to"));
  groupBox1->setTitle(tr("Source of errors"));
  percentBox->setText(tr("Percent of data (%)"));
  valueBox->setText(tr("5"));
  standardBox->setText(tr("Standard Deviation of Data"));
  yErrBox->setText(tr("&Y Error Bars"));
  buttonCancel->setText(tr("&Close"));
  columnBox->setText("Existing column");

  plotNames();
  errorColumnNames();

  // signals and slots connections
  connect(buttonAdd, &QPushButton::clicked, this, &ErrDialog::add);
  connect(buttonCancel, &QPushButton::clicked, this, &ErrDialog::reject);
  connect(percentBox, &QRadioButton::toggled, valueBox, &QLineEdit::setEnabled);
  connect(columnBox, &QRadioButton::toggled, tableNamesBox,
          &QComboBox::setEnabled);
  connect(buttonGroup2, &QButtonGroup::idClicked, this,
          &ErrDialog::errorColumnNames);
}

void ErrDialog::plotNames() {
  nameLabel->clear();
  plotted_columns_.clear();
  QVector<LineSpecial2D *> lslist = axisrect_->getLsVec();
  QVector<Curve2D *> curvelist = axisrect_->getCurveVec();
  QVector<Bar2D *> barlist = axisrect_->getBarVec();
  foreach (LineSpecial2D *ls, lslist) {
    DataBlockGraph *graphdata = ls->getdatablock_lsplot();
    PlotData::AssociatedData plotdata;
    plotdata.table = graphdata->gettable();
    plotdata.xcol = graphdata->getxcolumn();
    plotdata.ycol = graphdata->getycolumn();
    plotted_columns_ << plotdata;
    nameLabel->addItem(plotdata.table->name() + "_" + plotdata.ycol->name() +
                       "[" + QString::number(graphdata->getfrom() + 1) + ":" +
                       QString::number(graphdata->getto() + 1) + "]");
  }
  foreach (Curve2D *curve, curvelist) {
    if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Associated) {
      DataBlockCurve *curvedata = curve->getdatablock_cplot();
      PlotData::AssociatedData plotdata;
      plotdata.table = curvedata->gettable();
      plotdata.xcol = curvedata->getxcolumn();
      plotdata.ycol = curvedata->getycolumn();
      plotted_columns_ << plotdata;
      nameLabel->addItem(plotdata.table->name() + "_" + plotdata.ycol->name() +
                         "[" + QString::number(curvedata->getfrom() + 1) + ":" +
                         QString::number(curvedata->getto() + 1) + "]");
    }
  }
  foreach (Bar2D *bar, barlist) {
    if (!bar->ishistogram_barplot()) {
      DataBlockBar *bardata = bar->getdatablock_barplot();
      PlotData::AssociatedData plotdata;
      plotdata.table = bardata->gettable();
      plotdata.xcol = bardata->getxcolumn();
      plotdata.ycol = bardata->getycolumn();
      plotted_columns_ << plotdata;
      nameLabel->addItem(plotdata.table->name() + "_" + plotdata.ycol->name() +
                         "[" + QString::number(bardata->getfrom() + 1) + ":" +
                         QString::number(bardata->getto() + 1) + "]");
    }
  }
}

void ErrDialog::errorColumnNames() {
  error_columns_.clear();
  tableNamesBox->clear();
  if (xErrBox->isChecked()) {
    error_columns_ = app_->columnList(AlphaPlot::xErr);
  } else {
    error_columns_ = app_->columnList(AlphaPlot::yErr);
  }
  QPair<Table *, Column *> columnpair;

  foreach (columnpair, error_columns_) {
    tableNamesBox->addItem(columnpair.first->name() + "_" +
                           columnpair.second->name());
  }
}

void ErrDialog::add() {
  if (nameLabel->count() == 0 ||
      nameLabel->currentText().trimmed().length() == 0) {
    QMessageBox::critical(app_, tr("Error"),
                          tr("No compatible plot(s) available!"));
    return;
  }
  QVector<LineSpecial2D *> lslist = axisrect_->getLsVec();
  QVector<Curve2D *> curvelist = axisrect_->getCurveVec();
  QVector<Bar2D *> barlist = axisrect_->getBarVec();
  PlotData::AssociatedData selectplotdata =
      plotted_columns_.at(nameLabel->currentIndex());
  if (columnBox->isChecked()) {
    foreach (LineSpecial2D *ls, lslist) {
      DataBlockGraph *graphdata = ls->getdatablock_lsplot();
      if (selectplotdata.table == graphdata->gettable() &&
          selectplotdata.xcol == graphdata->getxcolumn() &&
          selectplotdata.ycol == graphdata->getycolumn()) {
        if (xErrBox->isChecked()) {
          if (selectplotdata.xcol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        } else {
          if (selectplotdata.ycol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        }
        Column *errors = getErrorCol(selectplotdata.table);
        if (!errors) return;
        (xErrBox->isChecked()) ? ls->setXerrorBar(selectplotdata.table, errors,
                                                  0, errors->rowCount() - 1)
                               : ls->setYerrorBar(selectplotdata.table, errors,
                                                  0, errors->rowCount() - 1);
      }
    }
    foreach (Curve2D *curve, curvelist) {
      DataBlockCurve *curvedata = curve->getdatablock_cplot();
      if (selectplotdata.table == curvedata->gettable() &&
          selectplotdata.xcol == curvedata->getxcolumn() &&
          selectplotdata.ycol == curvedata->getycolumn()) {
        if (xErrBox->isChecked()) {
          if (selectplotdata.xcol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        } else {
          if (selectplotdata.ycol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        }
        Column *errors = getErrorCol(selectplotdata.table);
        if (!errors) return;
        (xErrBox->isChecked())
            ? curve->setXerrorBar(selectplotdata.table, errors, 0,
                                  errors->rowCount() - 1)
            : curve->setYerrorBar(selectplotdata.table, errors, 0,
                                  errors->rowCount() - 1);
      }
    }
    foreach (Bar2D *bar, barlist) {
      DataBlockBar *bardata = bar->getdatablock_barplot();
      if (selectplotdata.table == bardata->gettable() &&
          selectplotdata.xcol == bardata->getxcolumn() &&
          selectplotdata.ycol == bardata->getycolumn()) {
        if (xErrBox->isChecked()) {
          if (selectplotdata.xcol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        } else {
          if (selectplotdata.ycol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        }
        Column *errors = getErrorCol(selectplotdata.table);
        if (!errors) return;
        (xErrBox->isChecked()) ? bar->setXerrorBar(selectplotdata.table, errors,
                                                   0, errors->rowCount() - 1)
                               : bar->setYerrorBar(selectplotdata.table, errors,
                                                   0, errors->rowCount() - 1);
      }
    }
  } else if (percentBox->isChecked() || standardBox->isChecked()) {
    foreach (LineSpecial2D *ls, lslist) {
      DataBlockGraph *graphdata = ls->getdatablock_lsplot();

      if (selectplotdata.table == graphdata->gettable() &&
          selectplotdata.xcol == graphdata->getxcolumn() &&
          selectplotdata.ycol == graphdata->getycolumn()) {
        if (xErrBox->isChecked()) {
          if (selectplotdata.xcol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        } else {
          if (selectplotdata.ycol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        }
        Column *errors = new Column("1", AlphaPlot::Numeric);
        (xErrBox->isChecked()) ? errors->setPlotDesignation(AlphaPlot::xErr)
                               : errors->setPlotDesignation(AlphaPlot::yErr);
        selectplotdata.table->d_future_table->addChild(errors);
        double fraction = valueBox->text().toDouble() / 100.0;
        Column *col;
        (xErrBox->isChecked()) ? col = selectplotdata.xcol
                               : col = selectplotdata.ycol;
        int rows = col->rowCount();
        if (percentBox->isChecked()) {
          for (int i = 0; i < rows; i++)
            errors->setValueAt(i, col->valueAt(i) * fraction);
        } else if (standardBox->isChecked()) {
          double average = 0.0;
          double dev = 0.0;
          for (int i = 0; i < rows; i++) average += col->valueAt(i);
          average /= rows;
          for (int i = 0; i < rows; i++)
            dev += pow(col->valueAt(i) - average, 2);
          dev = sqrt(dev / rows);
          for (int i = 0; i < rows; i++) errors->setValueAt(i, dev);
        }

        (xErrBox->isChecked()) ? ls->setXerrorBar(selectplotdata.table, errors,
                                                  0, errors->rowCount() - 1)
                               : ls->setYerrorBar(selectplotdata.table, errors,
                                                  0, errors->rowCount() - 1);
      }
    }
    foreach (Curve2D *curve, curvelist) {
      DataBlockCurve *curvedata = curve->getdatablock_cplot();

      if (selectplotdata.table == curvedata->gettable() &&
          selectplotdata.xcol == curvedata->getxcolumn() &&
          selectplotdata.ycol == curvedata->getycolumn()) {
        if (xErrBox->isChecked()) {
          if (selectplotdata.xcol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        } else {
          if (selectplotdata.ycol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        }
        Column *errors = new Column("1", AlphaPlot::Numeric);
        (xErrBox->isChecked()) ? errors->setPlotDesignation(AlphaPlot::xErr)
                               : errors->setPlotDesignation(AlphaPlot::yErr);
        selectplotdata.table->d_future_table->addChild(errors);
        double fraction = valueBox->text().toDouble() / 100.0;
        Column *col;
        (xErrBox->isChecked()) ? col = selectplotdata.xcol
                               : col = selectplotdata.ycol;
        int rows = col->rowCount();
        if (percentBox->isChecked()) {
          for (int i = 0; i < rows; i++)
            errors->setValueAt(i, col->valueAt(i) * fraction);
        } else if (standardBox->isChecked()) {
          double average = 0.0;
          double dev = 0.0;
          for (int i = 0; i < rows; i++) average += col->valueAt(i);
          average /= rows;
          for (int i = 0; i < rows; i++)
            dev += pow(col->valueAt(i) - average, 2);
          dev = sqrt(dev / rows);
          for (int i = 0; i < rows; i++) errors->setValueAt(i, dev);
        }

        (xErrBox->isChecked())
            ? curve->setXerrorBar(selectplotdata.table, errors, 0,
                                  errors->rowCount() - 1)
            : curve->setYerrorBar(selectplotdata.table, errors, 0,
                                  errors->rowCount() - 1);
      }
    }
    foreach (Bar2D *bar, barlist) {
      DataBlockBar *bardata = bar->getdatablock_barplot();

      if (selectplotdata.table == bardata->gettable() &&
          selectplotdata.xcol == bardata->getxcolumn() &&
          selectplotdata.ycol == bardata->getycolumn()) {
        if (xErrBox->isChecked()) {
          if (selectplotdata.xcol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        } else {
          if (selectplotdata.ycol->dataType() !=
              AlphaPlot::ColumnDataType::TypeDouble) {
            QMessageBox::critical(
                app_, tr("Error"),
                tr("You can only add error bars to numeric column(s)!"));
            return;
          }
        }
        Column *errors = new Column("1", AlphaPlot::Numeric);
        (xErrBox->isChecked()) ? errors->setPlotDesignation(AlphaPlot::xErr)
                               : errors->setPlotDesignation(AlphaPlot::yErr);
        selectplotdata.table->d_future_table->addChild(errors);
        double fraction = valueBox->text().toDouble() / 100.0;
        Column *col;
        (xErrBox->isChecked()) ? col = selectplotdata.xcol
                               : col = selectplotdata.ycol;
        int rows = col->rowCount();
        if (percentBox->isChecked()) {
          for (int i = 0; i < rows; i++)
            errors->setValueAt(i, col->valueAt(i) * fraction);
        } else if (standardBox->isChecked()) {
          double average = 0.0;
          double dev = 0.0;
          for (int i = 0; i < rows; i++) average += col->valueAt(i);
          average /= rows;
          for (int i = 0; i < rows; i++)
            dev += pow(col->valueAt(i) - average, 2);
          dev = sqrt(dev / rows);
          for (int i = 0; i < rows; i++) errors->setValueAt(i, dev);
        }

        (xErrBox->isChecked()) ? bar->setXerrorBar(selectplotdata.table, errors,
                                                   0, errors->rowCount() - 1)
                               : bar->setYerrorBar(selectplotdata.table, errors,
                                                   0, errors->rowCount() - 1);
      }
    }
  }
  axisrect_->parentPlot()->replot(
      QCustomPlot::RefreshPriority::rpQueuedRefresh);
  app_->modified();
  close();
}

ErrDialog::~ErrDialog() {}

Column *ErrDialog::getErrorCol(Table *parent) {
  if (tableNamesBox->count() < 1) {
    QMessageBox::critical(app_, tr("Error"), tr("No Error Colums available!"));
    return nullptr;
  }
  QStringList tablist = tableNamesBox->currentText().split("_");
  Q_ASSERT(tablist.count() == 2);
  Table *table = app_->table(tablist.at(0));
  if (!table) {
    QMessageBox::critical(app_, tr("Error"),
                          tr("Unable to find Table %1!").arg(tablist.at(0)));
    return nullptr;
  }
  if (table->numRows() != parent->numRows()) {
    QMessageBox::critical(
        app_, tr("Error"),
        tr("The selected columns have different numbers of rows!"));
    return nullptr;
  }
  int columnindex = table->colIndex(tablist.at(1));
  if (columnindex < 0) {
    QMessageBox::critical(app_, tr("Error"),
                          tr("Unable to locate the selected column name!"));
    return nullptr;
  }
  Column *errors = table->column(columnindex);
  if (!errors) {
    QMessageBox::critical(app_, tr("Error"),
                          tr("Unable to get Error column pointer!"));
    return nullptr;
  }

  if (errors->dataType() != AlphaPlot::ColumnDataType::TypeDouble) {
    QMessageBox::critical(app_, tr("Error"),
                          tr("You can only add numeric error column(s)!"));
    return nullptr;
  }

  return errors;
}
