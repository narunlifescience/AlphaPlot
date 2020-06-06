#include "Bar3D.h"

#include <qmath.h>

#include "DataManager3D.h"
#include "Matrix.h"
#include "Table.h"
#include "future/core/column/Column.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

Bar3D::Bar3D(Q3DBars *bar) : graph_(bar), data_(QVector<DataBlockBar3D *>()) {
  graph_->setColumnAxis(new QCategory3DAxis);
  graph_->setRowAxis(new QCategory3DAxis);
  graph_->setValueAxis(new QValue3DAxis);
}

Bar3D::~Bar3D() {}

void Bar3D::settabledata(Table *table, Column *xcolumn, Column *ycolumn,
                         Column *zcolumn) {
  DataBlockBar3D *block = new DataBlockBar3D(table, xcolumn, ycolumn, zcolumn);
  graph_->addSeries(block->getdataseries());
  data_ << block;
  block->setgradient(block->getdataseries(), Graph3DCommon::Gradient::BBRY);
  block->getdataseries()->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
  emit dataAdded();
}

void Bar3D::setmatrixdatamodel(Matrix *matrix) {
  DataBlockBar3D *block = new DataBlockBar3D(matrix);
  graph_->addSeries(block->getdataseries());
  data_ << block;
  block->setgradient(block->getdataseries(), Graph3DCommon::Gradient::BBRY);
  block->getdataseries()->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
  emit dataAdded();
}

Q3DBars *Bar3D::getGraph() const { return graph_; }

QVector<DataBlockBar3D *> Bar3D::getData() const { return data_; }

void Bar3D::save(XmlStreamWriter *xmlwriter) {
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
  (graph_->isBarSpacingRelative())
      ? xmlwriter->writeAttribute("relativespacing", "true")
      : xmlwriter->writeAttribute("relativespacing", "false");
  xmlwriter->writeAttribute("spacingx",
                            QString::number(graph_->barSpacing().rwidth()));
  xmlwriter->writeAttribute("spacingy",
                            QString::number(graph_->barSpacing().rheight()));
  xmlwriter->writeAttribute("thickness",
                            QString::number(graph_->barThickness()));
  (graph_->isOrthoProjection())
      ? xmlwriter->writeAttribute("orthoprojection", "true")
      : xmlwriter->writeAttribute("orthoprojection", "false");
  (graph_->isPolar()) ? xmlwriter->writeAttribute("polar", "true")
                      : xmlwriter->writeAttribute("polar", "false");
  xmlwriter->writeEndElement();
}

void Bar3D::load(XmlStreamReader *xmlreader, QList<Table *> tabs,
                 QList<Matrix *> mats) {}
