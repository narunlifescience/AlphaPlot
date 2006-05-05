#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <qwidget.h>

class QPushButton;
class QFrame;

class ColorButton : public QWidget
{
  Q_OBJECT

public:
  ColorButton(QWidget *parent);
  void setColor(const QColor& c);
  QColor color() const;

  QPushButton *selectButton;
  QFrame *display;

signals:
  void clicked();

protected:
  void init();
};

#endif
