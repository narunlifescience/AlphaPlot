#ifndef CONTROLWIDGET_H
#define CONTROLWIDGET_H

#include <QWidget>

class ControlWidget : public QWidget {
  Q_OBJECT
 public:
  explicit ControlWidget(QWidget *parent = nullptr);

 protected:
  void resizeEvent(QResizeEvent *event);

 signals:
  void widthChanged();
};

#endif  // CONTROLWIDGET_H
