#ifndef LAYOUT2D_H
#define LAYOUT2D_H

#include "../MyWidget.h"
#include "AxisRect2D.h"
#include "Plot2D.h"

#include <QHBoxLayout>
#include <QPushButton>

class QLabel;
class Column;
class AxisRect2D;
class LayoutGrid2D;
class LayoutButton2D;

class Layout2D : public MyWidget {
  Q_OBJECT

 public:
  Layout2D(const QString &label, QWidget *parent = nullptr,
           const QString name = QString(), Qt::WFlags f = 0);
  ~Layout2D();

  QCPDataMap *generateDataMap(Column *xData, Column *yData, int from, int to);
  void generateFunction2DPlot(QCPDataMap *dataMap, const QString xLabel,
                              const QString yLabel);
  void generateScatter2DPlot(Column *xData, Column *yData, int from, int to);

  enum LineScatterType {
    Line2D,
    Scatter2D,
    LineAndScatter2D,
    VerticalDropLine2D,
    Spline2D,
    CentralStepAndScatter2D,
    HorizontalStep2D,
    VerticalStep2D,
  };

  void generateLineScatter2DPlot(const LineScatterType &plotType, Column *xData,
                                 Column *yData, int from, int to);

  AxisRect2D *getSelectedAxisRect(int col, int row);
  int getAxisRectIndex(AxisRect2D *axisRect2d);

  void setLayoutDimension(QPair<int, int> dimension);
  void removeAxisRect(int index);

  int getLayoutRectGridIndex(QPair<int, int> coord);
  QPair<int, int> getLayoutRectGridCoordinate(int index);
  LayoutButton2D *addLayoutButton(int num);
  void setBackground(const QColor &background);

protected:
  void paintEvent(QPaintEvent *);

 private slots:
  void axisDoubleClicked(QCPAxis *axis, QCPAxis::SelectablePart part);
  AxisRect2D *addAxisRectItem();
  void removeAxisRectItem();
  void axisRectSetFocus(AxisRect2D *rect);
  void activateLayout(LayoutButton2D *button);

 private:
  Plot2D *plot2dCanvas_;

  LayoutGrid2D *layout_;
  QPair<int, int> layoutDimension_;
  QList<LayoutButton2D *> buttionlist_;

  QHBoxLayout *layoutButtonsBox_;
  QHBoxLayout *layoutManagebuttonsBox_;
  QPushButton *addLayoutButton_;
  QPushButton *removeLayoutButton_;
  QLabel *streachLabel_;

  AxisRect2D *currentAxisRect_;
  bool draggingLegend;
  QPointF dragLegendOrigin;

 private slots:
  void mouseMoveSignal(QMouseEvent *event);
  void mousePressSignal(QMouseEvent *event);
  void mouseReleaseSignal(QMouseEvent *);
  void mouseWheel();
  void beforeReplot();
  void legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item);
};

#endif  // LAYOUT2D_H
