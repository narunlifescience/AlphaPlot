/***************************************************************************
    File                 : ColorBox.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2007 by Ion Vasilief, Alex Kargovsky, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, kargovsky*yumr.phys.msu.su, thzs*gmx.net
    Description          : A combo box to select a standard color

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
#include "ColorBox.h"

#include <QPixmap>
#include <QPainter>
#include <algorithm>

const int colorsCnt = 24;

const QColor ColorBox::colors[] = {
  QColor(Qt::black),
  QColor(Qt::red),
  QColor(Qt::green),
  QColor(Qt::blue),
  QColor(Qt::cyan),
  QColor(Qt::magenta),
  QColor(Qt::yellow),
  QColor(Qt::darkYellow),
  QColor("darkBlue"),
  QColor("darkMagenta"),
  QColor("darkRed"),
  QColor("darkGreen"),
  QColor("darkCyan"),
  QColor("#0000A0"),
  QColor("#FF8000"),
  QColor("#8000FF"),
  QColor("#FF0080"),
  QColor(Qt::white),
  QColor("lightGray"),
  QColor("#808080"),
  QColor("#FFFF80"),
  QColor("#80FFFF"),
  QColor("#FF80FF"),
  QColor("#404040"),
  QColor("darkGray"),
};

ColorBox::ColorBox(QWidget *parent) : QComboBox(parent)
{
    setEditable(false);
	init();
}

void ColorBox::init()
{
	QPixmap icon = QPixmap(28, 16);
	QRect r = QRect(0, 0, 27, 15);

	icon.fill ( QColor (Qt::black) );
	this->addItem(icon, tr( "black" ) );
	
	QPainter p;
	p.begin(&icon);
	p.setBrush(QBrush(QColor (Qt::red)));
	p.drawRect(r);
	this->addItem(icon, tr( "red" ) );
	
	p.setBrush(QBrush(QColor (Qt::green)));
	p.drawRect(r);
	this->addItem(icon, tr( "green" ) );
	
	p.setBrush(QBrush(QColor (Qt::blue)));
	p.drawRect(r);
	this->addItem(icon, tr( "blue" ) );
	
	p.setBrush(QBrush(QColor (Qt::cyan)));
	p.drawRect(r);
	this->addItem(icon, tr( "cyan" ) );
	
	p.setBrush(QBrush(QColor (Qt::magenta)));
	p.drawRect(r);
	this->addItem(icon, tr( "magenta" ) );
	
	p.setBrush(QBrush(QColor (Qt::yellow)));
	p.drawRect(r);
	this->addItem(icon, tr( "yellow" ) );
	
	p.setBrush(QBrush(QColor (Qt::darkYellow)));
	p.drawRect(r);
	this->addItem(icon, tr( "dark yellow" ) );
		
	p.setBrush(QBrush(QColor (Qt::darkBlue)));
	p.drawRect(r);
	this->addItem(icon, tr( "navy" ) );
	
	p.setBrush(QBrush(QColor (Qt::darkMagenta)));
	p.drawRect(r);
	this->addItem(icon, tr( "purple" ) );
	
	p.setBrush(QBrush(QColor (Qt::darkRed)));
	p.drawRect(r);
	this->addItem(icon, tr( "wine" ) );
	
	p.setBrush(QBrush(QColor (Qt::darkGreen)));
	p.drawRect(r);
	this->addItem(icon, tr( "olive" ) );
	
	p.setBrush(QBrush(QColor (Qt::darkCyan)));
	p.drawRect(r);
	this->addItem(icon, tr( "dark cyan" ) );

	p.setBrush(QBrush(QColor ("#0000A0")));
	p.drawRect(r);
	this->addItem(icon, tr( "royal" ) );
	
	p.setBrush(QBrush(QColor("#FF8000")));
	p.drawRect(r);
	this->addItem(icon, tr( "orange" ) );
	
	p.setBrush(QBrush(QColor("#8000FF")));
	p.drawRect(r);
	this->addItem(icon, tr( "violet" ) );
	
	p.setBrush(QBrush(QColor("#FF0080")));
	p.drawRect(r);
	this->addItem(icon, tr( "pink" ) );

	p.setBrush(QBrush(QColor(Qt::white)));
	p.drawRect(r);
	this->addItem(icon,tr( "white" ) );
	
	p.setBrush(QBrush(QColor(Qt::lightGray)));
	p.drawRect(r);
	this->addItem(icon, tr( "light gray" ) );

	p.setBrush(QBrush(QColor("#808080")));
	p.drawRect(r);
	this->addItem(icon, tr( "gray" ) );
	
	p.setBrush(QBrush(QColor("#FFFF80")));
	p.drawRect(r);
	this->addItem(icon, tr( "light yellow" ) );
	
	p.setBrush(QBrush(QColor("#80FFFF")));
	p.drawRect(r);
	this->addItem(icon, tr( "light cyan" ) );
	
	p.setBrush(QBrush(QColor("#FF80FF")));
	p.drawRect(r);
	this->addItem(icon, tr( "light magenta" ) );

	p.setBrush(QBrush(QColor(Qt::darkGray)));
	p.drawRect(r);
	this->addItem(icon, tr( "dark gray" ) );
	p.end();
}

void ColorBox::setColor(const QColor& c)
{
	const QColor *ite = std::find(colors, colors + sizeof(colors), c);
	if (ite == colors + sizeof(colors))
		this->setCurrentIndex(0); // default color is black.
	else
		this->setCurrentIndex(ite - colors);
}

QColor ColorBox::color() const
{
	size_t i = this->currentIndex();
	if (i < sizeof(colors))
		return colors[this->currentIndex()];
	else
		return QColor(Qt::black); // default color is black.
}

int ColorBox::colorIndex(const QColor& c)
{
	const QColor *ite = std::find(colors, colors + sizeof(colors), c);
	return (ite - colors);
}

QColor ColorBox::color(int colorIndex)
{
	if (colorIndex < (int)sizeof(colors))
		return colors[colorIndex];
	else
		return QColor(Qt::black); // default color is black.
}

bool ColorBox::isValidColor(const QColor& color)
{
	for (int i = 0; i < colorsCnt; i++)
	{
        if (color == colors[i])
            return true;
    }
    return false;
}
