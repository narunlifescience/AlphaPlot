/***************************************************************************
    File                 : SymbolBox.h
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
#ifndef SYMBOLBOX_H
#define SYMBOLBOX_H

#include <QComboBox>
#include <qwt_symbol.h>

//! Plot symbol combo box
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
