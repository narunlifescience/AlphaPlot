#ifndef CUSTOM3DINTERACTIONS_H
#define CUSTOM3DINTERACTIONS_H
#ifdef PLOT3D_QT

#include <QtDataVisualization/QAbstract3DInputHandler>

using namespace QtDataVisualization;

class Custom3DInteractions : public QAbstract3DInputHandler {
  Q_OBJECT
 public:
  Custom3DInteractions(QObject *parent = nullptr);

  virtual void mousePressEvent(QMouseEvent *event, const QPoint &mousePos);
  virtual void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos);
  virtual void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos);
  virtual void wheelEvent(QWheelEvent *event);

 private:
  QPointF mousepoint_;
  bool mouserotation_;
};

#endif  // PLOT3D_QT
#endif  // CUSTOM3DINTERACTIONS_H
