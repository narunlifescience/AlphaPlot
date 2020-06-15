#include "Custom3DInteractions.h"

#include <QtDataVisualization/Q3DCamera>

const int Custom3DInteractions::deltazoom_ = 1;

Custom3DInteractions::Custom3DInteractions(QObject *parent)
    : QAbstract3DInputHandler(parent),
      mouserotation_(false),
      zoomfactor_(100) {}

void Custom3DInteractions::mousePressEvent(QMouseEvent *event,
                                           const QPoint &mousePos) {
  setInputPosition(mousePos);
  if (event->button() == Qt::MouseButton::LeftButton) {
    mousepoint_ = mousepoint_ - mousePos;
    mouserotation_ = true;
  }
  QAbstract3DInputHandler::mousePressEvent(event, mousePos);
}

void Custom3DInteractions::mouseReleaseEvent(QMouseEvent *event,
                                             const QPoint &mousePos) {
  if (event->button() == Qt::MouseButton::LeftButton) {
    mousepoint_ = mousePos;
    mouserotation_ = false;
  }
  QAbstract3DInputHandler::mouseReleaseEvent(event, mousePos);
}

void Custom3DInteractions::mouseMoveEvent(QMouseEvent *event,
                                          const QPoint &mousePos) {
  if (mouserotation_) {
    scene()->activeCamera()->setCameraPosition(
        int(mousePos.x() + mousepoint_.x()),
        int(mousePos.y() + mousepoint_.y()), zoomfactor_);
  }
  QAbstract3DInputHandler::mouseMoveEvent(event, mousePos);
}

void Custom3DInteractions::wheelEvent(QWheelEvent *event) {
  (event->angleDelta().y() > 0) ? zoomfactor_ += deltazoom_
                                : zoomfactor_ -= deltazoom_;
  scene()->activeCamera()->setZoomLevel(zoomfactor_);
  QAbstract3DInputHandler::wheelEvent(event);
}
