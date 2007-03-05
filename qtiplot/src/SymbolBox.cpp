/***************************************************************************
    File                 : symbolBox.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Plot symbol combo box
                           
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#include "SymbolBox.h"
#include <algorithm>

#include <qpixmap.h>
#include <qpainter.h>

const QwtSymbol::Style SymbolBox::symbols[] = {
  QwtSymbol::NoSymbol,
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
  QwtSymbol::HLine,
  QwtSymbol::VLine,
  QwtSymbol::Star1, 
  QwtSymbol::Star2, 
  QwtSymbol::Hexagon
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
	icon.fill ( QColor (Qt::gray) );
	const QRect r= QRect(0, 0, 14, 14);
	QPainter p(&icon);
	p.setBackgroundColor(QColor(Qt::gray));
	QwtSymbol symb;
	p.setBrush(QBrush(QColor(Qt::white)));

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

	symb.setStyle (QwtSymbol::HLine);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Horizontal Line"));

	symb.setStyle (QwtSymbol::VLine);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Vertical Line"));

	symb.setStyle (QwtSymbol::Star1);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Star 1"));

	symb.setStyle (QwtSymbol::Star2);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Star 2"));

	symb.setStyle (QwtSymbol::Hexagon);
	p.eraseRect ( r );
	symb.draw(&p, r);
    this->insertItem(icon,tr("Hexagon"));

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
    return QwtSymbol::NoSymbol;
}

int SymbolBox::symbolIndex(const QwtSymbol::Style& style)
{
  const QwtSymbol::Style*ite = std::find(symbols, symbols + sizeof(symbols), style);
  if (ite == symbols + sizeof(symbols))
    return 0; 
  else
    return (ite - symbols);
}

QwtSymbol::Style SymbolBox::style(int index)
{
  if (index < (int)sizeof(symbols))
    return symbols[index];
  else
    return QwtSymbol::NoSymbol;
}
