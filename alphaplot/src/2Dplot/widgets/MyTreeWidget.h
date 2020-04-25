#ifndef MYTREEWIDGET_H
#define MYTREEWIDGET_H

#include <QMenu>
#include <QMouseEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "2Dplot/AxisRect2D.h"

class AxisRect2D;
class QCustomPlot;

class MyTreeWidget : public QTreeWidget {
  Q_OBJECT

 public:
  enum class PropertyItemType : int {
    PlotCanvas = 0,
    Layout = 1,
    Grid = 2,
    Axis = 3,
    Legend = 4,
    TextItem = 5,
    LineItem = 6,
    ImageItem = 7,
    LSGraph = 8,
    ChannelGraph = 9,
    Curve = 10,
    StatBox = 11,
    Vector = 12,
    BarGraph = 13,
    PieGraph = 14,
    ColorMap = 15,
    ErrorBar = 16,
    Plot3DCanvas = 17,
    Plot3DAxisValue = 18,
    Plot3DAxisCatagory = 19,
    Plot3DSurface = 20,
    Plot3DBar = 21,
    Plot3DScatter = 22,
  };
  MyTreeWidget(QWidget *parent = nullptr);
  ~MyTreeWidget();
 signals:
  void adderrorbar();
 private slots:
  void CurrentItemChanged(QTreeWidgetItem *current);
  void showContextMenu(const QPoint &pos);
  void addfunctionplot();
  void addplot();
  void addAxis2D();
  void cloneAxis2D();
  void removeAxis2D();
  void removeLineSpecial2D();
  void removeLineSpecialChannel2D();
  void removeCurve2D();
  void removeBar2D();
  void removeVector2D();
  void removeStatBox2D();
  void removeErrorBar2D();
  void removeTextItem2D();
  void removeLineItem2D();
  void removeImageItem2D();

 private:
  template <class T>
  void moveplottablelayer(QAction *action,
                          const QCustomPlot::LayerInsertMode &mode);
  template <class T>
  void moveitemlayer(QAction *action, const QCustomPlot::LayerInsertMode &mode);

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
  QAction *removechannel_;
  QAction *removecurve_;
  QAction *removebar_;
  QAction *removevector_;
  QAction *removestatbox_;
  QAction *removeerrorbar_;
  QAction *removetextitem_;
  QAction *removelineitem_;
  QAction *removeimageitem_;
  //  clone axis
  QAction *clonetotopaxis_;
  QAction *clonetobottomaxis_;
  QAction *clonetoleftaxis_;
  QAction *clonetorightaxis_;
  // Errorbar
  QAction *adderrorbar_;
  // move layer up
  QAction *moveupls_;
  QAction *moveupchannel_;
  QAction *moveupcurve_;
  QAction *moveupbar_;
  QAction *moveupvector_;
  QAction *moveupstatbox_;
  QAction *moveuppie_;
  QAction *moveupcolormap_;
  QAction *moveuptextitem_;
  QAction *moveuplineitem_;
  QAction *moveupimageitem_;
  // move layer down
  QAction *movedownls_;
  QAction *movedownchannel_;
  QAction *movedowncurve_;
  QAction *movedownbar_;
  QAction *movedownvector_;
  QAction *movedownstatbox_;
  QAction *movedownpie_;
  QAction *movedowncolormap_;
  QAction *movedowntextitem_;
  QAction *movedownlineitem_;
  QAction *movedownimageitem_;
};

#endif  // MYTREEWIDGET_H
