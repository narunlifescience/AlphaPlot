#ifndef LAYOUT2D_H
#define LAYOUT2D_H

#include <QHBoxLayout>
#include <QPushButton>

#include "AxisRect2D.h"
#include "MyWidget.h"
#include "Plot2D.h"
#include "StatBox2D.h"
#include "future/core/column/Column.h"

class QLabel;
class Table;
class AxisRect2D;
class LayoutGrid2D;
class LayoutButton2D;
class XmlStreamWriter;

class Layout2D : public MyWidget {
  Q_OBJECT

 public:
  Layout2D(const QString &label, QWidget *parent = nullptr,
           const QString name = QString(), Qt::WindowFlags f = Qt::SubWindow);
  ~Layout2D();

  StatBox2D::BoxWhiskerData generateBoxWhiskerData(Table *table, Column *data,
                                                   int from, int to, int key);

  void generateFunction2DPlot(QVector<double> *xdata, QVector<double> *ydata,
                              const QString xLabel, const QString yLabel);
  void generateScatter2DPlot(Column *xData, Column *yData, int from, int to);
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
  void generateVector2DPlot(const Vector2D::VectorPlot &vectorplot,
                            Table *table, Column *x1Data, Column *y1Data,
                            Column *x2Data, Column *y2Data, int from, int to);
  void generatePie2DPlot(const Graph2DCommon::PieStyle &style, Table *table,
                         Column *xData, Column *yData, int from, int to);
  void generateColorMap2DPlot(Matrix *matrix, bool greyscale, bool contour);

  QList<AxisRect2D *> getAxisRectList();
  AxisRect2D *getSelectedAxisRect(int col, int row);
  int getAxisRectIndex(AxisRect2D *axisRect2d);
  AxisRect2D *getCurrentAxisRect();
  Plot2D *getPlotCanwas() const;
  int getbuttonboxheight() const {
    return addLayoutButton_->height() + (buttonboxmargin_ * 2);
  }

  void setLayoutDimension(QPair<int, int> dimension);
  void removeAxisRect(int index);

  int getLayoutRectGridIndex(QPair<int, int> coord);
  QPair<int, int> getLayoutRectGridCoordinate(int index);
  LayoutButton2D *addLayoutButton(int num);
  void setBackground(const QColor &background);
  void setGraphTool(const Graph2DCommon::Picker &picker);
  void print();
  void save(XmlStreamWriter *xmlwriter);
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
  QStringList dependentTableMatrixNames();
  void setAxisRangeDrag(bool value);
  void setAxisRangeZoom(bool value);

 private slots:
  AxisRect2D *addAxisRectWithAxis();
  AxisRect2D *addAxisRectItem(const AlphaPlot::ColumnDataType &xcoldatatype,
                              const AlphaPlot::ColumnDataType &ycoldatatype);
  void removeAxisRectItem();
  void axisRectSetFocus(AxisRect2D *rect);
  void activateLayout(LayoutButton2D *button);
  void showtooltip(QPointF position, double xval, double yval, Axis2D *xaxis,
                   Axis2D *yaxis);

 private:
  void addTextToAxisTicker(Column *col, Axis2D *axis, int from, int to);

 protected:
  void resizeEvent(QResizeEvent *event);

 private:
  QWidget *main_widget_;
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
