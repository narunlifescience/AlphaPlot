#include "Pie2D.h"
#include "../future/core/column/Column.h"
#include "AxisRect2D.h"
#include "Legend2D.h"
#include "core/Utilities.h"

Pie2D::Pie2D(AxisRect2D *axisrect)
    : QCPAbstractItem(axisrect->parentPlot()),
      topLeft(createPosition(QLatin1String("topLeft"))),
      bottomRight(createPosition(QLatin1String("bottomRight"))),
      axisrect_(axisrect),
      pieData_(new QVector<double>()),
      pieColors_(new QVector<QColor>()) {
  layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  topLeft->setCoords(axisrect->topLeft());
  bottomRight->setCoords(axisrect->bottomRight());
  setClipAxisRect(axisrect_);
  mPen.setColor(Qt::white);
  mBrush.setColor(Qt::red);
  mBrush.setStyle(Qt::SolidPattern);
  QList<Axis2D *> axes(axisrect_->getAxes2D());
  for (int i = 0; i < axes.size(); i++) {
    axes.at(i)->setshowhide_axis(false);
  }
}

Pie2D::~Pie2D() {
  delete pieData_;
  delete pieColors_;
}

void Pie2D::setGraphData(Column *xData, int from, int to) {
  double sum = 0.0;
  pieData_->clear();
  for (int i = from; i <= to; i++) {
    sum += xData->valueAt(i);
  }
  for (int i = from; i <= to; i++) {
    pieData_->append((xData->valueAt(i) / sum) * (360 * 16));
    pieColors_->append(
        Utilities::getRandColorGoldenRatio(Utilities::ColorPal::Light));
  }
}

double Pie2D::selectTest(const QPointF &pos, bool onlySelectable,
                         QVariant *details) const {
  Q_UNUSED(details)
  Q_UNUSED(pos)
  Q_UNUSED(onlySelectable)
  return 0.0;
}

Qt::PenStyle Pie2D::getstrokestyle_pieplot() const { return mPen.style(); }

QColor Pie2D::getstrokecolor_pieplot() const { return mPen.color(); }

double Pie2D::getstrokethickness_pieplot() const { return mPen.widthF(); }

void Pie2D::setstrokestyle_pieplot(const Qt::PenStyle &style) {
  mPen.setStyle(style);
}

void Pie2D::setstrokecolor_pieplot(const QColor &color) {
  mPen.setColor(color);
}

void Pie2D::setstrokethickness_pieplot(const double value) {
  mPen.setWidthF(value);
}

void Pie2D::draw(QCPPainter *painter) {
  if (pieData_->isEmpty()) return;
  int cumulativesum = 0;

  QPointF p1 = topLeft->pixelPosition();
  QPointF p2 = bottomRight->pixelPosition();
  if (p1.toPoint() == p2.toPoint()) return;
  QRectF ellipseRect = axisrect_->rect();
  if (ellipseRect.width() > ellipseRect.height()) {
    double dif = ellipseRect.width() - ellipseRect.height();
    double margin = (ellipseRect.height() * 2) / 100;
    ellipseRect.adjust(dif / 2 + margin, margin, -(dif / 2) - margin, -margin);
  } else {
    double dif = ellipseRect.height() - ellipseRect.width();
    double margin = (ellipseRect.width() * 2) / 100;
    ellipseRect.adjust(margin, dif / 2 + margin, -margin, -(dif / 2) - margin);
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

QPointF Pie2D::anchorPixelPosition(int anchorId) {
  Q_UNUSED(anchorId);
  return QPointF();
}
