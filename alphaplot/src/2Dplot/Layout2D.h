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
class PickerTool2D;
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
                                                   const int from, const int to,
                                                   const int key);

  void generateFunction2DPlot(QVector<double> *xdata, QVector<double> *ydata,
                              const PlotData::FunctionData funcdata);
  void generateScatterWithXerror2DPlot(Table *table, Column *xData,
                                       Column *yData, Column *xErr,
                                       const int from, const int to);
  void generateScatterWithYerror2DPlot(Table *table, Column *xData,
                                       Column *yData, Column *yErr,
                                       const int from, const int to);
  void generateScatterWithXYerror2DPlot(Table *table, Column *xData,
                                        Column *yData, Column *xErr,
                                        Column *yErr, const int from,
                                        const int to);
  QList<StatBox2D *> generateStatBox2DPlot(Table *table,
                                           QList<Column *> ycollist,
                                           const int from, const int to,
                                           const int key);
  void generateHistogram2DPlot(const AxisRect2D::BarType &barType,
                               const bool multilayout, Table *table,
                               QList<Column *> collist, const int from,
                               const int to);

  void generateLineSpecial2DPlot(
      const AxisRect2D::LineScatterSpecialType &plotType, Table *table,
      Column *xData, QList<Column *> ycollist, const int from, const int to);
  void generateLineSpecialChannel2DPlot(Table *table, Column *xData,
                                        QList<Column *> ycollist,
                                        const int from, const int to);
  void generateCurve2DPlot(const AxisRect2D::LineScatterType &plotType,
                           Table *table, Column *xcol, QList<Column *> ycollist,
                           const int from, const int to);
  void generateBar2DPlot(const AxisRect2D::BarType &barType, Table *table,
                         Column *xData, QList<Column *> ycollist,
                         const int from, const int to);
  void generateStakedBar2DPlot(const AxisRect2D::BarType &barType, Table *table,
                               Column *xData, QList<Column *> ycollist,
                               const int from, const int to);
  void generateGroupedBar2DPlot(const AxisRect2D::BarType &barType,
                                Table *table, Column *xData,
                                QList<Column *> ycollist, const int from,
                                const int to);
  void generateVector2DPlot(const Vector2D::VectorPlot &vectorplot,
                            Table *table, Column *x1Data, Column *y1Data,
                            Column *x2Data, Column *y2Data, const int from,
                            const int to);
  void generatePie2DPlot(const Graph2DCommon::PieStyle &style, Table *table,
                         Column *xData, Column *yData, const int from,
                         const int to);
  void generateColorMap2DPlot(Matrix *matrix, const bool greyscale,
                              const bool contour);
  void generateLayoutInset2D();

  QList<AxisRect2D *> getAxisRectList();
  AxisRect2D *getSelectedAxisRect(const int col, const int row);
  QPair<int, int> getAxisRectRowCol(AxisRect2D *axisRect2d);
  AxisRect2D *getCurrentAxisRect();
  Plot2D *getPlotCanwas() const;
  int getbuttonboxheight() const;
  PickerTool2D *getPickerTool() { return picker_; }

  void setLayoutDimension(const QPair<int, int> dimension);
  void removeAxisRect(const QPair<int, int> rowcol);

  int getLayoutRectGridIndex(const QPair<int, int> coord);
  QPair<int, int> getLayoutRectGridCoordinate(const int index);
  LayoutButton2D *addLayoutButton(const QPair<int, int> rowcol,
                                  AxisRect2D *axisrect);
  void setBackground(const QColor &background);
  void setGraphTool(const Graph2DCommon::Picker &picker);
  void streachLabelSetText(const QString &text);
  void print();
  void save(XmlStreamWriter *xmlwriter, const bool saveastemplate = false);
  bool load(XmlStreamReader *xmlreader, QList<Table *> tabs,
            QList<Matrix *> mats, bool setname = true);
  void loadIcons() const;
  void setLayoutButtonBoxVisible(const bool value);
  void copy(Layout2D *layout, QList<Table *> tables, QList<Matrix *> matrixs);

 public slots:
  bool exportGraph();
  bool exportGraphwithoutdialog(const QString &name,
                                const QString &selected_filter);
  void updateData(Matrix *matrix);
  void updateData(Table *table, const QString &name);
  void removeMatrix(Matrix *matrix);
  void removeColumn(Table *table, const QString &name);
  QList<MyWidget *> dependentTableMatrix();
  void setAxisRangeDrag(const bool value);
  void setAxisRangeZoom(const bool value);
  AxisRect2D *addAxisRectWithAxis();
  AxisRect2D *addAxisRectWithAxis(
      const Graph2DCommon::AddLayoutElement &position);
  AxisRect2D *addAxisRectWithAxis(const QPair<int, int> rowcol);
  void swapAxisRect(AxisRect2D *axisrect1, AxisRect2D *axisrect2);
  void removeAxisRectItem();
  void refresh() const;
  void copyToClipbord();
  void hideCurrentAxisRectIndicator(const bool status);
  void axisRectSetFocus(AxisRect2D *rect);

 private slots:
  AxisRect2D *addAxisRectItem(
      const AlphaPlot::ColumnDataType &xcoldatatype,
      const AlphaPlot::ColumnDataType &ycoldatatype,
      const Graph2DCommon::AddLayoutElement &addelement);
  AxisRect2D *addAxisRectItemAtRowCol(
      const AlphaPlot::ColumnDataType &xcoldatatype,
      const AlphaPlot::ColumnDataType &ycoldatatype,
      const QPair<int, int> rowcol);
  void activateLayout(LayoutButton2D *button);

 private:
  Curve2D *generateScatter2DPlot(Table *table, Column *xcol, Column *ycol,
                                 const int from, const int to);
  void addTextToAxisTicker(Column *col, Axis2D *axis, const int from,
                           const int to);
  void arrangeLayoutButtons();

 protected:
  void resizeEvent(QResizeEvent *event);

 private:
  PickerTool2D *picker_;
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
  static const int buttonboxmargin_;
  static const int defaultlayout2dwidth_;
  static const int defaultlayout2dheight_;
  static const int minimumlayout2dwidth_;
  static const int minimumlayout2dheight_;

 private slots:
  void mouseMoveSignal(QMouseEvent *event);
  void mousePressSignal(QMouseEvent *event);
  void mouseReleaseSignal(QMouseEvent *event);
  void mouseWheel();
  void beforeReplot();
  void exportPDF(const QString &filename);

 signals:
  void AxisRectCreated(AxisRect2D *, MyWidget *);
  void AxisRectRemoved(MyWidget *);
  void AxisRectSwap(AxisRect2D *, AxisRect2D *);
  void ResetPicker();
  void layout2DResized();
  void datapoint(Curve2D *curve, const double xval, const double yval);
  void showContextMenu();
};

#endif  // LAYOUT2D_H
