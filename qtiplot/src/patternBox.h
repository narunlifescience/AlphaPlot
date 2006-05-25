#ifndef PATTERNBOX_H
#define PATTERNBOX_H

#include <qcombobox.h>


class PatternBox : public QComboBox
{
  Q_OBJECT
public:
  PatternBox(bool rw, QWidget *parent);
  PatternBox(QWidget *parent);
  void setPattern(const Qt::BrushStyle& c);
  Qt::BrushStyle getSelectedPattern() const;

  static const Qt::BrushStyle patterns[];
  static int patternIndex(const Qt::BrushStyle& style);

protected:
  void init();
};

#endif
