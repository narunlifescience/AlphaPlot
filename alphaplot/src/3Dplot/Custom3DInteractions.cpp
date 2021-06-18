#include "Custom3DInteractions.h"

#include <QtDataVisualization/Q3DCamera>

const int Custom3DInteractions::deltazoom_ = 1;

Custom3DInteractions::Custom3DInteractions(QObject *parent)
    : QAbstract3DInputHandler(parent),
      mouserotation_(false),
      zoomfactor_(130),
      xrotation_(40),
      yrotation_(30) {}

void Custom3DInteractions::mousePressEvent(QMouseEvent *event,
                                           const QPoint &mousePos) {
  if (event->button() == Qt::MouseButton::LeftButton) {
    mousepoint_ = mousePos;
    xrotation_ = scene()->activeCamera()->xRotation();
    yrotation_ = scene()->activeCamera()->yRotation();
    mouserotation_ = true;
  } else if (event->button() == Qt::MouseButton::RightButton) {
    emit showContextMenu();
  }
  emit activateParentWindow();
  QAbstract3DInputHandler::mousePressEvent(event, mousePos);
}

void Custom3DInteractions::mouseReleaseEvent(QMouseEvent *event,
                                             const QPoint &mousePos) {
  if (event->button() == Qt::MouseButton::LeftButton) {
    mouserotation_ = false;
  }
  QAbstract3DInputHandler::mouseReleaseEvent(event, mousePos);
}

void Custom3DInteractions::mouseMoveEvent(QMouseEvent *event,
                                          const QPoint &mousePos) {
  if (mouserotation_) {
    QPoint point = mousePos - mousepoint_;
    int xrotation = xrotation_ + point.x();
    int yrotation = yrotation_ + point.y();
    scene()->activeCamera()->setCameraPosition(xrotation, yrotation,
                                               zoomfactor_);
  }
  QAbstract3DInputHandler::mouseMoveEvent(event, mousePos);
}

void Custom3DInteractions::wheelEvent(QWheelEvent *event) {
  zoomfactor_ = scene()->activeCamera()->zoomLevel();
  (event->angleDelta().y() > 0) ? zoomfactor_ += deltazoom_
                                : zoomfactor_ -= deltazoom_;
  scene()->activeCamera()->setZoomLevel(zoomfactor_);
  QAbstract3DInputHandler::wheelEvent(event);
}
