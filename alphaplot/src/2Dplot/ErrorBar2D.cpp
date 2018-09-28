#include "ErrorBar2D.h"
#include "Axis2D.h"
#include "Bar2D.h"
#include "Curve2D.h"
#include "DataManager2D.h"
#include "LineScatter2D.h"
#include "Table.h"
#include "core/Utilities.h"
#include "core/column/Column.h"

ErrorBar2D::ErrorBar2D(Table *table, Column *errorcol, int from, int to,
                       Axis2D *xAxis, Axis2D *yAxis,
                       QCPErrorBars::ErrorType errortype,
                       LineScatter2D *linescatter)
    : QCPErrorBars(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      errortype_(errortype),
      linescatter_(linescatter),
      errordata_(new DataBlockError(table, errorcol, from, to)) {
  setErrorType(errortype_);
  setData(errordata_->data());
  setDataPlottable(linescatter_);
  setAntialiased(false);
}

ErrorBar2D::ErrorBar2D(Table *table, Column *errorcol, int from, int to,
                       Axis2D *xAxis, Axis2D *yAxis,
                       QCPErrorBars::ErrorType errortype, Curve2D *curve)
    : QCPErrorBars(xAxis, yAxis),
      xAxis_(xAxis),
      yAxis_(yAxis),
      errortype_(errortype),
      curve_(curve),
      errordata_(new DataBlockError(table, errorcol, from, to)) {
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
      bar_(bar),
      errordata_(new DataBlockError(table, errorcol, from, to)) {
  setErrorType(errortype_);
  setData(errordata_->data());
  setDataPlottable(bar_);
}

ErrorBar2D::~ErrorBar2D() { delete errordata_; }
