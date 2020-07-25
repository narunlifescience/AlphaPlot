#include "LayoutInset2D.h"

#include "AxisRect2D.h"

LayoutInset2D::LayoutInset2D(AxisRect2D *axisrect)
    : QCPLayoutInset(),
      axisrect_(axisrect),
      layername_(
          QString("<LayoutInset2D>") +
          QDateTime::currentDateTime().toString("yyyy:MM:dd:hh:mm:ss:zzz")) {
  Q_ASSERT(axisrect_);
  // setting Layer
  QThread::msleep(1);
  //parentPlot()->addLayer(layername_, axisrect_->getAxes2D().at(0)->layer(),
  //                       QCustomPlot::limBelow);
  //setLayer(layername_);
  //layer()->setMode(QCPLayer::LayerMode::lmBuffered);
  QRect rect = axisrect_->rect();
  rect.setX(rect.x() + rect.width() / 10);
  rect.setY(rect.y() + rect.height() / 10);
  rect.setHeight(rect.height() / 3);
  rect.setWidth(rect.width() / 3);
  addElement(axisrect_, QRectF(0.6, 0.1, 0.35, 0.35));
}

LayoutInset2D::~LayoutInset2D() {}
