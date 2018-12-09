#include "Pie2D.h"
#include "../future/core/column/Column.h"
#include "AxisRect2D.h"
#include "Legend2D.h"
#include "Table.h"
#include "core/Utilities.h"

Pie2D::Pie2D(AxisRect2D *axisrect, Table *table, Column *xData, int from,
             int to)
    : QCPAbstractItem(axisrect->parentPlot()),
      // topLeft(createPosition(QLatin1String("topLeft"))),
      // bottomRight(createPosition(QLatin1String("bottomRight"))),
      axisrect_(axisrect),
      pieData_(new QVector<double>()),
      pieColors_(new QVector<QColor>()),
      pieLegendItems_(new QVector<PieLegendItem2D *>()),
      layername_(
          QDateTime::currentDateTime().toString("yyyy:MM:dd:hh:mm:ss:zzz")),
      marginpercent_(2),
      table_(table),
      xcolumn_(xData),
      from_(from),
      to_(to) {
  if (axisrect_->getAxes2D().count() > 0) {
    QThread::msleep(1);
    parentPlot()->addLayer(layername_, axisrect_->getAxes2D().at(0)->layer(),
                           QCustomPlot::limBelow);
    setLayer(layername_);
  }
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  // topLeft->setCoords(axisrect->topLeft());
  // bottomRight->setCoords(axisrect->bottomRight());
  setClipAxisRect(axisrect_);
  mPen.setColor(Qt::white);
  mBrush.setColor(Qt::red);
  mBrush.setStyle(Qt::SolidPattern);
  QList<Axis2D *> axes(axisrect_->getAxes2D());
  for (int i = 0; i < axes.size(); i++) {
    axes.at(i)->setshowhide_axis(false);
  }
  double sum = 0.0;
  pieData_->clear();
  for (int i = from; i <= to; i++) {
    sum += xData->valueAt(i);
  }
  for (int i = from; i <= to; i++) {
    pieData_->append((xData->valueAt(i) / sum) * (360 * 16));
    QColor color =
        Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light);
    pieColors_->append(color);
    PieLegendItem2D *pielegenditem = new PieLegendItem2D(
        axisrect_->getLegend(), color, QString::number(xData->valueAt(i)));
    pieLegendItems_->append(pielegenditem);
    axisrect_->getLegend()->addItem(pielegenditem);
  }
  setGraphData(table_, xcolumn_, from_, to_);
}

Pie2D::~Pie2D() {
  pieData_->clear();
  delete pieData_;
  pieColors_->clear();
  delete pieColors_;
  pieLegendItems_->clear();
  delete pieLegendItems_;
  parentPlot()->removeLayer(layer());
}

void Pie2D::setGraphData(Table *table, Column *xData, int from, int to) {
  table_ = table;
  xcolumn_ = xData;
  from_ = from;
  to_ = to;
  double sum = 0.0;
  pieData_->clear();
  for (int i = from; i <= to; i++) {
    sum += xData->valueAt(i);
  }
  for (int i = from; i <= to; i++) {
    pieData_->append((xData->valueAt(i) / sum) * (360 * 16));
    QColor color =
        Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light);
    pieColors_->append(color);
    PieLegendItem2D *pielegenditem = new PieLegendItem2D(
        axisrect_->getLegend(), color, QString::number(xData->valueAt(i)));
    pieLegendItems_->append(pielegenditem);
    axisrect_->getLegend()->addItem(pielegenditem);
  }
}

double Pie2D::selectTest(const QPointF &pos, bool onlySelectable,
                         QVariant *details) const {
  Q_UNUSED(details)
  Q_UNUSED(pos)
  Q_UNUSED(onlySelectable)
  return 0.0;
}

AxisRect2D *Pie2D::getaxisrect() const { return axisrect_; }

Qt::PenStyle Pie2D::getstrokestyle_pieplot() const { return mPen.style(); }

QColor Pie2D::getstrokecolor_pieplot() const { return mPen.color(); }

double Pie2D::getstrokethickness_pieplot() const { return mPen.widthF(); }

int Pie2D::getmarginpercent_pieplot() const { return marginpercent_; }

void Pie2D::setstrokestyle_pieplot(const Qt::PenStyle &style) {
  mPen.setStyle(style);
}

void Pie2D::setstrokecolor_pieplot(const QColor &color) {
  mPen.setColor(color);
}

void Pie2D::setstrokethickness_pieplot(const double value) {
  mPen.setWidthF(value);
}

void Pie2D::setmarginpercent_pieplot(const int value) {
  marginpercent_ = value;
}

void Pie2D::draw(QCPPainter *painter) {
  if (pieData_->isEmpty()) return;
  int cumulativesum = 0;

  // QPointF p1 = topLeft->pixelPosition();
  // QPointF p2 = bottomRight->pixelPosition();
  // if (p1.toPoint() == p2.toPoint()) return;
  QRectF ellipseRect = axisrect_->rect();
  if (ellipseRect.width() > ellipseRect.height()) {
    double dif = ellipseRect.width() - ellipseRect.height();
    double margin =
        (ellipseRect.height() * static_cast<double>(marginpercent_) / 2) / 100;
    ellipseRect.adjust(dif / 2 + margin, margin, -(dif / 2) - margin, -margin);
  } else {
    double dif = ellipseRect.height() - ellipseRect.width();
    double margin =
        (ellipseRect.width() * static_cast<double>(marginpercent_) / 2) / 100;
    ellipseRect.adjust(margin, (dif / 2) + margin, -margin,
                       -(dif / 2) - margin);
  }

  painter->setPen(mPen);
  painter->setBrush(mBrush);
  for (int i = 0; i < pieData_->size(); i++) {
    mBrush.setColor(pieColors_->at(i));
    painter->setBrush(mBrush);
    painter->drawPie(ellipseRect, cumulativesum,
                     static_cast<int>(pieData_->at(i)));
    cumulativesum += pieData_->at(i);
  }
}
