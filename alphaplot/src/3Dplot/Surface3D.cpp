#include "Surface3D.h"

#include <qmath.h>

#include "ApplicationWindow.h"
#include "DataManager3D.h"
#include "Matrix.h"
#include "Table.h"
#include "future/core/column/Column.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"
#include "scripting/Script.h"
#include "scripting/ScriptingEnv.h"

using namespace QtDataVisualization;

Surface3D::Surface3D(Q3DSurface *surface)
    : graph_(surface),
      plotType_(QSurface3DSeries::DrawFlag::DrawSurfaceAndWireframe),
      data_(QVector<DataBlockSurface3D *>()),
      counter_(0) {
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
  xmlwriter->writeStartElement("surface");
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
                     QList<Matrix *> mats, ApplicationWindow *app) {
  while (!xmlreader->atEnd()) {
    if (xmlreader->isEndElement() && xmlreader->name() == "surface") break;
    xmlreader->readNext();
    if (xmlreader->isEndElement() && xmlreader->name() == "surface") break;
    if (xmlreader->isStartElement() && xmlreader->name() == "surface") {
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
            "Surface3D cameraposition property setting error");

      // aspect ratio
      double aspectratio = xmlreader->readAttributeDouble("aspectratio", &ok);
      if (ok)
        graph_->setAspectRatio(aspectratio);
      else
        xmlreader->raiseWarning("Surface3D aspectratio property setting error");

      // horizontal aspect ratio
      double haspectratio =
          xmlreader->readAttributeDouble("horizontalaspectratio", &ok);
      if (ok)
        graph_->setHorizontalAspectRatio(haspectratio);
      else
        xmlreader->raiseWarning(
            "Surface3D horizontal aspectratio property setting error");

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
        xmlreader->raiseWarning(
            "Surface3D shadow quality property setting error");

      // flip horizontal grid
      bool fliphgrid = xmlreader->readAttributeBool("fliphorizontalgrid", &ok);
      if (ok)
        graph_->setFlipHorizontalGrid(fliphgrid);
      else
        xmlreader->raiseWarning(
            "Surface3D flip horizontal grid property setting error");

      // ortho projection
      bool orthoproj = xmlreader->readAttributeBool("orthoprojection", &ok);
      if (ok)
        graph_->setOrthoProjection(orthoproj);
      else
        xmlreader->raiseWarning(
            "Surface3D ortho projection property setting error");

      // flip horizontal grid
      bool polar = xmlreader->readAttributeBool("polar", &ok);
      if (ok)
        graph_->setPolar(polar);
      else
        xmlreader->raiseWarning("Surface3D polar property setting error");

      counter_ = 0;
      loadplot(xmlreader, tabs, mats, app);
    }
  }
}

void Surface3D::loadplot(XmlStreamReader *xmlreader, QList<Table *> tabs,
                         QList<Matrix *> mats, ApplicationWindow *app) {
  while (!xmlreader->atEnd()) {
    if (xmlreader->isEndElement() && xmlreader->name() == "plot") break;
    xmlreader->readNext();
    if (xmlreader->isEndElement() && xmlreader->name() == "plot") break;
    bool ok = false;
    // data
    QString data = xmlreader->readAttributeString("data", &ok);
    if (!ok) xmlreader->raiseError("Surface3D data property setting error");

    bool loadseries = false;
    // matrix data
    if (data == "matrix") {
      Matrix *matrix = nullptr;
      QString matname = xmlreader->readAttributeString("matrix", &ok);
      if (ok) {
        matrix = getMatrixByName(mats, matname);
      } else
        xmlreader->raiseError(tr("Surface3D Matrix not found error"));
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
        xmlreader->raiseError(tr("Surface3D Table not found error"));
      QString xcolname = xmlreader->readAttributeString("xcolumn", &ok);
      if (ok) {
        (table) ? xcolumn = table->column(xcolname) : xcolumn = nullptr;
      } else
        xmlreader->raiseWarning(tr("Surface3D Table X column not found error"));
      QString ycolname = xmlreader->readAttributeString("ycolumn", &ok);
      if (ok) {
        (table) ? ycolumn = table->column(ycolname) : ycolumn = nullptr;
      } else
        xmlreader->raiseWarning(tr("Surface3D Table Y column not found error"));
      QString zcolname = xmlreader->readAttributeString("zcolumn", &ok);
      if (ok) {
        (table) ? zcolumn = table->column(zcolname) : zcolumn = nullptr;
      } else
        xmlreader->raiseWarning(tr("Surface3D Table Z column not found error"));
      if (table && xcolumn && ycolumn && zcolumn) {
        qDebug() << "table is not a valid data structure for Surface3D";
        loadseries = false;
      }
    } else if (data == "function") {
      bool funcstatus = false;
      bool xlstatus = false;
      bool xustatus = false;
      bool ylstatus = false;
      bool yustatus = false;
      bool zlstatus = false;
      bool zustatus = false;
      bool xpointstatus = false;
      bool ypointstatus = false;

      // function
      QString func = xmlreader->readAttributeString("function", &ok);
      if (ok)
        funcstatus = true;
      else
        xmlreader->raiseWarning(tr("Surface3D function not found error"));

      // xl
      double xl = xmlreader->readAttributeDouble("xl", &ok);
      if (ok)
        xlstatus = true;
      else
        xmlreader->raiseWarning(tr("Surface3D xl not found error"));

      // xu
      double xu = xmlreader->readAttributeDouble("xu", &ok);
      if (ok)
        xustatus = true;
      else
        xmlreader->raiseWarning(tr("Surface3D xu not found error"));

      // yl
      double yl = xmlreader->readAttributeDouble("yl", &ok);
      if (ok)
        ylstatus = true;
      else
        xmlreader->raiseWarning(tr("Surface3D yl not found error"));

      // yu
      double yu = xmlreader->readAttributeDouble("yu", &ok);
      if (ok)
        yustatus = true;
      else
        xmlreader->raiseWarning(tr("Surface3D yu not found error"));

      // zl
      double zl = xmlreader->readAttributeDouble("zl", &ok);
      if (ok)
        zlstatus = true;
      else
        xmlreader->raiseWarning(tr("Surface3D zl not found error"));

      // zu
      double zu = xmlreader->readAttributeDouble("zu", &ok);
      if (ok)
        zustatus = true;
      else
        xmlreader->raiseWarning(tr("Surface3D zu not found error"));

      // xpoints
      int xpoints = xmlreader->readAttributeInt("xpoints", &ok);
      if (ok)
        xpointstatus = true;
      else
        xmlreader->raiseWarning(tr("Surface3D xpoints not found error"));

      // ypoints
      int ypoints = xmlreader->readAttributeInt("ypoints", &ok);
      if (ok)
        ypointstatus = true;
      else
        xmlreader->raiseWarning(tr("Surface3D ypoints not found error"));
      if (funcstatus && xlstatus && xustatus && ylstatus && yustatus &&
          zlstatus && zustatus && xpointstatus && ypointstatus) {
        Graph3DCommon::Function3DData funcdata;
        funcdata.function = func;
        funcdata.xl = xl;
        funcdata.xu = xu;
        funcdata.yl = yl;
        funcdata.yu = yu;
        funcdata.zl = zl;
        funcdata.zu = zu;
        funcdata.xpoints = xpoints;
        funcdata.ypoints = ypoints;
        setfunctiondata(app->generateFunction3ddata(funcdata), funcdata);
        loadseries = true;
      }
    }

    if (loadseries) {
      // visible
      QSurface3DSeries *series = data_.at(counter_)->getdataseries();
      bool vis = xmlreader->readAttributeBool("visible", &ok);
      (ok) ? series->setVisible(vis)
           : xmlreader->raiseWarning(
                 "Surface3D visible series property setting error");

      // flatshade
      bool flatshade = xmlreader->readAttributeDouble("flatshading", &ok);
      (ok) ? series->setFlatShadingEnabled(flatshade)
           : xmlreader->raiseWarning(
                 "Surface3D series flatshading property setting error");

      // draw mode
      QString dmode = xmlreader->readAttributeString("drawmode", &ok);
      if (ok) {
        if (dmode == "surface")
          series->setDrawMode(QSurface3DSeries::DrawFlag::DrawSurface);
        else if (dmode == "wireframe")
          series->setDrawMode(QSurface3DSeries::DrawFlag::DrawWireframe);
        else if (dmode == "surfaceandwireframe")
          series->setDrawMode(
              QSurface3DSeries::DrawFlag::DrawSurfaceAndWireframe);
      } else
        xmlreader->raiseWarning(
            "Surface3D drawmode series property setting error");

      // smooth
      bool smooth = xmlreader->readAttributeBool("meshsmooth", &ok);
      (ok) ? series->setMeshSmooth(smooth)
           : xmlreader->raiseWarning(
                 "Surface3D meshsmooth series property setting error");

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
            "Surface3D colorstyle series property setting error");

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
            "Surface3D gradient color series property setting error");

      // Solid Color
      QString solidcolor = xmlreader->readAttributeString("solidcolor", &ok);
      (ok) ? series->setBaseColor(QColor(solidcolor))
           : xmlreader->raiseWarning(
                 "Surface3D series solid color property setting error");

      // Highlight Color
      QString hcolor = xmlreader->readAttributeString("highlightcolor", &ok);
      (ok) ? series->setSingleHighlightColor(QColor(hcolor))
           : xmlreader->raiseWarning(
                 "Surface3D series highlight color property setting error");
    }
  }
  xmlreader->readNext();
  if (xmlreader->isStartElement() && xmlreader->name() == "plot") {
    loadplot(xmlreader, tabs, mats, app);
    counter_++;
  }
}

Table *Surface3D::getTableByName(QList<Table *> tabs, const QString name) {
  Table *table = nullptr;
  foreach (Table *tab, tabs) {
    if (tab->name() == name) table = tab;
  }
  return table;
}

Matrix *Surface3D::getMatrixByName(QList<Matrix *> mats, const QString name) {
  Matrix *matrix = nullptr;
  foreach (Matrix *mat, mats) {
    if (mat->name() == name) matrix = mat;
  }
  return matrix;
}

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
