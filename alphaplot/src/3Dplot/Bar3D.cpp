#include "Bar3D.h"

#include <qmath.h>

#include "DataManager3D.h"
#include "Matrix.h"
#include "Table.h"
#include "future/core/column/Column.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

Bar3D::Bar3D(Q3DBars *bar)
    : graph_(bar), data_(QVector<DataBlockBar3D *>()), counter_(0) {
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

void Bar3D::save(XmlStreamWriter *xmlwriter, const bool saveastemplate) {
  xmlwriter->writeStartElement("bar");
  xmlwriter->writeAttribute(
      "xrotation",
      QString::number(graph_->scene()->activeCamera()->xRotation()));
  xmlwriter->writeAttribute(
      "yrotation",
      QString::number(graph_->scene()->activeCamera()->yRotation()));
  xmlwriter->writeAttribute(
      "zoomlevel",
      QString::number(graph_->scene()->activeCamera()->zoomLevel()));
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
  if (!saveastemplate) {
    foreach (DataBlockBar3D *block, data_) {
      xmlwriter->writeStartElement("plot");
      // data
      if (block->ismatrix()) {
        xmlwriter->writeAttribute("data", "matrix");
        xmlwriter->writeAttribute("matrix", block->getmatrix()->name());
      } else {
        xmlwriter->writeAttribute("data", "table");
        xmlwriter->writeAttribute("table", block->gettable()->name());
        xmlwriter->writeAttribute("xcolumn", block->getxcolumn()->name());
        xmlwriter->writeAttribute("ycolumn", block->getycolumn()->name());
        xmlwriter->writeAttribute("zcolumn", block->getzcolumn()->name());
      }
      QBar3DSeries *series = block->getdataseries();
      (series->isVisible()) ? xmlwriter->writeAttribute("visible", "true")
                            : xmlwriter->writeAttribute("visible", "false");
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

void Bar3D::load(XmlStreamReader *xmlreader, QList<Table *> tabs,
                 QList<Matrix *> mats) {
  while (!xmlreader->atEnd()) {
    if (xmlreader->isEndElement() && xmlreader->name() == "bar") break;
    xmlreader->readNext();
    if (xmlreader->isEndElement() && xmlreader->name() == "bar") break;
    if (xmlreader->isStartElement() && xmlreader->name() == "bar") {
      bool ok = false;
      bool xrstatus = false;
      bool yrstatus = false;
      bool zlstatus = false;

      // x rotation
      double xrotation = xmlreader->readAttributeDouble("xrotation", &ok);
      if (ok) xrstatus = true;

      // y rotation
      double yrotation = xmlreader->readAttributeDouble("yrotation", &ok);
      if (ok) yrstatus = true;

      // zoomlevel
      double zoomlevel = xmlreader->readAttributeDouble("zoomlevel", &ok);
      if (ok) zlstatus = true;

      if (xrstatus && yrstatus && zlstatus)
        graph_->scene()->activeCamera()->setCameraPosition(xrotation, yrotation,
                                                           zoomlevel);
      else
        xmlreader->raiseWarning(
            "Bar3D cameraposition property setting error");

      // aspect ratio
      double aspectratio = xmlreader->readAttributeDouble("aspectratio", &ok);
      if (ok)
        graph_->setAspectRatio(aspectratio);
      else
        xmlreader->raiseWarning("Bar3D aspectratio property setting error");

      // horizontal aspect ratio
      double haspectratio =
          xmlreader->readAttributeDouble("horizontalaspectratio", &ok);
      if (ok)
        graph_->setHorizontalAspectRatio(haspectratio);
      else
        xmlreader->raiseWarning(
            "Bar3D horizontal aspectratio property setting error");

      // shadow quality
      QString shadowquality =
          xmlreader->readAttributeString("shadowquality", &ok);
      if (ok) {
        if (shadowquality == "low") {
          graph_->setShadowQuality(
              QAbstract3DGraph::ShadowQuality::ShadowQualityLow);
        } else if (shadowquality == "high") {
          graph_->setShadowQuality(
              QAbstract3DGraph::ShadowQuality::ShadowQualityHigh);
        } else if (shadowquality == "none") {
          graph_->setShadowQuality(
              QAbstract3DGraph::ShadowQuality::ShadowQualityNone);
        } else if (shadowquality == "medium") {
          graph_->setShadowQuality(
              QAbstract3DGraph::ShadowQuality::ShadowQualityMedium);
        } else if (shadowquality == "softlow") {
          graph_->setShadowQuality(
              QAbstract3DGraph::ShadowQuality::ShadowQualitySoftLow);
        } else if (shadowquality == "softhigh") {
          graph_->setShadowQuality(
              QAbstract3DGraph::ShadowQuality::ShadowQualitySoftHigh);
        } else if (shadowquality == "softmedium") {
          graph_->setShadowQuality(
              QAbstract3DGraph::ShadowQuality::ShadowQualitySoftMedium);
        }
      } else
        xmlreader->raiseWarning("Bar3D shadow quality property setting error");

      // relative bar spacing
      bool relsp = xmlreader->readAttributeBool("relativespacing", &ok);
      if (ok)
        graph_->setBarSpacingRelative(relsp);
      else
        xmlreader->raiseWarning(
            "Bar3D relative bar spacing property setting error");

      // bar spacing
      double spx = xmlreader->readAttributeDouble("spacingx", &ok);
      if (ok) {
        double spy = xmlreader->readAttributeDouble("spacingy", &ok);
        if (ok) {
          graph_->setBarSpacing(QSizeF(spx, spy));
        } else
          xmlreader->raiseWarning("Bar3D bar spacing Y property setting error");
      } else
        xmlreader->raiseWarning("Bar3D bar spacing X property setting error");

      // bar thickness
      double barthickness = xmlreader->readAttributeDouble("thickness", &ok);
      if (ok)
        graph_->setBarThickness(barthickness);
      else
        xmlreader->raiseWarning("Bar3D bar thickness property setting error");

      // ortho projection
      bool orthoproj = xmlreader->readAttributeBool("orthoprojection", &ok);
      if (ok)
        graph_->setOrthoProjection(orthoproj);
      else
        xmlreader->raiseWarning(
            "Bar3D ortho projection property setting error");

      // flip horizontal grid
      bool polar = xmlreader->readAttributeBool("polar", &ok);
      if (ok)
        graph_->setPolar(polar);
      else
        xmlreader->raiseWarning("Bar3D polar property setting error");

      counter_ = 0;
      loadplot(xmlreader, tabs, mats);
    }
  }
}

void Bar3D::loadplot(XmlStreamReader *xmlreader, QList<Table *> tabs,
                     QList<Matrix *> mats) {
  while (!xmlreader->atEnd()) {
    if (xmlreader->isEndElement() && xmlreader->name() == "plot") break;
    xmlreader->readNext();
    if (xmlreader->isEndElement() && xmlreader->name() == "plot") break;
    bool ok = false;
    // data
    QString data = xmlreader->readAttributeString("data", &ok);
    if (!ok) xmlreader->raiseError("Bar3D data property setting error");

    bool loadseries = false;
    // matrix data
    if (data == "matrix") {
      Matrix *matrix = nullptr;
      QString matname = xmlreader->readAttributeString("matrix", &ok);
      if (ok) {
        matrix = getMatrixByName(mats, matname);
      } else
        xmlreader->raiseError(tr("Bar3D Matrix not found error"));
      if (matrix) {
        setmatrixdatamodel(matrix);
        loadseries = true;
      }
      // Table data
    } else if (data == "table") {
      Table *table = nullptr;
      Column *xcolumn = nullptr;
      Column *ycolumn = nullptr;
      Column *zcolumn = nullptr;
      QString tablename = xmlreader->readAttributeString("table", &ok);
      if (ok) {
        table = getTableByName(tabs, tablename);
      } else
        xmlreader->raiseError(tr("Bar3D Table not found error"));
      QString xcolname = xmlreader->readAttributeString("xcolumn", &ok);
      if (ok) {
        (table) ? xcolumn = table->column(xcolname) : xcolumn = nullptr;
      } else
        xmlreader->raiseWarning(tr("Bar3D Table X column not found error"));
      QString ycolname = xmlreader->readAttributeString("ycolumn", &ok);
      if (ok) {
        (table) ? ycolumn = table->column(ycolname) : ycolumn = nullptr;
      } else
        xmlreader->raiseWarning(tr("Bar3D Table Y column not found error"));
      QString zcolname = xmlreader->readAttributeString("zcolumn", &ok);
      if (ok) {
        (table) ? zcolumn = table->column(zcolname) : zcolumn = nullptr;
      } else
        xmlreader->raiseWarning(tr("Bar3D Table Z column not found error"));
      if (table && xcolumn && ycolumn && zcolumn) {
        settabledata(table, xcolumn, ycolumn, zcolumn);
        loadseries = true;
      }
    }

    if (loadseries) {
      // visible
      QBar3DSeries *series = data_.at(counter_)->getdataseries();
      bool vis = xmlreader->readAttributeBool("visible", &ok);
      (ok) ? series->setVisible(vis)
           : xmlreader->raiseWarning(
                 "Bar3D visible series property setting error");

      // smooth
      bool smooth = xmlreader->readAttributeBool("meshsmooth", &ok);
      (ok) ? series->setMeshSmooth(smooth)
           : xmlreader->raiseWarning(
                 "Bar3D meshsmooth series property setting error");

      // color style
      QString colorstyle = xmlreader->readAttributeString("colorstyle", &ok);
      if (ok) {
        if (colorstyle == "solidcolor")
          series->setColorStyle(Q3DTheme::ColorStyle::ColorStyleUniform);
        else if (colorstyle == "rangegradient")
          series->setColorStyle(Q3DTheme::ColorStyle::ColorStyleRangeGradient);
        else if (colorstyle == "objectgradient")
          series->setColorStyle(Q3DTheme::ColorStyle::ColorStyleObjectGradient);
      } else
        xmlreader->raiseWarning(
            "Bar3D colorstyle series property setting error");

      // gradient
      QString gradient = xmlreader->readAttributeString("gradientcolor", &ok);
      if (ok) {
        if (gradient == "grayscale")
          data_.at(counter_)->setgradient(series,
                                          Graph3DCommon::Gradient::Grayscale);
        else if (gradient == "hot")
          data_.at(counter_)->setgradient(series, Graph3DCommon::Gradient::Hot);
        else if (gradient == "ion")
          data_.at(counter_)->setgradient(series, Graph3DCommon::Gradient::Ion);
        else if (gradient == "jet")
          data_.at(counter_)->setgradient(series, Graph3DCommon::Gradient::Jet);
        else if (gradient == "bbry")
          data_.at(counter_)->setgradient(series,
                                          Graph3DCommon::Gradient::BBRY);
        else if (gradient == "cold")
          data_.at(counter_)->setgradient(series,
                                          Graph3DCommon::Gradient::Cold);
        else if (gradient == "gyrd")
          data_.at(counter_)->setgradient(series,
                                          Graph3DCommon::Gradient::GYRD);
        else if (gradient == "hues")
          data_.at(counter_)->setgradient(series,
                                          Graph3DCommon::Gradient::Hues);
        else if (gradient == "candy")
          data_.at(counter_)->setgradient(series,
                                          Graph3DCommon::Gradient::Candy);
        else if (gradient == "night")
          data_.at(counter_)->setgradient(series,
                                          Graph3DCommon::Gradient::Night);
        else if (gradient == "polar")
          data_.at(counter_)->setgradient(series,
                                          Graph3DCommon::Gradient::Polar);
        else if (gradient == "thermal")
          data_.at(counter_)->setgradient(series,
                                          Graph3DCommon::Gradient::Thermal);
        else if (gradient == "spectrum")
          data_.at(counter_)->setgradient(series,
                                          Graph3DCommon::Gradient::Spectrum);
        else if (gradient == "geography")
          data_.at(counter_)->setgradient(series,
                                          Graph3DCommon::Gradient::Geography);
      } else
        xmlreader->raiseWarning(
            "Bar3D gradient color series property setting error");

      // Solid Color
      QString solidcolor = xmlreader->readAttributeString("solidcolor", &ok);
      (ok) ? series->setBaseColor(QColor(solidcolor))
           : xmlreader->raiseWarning(
                 "Bar3D series solid color property setting error");

      // Highlight Color
      QString hcolor = xmlreader->readAttributeString("highlightcolor", &ok);
      (ok) ? series->setSingleHighlightColor(QColor(hcolor))
           : xmlreader->raiseWarning(
                 "Bar3D series highlight color property setting error");
    }
  }
  xmlreader->readNext();
  if (xmlreader->isStartElement() && xmlreader->name() == "plot") {
    loadplot(xmlreader, tabs, mats);
    counter_++;
  }
}

Table *Bar3D::getTableByName(QList<Table *> tabs, const QString name) {
  Table *table = nullptr;
  foreach (Table *tab, tabs) {
    if (tab->name() == name) table = tab;
  }
  return table;
}

Matrix *Bar3D::getMatrixByName(QList<Matrix *> mats, const QString name) {
  Matrix *matrix = nullptr;
  foreach (Matrix *mat, mats) {
    if (mat->name() == name) matrix = mat;
  }
  return matrix;
}
