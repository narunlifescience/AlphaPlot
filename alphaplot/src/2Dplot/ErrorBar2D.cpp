#include "ErrorBar2D.h"
#include "Axis2D.h"
#include "Bar2D.h"
#include "Curve2D.h"
#include "DataManager2D.h"
#include "LineSpecial2D.h"
#include "Table.h"
#include "core/Utilities.h"
#include "core/column/Column.h"

ErrorBar2D::ErrorBar2D(Table *table, Column *errorcol, int from, int to,
                       Axis2D *xAxis, Axis2D *yAxis,
                       QCPErrorBars::ErrorType errortype,
                       LineSpecial2D *linespecial)
    : QCPErrorBars(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      errortype_(errortype),
      linespecial_(linespecial),
      curve_(nullptr),
      bar_(nullptr),
      errordata_(new DataBlockError(table, errorcol, from, to)) {
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  setErrorType(errortype_);
  setData(errordata_->data());
  setDataPlottable(linespecial_);
  setAntialiased(false);
}

ErrorBar2D::ErrorBar2D(Table *table, Column *errorcol, int from, int to,
                       Axis2D *xAxis, Axis2D *yAxis,
                       QCPErrorBars::ErrorType errortype, Curve2D *curve)
    : QCPErrorBars(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      errortype_(errortype),
      linespecial_(nullptr),
      curve_(curve),
      bar_(nullptr),
      errordata_(new DataBlockError(table, errorcol, from, to)) {
  setAntialiased(false);
  setErrorType(errortype_);
  setData(errordata_->data());
  setDataPlottable(curve_);
}

ErrorBar2D::ErrorBar2D(Table *table, Column *errorcol, int from, int to,
                       Axis2D *xAxis, Axis2D *yAxis,
                       QCPErrorBars::ErrorType errortype, Bar2D *bar)
    : QCPErrorBars(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      errortype_(errortype),
      linespecial_(nullptr),
      curve_(nullptr),
      bar_(bar),
      errordata_(new DataBlockError(table, errorcol, from, to)) {
  setErrorType(errortype_);
  setData(errordata_->data());
  setDataPlottable(bar_);
}

ErrorBar2D::~ErrorBar2D() { delete errordata_; }

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
