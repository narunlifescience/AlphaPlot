#ifndef DATAMANAGER2D_H
#define DATAMANAGER2D_H

#include <QList>
#include <QVector>
#include "../3rdparty/qcustomplot/qcustomplot.h"

class Table;
class Column;

class DataBlock2 {
 public:
  DataBlock2(Table *table, Column *xcolumn, Column *ycolumn, int from, int to);
  ~DataBlock2();

  // setters
  void addat(const int position, const bool valid, const double x,
             const double y);
  // getters
  int size() { return data_->size(); }
  QList<bool> *validitylist() { return isvalid_; }
  QVector<QCPGraphData> *data() { return data_; }
  QCPRange xrange() { return xrange_; }
  QCPRange yrange() { return yrange_; }
  Table *table() { return table_; }
  Column *xcolumn() { return xcolumn_; }
  Column *ycolumn() { return ycolumn_; }
  int from() { return from_; }
  int to() { return to_; }

 private:
  void generateDataBlock2();

 private:
  QList<bool> *isvalid_;
  QVector<QCPGraphData> *data_;
  QCPRange xrange_;
  QCPRange yrange_;
  Table *table_;
  Column *xcolumn_;
  Column *ycolumn_;
  int from_;
  int to_;
};

#endif  // DATAMANAGER2D_H
