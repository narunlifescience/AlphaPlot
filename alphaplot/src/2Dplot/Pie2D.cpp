#include "Pie2D.h"
#include "../future/core/column/Column.h"
#include "AxisRect2D.h"
#include "core/Utilities.h"

Pie2D::Pie2D(AxisRect2D *axisrect)
    : QCPAbstractItem(axisrect->parentPlot()),
      topLeft(createPosition(QLatin1String("topLeft"))),
      bottomRight(createPosition(QLatin1String("bottomRight"))),
      axisrect_(axisrect),
      pieData_(new QVector<double>()),
      pieColors_(new QVector<QColor>()) {
  topLeft->setCoords(axisrect->topLeft());
  bottomRight->setCoords(axisrect->bottomRight());
  // setClipAxisRect(axisrect_);
  // setClipToAxisRect(true);
  mPen.setColor(Qt::white);
  mBrush.setColor(Qt::red);
  mBrush.setStyle(Qt::SolidPattern);
  QList<Axis2D *> axes(axisrect_->getAxes2D());
  for(int i= 0; i<axes.size(); i++) {
      axes.at(i)->setshowhide_axis(false);
    }
}

Pie2D::~Pie2D() {
  delete pieData_;
  delete pieColors_;
}

void Pie2D::setGraphData(Column *xData, int from, int to) {
  double min_x, max_x, sum = 0.0;
  min_x = xData->valueAt(from);
  max_x = xData->valueAt(from);
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
  // axisrect_->width() > axisrect_->height()
  // QRect clip = clipRect().adjusted(-mPen.widthF(), -mPen.widthF(),
  //                                 mPen.widthF(), mPen.widthF());
  // if (ellipseRect.intersects(clip))  // only draw if bounding rect of ellipse
  // is
  // visible in cliprect

  painter->setPen(mPen);
  painter->setBrush(mBrush);
  // painter->setBrush();
  // painter->drawEllipse(ellipseRect);
  // painter->drawPie(ellipseRect, 0, 360*16);
  for (int i = 0; i < pieData_->size(); i++) {
    mBrush.setColor(pieColors_->at(i));
    painter->setBrush(mBrush);
    painter->drawPie(ellipseRect, cumulativesum, pieData_->at(i));
    cumulativesum += pieData_->at(i);
  }
}

QPointF Pie2D::anchorPixelPosition(int anchorId) { return QPointF(); }
