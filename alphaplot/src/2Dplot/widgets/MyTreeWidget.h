#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H

#include <QMenu>
#include <QMouseEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class MyTreeWidget : public QTreeWidget {
  Q_OBJECT;

 public:
  enum class PropertyItemType : int {
    Layout = 0,
    Grid = 1,
    Axis = 2,
    LSGraph = 3,
    Spline =4,
    Vector = 5,
    BarGraph = 6,
  };
  MyTreeWidget(QWidget* parent = nullptr);
  ~MyTreeWidget() {}

 private slots:
  void showContextMenu(const QPoint& pos);
  void addfunctiongraph();
  void addaxis();
  void removeaxis();
  void removels();

private:
  QAction *addgraph_;
  QAction *addfunctiongraph_;
  QAction *addaxis_;
  QAction *removeaxis_;
  QAction *removels_;

};

#endif  // MYTREEWIDGET_H
