#ifndef SCATTER3D_H
#define SCATTER3D_H

#include <QtDataVisualization/Q3DScatter>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QItemModelScatterDataProxy>

#include "3Dplot/Graph3DCommon.h"

class Matrix;
class Table;
class Column;
class DataBlockScatter3D;
class XmlStreamWriter;
class XmlStreamReader;
using namespace QtDataVisualization;

class Scatter3D : public QObject {
  Q_OBJECT
 public:
  Scatter3D(Q3DScatter *scatter);
  ~Scatter3D();

  void settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                    Column *zcolumn);
  void setmatrixdatamodel(Matrix *matrix);
  Q3DScatter *getGraph() const;
  QVector<DataBlockScatter3D *> getData() const;
  void save(XmlStreamWriter *xmlwriter);
  void load(XmlStreamReader *xmlreader, QList<Table *> tabs,
            QList<Matrix *> mats);

 signals:
  void dataAdded();

 private:
  Q3DScatter *graph_;
  QVector<DataBlockScatter3D *> data_;
};
#endif  // SCATTER3D_H
