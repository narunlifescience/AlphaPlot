#ifndef SYMBOLBOX_H
#define SYMBOLBOX_H

#include <qcombobox.h>
#include <qwt_symbol.h>

class SymbolBox : public QComboBox
{
  Q_OBJECT
public:
  SymbolBox(bool rw, QWidget *parent);
  SymbolBox(QWidget *parent);

  void setStyle(const QwtSymbol::Style& c);
  QwtSymbol::Style selectedSymbol() const;

  static QwtSymbol::Style style(int index);
  static int symbolIndex(const QwtSymbol::Style& style);

  static const QwtSymbol::Style symbols[];

protected:
  void init();
};

#endif
