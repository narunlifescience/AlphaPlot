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
#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Bar2D.h"
#include "2Dplot/Curve2D.h"
#include "2Dplot/DataManager2D.h"
#include "2Dplot/ErrorBar2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "ApplicationWindow.h"
#include "Table.h"
#include "core/column/Column.h"

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

ErrDialog::ErrDialog(QWidget *parent, AxisRect2D *axisrect, Qt::WindowFlags fl)
    : QDialog(parent, fl),
      axisrect_(axisrect),
      app_(qobject_cast<ApplicationWindow *>(parent->parent())) {
  Q_ASSERT(axisrect);
  Q_ASSERT(app_);
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
  connect(buttonAdd, SIGNAL(clicked()), this, SLOT(add()));
  connect(buttonCancel, SIGNAL(clicked()), this, SLOT(reject()));
  connect(percentBox, SIGNAL(toggled(bool)), valueBox, SLOT(setEnabled(bool)));
  connect(columnBox, SIGNAL(toggled(bool)), tableNamesBox,
          SLOT(setEnabled(bool)));
  connect(buttonGroup2, SIGNAL(buttonClicked(int)), this,
          SLOT(errorColumnNames()));
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
    if (curve->getplottype_curveplot() == Graph2DCommon::PlotType::Associated) {
      DataBlockCurve *curvedata = curve->getdatablock_curveplot();
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
  if (columnBox->isChecked()) {
  } else if (percentBox->isChecked()) {
    PlotData::AssociatedData selectplotdata =
        plotted_columns_.at(nameLabel->currentIndex());
    QVector<LineSpecial2D *> lslist = axisrect_->getLsVec();
    QVector<Curve2D *> curvelist = axisrect_->getCurveVec();
    QVector<Bar2D *> barlist = axisrect_->getBarVec();
    foreach (LineSpecial2D *ls, lslist) {
      DataBlockGraph *graphdata = ls->getdatablock_lsplot();

      if (selectplotdata.table == graphdata->gettable() &&
          selectplotdata.xcol == graphdata->getxcolumn() &&
          selectplotdata.ycol == graphdata->getycolumn()) {
        Column *errors = new Column("1", AlphaPlot::Numeric);
        (xErrBox->isChecked()) ? errors->setPlotDesignation(AlphaPlot::xErr)
                               : errors->setPlotDesignation(AlphaPlot::yErr);
        selectplotdata.table->d_future_table->addChild(errors);
        double fraction = fraction = valueBox->text().toDouble() / 100.0;
        Column *col;
        (xErrBox->isChecked()) ? col = selectplotdata.xcol
                               : col = selectplotdata.ycol;
        for (int i = 0; i < col->rowCount(); i++)
          errors->setValueAt(i, col->valueAt(i) * fraction);

        (xErrBox->isChecked()) ? ls->setXerrorBar(selectplotdata.table, errors,
                                                  0, errors->rowCount() - 1)
                               : ls->setYerrorBar(selectplotdata.table, errors,
                                                  0, errors->rowCount() - 1);
      }
    }
  } else if (standardBox->isChecked()) {
  }

  /*  int direction = -1;
    if (xErrBox->isChecked())
      direction = 0;
    else
      direction = 1;

    if (columnBox->isChecked())
      emit options(nameLabel->currentText(), tableNamesBox->currentText(),
                   direction);
    else {
      int type;
      if (percentBox->isChecked())
        type = 0;
      else
        type = 1;

      emit options(nameLabel->currentText(), type, valueBox->text(), direction);
    }*/

  close();
}

ErrDialog::~ErrDialog() {}
