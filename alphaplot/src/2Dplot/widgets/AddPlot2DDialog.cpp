#include "AddPlot2DDialog.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QKeySequence>
#include <QLabel>
#include <QLayout>
#include <QList>
#include <QListWidget>
#include <QMenu>
#include <QMessageBox>
#include <QPair>
#include <QPixmap>
#include <QPushButton>
#include <QShortcut>
#include <QtGlobal>

#include "../DataManager2D.h"
#include "2Dplot/Bar2D.h"
#include "2Dplot/Curve2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "ApplicationWindow.h"
#include "Folder.h"
#include "Matrix.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "core/column/Column.h"

AddPlot2DDialog::AddPlot2DDialog(QWidget *parent, AxisRect2D *axisrect,
                                 Qt::WindowFlags fl)
    : QDialog(parent, fl),
      app_(qobject_cast<ApplicationWindow *>(parent->parent())),
      axisrect_(axisrect) {
  Q_ASSERT(app_);
  Q_ASSERT(axisrect_);

  setWindowTitle(tr("Add 2D Plots"));
  setSizeGripEnabled(true);
  setFocus();

  QHBoxLayout *hl = new QHBoxLayout();

  hl->addWidget(new QLabel(tr("New Plot Type")));
  boxStyle_ = new QComboBox();
  boxStyle_->addItem(IconLoader::load("graph2d-scatter", IconLoader::LightDark),
                     tr(" Scatter"));
  boxStyle_->addItem(IconLoader::load("graph2d-line", IconLoader::LightDark),
                     tr(" Line"));
  boxStyle_->addItem(
      IconLoader::load("graph2d-line-scatter", IconLoader::LightDark),
      tr(" Line + Symbol"));
  boxStyle_->addItem(
      IconLoader::load("graph2d-vertical-drop", IconLoader::LightDark),
      tr(" Vertical drop lines"));
  boxStyle_->addItem(IconLoader::load("graph2d-spline", IconLoader::LightDark),
                     tr(" Spline"));
  boxStyle_->addItem(
      IconLoader::load("graph2d-vertical-step", IconLoader::LightDark),
      tr(" Vertical steps"));
  boxStyle_->addItem(
      IconLoader::load("graph2d-horizontal-step", IconLoader::LightDark),
      tr(" Horizontal steps"));
  boxStyle_->addItem(IconLoader::load("graph2d-area", IconLoader::LightDark),
                     tr(" Area"));
  boxStyle_->addItem(
      IconLoader::load("graph2d-vertical-bar", IconLoader::LightDark),
      tr(" Vertical Bars"));
  boxStyle_->addItem(
      IconLoader::load("graph2d-horizontal-bar", IconLoader::LightDark),
      tr(" Horizontal Bars"));
  boxStyle_->addItem(IconLoader::load("graph2d-channel", IconLoader::LightDark),
                     tr(" Channel fill"));
  hl->addWidget(boxStyle_);

  boxMatrixStyle_ = new QComboBox();
  boxMatrixStyle_->addItem(QPixmap(":/color_map.xpm"),
                           tr("Contour - Color Fill"));
  boxMatrixStyle_->addItem(QPixmap(":/contour_map.xpm"), tr("Contour Lines"));
  boxMatrixStyle_->addItem(QPixmap(":/gray_map.xpm"), tr("Gray Scale Map"));
  boxMatrixStyle_->hide();
  hl->addWidget(boxMatrixStyle_);
  hl->addStretch();

  QGridLayout *gl = new QGridLayout();
  gl->addWidget(new QLabel(tr("Available data")), 0, 0);
  gl->addWidget(new QLabel(tr("Graph contents")), 0, 2);

  available_ = new QListWidget();
  available_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  gl->addWidget(available_, 1, 0);

  QVBoxLayout *vl1 = new QVBoxLayout();
  btnAdd_ = new QPushButton();
  btnAdd_->setIcon(IconLoader::load("go-next", IconLoader::LightDark));
  vl1->addWidget(btnAdd_);

  vl1->addStretch();

  gl->addLayout(vl1, 1, 1);
  contents_ = new QListWidget();
  contents_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  gl->addWidget(contents_, 1, 2);

  QVBoxLayout *vl2 = new QVBoxLayout();
  btnAssociations_ = new QPushButton(tr("&Plot Associations..."));
  btnAssociations_->setEnabled(false);
  vl2->addWidget(btnAssociations_);

  btnEditFunction_ = new QPushButton(tr("&Edit Function..."));
  btnEditFunction_->setEnabled(false);
  vl2->addWidget(btnEditFunction_);

  btnOK_ = new QPushButton(tr("OK"));
  btnOK_->setDefault(true);
  vl2->addWidget(btnOK_);

  btnCancel_ = new QPushButton(tr("Close"));
  vl2->addWidget(btnCancel_);

  groupBox_ = new QGroupBox(tr("Row Select"));
  vl2->addWidget(groupBox_);
  rowFromLabel_ = new QLabel("From");
  rowToLabel_ = new QLabel("To");
  rowFromBox_ = new QSpinBox();
  rowToBox_ = new QSpinBox();
  QHBoxLayout *froml = new QHBoxLayout;
  froml->addWidget(rowFromLabel_);
  froml->addWidget(rowFromBox_);
  QHBoxLayout *tol = new QHBoxLayout;
  tol->addWidget(rowToLabel_);
  tol->addWidget(rowToBox_);
  QVBoxLayout *gboxl = new QVBoxLayout;
  gboxl->addLayout(froml);
  gboxl->addLayout(tol);
  groupBox_->setLayout(gboxl);

  vl2->addStretch();
  gl->addLayout(vl2, 1, 3);

  QVBoxLayout *vl3 = new QVBoxLayout(this);
  vl3->addLayout(hl);
  vl3->addLayout(gl);

  boxShowCurrentFolder_ = new QCheckBox(tr("Show current &folder only"));
  vl3->addWidget(boxShowCurrentFolder_);

  init();
  boxShowCurrentFolder_->setChecked(true);
  boxShowCurrentFolder_->setEnabled(false);

  connect(boxShowCurrentFolder_, &QCheckBox::toggled, this,
          &AddPlot2DDialog::showCurrentFolder);
  /*connect(btnAssociations, SIGNAL(clicked()), this,
          SLOT(showPlotAssociations()));
  connect(btnEditFunction, SIGNAL(clicked()), this,
  SLOT(showFunctionDialog()));*/
  connect(btnAdd_, &QPushButton::clicked, this, &AddPlot2DDialog::addCurves);
  connect(btnOK_, &QPushButton::clicked, this, &AddPlot2DDialog::close);
  connect(btnCancel_, &QPushButton::clicked, this, &AddPlot2DDialog::close);
  connect(available_, &QListWidget::itemSelectionChanged, this,
          &AddPlot2DDialog::enableAddBtn);
  connect(
      boxStyle_,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, [&](int index) {
        switch (index) {
          case 10:
            break;
          default:
            showCurrentFolder(true);
        }
      });

  QShortcut *shortcut = new QShortcut(QKeySequence("-"), this);
  connect(shortcut, &QShortcut::activated, this, &AddPlot2DDialog::addCurves);
}

void AddPlot2DDialog::showPlotAssociations() {
  int curve = contents_->currentRow();
  if (curve < 0) curve = 0;

  close();

  app_->showPlotAssociations(curve);
}

void AddPlot2DDialog::showFunctionDialog() {
  /*  int currentRow = contents->currentRow();
    close();
    app_->showFunctionDialog(d_graph, currentRow);*/
}

QSize AddPlot2DDialog::sizeHint() const { return QSize(700, 400); }

void AddPlot2DDialog::contextMenuEvent(QContextMenuEvent *e) {
  QPoint pos = available_->viewport()->mapFromGlobal(QCursor::pos());
  QRect rect = available_->visualItemRect(available_->currentItem());
  if (rect.contains(pos)) {
    QMenu contextMenu(this);
    QList<QListWidgetItem *> lst = available_->selectedItems();
    if (lst.size() > 1)
      contextMenu.addAction(tr("&Plot Selection"), this, SLOT(addCurves()));
    else if (lst.size() == 1)
      contextMenu.addAction(tr("&Plot"), this, SLOT(addCurves()));
    contextMenu.exec(QCursor::pos());
  }

  pos = contents_->viewport()->mapFromGlobal(QCursor::pos());
  rect = contents_->visualItemRect(contents_->currentItem());
  if (rect.contains(pos)) {
    QMenu contextMenu(this);
    QList<QListWidgetItem *> lst = contents_->selectedItems();
    if (lst.size() > 1)
      contextMenu.addAction(tr("&Delete Selection"), this,
                            SLOT(removeCurves()));
    else if (lst.size() == 1)
      contextMenu.addAction(tr("&Delete Curve"), this, SLOT(removeCurves()));
    contextMenu.exec(QCursor::pos());
  }

  e->accept();
}

void AddPlot2DDialog::init() {
  loadplotcontents();
  bool currentFolderOnly = app_->d_show_current_folder;
  boxShowCurrentFolder_->setChecked(currentFolderOnly);
  showCurrentFolder(currentFolderOnly);

  QStringList matrices = app_->matrixNames();
  if (!matrices.isEmpty()) {
    boxMatrixStyle_->show();
    available_->addItems(matrices);
  }

  if (!available_->count()) btnAdd_->setDisabled(true);
  enableAddBtn();
}

void AddPlot2DDialog::loadplotcontents() {
  contents_->clear();
  plotted_columns_.clear();
  QVector<LineSpecial2D *> lslist = axisrect_->getLsVec();
  QVector<Curve2D *> curvelist = axisrect_->getCurveVec();
  QVector<Bar2D *> barlist = axisrect_->getBarVec();
  QVector<QPair<LineSpecial2D *, LineSpecial2D *>> channellist =
      axisrect_->getChannelVec();
  foreach (LineSpecial2D *ls, lslist) {
    DataBlockGraph *graphdata = ls->getdatablock_lsplot();
    QPair<Column *, Column *> xycolpair = QPair<Column *, Column *>(
        graphdata->getxcolumn(), graphdata->getycolumn());
    QPair<Table *, QPair<Column *, Column *>> columnpair =
        QPair<Table *, QPair<Column *, Column *>>(graphdata->gettable(),
                                                  xycolpair);
    plotted_columns_ << columnpair;
    QListWidgetItem *item = new QListWidgetItem(
        ls->getIcon(),
        columnpair.first->name() + "_" + columnpair.second.first->name() + "_" +
            columnpair.second.second->name() + "[" +
            QString::number(graphdata->getfrom() + 1) + ":" +
            QString::number(graphdata->getto() + 1) + "]",
        contents_);
    contents_->addItem(item);
  }
  foreach (Curve2D *curve, curvelist) {
    if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Associated) {
      DataBlockCurve *curvedata = curve->getdatablock_cplot();
      QPair<Column *, Column *> xycolpair = QPair<Column *, Column *>(
          curvedata->getxcolumn(), curvedata->getycolumn());
      QPair<Table *, QPair<Column *, Column *>> columnpair =
          QPair<Table *, QPair<Column *, Column *>>(curvedata->gettable(),
                                                    xycolpair);
      plotted_columns_ << columnpair;
      QListWidgetItem *item = new QListWidgetItem(
          curve->getIcon(),
          columnpair.first->name() + "_" + columnpair.second.first->name() +
              "_" + columnpair.second.second->name() + "[" +
              QString::number(curvedata->getfrom() + 1) + ":" +
              QString::number(curvedata->getto() + 1) + "]",
          contents_);
      contents_->addItem(item);
    }
  }
  foreach (Bar2D *bar, barlist) {
    if (!bar->ishistogram_barplot()) {
      DataBlockBar *bardata = bar->getdatablock_barplot();
      QPair<Column *, Column *> xycolpair = QPair<Column *, Column *>(
          bardata->getxcolumn(), bardata->getycolumn());
      QPair<Table *, QPair<Column *, Column *>> columnpair =
          QPair<Table *, QPair<Column *, Column *>>(bardata->gettable(),
                                                    xycolpair);
      plotted_columns_ << columnpair;
      QListWidgetItem *item = new QListWidgetItem(
          bar->getIcon(),
          columnpair.first->name() + "_" + columnpair.second.first->name() +
              "_" + columnpair.second.second->name() + "[" +
              QString::number(bardata->getfrom() + 1) + ":" +
              QString::number(bardata->getto() + 1) + "]",
          contents_);
      contents_->addItem(item);
    }
  }
  for (int i = 0; i < channellist.size(); i++) {
    QPair<LineSpecial2D *, LineSpecial2D *> pair = channellist.at(i);
    // 1st
    DataBlockGraph *graphdata1 = pair.first->getdatablock_lsplot();
    QPair<Column *, Column *> xycolpair1 = QPair<Column *, Column *>(
        graphdata1->getxcolumn(), graphdata1->getycolumn());
    QPair<Table *, QPair<Column *, Column *>> columnpair1 =
        QPair<Table *, QPair<Column *, Column *>>(graphdata1->gettable(),
                                                  xycolpair1);
    // 2 nd
    DataBlockGraph *graphdata2 = pair.second->getdatablock_lsplot();
    QPair<Column *, Column *> xycolpair2 = QPair<Column *, Column *>(
        graphdata2->getxcolumn(), graphdata2->getycolumn());
    QPair<Table *, QPair<Column *, Column *>> columnpair2 =
        QPair<Table *, QPair<Column *, Column *>>(graphdata2->gettable(),
                                                  xycolpair2);
    plotted_columns_ << columnpair1 << columnpair2;
    QListWidgetItem *item = new QListWidgetItem(
        IconLoader::load("graph2d-channel", IconLoader::LightDark),
        columnpair1.first->name() + "_" + columnpair1.second.first->name() +
            "_" + columnpair1.second.second->name() + "_" +
            columnpair2.second.second->name() + "[" +
            QString::number(graphdata1->getfrom() + 1) + ":" +
            QString::number(graphdata1->getto() + 1) + "]",
        contents_);
    contents_->addItem(item);
  }
}

void AddPlot2DDialog::addCurves() {
  QPair<Table *, QPair<Column *, Column *>> pair;
  QList<QListWidgetItem *> lst = available_->selectedItems();

  if (!lst.size() || !axisrect_->getXAxes2D().count() ||
      !axisrect_->getYAxes2D().count())
    return;

  foreach (QListWidgetItem *item, lst) {
    pair = available_columns_.at(available_->row(item));
    if (pair.first->firstXCol() < 0) {
      QMessageBox::warning(
          this, tr("Error"),
          tr("No X column available in table: ") + pair.first->name() + "!");
      return;
    }
    if (rowFromBox_->value() <= 0 || rowToBox_->value() <= 0 ||
        rowToBox_->value() < rowFromBox_->value()) {
      QMessageBox::warning(
          this, tr("Error"),
          tr("From To row range error in table: ") + pair.first->name() + "!");
      return;
    }

    switch (boxStyle_->currentIndex()) {
      case 0: {
        axisrect_->addCurve2DPlot(
            AxisRect2D::LineScatterType::Scatter2D, pair.first,
            pair.second.first, pair.second.second, rowFromBox_->value() - 1,
            rowToBox_->value() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 1: {
        axisrect_->addCurve2DPlot(
            AxisRect2D::LineScatterType::Line2D, pair.first, pair.second.first,
            pair.second.second, rowFromBox_->value() - 1,
            rowToBox_->value() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 2: {
        axisrect_->addCurve2DPlot(
            AxisRect2D::LineScatterType::LineAndScatter2D, pair.first,
            pair.second.first, pair.second.second, rowFromBox_->value() - 1,
            rowToBox_->value() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 3: {
        axisrect_->addLineSpecial2DPlot(
            AxisRect2D::LineScatterSpecialType::VerticalDropLine2D, pair.first,
            pair.second.first, pair.second.second, rowFromBox_->value() - 1,
            rowToBox_->value() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 4: {
        if (rowToBox_->value() - rowFromBox_->value() < 2) {
          QMessageBox::warning(
              this, tr("Error"),
              tr("Please select three or more rows for spline plotting!"));
          return;
        }
        axisrect_->addCurve2DPlot(
            AxisRect2D::LineScatterType::Spline2D, pair.first,
            pair.second.first, pair.second.second, rowFromBox_->value() - 1,
            rowToBox_->value() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 5: {
        axisrect_->addLineSpecial2DPlot(
            AxisRect2D::LineScatterSpecialType::VerticalStep2D, pair.first,
            pair.second.first, pair.second.second, rowFromBox_->value() - 1,
            rowToBox_->value() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 6: {
        axisrect_->addLineSpecial2DPlot(
            AxisRect2D::LineScatterSpecialType::HorizontalStep2D, pair.first,
            pair.second.first, pair.second.second, rowFromBox_->value() - 1,
            rowToBox_->value() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 7: {
        axisrect_->addLineSpecial2DPlot(
            AxisRect2D::LineScatterSpecialType::Area2D, pair.first,
            pair.second.first, pair.second.second, rowFromBox_->value() - 1,
            rowToBox_->value() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 8: {
        axisrect_->addBox2DPlot(
            AxisRect2D::BarType::VerticalBars, pair.first, pair.second.first,
            pair.second.second, rowFromBox_->value() - 1,
            rowToBox_->value() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0), Bar2D::BarStyle::Individual);
      } break;
      case 9: {
        axisrect_->addBox2DPlot(
            AxisRect2D::BarType::HorizontalBars, pair.first, pair.second.first,
            pair.second.second, rowFromBox_->value() - 1,
            rowToBox_->value() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0), Bar2D::BarStyle::Individual);
      } break;
      case 10: {
      } break;
    }
  }
  axisrect_->replotBareBones();
  loadplotcontents();
  showCurrentFolder(app_->d_show_current_folder);
  enableAddBtn();
}

void AddPlot2DDialog::enableAddBtn() {
  if (available_->selectedItems().count() && available_columns_.count()) {
    groupBox_->setEnabled(true);
    btnAdd_->setEnabled(true);
    QPair<Table *, QPair<Column *, Column *>> pair =
        available_columns_.at(available_->currentRow());
    if (pair.second.first->rowCount() && pair.second.second->rowCount()) {
      rowFromBox_->setRange(1, pair.second.first->rowCount());
      rowToBox_->setRange(1, pair.second.first->rowCount());
      rowFromBox_->setValue(1);
      rowToBox_->setValue(pair.second.first->rowCount());
    } else {
      groupBox_->setEnabled(false);
      btnAdd_->setEnabled(false);
      rowFromBox_->setRange(0, 0);
      rowToBox_->setRange(0, 0);
      rowFromBox_->setValue(0);
      rowToBox_->setValue(0);
    }
  } else {
    groupBox_->setEnabled(false);
    btnAdd_->setEnabled(false);
    rowFromBox_->setRange(0, 0);
    rowToBox_->setRange(0, 0);
    rowFromBox_->setValue(0);
    rowToBox_->setValue(0);
  }
}

int AddPlot2DDialog::curveStyle() {
  int style = 0;
  switch (boxStyle_->currentIndex()) {
    case 0:
      style = static_cast<int>(ApplicationWindow::Graph::Line);
      break;
    case 1:
      style = static_cast<int>(ApplicationWindow::Graph::Scatter);
      break;
    case 2:
      style = static_cast<int>(ApplicationWindow::Graph::LineSymbols);
      break;
    case 3:
      style = static_cast<int>(ApplicationWindow::Graph::VerticalDropLines);
      break;
    case 4:
      style = static_cast<int>(ApplicationWindow::Graph::Spline);
      break;
    case 5:
      style = static_cast<int>(ApplicationWindow::Graph::VerticalSteps);
      break;
    case 6:
      style = static_cast<int>(ApplicationWindow::Graph::HorizontalSteps);
      break;
    case 7:
      style = static_cast<int>(ApplicationWindow::Graph::Area);
      break;
    case 8:
      style = static_cast<int>(ApplicationWindow::Graph::VerticalBars);
      break;
    case 9:
      style = static_cast<int>(ApplicationWindow::Graph::HorizontalBars);
      break;
    case 10:
      style = static_cast<int>(ApplicationWindow::Graph::Channel);
      break;
  }
  return style;
}

void AddPlot2DDialog::showCurrentFolder(bool currentFolder) {
  currentFolder = true;
  app_->d_show_current_folder = currentFolder;
  available_->clear();
  available_columns_.clear();

  if (currentFolder) {
    Folder *folder = app_->currentFolder();
    if (folder) {
      available_columns_ = app_->columnList(folder);
    }
  } else {
    available_columns_ = app_->columnList();
  }
  QPair<Table *, QPair<Column *, Column *>> columnpair;
  QPair<Table *, QPair<Column *, Column *>> plotcolumnpair;
  foreach (columnpair, available_columns_) {
    bool isplottable = false;
    foreach (plotcolumnpair, plotted_columns_) {
      if (plotcolumnpair == columnpair) {
        available_columns_.removeOne(plotcolumnpair);
        isplottable = true;
        break;
      }
    }
    if (!isplottable)
      available_->addItem(columnpair.first->name() + "_" +
                          columnpair.second.first->name() + "_" +
                          columnpair.second.second->name());
  }
}

void AddPlot2DDialog::closeEvent(QCloseEvent *event) {
  app_->d_add_curves_dialog_size = this->size();
  event->accept();
}
