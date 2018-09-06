#ifndef STATBOX2D_H
#define STATBOX2D_H

#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "Axis2D.h"

class StatBox2D : public QCPStatisticalBox {
  Q_OBJECT
 public:
  enum BoxWhiskerStyle {
    SD,
    SE,
    Perc_25_75,
    Perc_10_90,
    Perc_5_95,
    Perc_1_99,
    MinMax,
    Constant,
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
    BoxWhiskerData() {
      key = 0;
      mean = 0;
      median = 0;
      sd = 0;
      se = 0;
    }
  };

  explicit StatBox2D(Axis2D *xAxis, Axis2D *yAxis,
                     BoxWhiskerData boxWhiskerData);
  ~StatBox2D();

  void setBoxWhiskerData(BoxWhiskerData boxWhiskerData);
  QCPStatisticalBoxData setBoxStyle(const BoxWhiskerStyle &boxStyle,
                                    QCPStatisticalBoxData sBoxdata,
                                    BoxWhiskerData boxWhiskerData);
  QCPStatisticalBoxData setWhiskerStyle(const BoxWhiskerStyle &whiskerStyle,
                                        QCPStatisticalBoxData sBoxdata,
                                        BoxWhiskerData boxWhiskerData);
  void setBoxWhiskerWidth(double width) { setWidth(width); }
  double boxWhiskerWidth() { return width(); }

 private:
  QSharedPointer<QCPStatisticalBoxDataContainer> statBoxdataContainer_;
};

#endif  // STATBOX2D_H
