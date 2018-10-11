#include "Bar2D.h"
#include "DataManager2D.h"
#include "Table.h"
#include "future/core/column/Column.h"

#include <gsl/gsl_histogram.h>
#include <gsl/gsl_vector.h>

Bar2D::Bar2D(Table *table, Column *xcol, Column *ycol, int from, int to,
             Axis2D *xAxis, Axis2D *yAxis)
    : QCPBars(xAxis, yAxis),
      barwidth_(1),
      xaxis_(xAxis),
      yaxis_(yAxis),
      bardata_(new DataBlockBar(table, xcol, ycol, from, to)),
      ishistogram_(false),
      picker_(Graph2DCommon::Picker::None) {
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setSelectable(QCP::SelectionType::stSingleData);
  setData(bardata_->data());
}

Bar2D::Bar2D(Table *table, Column *ycol, int from, int to, Axis2D *xAxis,
             Axis2D *yAxis)
    : QCPBars(xAxis, yAxis),
      barwidth_(1),
      xaxis_(xAxis),
      yaxis_(yAxis),
      ishistogram_(true) {
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  int d_end_row = to;
  int d_start_row = from;
  bool d_autoBin = true;
  int d_bin_size;
  double d_begin;
  double d_end;
  double d_mean;
  double d_standard_deviation;
  double d_min;
  double d_max;

  int r = abs(d_end_row - d_start_row) + 1;
  QVarLengthArray<double> Y(r);

  Column *y_col_ptr = ycol;
  int yColType = table->columnType(table->colIndex(ycol->name()));
  int size = 0;
  for (int row = d_start_row; row <= d_end_row && row < y_col_ptr->rowCount();
       row++) {
    if (!y_col_ptr->isInvalid(row)) {
      if (yColType == Table::Text) {
        QString yval = y_col_ptr->textAt(row);
        bool valid_data = true;
        Y[size] = QLocale().toDouble(yval, &valid_data);
        if (!valid_data) continue;
      } else
        Y[size] = y_col_ptr->valueAt(row);
      size++;
    }
  }

  if (size < 2 || (size == 2 && Y[0] == Y[1])) {  // non valid histogram
    double X[2];
    Y.resize(2);
    for (int i = 0; i < 2; i++) {
      Y[i] = 0;
      X[i] = 0;
    }
    return;
  }

  int n;
  gsl_histogram *h;
  if (d_autoBin) {
    n = 10;
    h = gsl_histogram_alloc(n);
    if (!h) return;

    gsl_vector *v = gsl_vector_alloc(size);
    for (int i = 0; i < size; i++) gsl_vector_set(v, i, Y[i]);

    double min, max;
    gsl_vector_minmax(v, &min, &max);
    gsl_vector_free(v);

    d_begin = floor(min);
    d_end = ceil(max);
    d_bin_size = (d_end - d_begin) / (double)n;

    gsl_histogram_set_ranges_uniform(h, floor(min), ceil(max));
  } else {
    n = int((d_end - d_begin) / d_bin_size + 1);
    h = gsl_histogram_alloc(n);
    if (!h) return;

    double *range = new double[n + 2];
    for (int i = 0; i <= n + 1; i++) range[i] = d_begin + i * d_bin_size;

    gsl_histogram_set_ranges(h, range, n + 1);
    delete[] range;
  }

  for (int i = 0; i < size; i++) gsl_histogram_increment(h, Y[i]);

  double X[n];  // stores ranges (x) and bins (y)
  Y.resize(n);
  QSharedPointer<QCPBarsDataContainer> cont =
      QSharedPointer<QCPBarsDataContainer>(new QCPBarsDataContainer);
  for (int i = 0; i < n; i++) {
    QCPBarsData dat;
    Y[i] = gsl_histogram_get(h, i);
    dat.value = gsl_histogram_get(h, i);
    double lower, upper;
    gsl_histogram_get_range(h, i, &lower, &upper);
    X[i] = lower;
    dat.key = lower;
    cont.data()->add(dat);
  }

  setData(cont);

  d_mean = gsl_histogram_mean(h);
  d_standard_deviation = gsl_histogram_sigma(h);
  d_min = gsl_histogram_min_val(h);
  d_max = gsl_histogram_max_val(h);

  gsl_histogram_free(h);

  return;
}

Bar2D::~Bar2D() {
  if (!ishistogram_) delete bardata_;
}

Axis2D *Bar2D::getxaxis_barplot() const { return xaxis_; }

Axis2D *Bar2D::getyaxis_barplot() const { return yaxis_; }

Qt::PenStyle Bar2D::getstrokestyle_barplot() const { return pen().style(); }

QColor Bar2D::getstrokecolor_barplot() const { return pen().color(); }

double Bar2D::getstrokethickness_barplot() const { return pen().widthF(); }

QColor Bar2D::getfillcolor_barplot() const { return brush().color(); }

DataBlockBar *Bar2D::getdatablock_barplot() const { return bardata_; }

bool Bar2D::ishistogram_barplot() const { return ishistogram_; }

void Bar2D::setxaxis_barplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Bottom ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Top);
  if (axis == getxaxis_barplot()) return;

  xaxis_ = axis;
  setKeyAxis(axis);
}

void Bar2D::setyaxis_barplot(Axis2D *axis) {
  Q_ASSERT(axis->getorientation_axis() == Axis2D::AxisOreantation::Left ||
           axis->getorientation_axis() == Axis2D::AxisOreantation::Right);
  if (axis == getyaxis_barplot()) return;

  yaxis_ = axis;
  setValueAxis(axis);
}

void Bar2D::setstrokestyle_barplot(const Qt::PenStyle &style) {
  QPen p = pen();
  p.setStyle(style);
  setPen(p);
}

void Bar2D::setstrokecolor_barplot(const QColor &color) {
  QPen p = pen();
  p.setColor(color);
  setPen(p);
}

void Bar2D::setstrokethickness_barplot(const double value) {
  QPen p = pen();
  p.setWidthF(value);
  setPen(p);
}

void Bar2D::setfillcolor_barplot(const QColor &color) {
  QBrush b = brush();
  b.setColor(color);
  setBrush(b);
}

void Bar2D::setBarData(Table *table, Column *xcol, Column *ycol, int from,
                       int to) {
  bardata_->regenerateDataBlock(table, xcol, ycol, from, to);
  setData(bardata_->data());
}

void Bar2D::setBarWidth(double barwidth) {
  setWidth(barwidth / static_cast<double>(data().data()->size()));
}

double Bar2D::getBarWidth() { return barwidth_; }

void Bar2D::setpicker_barplot(const Graph2DCommon::Picker picker) {
  picker_ = picker;
}

void Bar2D::mousePressEvent(QMouseEvent *event, const QVariant &details) {
  if (event->button() == Qt::LeftButton) {
    switch (picker_) {
      case Graph2DCommon::Picker::None:
        break;
      case Graph2DCommon::Picker::DataPoint:
        datapicker(event, details);
        break;
      case Graph2DCommon::Picker::DataGraph:
        graphpicker(event, details);
        break;
      case Graph2DCommon::Picker::DataMove:
        movepicker(event, details);
        break;
      case Graph2DCommon::Picker::DataRemove:
        removepicker(event, details);
        break;
    }
  }
  QCPBars::mousePressEvent(event, details);
}

void Bar2D::datapicker(QMouseEvent *event, const QVariant &details) {
  QCPBarsDataContainer::const_iterator it = data()->constEnd();
  QCPDataSelection dataPoints = details.value<QCPDataSelection>();
  if (dataPoints.dataPointCount() > 0) {
    dataPoints.dataRange();
    it = data()->at(dataPoints.dataRange().begin());
    QPointF point = coordsToPixels(it->mainKey(), it->mainValue());
    emit showtooltip(point, it->mainKey(), it->mainValue());
  }
}

void Bar2D::graphpicker(QMouseEvent *event, const QVariant &details) {
  double xvalue, yvalue;
  pixelsToCoords(event->posF(), xvalue, yvalue);
  emit showtooltip(event->posF(), xvalue, yvalue);
}

void Bar2D::movepicker(QMouseEvent *event, const QVariant &details) {}

void Bar2D::removepicker(QMouseEvent *event, const QVariant &details) {}
