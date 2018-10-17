#ifndef DATAMANAGER2D_H
#define DATAMANAGER2D_H

#include <QList>
#include <QVector>
#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Graph2DCommon.h"

class Table;
class Column;

class DataBlockGraph {
 public:
  DataBlockGraph(Table *table, Column *xcolumn, Column *ycolumn,
                 const int start_row, const int end_row);
  ~DataBlockGraph();
  void regenerateDataBlock(Table *table, Column *xcolumn, Column *ycolumn,
                           const int start_row, const int end_row);

  // getters
  int size() const { return data_->size(); }
  QSharedPointer<QCPGraphDataContainer> data() const { return data_; }
  PlotData::AssociatedData *getassociateddata() {return  associateddata_;}
  Table *gettable() const { return associateddata_->table; }
  Column *getxcolumn() const { return associateddata_->xcol; }
  Column *getycolumn() { return associateddata_->ycol; }
  int getfrom() const { return associateddata_->from; }
  int getto() const { return associateddata_->to; }

  // Setters
  void settable(Table *table) { associateddata_->table = table; }
  void setxcolumn(Column *column) { associateddata_->xcol = column; }
  void setycolumn(Column *column) { associateddata_->ycol = column; }
  void setfrom(const int from) { associateddata_->from = from; }
  void setto(const int to) { associateddata_->to = to; }

 private:
  QSharedPointer<QCPGraphDataContainer> data_;
  PlotData::AssociatedData *associateddata_;
};

class DataBlockCurve {
 public:
  DataBlockCurve(Table *table, Column *xcol, Column *ycol, const int from,
                 const int to);
  ~DataBlockCurve();
  void regenerateDataBlock(Table *table, Column *xcolumn, Column *ycolumn,
                           const int from, const int to);

  // getters
  int size() const { return data_->size(); }
  QSharedPointer<QCPCurveDataContainer> data() const { return data_; }
  PlotData::AssociatedData *getassociateddata() {return  associateddata_;}
  Table *gettable() const { return associateddata_->table; }
  Column *getxcolumn() const { return associateddata_->xcol; }
  Column *getycolumn() { return associateddata_->ycol; }
  int getfrom() const { return associateddata_->from; }
  int getto() const { return associateddata_->to; }

  // Setters
  void settable(Table *table) { associateddata_->table = table; }
  void setxcolumn(Column *column) { associateddata_->xcol = column; }
  void setycolumn(Column *column) { associateddata_->ycol = column; }
  void setfrom(const int from) { associateddata_->from = from; }
  void setto(const int to) { associateddata_->to = to; }

 private:
  QSharedPointer<QCPCurveDataContainer> data_;
  PlotData::AssociatedData *associateddata_;
};

class DataBlockBar {
 public:
  DataBlockBar(Table *table, Column *xcol, Column *ycol, const int from,
               const int to);
  ~DataBlockBar();
  void regenerateDataBlock(Table *table, Column *xcolumn, Column *ycolumn,
                           const int from, const int to);

  // getters
  int size() const { return data_->size(); }
  QSharedPointer<QCPBarsDataContainer> data() const { return data_; }
  PlotData::AssociatedData *getassociateddata() {return  associateddata_;}
  Table *gettable() const { return associateddata_->table; }
  Column *getxcolumn() const { return associateddata_->xcol; }
  Column *getycolumn() { return associateddata_->ycol; }
  int getfrom() const { return associateddata_->from; }
  int getto() const { return associateddata_->to; }

  // Setters
  void settable(Table *table) { associateddata_->table = table; }
  void setxcolumn(Column *column) { associateddata_->xcol = column; }
  void setycolumn(Column *column) { associateddata_->ycol = column; }
  void setfrom(const int from) { associateddata_->from = from; }
  void setto(const int to) { associateddata_->to = to; }

 private:
  QSharedPointer<QCPBarsDataContainer> data_;
PlotData::AssociatedData *associateddata_;
};

class DataBlockError {
 public:
  DataBlockError(Table *table, Column *errorcol, const int from, const int to);
  ~DataBlockError();
  void regenerateDataBlock(Table *table, Column *errorcolumn, const int from,
                           const int to);

  // getters
  int size() const { return data_->size(); }
  QSharedPointer<QCPErrorBarsDataContainer> data() const { return data_; }
  Table *gettable() const { return table_; }
  Column *geterrorcolumn() const { return errorcolumn_; }
  int getfrom() const { return from_; }
  int getto() const { return to_; }

  // Setters
  void settable(Table *table) { table_ = table; }
  void seterrorcolumn(Column *column) { errorcolumn_ = column; }
  void setfrom(const int from) { from_ = from; }
  void setto(const int to) { to_ = to; }

 private:
  QSharedPointer<QCPErrorBarsDataContainer> data_;
  Table *table_;
  Column *errorcolumn_;
  int from_;
  int to_;
};

#endif  // DATAMANAGER2D_H
