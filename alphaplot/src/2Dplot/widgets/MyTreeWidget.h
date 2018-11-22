#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H

#include <QMenu>
#include <QMouseEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>

class AxisRect2D;

class MyTreeWidget : public QTreeWidget {
  Q_OBJECT

 public:
  enum class PropertyItemType : int {
    Layout = 0,
    Grid = 1,
    Axis = 2,
    Legend = 3,
    TextItem = 4,
    LineItem = 5,
    ImageItem = 6,
    LSGraph = 7,
    Curve = 8,
    StatBox = 9,
    Vector = 10,
    BarGraph = 11,
    PieGraph = 12,
    ColorMap = 13,
  };
  MyTreeWidget(QWidget *parent = nullptr);
  ~MyTreeWidget();

 private slots:
  void CurrentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);
  void showContextMenu(const QPoint &pos);
  void addfunctionplot();
  void addplot();
  void addAxis2D();
  void removeAxis2D();
  void removeLineSpecial2D();
  void removeCurve2D();
  void removeBar2D();
  void removeVector2D();
  void removeStatBox2D();
  void removeTextItem2D();
  void removeLineItem2D();
  void removeImageItem2D();

 private:
  QWidget *widget_;
  QAction *addgraph_;
  QAction *addfunctionplot_;
  QAction *leftvalueaxis_;
  QAction *leftlogaxis_;
  QAction *leftpiaxis_;
  QAction *lefttextaxis_;
  QAction *lefttimeaxis_;
  QAction *leftdatetimeaxis_;
  QAction *bottomvalueaxis_;
  QAction *bottomlogaxis_;
  QAction *bottompiaxis_;
  QAction *bottomtextaxis_;
  QAction *bottomtimeaxis_;
  QAction *bottomdatetimeaxis_;
  QAction *rightvalueaxis_;
  QAction *rightlogaxis_;
  QAction *rightpiaxis_;
  QAction *righttextaxis_;
  QAction *righttimeaxis_;
  QAction *rightdatetimeaxis_;
  QAction *topvalueaxis_;
  QAction *toplogaxis_;
  QAction *toppiaxis_;
  QAction *toptextaxis_;
  QAction *toptimeaxis_;
  QAction *topdatetimeaxis_;
  QAction *removeaxis_;
  QAction *removels_;
  QAction *removecurve_;
  QAction *removebar_;
  QAction *removevector_;
  QAction *removestatbox_;
  QAction *removetextitem_;
  QAction *removelineitem_;
  QAction *removeimageitem_;
};

#endif  // MYTREEWIDGET_H
