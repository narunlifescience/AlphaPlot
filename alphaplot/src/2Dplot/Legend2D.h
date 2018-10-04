#ifndef LEGEND2D_H
#define LEGEND2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"

class LineScatter2D;

class Legend2D : public QCPLegend {
  Q_OBJECT
 public:
  Legend2D();
  ~Legend2D();

  QColor getborderstrokecolor_legend() const;
  double getborderstrokethickness_legend() const;
  Qt::PenStyle getborderstrokestyle_legend() const;

  void setborderstrokecolor_legend(const QColor &color);
  void setborderstrokethickness_legend(const double value);
  void setborderstrokestyle_legend(const Qt::PenStyle &style);

 signals:
  void legendClicked();

 protected:
  void mousePressEvent(QMouseEvent *);
};

class LegendItem2D : public QCPPlottableLegendItem {
  Q_OBJECT
 public:
  explicit LegendItem2D(Legend2D *legend, QCPAbstractPlottable *plottable);
  ~LegendItem2D();

 signals:
  void legendItemClicked();

 protected:
  void mousePressEvent(QMouseEvent *);
};

#endif  // LEGEND2D_H
