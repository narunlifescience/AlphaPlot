#include "AddPlot2DDialog.h"

#include "ApplicationWindow.h"
#include "Folder.h"
#include "FunctionCurve.h"
#include "Graph.h"
#include "Matrix.h"
#include "PlotCurve.h"
#include "Table.h"
#include "core/IconLoader.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QKeySequence>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QMenu>
#include <QPixmap>
#include <QPushButton>
#include <QShortcut>

#include <QMessageBox>

AddPlot2DDialog::AddPlot2DDialog(QWidget *parent, Qt::WFlags fl)
    : QDialog(parent, fl), app_(nullptr), axisrect_(nullptr) {
  Q_ASSERT(parent);
  app_ = qobject_cast<ApplicationWindow *>(parent->parent());
  Q_ASSERT(app_);

  setWindowTitle(tr("Add/Remove 2D Plots"));
  setSizeGripEnabled(true);
  setFocus();

  QHBoxLayout *hl = new QHBoxLayout();

  hl->addWidget(new QLabel(tr("New Plot Type")));
  boxStyle = new QComboBox();
  boxStyle->addItem(IconLoader::load("graph2d-line", IconLoader::LightDark),
                    tr(" Line"));
  boxStyle->addItem(IconLoader::load("graph2d-scatter", IconLoader::LightDark),
                    tr(" Scatter"));
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

  btnRemove = new QPushButton();
  btnRemove->setIcon(IconLoader::load("go-previous", IconLoader::LightDark));
  btnRemove->setFixedWidth(35);
  btnRemove->setFixedHeight(30);
  vl1->addWidget(btnRemove);
  vl1->addStretch();

  gl->addLayout(vl1, 1, 1);
  contents = new QListWidget();
  contents->setSelectionMode(QAbstractItemView::ExtendedSelection);
  gl->addWidget(contents, 1, 2);

  QVBoxLayout *vl2 = new QVBoxLayout();
  btnAssociations = new QPushButton(tr("&Plot Associations..."));
  btnAssociations->setEnabled(false);
  vl2->addWidget(btnAssociations);

  btnRange = new QPushButton(tr("Edit &Range..."));
  btnRange->setEnabled(false);
  vl2->addWidget(btnRange);

  btnEditFunction = new QPushButton(tr("&Edit Function..."));
  btnEditFunction->setEnabled(false);
  vl2->addWidget(btnEditFunction);

  btnOK = new QPushButton(tr("OK"));
  btnOK->setDefault(true);
  vl2->addWidget(btnOK);

  btnCancel = new QPushButton(tr("Close"));
  vl2->addWidget(btnCancel);

  boxShowRange = new QCheckBox(tr("&Show Range"));
  vl2->addWidget(boxShowRange);

  vl2->addStretch();
  gl->addLayout(vl2, 1, 3);

  QVBoxLayout *vl3 = new QVBoxLayout(this);
  vl3->addLayout(hl);
  vl3->addLayout(gl);

  boxShowCurrentFolder = new QCheckBox(tr("Show current &folder only"));
  vl3->addWidget(boxShowCurrentFolder);

  init();

  connect(boxShowCurrentFolder, SIGNAL(toggled(bool)), this,
          SLOT(showCurrentFolder(bool)));
  connect(boxShowRange, SIGNAL(toggled(bool)), this,
          SLOT(showCurveRange(bool)));
  connect(btnRange, SIGNAL(clicked()), this, SLOT(showCurveRangeDialog()));
  connect(btnAssociations, SIGNAL(clicked()), this,
          SLOT(showPlotAssociations()));
  connect(btnEditFunction, SIGNAL(clicked()), this, SLOT(showFunctionDialog()));
  connect(btnAdd, SIGNAL(clicked()), this, SLOT(addCurves()));
  connect(btnRemove, SIGNAL(clicked()), this, SLOT(removeCurves()));
  connect(btnOK, SIGNAL(clicked()), this, SLOT(close()));
  connect(btnCancel, SIGNAL(clicked()), this, SLOT(close()));
  connect(contents, SIGNAL(currentRowChanged(int)), this,
          SLOT(showCurveBtn(int)));
  connect(contents, SIGNAL(itemSelectionChanged()), this,
          SLOT(enableRemoveBtn()));
  connect(available, SIGNAL(itemSelectionChanged()), this,
          SLOT(enableAddBtn()));

  QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(removeCurves()));
  shortcut = new QShortcut(QKeySequence("-"), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(removeCurves()));
  shortcut = new QShortcut(QKeySequence("+"), this);
  connect(shortcut, SIGNAL(activated()), this, SLOT(addCurves()));
}

void AddPlot2DDialog::showCurveBtn(int) {
 /* QwtPlotItem *it = d_graph->plotItem(contents->currentRow());
  if (!it) return;

  bool function = false;
  bool associations = false;
  bool range = false;

  if (it->rtti() != QwtPlotItem::Rtti_PlotSpectrogram) {
    PlotCurve *c = (PlotCurve *)it;
    if (c->type() == Graph::Function)
      function = true;
    else {
      associations = true;
      if (c->type() != Graph::ErrorBars) range = true;
    }
  }

  btnEditFunction->setEnabled(function);
  btnAssociations->setEnabled(associations);
  btnRange->setEnabled(range);*/
}

void AddPlot2DDialog::showCurveRangeDialog() {
  /*int curve = contents->currentRow();
  if (curve < 0) curve = 0;

  app_->showCurveRangeDialog(d_graph, curve);
  updateCurveRange();*/
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
  bool currentFolderOnly = app_->d_show_current_folder;
  boxShowCurrentFolder->setChecked(currentFolderOnly);
  showCurrentFolder(currentFolderOnly);

  QStringList matrices = app_->matrixNames();
  if (!matrices.isEmpty()) {
    boxMatrixStyle->show();
    available->addItems(matrices);
  }

  int style = app_->defaultCurveStyle;
  if (style == Graph::Line)
    boxStyle->setCurrentIndex(0);
  else if (style == Graph::Scatter)
    boxStyle->setCurrentIndex(1);
  else if (style == Graph::LineSymbols)
    boxStyle->setCurrentIndex(2);
  else if (style == Graph::VerticalDropLines)
    boxStyle->setCurrentIndex(3);
  else if (style == Graph::Spline)
    boxStyle->setCurrentIndex(4);
  else if (style == Graph::VerticalSteps)
    boxStyle->setCurrentIndex(5);
  else if (style == Graph::HorizontalSteps)
    boxStyle->setCurrentIndex(6);
  else if (style == Graph::Area)
    boxStyle->setCurrentIndex(7);
  else if (style == Graph::VerticalBars)
    boxStyle->setCurrentIndex(8);
  else if (style == Graph::HorizontalBars)
    boxStyle->setCurrentIndex(9);

  if (!available->count()) btnAdd->setDisabled(true);
}

void AddPlot2DDialog::setLayout2D(AxisRect2D *axisrect) {
  if(!axisrect) return;

  axisrect_ = axisrect;
  //contents->addItems(d_graph->plotItemsList());
  enableRemoveBtn();
  enableAddBtn();
}

void AddPlot2DDialog::addCurves() {
  /*QStringList emptyColumns;
  QList<QListWidgetItem *> lst = available->selectedItems();
  for (int i = 0; i < lst.size(); ++i) {
    QString text = lst.at(i)->text();
    if (contents->findItems(text, Qt::MatchExactly).isEmpty()) {
      if (!addCurve(text)) emptyColumns << text;
    }
  }
  d_graph->updatePlot();
  Graph::showPlotErrorMessage(this, emptyColumns);

  showCurveRange(boxShowRange->isChecked());*/
}

bool AddPlot2DDialog::addCurve(const QString &name) {
  /*QStringList matrices = app_->matrixNames();
  if (matrices.contains(name)) {
    Matrix *m = app_->matrix(name);
    if (!m) return false;

    switch (boxMatrixStyle->currentIndex()) {
      case 0:
        d_graph->plotSpectrogram(m, Graph::ColorMap);
        break;
      case 1:
        d_graph->plotSpectrogram(m, Graph::ContourMap);
        break;
      case 2:
        d_graph->plotSpectrogram(m, Graph::GrayMap);
        break;
    }

    contents->addItem(name);
    return true;
  }

  int style = curveStyle();
  Table *t = app_->table(name);
  if (t && d_graph->insertCurve(t, name, style)) {
    CurveLayout cl = Graph::initCurveLayout();
    int color, symbol;
    d_graph->guessUniqueCurveLayout(color, symbol);

    cl.lCol = color;
    cl.symCol = color;
    cl.fillCol = color;
    cl.lWidth = app_->defaultCurveLineWidth;
    cl.sSize = app_->defaultSymbolSize;
    cl.sType = symbol;

    if (style == Graph::Line)
      cl.sType = 0;
    else if (style == Graph::VerticalBars || style == Graph::HorizontalBars) {
      cl.filledArea = 1;
      cl.lCol = 0;
      cl.aCol = color;
      cl.sType = 0;
    } else if (style == Graph::Area) {
      cl.filledArea = 1;
      cl.aCol = color;
      cl.sType = 0;
    } else if (style == Graph::VerticalDropLines)
      cl.connectType = 2;
    else if (style == Graph::VerticalSteps || style == Graph::HorizontalSteps) {
      cl.connectType = 3;
      cl.sType = 0;
    } else if (style == Graph::Spline)
      cl.connectType = 5;

    d_graph->updateCurveLayout(d_graph->curves() - 1, &cl);

    contents->addItem(name);
    return true;
  }
  return false;*/
}

void AddPlot2DDialog::removeCurves() {
  /*QList<QListWidgetItem *> lst = contents->selectedItems();
  for (int i = 0; i < lst.size(); ++i) {
    QListWidgetItem *it = lst.at(i);
    QString s = it->text();
    if (boxShowRange->isChecked()) {
      QStringList lst = s.split("[");
      s = lst[0];
    }
    d_graph->removeCurve(s);
  }

  showCurveRange(boxShowRange->isChecked());
  d_graph->updatePlot();*/
}

void AddPlot2DDialog::enableAddBtn() {
  btnAdd->setEnabled(available->count() > 0 &&
                     !available->selectedItems().isEmpty());
}

void AddPlot2DDialog::enableRemoveBtn() {
  btnRemove->setEnabled(contents->count() > 0 &&
                        !contents->selectedItems().isEmpty());
}

int AddPlot2DDialog::curveStyle() {
  int style = 0;
  switch (boxStyle->currentIndex()) {
    case 0:
      style = Graph::Line;
      break;
    case 1:
      style = Graph::Scatter;
      break;
    case 2:
      style = Graph::LineSymbols;
      break;
    case 3:
      style = Graph::VerticalDropLines;
      break;
    case 4:
      style = Graph::Spline;
      break;
    case 5:
      style = Graph::VerticalSteps;
      break;
    case 6:
      style = Graph::HorizontalSteps;
      break;
    case 7:
      style = Graph::Area;
      break;
    case 8:
      style = Graph::VerticalBars;
      break;
    case 9:
      style = Graph::HorizontalBars;
      break;
  }
  return style;
}

void AddPlot2DDialog::showCurveRange(bool on) {
  /*int row = contents->currentRow();
  contents->clear();
  if (on) {
    QStringList lst = QStringList();
    for (int i = 0; i < d_graph->curves(); i++) {
      QwtPlotItem *it = d_graph->plotItem(i);
      if (!it) continue;

      if (it->rtti() == QwtPlotItem::Rtti_PlotCurve &&
          ((PlotCurve *)it)->type() != Graph::Function) {
        DataCurve *c = (DataCurve *)it;
        lst << c->title().text() + "[" + QString::number(c->startRow() + 1) +
                   ":" + QString::number(c->endRow() + 1) + "]";
      } else
        lst << it->title().text();
    }
    contents->addItems(lst);
  } else
    contents->addItems(d_graph->plotItemsList());

  contents->setCurrentRow(row);
  enableRemoveBtn();*/
}

void AddPlot2DDialog::updateCurveRange() {
  showCurveRange(boxShowRange->isChecked());
}

void AddPlot2DDialog::showCurrentFolder(bool currentFolder) {
  app_->d_show_current_folder = currentFolder;
  available->clear();

  if (currentFolder) {
    Folder *f = app_->currentFolder();
    if (f) {
      QStringList columns;
      foreach (QWidget *w, f->windowsList()) {
        if (!w->inherits("Table")) continue;

        Table *t = (Table *)w;
        for (int i = 0; i < t->numCols(); i++) {
          if (t->colPlotDesignation(i) == AlphaPlot::Y)
            columns << QString(t->name()) + "_" + t->colLabel(i);
        }
      }
      available->addItems(columns);
    }
  } else
    available->addItems(app_->columnsList(AlphaPlot::Y));
}

void AddPlot2DDialog::closeEvent(QCloseEvent *e) {
  app_->d_add_curves_dialog_size = this->size();
  e->accept();
}
