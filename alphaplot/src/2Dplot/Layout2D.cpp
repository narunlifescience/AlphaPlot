#include "Layout2D.h"
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
#include "Curve2D.h"
#include "DataManager2D.h"
#include "LayoutGrid2D.h"
#include "Legend2D.h"
#include "LineSpecial2D.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "core/Utilities.h"
#include "widgets/ImageExportDialog2D.h"
#include "widgets/LayoutButton2D.h"

#include <gsl/gsl_sort.h>
#include <gsl/gsl_statistics.h>

Layout2D::Layout2D(const QString &label, QWidget *parent, const QString name,
                   Qt::WindowFlags f)
    : MyWidget(label, parent, name, f),
      main_widget_(new QWidget(this)),
      plot2dCanvas_(new Plot2D(main_widget_)),
      layout_(new LayoutGrid2D()),
      buttionlist_(QList<LayoutButton2D *>()),
      currentAxisRect_(nullptr),
      picker_(Graph2DCommon::Picker::None) {
  main_widget_->setContentsMargins(0, 0, 0, 0);
  if (name.isEmpty()) setObjectName("multilayout2d plot");
  QDateTime birthday = QDateTime::currentDateTime();
  setBirthDate(birthday.toString(Qt::LocalDate));

  layoutManagebuttonsBox_ = new QHBoxLayout();
  addLayoutButton_ = new QPushButton();
  addLayoutButton_->setToolTip(tr("Add layer"));
  addLayoutButton_->setIcon(
      IconLoader::load("list-add", IconLoader::LightDark));
  addLayoutButton_->setMaximumWidth(LayoutButton2D::btnSize());
  addLayoutButton_->setMaximumHeight(LayoutButton2D::btnSize());
  connect(addLayoutButton_, SIGNAL(clicked()), this, SLOT(addAxisRectItem()));
  layoutManagebuttonsBox_->addWidget(addLayoutButton_);

  removeLayoutButton_ = new QPushButton();
  removeLayoutButton_->setToolTip(tr("Remove active layer"));
  removeLayoutButton_->setIcon(
      IconLoader::load("list-remove", IconLoader::General));
  removeLayoutButton_->setMaximumWidth(LayoutButton2D::btnSize());
  removeLayoutButton_->setMaximumHeight(LayoutButton2D::btnSize());
  connect(removeLayoutButton_, SIGNAL(clicked()), this,
          SLOT(removeAxisRectItem()));
  layoutManagebuttonsBox_->addWidget(removeLayoutButton_);

  layoutButtonsBox_ = new QHBoxLayout();
  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addLayout(layoutButtonsBox_);
  streachLabel_ = new QLabel();
  hbox->addWidget(streachLabel_);
  hbox->addLayout(layoutManagebuttonsBox_);
  setBackground(plot2dCanvas_->getBackgroundColor().color());

  QVBoxLayout *layout = new QVBoxLayout(main_widget_);
  layout->addLayout(hbox);
  layout->addWidget(plot2dCanvas_, 1);
  setWidget(main_widget_);
  layout->setMargin(0);
  layout->setSpacing(0);
  setGeometry(QRect(0, 0, 500, 400));
  setMinimumSize(QSize(100, 100));
  setFocusPolicy(Qt::StrongFocus);

  plot2dCanvas_->plotLayout()->addElement(0, 0, layout_);

  // connections
  connect(plot2dCanvas_, SIGNAL(mouseMove(QMouseEvent *)), this,
          SLOT(mouseMoveSignal(QMouseEvent *)));
  connect(plot2dCanvas_, SIGNAL(mousePress(QMouseEvent *)), this,
          SLOT(mousePressSignal(QMouseEvent *)));
  connect(plot2dCanvas_, SIGNAL(mouseRelease(QMouseEvent *)), this,
          SLOT(mouseReleaseSignal(QMouseEvent *)));
  connect(plot2dCanvas_, SIGNAL(beforeReplot()), this, SLOT(beforeReplot()));

  connect(plot2dCanvas_, SIGNAL(mouseWheel(QWheelEvent *)), this,
          SLOT(mouseWheel()));
}

Layout2D::~Layout2D() {
  delete layout_;
  // delete plot2dCanvas_;
}

StatBox2D::BoxWhiskerData Layout2D::generateBoxWhiskerData(Column *colData,
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

  QString name = "f(" + xLabel + ") = " + yLabel;
  Curve2D *curve =
      element->addFunction2DPlot(xdata, ydata, xAxis.at(0), yAxis.at(0), name);
  curve->rescaleAxes();
  curve->layer()->replot();
}

void Layout2D::generateStatBox2DPlot(Table *table, QList<Column *> ycollist,
                                     int from, int to, int key) {
  QList<StatBox2D::BoxWhiskerData> statBoxData;
  foreach (Column *col, ycollist) {
    statBoxData << generateBoxWhiskerData(col, from, to, key);
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
    element->addStatBox2DPlot(table, data, xAxis.at(0), yAxis.at(0));
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

  plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
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
      plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
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
    plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
  }
}

void Layout2D::generateBar2DPlot(const AxisRect2D::BarType &barType,
                                 Table *table, Column *xData,
                                 QList<Column *> ycollist, int from, int to) {
  AxisRect2D *element =
      addAxisRectItem(xData->dataType(), ycollist.at(0)->dataType());
  QList<Axis2D *> xAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Bottom);
  xAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Top);
  QList<Axis2D *> yAxis =
      element->getAxesOrientedTo(Axis2D::AxisOreantation::Left);
  yAxis << element->getAxesOrientedTo(Axis2D::AxisOreantation::Right);
  addTextToAxisTicker(xData, xAxis.at(0));
  addTextToAxisTicker(ycollist.at(0), yAxis.at(0));
  foreach (Column *col, ycollist) {
    Bar2D *bar = element->addBox2DPlot(barType, table, xData, col, from, to,
                                       xAxis.at(0), yAxis.at(0));
    bar->rescaleAxes();
  }
  plot2dCanvas_->replot();
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
  vex->layer()->replot();
}

void Layout2D::generatePie2DPlot(Table *table, Column *xData, int from,
                                 int to) {
  AxisRect2D *element = addAxisRectItem(AlphaPlot::ColumnDataType::TypeDouble,
                                        AlphaPlot::ColumnDataType::TypeDouble);

  Pie2D *pie = element->addPie2DPlot(table, xData, from, to);
  pie->layer()->replot();
}

QList<AxisRect2D *> Layout2D::getAxisRectList() {
  QList<AxisRect2D *> elementslist;
  for (int i = 0; i < layout_->elementCount(); i++) {
    elementslist.append(static_cast<AxisRect2D *>(layout_->elementAt(i)));
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
  addTextToAxisTicker(xData, xAxis.at(0));
  addTextToAxisTicker(ycollist.at(0), yAxis.at(0));
  foreach (Column *col, ycollist) {
    LineSpecial2D *linescatter = element->addLineScatter2DPlot(
        plotType, table, xData, col, from, to, xAxis.at(0), yAxis.at(0));
    linescatter->rescaleAxes();
  }
  plot2dCanvas_->replot();
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
  addTextToAxisTicker(xcol, xAxis.at(0));
  addTextToAxisTicker(ycollist.at(0), yAxis.at(0));
  foreach (Column *col, ycollist) {
    Curve2D *curve = element->addCurve2DPlot(plotType, table, xcol, col, from,
                                             to, xAxis.at(0), yAxis.at(0));
    curve->rescaleAxes();
  }
  plot2dCanvas_->replot();
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

  plot2dCanvas_->replot();
  addLayoutButton(col);

  connect(axisRect2d, SIGNAL(AxisRectClicked(AxisRect2D *)), this,
          SLOT(axisRectSetFocus(AxisRect2D *)));
  connect(axisRect2d, SIGNAL(showtooltip(QPointF, double, double)), this,
          SLOT(showtooltip(QPointF, double, double)));

  emit AxisRectCreated(axisRect2d, this);
  if (!currentAxisRect_) axisRectSetFocus(axisRect2d);
  return axisRect2d;
}

void Layout2D::removeAxisRectItem() {
  removeAxisRect(getAxisRectIndex(currentAxisRect_));
  emit AxisRectRemoved(this);
}

void Layout2D::axisRectSetFocus(AxisRect2D *rect) {
  if (!rect) return;

  LayoutButton2D *button;
  if (currentAxisRect_) {
    if (currentAxisRect_ != rect) {
      currentAxisRect_->setSelected(false);
      button = buttionlist_.at(getAxisRectIndex(currentAxisRect_));
      if (button) button->setActive(false);
    }
  }
  currentAxisRect_ = rect;
  currentAxisRect_->setSelected(true);
  button = buttionlist_.at(getAxisRectIndex(rect));
  if (button) button->setActive(true);
  plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void Layout2D::activateLayout(LayoutButton2D *button) {
  for (int i = 0; i < buttionlist_.size(); i++) {
    if (buttionlist_.at(i) == button) {
      axisRectSetFocus(static_cast<AxisRect2D *>(layout_->elementAt(i)));
      break;
    }
  }
}

void Layout2D::showtooltip(QPointF position, double xval, double yval) {
  QToolTip::showText(mapToGlobal(QPoint(static_cast<int>(position.x()),
                                        static_cast<int>(position.y()))),
                     QString::number(xval) + ", " + QString::number(yval));
}

void Layout2D::addTextToAxisTicker(Column *col, Axis2D *axis) {
  if (col->dataType() == AlphaPlot::ColumnDataType::TypeString) {
    QSharedPointer<QCPAxisTickerText> textticker =
        qSharedPointerCast<QCPAxisTickerText>(axis->getticker_axis());
    for (int i = 0; i < col->rowCount(); i++) {
      textticker->addTick(i, col->textAt(i));
    }
    axis->setticklabelrotation_axis(45);
  }
}

void Layout2D::mouseMoveSignal(QMouseEvent *event) { Q_UNUSED(event); }

void Layout2D::mousePressSignal(QMouseEvent *event) { Q_UNUSED(event); }

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

void Layout2D::mouseWheel() {
  // zoom axis individually or in group
  //  if (currentAxisRect_->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
  //    currentAxisRect_->setRangeZoom(ui->customPlot->xAxis->orientation());
  //  else if
  //  (ui->customPlot->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
  //    currentAxisRect_->setRangeZoom(ui->customPlot->yAxis->orientation());
  //  else
  currentAxisRect_->setRangeZoom(Qt::Horizontal | Qt::Vertical);
}

void Layout2D::beforeReplot() {
  if (currentAxisRect_) {
    currentAxisRect_->updateLegendRect();
  }
}

void Layout2D::refresh() {
  plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

bool Layout2D::exportGraph() {
  std::unique_ptr<ImageExportDialog2D> ied(
       new ImageExportDialog2D(nullptr, plot2dCanvas_ != nullptr));
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

void Layout2D::updateData(Table *table, const QString &name) {
  if (!currentAxisRect_) return;
  if (!table) return;
  Column *col = table->column(table->colIndex(name));
  if (!col) return;
  bool modified = false;
  foreach (AxisRect2D *axisrect, getAxisRectList()) {
    QVector<LineSpecial2D *> lslist = axisrect->getLsVec();
    QVector<Curve2D *> curvelist = axisrect->getCurveVec();
    QVector<Bar2D *> barlist = axisrect->getBarVec();
    QVector<Vector2D *> vectorlist = axisrect->getVectorVec();
    foreach (LineSpecial2D *ls, lslist) {
      PlotData::AssociatedData *data =
          ls->getdatablock_lsplot()->getassociateddata();
      if (data->table == table) {
        if (data->xcol == col || data->ycol == col) {
          ls->setGraphData(data->table, data->xcol, data->ycol, data->from,
                           data->to);
          modified = true;
        }
      }
    }
    foreach (Curve2D *curve, curvelist) {
      if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Associated) {
        PlotData::AssociatedData *data =
            curve->getdatablock_cplot()->getassociateddata();
        if (data->table == table) {
          if (data->xcol == col || data->ycol == col) {
            curve->setCurveData(data->table, data->xcol, data->ycol, data->from,
                                data->to);
            modified = true;
          }
        }
      }
    }

    foreach (Bar2D *bar, barlist) {
      if (!bar->ishistogram_barplot()) {
        PlotData::AssociatedData *data =
            bar->getdatablock_barplot()->getassociateddata();
        if (data->table == table) {
          if (data->xcol == col || data->ycol == col) {
            bar->setBarData(data->table, data->xcol, data->ycol, data->from,
                            data->to);
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
  }
  if (modified)
    plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void Layout2D::removeColumn(Table *table, const QString &name) {
  if (!currentAxisRect_) return;
  if (!table) return;
  Column *col = table->column(table->colIndex(name));
  if (!col) return;
  bool removed = false;
  foreach (AxisRect2D *axisrect, getAxisRectList()) {
    QVector<LineSpecial2D *> lslist = axisrect->getLsVec();
    QVector<Curve2D *> curvelist = axisrect->getCurveVec();
    QVector<Bar2D *> barlist = axisrect->getBarVec();
    QVector<Vector2D *> vectorlist = axisrect->getVectorVec();
    foreach (LineSpecial2D *ls, lslist) {
      PlotData::AssociatedData *data =
          ls->getdatablock_lsplot()->getassociateddata();
      if (data->table == table) {
        if (data->xcol == col || data->ycol == col) {
          axisrect->removeLineScatter2D(ls);
          removed = true;
        }
      }
    }
    foreach (Curve2D *curve, curvelist) {
      if (curve->getplottype_cplot() == Graph2DCommon::PlotType::Associated) {
        PlotData::AssociatedData *data =
            curve->getdatablock_cplot()->getassociateddata();
        if (data->table == table) {
          if (data->xcol == col || data->ycol == col) {
            axisrect->removeCurve2D(curve);
            removed = true;
          }
        }
      }
    }

    foreach (Bar2D *bar, barlist) {
      if (!bar->ishistogram_barplot()) {
        PlotData::AssociatedData *data =
            bar->getdatablock_barplot()->getassociateddata();
        if (data->table == table) {
          if (data->xcol == col || data->ycol == col) {
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
  }
  if (removed)
    plot2dCanvas_->replot(QCustomPlot::RefreshPriority::rpQueuedReplot);
}

void Layout2D::exportPDF(const QString &filename) {
  currentAxisRect_->setPrintorExportJob(true);
  plot2dCanvas_->savePdf(filename);
  currentAxisRect_->setPrintorExportJob(false);
}

void Layout2D::renderPlot(QPrinter *printer) {
  printer->setPageSize(QPrinter::A4);
  QCPPainter painter(printer);
  QRectF pageRect = printer->pageRect(QPrinter::DevicePixel);

  int plotWidth = plot2dCanvas_->viewport().width();
  int plotHeight = plot2dCanvas_->viewport().height();
  double scale = pageRect.width() / static_cast<double>(plotWidth);

  painter.setMode(QCPPainter::pmVectorized);
  painter.setMode(QCPPainter::pmNoCaching);
  // comment this out if you want cosmetic thin lines (always 1 pixel thick
  // independent of pdf zoom level)
  // painter.setMode(QCPPainter::pmNonCosmetic);
  painter.scale(scale, scale);
  currentAxisRect_->setPrintorExportJob(true);
  plot2dCanvas_->toPainter(&painter, plotWidth, plotHeight);
  currentAxisRect_->setPrintorExportJob(false);
}

void Layout2D::setLayoutDimension(QPair<int, int> dimension) {
  layoutDimension_.first = dimension.first;
  layoutDimension_.second = dimension.second;
}

void Layout2D::removeAxisRect(int index) {
  // if no elements to remove
  if (index < 0) {
    qDebug() << "no element to remove from the layout";
    return;
  }

  // if removed element is the currently selected element
  AxisRect2D *axrect = static_cast<AxisRect2D *>(layout_->elementAt(index));

  if (!axrect) return;

  if (axrect->isSelected()) {
    axrect = nullptr;
    if (index == 0) {
      axrect = static_cast<AxisRect2D *>(layout_->elementAt(index + 1));
    } else {
      axrect = static_cast<AxisRect2D *>(layout_->elementAt(index - 1));
    }

    if (axrect) {
      axisRectSetFocus(axrect);
    } else {
      currentAxisRect_ = nullptr;
    }
  }

  // remove layout button
  buttionlist_.takeLast()->close();

  // remove the element & adjust layout accordingly
  layout_->remove(layout_->elementAt(index));
  layout_->simplify();
  plot2dCanvas_->replot();
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

  connect(button, SIGNAL(clicked(LayoutButton2D *)), this,
          SLOT(activateLayout(LayoutButton2D *)));
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
    case Graph2DCommon::Picker::None:
      plot2dCanvas_->unsetCursor();
      break;
    case Graph2DCommon::Picker::DataPoint:
      plot2dCanvas_->setCursor(Qt::CursorShape::CrossCursor);
      break;
    case Graph2DCommon::Picker::DataGraph:
      plot2dCanvas_->setCursor(Qt::CursorShape::CrossCursor);
      break;
    case Graph2DCommon::Picker::DataMove:
      plot2dCanvas_->setCursor(Qt::CursorShape::OpenHandCursor);
      break;
    case Graph2DCommon::Picker::DataRemove:
      plot2dCanvas_->setCursor(Qt::CursorShape::PointingHandCursor);
      break;
  }
  QList<AxisRect2D *> axisrectlist = getAxisRectList();
  foreach (AxisRect2D *axisrect, axisrectlist) {
    axisrect->setGraphTool(picker_);
  }
}

void Layout2D::print() {
  QPrinter printer;
  QPrintPreviewDialog previewDialog(&printer, this);
  connect(&previewDialog, SIGNAL(paintRequested(QPrinter *)),
          SLOT(renderPlot(QPrinter *)));
  previewDialog.exec();
}

AxisRect2D *Layout2D::addAxisRectItem() {
  return addAxisRectItem(AlphaPlot::ColumnDataType::TypeDouble,
                         AlphaPlot::ColumnDataType::TypeDouble);
}
