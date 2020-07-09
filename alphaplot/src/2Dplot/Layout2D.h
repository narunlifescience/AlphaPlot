#ifndef LAYOUT2D_H
#define LAYOUT2D_H

#include <QHBoxLayout>
#include <QPushButton>

#include "AxisRect2D.h"
#include "MyWidget.h"
#include "globals.h"

class QLabel;
class Table;
class AxisRect2D;
class LayoutGrid2D;
class ToolButton;
class LayoutButton2D;
class XmlStreamWriter;

class Layout2D : public MyWidget {
  Q_OBJECT

 public:
  Layout2D(const QString &label, QWidget *parent = nullptr,
           const QString name = QString(), Qt::WindowFlags f = Qt::SubWindow);
  ~Layout2D();

  LayoutGrid2D *getLayoutGrid() const { return layout_; }

  StatBox2D::BoxWhiskerData generateBoxWhiskerData(Table *table, Column *data,
                                                   int from, int to, int key);

  void generateFunction2DPlot(QVector<double> *xdata, QVector<double> *ydata,
                              const QString xLabel, const QString yLabel);
  void generateScatterWithXerror2DPlot(Table *table, Column *xData,
                                       Column *yData, Column *xErr, int from,
                                       int to);
  void generateScatterWithYerror2DPlot(Table *table, Column *xData,
                                       Column *yData, Column *yErr, int from,
                                       int to);
  void generateScatterWithXYerror2DPlot(Table *table, Column *xData,
                                        Column *yData, Column *xErr,
                                        Column *yErr, int from, int to);
  QList<StatBox2D *> generateStatBox2DPlot(Table *table,
                                           QList<Column *> ycollist, int from,
                                           int to, int key);
  void generateHistogram2DPlot(const AxisRect2D::BarType &barType,
                               bool multilayout, Table *table,
                               QList<Column *> collist, int from, int to);

  void generateLineSpecial2DPlot(
      const AxisRect2D::LineScatterSpecialType &plotType, Table *table,
      Column *xData, QList<Column *> ycollist, int from, int to);
  void generateLineSpecialChannel2DPlot(Table *table, Column *xData,
                                        QList<Column *> ycollist, int from,
                                        int to);
  void generateCurve2DPlot(const AxisRect2D::LineScatterType &plotType,
                           Table *table, Column *xcol, QList<Column *> ycollist,
                           int from, int to);
  void generateBar2DPlot(const AxisRect2D::BarType &barType, Table *table,
                         Column *xData, QList<Column *> ycollist, int from,
                         int to);
  void generateStakedBar2DPlot(const AxisRect2D::BarType &barType, Table *table,
                               Column *xData, QList<Column *> ycollist,
                               int from, int to);
  void generateGroupedBar2DPlot(const AxisRect2D::BarType &barType,
                                Table *table, Column *xData,
                                QList<Column *> ycollist, int from, int to);
  void generateVector2DPlot(const Vector2D::VectorPlot &vectorplot,
                            Table *table, Column *x1Data, Column *y1Data,
                            Column *x2Data, Column *y2Data, int from, int to);
  void generatePie2DPlot(const Graph2DCommon::PieStyle &style, Table *table,
                         Column *xData, Column *yData, int from, int to);
  void generateColorMap2DPlot(Matrix *matrix, bool greyscale, bool contour);

  QList<AxisRect2D *> getAxisRectList();
  AxisRect2D *getSelectedAxisRect(int col, int row);
  QPair<int, int> getAxisRectRowCol(AxisRect2D *axisRect2d);
  AxisRect2D *getCurrentAxisRect();
  Plot2D *getPlotCanwas() const;
  int getbuttonboxheight() const;

  void setLayoutDimension(QPair<int, int> dimension);
  void removeAxisRect(const QPair<int, int> rowcol);

  int getLayoutRectGridIndex(QPair<int, int> coord);
  QPair<int, int> getLayoutRectGridCoordinate(int index);
  LayoutButton2D *addLayoutButton(const QPair<int, int> rowcol,
                                  AxisRect2D *axisrect);
  void setBackground(const QColor &background);
  void setGraphTool(const Graph2DCommon::Picker &picker);
  void print();
  void save(XmlStreamWriter *xmlwriter, const bool saveastemplate = false);
  bool load(XmlStreamReader *xmlreader, QList<Table *> tabs,
            QList<Matrix *> mats);
  void loadIcons();
  void setLayoutButtonBoxVisible(const bool value);

 public slots:
  bool exportGraph();
  bool exportGraphwithoutdialog(const QString &name,
                                const QString &selected_filter);
  void updateData(Matrix *matrix);
  void updateData(Table *table, const QString &name);
  void removeMatrix(Matrix *matrix);
  void removeColumn(Table *table, const QString &name);
  QList<MyWidget *> dependentTableMatrix();
  void setAxisRangeDrag(bool value);
  void setAxisRangeZoom(bool value);
  AxisRect2D *addAxisRectWithAxis();
  AxisRect2D *addAxisRectWithAxis(
      const Graph2DCommon::AddLayoutElement &position);
  AxisRect2D *addAxisRectWithAxis(const QPair<int, int> rowcol);
  void removeAxisRectItem();

 private slots:
  AxisRect2D *addAxisRectItem(
      const AlphaPlot::ColumnDataType &xcoldatatype,
      const AlphaPlot::ColumnDataType &ycoldatatype,
      const Graph2DCommon::AddLayoutElement &addelement);
  AxisRect2D *addAxisRectItemAtRowCol(
      const AlphaPlot::ColumnDataType &xcoldatatype,
      const AlphaPlot::ColumnDataType &ycoldatatype, QPair<int, int> rowcol);
  void axisRectSetFocus(AxisRect2D *rect);
  void activateLayout(LayoutButton2D *button);
  void showtooltip(QPointF position, double xval, double yval, Axis2D *xaxis,
                   Axis2D *yaxis);

 private:
  Curve2D *generateScatter2DPlot(Table *table, Column *xcol, Column *ycol,
                                 int from, int to);
  void addTextToAxisTicker(Column *col, Axis2D *axis, int from, int to);
  void arrangeLayoutButtons();

 protected:
  void resizeEvent(QResizeEvent *event);

 private:
  QWidget *main_widget_;
  Plot2D *plot2dCanvas_;

  LayoutGrid2D *layout_;
  QPair<int, int> layoutDimension_;
  QList<QPair<LayoutButton2D *, AxisRect2D *>> buttionlist_;

  QHBoxLayout *layoutButtonsBox_;
  QHBoxLayout *layoutManagebuttonsBox_;
  ToolButton *refreshPlotButton_;
  ToolButton *addLayoutButton_;
  ToolButton *removeLayoutButton_;
  QLabel *streachLabel_;
  QMenu *addlayoutmenu_;
  QAction *addLayoutupaction;
  QAction *addLayoutdownaction;
  QAction *addLayoutleftaction;
  QAction *addLayoutrightaction;

  AxisRect2D *currentAxisRect_;
  Graph2DCommon::Picker picker_;
  static const int buttonboxmargin_;
  static const int defaultlayout2dwidth_;
  static const int defaultlayout2dheight_;
  static const int minimumlayout2dwidth_;
  static const int minimumlayout2dheight_;

  QCPItemStraightLine *xpickerline_;
  QCPItemStraightLine *ypickerline_;

 private slots:
  void mouseMoveSignal(QMouseEvent *event);
  void mousePressSignal(QMouseEvent *event);
  void mouseReleaseSignal(QMouseEvent *event);
  void mouseWheel();
  void beforeReplot();
  void refresh();
  void exportPDF(const QString &filename);

 signals:
  void AxisRectCreated(AxisRect2D *, MyWidget *);
  void AxisRectRemoved(MyWidget *);
  void ResetPicker();
  void layout2DResized();
  void datapoint(Curve2D *curve, double xval, double yval);
};

#endif  // LAYOUT2D_H
