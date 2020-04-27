#include "Layout2D.h"

#include <gsl/gsl_sort.h>
#include <gsl/gsl_statistics.h>

#include <QDateTime>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QStyleOption>
#include <QToolTip>
#include <QVBoxLayout>

#include "AxisRect2D.h"
#include "Bar2D.h"
#include "ColorMap2D.h"
#include "Curve2D.h"
#include "DataManager2D.h"
#include "ErrorBar2D.h"
#include "LayoutGrid2D.h"
#include "Legend2D.h"
#include "LineSpecial2D.h"
#include "Matrix.h"
#include "Table.h"
#include "TextItem2D.h"
#include "core/IconLoader.h"
#include "core/Utilities.h"
#include "future/core/datatypes/DateTime2StringFilter.h"
#include "future/lib/XmlStreamWriter.h"
#include "plotcommon/widgets/ImageExportDialog.h"
#include "widgets/LayoutButton2D.h"

const int Layout2D::buttonboxmargin_ = 2;
const int Layout2D::defaultlayout2dwidth_ = 500;
const int Layout2D::defaultlayout2dheight_ = 400;
const int Layout2D::minimumlayout2dwidth_ = 100;
const int Layout2D::minimumlayout2dheight_ = 100;

Layout2D::Layout2D(const QString &label, QWidget *parent, const QString name,
                   Qt::WindowFlags f)
    : MyWidget(label, parent, name, f),
      main_widget_(new QWidget(this)),
      plot2dCanvas_(new Plot2D(main_widget_)),
      layout_(new LayoutGrid2D()),
      buttionlist_(QList<LayoutButton2D *>()),
      currentAxisRect_(nullptr),
      picker_(Graph2DCommon::Picker::None),
      xpickerline_(nullptr),
      ypickerline_(nullptr) {
  main_widget_->setContentsMargins(0, 0, 0, 0);
  if (name.isEmpty()) setObjectName("layout2d plot");
  QDateTime birthday = QDateTime::currentDateTime();
  setBirthDate(birthday.toString(Qt::LocalDate));

  layoutManagebuttonsBox_ = new QHBoxLayout();
  addLayoutButton_ = new QPushButton();
  addLayoutButton_->setToolTip(tr("Add layer"));
  addLayoutButton_->setMaximumWidth(LayoutButton2D::btnSize());
  addLayoutButton_->setMaximumHeight(LayoutButton2D::btnSize());
  layoutManagebuttonsBox_->addWidget(addLayoutButton_);

  removeLayoutButton_ = new QPushButton();
  removeLayoutButton_->setToolTip(tr("Remove active layer"));
  removeLayoutButton_->setMaximumWidth(LayoutButton2D::btnSize());
  removeLayoutButton_->setMaximumHeight(LayoutButton2D::btnSize());
  layoutManagebuttonsBox_->addWidget(removeLayoutButton_);

  layoutButtonsBox_ = new QHBoxLayout();
  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addLayout(layoutButtonsBox_);
  streachLabel_ = new QLabel();
  hbox->addWidget(streachLabel_);
  hbox->addLayout(layoutManagebuttonsBox_);
  setBackground(plot2dCanvas_->getBackgroundColor());
  layoutButtonsBox_->setContentsMargins(buttonboxmargin_, buttonboxmargin_,
                                        buttonboxmargin_, buttonboxmargin_);
  layoutManagebuttonsBox_->setContentsMargins(
      buttonboxmargin_, buttonboxmargin_, buttonboxmargin_, buttonboxmargin_);
  layoutButtonsBox_->setSpacing(buttonboxmargin_);
  layoutManagebuttonsBox_->setSpacing(buttonboxmargin_);

  QVBoxLayout *layout = new QVBoxLayout(main_widget_);
  layout->addLayout(hbox);
  layout->addWidget(plot2dCanvas_, 1);
  setWidget(main_widget_);
  layout->setMargin(0);
  layout->setSpacing(0);
  setGeometry(QRect(0, 0, defaultlayout2dwidth_, defaultlayout2dheight_));
  setMinimumSize(QSize(minimumlayout2dwidth_, minimumlayout2dheight_));
  setFocusPolicy(Qt::StrongFocus);

  plot2dCanvas_->plotLayout()->addElement(0, 0, layout_);
  loadIcons();

  // connections
  connect(addLayoutButton_, SIGNAL(clicked()), this,
          SLOT(addAxisRectWithAxis()));
  connect(removeLayoutButton_, &QPushButton::clicked, this,
          &Layout2D::removeAxisRectItem);
  connect(plot2dCanvas_, &Plot2D::mouseMove, this, &Layout2D::mouseMoveSignal);
  connect(plot2dCanvas_, &Plot2D::mousePress, this,
          &Layout2D::mousePressSignal);
  connect(plot2dCanvas_, &Plot2D::mouseRelease, this,
          &Layout2D::mouseReleaseSignal);
  connect(plot2dCanvas_, &Plot2D::mouseWheel, this, &Layout2D::mouseWheel);
  connect(plot2dCanvas_, &Plot2D::beforeReplot, this, &Layout2D::beforeReplot);
  connect(plot2dCanvas_, &Plot2D::backgroundColorChange, this,
          &Layout2D::setBackground);
}

Layout2D::~Layout2D() {
  delete layout_;
  if (xpickerline_) {
    plot2dCanvas_->removeItem(xpickerline_);
    xpickerline_ = nullptr;
  }
  if (ypickerline_) {
    plot2dCanvas_->removeItem(ypickerline_);
    ypickerline_ = nullptr;
  }
}

StatBox2D::BoxWhiskerData Layout2D::generateBoxWhiskerData(Table *table,
                                                           Column *colData,
                                                           int from, int to,
                                                           int key) {
  size_t size = static_cast<size_t>((to - from) + 1);

  double *data = new double[size];

  for (int i = 0, j = from; j < to + 1; i++, j++) {
    data[i] = colData->valueAt(i);
  }
  // sort the data
  gsl_sort(data, 1, size);

  StatBox2D::BoxWhiskerData statBoxData;
  statBoxData.table_ = table;
  statBoxData.column_ = colData;
  statBoxData.from_ = from;
  statBoxData.to_ = to;
  statBoxData.key = key;
  // basic stats
  statBoxData.mean = gsl_stats_mean(data, 1, size);
  statBoxData.median = gsl_stats_median_from_sorted_data(data, 1, size);
  statBoxData.sd = gsl_stats_sd(data, 1, size);
  statBoxData.se = statBoxData.sd / sqrt(static_cast<double>(size));
  // data bounds
  statBoxData.boxWhiskerDataBounds.sd_lower = statBoxData.mean - statBoxData.sd;
  statBoxData.boxWhiskerDataBounds.sd_upper = statBoxData.mean + statBoxData.sd;
  statBoxData.boxWhiskerDataBounds.se_lower = statBoxData.mean - statBoxData.se;
  statBoxData.boxWhiskerDataBounds.se_upper = statBoxData.mean + statBoxData.se;
  statBoxData.boxWhiskerDataBounds.perc_1 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.01);
  statBoxData.boxWhiskerDataBounds.perc_5 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.05);
  statBoxData.boxWhiskerDataBounds.perc_10 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.10);
  statBoxData.boxWhiskerDataBounds.perc_25 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.25);
  statBoxData.boxWhiskerDataBounds.perc_75 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.75);
  statBoxData.boxWhiskerDataBounds.perc_90 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.90);
  statBoxData.boxWhiskerDataBounds.perc_95 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.95);
  statBoxData.boxWhiskerDataBounds.perc_99 =
      gsl_stats_quantile_from_sorted_data(data, 1, size, 0.99);
  statBoxData.boxWhiskerDataBounds.max = data[size - 1];
  statBoxData.boxWhiskerDataBounds.min = data[0];
  statBoxData.name = colData->name();

  // delete the double data pointer
  delete[] data;

  return statBoxData;
}

void Layout2D::generateFunction2DPlot(QVector<double> *xdata,
                                      QVector<double> *ydata,
                                      const QString xLabel,
                                      const QString yLabel) {
  AxisRect2D *element = addAxisRectItem(AlphaPlot::ColumnDataType::TypeDouble,
                                        AlphaPlot::ColumnDataType::TypeDouble);
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
  xAxis.at(0)->setLabel(xLabel);
  yAxis.at(0)->setLabel(yLabel);

  QString name = "f(" + xLabel + ") : " + yLabel;
  Curve2D *curve =
      element->addFunction2DPlot(xdata, ydata, xAxis.at(0), yAxis.at(0), name);
  curve->rescaleAxes();
}

QList<StatBox2D *> Layout2D::generateStatBox2DPlot(Table *table,
                                                   QList<Column *> ycollist,
                                                   int from, int to, int key) {
  QList<StatBox2D *> statboxs;
  QList<StatBox2D::BoxWhiskerData> statBoxData;
  foreach (Column *col, ycollist) {
    statBoxData << generateBoxWhiskerData(table, col, from, to, key);
  }

  AxisRect2D *element = addAxisRectItem(AlphaPlot::ColumnDataType::TypeString,
                                        AlphaPlot::ColumnDataType::TypeDouble);
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);

  QSharedPointer<QCPAxisTickerText> textTicker =
      qSharedPointerCast<QCPAxisTickerText>(xAxis.at(0)->getticker_axis());
  for (int i = 0; i < statBoxData.size(); i++) {
    StatBox2D::BoxWhiskerData data = statBoxData.at(i);
    data.key = i + 1;
    statboxs << element->addStatBox2DPlot(data, xAxis.at(0), yAxis.at(0));
    textTicker->addTick(data.key, data.name);
  }
  xAxis.at(0)->setTicker(textTicker);

  xAxis.at(0)->rescale();
  yAxis.at(0)->rescale();
  QCPRange keyRange = xAxis.at(0)->range();
  QCPRange valueRange = yAxis.at(0)->range();
  double keyRangeSpan = keyRange.upper - keyRange.lower;
  double valueRangeSpan = valueRange.upper - valueRange.lower;
  xAxis.at(0)->setRange(QCPRange(keyRange.lower - keyRangeSpan * 0.2,
                                 keyRange.upper + keyRangeSpan * 0.2));
  yAxis.at(0)->setRange(QCPRange(valueRange.lower - valueRangeSpan * 0.2,
                                 valueRange.upper + valueRangeSpan * 0.2));
  return statboxs;
}

void Layout2D::generateHistogram2DPlot(const AxisRect2D::BarType &barType,
                                       bool multilayout, Table *table,
                                       QList<Column *> collist, int from,
                                       int to) {
  if (multilayout) {
    foreach (Column *col, collist) {
      AxisRect2D *element =
          addAxisRectItem(AlphaPlot::ColumnDataType::TypeDouble,
                          AlphaPlot::ColumnDataType::TypeDouble);
      QList<Axis2D *> xAxis =
          element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
      xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
      QList<Axis2D *> yAxis =
          element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
      yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
      element->addHistogram2DPlot(barType, table, col, from, to, xAxis.at(0),
                                  yAxis.at(0));
      xAxis.at(0)->rescale();
      yAxis.at(0)->rescale();
    }
  } else {
    AxisRect2D *element =
        addAxisRectItem(AlphaPlot::ColumnDataType::TypeDouble,
                        AlphaPlot::ColumnDataType::TypeDouble);
    QList<Axis2D *> xAxis =
        element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
    xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
    QList<Axis2D *> yAxis =
        element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
    yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
    foreach (Column *col, collist) {
      element->addHistogram2DPlot(barType, table, col, from, to, xAxis.at(0),
                                  yAxis.at(0));
    }
    xAxis.at(0)->rescale();
    yAxis.at(0)->rescale();
  }
}

void Layout2D::generateBar2DPlot(const AxisRect2D::BarType &barType,
                                 Table *table, Column *xData,
                                 QList<Column *> ycollist, int from, int to) {
  AxisRect2D *element = nullptr;
  QList<Axis2D *> xAxis;
  QList<Axis2D *> yAxis;
  switch (barType) {
    case AxisRect2D::BarType::HorizontalBars: {
      element = addAxisRectItem(ycollist.at(0)->dataType(), xData->dataType());
      xAxis = element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
      xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
      yAxis = element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
      yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
      addTextToAxisTicker(xData, yAxis.at(0), from, to);
      addTextToAxisTicker(ycollist.at(0), xAxis.at(0), from, to);
    } break;
    case AxisRect2D::BarType::VerticalBars: {
      element = addAxisRectItem(xData->dataType(), ycollist.at(0)->dataType());
      xAxis = element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
      xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
      yAxis = element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
      yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
      addTextToAxisTicker(xData, xAxis.at(0), from, to);
      addTextToAxisTicker(ycollist.at(0), yAxis.at(0), from, to);
    } break;
  }

  foreach (Column *col, ycollist) {
    Bar2D *bar = element->addBox2DPlot(barType, table, xData, col, from, to,
                                       xAxis.at(0), yAxis.at(0));
    bar->rescaleAxes();
  }
}

void Layout2D::generateStakedBar2DPlot(const AxisRect2D::BarType &barType,
                                       Table *table, Column *xData,
                                       QList<Column *> ycollist, int from,
                                       int to) {
  AxisRect2D *element = nullptr;
  QList<Axis2D *> xAxis;
  QList<Axis2D *> yAxis;
  switch (barType) {
    case AxisRect2D::BarType::HorizontalBars: {
      element = addAxisRectItem(ycollist.at(0)->dataType(), xData->dataType());
      xAxis = element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
      xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
      yAxis = element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
      yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
      addTextToAxisTicker(xData, yAxis.at(0), from, to);
      addTextToAxisTicker(ycollist.at(0), xAxis.at(0), from, to);
    } break;
    case AxisRect2D::BarType::VerticalBars: {
      element = addAxisRectItem(xData->dataType(), ycollist.at(0)->dataType());
      xAxis = element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
      xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
      yAxis = element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
      yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
      addTextToAxisTicker(xData, xAxis.at(0), from, to);
      addTextToAxisTicker(ycollist.at(0), yAxis.at(0), from, to);
    } break;
  }

  QList<Bar2D *> bars;
  for (int i = 0; i < ycollist.size(); i++) {
    Bar2D *bar = element->addBox2DPlot(barType, table, xData, ycollist.at(i),
                                       from, to, xAxis.at(0), yAxis.at(0), i);
    bars.append(bar);
    bar->setStackingGap(1);
  }
  // create the stack
  Bar2D *basebar = nullptr;
  foreach (Bar2D *bar, bars) {
    if (basebar) bar->moveAbove(basebar);
    basebar = bar;
  }
}

void Layout2D::generateVector2DPlot(const Vector2D::VectorPlot &vectorplot,
                                    Table *table, Column *x1Data,
                                    Column *y1Data, Column *x2Data,
                                    Column *y2Data, int from, int to) {
  AxisRect2D *element = addAxisRectItem(AlphaPlot::ColumnDataType::TypeDouble,
                                        AlphaPlot::ColumnDataType::TypeDouble);
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);

  Vector2D *vex =
      element->addVectorPlot(vectorplot, table, x1Data, y1Data, x2Data, y2Data,
                             from, to, xAxis.at(0), yAxis.at(0));
  vex->rescaleAxes();
}

void Layout2D::generatePie2DPlot(Table *table, Column *xData, Column *yData,
                                 int from, int to) {
  AxisRect2D *element = addAxisRectItem(AlphaPlot::ColumnDataType::TypeDouble,
                                        AlphaPlot::ColumnDataType::TypeDouble);

  element->addPie2DPlot(table, xData, yData, from, to);
}

void Layout2D::generateColorMap2DPlot(Matrix *matrix, bool greyscale,
                                      bool contour) {
  AxisRect2D *element = addAxisRectItem(AlphaPlot::ColumnDataType::TypeDouble,
                                        AlphaPlot::ColumnDataType::TypeDouble);
  setLayoutButtonBoxVisible(false);
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
  ColorMap2D *colormap =
      element->addColorMap2DPlot(matrix, xAxis.at(0), yAxis.at(0));
  if (greyscale)
    colormap->setgradient_colormap(ColorMap2D::Gradient::Grayscale);
  if (contour) {
    colormap->setlevelcount_colormap(5);
  }
  colormap->rescaleAxes();
}

QList<AxisRect2D *> Layout2D::getAxisRectList() {
  QList<AxisRect2D *> elementslist;
  for (int i = 0; i < layout_->elementCount(); i++) {
    AxisRect2D *axisrect = dynamic_cast<AxisRect2D *>(layout_->elementAt(i));
    if (axisrect)
      elementslist.append(axisrect);
    else
      qDebug() << "unable to cast AxisRect2D";
  }
  return elementslist;
}

void Layout2D::generateLineSpecial2DPlot(
    const AxisRect2D::LineScatterSpecialType &plotType, Table *table,
    Column *xData, QList<Column *> ycollist, int from, int to) {
  AxisRect2D *element =
      addAxisRectItem(xData->dataType(), ycollist.at(0)->dataType());
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
  addTextToAxisTicker(xData, xAxis.at(0), from, to);
  addTextToAxisTicker(ycollist.at(0), yAxis.at(0), from, to);
  foreach (Column *col, ycollist) {
    LineSpecial2D *linescatter = element->addLineSpecial2DPlot(
        plotType, table, xData, col, from, to, xAxis.at(0), yAxis.at(0));
    linescatter->rescaleAxes();
  }
}

void Layout2D::generateLineSpecialChannel2DPlot(Table *table, Column *xData,
                                                QList<Column *> ycollist,
                                                int from, int to) {
  Q_ASSERT(ycollist.count() == 2);
  AxisRect2D *element =
      addAxisRectItem(xData->dataType(), ycollist.at(0)->dataType());
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
  addTextToAxisTicker(xData, xAxis.at(0), from, to);
  addTextToAxisTicker(ycollist.at(0), yAxis.at(0), from, to);

  element->addLineSpecialChannel2DPlot(table, xData, ycollist.at(0),
                                       ycollist.at(1), from, to, xAxis.at(0),
                                       yAxis.at(0));
  xAxis.at(0)->rescale();
  yAxis.at(0)->rescale();
}

void Layout2D::generateCurve2DPlot(const AxisRect2D::LineScatterType &plotType,
                                   Table *table, Column *xcol,
                                   QList<Column *> ycollist, int from, int to) {
  AxisRect2D *element =
      addAxisRectItem(xcol->dataType(), ycollist.at(0)->dataType());
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
  addTextToAxisTicker(xcol, xAxis.at(0), from, to);
  addTextToAxisTicker(ycollist.at(0), yAxis.at(0), from, to);
  foreach (Column *col, ycollist) {
    Curve2D *curve = element->addCurve2DPlot(plotType, table, xcol, col, from,
                                             to, xAxis.at(0), yAxis.at(0));
    curve->rescaleAxes();
  }
}

AxisRect2D *Layout2D::getSelectedAxisRect(int col, int row) {
  return static_cast<AxisRect2D *>(layout_->element(row, col));
}

int Layout2D::getAxisRectIndex(AxisRect2D *axisRect2d) {
  AxisRect2D *axisRect;
  for (int i = 0; i < layout_->elementCount(); i++) {
    axisRect = static_cast<AxisRect2D *>(layout_->elementAt(i));
    if (axisRect == axisRect2d) return i;
  }

  return static_cast<int>(std::nan("invalid index"));
}

AxisRect2D *Layout2D::getCurrentAxisRect() { return currentAxisRect_; }

Plot2D *Layout2D::getPlotCanwas() const { return plot2dCanvas_; }

AxisRect2D *Layout2D::addAxisRectItem(
    const AlphaPlot::ColumnDataType &xcoldatatype,
    const AlphaPlot::ColumnDataType &ycoldatatype) {
  /*int rowcount = layout_->rowCount();
  int colcount = layout_->columnCount();

  int lastIndex = layout_->elementCount() - 1;
  int col = 0, row = 0;
  if (lastIndex + 1 > 0) {
    int maxLastIndex = (rowcount * colcount) - 1;

    if (lastIndex == maxLastIndex) {
      // col = colcount;
      // row = 0;
      col = colcount;
      row = 0;
    } else {
      col = colcount;
      row = 0;
    }
  }*/
  Q_ASSERT(xcoldatatype != AlphaPlot::ColumnDataType::TypeDay &&
           ycoldatatype != AlphaPlot::ColumnDataType::TypeDay &&
           xcoldatatype != AlphaPlot::ColumnDataType::TypeMonth &&
           ycoldatatype != AlphaPlot::ColumnDataType::TypeMonth);

  int row = 0;
  int col = layout_->elementCount();

  AxisRect2D *axisRect2d = new AxisRect2D(plot2dCanvas_);
  // axisrectitem->setData(0, Qt::UserRole, 3);
  Axis2D *xAxis = nullptr;
  switch (xcoldatatype) {
    case AlphaPlot::ColumnDataType::TypeDouble:
      xAxis = axisRect2d->addAxis2D(Axis2D::AxisOreantation::Bottom,
                                    Axis2D::TickerType::Value);
      break;
    case AlphaPlot::ColumnDataType::TypeDateTime:
      xAxis = axisRect2d->addAxis2D(Axis2D::AxisOreantation::Bottom,
                                    Axis2D::TickerType::DateTime);
      break;
    case AlphaPlot::ColumnDataType::TypeString: {
      xAxis = axisRect2d->addAxis2D(Axis2D::AxisOreantation::Bottom,
                                    Axis2D::TickerType::Text);
      break;
    }
    default:
      qDebug() << "invalid data type" << xcoldatatype;
  }

  Axis2D *yAxis = nullptr;
  switch (ycoldatatype) {
    case AlphaPlot::ColumnDataType::TypeDouble:
      yAxis = axisRect2d->addAxis2D(Axis2D::AxisOreantation::Left,
                                    Axis2D::TickerType::Value);
      break;
    case AlphaPlot::ColumnDataType::TypeDateTime:
      yAxis = axisRect2d->addAxis2D(Axis2D::AxisOreantation::Left,
                                    Axis2D::TickerType::DateTime);
      break;
    case AlphaPlot::ColumnDataType::TypeString:
      yAxis = axisRect2d->addAxis2D(Axis2D::AxisOreantation::Left,
                                    Axis2D::TickerType::Text);
      break;
    default:
      qDebug() << "invalid data type" << xcoldatatype;
  }

  axisRect2d->bindGridTo(xAxis);
  axisRect2d->bindGridTo(yAxis);

  xAxis->setRange(0, 100);
  yAxis->setRange(0, 100);
  xAxis->setLabel("X Axis Title");
  yAxis->setLabel("Y Axis Title");
  layout_->addElement(row, col, axisRect2d);

  plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
  addLayoutButton(col);

  connect(axisRect2d, &AxisRect2D::AxisRectClicked, this,
          &Layout2D::axisRectSetFocus);
  connect(axisRect2d, &AxisRect2D::showtooltip, this, &Layout2D::showtooltip);

  emit AxisRectCreated(axisRect2d, this);
  if (!currentAxisRect_) axisRectSetFocus(axisRect2d);
  return axisRect2d;
}

void Layout2D::removeAxisRectItem() {
  if (!currentAxisRect_) return;
  foreach (TextItem2D *item, currentAxisRect_->getTextItemVec()) {
    currentAxisRect_->removeTextItem2D(item);
  }
  foreach (LineItem2D *item, currentAxisRect_->getLineItemVec()) {
    currentAxisRect_->removeLineItem2D(item);
  }
  foreach (ImageItem2D *item, currentAxisRect_->getImageItemVec()) {
    currentAxisRect_->removeImageItem2D(item);
  }
  foreach (LineSpecial2D *ls, currentAxisRect_->getLsVec()) {
    currentAxisRect_->removeLineSpecial2D(ls);
  }
  for (int i = 0; i < currentAxisRect_->getChannelVec().count(); i++) {
    currentAxisRect_->removeChannel2D(currentAxisRect_->getChannelVec().at(i));
  }
  foreach (Curve2D *curve, currentAxisRect_->getCurveVec()) {
    currentAxisRect_->removeCurve2D(curve);
  }
  foreach (Bar2D *bar, currentAxisRect_->getBarVec()) {
    currentAxisRect_->removeBar2D(bar);
  }
  foreach (StatBox2D *statbox, currentAxisRect_->getStatBoxVec()) {
    currentAxisRect_->removeStatBox2D(statbox);
  }
  foreach (Vector2D *vector, currentAxisRect_->getVectorVec()) {
    currentAxisRect_->removeVector2D(vector);
  }
  foreach (Pie2D *pie, currentAxisRect_->getPieVec()) {
    currentAxisRect_->removePie2D(pie);
  }
  auto gridpair = currentAxisRect_->getGridPair();
  delete gridpair.first.first;
  delete gridpair.second.first;
  foreach (Axis2D *axis, currentAxisRect_->getAxes2D()) {
    currentAxisRect_->removeAxis2D(axis, true);
  }

  removeAxisRect(getAxisRectIndex(currentAxisRect_));
  emit AxisRectRemoved(this);
}

void Layout2D::axisRectSetFocus(AxisRect2D *rect) {
  if (!rect) return;

  LayoutButton2D *button = nullptr;
  if (currentAxisRect_) {
    if (currentAxisRect_ != rect) {
      currentAxisRect_->setSelected(false);
      button = buttionlist_.at(getAxisRectIndex(currentAxisRect_));
      if (button) button->setActive(false);
    }
  }

  currentAxisRect_ = rect;
  currentAxisRect_->setSelected(true);
  if (getAxisRectIndex(rect) < buttionlist_.size())
    button = buttionlist_.at(getAxisRectIndex(rect));
  if (button) button->setActive(true);
  plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpImmediateRefresh);
}

void Layout2D::activateLayout(LayoutButton2D *button) {
  for (int i = 0; i < buttionlist_.size(); i++) {
    if (buttionlist_.at(i) == button) {
      axisRectSetFocus(static_cast<AxisRect2D *>(layout_->elementAt(i)));
      break;
    }
  }
}

void Layout2D::showtooltip(QPointF position, double xval, double yval,
                           Axis2D *xaxis, Axis2D *yaxis) {
  QToolTip::showText(mapToGlobal(QPoint(static_cast<int>(position.x()),
                                        static_cast<int>(position.y()))),
                     QString::number(xval) + ", " + QString::number(yval));
  xpickerline_->setPen(QPen(Qt::red, 1));
  ypickerline_->setPen(QPen(Qt::red, 1));
  xpickerline_->setAntialiased(false);
  ypickerline_->setAntialiased(false);
  xpickerline_->setVisible(true);
  ypickerline_->setVisible(true);
  foreach (QCPItemPosition *position, xpickerline_->positions()) {
    position->setAxes(xaxis, yaxis);
  }
  xpickerline_->setClipAxisRect(xaxis->axisRect());
  xpickerline_->setClipToAxisRect(true);
  xpickerline_->position("point1")->setCoords(xval, yaxis->range().lower);
  xpickerline_->position("point2")->setCoords(xval, yaxis->range().upper);
  foreach (QCPItemPosition *position, ypickerline_->positions()) {
    position->setAxes(xaxis, yaxis);
  }
  ypickerline_->setClipAxisRect(xaxis->axisRect());
  ypickerline_->setClipToAxisRect(true);
  ypickerline_->position("point1")->setCoords(xaxis->range().lower, yval);
  ypickerline_->position("point2")->setCoords(xaxis->range().upper, yval);
}

void Layout2D::addTextToAxisTicker(Column *col, Axis2D *axis, int from,
                                   int to) {
  if (col->dataType() == AlphaPlot::ColumnDataType::TypeString) {
    axis->settickertext(col, from, to);
  } else if (col->dataType() == AlphaPlot::ColumnDataType::TypeDateTime) {
    QSharedPointer<QCPAxisTicker> ticker = axis->getticker_axis();
    ticker.staticCast<QCPAxisTickerDateTime>()->setDateTimeFormat(
        Utilities::splitstring(
            static_cast<DateTime2StringFilter *>(col->outputFilter())
                ->format()));
  }
}

void Layout2D::resizeEvent(QResizeEvent *event) {
  emit layout2DResized();
  MyWidget::resizeEvent(event);
}

void Layout2D::mouseMoveSignal(QMouseEvent *event) { Q_UNUSED(event) }

void Layout2D::mousePressSignal(QMouseEvent *event) { Q_UNUSED(event) }

void Layout2D::mouseReleaseSignal(QMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    QPointF startPos = event->localPos();
    QMenu *menu = new QMenu();
    menu->setAttribute(Qt::WA_DeleteOnClose);
    menu->addAction(IconLoader::load("edit-recalculate", IconLoader::LightDark),
                    "Refresh", this, &Layout2D::refresh);
    menu->addAction("Export", this, &Layout2D::exportGraph);
    menu->addAction(IconLoader::load("edit-print", IconLoader::LightDark),
                    "Print", this, &Layout2D::print);
    menu->popup(plot2dCanvas_->mapToGlobal(QPoint(
        static_cast<int>(startPos.x()), static_cast<int>(startPos.y()))));
  }
}

void Layout2D::mouseWheel() {}

void Layout2D::beforeReplot() {
  if (currentAxisRect_) {
    currentAxisRect_->updateLegendRect();
  }
}

void Layout2D::refresh() {
  plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpImmediateRefresh);
}

bool Layout2D::exportGraph() {
  std::unique_ptr<ImageExportDialog> ied(new ImageExportDialog(nullptr));
  ied->enableraster_antialias(false);
  ied->setraster_antialias(16);
  ied->setraster_height(plot2dCanvas_->height());
  ied->setraster_width(plot2dCanvas_->width());
  ied->setvector_height(plot2dCanvas_->height());
  ied->setraster_width(plot2dCanvas_->width());
  if (ied->exec() != QDialog::Accepted) return false;
  if (ied->selectedFiles().isEmpty()) return false;
  QString selected_filter = ied->selectedNameFilter();

  QString file_name = ied->selectedFiles()[0];
  QFileInfo file_info(file_name);
  if (!file_info.fileName().contains("."))
    file_name.append(selected_filter.remove("*"));

  QFile file(file_name);
  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(
        this, tr("Export Error"),
        tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that "
           "you have the right to write to this location!")
            .arg(file_name));
    return false;
  }

  int raster_resolution = ied->raster_resolution();
  int raster_width = ied->raster_width();
  int raster_height = ied->raster_height();
  double raster_scale = ied->raster_scale();
  int raster_quality = ied->raster_quality();

  int vector_width = ied->vector_width();
  int vector_height = ied->vector_height();

  bool success = false;
  currentAxisRect_->setPrintorExportJob(true);
  if (selected_filter.contains(".pdf")) {
    success = plot2dCanvas_->savePdf(file_name, vector_width, vector_height);
  } else if (selected_filter.contains(".svg")) {
    success = plot2dCanvas_->saveSvg(file_name, vector_width, vector_height);
  } else if (selected_filter.contains(".ps")) {
    success = plot2dCanvas_->savePs(file_name, vector_width, vector_height);
  } else {
    QByteArray ba = selected_filter.toLatin1();
    ba = ba.trimmed();
    ba.remove(0, 1);
    const char *c_char = ba.data();
    success = plot2dCanvas_->saveRastered(file_name, raster_width,
                                          raster_height, raster_scale, c_char,
                                          raster_quality, raster_resolution);
  }
  currentAxisRect_->setPrintorExportJob(false);
  if (!success) {
    QMessageBox::critical(
        this, tr("Export Error"),
        tr("Unknown error exporting: <br><h4> %1 </h4><p>May be "
           "the advanced image export parameters are not rightfully set!")
            .arg(file_name));
  }
  return success;
}

bool Layout2D::exportGraphwithoutdialog(const QString &name,
                                        const QString &selected_filter) {
  int raster_width = plot2dCanvas_->width();
  int raster_height = plot2dCanvas_->height();
  double raster_scale = 1;
  int raster_quality = 100;

  int vector_width = plot2dCanvas_->width();
  int vector_height = plot2dCanvas_->height();

  bool success = false;
  currentAxisRect_->setPrintorExportJob(true);
  if (selected_filter.contains(".pdf")) {
    success = plot2dCanvas_->savePdf(name, vector_width, vector_height);
  } else if (selected_filter.contains(".svg")) {
    success = plot2dCanvas_->saveSvg(name, vector_width, vector_height);
  } else if (selected_filter.contains(".ps")) {
    success = plot2dCanvas_->savePs(name, vector_width, vector_height);
  } else {
    QByteArray ba = selected_filter.toLatin1();
    ba = ba.trimmed();
    ba.remove(0, 1);
    const char *c_char = ba.data();
    success = plot2dCanvas_->saveRastered(name, raster_width, raster_height,
                                          raster_scale, c_char, raster_quality);
  }
  currentAxisRect_->setPrintorExportJob(false);
  if (!success) {
    QMessageBox::critical(
        this, tr("Export Error"),
        tr("Unknown error exporting: <br><h4> %1 </h4><p>May be "
           "the advanced image export parameters are not rightfully set!")
            .arg(name));
  }
  return success;
}

void Layout2D::updateData(Matrix *matrix) {
  foreach (AxisRect2D *axisrect, getAxisRectList()) {
    QVector<ColorMap2D *> colormapvec = axisrect->getColorMapVec();
    foreach (ColorMap2D *colormap, colormapvec) {
      if (colormap->getmatrix_colormap() == matrix) {
        colormap->setColorMapData(matrix);
        colormap->layer()->replot();
        colormap->getxaxis()->getaxisrect_axis()->replotBareBones();
      }
    }
  }
}

void Layout2D::updateData(Table *table, const QString &name) {
  if (!currentAxisRect_) return;
  if (!table) return;
  Column *col = table->column(table->colIndex(name));
  if (!col) return;
  bool modified = false;
  foreach (AxisRect2D *axisrect, getAxisRectList()) {
    QVector<LineSpecial2D *> lslist = axisrect->getLsVec();
    QVector<QPair<LineSpecial2D *, LineSpecial2D *>> channellist =
        axisrect->getChannelVec();
    QVector<Curve2D *> curvelist = axisrect->getCurveVec();
    QVector<Bar2D *> barlist = axisrect->getBarVec();
    QVector<StatBox2D *> statboxlist = axisrect->getStatBoxVec();
    QVector<Vector2D *> vectorlist = axisrect->getVectorVec();
    QVector<Pie2D *> pieveclist = axisrect->getPieVec();
    foreach (LineSpecial2D *ls, lslist) {
      PlotData::AssociatedData *data =
          ls->getdatablock_lsplot()->getassociateddata();
      if (ls->getxerrorbar_lsplot()) {
        DataBlockError *xerror =
            ls->getxerrorbar_lsplot()->getdatablock_error();
        if (xerror->gettable() == table) {
          if (xerror->geterrorcolumn() == col) {
            ls->getxerrorbar_lsplot()->setErrorData(
                xerror->gettable(), xerror->geterrorcolumn(), xerror->getfrom(),
                xerror->getto());
            modified = true;
          }
        }
      }
      if (ls->getyerrorbar_lsplot()) {
        DataBlockError *yerror =
            ls->getyerrorbar_lsplot()->getdatablock_error();
        if (yerror->gettable() == table) {
          if (yerror->geterrorcolumn() == col) {
            ls->getyerrorbar_lsplot()->setErrorData(
                yerror->gettable(), yerror->geterrorcolumn(), yerror->getfrom(),
                yerror->getto());
            modified = true;
          }
        }
      }
      if (data->table == table) {
        if (data->xcol == col || data->ycol == col) {
          ls->setGraphData(data->table, data->xcol, data->ycol, data->from,
                           data->to);
          modified = true;
        }
      }
    }
    for (int i = 0; i < channellist.count(); i++) {
      QPair<LineSpecial2D *, LineSpecial2D *> channel = channellist.at(i);
      PlotData::AssociatedData *data1 =
          channel.first->getdatablock_lsplot()->getassociateddata();
      PlotData::AssociatedData *data2 =
          channel.second->getdatablock_lsplot()->getassociateddata();
      if (data1->table == table) {
        if (data1->xcol == col || data1->ycol == col) {
          channel.first->setGraphData(data1->table, data1->xcol, data1->ycol,
                                      data1->from, data1->to);
          modified = true;
        }
      }
      if (data2->table == table) {
        if (data2->xcol == col || data2->ycol == col) {
          channel.second->setGraphData(data2->table, data2->xcol, data2->ycol,
                                       data2->from, data2->to);
          modified = true;
        }
      }
    }
    foreach (Curve2D *curve, curvelist) {
      if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Associated) {
        PlotData::AssociatedData *data =
            curve->getdatablock_cplot()->getassociateddata();
        if (curve->getxerrorbar_curveplot()) {
          DataBlockError *xerror =
              curve->getxerrorbar_curveplot()->getdatablock_error();
          if (xerror->gettable() == table) {
            if (xerror->geterrorcolumn() == col) {
              curve->getxerrorbar_curveplot()->setErrorData(
                  xerror->gettable(), xerror->geterrorcolumn(),
                  xerror->getfrom(), xerror->getto());
              modified = true;
            }
          }
        }
        if (curve->getyerrorbar_curveplot()) {
          DataBlockError *yerror =
              curve->getyerrorbar_curveplot()->getdatablock_error();
          if (yerror->gettable() == table) {
            if (yerror->geterrorcolumn() == col) {
              curve->getyerrorbar_curveplot()->setErrorData(
                  yerror->gettable(), yerror->geterrorcolumn(),
                  yerror->getfrom(), yerror->getto());
              modified = true;
            }
          }
        }
        if (data->table == table) {
          if (data->xcol == col || data->ycol == col) {
            curve->setCurveData(data->table, data->xcol, data->ycol, data->from,
                                data->to);
            modified = true;
          }
        }
      }
    }
    foreach (StatBox2D *statbox, statboxlist) {
      if (statbox->gettable_statbox() == table) {
        if (statbox->getcolumn_statbox() == col) {
          int key = int(statbox->getboxwhiskerdata_statbox().key);
          StatBox2D::BoxWhiskerData data = generateBoxWhiskerData(
              statbox->getboxwhiskerdata_statbox().table_,
              statbox->getboxwhiskerdata_statbox().column_,
              statbox->getboxwhiskerdata_statbox().from_,
              statbox->getboxwhiskerdata_statbox().to_, key);
          statbox->setboxwhiskerdata(data);
          modified = true;
        }
      }
    }
    foreach (Bar2D *bar, barlist) {
      if (!bar->ishistogram_barplot()) {
        PlotData::AssociatedData *data =
            bar->getdatablock_barplot()->getassociateddata();
        if (bar->getxerrorbar_barplot()) {
          DataBlockError *xerror =
              bar->getxerrorbar_barplot()->getdatablock_error();
          if (xerror->gettable() == table) {
            if (xerror->geterrorcolumn() == col) {
              bar->getxerrorbar_barplot()->setErrorData(
                  xerror->gettable(), xerror->geterrorcolumn(),
                  xerror->getfrom(), xerror->getto());
              modified = true;
            }
          }
        }
        if (bar->getyerrorbar_barplot()) {
          DataBlockError *yerror =
              bar->getyerrorbar_barplot()->getdatablock_error();
          if (yerror->gettable() == table) {
            if (yerror->geterrorcolumn() == col) {
              bar->getyerrorbar_barplot()->setErrorData(
                  yerror->gettable(), yerror->geterrorcolumn(),
                  yerror->getfrom(), yerror->getto());
              modified = true;
            }
          }
        }
        if (data->table == table) {
          if (data->xcol == col || data->ycol == col) {
            bar->setBarData(data->table, data->xcol, data->ycol, data->from,
                            data->to);
            modified = true;
          }
        }
      } else {
        if (bar->gettable_histogram() == table) {
          if (bar->getcolumn_histogram() == col) {
            bar->setBarData(bar->gettable_histogram(),
                            bar->getcolumn_histogram(),
                            bar->getfrom_histogram(), bar->getto_histogram());
            modified = true;
          }
        }
      }
    }
    foreach (Vector2D *vector, vectorlist) {
      if (vector->gettable_vecplot() == table) {
        if (vector->getfirstcol_vecplot() == col ||
            vector->getsecondcol_vecplot() == col ||
            vector->getthirdcol_vecplot() == col ||
            vector->getfourthcol_vecplot() == col) {
          vector->setGraphData(
              vector->gettable_vecplot(), vector->getfirstcol_vecplot(),
              vector->getsecondcol_vecplot(), vector->getthirdcol_vecplot(),
              vector->getfourthcol_vecplot(), vector->getfrom_vecplot(),
              vector->getto_vecplot());
          modified = true;
        }
      }
    }
    foreach (Pie2D *pie, pieveclist) {
      if (pie->gettable_pieplot() == table) {
        if (pie->getxcolumn_pieplot() == col) {
          pie->setGraphData(pie->gettable_pieplot(), pie->getxcolumn_pieplot(),
                            pie->getycolumn_pieplot(), pie->getfrom_pieplot(),
                            pie->getto_pieplot());
          modified = true;
        }
      }
    }
  }
  if (modified)
    plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void Layout2D::removeMatrix(Matrix *matrix) {
  foreach (AxisRect2D *axisrect, getAxisRectList()) {
    QVector<ColorMap2D *> colormapvec = axisrect->getColorMapVec();
    foreach (ColorMap2D *colormap, colormapvec) {
      if (colormap->getmatrix_colormap() == matrix) {
        axisrect->removeColorMap2D(colormap);
        plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      }
    }
  }
}

void Layout2D::removeColumn(Table *table, const QString &name) {
  if (!currentAxisRect_) return;
  if (!table) return;
  Column *col = table->column(table->colIndex(name));
  if (!col) return;
  bool removed = false;
  foreach (AxisRect2D *axisrect, getAxisRectList()) {
    QVector<LineSpecial2D *> lslist = axisrect->getLsVec();
    QVector<QPair<LineSpecial2D *, LineSpecial2D *>> channellist =
        axisrect->getChannelVec();
    QVector<Curve2D *> curvelist = axisrect->getCurveVec();
    QVector<Bar2D *> barlist = axisrect->getBarVec();
    QVector<StatBox2D *> statboxlist = axisrect->getStatBoxVec();
    QVector<Vector2D *> vectorlist = axisrect->getVectorVec();
    QVector<Pie2D *> pieveclist = axisrect->getPieVec();
    foreach (LineSpecial2D *ls, lslist) {
      PlotData::AssociatedData *data =
          ls->getdatablock_lsplot()->getassociateddata();
      if (ls->getxerrorbar_lsplot()) {
        DataBlockError *xerror =
            ls->getxerrorbar_lsplot()->getdatablock_error();
        if (xerror->gettable() == table) {
          if (xerror->geterrorcolumn() == col) {
            ls->removeXerrorBar();
            removed = true;
          }
        }
      }
      if (ls->getyerrorbar_lsplot()) {
        DataBlockError *yerror =
            ls->getyerrorbar_lsplot()->getdatablock_error();
        if (yerror->gettable() == table) {
          if (yerror->geterrorcolumn() == col) {
            ls->removeYerrorBar();
            removed = true;
          }
        }
      }
      if (data->table == table) {
        if (data->xcol == col || data->ycol == col) {
          axisrect->removeLineSpecial2D(ls);
          removed = true;
        }
      }
    }
    for (int i = 0; i < channellist.count(); i++) {
      QPair<LineSpecial2D *, LineSpecial2D *> channel = channellist.at(i);
      PlotData::AssociatedData *data1 =
          channel.first->getdatablock_lsplot()->getassociateddata();
      PlotData::AssociatedData *data2 =
          channel.second->getdatablock_lsplot()->getassociateddata();
      if (data1->table == table || data2->table == table) {
        if (data1->xcol == col || data1->ycol == col || data2->xcol == col ||
            data2->ycol == col) {
          channel.first->setGraphData(data1->table, data1->xcol, data1->ycol,
                                      data1->from, data1->to);
          axisrect->removeChannel2D(channel);
          removed = true;
        }
      }
    }
    foreach (Curve2D *curve, curvelist) {
      if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Associated) {
        PlotData::AssociatedData *data =
            curve->getdatablock_cplot()->getassociateddata();
        if (curve->getxerrorbar_curveplot()) {
          DataBlockError *xerror =
              curve->getxerrorbar_curveplot()->getdatablock_error();
          if (xerror->gettable() == table) {
            if (xerror->geterrorcolumn() == col) {
              curve->removeXerrorBar();
              removed = true;
            }
          }
        }
        if (curve->getyerrorbar_curveplot()) {
          DataBlockError *yerror =
              curve->getyerrorbar_curveplot()->getdatablock_error();
          if (yerror->gettable() == table) {
            if (yerror->geterrorcolumn() == col) {
              curve->removeYerrorBar();
              removed = true;
            }
          }
        }
        if (data->table == table) {
          if (data->xcol == col || data->ycol == col) {
            axisrect->removeCurve2D(curve);
            removed = true;
          }
        }
      }
    }
    foreach (StatBox2D *statbox, statboxlist) {
      if (statbox->gettable_statbox() == table) {
        if (statbox->getcolumn_statbox() == col) {
          axisrect->removeStatBox2D(statbox);
          removed = true;
        }
      }
    }
    foreach (Bar2D *bar, barlist) {
      if (!bar->ishistogram_barplot()) {
        PlotData::AssociatedData *data =
            bar->getdatablock_barplot()->getassociateddata();
        if (bar->getxerrorbar_barplot()) {
          DataBlockError *xerror =
              bar->getxerrorbar_barplot()->getdatablock_error();
          if (xerror->gettable() == table) {
            if (xerror->geterrorcolumn() == col) {
              bar->removeXerrorBar();
              removed = true;
            }
          }
        }
        if (bar->getyerrorbar_barplot()) {
          DataBlockError *yerror =
              bar->getyerrorbar_barplot()->getdatablock_error();
          if (yerror->gettable() == table) {
            if (yerror->geterrorcolumn() == col) {
              bar->removeYerrorBar();
              removed = true;
            }
          }
        }
        if (data->table == table) {
          if (data->xcol == col || data->ycol == col) {
            axisrect->removeBar2D(bar);
            removed = true;
          }
        }
      } else {
        if (bar->gettable_histogram() == table) {
          if (bar->getcolumn_histogram() == col) {
            axisrect->removeBar2D(bar);
            removed = true;
          }
        }
      }
    }
    foreach (Vector2D *vector, vectorlist) {
      if (vector->gettable_vecplot() == table) {
        if (vector->getfirstcol_vecplot() == col ||
            vector->getsecondcol_vecplot() == col ||
            vector->getthirdcol_vecplot() == col ||
            vector->getfourthcol_vecplot() == col) {
          axisrect->removeVector2D(vector);
          removed = true;
        }
      }
    }
    foreach (Pie2D *pie, pieveclist) {
      if (pie->gettable_pieplot() == table) {
        if (pie->getxcolumn_pieplot() == col) {
          axisrect->removePie2D(pie);
          removed = true;
        }
      }
    }
  }
  if (removed)
    plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

QStringList Layout2D::dependentTableMatrixNames() {
  QStringList dependeon;
  foreach (AxisRect2D *axisrect, getAxisRectList()) {
    QVector<LineSpecial2D *> lslist = axisrect->getLsVec();
    QVector<QPair<LineSpecial2D *, LineSpecial2D *>> channellist =
        axisrect->getChannelVec();
    QVector<Curve2D *> curvelist = axisrect->getCurveVec();
    QVector<Bar2D *> barlist = axisrect->getBarVec();
    QVector<StatBox2D *> statboxlist = axisrect->getStatBoxVec();
    QVector<Vector2D *> vectorlist = axisrect->getVectorVec();
    QVector<Pie2D *> pieveclist = axisrect->getPieVec();
    QVector<ColorMap2D *> colormapvec = axisrect->getColorMapVec();
    foreach (LineSpecial2D *ls, lslist) {
      PlotData::AssociatedData *data =
          ls->getdatablock_lsplot()->getassociateddata();
      if (!dependeon.contains(data->table->name()))
        dependeon << data->table->name();
      if (ls->getxerrorbar_lsplot()) {
        DataBlockError *xerror =
            ls->getxerrorbar_lsplot()->getdatablock_error();
        if (!dependeon.contains(xerror->gettable()->name()))
          dependeon << xerror->gettable()->name();
      }
      if (ls->getyerrorbar_lsplot()) {
        DataBlockError *yerror =
            ls->getyerrorbar_lsplot()->getdatablock_error();
        if (!dependeon.contains(yerror->gettable()->name()))
          dependeon << yerror->gettable()->name();
      }
    }
    for (int i = 0; i < channellist.count(); i++) {
      QPair<LineSpecial2D *, LineSpecial2D *> channel = channellist.at(i);
      PlotData::AssociatedData *data1 =
          channel.first->getdatablock_lsplot()->getassociateddata();
      PlotData::AssociatedData *data2 =
          channel.second->getdatablock_lsplot()->getassociateddata();
      if (!dependeon.contains(data1->table->name()))
        dependeon << data1->table->name();
      if (!dependeon.contains(data2->table->name()))
        dependeon << data2->table->name();
    }
    foreach (Curve2D *curve, curvelist) {
      if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Associated) {
        PlotData::AssociatedData *data =
            curve->getdatablock_cplot()->getassociateddata();
        if (!dependeon.contains(data->table->name()))
          dependeon << data->table->name();
        if (curve->getxerrorbar_curveplot()) {
          DataBlockError *xerror =
              curve->getxerrorbar_curveplot()->getdatablock_error();
          if (!dependeon.contains(xerror->gettable()->name()))
            dependeon << xerror->gettable()->name();
        }
        if (curve->getyerrorbar_curveplot()) {
          DataBlockError *yerror =
              curve->getyerrorbar_curveplot()->getdatablock_error();
          if (!dependeon.contains(yerror->gettable()->name()))
            dependeon << yerror->gettable()->name();
        }
      }
    }
    foreach (StatBox2D *statbox, statboxlist) {
      if (!dependeon.contains(
              statbox->getboxwhiskerdata_statbox().table_->name()))
        dependeon << statbox->getboxwhiskerdata_statbox().table_->name();
    }

    foreach (Bar2D *bar, barlist) {
      if (!bar->ishistogram_barplot()) {
        PlotData::AssociatedData *data =
            bar->getdatablock_barplot()->getassociateddata();
        if (!dependeon.contains(data->table->name()))
          dependeon << data->table->name();
        if (bar->getxerrorbar_barplot()) {
          DataBlockError *xerror =
              bar->getxerrorbar_barplot()->getdatablock_error();
          if (!dependeon.contains(xerror->gettable()->name()))
            dependeon << xerror->gettable()->name();
        }
        if (bar->getyerrorbar_barplot()) {
          DataBlockError *yerror =
              bar->getyerrorbar_barplot()->getdatablock_error();
          if (!dependeon.contains(yerror->gettable()->name()))
            dependeon << yerror->gettable()->name();
        }
      } else {
        if (!dependeon.contains(bar->gettable_histogram()->name()))
          dependeon << bar->gettable_histogram()->name();
      }
    }

    foreach (Vector2D *vector, vectorlist) {
      if (!dependeon.contains(vector->gettable_vecplot()->name()))
        dependeon << vector->gettable_vecplot()->name();
    }
    foreach (Pie2D *pie, pieveclist) {
      if (!dependeon.contains(pie->gettable_pieplot()->name()))
        dependeon << pie->gettable_pieplot()->name();
    }
    foreach (ColorMap2D *colormap, colormapvec) {
      if (!dependeon.contains(colormap->getmatrix_colormap()->name()))
        dependeon << colormap->getmatrix_colormap()->name();
    }
  }
  return dependeon;
}

void Layout2D::setAxisRangeZoom(bool value) {
  foreach (AxisRect2D *axisrect, getAxisRectList()) {
    (value) ? axisrect->setRangeZoomAxes(axisrect->axes())
            : axisrect->setRangeZoomAxes(QList<QCPAxis *>());
  }
}

void Layout2D::setAxisRangeDrag(bool value) {
  foreach (AxisRect2D *axisrect, getAxisRectList()) {
    (value) ? axisrect->setRangeDragAxes(axisrect->axes())
            : axisrect->setRangeDragAxes(QList<QCPAxis *>());
  }
}

void Layout2D::exportPDF(const QString &filename) {
  currentAxisRect_->setPrintorExportJob(true);
  plot2dCanvas_->savePdf(filename);
  currentAxisRect_->setPrintorExportJob(false);
}

void Layout2D::setLayoutDimension(QPair<int, int> dimension) {
  layoutDimension_.first = dimension.first;
  layoutDimension_.second = dimension.second;
}

void Layout2D::removeAxisRect(int index) {
  // if no elements to remove
  if (index < 0 || index >= getAxisRectList().count()) {
    qDebug() << "unable to remove axisrect2D at index :" << index;
    return;
  }

  // if removed element is the currently selected element
  AxisRect2D *axrect = dynamic_cast<AxisRect2D *>(layout_->elementAt(index));
  if (!axrect) {
    qDebug() << "unable to remove axisrect2D at index :" << index;
    return;
  }
  axrect = nullptr;
  if (index == 0 && index + 1 < getAxisRectList().count())
    axrect = dynamic_cast<AxisRect2D *>(layout_->elementAt(index + 1));
  else if (index > 0)
    axrect = dynamic_cast<AxisRect2D *>(layout_->elementAt(index - 1));
  else {
    qDebug() << "unable to remove axisrect2D at index :" << index;
    axrect = nullptr;
  }

  if (axrect) {
    axisRectSetFocus(axrect);
  } else {
    currentAxisRect_ = nullptr;
  }

  // remove layout button
  buttionlist_.takeLast()->close();

  // remove the element & adjust layout accordingly
  layout_->remove(layout_->elementAt(index));
  layout_->simplify();
  if (axrect) axisRectSetFocus(axrect);
  plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

int Layout2D::getLayoutRectGridIndex(QPair<int, int> coord) {
  int index = ((colorCount()) * coord.second) + coord.first;
  return index;
}

QPair<int, int> Layout2D::getLayoutRectGridCoordinate(int index) {
  QPair<int, int> pair;
  pair.first = index / (layout_->columnCount());
  pair.second = index % (layout_->columnCount());
  return pair;
}

LayoutButton2D *Layout2D::addLayoutButton(int num) {
  LayoutButton2D *button = new LayoutButton2D(QString::number(++num));

  connect(button, &LayoutButton2D::clicked, this, &Layout2D::activateLayout);
  /*connect(button, SIGNAL(showContextMenu()), this,
          SIGNAL(showLayoutButtonContextMenu()));
  connect(button, SIGNAL(showCurvesDialog()), this,
  SIGNAL(showCurvesDialog()));*/

  buttionlist_ << button;
  layoutButtonsBox_->addWidget(button);
  return button;
}

void Layout2D::setBackground(const QColor &background) {
  QString baseColor = QString("rgba(%0,%1,%2,%3)")
                          .arg(background.red())
                          .arg(background.green())
                          .arg(background.blue())
                          .arg(background.alpha());
  streachLabel_->setStyleSheet(".QLabel { background-color:" + baseColor +
                               ";}");
  main_widget_->setStyleSheet(".QWidget { background-color:" + baseColor +
                              ";}");
}

void Layout2D::setGraphTool(const Graph2DCommon::Picker &picker) {
  picker_ = picker;
  switch (picker_) {
    case Graph2DCommon::Picker::None: {
      if (xpickerline_) {
        plot2dCanvas_->removeItem(xpickerline_);
        xpickerline_ = nullptr;
      }
      if (ypickerline_) {
        plot2dCanvas_->removeItem(ypickerline_);
        ypickerline_ = nullptr;
      }
      plot2dCanvas_->unsetCursor();
      plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      setAxisRangeDrag(false);
      setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::DataPoint: {
      if (!xpickerline_) xpickerline_ = new QCPItemStraightLine(plot2dCanvas_);
      if (!ypickerline_) ypickerline_ = new QCPItemStraightLine(plot2dCanvas_);
      xpickerline_->setVisible(false);
      ypickerline_->setVisible(false);
      plot2dCanvas_->setCursor(Qt::CursorShape::CrossCursor);
      plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      setAxisRangeDrag(false);
      setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::DataGraph: {
      if (!xpickerline_) xpickerline_ = new QCPItemStraightLine(plot2dCanvas_);
      if (!ypickerline_) ypickerline_ = new QCPItemStraightLine(plot2dCanvas_);
      xpickerline_->setVisible(false);
      ypickerline_->setVisible(false);
      plot2dCanvas_->setCursor(Qt::CursorShape::CrossCursor);
      plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      setAxisRangeDrag(false);
      setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::DataMove: {
      if (xpickerline_) {
        plot2dCanvas_->removeItem(xpickerline_);
        xpickerline_ = nullptr;
      }
      if (ypickerline_) {
        plot2dCanvas_->removeItem(ypickerline_);
        ypickerline_ = nullptr;
      }
      plot2dCanvas_->setCursor(Qt::CursorShape::OpenHandCursor);
      plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      setAxisRangeDrag(false);
      setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::DataRemove: {
      if (xpickerline_) {
        plot2dCanvas_->removeItem(xpickerline_);
        xpickerline_ = nullptr;
      }
      if (ypickerline_) {
        plot2dCanvas_->removeItem(ypickerline_);
        ypickerline_ = nullptr;
      }
      plot2dCanvas_->setCursor(Qt::CursorShape::PointingHandCursor);
      plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      setAxisRangeDrag(false);
      setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::DragRange: {
      if (xpickerline_) {
        plot2dCanvas_->removeItem(xpickerline_);
        xpickerline_ = nullptr;
      }
      if (ypickerline_) {
        plot2dCanvas_->removeItem(ypickerline_);
        ypickerline_ = nullptr;
      }
      plot2dCanvas_->setCursor(Qt::CursorShape::SizeAllCursor);
      plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      setAxisRangeDrag(true);
      setAxisRangeZoom(false);
    } break;
    case Graph2DCommon::Picker::ZoomRange: {
      if (xpickerline_) {
        plot2dCanvas_->removeItem(xpickerline_);
        xpickerline_ = nullptr;
      }
      if (ypickerline_) {
        plot2dCanvas_->removeItem(ypickerline_);
        ypickerline_ = nullptr;
      }
      plot2dCanvas_->setCursor(Qt::CursorShape::UpArrowCursor);
      plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
      setAxisRangeDrag(false);
      setAxisRangeZoom(true);
    } break;
  }
  QList<AxisRect2D *> axisrectlist = getAxisRectList();
  foreach (AxisRect2D *axisrect, axisrectlist) {
    axisrect->setGraphTool(picker_);
  }
}

void Layout2D::print() {
  std::unique_ptr<QPrinter> printer = std::unique_ptr<QPrinter>(new QPrinter);
  std::unique_ptr<QPrintPreviewDialog> previewDialog =
      std::unique_ptr<QPrintPreviewDialog>(
          new QPrintPreviewDialog(printer.get(), this));
  connect(previewDialog.get(), &QPrintPreviewDialog::paintRequested,
          [=](QPrinter *printer) {
            printer->setPageSize(QPrinter::A4);
            printer->setColorMode(QPrinter::Color);
            std::unique_ptr<QCPPainter> painter =
                std::unique_ptr<QCPPainter>(new QCPPainter(printer));
            QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

            int plotWidth = plot2dCanvas_->viewport().width();
            int plotHeight = plot2dCanvas_->viewport().height();
            double scale = pageRect.width() / static_cast<double>(plotWidth);

            painter->setMode(QCPPainter::pmVectorized);
            painter->setMode(QCPPainter::pmNoCaching);
            // comment this out if you want cosmetic thin lines (always 1 pixel
            // thick independent of pdf zoom level)
            // painter.setMode(QCPPainter::pmNonCosmetic);
            painter->scale(scale, scale);
            currentAxisRect_->setPrintorExportJob(true);
            plot2dCanvas_->toPainter(painter.get(), plotWidth, plotHeight);
            currentAxisRect_->setPrintorExportJob(false);
          });
  previewDialog->exec();
}

void Layout2D::save(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("plot2d");
  xmlwriter->writeAttribute("x", QString::number(pos().x()));
  xmlwriter->writeAttribute("y", QString::number(pos().y()));
  xmlwriter->writeAttribute("width", QString::number(width()));
  xmlwriter->writeAttribute("height", QString::number(height()));
  xmlwriter->writeAttribute("creation_time", birthDate());
  xmlwriter->writeAttribute("caption_spec", QString::number(captionPolicy()));
  xmlwriter->writeAttribute("name", name());
  xmlwriter->writeAttribute("label", windowLabel());
  xmlwriter->writeStartElement("canvas");
  xmlwriter->writeAttribute(
      "devicepixelratio",
      QString::number(plot2dCanvas_->bufferDevicePixelRatio()));
  (plot2dCanvas_->openGl()) ? xmlwriter->writeAttribute("opengl", "true")
                            : xmlwriter->writeAttribute("opengl", "false");
  xmlwriter->writeAttribute(
      "backgroundcolor",
      plot2dCanvas_->getBackgroundColor().name(QColor::HexArgb));
  xmlwriter->writeEndElement();
  foreach (AxisRect2D *axisrect, getAxisRectList()) {
    axisrect->save(xmlwriter, getAxisRectIndex(axisrect));
  }
  xmlwriter->writeEndElement();
}

bool Layout2D::load(XmlStreamReader *xmlreader, QList<Table *> tabs,
                    QList<Matrix *> mats) {
  if (xmlreader->isStartElement() && xmlreader->name() == "plot2d") {
    bool ok = false;

    // read caption spec
    int x = xmlreader->readAttributeInt("x", &ok);
    if (ok) {
      int y = xmlreader->readAttributeInt("y", &ok);
      if (ok) {
        int width = xmlreader->readAttributeInt("width", &ok);
        if (ok) {
          int height = xmlreader->readAttributeInt("height", &ok);
          if (ok) {
            setGeometry(x, y, width, height);
          }
        }
      }
    } else
      xmlreader->raiseWarning(tr("Plot2D geometry setting error."));

    // read creation time
    QString time = xmlreader->readAttributeString("creation_time", &ok);
    QDateTime creation_time =
        QDateTime::fromString(time, "yyyy-dd-MM hh:mm:ss:zzz");
    if (!time.isEmpty() && creation_time.isValid() && ok) {
      setBirthDate(creation_time.toString(Qt::LocalDate));
    } else {
      xmlreader->raiseWarning(
          tr("Invalid creation time. Using current time insted."));
      setBirthDate(QDateTime::currentDateTime().toString(Qt::LocalDate));
    }
    // read caption spec
    int captionspec = xmlreader->readAttributeInt("caption_spec", &ok);
    if (ok)
      setCaptionPolicy(static_cast<MyWidget::CaptionPolicy>(captionspec));
    else
      xmlreader->raiseWarning(tr("Invalid caption policy or read error."));
    // read name
    QString name = xmlreader->readAttributeString("name", &ok);
    if (ok) {
      setName(name);
    } else
      xmlreader->raiseWarning(tr("Layout2D name missing or empty"));

    // read label
    QString label = xmlreader->readAttributeString("label", &ok);
    if (ok) {
      setWindowLabel(name);
    } else
      xmlreader->raiseWarning(tr("Layout2D label missing or empty"));

    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "canvas") break;
      if (xmlreader->isStartElement() && xmlreader->name() == "canvas") {
        double dpr = xmlreader->readAttributeDouble("devicepixelratio", &ok);
        if (ok)
          plot2dCanvas_->setBufferDevicePixelRatio(dpr);
        else
          xmlreader->raiseWarning(
              tr("Layout2D devicepixelratio missing or empty"));
        bool opgl = xmlreader->readAttributeBool("opengl", &ok);
        if (ok)
          plot2dCanvas_->setOpenGl(opgl);
        else
          xmlreader->raiseWarning(tr("Layout2D opengl missing or empty"));
        //
        QString bkcol = xmlreader->readAttributeString("backgroundcolor", &ok);
        if (ok)
          plot2dCanvas_->setBackgroundColor(bkcol);
        else
          xmlreader->raiseWarning(
              tr("Layout2D background color missing or empty"));
      }
    }
    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "plot2d") break;
      if (xmlreader->isStartElement() && xmlreader->name() == "layout") {
        AxisRect2D *axisrect = addAxisRectWithAxis();
        foreach (Axis2D *axis, axisrect->getAxes2D()) {
          axisrect->removeAxis2D(axis, true);
        }
        axisrect->setGridPairToNullptr();
        axisrect->load(xmlreader, tabs, mats);
      } else
        // unknown element
        xmlreader->raiseWarning(
            tr("unknown element '%1'").arg(xmlreader->name().toString()));
    }
  } else  // no plot2d element
    xmlreader->raiseError(tr("no plot2d element found"));

  return !xmlreader->hasError();
}

void Layout2D::loadIcons() {
  addLayoutButton_->setIcon(
      IconLoader::load("list-add", IconLoader::LightDark));
  removeLayoutButton_->setIcon(
      IconLoader::load("list-remove", IconLoader::General));
}

void Layout2D::setLayoutButtonBoxVisible(const bool value) {
  if (value) {
    addLayoutButton_->setEnabled(true);
    removeLayoutButton_->setEnabled(true);
    addLayoutButton_->setVisible(true);
    removeLayoutButton_->setVisible(true);
  } else {
    addLayoutButton_->setDisabled(true);
    removeLayoutButton_->setDisabled(true);
    addLayoutButton_->setHidden(true);
    removeLayoutButton_->setHidden(true);
  }
}

AxisRect2D *Layout2D::addAxisRectWithAxis() {
  return addAxisRectItem(AlphaPlot::ColumnDataType::TypeDouble,
                         AlphaPlot::ColumnDataType::TypeDouble);
}
