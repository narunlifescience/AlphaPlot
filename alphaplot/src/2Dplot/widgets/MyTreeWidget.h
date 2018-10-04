#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H

#include <QMenu>
#include <QMouseEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class AxisRect2D;

class MyTreeWidget : public QTreeWidget {
  Q_OBJECT;

 public:
  enum class PropertyItemType : int {
    Layout = 0,
    Grid = 1,
    Axis = 2,
    Legend =3,
    TextItem = 4,
    LSGraph = 5,
    Curve = 6,
    Spline = 7,
    StatBox = 8,
    Vector = 9,
    BarGraph = 10,
  };
  MyTreeWidget(QWidget *parent = nullptr);
  ~MyTreeWidget() {}

 private slots:
  void showContextMenu(const QPoint &pos);
  void addfunctiongraph();
  void addplot();
  void addaxis();
  void removeaxis();
  void removels();
  void removecurve();
  void removespline();
  void removebar();
  // void removevector();
  // void removepie();

 private:
  QWidget *widget_;
  QAction *addgraph_;
  QAction *addfunctiongraph_;
  QAction *addaxis_;
  QAction *removeaxis_;
  QAction *removels_;
  QAction *removespline_;
  QAction *removecurve_;
  QAction *removebar_;
  QAction *removevector_;
  QAction *removepie_;
};

#endif  // MYTREEWIDGET_H
