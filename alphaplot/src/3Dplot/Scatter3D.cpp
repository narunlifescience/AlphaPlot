#include "Scatter3D.h"

#include "Custom3DInteractions.h"
#include "Matrix.h"
#include "Table.h"
#include "future/core/column/Column.h"

Scatter3D::Scatter3D(Q3DScatter *scatter)
    : graph_(scatter),
      matrix_(nullptr),
      custominter_(new Custom3DInteractions),
      dataArray_(new QScatterDataArray),
      dataProxy_(new QScatterDataProxy()),
      dataSeries_(new QScatter3DSeries),
      modelDataProxy_(new QItemModelScatterDataProxy) {
  graph_->activeTheme()->setType(Q3DTheme::ThemeDigia);
  graph_->setActiveInputHandler(custominter_);
  graph_->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
  graph_->scene()->activeCamera()->setCameraPreset(
      Q3DCamera::CameraPresetFront);

  graph_->setAxisX(new QValue3DAxis);
  graph_->setAxisY(new QValue3DAxis);
  graph_->setAxisZ(new QValue3DAxis);
  graph_->axisX()->setTitle("X");
  graph_->axisY()->setTitle("Y");
  graph_->axisZ()->setTitle("Z");
  graph_->axisX()->setTitleVisible(true);
  graph_->axisY()->setTitleVisible(true);
  graph_->axisZ()->setTitleVisible(true);
}

Scatter3D::~Scatter3D() {}

void Scatter3D::settabledata(Table *table, Column *xcolumn, Column *ycolumn, QList<Column *> zcolumns)
{

}

void Scatter3D::settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                             Column *zcolumn) {}

void Scatter3D::setmatrixdatamodel(Matrix *matrix) {
  matrix_ = matrix;
  // graph_->removeSeries(dataSeries_.get());
  /*modelDataProxy_.get()->setItemModel(matrix->getmodel());
  dataSeries_->setDataProxy(modelDataProxy_.get());
  dataSeries_->setMeshSmooth(true);
  graph_->addSeries(dataSeries_.get());*/

  dataArray_->resize((matrix_->numRows()) * (matrix_->numCols()));
  QScatterDataItem *ptrToDataArray = &dataArray_->first();

  for (int i = 0; i < matrix->numRows(); i++) {
    for (int j = 0; j < matrix_->numCols(); j++) {
      double x = i;
      double y = j;
      double z = matrix_->cell(i, j);
      ptrToDataArray->setPosition(QVector3D(y, z, x));
      ptrToDataArray++;
    }
  }

  dataProxy_->resetArray(dataArray_.get());
  dataSeries_->setDataProxy(dataProxy_.get());
  dataSeries_->setMeshSmooth(true);
  graph_->addSeries(dataSeries_.get());
  setGradient();
  graph_->show();
}

void Scatter3D::setGradient() {
  QLinearGradient gr;
  gr.setColorAt(0.2, Qt::black);
  gr.setColorAt(0.33, Qt::blue);
  gr.setColorAt(0.67, Qt::red);
  gr.setColorAt(1.0, Qt::yellow);

  dataSeries_->setBaseColor(Qt::red);
  dataSeries_->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
  graph_->setSelectionMode(QAbstract3DGraph::SelectionItemAndRow |
                           QAbstract3DGraph::SelectionSlice);
}

Matrix *Scatter3D::getMatrix() { return matrix_; }

Q3DScatter *Scatter3D::getGraph() const { return graph_; }
