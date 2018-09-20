#include "DataManager2D.h"

#include <QObject>
#include "../future/core/column/Column.h"
#include "Table.h"

DataBlock2::DataBlock2(Table *table, Column *xcolumn, Column *ycolumn, int from,
                       int to)
    : isvalid_(new QList<bool>()),
      data_(new QVector<QCPGraphData>()),
      table_(table),
      xcolumn_(xcolumn),
      ycolumn_(ycolumn),
      from_(from),
      to_(to) {
  generateDataBlock2();
}

DataBlock2::~DataBlock2() {
  delete isvalid_;
  delete data_;
}

void DataBlock2::addat(const int position, const bool valid, const double x,
                       const double y) {
  Q_ASSERT(position >= 0);
  isvalid_->insert(position, valid);
  data_->insert(position, QCPGraphData(x, y));

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

void DataBlock2::generateDataBlock2() {
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
      data_->append(data);
    } else {
      isvalid_->append(true);
      double xdata = xcolumn_->valueAt(row);
      double ydata = ycolumn_->valueAt(row);
      QCPGraphData data(xdata, ydata);
      data_->append(data);

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

DataBlockCurveData::DataBlockCurveData(Table *table, QString xcolname,
                                       QString ycolname, int from, int to)
    : isvalid_(new QList<bool>()),
      data_(new QCPCurveDataContainer),
      table_(table),
      xcolname_(xcolname),
      ycolname_(ycolname),
      from_(from),
      to_(to) {
  generateDataBlockCurveData();
}

DataBlockCurveData::~DataBlockCurveData() {
  delete isvalid_;
}

void DataBlockCurveData::addat(const int position, const bool valid,
                               const double x, const double y) {
  Q_ASSERT(position >= 0);
  isvalid_->insert(position, valid);
  data_->add( QCPCurveData(position, x, y));

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

void DataBlockCurveData::generateDataBlockCurveData() {
  // strip unused end rows
  int end_row = to_;
  Column *xcolumn_ = table_->column(table_->colIndex(xcolname_));
  Column *ycolumn_ = table_->column(table_->colIndex(ycolname_));
  if (end_row >= xcolumn_->rowCount()) end_row = xcolumn_->rowCount() - 1;
  if (end_row >= ycolumn_->rowCount()) end_row = ycolumn_->rowCount() - 1;

  // determine rows for which all columns have valid content
  for (int i = 0, row = from_; row <= end_row; row++) {
    if (xcolumn_->isInvalid(row) || ycolumn_->isInvalid(row)) {
      isvalid_->append(false);
      QCPCurveData data(i, std::numeric_limits<double>::quiet_NaN(),
                        std::numeric_limits<double>::quiet_NaN());
      data_->add(data);
    } else {
      isvalid_->append(true);
      double xdata = xcolumn_->valueAt(row);
      double ydata = ycolumn_->valueAt(row);
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
