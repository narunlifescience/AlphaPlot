#ifndef STATBOX2D_H
#define STATBOX2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Graph2DCommon.h"

class Axis2D;
class Table;
class XmlStreamReader;
class XmlStreamWriter;

class StatBox2D : public QCPStatisticalBox {
  Q_OBJECT
 public:
  enum BoxWhiskerStyle {
    SD = 0,
    SE = 1,
    Perc_25_75 = 2,
    Perc_10_90 = 3,
    Perc_5_95 = 4,
    Perc_1_99 = 5,
    MinMax = 6,
    Constant = 7,
  };

  struct BoxWhiskerDataBounds {
    double sd_upper;
    double sd_lower;
    double se_upper;
    double se_lower;
    double perc_25;
    double perc_75;
    double perc_10;
    double perc_90;
    double perc_5;
    double perc_95;
    double perc_1;
    double perc_99;
    double min;
    double max;
    double constant_lower;
    double constant_upper;
    BoxWhiskerDataBounds() {
      sd_upper = 0;
      sd_lower = 0;
      se_upper = 0;
      se_lower = 0;
      perc_25 = 0;
      perc_75 = 0;
      perc_10 = 0;
      perc_90 = 0;
      perc_5 = 0;
      perc_95 = 0;
      perc_1 = 0;
      perc_99 = 0;
      min = 0;
      max = 0;
      constant_lower = 0;
      constant_upper = 0;
    }
  };

  struct BoxWhiskerData {
    double key;
    double mean;
    double median;
    double sd;
    double se;
    BoxWhiskerDataBounds boxWhiskerDataBounds;
    QString name;
    Table *table_;
    Column *column_;
    int from_;
    int to_;
    BoxWhiskerData() {
      key = 0;
      mean = 0;
      median = 0;
      sd = 0;
      se = 0;
      name = QString();
      table_ = nullptr;
      column_ = nullptr;
      from_ = -1;
      to_ = -1;
    }
  };

  explicit StatBox2D(BoxWhiskerData boxWhiskerData, Axis2D *xAxis,
                     Axis2D *yAxis);
  ~StatBox2D();

  Axis2D *getxaxis() const;
  Axis2D *getyaxis() const;
  BoxWhiskerStyle getboxstyle_statbox() const;
  BoxWhiskerStyle getwhiskerstyle_statbox() const;
  QColor getfillcolor_statbox() const;
  Qt::BrushStyle getfillstyle_statbox() const;
  bool getfillstatus_statbox() const;
  Qt::PenStyle getwhiskerstrokestyle_statbox() const;
  QColor getwhiskerstrokecolor_statbox() const;
  double getwhiskerstrokethickness_statbox() const;
  Qt::PenStyle getwhiskerbarstrokestyle_statbox() const;
  QColor getwhiskerbarstrokecolor_statbox() const;
  double getwhiskerbarstrokethickness_statbox() const;
  Qt::PenStyle getmedianstrokestyle_statbox() const;
  QColor getmedianstrokecolor_statbox() const;
  double getmedianstrokethickness_statbox() const;
  Graph2DCommon::ScatterStyle getscattershape_statbox() const;
  QColor getscatterfillcolor_statbox() const;
  double getscattersize_statbox() const;
  Qt::PenStyle getscatterstrokestyle_statbox() const;
  QColor getscatterstrokecolor_statbox() const;
  double getscatterstrokethickness_statbox() const;
  BoxWhiskerData getboxwhiskerdata_statbox() const { return boxwhiskerdata_; }
  Table *gettable_statbox() { return boxwhiskerdata_.table_; }
  Column *getcolumn_statbox() { return boxwhiskerdata_.column_; }
  int getfrom_statbox() const { return boxwhiskerdata_.from_; }
  int getto_statbox() const { return boxwhiskerdata_.to_; }
  QIcon getIcon() const { return icon_; }

  void setxaxis_statbox(Axis2D *axis);
  void setyaxis_statbox(Axis2D *axis);
  void setboxwhiskerdata(const BoxWhiskerData boxWhiskerData);
  void setboxstyle_statbox(const BoxWhiskerStyle &boxStyle);
  void setwhiskerstyle_statbox(const BoxWhiskerStyle &whiskerStyle);
  void setfillcolor_statbox(const QColor &color);
  void setfillstyle_statbox(const Qt::BrushStyle &style);
  void setfillstatus_statbox(const bool status);
  void setwhiskerstrokestyle_statbox(const Qt::PenStyle &style);
  void setwhiskerstrokecolor_statbox(const QColor &color);
  void setwhiskerstrokethickness_statbox(const double value);
  void setwhiskerbarstrokestyle_statbox(const Qt::PenStyle &style);
  void setwhiskerbarstrokecolor_statbox(const QColor &color);
  void setwhiskerbarstrokethickness_statbox(const double value);
  void setmedianstrokestyle_statbox(const Qt::PenStyle &style);
  void setmedianstrokecolor_statbox(const QColor &color);
  void setmedianstrokethickness_statbox(const double value);
  void setscattershape_statbox(const Graph2DCommon::ScatterStyle &shape);
  void setscatterfillcolor_statbox(const QColor &color);
  void setscattersize_statbox(const double value);
  void setscatterstrokestyle_statbox(const Qt::PenStyle &style);
  void setscatterstrokecolor_statbox(const QColor &color);
  void setscatterstrokethickness_statbox(const double value);
  void reloaddata_statbox();
  void setlegendtext_statbox(const QString name);

  void save(XmlStreamWriter *xmlwriter, int xaxis, int yaxis);
  bool load(XmlStreamReader *xmlreader);

 protected:
  void mousePressEvent(QMouseEvent *event, const QVariant &details);

 private:
  void datapicker(QMouseEvent *event, const QVariant &details);
  void movepicker(QMouseEvent *, const QVariant &);
  void removepicker(QMouseEvent *, const QVariant &);

 private:
  Axis2D *xAxis_;
  Axis2D *yAxis_;
  QString layername_;
  BoxWhiskerData boxwhiskerdata_;
  QCPStatisticalBoxData sBoxdata_;
  QCPScatterStyle *scatterstyle_;
  BoxWhiskerStyle boxstyle_;
  BoxWhiskerStyle whiskerstyle_;
  QIcon icon_;
};

#endif  // STATBOX2D_H
