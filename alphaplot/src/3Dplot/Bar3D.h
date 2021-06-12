#ifndef BAR3D_H
#define BAR3D_H

#include <QtDataVisualization/Q3DBars>
#include <memory>

#include "3Dplot/Graph3DCommon.h"

class Matrix;
class Table;
class Column;
class DataBlockBar3D;
class XmlStreamWriter;
class XmlStreamReader;

using namespace QtDataVisualization;

class Bar3D : public QObject {
  Q_OBJECT
 public:
  Bar3D(Q3DBars *bar);
  ~Bar3D();

  void settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                    Column *zcolumn);
  void setmatrixdatamodel(Matrix *matrix);

  Q3DBars *getGraph() const;
  QVector<DataBlockBar3D *> getData() const;

  void save(XmlStreamWriter *xmlwriter, const bool saveastemplate = false);
  void load(XmlStreamReader *xmlreader, QList<Table *> tabs,
            QList<Matrix *> mats);

 signals:
  void dataAdded();

 private:
  void loadplot(XmlStreamReader *xmlreader, QList<Table *> tabs,
                QList<Matrix *> mats);
  Table *getTableByName(QList<Table *> tabs, const QString name);
  Matrix *getMatrixByName(QList<Matrix *> mats, const QString name);
  Q3DBars *graph_;
  QVector<DataBlockBar3D *> data_;
  int counter_;
};

#endif  // BAR3D_H
