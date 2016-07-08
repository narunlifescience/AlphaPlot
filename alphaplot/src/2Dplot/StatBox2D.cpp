#include "StatBox2D.h"

StatBox2D::StatBox2D(Axis2D *xAxis, Axis2D *yAxis,
                     BoxWhiskerTotalData boxWhiskerData)
    : QCPStatisticalBox(xAxis, yAxis),
      boxWhiskerData_(boxWhiskerData) {
  setBoxStyle(Perc_25_75);
  setWhiskerStyle(MinMax);
  setKey(boxWhiskerData_.key);
  setMedian(boxWhiskerData_.median);
}

StatBox2D::~StatBox2D() {}

void StatBox2D::setBoxWhiskerData(BoxWhiskerTotalData boxWhiskerData) {
  boxWhiskerData_ = boxWhiskerData;
}

void StatBox2D::setBoxStyle(const StatBox2D::BoxWhiskerStyle &boxStyle) {
  switch (boxStyle) {
    case SD:
      setLowerQuartile(boxWhiskerData_.boxData.sd_lower);
      setUpperQuartile(boxWhiskerData_.boxData.sd_upper);
      break;
    case SE:
      setLowerQuartile(boxWhiskerData_.boxData.se_lower);
      setUpperQuartile(boxWhiskerData_.boxData.se_upper);
      break;
    case Perc_25_75:
      setLowerQuartile(boxWhiskerData_.boxData.perc_25);
      setUpperQuartile(boxWhiskerData_.boxData.perc_75);
      break;
    case Perc_10_90:
      setLowerQuartile(boxWhiskerData_.boxData.perc_10);
      setUpperQuartile(boxWhiskerData_.boxData.perc_90);
      break;
    case Perc_5_95:
      setLowerQuartile(boxWhiskerData_.boxData.perc_5);
      setUpperQuartile(boxWhiskerData_.boxData.perc_95);
      break;
    case Perc_1_99:
      setLowerQuartile(boxWhiskerData_.boxData.perc_1);
      setUpperQuartile(boxWhiskerData_.boxData.perc_99);
      break;
    case MinMax:
      setLowerQuartile(boxWhiskerData_.boxData.min);
      setUpperQuartile(boxWhiskerData_.boxData.max);
      break;
    case Constant:
      setLowerQuartile(boxWhiskerData_.boxData.constant_lower);
      setUpperQuartile(boxWhiskerData_.boxData.constant_upper);
      break;
  }
}

void StatBox2D::setWhiskerStyle(
    const StatBox2D::BoxWhiskerStyle &whiskerStyle) {
  switch (whiskerStyle) {
    case SD:
      setMinimum(boxWhiskerData_.whiskerData.sd_lower);
      setMaximum(boxWhiskerData_.whiskerData.sd_upper);
      break;
    case SE:
      setMinimum(boxWhiskerData_.whiskerData.se_lower);
      setMaximum(boxWhiskerData_.whiskerData.se_upper);
      break;
    case Perc_25_75:
      setMinimum(boxWhiskerData_.whiskerData.perc_25);
      setMaximum(boxWhiskerData_.whiskerData.perc_75);
      break;
    case Perc_10_90:
      setMinimum(boxWhiskerData_.whiskerData.perc_10);
      setMaximum(boxWhiskerData_.whiskerData.perc_90);
      break;
    case Perc_5_95:
      setMinimum(boxWhiskerData_.whiskerData.perc_5);
      setMaximum(boxWhiskerData_.whiskerData.perc_95);
      break;
    case Perc_1_99:
      setMinimum(boxWhiskerData_.whiskerData.perc_1);
      setMaximum(boxWhiskerData_.whiskerData.perc_99);
      break;
    case MinMax:
      setMinimum(boxWhiskerData_.whiskerData.min);
      setMaximum(boxWhiskerData_.whiskerData.max);
      break;
    case Constant:
      setMinimum(boxWhiskerData_.whiskerData.constant_lower);
      setMaximum(boxWhiskerData_.whiskerData.constant_upper);
      break;
  }
}
