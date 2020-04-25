#include "Surface3D.h"

#include <qmath.h>

#include "Custom3DInteractions.h"
#include "Matrix.h"

Surface3D::Surface3D(Q3DSurface *surface)
    : graph_(surface),
      matrix_(nullptr),
      plotType_(QSurface3DSeries::DrawFlag::DrawSurfaceAndWireframe),
      custominter_(new Custom3DInteractions),
      functionDataProxy_(new QSurfaceDataProxy()),
      dataSeries_(new QSurface3DSeries),
      modelDataProxy_(new QItemModelSurfaceDataProxy) {
  graph_->activeTheme()->setType(Q3DTheme::ThemeDigia);
  graph_->setActiveInputHandler(custominter_);
  graph_->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
  graph_->scene()->activeCamera()->setCameraPreset(
      Q3DCamera::CameraPresetFront);

  graph_->setAxisX(new QValue3DAxis);
  graph_->setAxisY(new QValue3DAxis);
  graph_->setAxisZ(new QValue3DAxis);
  graph_->axisX()->setTitleVisible(true);
  graph_->axisY()->setTitleVisible(true);
  graph_->axisZ()->setTitleVisible(true);
}

Surface3D::~Surface3D() {}

void Surface3D::setfunctiondata(
    QList<QPair<QPair<double, double>, double>> *data,
    const Graph3DCommon::Function3DData &funcdata) {
  functionData_ = funcdata;
  int points = funcdata.xpoints;
  QSurfaceDataArray *dataArray = new QSurfaceDataArray;
  dataArray->reserve(points);
  for (int i = 0; i < points * points;) {
    // create a new row
    QSurfaceDataRow *newRow = new QSurfaceDataRow(points);
    // get pointer to firsr row
    QSurfaceDataItem *newRowPtr = &newRow->first();
    for (int j = 0; j < points; j++) {
      double x = data->at(i).first.first;
      double y = data->at(i).first.second;
      double z = data->at(i).second;
      newRowPtr->setPosition(QVector3D(y, z, x));
      newRowPtr++;
      i++;
    }
    *dataArray << newRow;
  }
  // delete data
  data->clear();
  delete data;
  functionDataProxy_->resetArray(dataArray);
  dataSeries_.get()->setDataProxy(functionDataProxy_.get());
  graph_->addSeries(dataSeries_.get());
  setGradient();
}

void Surface3D::setmatrixdatamodel(Matrix *matrix) {
  matrix_ = matrix;
  modelDataProxy_.get()->setItemModel(matrix->getmodel());
  modelDataProxy_->setUseModelCategories(true);
  dataSeries_.get()->setDataProxy(modelDataProxy_.get());
  graph_->addSeries(dataSeries_.get());
  setGradient();
}

Matrix *Surface3D::getMatrix() { return matrix_; }

Q3DSurface *Surface3D::getGraph() const { return graph_; }

void Surface3D::setGradient() {
  QLinearGradient gr;
  gr.setColorAt(0.0, Qt::black);
  gr.setColorAt(0.33, Qt::blue);
  gr.setColorAt(0.67, Qt::red);
  gr.setColorAt(1.0, Qt::yellow);

  graph_->seriesList().at(0)->setBaseGradient(gr);
  setSurfaceMeshType(QSurface3DSeries::DrawFlag::DrawSurfaceAndWireframe);
  graph_->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}

void Surface3D::setSurfaceMeshType(const QSurface3DSeries::DrawFlag &type) {
  if (graph_->seriesList().isEmpty()) return;
  plotType_ = type;
  graph_->seriesList().at(0)->setDrawMode(type);
}

QSurface3DSeries::DrawFlag Surface3D::getSurfaceMeshType() const {
  return plotType_;
}
