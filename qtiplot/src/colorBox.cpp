/***************************************************************************
    File                 : colorBox.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
#include "colorBox.h"

#include <QPixmap>
#include <algorithm>

const int colorsCnt = 16;

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
  QColor(Qt::white),
  QColor("lightGray"),
  QColor("darkGray"),
};

ColorBox::ColorBox(QWidget *parent) : QComboBox(parent)
{
    setEditable(false);
	init();
}

void ColorBox::init()
{
	QPixmap icon = QPixmap(28, 14);

	icon.fill ( QColor (Qt::black) );
	this->addItem(icon, tr( "black" ) );
	icon.fill (QColor (Qt::red) );
	this->addItem(icon, tr( "red" ) );
	icon.fill (QColor (Qt::green) );
	this->addItem(icon, tr( "green" ) );
	icon.fill (QColor (Qt::blue) );
	this->addItem(icon, tr( "blue" ) );
	icon.fill (QColor (Qt::cyan) );
	this->addItem(icon, tr( "cyan" ) );
	icon.fill (QColor (Qt::magenta) );
	this->addItem(icon, tr( "magenta" ) );
	icon.fill (QColor (Qt::yellow) );
	this->addItem(icon, tr( "yellow" ) );
	icon.fill (QColor (Qt::darkYellow) );
	this->addItem(icon, tr( "dark yellow" ) );
	icon.fill (QColor (Qt::darkBlue) );
	this->addItem(icon, tr( "navy" ) );
	icon.fill (QColor (Qt::darkMagenta) );
	this->addItem(icon, tr( "purple" ) );
	icon.fill (QColor (Qt::darkRed) );
	this->addItem(icon, tr( "wine" ) );
	icon.fill (QColor (Qt::darkGreen) );
	this->addItem(icon, tr( "olive" ) );
	icon.fill (QColor (Qt::darkCyan) );
	this->addItem(icon, tr( "dark cyan" ) );
	icon.fill (QColor (Qt::white) );
	this->addItem(icon,tr( "white" ) );
	icon.fill (QColor (Qt::lightGray) );
	this->addItem(icon, tr( "light gray" ) );
	icon.fill (QColor (Qt::darkGray) );
	this->addItem(icon, tr( "dark gray" ) );    
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


