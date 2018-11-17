#include "Surface3D.h"
#include <qmath.h>
#include "Custom3DInteractions.h"

const int sampleCountX = 50;
const int sampleCountZ = 50;
const int heightMapGridStepX = 6;
const int heightMapGridStepZ = 6;
const float sampleMin = -8.0f;
const float sampleMax = 8.0f;

Surface3D::Surface3D(Q3DSurface *surface)
    : graph_(surface), custominter_(new Custom3DInteractions) {
  graph_->activeTheme()->setType(Q3DTheme::ThemeDigia);
  graph_->setActiveInputHandler(custominter_);
  graph_->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
  graph_->scene()->activeCamera()->setCameraPreset(
      Q3DCamera::CameraPresetFront);

  graph_->setAxisX(new QValue3DAxis);
  graph_->setAxisY(new QValue3DAxis);
  graph_->setAxisZ(new QValue3DAxis);

  sqrtSinProxy_ = new QSurfaceDataProxy();
  sqrtSinSeries_ = new QSurface3DSeries(sqrtSinProxy_);
  fillSqrtSinProxy();
  //graph_->addSeries(sqrtSinSeries_);
}

Surface3D::~Surface3D() {}

void Surface3D::fillfunctiondata(
    QList<QPair<QPair<double, double>, double> > *data) {
  QSurfaceDataArray *dataArray = new QSurfaceDataArray;
  dataArray->reserve(data->size());
  for (int i = 0; i < data->size(); i++) {
    QSurfaceDataRow *newRow = new QSurfaceDataRow(data->size());
    (*newRow)[i].setPosition(QVector3D(
        data->at(i).first.first, data->at(i).first.second, data->at(i).second));
    *dataArray << newRow;
  }
  sqrtSinProxy_->resetArray(dataArray);
  graph_->addSeries(sqrtSinSeries_);
}

void Surface3D::fillSqrtSinProxy() {
  float stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
  float stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);

  QSurfaceDataArray *dataArray = new QSurfaceDataArray;
  dataArray->reserve(sampleCountZ);
  for (int i = 0; i < sampleCountZ; i++) {
    QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);
    // Keep values within range bounds, since just adding step can cause minor
    // drift due
    // to the rounding errors.
    float z = qMin(sampleMax, (i * stepZ + sampleMin));
    qDebug() << "z = " << z;
    int index = 0;
    for (int j = 0; j < sampleCountX; j++) {
      float x = qMin(sampleMax, (j * stepX + sampleMin));
      float R = qSqrt(z * z + x * x) + 0.01f;
      float y = (qSin(R) / R + 0.24f) * 1.61f;
      (*newRow)[index++].setPosition(QVector3D(x, y, z));
      qDebug() << "x = " << x;
      qDebug() << "y = " << z;
    }
    *dataArray << newRow;
  }

  sqrtSinProxy_->resetArray(dataArray);
}
