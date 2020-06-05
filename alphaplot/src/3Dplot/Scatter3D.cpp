#include "Scatter3D.h"

#include "3Dplot/DataManager3D.h"
#include "Matrix.h"
#include "Table.h"
#include "future/core/column/Column.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

Scatter3D::Scatter3D(Q3DScatter *scatter)
    : graph_(scatter), data_(QVector<DataBlockScatter3D *>()) {
  graph_->setAxisX(new QValue3DAxis);
  graph_->setAxisY(new QValue3DAxis);
  graph_->setAxisZ(new QValue3DAxis);
}

Scatter3D::~Scatter3D() {}

void Scatter3D::settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                             Column *zcolumn) {
  DataBlockScatter3D *block =
      new DataBlockScatter3D(table, xcolumn, ycolumn, zcolumn);
  graph_->addSeries(block->getdataseries());
  data_ << block;
  block->setgradient(block->getdataseries(), Graph3DCommon::Gradient::BBRY);
  block->getdataseries()->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
  emit dataAdded();
}

void Scatter3D::setmatrixdatamodel(Matrix *matrix) {
  DataBlockScatter3D *block = new DataBlockScatter3D(matrix);
  graph_->addSeries(block->getdataseries());
  data_ << block;
  block->setgradient(block->getdataseries(), Graph3DCommon::Gradient::BBRY);
  block->getdataseries()->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
  emit dataAdded();
}

Q3DScatter *Scatter3D::getGraph() const { return graph_; }

QVector<DataBlockScatter3D *> Scatter3D::getData() const { return data_; }

void Scatter3D::save(XmlStreamWriter *xmlwriter)
{

}

void Scatter3D::load(XmlStreamReader *xmlreader)
{

}
