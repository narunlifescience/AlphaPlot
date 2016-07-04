#ifndef LAYOUT2D_H
#define LAYOUT2D_H

#include "../MyWidget.h"
#include "AxisRect2D.h"
#include "Plot2D.h"

#include <QHBoxLayout>
#include <QPushButton>

class Column;
class AxisRect2D;
class LayoutGrid2D;

// Button with layout number
class LayoutButton : public QPushButton {
  Q_OBJECT

 public:
  LayoutButton(const QString &text = QString::null, QWidget *parent = 0);
  ~LayoutButton();

  static int btnSize() { return 18; }

 protected:
  void mousePressEvent(QMouseEvent *event);
  void mouseDoubleClickEvent(QMouseEvent *);

 signals:
  void showCurvesDialog();
  void clicked(LayoutButton *);
  void showContextMenu();
};

class Layout2D : public MyWidget {
  Q_OBJECT
 public:
  Layout2D(const QString &label, QWidget *parent = nullptr,
           const QString name = QString(), Qt::WFlags f = 0);
  ~Layout2D();
  bool eventFilter(QObject *object, QEvent *e);
  QCPDataMap *generateDataMap(Column *xData, Column *yData, int from, int to);
  void generateFunction2DPlot(QCPDataMap *dataMap, const double xMin,
                              const double xMax, const double yMin,
                              const double yMax, const QString yLabel);
  void generateLineScatter2DPlot(Column *xData, Column *yData, int from,
                                 int to) const;

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

  struct AxisRectItem {
    QPair<Grid2D *, Grid2D *> grids_;
    QPair<int, int> coordinates_;
    QMap<Axis2D::AxisOreantation, QList<Axis2D *>> axises_;
    QMap<LineScatterType, QList<LineScatter2D *>> lineScatter_;
  };

  AxisRectItem getSelectedAxisRect(int col, int row);
  int getAxisRectIndex(AxisRectItem item);

  void setLayoutDimension(QPair<int, int> dimension);
  void removeAxisRect(int index);
  void removeAxisRect(int col, int row);

  int getLayoutRectGridIndex(QPair<int, int> coord);
  QPair<int, int> getLayoutRectGridCoordinate(int index);
  LayoutButton *addLayoutButton(int num);

 private slots:
  void axisDoubleClicked(QCPAxis *, QCPAxis::SelectablePart);
  AxisRectItem addAxisRectItem();
  void removeAxisRectItem();

 private:
  Plot2D *plot2dCanvas_;

  LayoutGrid2D *layout_;
  AxisRectItem items_;
  QPair<int, int> layoutDimension_;
  QMap<int, AxisRectItem> layoutElementList_;
  QList<LayoutButton *> buttionlist_;

  QHBoxLayout *layoutButtonsBox_;
  QHBoxLayout *toolbuttonsBox_;
  QPushButton *addLayoutButton_;
  QPushButton *removeLayoutButton_;
};

#endif  // LAYOUT2D_H
