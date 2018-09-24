#ifndef DATAMANAGER2D_H
#define DATAMANAGER2D_H

#include <QList>
#include <QVector>
#include "../3rdparty/qcustomplot/qcustomplot.h"

class Table;
class Column;

class DataBlockGraph {
 public:
  DataBlockGraph(Table *table, Column *xcolumn, Column *ycolumn, int from,
                 int to);
  ~DataBlockGraph();

  // setters
  void addat(const int position, const bool valid, const double x,
             const double y);
  // getters
  int size() { return data_->size(); }
  QList<bool> *validitylist() { return isvalid_; }
  QSharedPointer<QCPGraphDataContainer> data() { return data_; }
  QCPRange xrange() { return xrange_; }
  QCPRange yrange() { return yrange_; }
  Table *table() { return table_; }
  Column *xcolumn() { return xcolumn_; }
  Column *ycolumn() { return ycolumn_; }
  int from() { return from_; }
  int to() { return to_; }

 private:
  void generateDataBlockGraph();

 private:
  QList<bool> *isvalid_;
  QSharedPointer<QCPGraphDataContainer> data_;
  QCPRange xrange_;
  QCPRange yrange_;
  Table *table_;
  Column *xcolumn_;
  Column *ycolumn_;
  int from_;
  int to_;
};

class DataBlockCurve {
 public:
  DataBlockCurve(Table *table, Column *xcol, Column *ycol, int from, int to);
  ~DataBlockCurve();

  // setters
  void addat(const int position, const bool valid, const double x,
             const double y);
  // getters
  int size() { return data_->size(); }
  QList<bool> *validitylist() { return isvalid_; }
  QSharedPointer<QCPCurveDataContainer> data() { return data_; }
  QCPRange xrange() { return xrange_; }
  QCPRange yrange() { return yrange_; }
  Table *table() { return table_; }
  Column *xcolumn() { return xcol_; }
  Column *ycolumn() { return ycol_; }
  int from() { return from_; }
  int to() { return to_; }

 private:
  void generateDataBlockCurve();

 private:
  QList<bool> *isvalid_;
  QSharedPointer<QCPCurveDataContainer> data_;
  QCPRange xrange_;
  QCPRange yrange_;
  Table *table_;
  Column *xcol_;
  Column *ycol_;
  int from_;
  int to_;
};

#endif  // DATAMANAGER2D_H
