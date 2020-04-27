#include "Surface3D.h"

#include <qmath.h>

#include "Custom3DInteractions.h"
#include "DataManager3D.h"
#include "Matrix.h"
#include "Table.h"
#include "future/core/column/Column.h"

Surface3D::Surface3D(Q3DSurface *surface)
    : graph_(surface),
      plotType_(QSurface3DSeries::DrawFlag::DrawSurfaceAndWireframe),
      custominter_(new Custom3DInteractions),
      data_(nullptr) {
  graph_->activeTheme()->setType(Q3DTheme::ThemeDigia);
  graph_->setActiveInputHandler(custominter_);
  graph_->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
  graph_->scene()->activeCamera()->setCameraPreset(
      Q3DCamera::CameraPresetFront);

  graph_->setAxisX(new QValue3DAxis);
  graph_->setAxisY(new QValue3DAxis);
  graph_->setAxisZ(new QValue3DAxis);
}

Surface3D::~Surface3D() {}

void Surface3D::setfunctiondata(
    QList<QPair<QPair<double, double>, double>> *data,
    const Graph3DCommon::Function3DData &funcdata) {
  if (data_ != nullptr) return;
  data_ = new DataBlockSurface3D(data, funcdata);
  graph_->addSeries(data_->getdataseries());
  setGradient();
}

void Surface3D::settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                             QList<Column *> zcolumns) {
  if (data_ != nullptr) return;
  data_ = new DataBlockSurface3D(table, xcolumn, ycolumn, zcolumns);
  graph_->addSeries(data_->getdataseries());
  setGradient();
}

void Surface3D::setmatrixdatamodel(Matrix *matrix) {
  if (data_ != nullptr) return;
  data_ = new DataBlockSurface3D(matrix);
  graph_->addSeries(data_->getdataseries());
  setGradient();
}

Q3DSurface *Surface3D::getGraph() const { return graph_; }

DataBlockSurface3D *Surface3D::getData() const { return data_; }

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
