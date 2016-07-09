#include "StatBox2D.h"

StatBox2D::StatBox2D(Axis2D *xAxis, Axis2D *yAxis,
                     BoxWhiskerData boxWhiskerData)
    : QCPStatisticalBox(xAxis, yAxis), boxWhiskerData_(boxWhiskerData) {
  setBoxStyle(Perc_25_75);
  setWhiskerStyle(MinMax);
  setKey(boxWhiskerData_.key);
  setMedian(boxWhiskerData_.median);
}

StatBox2D::~StatBox2D() {}

void StatBox2D::setBoxWhiskerData(BoxWhiskerData boxWhiskerData) {
  boxWhiskerData_ = boxWhiskerData;
}

void StatBox2D::setBoxStyle(const StatBox2D::BoxWhiskerStyle &boxStyle) {
  switch (boxStyle) {
    case SD:
      setLowerQuartile(boxWhiskerData_.boxWhiskerDataBounds.sd_lower);
      setUpperQuartile(boxWhiskerData_.boxWhiskerDataBounds.sd_upper);
      break;
    case SE:
      setLowerQuartile(boxWhiskerData_.boxWhiskerDataBounds.se_lower);
      setUpperQuartile(boxWhiskerData_.boxWhiskerDataBounds.se_upper);
      break;
    case Perc_25_75:
      setLowerQuartile(boxWhiskerData_.boxWhiskerDataBounds.perc_25);
      setUpperQuartile(boxWhiskerData_.boxWhiskerDataBounds.perc_75);
      break;
    case Perc_10_90:
      setLowerQuartile(boxWhiskerData_.boxWhiskerDataBounds.perc_10);
      setUpperQuartile(boxWhiskerData_.boxWhiskerDataBounds.perc_90);
      break;
    case Perc_5_95:
      setLowerQuartile(boxWhiskerData_.boxWhiskerDataBounds.perc_5);
      setUpperQuartile(boxWhiskerData_.boxWhiskerDataBounds.perc_95);
      break;
    case Perc_1_99:
      setLowerQuartile(boxWhiskerData_.boxWhiskerDataBounds.perc_1);
      setUpperQuartile(boxWhiskerData_.boxWhiskerDataBounds.perc_99);
      break;
    case MinMax:
      setLowerQuartile(boxWhiskerData_.boxWhiskerDataBounds.min);
      setUpperQuartile(boxWhiskerData_.boxWhiskerDataBounds.max);
      break;
    case Constant:
      setLowerQuartile(boxWhiskerData_.boxWhiskerDataBounds.constant_lower);
      setUpperQuartile(boxWhiskerData_.boxWhiskerDataBounds.constant_upper);
      break;
  }
}

void StatBox2D::setWhiskerStyle(
    const StatBox2D::BoxWhiskerStyle &whiskerStyle) {
  switch (whiskerStyle) {
    case SD:
      setMinimum(boxWhiskerData_.boxWhiskerDataBounds.sd_lower);
      setMaximum(boxWhiskerData_.boxWhiskerDataBounds.sd_upper);
      break;
    case SE:
      setMinimum(boxWhiskerData_.boxWhiskerDataBounds.se_lower);
      setMaximum(boxWhiskerData_.boxWhiskerDataBounds.se_upper);
      break;
    case Perc_25_75:
      setMinimum(boxWhiskerData_.boxWhiskerDataBounds.perc_25);
      setMaximum(boxWhiskerData_.boxWhiskerDataBounds.perc_75);
      break;
    case Perc_10_90:
      setMinimum(boxWhiskerData_.boxWhiskerDataBounds.perc_10);
      setMaximum(boxWhiskerData_.boxWhiskerDataBounds.perc_90);
      break;
    case Perc_5_95:
      setMinimum(boxWhiskerData_.boxWhiskerDataBounds.perc_5);
      setMaximum(boxWhiskerData_.boxWhiskerDataBounds.perc_95);
      break;
    case Perc_1_99:
      setMinimum(boxWhiskerData_.boxWhiskerDataBounds.perc_1);
      setMaximum(boxWhiskerData_.boxWhiskerDataBounds.perc_99);
      break;
    case MinMax:
      setMinimum(boxWhiskerData_.boxWhiskerDataBounds.min);
      setMaximum(boxWhiskerData_.boxWhiskerDataBounds.max);
      break;
    case Constant:
      setMinimum(boxWhiskerData_.boxWhiskerDataBounds.constant_lower);
      setMaximum(boxWhiskerData_.boxWhiskerDataBounds.constant_upper);
      break;
  }
}
