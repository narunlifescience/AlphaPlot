#ifndef LAYOUT3D_H
#define LAYOUT3D_H

#include "MyWidget.h"

class Layout3D : public MyWidget {
  Q_OBJECT
 public:
  Layout3D(const QString &label, QWidget *parent = nullptr,
           const QString name = QString(), Qt::WindowFlags f = Qt::Widget);
  ~Layout3D();

  void generateSurfacePlot3D();

 private:
  QWidget *main_widget_;
};

#endif  // LAYOUT3D_H
