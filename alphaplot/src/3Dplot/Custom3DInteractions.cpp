#include "Custom3DInteractions.h"

#include <QtDataVisualization/Q3DCamera>

Custom3DInteractions::Custom3DInteractions(QObject *parent)
    : QAbstract3DInputHandler(parent) {}

void Custom3DInteractions::mouseMoveEvent(QMouseEvent *event,
                                          const QPoint &mousePos) {
  Q_UNUSED(event)
  setInputPosition(mousePos);
}

void Custom3DInteractions::wheelEvent(QWheelEvent *event) {
  // Adjust zoom level based on what zoom range we're in.
  float zoomLevel = scene()->activeCamera()->zoomLevel();
  if (zoomLevel > 100)
    zoomLevel += event->angleDelta().y() / 12;
  else if (zoomLevel > 50)
    zoomLevel += event->angleDelta().y() / 60;
  else
    zoomLevel += event->angleDelta().y() / 120;
  if (zoomLevel > 500)
    zoomLevel = 500;
  else if (zoomLevel < 10)
    zoomLevel = 10;

  scene()->activeCamera()->setZoomLevel(zoomLevel);
}
