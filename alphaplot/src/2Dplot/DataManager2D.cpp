#include "DataManager2D.h"

#include <QObject>
#include "../future/core/column/Column.h"
#include "Table.h"

DataBlockGraph::DataBlockGraph(Table *table, Column *xcolumn, Column *ycolumn,
                               int from, int to)
    : isvalid_(new QList<bool>()),
      data_(new QCPGraphDataContainer),
      table_(table),
      xcolumn_(xcolumn),
      ycolumn_(ycolumn),
      from_(from),
      to_(to) {
  generateDataBlockGraph();
}

DataBlockGraph::~DataBlockGraph() { delete isvalid_; }

void DataBlockGraph::addat(const int position, const bool valid, const double x,
                           const double y) {
  Q_ASSERT(position >= 0);
  isvalid_->insert(position, valid);
  data_->add(QCPGraphData(x, y));

  if (xrange_.lower > x) {
    xrange_.lower = x;
  }

  if (xrange_.upper > x) {
    xrange_.upper = x;
  }

  if (yrange_.lower > y) {
    yrange_.lower = y;
  }

  if (yrange_.upper > y) {
    yrange_.upper = y;
  }
}

void DataBlockGraph::generateDataBlockGraph() {
  QVector<QCPGraphData> *gdvector = new QVector<QCPGraphData>();

  // strip unused end rows
  int end_row = to_;
  if (end_row >= xcolumn_->rowCount()) end_row = xcolumn_->rowCount() - 1;
  if (end_row >= ycolumn_->rowCount()) end_row = ycolumn_->rowCount() - 1;

  // determine rows for which all columns have valid content
  for (int row = from_; row <= end_row; row++) {
    if (xcolumn_->isInvalid(row) || ycolumn_->isInvalid(row)) {
      isvalid_->append(false);
      QCPGraphData data(std::numeric_limits<double>::quiet_NaN(),
                        std::numeric_limits<double>::quiet_NaN());
      gdvector->append(data);
    } else {
      isvalid_->append(true);
      double xdata = xcolumn_->valueAt(row);
      double ydata = ycolumn_->valueAt(row);
      QCPGraphData data(xdata, ydata);
      gdvector->append(data);

      if (xrange_.lower > xdata) {
        xrange_.lower = xdata;
      }

      if (xrange_.upper > xdata) {
        xrange_.upper = xdata;
      }

      if (yrange_.lower > ydata) {
        yrange_.lower = ydata;
      }

      if (yrange_.upper > ydata) {
        yrange_.upper = ydata;
      }
    }
  }
  data_.data()->add(*gdvector, true);
  gdvector->clear();
  delete gdvector;
}

DataBlockCurve::DataBlockCurve(Table *table, Column *xcol, Column *ycol,
                               int from, int to)
    : isvalid_(new QList<bool>()),
      data_(new QCPCurveDataContainer),
      table_(table),
      xcol_(xcol),
      ycol_(ycol),
      from_(from),
      to_(to) {
  generateDataBlockCurve();
}

DataBlockCurve::~DataBlockCurve() { delete isvalid_; }

void DataBlockCurve::addat(const int position, const bool valid, const double x,
                           const double y) {
  Q_ASSERT(position >= 0);
  isvalid_->insert(position, valid);
  data_->add(QCPCurveData(position, x, y));

  if (xrange_.lower > x) {
    xrange_.lower = x;
  }

  if (xrange_.upper > x) {
    xrange_.upper = x;
  }

  if (yrange_.lower > y) {
    yrange_.lower = y;
  }

  if (yrange_.upper > y) {
    yrange_.upper = y;
  }
}

void DataBlockCurve::generateDataBlockCurve() {
  // strip unused end rows
  int end_row = to_;
  if (end_row >= xcol_->rowCount()) end_row = xcol_->rowCount() - 1;
  if (end_row >= ycol_->rowCount()) end_row = ycol_->rowCount() - 1;

  // determine rows for which all columns have valid content
  for (int i = 0, row = from_; row <= end_row; row++) {
    if (xcol_->isInvalid(row) || ycol_->isInvalid(row)) {
      isvalid_->append(false);
      QCPCurveData data(i, std::numeric_limits<double>::quiet_NaN(),
                        std::numeric_limits<double>::quiet_NaN());
      data_->add(data);
    } else {
      isvalid_->append(true);
      double xdata = xcol_->valueAt(row);
      double ydata = ycol_->valueAt(row);
      QCPCurveData data(i, xdata, ydata);
      data_->add(data);

      if (xrange_.lower > xdata) {
        xrange_.lower = xdata;
      }

      if (xrange_.upper > xdata) {
        xrange_.upper = xdata;
      }

      if (yrange_.lower > ydata) {
        yrange_.lower = ydata;
      }

      if (yrange_.upper > ydata) {
        yrange_.upper = ydata;
      }
    }
  }
}
