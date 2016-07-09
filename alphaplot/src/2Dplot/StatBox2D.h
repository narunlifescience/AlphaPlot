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
    double sd_upper = 0;
    double sd_lower = 0;
    double se_upper = 0;
    double se_lower = 0;
    double perc_25 = 0;
    double perc_75 = 0;
    double perc_10 = 0;
    double perc_90 = 0;
    double perc_5 = 0;
    double perc_95 = 0;
    double perc_1 = 0;
    double perc_99 = 0;
    double min = 0;
    double max = 0;
    double constant_lower = 0;
    double constant_upper = 0;
  };

  struct BoxWhiskerData {
    double key;
    double mean;
    double median;
    double sd;
    double se;
    BoxWhiskerDataBounds boxWhiskerDataBounds;
  };

  explicit StatBox2D(Axis2D *xAxis, Axis2D *yAxis,
                     BoxWhiskerData boxWhiskerData);
  ~StatBox2D();

  void setBoxWhiskerData(BoxWhiskerData boxWhiskerData);
  void setBoxStyle(const BoxWhiskerStyle &boxStyle);
  void setWhiskerStyle(const BoxWhiskerStyle &whiskerStyle);

 private:
  BoxWhiskerData boxWhiskerData_;
};

#endif  // STATBOX2D_H
