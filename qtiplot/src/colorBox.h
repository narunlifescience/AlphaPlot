#ifndef COLORBOX_H
#define COLORBOX_H

#include <qcombobox.h>

class ColorBox : public QComboBox
{
  Q_OBJECT
public:
  ColorBox(bool rw, QWidget *parent);
  ColorBox(QWidget *parent);
  void setColor(const QColor& c);
  QColor color() const;

  static int colorIndex(const QColor& c);
  static QColor color(int colorIndex);
  static const QColor colors[];

protected:
  void init();
};

#endif
