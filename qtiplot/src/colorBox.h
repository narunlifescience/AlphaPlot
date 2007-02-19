/***************************************************************************
    File                 : colorBox.h
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
#ifndef COLORBOX_H
#define COLORBOX_H

#include <QComboBox>

//! A combo box to select a standard color
class ColorBox : public QComboBox
{
	Q_OBJECT

public:
	//! Constructor
	/**
	 * \param rw flag: box editable or not
	 * \param parent parent widget
	 */
	ColorBox(bool rw, QWidget *parent);
	//! Constructor
	/**
	 * \param parent parent widget
	 */
	ColorBox(QWidget *parent);
	//! Set the current color
	void setColor(const QColor& c);
	//! Return the current color
	QColor color() const;
		
	//! Return the index for a given color
	static int colorIndex(const QColor& c);
	//! Return the color at index 'colorindex'
	static QColor color(int colorIndex);
    //! Returns TRUE if the color is included in the color box, otherwise returns FALSE.
    static bool isValidColor(const QColor& color);

protected:
	//! Internal initialization function
	void init();
	//! Array containing the 16 colors
	static const QColor colors[];
};

#endif

