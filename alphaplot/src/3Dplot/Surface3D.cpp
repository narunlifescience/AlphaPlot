#include "Surface3D.h"

#include <qmath.h>

#include "DataManager3D.h"
#include "Matrix.h"
#include "Table.h"
#include "future/core/column/Column.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

using namespace QtDataVisualization;

Surface3D::Surface3D(Q3DSurface *surface)
    : graph_(surface),
      plotType_(QSurface3DSeries::DrawFlag::DrawSurfaceAndWireframe),
      data_(QVector<DataBlockSurface3D *>()) {
  graph_->setAxisX(new QValue3DAxis);
  graph_->setAxisY(new QValue3DAxis);
  graph_->setAxisZ(new QValue3DAxis);
}

Surface3D::~Surface3D() {}

void Surface3D::setfunctiondata(
    QList<QPair<QPair<double, double>, double>> *data,
    const Graph3DCommon::Function3DData &funcdata) {
  DataBlockSurface3D *block = new DataBlockSurface3D(data, funcdata);
  graph_->addSeries(block->getdataseries());
  data_ << block;
  block->setgradient(block->getdataseries(), Graph3DCommon::Gradient::BBRY);
  block->getdataseries()->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
  emit dataAdded();
}

void Surface3D::setmatrixdatamodel(Matrix *matrix) {
  DataBlockSurface3D *block = new DataBlockSurface3D(matrix);
  graph_->addSeries(block->getdataseries());
  data_ << block;
  block->setgradient(block->getdataseries(), Graph3DCommon::Gradient::BBRY);
  block->getdataseries()->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
  emit dataAdded();
}

Q3DSurface *Surface3D::getGraph() const { return graph_; }

QVector<DataBlockSurface3D *> Surface3D::getData() const { return data_; }

void Surface3D::save(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("plot");
  xmlwriter->writeAttribute("aspectratio",
                            QString::number(graph_->aspectRatio()));
  xmlwriter->writeAttribute("horizontalaspectratio",
                            QString::number(graph_->horizontalAspectRatio()));
  switch (graph_->shadowQuality()) {
    case QAbstract3DGraph::ShadowQuality::ShadowQualityLow:
      xmlwriter->writeAttribute("shadowquality", "low");
      break;
    case QAbstract3DGraph::ShadowQuality::ShadowQualityHigh:
      xmlwriter->writeAttribute("shadowquality", "high");
      break;
    case QAbstract3DGraph::ShadowQuality::ShadowQualityNone:
      xmlwriter->writeAttribute("shadowquality", "none");
      break;
    case QAbstract3DGraph::ShadowQuality::ShadowQualityMedium:
      xmlwriter->writeAttribute("shadowquality", "medium");
      break;
    case QAbstract3DGraph::ShadowQuality::ShadowQualitySoftLow:
      xmlwriter->writeAttribute("shadowquality", "softlow");
      break;
    case QAbstract3DGraph::ShadowQuality::ShadowQualitySoftHigh:
      xmlwriter->writeAttribute("shadowquality", "softhigh");
      break;
    case QAbstract3DGraph::ShadowQuality::ShadowQualitySoftMedium:
      xmlwriter->writeAttribute("shadowquality", "softmedium");
      break;
  }
  (graph_->flipHorizontalGrid())
      ? xmlwriter->writeAttribute("fliphorizontalgrid", "true")
      : xmlwriter->writeAttribute("fliphorizontalgrid", "false");
  (graph_->isOrthoProjection())
      ? xmlwriter->writeAttribute("orthoprojection", "true")
      : xmlwriter->writeAttribute("orthoprojection", "false");
  (graph_->isPolar())
      ? xmlwriter->writeAttribute("polar", "true")
      : xmlwriter->writeAttribute("polar", "false");
  xmlwriter->writeEndElement();
}

void Surface3D::load(XmlStreamReader *xmlreader, QList<Table *> tabs,
                     QList<Matrix *> mats) {}

void Surface3D::setSurfaceMeshType(const QSurface3DSeries::DrawFlag &type) {
  if (graph_->seriesList().isEmpty()) return;
  plotType_ = type;
  foreach (QSurface3DSeries *series, graph_->seriesList()) {
    series->setDrawMode(type);
  }
}

void Surface3D::setSurfaceMeshType(const QSurface3DSeries::DrawFlag &type,
                                   QSurface3DSeries *series) {
  series->setDrawMode(type);
}

QtDataVisualization::QSurface3DSeries::DrawFlag Surface3D::getSurfaceMeshType(
    QSurface3DSeries *series) const {
  if (series->drawMode().testFlag(QSurface3DSeries::DrawFlag::DrawSurface))
    return QSurface3DSeries::DrawFlag::DrawSurface;
  else if (series->drawMode().testFlag(
               QSurface3DSeries::DrawFlag::DrawSurfaceAndWireframe))
    return QSurface3DSeries::DrawFlag::DrawSurfaceAndWireframe;
  else if (series->drawMode().testFlag(
               QSurface3DSeries::DrawFlag::DrawWireframe))
    return QSurface3DSeries::DrawFlag::DrawWireframe;
  else
    return QSurface3DSeries::DrawFlag::DrawSurfaceAndWireframe;
}
