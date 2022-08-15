#include "ErrorBar2D.h"

#include "Axis2D.h"
#include "Bar2D.h"
#include "Curve2D.h"
#include "DataManager2D.h"
#include "LineSpecial2D.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "core/Utilities.h"
#include "core/column/Column.h"
#include "future/lib/XmlStreamReader.h"
#include "future/lib/XmlStreamWriter.h"

ErrorBar2D::ErrorBar2D(Table *table, Column *errorcol, int from, int to,
                       Axis2D *xAxis, Axis2D *yAxis,
                       QCPErrorBars::ErrorType errortype,
                       LineSpecial2D *linespecial)
    : QCPErrorBars(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      errortype_(errortype),
      gtype_(GraphType::LineSpecial),
      linespecial_(linespecial),
      curve_(nullptr),
      bar_(nullptr),
      errordata_(new DataBlockError(table, errorcol, from, to)) {
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setErrorType(errortype_);
  setData(errordata_->data());
  setDataPlottable(linespecial_);
  setAntialiased(false);
  setAntialiasedFill(false);
  setAntialiasedScatters(false);
}

ErrorBar2D::ErrorBar2D(Table *table, Column *errorcol, int from, int to,
                       Axis2D *xAxis, Axis2D *yAxis,
                       QCPErrorBars::ErrorType errortype, Curve2D *curve)
    : QCPErrorBars(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      errortype_(errortype),
      gtype_(GraphType::Curve),
      linespecial_(nullptr),
      curve_(curve),
      bar_(nullptr),
      errordata_(new DataBlockError(table, errorcol, from, to)) {
  setAntialiased(false);
  setErrorType(errortype_);
  setData(errordata_->data());
  setDataPlottable(curve_);
  setAntialiased(false);
  setAntialiasedFill(false);
  setAntialiasedScatters(false);
}

ErrorBar2D::ErrorBar2D(Table *table, Column *errorcol, int from, int to,
                       Axis2D *xAxis, Axis2D *yAxis,
                       QCPErrorBars::ErrorType errortype, Bar2D *bar)
    : QCPErrorBars(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      errortype_(errortype),
      gtype_(GraphType::Bar),
      linespecial_(nullptr),
      curve_(nullptr),
      bar_(bar),
      errordata_(new DataBlockError(table, errorcol, from, to)) {
  setErrorType(errortype_);
  setData(errordata_->data());
  setDataPlottable(bar_);
  setAntialiased(false);
  setAntialiasedFill(false);
  setAntialiasedScatters(false);
}

ErrorBar2D::~ErrorBar2D() { delete errordata_; }

QString ErrorBar2D::getItemName() {
  return errordata_->gettable()->name() + "_" +
         errordata_->geterrorcolumn()->name() + "[" +
         QString::number(errordata_->getfrom() + 1) + ":" +
         QString::number(errordata_->getto() + 1) + "]";
}

QIcon ErrorBar2D::getItemIcon() {
  if (errortype_ == QCPErrorBars::ErrorType::etKeyError)
    return IconLoader::load("graph-x-error", IconLoader::LightDark);
  else
    return IconLoader::load("graph-y-error", IconLoader::LightDark);
}

QString ErrorBar2D::getItemTooltip() {
  QString tooltip = Utilities::getTooltipText(Utilities::TooltipType::x);
  tooltip = tooltip.arg(errordata_->gettable()->name(),
                        errordata_->geterrorcolumn()->name(),
                        QString::number(errordata_->getfrom() + 1),
                        QString::number(errordata_->getto() + 1));
  return tooltip;
}

void ErrorBar2D::setErrorData(Table *table, Column *errorcol, int from,
                              int to) {
  errordata_->regenerateDataBlock(table, errorcol, from, to);
  setData(errordata_->data());
}

bool ErrorBar2D::getfillstatus_errorbar() const {
  if (brush().style() == Qt::NoBrush) {
    return false;
  } else {
    return true;
  }
}

void ErrorBar2D::setfillstatus_errorbar(const bool status) {
  if (status) {
    QBrush b = brush();
    b.setStyle(Qt::SolidPattern);
    setBrush(b);
  } else {
    QBrush b = brush();
    b.setStyle(Qt::NoBrush);
    setBrush(b);
  }
}

void ErrorBar2D::save(XmlStreamWriter *xmlwriter) {
  xmlwriter->writeStartElement("errorbar");
  switch (errortype_) {
    case QCPErrorBars::ErrorType::etKeyError:
      xmlwriter->writeAttribute("type", "x");
      break;
    case QCPErrorBars::ErrorType::etValueError:
      xmlwriter->writeAttribute("type", "y");
      break;
  }

  // data
  xmlwriter->writeAttribute("table", errordata_->gettable()->name());
  xmlwriter->writeAttribute("errcolumn", errordata_->geterrorcolumn()->name());
  xmlwriter->writeAttribute("from", QString::number(errordata_->getfrom()));
  xmlwriter->writeAttribute("to", QString::number(errordata_->getto()));

  xmlwriter->writeAttribute("whiskerwidth", QString::number(whiskerWidth()));
  xmlwriter->writeAttribute("symbolgap", QString::number(symbolGap()));
  (getfillstatus_errorbar()) ? xmlwriter->writeAttribute("fill", "true")
                             : xmlwriter->writeAttribute("fill", "false");
  (antialiasedFill()) ? xmlwriter->writeAttribute("antialiasfill", "true")
                      : xmlwriter->writeAttribute("antialiasfill", "false");
  xmlwriter->writePen(pen());
  xmlwriter->writeBrush(brush());
  xmlwriter->writeEndElement();
}

bool ErrorBar2D::load(XmlStreamReader *xmlreader) {
  if (xmlreader->isStartElement() && xmlreader->name() == "errorbar") {
    bool ok;

    // whiskerwidth property
    int whiskerwdth = xmlreader->readAttributeInt("whiskerwidth", &ok);
    (ok) ? setWhiskerWidth(whiskerwdth)
         : xmlreader->raiseWarning(
               tr("ErrorBar2D whiskerwidth property setting error"));

    // symbolgap property
    int symbolgp = xmlreader->readAttributeInt("symbolgap", &ok);
    (ok) ? setSymbolGap(symbolgp)
         : xmlreader->raiseWarning(
               tr("ErrorBar2D symbolgap property setting error"));

    // fill status property
    bool fill = xmlreader->readAttributeBool("fill", &ok);
    (ok) ? setfillstatus_errorbar(fill)
         : xmlreader->raiseWarning(
               tr("ErrorBar2D fill status property setting error"));

    // antialiasfill status property
    bool antialiasf = xmlreader->readAttributeBool("antialiasfill", &ok);
    (ok) ? setAntialiasedFill(antialiasf)
         : xmlreader->raiseWarning(
               tr("ErrorBar2D antialias fill property setting error"));

    // strokepen property
    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "pen") break;
      // pen
      if (xmlreader->isStartElement() && xmlreader->name() == "pen") {
        QPen strokep = xmlreader->readPen(&ok);
        if (ok)
          setPen(strokep);
        else
          xmlreader->raiseWarning(
              tr("ErrorBar2D strokepen property setting error"));
      }
    }

    // strokepen property
    while (!xmlreader->atEnd()) {
      xmlreader->readNext();
      if (xmlreader->isEndElement() && xmlreader->name() == "brush") break;
      // pen
      if (xmlreader->isStartElement() && xmlreader->name() == "brush") {
        QBrush b = xmlreader->readBrush(&ok);
        if (ok)
          setBrush(b);
        else
          xmlreader->raiseWarning(
              tr("ErrorBar2D brush property setting error"));
      }
    }

  } else  // no element
    xmlreader->raiseError(tr("no ErrorBar2D item element found"));

  return !xmlreader->hasError();
}
