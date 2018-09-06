#include "StatBox2D.h"

StatBox2D::StatBox2D(Axis2D *xAxis, Axis2D *yAxis,
                     BoxWhiskerData boxWhiskerData)
    : QCPStatisticalBox(xAxis, yAxis),
      statBoxdataContainer_(QSharedPointer<QCPStatisticalBoxDataContainer>(
          new QCPStatisticalBoxDataContainer())) {
  QCPStatisticalBoxData sBoxdata;
  sBoxdata.key = boxWhiskerData.key;
  sBoxdata.median = boxWhiskerData.median;
  sBoxdata = setBoxStyle(Perc_25_75, sBoxdata, boxWhiskerData);
  sBoxdata = setWhiskerStyle(Perc_5_95, sBoxdata, boxWhiskerData);
  sBoxdata.outliers << boxWhiskerData.boxWhiskerDataBounds.min
                    << boxWhiskerData.boxWhiskerDataBounds.max;
  statBoxdataContainer_->add(sBoxdata);
  setData(statBoxdataContainer_);
}

StatBox2D::~StatBox2D() {}

void StatBox2D::setBoxWhiskerData(BoxWhiskerData boxWhiskerData) {
  QSharedPointer<QCPStatisticalBoxDataContainer> statBoxdata =
      QSharedPointer<QCPStatisticalBoxDataContainer>(
          new QCPStatisticalBoxDataContainer());
  QCPStatisticalBoxData sBoxdata;
  sBoxdata.key = boxWhiskerData.key;
  sBoxdata.median = boxWhiskerData.median;
  statBoxdata->add(sBoxdata);
  setData(statBoxdata);
}

QCPStatisticalBoxData StatBox2D::setBoxStyle(
    const StatBox2D::BoxWhiskerStyle &boxStyle, QCPStatisticalBoxData sBoxdata,
    BoxWhiskerData boxWhiskerData) {
  switch (boxStyle) {
    case SD:
      sBoxdata.lowerQuartile = boxWhiskerData.boxWhiskerDataBounds.sd_lower;
      sBoxdata.upperQuartile = boxWhiskerData.boxWhiskerDataBounds.sd_upper;
      break;
    case SE:
      sBoxdata.lowerQuartile = boxWhiskerData.boxWhiskerDataBounds.se_lower;
      sBoxdata.upperQuartile = boxWhiskerData.boxWhiskerDataBounds.se_upper;
      break;
    case Perc_25_75:
      sBoxdata.lowerQuartile = boxWhiskerData.boxWhiskerDataBounds.perc_25;
      sBoxdata.upperQuartile = boxWhiskerData.boxWhiskerDataBounds.perc_75;
      break;
    case Perc_10_90:
      sBoxdata.lowerQuartile = boxWhiskerData.boxWhiskerDataBounds.perc_10;
      sBoxdata.upperQuartile = boxWhiskerData.boxWhiskerDataBounds.perc_90;
      break;
    case Perc_5_95:
      sBoxdata.lowerQuartile = boxWhiskerData.boxWhiskerDataBounds.perc_5;
      sBoxdata.upperQuartile = boxWhiskerData.boxWhiskerDataBounds.perc_95;
      break;
    case Perc_1_99:
      sBoxdata.lowerQuartile = boxWhiskerData.boxWhiskerDataBounds.perc_1;
      sBoxdata.upperQuartile = boxWhiskerData.boxWhiskerDataBounds.perc_99;
      break;
    case MinMax:
      sBoxdata.lowerQuartile = boxWhiskerData.boxWhiskerDataBounds.min;
      sBoxdata.upperQuartile = boxWhiskerData.boxWhiskerDataBounds.max;
      break;
    case Constant:
      sBoxdata.lowerQuartile =
          boxWhiskerData.boxWhiskerDataBounds.constant_lower;
      sBoxdata.upperQuartile =
          boxWhiskerData.boxWhiskerDataBounds.constant_upper;
      break;
  }
  return sBoxdata;
}

QCPStatisticalBoxData StatBox2D::setWhiskerStyle(
    const StatBox2D::BoxWhiskerStyle &whiskerStyle,
    QCPStatisticalBoxData sBoxdata, BoxWhiskerData boxWhiskerData) {
  switch (whiskerStyle) {
    case SD:
      sBoxdata.minimum = boxWhiskerData.boxWhiskerDataBounds.sd_lower;
      sBoxdata.maximum = boxWhiskerData.boxWhiskerDataBounds.sd_upper;
      break;
    case SE:
      sBoxdata.minimum = boxWhiskerData.boxWhiskerDataBounds.se_lower;
      sBoxdata.maximum = boxWhiskerData.boxWhiskerDataBounds.se_upper;
      break;
    case Perc_25_75:
      sBoxdata.minimum = boxWhiskerData.boxWhiskerDataBounds.perc_25;
      sBoxdata.maximum = boxWhiskerData.boxWhiskerDataBounds.perc_75;
      break;
    case Perc_10_90:
      sBoxdata.minimum = boxWhiskerData.boxWhiskerDataBounds.perc_10;
      sBoxdata.maximum = boxWhiskerData.boxWhiskerDataBounds.perc_90;
      break;
    case Perc_5_95:
      sBoxdata.minimum = boxWhiskerData.boxWhiskerDataBounds.perc_5;
      sBoxdata.maximum = boxWhiskerData.boxWhiskerDataBounds.perc_95;
      break;
    case Perc_1_99:
      sBoxdata.minimum = boxWhiskerData.boxWhiskerDataBounds.perc_1;
      sBoxdata.maximum = boxWhiskerData.boxWhiskerDataBounds.perc_99;
      break;
    case MinMax:
      sBoxdata.minimum = boxWhiskerData.boxWhiskerDataBounds.min;
      sBoxdata.maximum = boxWhiskerData.boxWhiskerDataBounds.max;
      break;
    case Constant:
      sBoxdata.minimum = boxWhiskerData.boxWhiskerDataBounds.constant_lower;
      sBoxdata.maximum = boxWhiskerData.boxWhiskerDataBounds.constant_upper;
      break;
  }
  return sBoxdata;
}
