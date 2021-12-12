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
#include <QPixmap>
#include <QPushButton>
#include <QSharedPointer>
#include <QShortcut>

#include "2Dplot/Bar2D.h"
#include "2Dplot/Curve2D.h"
#include "2Dplot/DataManager2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "2Dplot/Pie2D.h"
#include "2Dplot/Vector2D.h"
#include "Folder.h"
#include "Matrix.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "core/column/Column.h"

AddPlot2DDialog::AddPlot2DDialog(QWidget *parent, AxisRect2D *axisrect,
                                 const Type &type, Qt::WindowFlags fl)
    : QDialog(parent, fl),
      app_(qobject_cast<ApplicationWindow *>(parent->parent())),
      axisrect_(axisrect),
      type_(type),
      plotnos_(0),
      xyorxyyplotnos_(0) {
  Q_ASSERT(app_);
  Q_ASSERT(axisrect_);

  setWindowTitle(tr("Add 2D Plots"));
  setSizeGripEnabled(true);
  setFocus();

  QHBoxLayout *hl = new QHBoxLayout();

  hl->addWidget(new QLabel(tr("New Plot Type")));
  boxStyle_ = new QComboBox();
  switch (type_) {
    case Type::Table_Y:
      boxStyle_->addItem(IconLoader::load("graph2d-box", IconLoader::LightDark),
                         tr(" Box"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-histogram", IconLoader::LightDark),
          tr(" Histogram"));
      boxStyle_->addItem(IconLoader::load("graph2d-pie", IconLoader::LightDark),
                         tr(" Pie"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-halfpie", IconLoader::LightDark),
          tr(" Half Pie"));
      break;
    case Type::Table_X_Y:
    case Type::Table_X_Y_Y:
      boxStyle_->addItem(
          IconLoader::load("graph2d-scatter", IconLoader::LightDark),
          tr(" Scatter"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-line", IconLoader::LightDark), tr(" Line"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-line-scatter", IconLoader::LightDark),
          tr(" Line + Symbol"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-vertical-drop", IconLoader::LightDark),
          tr(" Vertical drop lines"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-spline", IconLoader::LightDark),
          tr(" Spline"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-vertical-step", IconLoader::LightDark),
          tr(" Vertical steps"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-horizontal-step", IconLoader::LightDark),
          tr(" Horizontal steps"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-area", IconLoader::LightDark), tr(" Area"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-vertical-bar", IconLoader::LightDark),
          tr(" Vertical Bars"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-horizontal-bar", IconLoader::LightDark),
          tr(" Horizontal Bars"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-channel", IconLoader::LightDark),
          tr(" Channel fill"));
      break;
    case Type::Table_X_Y_Y_Y:
      boxStyle_->addItem(
          IconLoader::load("graph2d-vector-xyam", IconLoader::LightDark),
          tr(" Vector XYAM"));
      boxStyle_->addItem(
          IconLoader::load("graph2d-vector-xy", IconLoader::LightDark),
          tr(" Vector XYXY"));
      break;
  }
  hl->addWidget(boxStyle_);
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
  btnAssociations_ = new QPushButton(tr("&Go to Plot Associations..."));
  btnAssociations_->setEnabled(false);
  vl2->addWidget(btnAssociations_);

  btnEditFunction_ = new QPushButton(tr("&Function Details..."));
  btnEditFunction_->setEnabled(false);
  vl2->addWidget(btnEditFunction_);

  xaxisLabel_ = new QLabel(tr("X axis:"));
  boxXaxis_ = new QComboBox;
  yaxisLabel_ = new QLabel(tr("Y axis:"));
  boxYaxis_ = new QComboBox;
  vl2->addWidget(xaxisLabel_);
  vl2->addWidget(boxXaxis_);
  vl2->addWidget(yaxisLabel_);
  vl2->addWidget(boxYaxis_);
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

  btnOK_ = new QPushButton(tr("OK"));
  btnOK_->setDefault(true);
  vl2->addWidget(btnOK_);

  btnCancel_ = new QPushButton(tr("Close"));
  vl2->addWidget(btnCancel_);

  vl2->addStretch();
  gl->addLayout(vl2, 1, 3);

  QVBoxLayout *vl3 = new QVBoxLayout(this);
  vl3->addLayout(hl);
  vl3->addLayout(gl);

  init();

  connect(btnAdd_, &QPushButton::clicked, this, &AddPlot2DDialog::addPlots);
  connect(btnOK_, &QPushButton::clicked, this, &AddPlot2DDialog::close);
  connect(btnCancel_, &QPushButton::clicked, this, &AddPlot2DDialog::close);
  connect(available_, &QListWidget::itemSelectionChanged, this,
          &AddPlot2DDialog::enableAddBtn);
  connect(contents_, &QListWidget::itemSelectionChanged, this, [&]() {
    if (contents_->selectedItems().count() && plotted_columns_.count()) {
      QString string = contents_->currentItem()->text();
      if (string.contains("_(XY)[", Qt::CaseSensitive) ||
          string.contains("_(Parametric:", Qt::CaseSensitive) ||
          string.contains("_(Polar:", Qt::CaseSensitive)) {
        btnAssociations_->setEnabled(false);
        btnEditFunction_->setEnabled(true);
      } else {
        btnAssociations_->setEnabled(true);
        btnEditFunction_->setEnabled(false);
      }
    } else {
      btnAssociations_->setEnabled(false);
      btnEditFunction_->setEnabled(false);
    }
  });
  connect(btnAssociations_, &QPushButton::pressed, this,
          &AddPlot2DDialog::showPlotAssociations);
  connect(btnEditFunction_, &QPushButton::clicked, this,
          &AddPlot2DDialog::showFunctionDialog);
  connect(
      boxStyle_,
      static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
      this, &AddPlot2DDialog::populateAvailable);

  QShortcut *shortcut = new QShortcut(QKeySequence("-"), this);
  connect(shortcut, &QShortcut::activated, this, &AddPlot2DDialog::addPlots);
}

void AddPlot2DDialog::showPlotAssociations() {
  Data data = plotted_columns_.at(contents_->currentRow());
  app_->d_workspace->setActiveSubWindow(data.table);
  if (data.xcol) data.table->selectColumn(data.xcol->index());
  if (data.ycol1) data.table->selectColumn(data.ycol1->index());
  if (data.ycol2) data.table->selectColumn(data.ycol2->index());
  if (data.ycol3) data.table->selectColumn(data.ycol3->index());
  close();
}

void AddPlot2DDialog::showFunctionDialog() {
  QMessageBox::information(this, tr("Function Plot details"),
                           contents_->currentItem()->text());
}

QSize AddPlot2DDialog::sizeHint() const { return QSize(700, 400); }

void AddPlot2DDialog::contextMenuEvent(QContextMenuEvent *e) {
  QPoint pos = contents_->viewport()->mapFromGlobal(QCursor::pos());
  QRect rect = contents_->visualItemRect(contents_->currentItem());
  if (rect.contains(pos)) {
    QMenu contextMenu(this);
    QList<QListWidgetItem *> lst = contents_->selectedItems();
    if (lst.size() == 1)
      contextMenu.addAction(tr("&Delete Plot"), this,
                            &AddPlot2DDialog::removePlots);
    contextMenu.exec(QCursor::pos());
  }

  e->accept();
}

void AddPlot2DDialog::init() {
  populatePlotted();
  populateAvailable();
  populateAxes();
  enableAddBtn();
}

void AddPlot2DDialog::populateAxes() {
  xaxis_list_.clear();
  yaxis_list_.clear();
  boxXaxis_->clear();
  boxYaxis_->clear();
  xaxis_list_ = axisrect_->getXAxes2D();
  yaxis_list_ = axisrect_->getYAxes2D();
  foreach (Axis2D *axis, xaxis_list_) {
    boxXaxis_->addItem(QString(axis->getname_axis() +
                               QString::number(axis->getnumber_axis())));
  }
  foreach (Axis2D *axis, yaxis_list_) {
    boxYaxis_->addItem(QString(axis->getname_axis() +
                               QString::number(axis->getnumber_axis())));
  }
}

void AddPlot2DDialog::populatePlotted() {
  contents_->clear();
  plotted_columns_.clear();
  QVector<StatBox2D *> boxlist = axisrect_->getStatBoxVec();
  QVector<Pie2D *> pielist = axisrect_->getPieVec();
  QVector<LineSpecial2D *> lslist = axisrect_->getLsVec();
  QVector<Curve2D *> curvelist = axisrect_->getCurveVec();
  QVector<Bar2D *> barlist = axisrect_->getBarVec();
  QVector<QPair<LineSpecial2D *, LineSpecial2D *>> channellist =
      axisrect_->getChannelVec();
  QVector<Vector2D *> veclist = axisrect_->getVectorVec();
  plotnos_ = boxlist.count() + pielist.count() + lslist.count() +
             curvelist.count() + barlist.count() + channellist.count() +
             veclist.count();
  xyorxyyplotnos_ = lslist.count() + curvelist.count() + barlist.count() +
                    channellist.count();

  foreach (StatBox2D *box, boxlist) {
    Data boxdata;
    boxdata.type = Type::Table_Y;
    boxdata.table = box->getboxwhiskerdata_statbox().table_;
    boxdata.ycol1 = box->getboxwhiskerdata_statbox().column_;
    plotted_columns_ << boxdata;
    QListWidgetItem *item = new QListWidgetItem(
        box->getIcon(),
        boxdata.table->name() + "_" + boxdata.ycol1->name() + "[" +
            QString::number(box->getboxwhiskerdata_statbox().from_ + 1) + ":" +
            QString::number(box->getboxwhiskerdata_statbox().to_ + 1) + "]",
        contents_);
    contents_->addItem(item);
  }
  foreach (Pie2D *pie, pielist) {
    Data pdata;
    pdata.type = Type::Table_Y;
    pdata.table = pie->gettable_pieplot();
    pdata.xcol = pie->getxcolumn_pieplot();
    pdata.ycol1 = pie->getycolumn_pieplot();
    plotted_columns_ << pdata;
    QListWidgetItem *item = new QListWidgetItem(
        pie->getIcon(),
        pdata.table->name() + "_" + pdata.ycol1->name() +
            "(Label: " + pdata.xcol->name() + ")[" +
            QString::number(pie->getfrom_pieplot() + 1) + ":" +
            QString::number(pie->getto_pieplot() + 1) + "]",
        contents_);
    contents_->addItem(item);
  }
  foreach (LineSpecial2D *ls, lslist) {
    DataBlockGraph *graphdata = ls->getdatablock_lsplot();
    Data gdata;
    gdata.type = Type::Table_X_Y;
    gdata.table = graphdata->gettable();
    gdata.xcol = graphdata->getxcolumn();
    gdata.ycol1 = graphdata->getycolumn();
    plotted_columns_ << gdata;
    QListWidgetItem *item = new QListWidgetItem(
        ls->getIcon(),
        gdata.table->name() + "_" + gdata.xcol->name() + "_" +
            gdata.ycol1->name() + "[" +
            QString::number(graphdata->getfrom() + 1) + ":" +
            QString::number(graphdata->getto() + 1) + "]",
        contents_);
    contents_->addItem(item);
  }
  foreach (Curve2D *curve, curvelist) {
    if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Associated) {
      DataBlockCurve *curvedata = curve->getdatablock_cplot();
      Data cdata;
      cdata.type = Type::Table_X_Y;
      cdata.table = curvedata->gettable();
      cdata.xcol = curvedata->getxcolumn();
      cdata.ycol1 = curvedata->getycolumn();
      plotted_columns_ << cdata;
      QListWidgetItem *item = new QListWidgetItem(
          curve->getIcon(),
          cdata.table->name() + "_" + cdata.xcol->name() + "_" +
              cdata.ycol1->name() + "[" +
              QString::number(curvedata->getfrom() + 1) + ":" +
              QString::number(curvedata->getto() + 1) + "]",
          contents_);
      contents_->addItem(item);
    } else {
      PlotData::FunctionData funcdata = curve->getfuncdata_cplot();
      Data data;
      data.type = Type::Table_X_Y;
      QString string;
      switch (funcdata.type) {
        case 0: {
          QString functype, func;
          if (funcdata.functions.size() == 1) {
            functype = QString(tr("XY"));
            func = funcdata.functions.at(0);
          } else {
            functype = QString(tr("Unknown"));
            func = QString(tr("unknown"));
          }
          string += func + "_(" + functype + ")" + "[" +
                    QString::number(funcdata.points) + " ," +
                    QString::number(funcdata.from) + ":" +
                    QString::number(funcdata.to) + "]";
        } break;
        case 1: {
          QString functype, func1, func2;
          if (funcdata.functions.size() == 2) {
            functype = QString(tr("Parametric"));
            func1 = funcdata.functions.at(0);
            func2 = funcdata.functions.at(1);
          } else {
            functype = QString(tr("Unknown"));
            func1 = QString(tr("unknown"));
            func2 = QString(tr("unknown"));
          }
          string += func1 + "_" + func2 + "_(" + functype + ":" +
                    funcdata.parameter + ")" + "[" +
                    QString::number(funcdata.points) + " ," +
                    QString::number(funcdata.from) + ":" +
                    QString::number(funcdata.to) + "]";
        } break;
        case 2: {
          QString functype, func1, func2;
          if (funcdata.functions.size() == 2) {
            functype = QString(tr("Polar"));
            func1 = funcdata.functions.at(0);
            func2 = funcdata.functions.at(1);
          } else {
            functype = QString(tr("Unknown"));
            func1 = QString(tr("unknown"));
            func2 = QString(tr("unknown"));
          }
          string += func1 + "_" + func2 + "_(" + functype + ":" +
                    funcdata.parameter + ")" + "[" +
                    QString::number(funcdata.points) + " ," +
                    QString::number(funcdata.from) + ":" +
                    QString::number(funcdata.to) + "]";
        } break;
      }
      data.fd.type = funcdata.type;
      data.fd.functions = funcdata.functions;
      data.fd.parameter = funcdata.parameter;
      data.fd.points = funcdata.points;
      data.fd.from = funcdata.from;
      data.fd.to = funcdata.to;
      plotted_columns_ << data;
      QListWidgetItem *item =
          new QListWidgetItem(curve->getIcon(), string, contents_);
      contents_->addItem(item);
    }
  }
  foreach (Bar2D *bar, barlist) {
    if (!bar->ishistogram_barplot()) {
      DataBlockBar *bardata = bar->getdatablock_barplot();
      Data bdata;
      bdata.type = Type::Table_X_Y;
      bdata.table = bardata->gettable();
      bdata.xcol = bardata->getxcolumn();
      bdata.ycol1 = bardata->getycolumn();
      plotted_columns_ << bdata;
      QListWidgetItem *item = new QListWidgetItem(
          bar->getIcon(),
          bdata.table->name() + "_" + bdata.xcol->name() + "_" +
              bdata.ycol1->name() + "[" +
              QString::number(bardata->getfrom() + 1) + ":" +
              QString::number(bardata->getto() + 1) + "]",
          contents_);
      contents_->addItem(item);
    } else {
      DataBlockHist *histdata = bar->getdatablock_histplot();
      Data hdata;
      hdata.type = Type::Table_Y;
      hdata.table = histdata->gettable();
      hdata.xcol = histdata->getcolumn();
      plotted_columns_ << hdata;
      QListWidgetItem *item = new QListWidgetItem(
          bar->getIcon(),
          hdata.table->name() + "_" + hdata.xcol->name() + "[" +
              QString::number(histdata->getfrom() + 1) + ":" +
              QString::number(histdata->getto() + 1) + "]",
          contents_);
      contents_->addItem(item);
    }
  }
  for (int i = 0; i < channellist.size(); i++) {
    QPair<LineSpecial2D *, LineSpecial2D *> pair = channellist.at(i);
    // 1st
    DataBlockGraph *graphdata1 = pair.first->getdatablock_lsplot();
    DataBlockGraph *graphdata2 = pair.second->getdatablock_lsplot();
    Data cldata;
    cldata.type = Type::Table_X_Y_Y;
    cldata.table = graphdata1->gettable();
    cldata.xcol = graphdata1->getxcolumn();
    cldata.ycol1 = graphdata1->getycolumn();
    cldata.ycol2 = graphdata2->getycolumn();
    plotted_columns_ << cldata;

    QListWidgetItem *item = new QListWidgetItem(
        IconLoader::load("graph2d-channel", IconLoader::LightDark),
        cldata.table->name() + "_" + cldata.xcol->name() + "_" +
            cldata.ycol1->name() + "_" + cldata.ycol2->name() + "[" +
            QString::number(graphdata1->getfrom() + 1) + ":" +
            QString::number(graphdata1->getto() + 1) + "]",
        contents_);
    contents_->addItem(item);
  }
  foreach (Vector2D *vec, veclist) {
    Data vdata;
    vdata.type = Type::Table_X_Y_Y_Y;
    vdata.table = vec->gettable_vecplot();
    vdata.xcol = vec->getfirstcol_vecplot();
    vdata.ycol1 = vec->getsecondcol_vecplot();
    vdata.ycol2 = vec->getthirdcol_vecplot();
    vdata.ycol3 = vec->getfourthcol_vecplot();
    plotted_columns_ << vdata;
    QListWidgetItem *item = new QListWidgetItem(
        vec->getIcon(),
        vdata.table->name() + "_" + vdata.xcol->name() + "_" +
            vdata.ycol1->name() + "_" + vdata.ycol2->name() + "_" +
            vdata.ycol3->name() + "[" +
            QString::number(vec->getfrom_vecplot() + 1) + ":" +
            QString::number(vec->getto_vecplot() + 1) + "]",
        contents_);
    contents_->addItem(item);
  }
}

void AddPlot2DDialog::populateAvailable() {
  available_->clear();
  available_columns_.clear();
  QList<MyWidget *> subwindowlist = app_->currentFolder()->windowsList();
  QList<Table *> tables;
  // extract all tables
  foreach (QMdiSubWindow *subwindow, subwindowlist) {
    Table *t = qobject_cast<Table *>(subwindow);
    if (!t) continue;
    tables << t;
  }

  // dont populate for these plots
  if (type_ == Type::Table_X_Y_Y_Y && plotnos_ != 0) return;
  if (type_ == Type::Table_Y && plotnos_ != 0 &&
      plotStyle() == ApplicationWindow::Graph::Box)
    foreach (Data d, plotted_columns_) {
      if (d.xcol != nullptr) return;
    }

  if (type_ == Type::Table_Y && plotnos_ != 0 &&
      plotStyle() == ApplicationWindow::Graph::Pie)
    return;

  if ((type_ == Type::Table_X_Y || type_ == Type::Table_X_Y_Y) &&
      plotnos_ - xyorxyyplotnos_ != 0)
    return;

  switch (type_) {
    case Type::Table_Y:
      foreach (Table *table, tables) {
        QList<Column *> ylist;
        QList<Column *> xlist;

        for (int i = 0; i < table->numCols(); i++) {
          if (table->colPlotDesignation(i) == AlphaPlot::PlotDesignation::Y)
            ylist << table->column(i);
          else if (table->colPlotDesignation(i) ==
                   AlphaPlot::PlotDesignation::X)
            xlist << table->column(i);
        }

        Data data;
        data.type = Type::Table_Y;
        switch (plotStyle()) {
          case ApplicationWindow::Graph::Pie:
            foreach (Column *xcol, xlist) {
              foreach (Column *ycol, ylist) {
                if (xcol->dataType() == AlphaPlot::ColumnDataType::TypeString) {
                  data.table = table;
                  data.xcol = xcol;
                  data.ycol1 = ycol;
                  bool isplotable = false;
                  foreach (Data dat, plotted_columns_) {
                    if (data.xcol == dat.xcol && data.ycol1 == dat.ycol1) {
                      isplotable = true;
                      break;
                    }
                  }
                  if (!isplotable) {
                    available_columns_ << data;
                    QListWidgetItem *item = new QListWidgetItem(
                        IconLoader::load("graph2d-pie", IconLoader::LightDark),
                        data.table->name() + "_" + data.ycol1->name() +
                            "(Label: " + data.xcol->name() + ")",
                        available_);
                    available_->addItem(item);
                  }
                }
              }
            }
            break;
          case ApplicationWindow::Graph::Box:
            foreach (Column *ycol, ylist) {
              data.table = table;
              data.ycol1 = ycol;
              bool isplotable = false;
              foreach (Data dat, plotted_columns_) {
                if (data.ycol1 == dat.ycol1) {
                  isplotable = true;
                  break;
                }
              }
              if (!isplotable) {
                available_columns_ << data;
                QListWidgetItem *item = new QListWidgetItem(
                    IconLoader::load("graph2d-box", IconLoader::LightDark),
                    data.table->name() + "_" + data.ycol1->name(), available_);
                available_->addItem(item);
              }
            }
            break;
          case ApplicationWindow::Graph::Histogram: {
            QList<Column *> tempcol;
            tempcol.append(xlist);
            tempcol.append(ylist);

            foreach (Column *col, tempcol) {
              data.table = table;
              data.xcol = col;
              bool isplotable = false;
              foreach (Data dat, plotted_columns_) {
                if (data.xcol == dat.xcol && !dat.ycol1) {
                  isplotable = true;
                  break;
                }
              }
              if (!isplotable) {
                available_columns_ << data;
                QListWidgetItem *item = new QListWidgetItem(
                    IconLoader::load("graph2d-histogram",
                                     IconLoader::LightDark),
                    table->name() + "_" + col->name(), available_);
                available_->addItem(item);
              }
            }
          } break;
          default:
            qDebug() << "unknown availabel_columns_(Type::Table_Y)";
            break;
        }
      }
      break;
    case Type::Table_X_Y:
    case Type::Table_X_Y_Y:
      foreach (Table *table, tables) {
        QList<Column *> ylist;
        QList<Column *> xlist;

        for (int i = 0; i < table->numCols(); i++) {
          if (table->colPlotDesignation(i) == AlphaPlot::PlotDesignation::Y)
            ylist << table->column(i);
          else if (table->colPlotDesignation(i) ==
                   AlphaPlot::PlotDesignation::X)
            xlist << table->column(i);
        }

        Data data;
        data.type = Type::Table_X_Y;
        switch (plotStyle()) {
          case ApplicationWindow::Graph::Channel:
            foreach (Column *xcol, xlist) {
              int temp = 0;
              for (int i = 0; i < ylist.size(); i++) {
                for (int j = temp; j < ylist.size(); j++) {
                  if (ylist.at(i) != ylist.at(j)) {
                    data.table = table;
                    data.xcol = xcol;
                    data.ycol1 = ylist.at(i);
                    data.ycol2 = ylist.at(j);
                    bool isplotable = false;
                    foreach (Data dat, plotted_columns_) {
                      if (dat.ycol2 == nullptr) {
                        if (data.xcol == dat.xcol &&
                            (data.ycol1 == dat.ycol1 ||
                             data.ycol2 == dat.ycol1)) {
                          isplotable = true;
                          break;
                        }
                      } else {
                        if (data.xcol == dat.xcol &&
                            (data.ycol1 == dat.ycol1 ||
                             data.ycol1 == dat.ycol2 ||
                             data.ycol2 == dat.ycol1 ||
                             data.ycol2 == dat.ycol2)) {
                          isplotable = true;
                          break;
                        }
                      }
                    }
                    if (!isplotable) {
                      available_columns_ << data;
                      QListWidgetItem *item = new QListWidgetItem(
                          data.table->name() + "_" + data.xcol->name() + "_" +
                              data.ycol1->name() + "_" + data.ycol2->name(),
                          available_);
                      available_->addItem(item);
                    }
                  }
                }
                temp++;
              }
            }
            break;
          default:
            foreach (Column *xcol, xlist) {
              foreach (Column *ycol, ylist) {
                data.table = table;
                data.xcol = xcol;
                data.ycol1 = ycol;
                bool isplotable = false;
                foreach (Data dat, plotted_columns_) {
                  if (dat.ycol2 != nullptr) {
                    if (data.xcol == dat.xcol &&
                        (data.ycol1 == dat.ycol1 || data.ycol1 == dat.ycol2)) {
                      isplotable = true;
                      break;
                    }
                  } else {
                    if (data.xcol == dat.xcol && data.ycol1 == dat.ycol1) {
                      isplotable = true;
                      break;
                    }
                  }
                }
                if (!isplotable) {
                  available_columns_ << data;
                  QListWidgetItem *item = new QListWidgetItem(
                      data.table->name() + "_" + data.xcol->name() + "_" +
                          data.ycol1->name(),
                      available_);
                  available_->addItem(item);
                }
              }
            }
            break;
        }
      }
      break;
    case Type::Table_X_Y_Y_Y:
      foreach (Table *table, tables) {
        QList<Column *> ylist;
        QList<Column *> xlist;

        for (int i = 0; i < table->numCols(); i++) {
          if (table->colPlotDesignation(i) == AlphaPlot::PlotDesignation::Y)
            ylist << table->column(i);
          else if (table->colPlotDesignation(i) ==
                   AlphaPlot::PlotDesignation::X)
            xlist << table->column(i);
        }

        Data data;
        data.type = Type::Table_X_Y_Y_Y;
        foreach (Column *xcol, xlist) {
          for (int i = 0; i < ylist.size(); i++) {
            for (int j = 0; j < ylist.size(); j++) {
              for (int k = 0; k < ylist.size(); k++) {
                if (ylist.at(i) != ylist.at(j) && ylist.at(i) != ylist.at(k)) {
                  data.table = table;
                  data.xcol = xcol;
                  data.ycol1 = ylist.at(i);
                  data.ycol2 = ylist.at(j);
                  data.ycol3 = ylist.at(k);
                  if (data.ycol1 != data.ycol2 && data.ycol1 != data.ycol3 &&
                      data.ycol2 != data.ycol3) {
                    available_columns_ << data;
                    QListWidgetItem *item = new QListWidgetItem(
                        data.table->name() + "_" + data.xcol->name() + "_" +
                            data.ycol1->name() + "_" + data.ycol2->name() +
                            "_" + data.ycol3->name(),
                        available_);
                    available_->addItem(item);
                  }
                }
              }
            }
          }
        }
      }
      break;
  }
}

bool AddPlot2DDialog::axisColumTypeCompatibilityCheck(Data data) {
  Axis2D::TickerType xtkr =
      xaxis_list_.at(boxXaxis_->currentIndex())->gettickertype_axis();
  Axis2D::TickerType ytkr =
      yaxis_list_.at(boxYaxis_->currentIndex())->gettickertype_axis();

  // check axis colum type compatibility before adding plot(fix later)
  switch (type_) {
    case Type::Table_Y:
      switch (plotStyle()) {
        case ApplicationWindow::Graph::Box:
          if (xtkr == Axis2D::TickerType::Text &&
              (ytkr == Axis2D::TickerType::Value &&
               data.ycol1->dataType() == AlphaPlot::ColumnDataType::TypeDouble))
            return true;
          else if (xtkr == Axis2D::TickerType::Text &&
                   (ytkr == Axis2D::TickerType::Log &&
                    data.ycol1->dataType() ==
                        AlphaPlot::ColumnDataType::TypeDouble))
            return true;
          else if (xtkr == Axis2D::TickerType::Text &&
                   (ytkr == Axis2D::TickerType::Pi &&
                    data.ycol1->dataType() ==
                        AlphaPlot::ColumnDataType::TypeDouble))
            return true;
          else
            return false;
          break;

        case ApplicationWindow::Graph::Histogram:
          if ((xtkr == Axis2D::TickerType::Value ||
               xtkr == Axis2D::TickerType::Pi ||
               xtkr == Axis2D::TickerType::Log) &&
              (ytkr == Axis2D::TickerType::Value &&
               data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeDouble))
            return true;
          else if ((xtkr == Axis2D::TickerType::Value ||
                    xtkr == Axis2D::TickerType::Pi ||
                    xtkr == Axis2D::TickerType::Log) &&
                   (ytkr == Axis2D::TickerType::Pi &&
                    data.xcol->dataType() ==
                        AlphaPlot::ColumnDataType::TypeDouble))
            return true;
          else if ((xtkr == Axis2D::TickerType::Value ||
                    xtkr == Axis2D::TickerType::Pi ||
                    xtkr == Axis2D::TickerType::Log) &&
                   (ytkr == Axis2D::TickerType::Log &&
                    data.xcol->dataType() ==
                        AlphaPlot::ColumnDataType::TypeDouble))
            return true;
          else
            return false;
          break;

        case ApplicationWindow::Graph::Pie:
          if (data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeString &&
              data.ycol1->dataType() == AlphaPlot::ColumnDataType::TypeDouble)
            return true;
          else
            return false;
          break;

        default:
          return false;
      }
      break;
    case Type::Table_X_Y:
    case Type::Table_X_Y_Y:
      if (((xtkr == Axis2D::TickerType::Text &&
            data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeString) ||
           (xtkr == Axis2D::TickerType::Value &&
            data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (xtkr == Axis2D::TickerType::Pi &&
            data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (xtkr == Axis2D::TickerType::Log &&
            data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (xtkr == Axis2D::TickerType::DateTime &&
            data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeDateTime) ||
           (xtkr == Axis2D::TickerType::Time &&
            data.xcol->dataType() ==
                AlphaPlot::ColumnDataType::TypeDateTime)) &&
          ((ytkr == Axis2D::TickerType::Text &&
            data.ycol1->dataType() == AlphaPlot::ColumnDataType::TypeString) ||
           (ytkr == Axis2D::TickerType::Value &&
            data.ycol1->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (ytkr == Axis2D::TickerType::Pi &&
            data.ycol1->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (ytkr == Axis2D::TickerType::Log &&
            data.ycol1->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (ytkr == Axis2D::TickerType::DateTime &&
            data.ycol1->dataType() ==
                AlphaPlot::ColumnDataType::TypeDateTime) ||
           (ytkr == Axis2D::TickerType::Time &&
            data.ycol1->dataType() ==
                AlphaPlot::ColumnDataType::TypeDateTime))) {
        if (plotStyle() == ApplicationWindow::Graph::Channel) {
          if (data.ycol1->dataType() == data.ycol2->dataType())
            return true;
          else
            return false;
        } else {
          return true;
        }
      } else
        return false;
      break;
    case Type::Table_X_Y_Y_Y:
      if (((xtkr == Axis2D::TickerType::Text &&
            data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeString) ||
           (xtkr == Axis2D::TickerType::Value &&
            data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (xtkr == Axis2D::TickerType::Pi &&
            data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (xtkr == Axis2D::TickerType::Log &&
            data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (xtkr == Axis2D::TickerType::DateTime &&
            data.xcol->dataType() == AlphaPlot::ColumnDataType::TypeDateTime) ||
           (xtkr == Axis2D::TickerType::Time &&
            data.xcol->dataType() ==
                AlphaPlot::ColumnDataType::TypeDateTime)) &&
          ((ytkr == Axis2D::TickerType::Text &&
            data.ycol1->dataType() == AlphaPlot::ColumnDataType::TypeString) ||
           (ytkr == Axis2D::TickerType::Value &&
            data.ycol1->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (ytkr == Axis2D::TickerType::Pi &&
            data.ycol1->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (ytkr == Axis2D::TickerType::Log &&
            data.ycol1->dataType() == AlphaPlot::ColumnDataType::TypeDouble) ||
           (ytkr == Axis2D::TickerType::DateTime &&
            data.ycol1->dataType() ==
                AlphaPlot::ColumnDataType::TypeDateTime) ||
           (ytkr == Axis2D::TickerType::Time &&
            data.ycol1->dataType() ==
                AlphaPlot::ColumnDataType::TypeDateTime))) {
        if (plotStyle() == ApplicationWindow::Graph::VectXYAM ||
            plotStyle() == ApplicationWindow::Graph::VectXYXY) {
          if (data.ycol1->dataType() == data.ycol2->dataType() &&
              data.ycol2->dataType() == data.ycol3->dataType())
            return true;
          else
            return false;
        } else {
          return true;
        }
      } else
        return false;
      break;
  }
  return false;
}

void AddPlot2DDialog::addPlots() {
  Data data;
  QList<QListWidgetItem *> lst = available_->selectedItems();

  if (!lst.size() || !axisrect_->getXAxes2D().count() ||
      !axisrect_->getYAxes2D().count())
    return;

  foreach (QListWidgetItem *item, lst) {
    data = available_columns_.at(available_->row(item));
    if (data.table->firstXCol() < 0) {
      QMessageBox::warning(
          this, tr("Error"),
          tr("No X column available in table: ") + data.table->name() + "!");
      return;
    }
    if (rowFromBox_->value() <= 0 || rowToBox_->value() <= 0 ||
        rowToBox_->value() < rowFromBox_->value()) {
      QMessageBox::warning(
          this, tr("Error"),
          tr("From To row range error in table: ") + data.table->name() + "!");
      return;
    }

    if (!axisColumTypeCompatibilityCheck(data)) {
      QMessageBox::warning(
          this, tr("Error"),
          tr("Plot cannot be added due to column type -> axis type mismatch!") +
              data.table->name() + "!");
      return;
    }

    switch (type_) {
      case Type::Table_Y:
        switch (plotStyle()) {
          case ApplicationWindow::Graph::Box: {
            Axis2D *ax = xaxis_list_.at(boxXaxis_->currentIndex());
            QSharedPointer<QCPAxisTickerText> textTicker =
                qSharedPointerCast<QCPAxisTickerText>(ax->getticker_axis());
            double datakey = 1;
            foreach (StatBox2D *box, axisrect_->getStatBoxVec()) {
              if (box->getboxwhiskerdata_statbox().key > datakey)
                datakey = box->getboxwhiskerdata_statbox().key;
            }
            StatBox2D::BoxWhiskerData sboxdata;
            sboxdata = axisrect_->generateBoxWhiskerData(
                data.table, data.ycol1, rowFromBox_->value() - 1,
                rowToBox_->value() - 1, datakey + 1);
            axisrect_->addStatBox2DPlot(
                sboxdata, xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()));
            textTicker->addTick(sboxdata.key, sboxdata.name);
            ax->setTicker(textTicker);
          } break;
          case ApplicationWindow::Graph::Histogram:
            axisrect_->addHistogram2DPlot(
                AxisRect2D::BarType::VerticalBars, data.table, data.xcol,
                rowFromBox_->value() - 1, rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()));
            break;
          case ApplicationWindow::Graph::Pie:
            if (boxStyle_->currentIndex() == 1 && plotnos_ == 0)
              axisrect_->addPie2DPlot(
                  Graph2DCommon::PieStyle::Pie, data.table, data.xcol,
                  data.ycol1, rowFromBox_->value() - 1, rowToBox_->value() - 1);
            else if (boxStyle_->currentIndex() == 2 && plotnos_ == 0)
              axisrect_->addPie2DPlot(
                  Graph2DCommon::PieStyle::HalfPie, data.table, data.xcol,
                  data.ycol1, rowFromBox_->value() - 1, rowToBox_->value() - 1);
            break;
          default:
            qDebug() << "Unknown ApplicationWindow::Graph type";
            break;
        }
        break;
      case Type::Table_X_Y:
      case Type::Table_X_Y_Y:
        switch (plotStyle()) {
          case ApplicationWindow::Graph::Scatter: {
            axisrect_->addCurve2DPlot(
                AxisRect2D::LineScatterType::Scatter2D, data.table, data.xcol,
                data.ycol1, rowFromBox_->value() - 1, rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()));
          } break;
          case ApplicationWindow::Graph::Line: {
            axisrect_->addCurve2DPlot(
                AxisRect2D::LineScatterType::Line2D, data.table, data.xcol,
                data.ycol1, rowFromBox_->value() - 1, rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()));
          } break;
          case ApplicationWindow::Graph::LineSymbols: {
            axisrect_->addCurve2DPlot(
                AxisRect2D::LineScatterType::LineAndScatter2D, data.table,
                data.xcol, data.ycol1, rowFromBox_->value() - 1,
                rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()));
          } break;
          case ApplicationWindow::Graph::VerticalDropLines: {
            axisrect_->addLineSpecial2DPlot(
                AxisRect2D::LineScatterSpecialType::VerticalDropLine2D,
                data.table, data.xcol, data.ycol1, rowFromBox_->value() - 1,
                rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()));
          } break;
          case ApplicationWindow::Graph::Spline: {
            if (rowToBox_->value() - rowFromBox_->value() < 2) {
              QMessageBox::warning(
                  this, tr("Error"),
                  tr("Please select three or more rows for spline plotting!"));
              return;
            }
            axisrect_->addCurve2DPlot(
                AxisRect2D::LineScatterType::Spline2D, data.table, data.xcol,
                data.ycol1, rowFromBox_->value() - 1, rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()));
          } break;
          case ApplicationWindow::Graph::VerticalSteps: {
            axisrect_->addLineSpecial2DPlot(
                AxisRect2D::LineScatterSpecialType::VerticalStep2D, data.table,
                data.xcol, data.ycol1, rowFromBox_->value() - 1,
                rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()));
          } break;
          case ApplicationWindow::Graph::HorizontalSteps: {
            axisrect_->addLineSpecial2DPlot(
                AxisRect2D::LineScatterSpecialType::HorizontalStep2D,
                data.table, data.xcol, data.ycol1, rowFromBox_->value() - 1,
                rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()));
          } break;
          case ApplicationWindow::Graph::Area: {
            axisrect_->addLineSpecial2DPlot(
                AxisRect2D::LineScatterSpecialType::Area2D, data.table,
                data.xcol, data.ycol1, rowFromBox_->value() - 1,
                rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()));
          } break;
          case ApplicationWindow::Graph::VerticalBars: {
            axisrect_->addBox2DPlot(
                AxisRect2D::BarType::VerticalBars, data.table, data.xcol,
                data.ycol1, rowFromBox_->value() - 1, rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()),
                Bar2D::BarStyle::Individual);
          } break;
          case ApplicationWindow::Graph::HorizontalBars: {
            axisrect_->addBox2DPlot(
                AxisRect2D::BarType::HorizontalBars, data.table, data.xcol,
                data.ycol1, rowFromBox_->value() - 1, rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()),
                Bar2D::BarStyle::Individual);
          } break;
          case ApplicationWindow::Graph::Channel: {
            axisrect_->addLineSpecialChannel2DPlot(
                data.table, data.xcol, data.ycol1, data.ycol2,
                rowFromBox_->value() - 1, rowToBox_->value() - 1,
                xaxis_list_.at(boxXaxis_->currentIndex()),
                yaxis_list_.at(boxYaxis_->currentIndex()));
          } break;
          default:
            qDebug() << "Unknown ApplicationWindow::Graph type";
            break;
        }
        break;
      case Type::Table_X_Y_Y_Y:
        switch (plotStyle()) {
          case ApplicationWindow::Graph::VectXYAM:
            if (plotnos_ == 0)
              axisrect_->addVectorPlot(
                  Vector2D::VectorPlot::XYAM, data.table, data.xcol, data.ycol1,
                  data.ycol2, data.ycol3, rowFromBox_->value() - 1,
                  rowToBox_->value() - 1,
                  xaxis_list_.at(boxXaxis_->currentIndex()),
                  yaxis_list_.at(boxYaxis_->currentIndex()));
            break;
          case ApplicationWindow::Graph::VectXYXY:
            if (plotnos_ == 0)
              axisrect_->addVectorPlot(
                  Vector2D::VectorPlot::XYXY, data.table, data.xcol, data.ycol1,
                  data.ycol2, data.ycol3, rowFromBox_->value() - 1,
                  rowToBox_->value() - 1,
                  xaxis_list_.at(boxXaxis_->currentIndex()),
                  yaxis_list_.at(boxYaxis_->currentIndex()));
            break;
          default:
            qDebug() << "Unknown ApplicationWindow::Graph type";
            break;
        }
        break;
    }
  }
  axisrect_->replotBareBones();
  populatePlotted();
  populateAvailable();
  enableAddBtn();
}

void AddPlot2DDialog::removePlots() {
  QList<QListWidgetItem *> lst = contents_->selectedItems();
  if (lst.size() != 1) return;
  int row = contents_->currentRow();
  Data data = plotted_columns_.at(row);

  QVector<StatBox2D *> boxlist = axisrect_->getStatBoxVec();
  QVector<Pie2D *> pielist = axisrect_->getPieVec();
  QVector<LineSpecial2D *> lslist = axisrect_->getLsVec();
  QVector<Curve2D *> curvelist = axisrect_->getCurveVec();
  QVector<Bar2D *> barlist = axisrect_->getBarVec();
  QVector<QPair<LineSpecial2D *, LineSpecial2D *>> channellist =
      axisrect_->getChannelVec();
  QVector<Vector2D *> veclist = axisrect_->getVectorVec();

  switch (data.type) {
    case Type::Table_Y:
      foreach (StatBox2D *sbox, boxlist) {
        if (sbox->getboxwhiskerdata_statbox().table_ == data.table &&
            data.xcol == nullptr &&
            sbox->getboxwhiskerdata_statbox().column_ == data.ycol1 &&
            data.ycol2 == nullptr && data.ycol3 == nullptr) {
          axisrect_->removeStatBox2D(sbox);
          populatePlotted();
          populateAvailable();
          return;
        }
      }
      foreach (Bar2D *bar, barlist) {
        if (bar->ishistogram_barplot())
          if (bar->getdatablock_histplot()->gettable() == data.table &&
              bar->getdatablock_histplot()->getcolumn() == data.xcol &&
              data.ycol1 == nullptr && data.ycol2 == nullptr &&
              data.ycol3 == nullptr) {
            axisrect_->removeBar2D(bar);
            populatePlotted();
            populateAvailable();
            return;
          }
      }
      foreach (Pie2D *pie, pielist) {
        if (pie->gettable_pieplot() == data.table &&
            pie->getxcolumn_pieplot() == data.xcol &&
            pie->getycolumn_pieplot() == data.ycol1 && data.ycol2 == nullptr &&
            data.ycol3 == nullptr) {
          axisrect_->removePie2D(pie);
          populatePlotted();
          populateAvailable();
          return;
        }
      }
      break;
    case Type::Table_X_Y:
    case Type::Table_X_Y_Y:
      foreach (LineSpecial2D *ls, lslist) {
        if (ls->getdatablock_lsplot()->gettable() == data.table &&
            ls->getdatablock_lsplot()->getxcolumn() == data.xcol &&
            ls->getdatablock_lsplot()->getycolumn() == data.ycol1 &&
            data.ycol2 == nullptr && data.ycol3 == nullptr) {
          axisrect_->removeLineSpecial2D(ls);
          populatePlotted();
          populateAvailable();
          return;
        }
      }
      foreach (Curve2D *curve, curvelist) {
        if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Associated) {
          if (curve->getdatablock_cplot()->gettable() == data.table &&
              curve->getdatablock_cplot()->getxcolumn() == data.xcol &&
              curve->getdatablock_cplot()->getycolumn() == data.ycol1 &&
              data.ycol2 == nullptr && data.ycol3 == nullptr) {
            axisrect_->removeCurve2D(curve);
            populatePlotted();
            populateAvailable();
            return;
          }
        } else {
          PlotData::FunctionData fd = curve->getfuncdata_cplot();
          if (fd.type == data.fd.type && fd.functions == data.fd.functions &&
              fd.parameter == data.fd.parameter &&
              fd.points == data.fd.points && fd.from == data.fd.from &&
              fd.to == data.fd.to) {
            axisrect_->removeCurve2D(curve);
            populatePlotted();
            return;
          }
        }
      }
      foreach (Bar2D *bar, barlist) {
        if (!bar->ishistogram_barplot())
          if (bar->getdatablock_barplot()->gettable() == data.table &&
              bar->getdatablock_barplot()->getxcolumn() == data.xcol &&
              bar->getdatablock_barplot()->getycolumn() == data.ycol1 &&
              data.ycol2 == nullptr && data.ycol3 == nullptr) {
            axisrect_->removeBar2D(bar);
            populatePlotted();
            populateAvailable();
            return;
          }
      }
      for (int i = 0; i < channellist.size(); i++) {
        QPair<LineSpecial2D *, LineSpecial2D *> pair = channellist.at(i);
        if (pair.first->getdatablock_lsplot()->gettable() == data.table &&
            pair.first->getdatablock_lsplot()->getxcolumn() == data.xcol &&
            pair.first->getdatablock_lsplot()->getycolumn() == data.ycol1 &&
            pair.second->getdatablock_lsplot()->getycolumn() == data.ycol2 &&
            data.ycol3 == nullptr) {
          axisrect_->removeChannel2D(pair);
          populatePlotted();
          populateAvailable();
          return;
        }
      }
      break;
    case Type::Table_X_Y_Y_Y:
      foreach (Vector2D *vec, veclist) {
        if (vec->gettable_vecplot() == data.table &&
            vec->getfirstcol_vecplot() == data.xcol &&
            vec->getsecondcol_vecplot() == data.ycol1 &&
            vec->getthirdcol_vecplot() == data.ycol2 &&
            vec->getfourthcol_vecplot() == data.ycol3) {
          axisrect_->removeVector2D(vec);
          populatePlotted();
          populateAvailable();
          return;
        }
      }
      break;
  }
}

void AddPlot2DDialog::enableAddBtn() {
  if (available_->selectedItems().count() && available_columns_.count()) {
    xaxisLabel_->setEnabled(true);
    boxXaxis_->setEnabled(true);
    yaxisLabel_->setEnabled(true);
    boxYaxis_->setEnabled(true);
    groupBox_->setEnabled(true);
    btnAdd_->setEnabled(true);
    Data data = available_columns_.at(available_->currentRow());
    bool nocolumnnull = false;
    bool ishistogram = false;
    switch (data.type) {
      case Type::Table_Y:
        switch (plotStyle()) {
          case ApplicationWindow::Graph::Box:
            if (data.ycol1->rowCount()) nocolumnnull = true;
            break;
          case ApplicationWindow::Graph::Histogram:
            if (data.xcol->rowCount()) nocolumnnull = true;
            ishistogram = true;
            break;
          case ApplicationWindow::Graph::Pie:
            if (data.ycol1->rowCount() && data.xcol->rowCount())
              nocolumnnull = true;
            break;
          default:
            qDebug() << "Unknown Type::Table_Y curvestyle";
            break;
        }
        break;
      case Type::Table_X_Y:
      case Type::Table_X_Y_Y:
        switch (plotStyle()) {
          case ApplicationWindow::Graph::Channel:
            if (data.ycol1->rowCount() && data.xcol->rowCount() &&
                data.ycol2->rowCount())
              nocolumnnull = true;
            break;
          default:
            if (data.ycol1->rowCount() && data.xcol->rowCount())
              nocolumnnull = true;
            break;
        }
        break;
      case Type::Table_X_Y_Y_Y:
        if (data.ycol1->rowCount() && data.xcol->rowCount() &&
            data.ycol2->rowCount() && data.ycol3->rowCount())
          nocolumnnull = true;
        break;
    }
    if ((nocolumnnull && data.ycol1) ||
        (nocolumnnull && data.xcol && ishistogram)) {
      if (!ishistogram) {
        rowFromBox_->setRange(1, data.ycol1->rowCount());
        rowToBox_->setRange(1, data.ycol1->rowCount());
        rowFromBox_->setValue(1);
        rowToBox_->setValue(data.ycol1->rowCount());
      } else {
        rowFromBox_->setRange(1, data.xcol->rowCount());
        rowToBox_->setRange(1, data.xcol->rowCount());
        rowFromBox_->setValue(1);
        rowToBox_->setValue(data.xcol->rowCount());
      }
    } else {
      xaxisLabel_->setEnabled(false);
      boxXaxis_->setEnabled(false);
      yaxisLabel_->setEnabled(false);
      boxYaxis_->setEnabled(false);
      groupBox_->setEnabled(false);
      btnAdd_->setEnabled(false);
      rowFromBox_->setRange(0, 0);
      rowToBox_->setRange(0, 0);
      rowFromBox_->setValue(0);
      rowToBox_->setValue(0);
    }
  } else {
    xaxisLabel_->setEnabled(false);
    boxXaxis_->setEnabled(false);
    yaxisLabel_->setEnabled(false);
    boxYaxis_->setEnabled(false);
    groupBox_->setEnabled(false);
    btnAdd_->setEnabled(false);
    rowFromBox_->setRange(0, 0);
    rowToBox_->setRange(0, 0);
    rowFromBox_->setValue(0);
    rowToBox_->setValue(0);
  }
}

ApplicationWindow::Graph AddPlot2DDialog::plotStyle() {
  ApplicationWindow::Graph style = ApplicationWindow::Graph::Scatter;
  switch (type_) {
    case Type::Table_Y:
      switch (boxStyle_->currentIndex()) {
        case 0:
          style = ApplicationWindow::Graph::Box;
          break;
        case 1:
          style = ApplicationWindow::Graph::Histogram;
          break;
        case 2:
          style = ApplicationWindow::Graph::Pie;
          break;
        case 3:
          style = ApplicationWindow::Graph::Pie;
          break;
      }
      break;
    case Type::Table_X_Y:
    case Type::Table_X_Y_Y:
      switch (boxStyle_->currentIndex()) {
        case 0:
          style = ApplicationWindow::Graph::Scatter;
          break;
        case 1:
          style = ApplicationWindow::Graph::Line;
          break;
        case 2:
          style = ApplicationWindow::Graph::LineSymbols;
          break;
        case 3:
          style = ApplicationWindow::Graph::VerticalDropLines;
          break;
        case 4:
          style = ApplicationWindow::Graph::Spline;
          break;
        case 5:
          style = ApplicationWindow::Graph::VerticalSteps;
          break;
        case 6:
          style = ApplicationWindow::Graph::HorizontalSteps;
          break;
        case 7:
          style = ApplicationWindow::Graph::Area;
          break;
        case 8:
          style = ApplicationWindow::Graph::VerticalBars;
          break;
        case 9:
          style = ApplicationWindow::Graph::HorizontalBars;
          break;
        case 10:
          style = ApplicationWindow::Graph::Channel;
          break;
      }
      break;
    case Type::Table_X_Y_Y_Y:
      switch (boxStyle_->currentIndex()) {
        case 0:
          style = ApplicationWindow::Graph::VectXYAM;
          break;
        case 1:
          style = ApplicationWindow::Graph::VectXYXY;
          break;
      }
      break;
  }

  return style;
}

void AddPlot2DDialog::closeEvent(QCloseEvent *event) {
  app_->d_add_curves_dialog_size = this->size();
  event->accept();
}
