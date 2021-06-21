#ifndef VECTOR2D_H
#define VECTOR2D_H
#include "../3rdparty/qcustomplot/qcustomplot.h"

class Axis2D;
class Column;
class Table;
class XmlStreamReader;
class XmlStreamWriter;

class Vector2D : public QCPGraph {
  Q_OBJECT
 public:
  enum class VectorPlot : int { XYXY = 0, XYAM = 1 };
  Vector2D(const VectorPlot &vectorplot, Table *table, Column *x1Data,
           Column *y1Data, Column *x2Data, Column *y2Data, int from, int to,
           Axis2D *xAxis, Axis2D *yAxis);
  ~Vector2D();

  enum class Position : int { Tail = 0, Middle = 1, Head = 2 };
  enum class LineEnd : int {
    None = 0,
    FlatArrow = 1,
    SpikeArrow = 2,
    LineArrow = 3,
    Disc = 4,
    Square = 5,
    Diamond = 6,
    Bar = 7,
    HalfBar = 8,
    SkewedBar = 9,
  };
  enum class LineEndLocation : int {
    Tail = 0,
    Head = 1,
  };
  void setGraphData(Table *table, Column *x1Data, Column *y1Data,
                    Column *x2Data, Column *y2Data, int from, int to);
  void drawLine(double x1, double y1, double x2, double y2);

  // Getters
  Axis2D *getxaxis();
  Axis2D *getyaxis();
  QColor getlinestrokecolor_vecplot() const;
  Qt::PenStyle getlinestrokestyle_vecplot() const;
  double getlinestrokethickness_vecplot() const;
  bool getlineantialiased_vecplot() const;
  LineEnd getendstyle_vecplot(const LineEndLocation &location) const;
  double getendwidth_vecplot(const LineEndLocation &location) const;
  double getendheight_vecplot(const LineEndLocation &location) const;
  bool getendinverted_vecplot(const LineEndLocation &location) const;
  bool getlegendvisible_vecplot() const;
  QString getlegendtext_vecplot() const;
  Table *gettable_vecplot() const { return table_; }
  Column *getfirstcol_vecplot() const { return x1col_; }
  Column *getsecondcol_vecplot() const { return y1col_; }
  Column *getthirdcol_vecplot() const { return x2col_; }
  Column *getfourthcol_vecplot() const { return y2col_; }
  int getfrom_vecplot() const { return from_; }
  int getto_vecplot() const { return to_; }
  QIcon getIcon() const { return icon_; }

  // Setters
  void setxaxis_vecplot(Axis2D *axis);
  void setyaxis_vecplot(Axis2D *axis);
  void setlinestrokecolor_vecplot(const QColor &color);
  void setlinestrokestyle_vecplot(const Qt::PenStyle &style);
  void setlinestrokethickness_vecplot(const double value);
  void setlineantialiased_vecplot(bool status);
  void setendstyle_vecplot(const LineEnd &end, const LineEndLocation &location);
  void setendwidth_vecplot(const double value, const LineEndLocation &location);
  void setendheight_vecplot(const double value,
                            const LineEndLocation &location);
  void setendinverted_vecplot(const bool value,
                              const LineEndLocation &location);
  void setlegendvisible_vecplot(const bool value);
  void setlegendtext_vecplot(const QString &name);

  void save(XmlStreamWriter *xmlwriter, int xaxis, int yaxis);
  bool load(XmlStreamReader *xmlreader);

 private:
  void reloadendings(const LineEndLocation &location);
 void datapicker(QMouseEvent *, const QVariant &);
 void graphpicker(QMouseEvent *, const QVariant &);
 void movepicker(QMouseEvent *, const QVariant &);
 void removepicker(QMouseEvent *, const QVariant &);

 private:
  VectorPlot vectorplot_;
  Table *table_;
  Column *x1col_;
  Column *y1col_;
  Column *x2col_;
  Column *y2col_;
  int from_;
  int to_;
  Axis2D *xaxis_;
  Axis2D *yaxis_;
  QString layername_;
  QList<QCPItemLine *> linelist_;
  Position d_position_;
  QCPLineEnding *start_;
  QCPLineEnding *stop_;
  QIcon icon_;
};

#endif  // VECTOR2D_H
