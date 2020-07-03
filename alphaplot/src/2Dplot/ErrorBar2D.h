#ifndef ERRORBAR2D_H
#define ERRORBAR2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"

class Table;
class Column;
class Axis2D;
class Curve2D;
class LineSpecial2D;
class Bar2D;
class DataBlockError;
class XmlStreamWriter;
class XmlStreamReader;

class ErrorBar2D : public QCPErrorBars {
 public:
  ErrorBar2D(Table *table, Column *errorcol, int from, int to, Axis2D *xAxis,
             Axis2D *yAxis, QCPErrorBars::ErrorType errortype,
             LineSpecial2D *linespecial);
  ErrorBar2D(Table *table, Column *errorcol, int from, int to, Axis2D *xAxis,
             Axis2D *yAxis, QCPErrorBars::ErrorType errortype, Curve2D *curve);
  ErrorBar2D(Table *table, Column *errorcol, int from, int to, Axis2D *xAxis,
             Axis2D *yAxis, QCPErrorBars::ErrorType errortype, Bar2D *bar);
  ~ErrorBar2D();

  void setErrorData(Table *table, Column *errorcol, int from, int to);
  bool getfillstatus_errorbar() const;
  QCPErrorBars::ErrorType geterrortype_errorbar() { return errortype_; }
  LineSpecial2D *getlinespecial2d_errorbar() { return linespecial_; }
  Curve2D *getcurve2d_errorbar() { return curve_; }
  Bar2D *getbar2d_errorbar() { return bar_; }
  DataBlockError *getdatablock_error() { return errordata_; }
  void setfillstatus_errorbar(const bool status);

  void save(XmlStreamWriter *xmlwriter);
  bool load(XmlStreamReader *xmlreader);

 public:
  Axis2D *xAxis_;
  Axis2D *yAxis_;
  QCPErrorBars::ErrorType errortype_;
  LineSpecial2D *linespecial_;
  Curve2D *curve_;
  Bar2D *bar_;
  DataBlockError *errordata_;
};

#endif  // ERRORBAR2D_H
