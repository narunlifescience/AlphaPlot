#ifndef COLORBUTTON_H
#define COLORBUTTON_H

#include <qpushbutton.h>

class ColorButton : public QPushButton
{
  Q_OBJECT

public:
  ColorButton(QWidget *parent);
  void setColor(const QColor& c);
  QColor color() const;

protected:
  void init();
};

#endif
