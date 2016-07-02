#ifndef LAYOUT2D_H
#define LAYOUT2D_H

#include "../MyWidget.h"
#include "Plot2D.h"

class Column;

class Layout2D : public MyWidget {
  Q_OBJECT
 public:
  Layout2D(const QString &label, QWidget *parent = nullptr,
           const QString name = QString(), Qt::WFlags f = 0);
  ~Layout2D();
  bool eventFilter(QObject *object, QEvent *e);
  QCPDataMap *generateDataMap(Column *xData, Column *yData);
  void generateFunction2DPlot(QCPDataMap *dataMap, const double xMin,
                              const double xMax, const double yMin,
                              const double yMax, const QString yLabel) const;

 private slots:
  void axisDoubleClicked(QCPAxis *, QCPAxis::SelectablePart);

 private:
  Plot2D *plot2dCanvas_;
};
#endif  // LAYOUT2D_H
