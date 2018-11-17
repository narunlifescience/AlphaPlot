#ifndef CUSTOM3DINTERACTIONS_H
#define CUSTOM3DINTERACTIONS_H

#include <QtDataVisualization/QAbstract3DInputHandler>

using namespace QtDataVisualization;

class Custom3DInteractions : public QAbstract3DInputHandler {
  Q_OBJECT
 public:
  Custom3DInteractions(QObject *parent = nullptr);

  virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
  virtual void wheelEvent(QWheelEvent *event);
};

#endif  // CUSTOM3DINTERACTIONS_H
