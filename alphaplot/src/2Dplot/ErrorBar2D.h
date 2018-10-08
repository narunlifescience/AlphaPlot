#ifndef ERRORBAR2D_H
#define ERRORBAR2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Table;
class Column;
class Curve2D;
class LineSpecial2D;
class Bar2D;
class DataBlockError;

class ErrorBar2D : QCPErrorBars {
 public:
  ErrorBar2D(Table *table, Column *errorcol, int from, int to, Axis2D *xAxis,
             Axis2D *yAxis, QCPErrorBars::ErrorType errortype,
             LineSpecial2D *linescatter);
  ErrorBar2D(Table *table, Column *errorcol, int from, int to, Axis2D *xAxis,
             Axis2D *yAxis, QCPErrorBars::ErrorType errortype, Curve2D *curve);
  ErrorBar2D(Table *table, Column *errorcol, int from, int to, Axis2D *xAxis,
             Axis2D *yAxis, QCPErrorBars::ErrorType errortype, Bar2D *bar);
  ~ErrorBar2D();

 public:
  Axis2D *xAxis_;
  Axis2D *yAxis_;
  QCPErrorBars::ErrorType errortype_;
  LineSpecial2D *linescatter_;
  Curve2D *curve_;
  Bar2D *bar_;
  DataBlockError *errordata_;
};

#endif  // ERRORBAR2D_H
