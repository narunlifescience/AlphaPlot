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
  boxStyle = new QComboBox();
  boxStyle->addItem(IconLoader::load("graph2d-scatter", IconLoader::LightDark),
                    tr(" Scatter"));
  boxStyle->addItem(IconLoader::load("graph2d-line", IconLoader::LightDark),
                    tr(" Line"));
  boxStyle->addItem(
      IconLoader::load("graph2d-line-scatter", IconLoader::LightDark),
      tr(" Line + Symbol"));
  boxStyle->addItem(
      IconLoader::load("graph2d-vertical-drop", IconLoader::LightDark),
      tr(" Vertical drop lines"));
  boxStyle->addItem(IconLoader::load("graph2d-spline", IconLoader::LightDark),
                    tr(" Spline"));
  boxStyle->addItem(
      IconLoader::load("graph2d-vertical-step", IconLoader::LightDark),
      tr(" Vertical steps"));
  boxStyle->addItem(
      IconLoader::load("graph2d-horizontal-step", IconLoader::LightDark),
      tr(" Horizontal steps"));
  boxStyle->addItem(IconLoader::load("graph2d-area", IconLoader::LightDark),
                    tr(" Area"));
  boxStyle->addItem(
      IconLoader::load("graph2d-vertical-bar", IconLoader::LightDark),
      tr(" Vertical Bars"));
  boxStyle->addItem(
      IconLoader::load("graph2d-horizontal-bar", IconLoader::LightDark),
      tr(" Horizontal Bars"));
  hl->addWidget(boxStyle);

  boxMatrixStyle = new QComboBox();
  boxMatrixStyle->addItem(QPixmap(":/color_map.xpm"),
                          tr("Contour - Color Fill"));
  boxMatrixStyle->addItem(QPixmap(":/contour_map.xpm"), tr("Contour Lines"));
  boxMatrixStyle->addItem(QPixmap(":/gray_map.xpm"), tr("Gray Scale Map"));
  boxMatrixStyle->hide();
  hl->addWidget(boxMatrixStyle);
  hl->addStretch();

  QGridLayout *gl = new QGridLayout();
  gl->addWidget(new QLabel(tr("Available data")), 0, 0);
  gl->addWidget(new QLabel(tr("Graph contents")), 0, 2);

  available = new QListWidget();
  available->setSelectionMode(QAbstractItemView::ExtendedSelection);
  gl->addWidget(available, 1, 0);

  QVBoxLayout *vl1 = new QVBoxLayout();
  btnAdd = new QPushButton();
  btnAdd->setIcon(IconLoader::load("go-next", IconLoader::LightDark));
  btnAdd->setFixedWidth(35);
  btnAdd->setFixedHeight(30);
  vl1->addWidget(btnAdd);

  vl1->addStretch();

  gl->addLayout(vl1, 1, 1);
  contents = new QListWidget();
  contents->setSelectionMode(QAbstractItemView::ExtendedSelection);
  gl->addWidget(contents, 1, 2);

  QVBoxLayout *vl2 = new QVBoxLayout();
  btnAssociations = new QPushButton(tr("&Plot Associations..."));
  btnAssociations->setEnabled(false);
  vl2->addWidget(btnAssociations);

  btnEditFunction = new QPushButton(tr("&Edit Function..."));
  btnEditFunction->setEnabled(false);
  vl2->addWidget(btnEditFunction);

  btnOK = new QPushButton(tr("OK"));
  btnOK->setDefault(true);
  vl2->addWidget(btnOK);

  btnCancel = new QPushButton(tr("Close"));
  vl2->addWidget(btnCancel);

  vl2->addStretch();
  gl->addLayout(vl2, 1, 3);

  QVBoxLayout *vl3 = new QVBoxLayout(this);
  vl3->addLayout(hl);
  vl3->addLayout(gl);

  boxShowCurrentFolder = new QCheckBox(tr("Show current &folder only"));
  vl3->addWidget(boxShowCurrentFolder);

  init();
  boxShowCurrentFolder->setChecked(true);
  boxShowCurrentFolder->setEnabled(false);

  connect(boxShowCurrentFolder, SIGNAL(toggled(bool)), this,
          SLOT(showCurrentFolder(bool)));
  connect(btnAssociations, SIGNAL(clicked()), this,
          SLOT(showPlotAssociations()));
  connect(btnEditFunction, SIGNAL(clicked()), this, SLOT(showFunctionDialog()));
  connect(btnAdd, SIGNAL(clicked()), this, SLOT(addCurves()));
  connect(btnOK, SIGNAL(clicked()), this, SLOT(close()));
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(close()));
  connect(available, SIGNAL(itemSelectionChanged()), this,
          SLOT(enableAddBtn()));

  QShortcut *shortcut = new QShortcut(QKeySequence("-"), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(addCurves()));
}

void AddPlot2DDialog::showPlotAssociations() {
  int curve = contents->currentRow();
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
  QPoint pos = available->viewport()->mapFromGlobal(QCursor::pos());
  QRect rect = available->visualItemRect(available->currentItem());
  if (rect.contains(pos)) {
    QMenu contextMenu(this);
    QList<QListWidgetItem *> lst = available->selectedItems();
    if (lst.size() > 1)
      contextMenu.addAction(tr("&Plot Selection"), this, SLOT(addCurves()));
    else if (lst.size() == 1)
      contextMenu.addAction(tr("&Plot"), this, SLOT(addCurves()));
    contextMenu.exec(QCursor::pos());
  }

  pos = contents->viewport()->mapFromGlobal(QCursor::pos());
  rect = contents->visualItemRect(contents->currentItem());
  if (rect.contains(pos)) {
    QMenu contextMenu(this);
    QList<QListWidgetItem *> lst = contents->selectedItems();
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
  boxShowCurrentFolder->setChecked(currentFolderOnly);
  showCurrentFolder(currentFolderOnly);

  QStringList matrices = app_->matrixNames();
  if (!matrices.isEmpty()) {
    boxMatrixStyle->show();
    available->addItems(matrices);
  }

  if (!available->count()) btnAdd->setDisabled(true);
  enableAddBtn();
}

void AddPlot2DDialog::loadplotcontents() {
  contents->clear();
  plotted_columns_.clear();
  QVector<LineSpecial2D *> lslist = axisrect_->getLsVec();
  QVector<Curve2D *> curvelist = axisrect_->getCurveVec();
  QVector<Bar2D *> barlist = axisrect_->getBarVec();
  foreach (LineSpecial2D *ls, lslist) {
    DataBlockGraph *graphdata = ls->getdatablock_lsplot();
    QPair<Table *, Column *> columnpair = QPair<Table *, Column *>(
        graphdata->gettable(), graphdata->getycolumn());
    plotted_columns_ << columnpair;
    QListWidgetItem *item = new QListWidgetItem(
        ls->getIcon(),
        columnpair.first->name() + "_" + columnpair.second->name() + "[" +
            QString::number(graphdata->getfrom() + 1) + ":" +
            QString::number(graphdata->getto() + 1) + "]",
        contents);
    contents->addItem(item);
  }
  foreach (Curve2D *curve, curvelist) {
    if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Associated) {
      DataBlockCurve *curvedata = curve->getdatablock_cplot();
      QPair<Table *, Column *> columnpair = QPair<Table *, Column *>(
          curvedata->gettable(), curvedata->getycolumn());
      plotted_columns_ << columnpair;
      QListWidgetItem *item = new QListWidgetItem(
          curve->getIcon(),
          columnpair.first->name() + "_" + columnpair.second->name() + "[" +
              QString::number(curvedata->getfrom() + 1) + ":" +
              QString::number(curvedata->getto() + 1) + "]",
          contents);
      contents->addItem(item);
    }
  }
  foreach (Bar2D *bar, barlist) {
    if (!bar->ishistogram_barplot()) {
      DataBlockBar *bardata = bar->getdatablock_barplot();
      QPair<Table *, Column *> columnpair =
          QPair<Table *, Column *>(bardata->gettable(), bardata->getycolumn());
      plotted_columns_ << columnpair;
      QListWidgetItem *item = new QListWidgetItem(
          bar->getIcon(),
          columnpair.first->name() + "_" + columnpair.second->name() + "[" +
              QString::number(bardata->getfrom() + 1) + ":" +
              QString::number(bardata->getto() + 1) + "]",
          contents);
      contents->addItem(item);
    }
  }
}

void AddPlot2DDialog::addCurves() {
  QPair<Table *, Column *> pair;
  QList<QListWidgetItem *> lst = available->selectedItems();

  if (!lst.size() && !axisrect_->getXAxes2D().count() &&
      !axisrect_->getYAxes2D().count())
    return;

  foreach (QListWidgetItem *item, lst) {
    pair = available_columns_.at(available->row(item));
    if (pair.first->firstXCol() < 0) {
      QMessageBox::warning(
          this, tr("Error"),
          tr("No X column available in table: ") + pair.first->name() + "!");
      return;
    }
    switch (boxStyle->currentIndex()) {
      case 0: {
        axisrect_->addCurve2DPlot(
            AxisRect2D::LineScatterType::Scatter2D, pair.first,
            pair.first->column(pair.first->firstXCol()), pair.second, 0,
            pair.second->rowCount() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 1: {
        axisrect_->addCurve2DPlot(
            AxisRect2D::LineScatterType::Line2D, pair.first,
            pair.first->column(pair.first->firstXCol()), pair.second, 0,
            pair.second->rowCount() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 2: {
        axisrect_->addCurve2DPlot(
            AxisRect2D::LineScatterType::LineAndScatter2D, pair.first,
            pair.first->column(pair.first->firstXCol()), pair.second, 0,
            pair.second->rowCount() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 3: {
        axisrect_->addLineSpecial2DPlot(
            AxisRect2D::LineScatterSpecialType::VerticalDropLine2D, pair.first,
            pair.first->column(pair.first->firstXCol()), pair.second, 0,
            pair.second->rowCount() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 4: {
        axisrect_->addCurve2DPlot(
            AxisRect2D::LineScatterType::Spline2D, pair.first,
            pair.first->column(pair.first->firstXCol()), pair.second, 0,
            pair.second->rowCount() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 5: {
        axisrect_->addLineSpecial2DPlot(
            AxisRect2D::LineScatterSpecialType::VerticalStep2D, pair.first,
            pair.first->column(pair.first->firstXCol()), pair.second, 0,
            pair.second->rowCount() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 6: {
        axisrect_->addLineSpecial2DPlot(
            AxisRect2D::LineScatterSpecialType::HorizontalStep2D, pair.first,
            pair.first->column(pair.first->firstXCol()), pair.second, 0,
            pair.second->rowCount() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 7: {
        axisrect_->addLineSpecial2DPlot(
            AxisRect2D::LineScatterSpecialType::Area2D, pair.first,
            pair.first->column(pair.first->firstXCol()), pair.second, 0,
            pair.second->rowCount() - 1, axisrect_->getXAxis(0),
            axisrect_->getYAxis(0));
      } break;
      case 8: {
        axisrect_->addBox2DPlot(AxisRect2D::BarType::VerticalBars, pair.first,
                                pair.first->column(pair.first->firstXCol()),
                                pair.second, 0, pair.second->rowCount() - 1,
                                axisrect_->getXAxis(0), axisrect_->getYAxis(0),
                                Bar2D::BarStyle::Individual);
      } break;
      case 9: {
        axisrect_->addBox2DPlot(AxisRect2D::BarType::HorizontalBars, pair.first,
                                pair.first->column(pair.first->firstXCol()),
                                pair.second, 0, pair.second->rowCount() - 1,
                                axisrect_->getXAxis(0), axisrect_->getYAxis(0),
                                Bar2D::BarStyle::Individual);
      } break;
    }
  }
  axisrect_->replotBareBones();
  loadplotcontents();
  showCurrentFolder(app_->d_show_current_folder);
  enableAddBtn();
}

void AddPlot2DDialog::enableAddBtn() {
  btnAdd->setEnabled(available->count() > 0 &&
                     !available->selectedItems().isEmpty());
}

int AddPlot2DDialog::curveStyle() {
  int style = 0;
  switch (boxStyle->currentIndex()) {
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
  }
  return style;
}

void AddPlot2DDialog::showCurrentFolder(bool currentFolder) {
  app_->d_show_current_folder = currentFolder;
  available->clear();
  available_columns_.clear();

  if (currentFolder) {
    Folder *folder = app_->currentFolder();
    if (folder) {
      available_columns_ = app_->columnList(folder, AlphaPlot::Y);
    }
  } else {
    available_columns_ = app_->columnList(AlphaPlot::Y);
  }
  QPair<Table *, Column *> columnpair;
  QPair<Table *, Column *> plotcolumnpair;
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
      available->addItem(columnpair.first->name() + "_" +
                         columnpair.second->name());
  }
}

void AddPlot2DDialog::closeEvent(QCloseEvent *event) {
  app_->d_add_curves_dialog_size = this->size();
  event->accept();
}
