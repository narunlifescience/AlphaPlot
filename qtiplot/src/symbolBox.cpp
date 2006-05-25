#include "symbolBox.h"
#include <algorithm>

#include <qpixmap.h>
#include <qpainter.h>

const QwtSymbol::Style SymbolBox::symbols[] = {
  QwtSymbol::None,
  QwtSymbol::Ellipse,
  QwtSymbol::Rect,
  QwtSymbol::Diamond,
  QwtSymbol::Triangle,
  QwtSymbol::DTriangle,
  QwtSymbol::UTriangle,
  QwtSymbol::LTriangle,
  QwtSymbol::RTriangle, 
  QwtSymbol::Cross, 
  QwtSymbol::XCross, 
};

SymbolBox::SymbolBox(bool rw, QWidget *parent) : QComboBox(rw, parent)
{
  init();
}

SymbolBox::SymbolBox(QWidget *parent) : QComboBox(parent)
{
  init();
}

void SymbolBox::init()
{
	QPixmap icon = QPixmap(14, 14);
	icon.fill ( QColor (gray) );
	const QRect r= QRect(0, 0, 14, 14);
	QPainter p(&icon);
	p.setBackgroundColor(QColor(gray));
	QwtSymbol symb;
	p.setBrush(QBrush(QColor(white)));

	this->insertItem(tr("No Symbol" ));

	symb.setStyle (QwtSymbol::Ellipse);
	symb.draw(&p, r);
	this->insertItem(icon, tr("Ellipse" ));

	symb.setStyle (QwtSymbol::Rect);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Rectangle"));

	symb.setStyle (QwtSymbol::Diamond);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Diamond"));

	symb.setStyle (QwtSymbol::Triangle);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Triangle"));

	symb.setStyle (QwtSymbol::DTriangle);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Down Triangle"));

	symb.setStyle (QwtSymbol::UTriangle);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Up Triangle"));

	symb.setStyle (QwtSymbol::LTriangle);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Left Triangle"));

	symb.setStyle (QwtSymbol::RTriangle);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Right Triangle"));

	symb.setStyle (QwtSymbol::Cross);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Cross"));

	symb.setStyle (QwtSymbol::XCross);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Diagonal Cross"));

	p.end();
}

void SymbolBox::setStyle(const QwtSymbol::Style& style)
{
  const QwtSymbol::Style*ite = std::find(symbols, symbols + sizeof(symbols), style);
  if (ite == symbols + sizeof(symbols))
    this->setCurrentItem(0);
  else 
    this->setCurrentItem(ite - symbols);
}

QwtSymbol::Style SymbolBox::selectedSymbol() const
{
  size_t i = this->currentItem();
  if (i < sizeof(symbols))
    return symbols[this->currentItem()];
  else
    return QwtSymbol::None;
}

int SymbolBox::symbolIndex(const QwtSymbol::Style& style)
{
  const QwtSymbol::Style*ite = std::find(symbols, symbols + sizeof(symbols), style);
  if (ite == symbols + sizeof(symbols))
    return 0; // default pattern is None.
  else
    return (ite - symbols);
}

QwtSymbol::Style SymbolBox::style(int index)
{
  if (index < (int)sizeof(symbols))
    return symbols[index];
  else
    return QwtSymbol::None;
}
