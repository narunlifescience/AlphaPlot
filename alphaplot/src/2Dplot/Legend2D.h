#ifndef LEGEND2D_H
#define LEGEND2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "AxisRect2D.h"

class LineSpecial2D;

class Legend2D : public QCPLegend {
  Q_OBJECT
 public:
  Legend2D(AxisRect2D *axisrect);
  ~Legend2D();

  bool gethidden_legend() const;
  QColor getborderstrokecolor_legend() const;
  double getborderstrokethickness_legend() const;
  Qt::PenStyle getborderstrokestyle_legend() const;

  void sethidden_legend(const bool status);
  void setborderstrokecolor_legend(const QColor &color);
  void setborderstrokethickness_legend(const double value);
  void setborderstrokestyle_legend(const Qt::PenStyle &style);

 signals:
  void legendClicked();

 protected:
  void mousePressEvent(QMouseEvent *event, const QVariant &details);
  void mouseMoveEvent(QMouseEvent *event, const QPointF &startPos);
  void mouseReleaseEvent(QMouseEvent *event, const QPointF &startPos);

 private:
  AxisRect2D *axisrect_;
  bool draggingLegend_;
  QPointF dragLegendOrigin_;
};

class LegendItem2D : public QCPPlottableLegendItem {
  Q_OBJECT
 public:
  explicit LegendItem2D(Legend2D *legend, QCPAbstractPlottable *plottable);
  ~LegendItem2D();

 signals:
  void legendItemClicked();

protected:
 void mousePressEvent(QMouseEvent *event, const QVariant &details);
};

#endif  // LEGEND2D_H
