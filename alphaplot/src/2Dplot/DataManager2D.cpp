#include "DataManager2D.h"

#include "Table.h"
#include "future/core/column/Column.h"

DataBlockGraph::DataBlockGraph(Table *table, Column *xcolumn, Column *ycolumn,
                               const int from, const int to)
    : data_(new QCPGraphDataContainer),
      table_(table),
      xcolumn_(xcolumn),
      ycolumn_(ycolumn),
      from_(from),
      to_(to) {
  regenerateDataBlock(table, xcolumn, ycolumn, from, to);
}

DataBlockGraph::~DataBlockGraph() {}

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
    if (xcol->isInvalid(row) || ycol->isInvalid(row)) {
      QCPGraphData data(std::numeric_limits<double>::quiet_NaN(),
                        std::numeric_limits<double>::quiet_NaN());
      data_.data()->add(data);
    } else {
      double xdata = std::numeric_limits<double>::quiet_NaN();
      double ydata = std::numeric_limits<double>::quiet_NaN();
      switch (xcol->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          xdata = xcol->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          xdata = xcol->dateTimeAt(row).toTime_t() +
                  static_cast<uint>(24 * 3600 * i);
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          xdata = i;
          break;
      }
      switch (ycol->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          ydata = ycol->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          ydata = ycol->dateTimeAt(row).toTime_t() +
                  static_cast<uint>(24 * 3600 * i);
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          ydata = i;
          break;
      }
      QCPGraphData data(xdata, ydata);
      data_.data()->add(data);
    }
    i++;
  }
}

DataBlockCurve::DataBlockCurve(Table *table, Column *xcol, Column *ycol,
                               const int from, const int to)
    : data_(new QCPCurveDataContainer),
      table_(table),
      xcolumn_(xcol),
      ycolumn_(ycol),
      from_(from),
      to_(to) {
  regenerateDataBlock(table, xcol, ycol, from, to);
}

DataBlockCurve::~DataBlockCurve() {}

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
    if (xcol->isInvalid(row) || ycol->isInvalid(row)) {
      QCPCurveData data(i, std::numeric_limits<double>::quiet_NaN(),
                        std::numeric_limits<double>::quiet_NaN());
      data_->add(data);
    } else {
      double xdata = std::numeric_limits<double>::quiet_NaN();
      double ydata = std::numeric_limits<double>::quiet_NaN();
      switch (xcol->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          xdata = xcol->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          xdata = xcol->dateTimeAt(row).toTime_t() +
                  static_cast<uint>(24 * 3600 * i);
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          xdata = i;
          break;
      }
      switch (ycol->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          ydata = ycol->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          ydata = ycol->dateTimeAt(row).toTime_t() +
                  static_cast<uint>(24 * 3600 * i);
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          ydata = i;
          break;
      }

      QCPCurveData data(i, xdata, ydata);
      data_->add(data);
    }
    i++;
  }
}

DataBlockBar::DataBlockBar(Table *table, Column *xcol, Column *ycol,
                           const int from, const int to)
    : data_(new QCPBarsDataContainer),
      table_(table),
      xcolumn_(xcol),
      ycolumn_(ycol),
      from_(from),
      to_(to) {
  regenerateDataBlock(table, xcol, ycol, from, to);
}

DataBlockBar::~DataBlockBar() {}

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
    if (xcolumn->isInvalid(row) || ycolumn->isInvalid(row)) {
      QCPBarsData data(std::numeric_limits<double>::quiet_NaN(),
                       std::numeric_limits<double>::quiet_NaN());
      data_->add(data);
    } else {
      double xdata = std::numeric_limits<double>::quiet_NaN();
      double ydata = std::numeric_limits<double>::quiet_NaN();
      switch (xcolumn->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          xdata = xcolumn->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          xdata = xcolumn->dateTimeAt(row).toTime_t() +
                  static_cast<uint>(24 * 3600 * i);
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          xdata = i;
          break;
      }
      switch (ycolumn->dataType()) {
        case AlphaPlot::ColumnDataType::TypeDouble:
          ydata = ycolumn->valueAt(row);
          break;
        case AlphaPlot::ColumnDataType::TypeDateTime:
          ydata = ycolumn->dateTimeAt(row).toTime_t() +
                  static_cast<uint>(24 * 3600 * i);
          break;
        case AlphaPlot::ColumnDataType::TypeString:
          ydata = i;
          break;
      }
      QCPBarsData data(xdata, ydata);
      data_->add(data);
    }
    i++;
  }
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
