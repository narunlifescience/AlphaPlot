#ifndef LINEITEM2D_H
#define LINEITEM2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "AxisRect2D.h"
#include "Plot2D.h"

class LineItem2D : public QCPItemLine {
 public:
  LineItem2D(AxisRect2D *axisrect, Plot2D *plot);
  ~LineItem2D();

  QColor getstrokecolor_lineitem() const;
  double getstrokethickness_lineitem() const;
  Qt::PenStyle getstrokestyle_lineitem() const;

  void setstrokecolor_lineitem(const QColor &color);
  void setstrokethickness_lineitem(const double value);
  void setstrokestyle_lineitem(const Qt::PenStyle &style);

 protected:
  void draw(QCPPainter *painter);
  void mousePressEvent(QMouseEvent *event, const QVariant &details);
  void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos);
  void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos);

 private:
  static const int selectionpixelsize_;
  AxisRect2D *axisrect_;
  bool draggingendlineitem_;
  bool draggingstartlineitem_;
  bool lineitemclicked_;
};

#endif  // LINEITEM2D_H
