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

void Surface3D::save(XmlStreamWriter *xmlwriter, const bool saveastemplate) {
  xmlwriter->writeStartElement("plotbasic");
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
  (graph_->isPolar()) ? xmlwriter->writeAttribute("polar", "true")
                      : xmlwriter->writeAttribute("polar", "false");
  // graph
  if (!saveastemplate) {
    foreach (DataBlockSurface3D *block, data_) {
      xmlwriter->writeStartElement("plot");
      // data
      if (block->ismatrix()) {
        xmlwriter->writeAttribute("data", "matrix");
        xmlwriter->writeAttribute("matrix", block->getmatrix()->name());
      } else if (block->istable()) {
        xmlwriter->writeAttribute("data", "table");
        xmlwriter->writeAttribute("table", block->gettable()->name());
        xmlwriter->writeAttribute("xcolumn", block->getxcolumn()->name());
        xmlwriter->writeAttribute("ycolumn", block->getycolumn()->name());
        xmlwriter->writeAttribute("zcolumn", block->getzcolumn()->name());
      } else {
        xmlwriter->writeAttribute("data", "function");
        xmlwriter->writeAttribute("function", block->getfunction());
        xmlwriter->writeAttribute("xl", QString::number(block->getxlower()));
        xmlwriter->writeAttribute("xu", QString::number(block->getxupper()));
        xmlwriter->writeAttribute("yl", QString::number(block->getylower()));
        xmlwriter->writeAttribute("yu", QString::number(block->getyupper()));
        xmlwriter->writeAttribute("zl", QString::number(block->getzlower()));
        xmlwriter->writeAttribute("zu", QString::number(block->getzupper()));
        xmlwriter->writeAttribute("xpoints",
                                  QString::number(block->getxpoints()));
        xmlwriter->writeAttribute("ypoints",
                                  QString::number(block->getypoints()));
      }
      QSurface3DSeries *series = block->getdataseries();
      (series->isVisible()) ? xmlwriter->writeAttribute("visible", "true")
                            : xmlwriter->writeAttribute("visible", "false");
      if (series->isFlatShadingEnabled())
        (series->isFlatShadingEnabled())
            ? xmlwriter->writeAttribute("flatshading", "true")
            : xmlwriter->writeAttribute("flatshading", "false");
      switch (series->drawMode()) {
        case QSurface3DSeries::DrawFlag::DrawSurface:
          xmlwriter->writeAttribute("drawmode", "surface");
          break;
        case QSurface3DSeries::DrawFlag::DrawWireframe:
          xmlwriter->writeAttribute("drawmode", "wireframe");
          break;
        case QSurface3DSeries::DrawFlag::DrawSurfaceAndWireframe:
          xmlwriter->writeAttribute("drawmode", "surfaceandwireframe");
          break;
      }
      (series->isMeshSmooth())
          ? xmlwriter->writeAttribute("meshsmooth", "true")
          : xmlwriter->writeAttribute("meshsmooth", "false");
      switch (series->colorStyle()) {
        case Q3DTheme::ColorStyle::ColorStyleUniform:
          xmlwriter->writeAttribute("colorstyle", "solidcolor");
          break;
        case Q3DTheme::ColorStyle::ColorStyleRangeGradient:
          xmlwriter->writeAttribute("colorstyle", "rangegradient");
          break;
        case Q3DTheme::ColorStyle::ColorStyleObjectGradient:
          xmlwriter->writeAttribute("colorstyle", "objectgradient");
          break;
      }
      switch (block->getgradient()) {
        case Graph3DCommon::Gradient::Grayscale:
          xmlwriter->writeAttribute("gradientcolor", "grayscale");
          break;
        case Graph3DCommon::Gradient::Hot:
          xmlwriter->writeAttribute("gradientcolor", "hot");
          break;
        case Graph3DCommon::Gradient::Ion:
          xmlwriter->writeAttribute("gradientcolor", "ion");
          break;
        case Graph3DCommon::Gradient::Jet:
          xmlwriter->writeAttribute("gradientcolor", "jet");
          break;
        case Graph3DCommon::Gradient::BBRY:
          xmlwriter->writeAttribute("gradientcolor", "bbry");
          break;
        case Graph3DCommon::Gradient::Cold:
          xmlwriter->writeAttribute("gradientcolor", "cold");
          break;
        case Graph3DCommon::Gradient::GYRD:
          xmlwriter->writeAttribute("gradientcolor", "gyrd");
          break;
        case Graph3DCommon::Gradient::Hues:
          xmlwriter->writeAttribute("gradientcolor", "hues");
          break;
        case Graph3DCommon::Gradient::Candy:
          xmlwriter->writeAttribute("gradientcolor", "candy");
          break;
        case Graph3DCommon::Gradient::Night:
          xmlwriter->writeAttribute("gradientcolor", "night");
          break;
        case Graph3DCommon::Gradient::Polar:
          xmlwriter->writeAttribute("gradientcolor", "polar");
          break;
        case Graph3DCommon::Gradient::Thermal:
          xmlwriter->writeAttribute("gradientcolor", "thermal");
          break;
        case Graph3DCommon::Gradient::Spectrum:
          xmlwriter->writeAttribute("gradientcolor", "spectrum");
          break;
        case Graph3DCommon::Gradient::Geography:
          xmlwriter->writeAttribute("gradientcolor", "geography");
          break;
      }
      xmlwriter->writeAttribute("solidcolor",
                                series->baseColor().name(QColor::HexArgb));
      xmlwriter->writeAttribute(
          "highlightcolor",
          series->singleHighlightColor().name(QColor::HexArgb));
      xmlwriter->writeEndElement();
    }
  }
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
