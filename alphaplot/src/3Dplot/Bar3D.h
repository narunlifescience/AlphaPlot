#ifndef BAR3D_H
#define BAR3D_H

#include <QtDataVisualization/Q3DBars>
#include <QtDataVisualization/QItemModelBarDataProxy>
#include <memory>

#include "3Dplot/Graph3DCommon.h"

class Custom3DInteractions;
class Matrix;
class Table;
class Column;
class DataBlockBar3D;

using namespace QtDataVisualization;

class Bar3D : public QObject {
  Q_OBJECT
 public:
  Bar3D(Q3DBars *bar);
  ~Bar3D();

  void settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                    QList<Column *> zcolumns);
  void settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                    Column *zcolumn);
  void setmatrixdatamodel(Matrix *matrix);

  Q3DBars *getGraph() const;
  DataBlockBar3D * getData() const;

 private:
  void setGradient();

 private:
  Q3DBars *graph_;
  Custom3DInteractions *custominter_;
  DataBlockBar3D *data_;
};

#endif  // BAR3D_H
