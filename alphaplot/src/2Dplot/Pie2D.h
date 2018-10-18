#ifndef PIE2D_H
#define PIE2D_H

#include <QObject>
#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class Column;
class Table;
class LegendItem2D;

class Pie2D : public QCPAbstractItem {
  Q_OBJECT
 public:
  Pie2D(AxisRect2D *axisrect);
  ~Pie2D();

  void setGraphData(Table *table, Column *xData, int from, int to);
  // reimplemented virtual methods:
  double selectTest(const QPointF &pos, bool onlySelectable,
                    QVariant *details = nullptr) const;

  Qt::PenStyle getstrokestyle_pieplot() const;
  QColor getstrokecolor_pieplot() const;
  double getstrokethickness_pieplot() const;
  int getmarginpercent_pieplot() const;

  void setstrokestyle_pieplot(const Qt::PenStyle &style);
  void setstrokecolor_pieplot(const QColor &color);
  void setstrokethickness_pieplot(const double value);
  void setmarginpercent_pieplot(const int value);

 protected:
  void draw(QCPPainter *painter);

 private:
  AxisRect2D *axisrect_;
  QVector<double> *pieData_;
  QVector<QColor> *pieColors_;
  QPen mPen;
  QBrush mBrush;
  int marginpercent_;
};

#endif  // PIE2D_H
