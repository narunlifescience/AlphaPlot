#include "DataManager2D.h"

#include <gsl/gsl_histogram.h>
#include <gsl/gsl_vector.h>

#include "Table.h"
#include "future/core/column/Column.h"

DataBlockGraph::DataBlockGraph(Table *table, Column *xcolumn, Column *ycolumn,
                               const int from, const int to)
    : data_(new QCPGraphDataContainer),
      associateddata_(new PlotData::AssociatedData) {
  associateddata_->table = table;
  associateddata_->xcol = xcolumn;
  associateddata_->ycol = ycolumn;
  associateddata_->from = from;
  associateddata_->to = to;
  regenerateDataBlock(table, xcolumn, ycolumn, from, to);
}

DataBlockGraph::~DataBlockGraph() { delete associateddata_; }

void DataBlockGraph::regenerateDataBlock(Table *table, Column *xcolumn,
                                         Column *ycolumn, const int from,
                                         const int to) {
  this->settable(table);
  this->setxcolumn(xcolumn);
  this->setycolumn(ycolumn);
  this->setfrom(from);
  this->setto(to);

  Column *xcol = this->getxcolumn();
  Column *ycol = this->getycolumn();
  int start_row = this->getfrom();
  int end_row = this->getto();

  data_.data()->clear();

  // strip unused end rows
  if (end_row >= xcol->rowCount()) end_row = xcol->rowCount() - 1;
  if (end_row >= ycol->rowCount()) end_row = ycol->rowCount() - 1;

  // determine rows for which all columns have valid content
  for (int i = 0, row = start_row; row <= end_row; row++) {
    if (!xcol->isInvalid(row) && !ycol->isInvalid(row)) {
      double xdata = std::numeric_limits<double>::quiet_NaN();
      double ydata = std::numeric_limits<double>::quiet_NaN();
      switch (xcol->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          xdata = xcol->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          xdata = QCPAxisTickerDateTime::dateTimeToKey(xcol->dateTimeAt(row));
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          xdata = i;
          break;
        default:
          break;
      }
      switch (ycol->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          ydata = ycol->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          ydata = QCPAxisTickerDateTime::dateTimeToKey(ycol->dateTimeAt(row));
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          ydata = i;
          break;
        default:
          break;
      }
      QCPGraphData data(xdata, ydata);
      data_.data()->add(data);
    }
    i++;
  }
}

bool DataBlockGraph::movedatafromtable(const double key, const double value,
                                       const double newkey,
                                       const double newvalue) {
  for (int i = associateddata_->from; i < associateddata_->to + 1; i++) {
    if (associateddata_->xcol->valueAt(i) == key) {
      if (associateddata_->ycol->valueAt(i) == value) {
        associateddata_->xcol->asStringColumn()->setTextAt(
            i, QString::number(newkey));
        associateddata_->ycol->asStringColumn()->setTextAt(
            i, QString::number(newvalue));
        return true;
      }
    }
  }
  qDebug() << "unable to move data point " << key << ", " << value
           << " in column(s)" << associateddata_->xcol->name() << ", "
           << associateddata_->ycol->name()
           << " from associated table: " << associateddata_->table->name();
  return false;
}

bool DataBlockGraph::removedatafromtable(const double key, const double value) {
  for (int i = associateddata_->from; i < associateddata_->to + 1; i++) {
    if (associateddata_->xcol->valueAt(i) == key) {
      if (associateddata_->ycol->valueAt(i) == value) {
        associateddata_->xcol->asStringColumn()->setTextAt(i, QString());
        associateddata_->ycol->asStringColumn()->setTextAt(i, QString());
        return true;
      }
    }
  }
  qDebug() << "unable to find data point " << key << ", " << value
           << " in column(s)" << associateddata_->xcol->name() << ", "
           << associateddata_->ycol->name()
           << " from associated table: " << associateddata_->table->name();
  return false;
}

DataBlockCurve::DataBlockCurve(Table *table, Column *xcol, Column *ycol,
                               const int from, const int to)
    : data_(new QCPCurveDataContainer),
      associateddata_(new PlotData::AssociatedData) {
  associateddata_->table = table;
  associateddata_->xcol = xcol;
  associateddata_->ycol = ycol;
  associateddata_->from = from;
  associateddata_->to = to;
  regenerateDataBlock(table, xcol, ycol, from, to);
}

DataBlockCurve::~DataBlockCurve() { delete associateddata_; }

void DataBlockCurve::regenerateDataBlock(Table *table, Column *xcolumn,
                                         Column *ycolumn, const int from,
                                         const int to) {
  this->settable(table);
  this->setxcolumn(xcolumn);
  this->setycolumn(ycolumn);
  this->setfrom(from);
  this->setto(to);

  Column *xcol = this->getxcolumn();
  Column *ycol = this->getycolumn();
  int start_row = this->getfrom();
  int end_row = this->getto();

  data_.data()->clear();

  // strip unused end rows
  if (end_row >= xcol->rowCount()) end_row = xcol->rowCount() - 1;
  if (end_row >= ycol->rowCount()) end_row = ycol->rowCount() - 1;

  // determine rows for which all columns have valid content
  for (int i = 0, row = start_row; row <= end_row; row++) {
    if (!xcol->isInvalid(row) && !ycol->isInvalid(row)) {
      double xdata = std::numeric_limits<double>::quiet_NaN();
      double ydata = std::numeric_limits<double>::quiet_NaN();
      switch (xcol->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          xdata = xcol->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          xdata = QCPAxisTickerDateTime::dateTimeToKey(xcol->dateTimeAt(row));
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          xdata = i;
          break;
        default:
          break;
      }
      switch (ycol->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          ydata = ycol->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          ydata = QCPAxisTickerDateTime::dateTimeToKey(ycol->dateTimeAt(row));
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          ydata = i;
          break;
        default:
          break;
      }

      QCPCurveData data(i, xdata, ydata);
      data_->add(data);
    }
    i++;
  }
}

bool DataBlockCurve::movedatafromtable(const double key, const double value,
                                       const double newkey,
                                       const double newvalue) {
  for (int i = associateddata_->from; i < associateddata_->to + 1; i++) {
    if (associateddata_->xcol->valueAt(i) == key) {
      if (associateddata_->ycol->valueAt(i) == value) {
        associateddata_->xcol->asStringColumn()->setTextAt(
            i, QString::number(newkey));
        associateddata_->ycol->asStringColumn()->setTextAt(
            i, QString::number(newvalue));
        return true;
      }
    }
  }
  qDebug() << "unable to move data point " << key << ", " << value
           << " in column(s)" << associateddata_->xcol->name() << ", "
           << associateddata_->ycol->name()
           << " from associated table: " << associateddata_->table->name();
  return false;
}

bool DataBlockCurve::removedatafromtable(const double key, const double value) {
  for (int i = associateddata_->from; i < associateddata_->to + 1; i++) {
    if (associateddata_->xcol->valueAt(i) == key) {
      if (associateddata_->ycol->valueAt(i) == value) {
        associateddata_->xcol->asStringColumn()->setTextAt(i, QString());
        associateddata_->ycol->asStringColumn()->setTextAt(i, QString());
        return true;
      }
    }
  }
  qDebug() << "unable to find data point " << key << ", " << value
           << " in column(s)" << associateddata_->xcol->name() << ", "
           << associateddata_->ycol->name()
           << " from associated table: " << associateddata_->table->name();
  return false;
}

DataBlockBar::DataBlockBar(Table *table, Column *xcol, Column *ycol,
                           const int from, const int to)
    : data_(new QCPBarsDataContainer),
      associateddata_(new PlotData::AssociatedData) {
  associateddata_->table = table;
  associateddata_->xcol = xcol;
  associateddata_->ycol = ycol;
  associateddata_->from = from;
  associateddata_->to = to;
  regenerateDataBlock(table, xcol, ycol, from, to);
}

DataBlockBar::~DataBlockBar() { delete associateddata_; }

void DataBlockBar::regenerateDataBlock(Table *table, Column *xcolumn,
                                       Column *ycolumn, const int from,
                                       const int to) {
  this->settable(table);
  this->setxcolumn(xcolumn);
  this->setycolumn(ycolumn);
  this->setfrom(from);
  this->setto(to);

  int start_row = from;
  int end_row = to;

  data_.data()->clear();

  // strip unused end rows
  if (end_row >= xcolumn->rowCount()) end_row = xcolumn->rowCount() - 1;
  if (end_row >= ycolumn->rowCount()) end_row = ycolumn->rowCount() - 1;

  // determine rows for which all columns have valid content
  for (int i = 0, row = start_row; row <= end_row; row++) {
    if (!xcolumn->isInvalid(row) && !ycolumn->isInvalid(row)) {
      double xdata = std::numeric_limits<double>::quiet_NaN();
      double ydata = std::numeric_limits<double>::quiet_NaN();
      switch (xcolumn->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          xdata = xcolumn->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          xdata =
              QCPAxisTickerDateTime::dateTimeToKey(xcolumn->dateTimeAt(row));
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          xdata = i;
          break;
        default:
          break;
      }
      switch (ycolumn->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          ydata = ycolumn->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          ydata =
              QCPAxisTickerDateTime::dateTimeToKey(ycolumn->dateTimeAt(row));
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          ydata = i;
          break;
        default:
          break;
      }
      QCPBarsData data(xdata, ydata);
      data_->add(data);
    }
    i++;
  }
}

bool DataBlockBar::movedatafromtable(const double key, const double value,
                                     const double newkey,
                                     const double newvalue) {
  for (int i = associateddata_->from; i < associateddata_->to + 1; i++) {
    if (associateddata_->xcol->valueAt(i) == key) {
      if (associateddata_->ycol->valueAt(i) == value) {
        associateddata_->xcol->asStringColumn()->setTextAt(
            i, QString::number(newkey));
        associateddata_->ycol->asStringColumn()->setTextAt(
            i, QString::number(newvalue));
        return true;
      }
    }
  }
  qDebug() << "unable to move data point " << key << ", " << value
           << " in column(s)" << associateddata_->xcol->name() << ", "
           << associateddata_->ycol->name()
           << " from associated table: " << associateddata_->table->name();
  return false;
}

bool DataBlockBar::removedatafromtable(const double key, const double value) {
  for (int i = associateddata_->from; i < associateddata_->to + 1; i++) {
    if (associateddata_->xcol->valueAt(i) == key) {
      if (associateddata_->ycol->valueAt(i) == value) {
        associateddata_->xcol->asStringColumn()->setTextAt(i, QString());
        associateddata_->ycol->asStringColumn()->setTextAt(i, QString());
        return true;
      }
    }
  }
  qDebug() << "unable to find data point " << key << ", " << value
           << " in column(s)" << associateddata_->xcol->name() << ", "
           << associateddata_->ycol->name()
           << " from associated table: " << associateddata_->table->name();
  return false;
}

DataBlockError::DataBlockError(Table *table, Column *errorcol, const int from,
                               const int to)
    : data_(new QCPErrorBarsDataContainer),
      table_(table),
      errorcolumn_(errorcol),
      from_(from),
      to_(to) {
  regenerateDataBlock(table, errorcol, from, to);
}

DataBlockError::~DataBlockError() {}

void DataBlockError::regenerateDataBlock(Table *table, Column *errorcolumn,
                                         const int from, const int to) {
  this->settable(table);
  this->seterrorcolumn(errorcolumn);
  this->setfrom(from);
  this->setto(to);

  int start_row = from;
  int end_row = to;

  data_.data()->clear();

  // strip unused end rows
  if (end_row >= errorcolumn->rowCount()) end_row = errorcolumn->rowCount() - 1;

  // determine rows for which all columns have valid content
  for (int row = start_row; row <= end_row; row++) {
    if (errorcolumn->isInvalid(row)) {
      QCPErrorBarsData data(std::numeric_limits<double>::quiet_NaN());
      data_->append(data);
    } else {
      double errordata = errorcolumn->valueAt(row);
      QCPErrorBarsData data(errordata);
      data_->append(data);
    }
  }
}

DataBlockHist::DataBlockHist(Table *table, Column *col, const int from,
                             const int to)
    : data_(new QCPBarsDataContainer), histdata_(new PlotData::HistData) {
  histdata_->table = table;
  histdata_->col = col;
  histdata_->from = from;
  histdata_->to = to;
  regenerateDataBlock(table, col, from, to);
}

DataBlockHist::~DataBlockHist() {}

void DataBlockHist::regenerateDataBlock(Table *table, Column *col,
                                        const int from, const int to) {
  data_.data()->clear();
  settable(table);
  setcolumn(col);
  setfrom(from);
  setto(to);
  int d_end_row = to;
  int d_start_row = from;

  double d_mean = 0.0;
  double d_standard_deviation = 0.0;
  double d_min = 0.0;
  double d_max = 0.0;

  int r = abs(d_end_row - d_start_row) + 1;
  QVarLengthArray<double> Y(r);

  Column *y_col_ptr = col;
  int yColType = table->columnType(table->colIndex(col->name()));
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
  if (histdata_->autobin) {
    n = 10;
    h = gsl_histogram_alloc(n);
    if (!h) return;

    gsl_vector *v = gsl_vector_alloc(size);
    for (int i = 0; i < size; i++) gsl_vector_set(v, i, Y[i]);

    double min, max;
    gsl_vector_minmax(v, &min, &max);
    gsl_vector_free(v);

    histdata_->begin = floor(min);
    histdata_->end = ceil(max);
    histdata_->binsize =
        (histdata_->end - histdata_->begin) / static_cast<double>(n);

    gsl_histogram_set_ranges_uniform(h, floor(min), ceil(max));
  } else {
    n = static_cast<int>(
        (histdata_->end - histdata_->begin) / histdata_->binsize + 1);
    h = gsl_histogram_alloc(n);
    if (!h) return;

    double *range = new double[n + 2];
    for (int i = 0; i <= n + 1; i++)
      range[i] = histdata_->begin + i * histdata_->binsize;

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

  data_ = cont;

  d_mean = gsl_histogram_mean(h);
  d_standard_deviation = gsl_histogram_sigma(h);
  d_min = gsl_histogram_min_val(h);
  d_max = gsl_histogram_max_val(h);

  gsl_histogram_free(h);
}
