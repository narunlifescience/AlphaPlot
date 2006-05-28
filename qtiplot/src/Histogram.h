/***************************************************************************
    File                 : Histogram.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Histogram class
                           
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
#include "BarCurve.h"

//! Histogram class
class QwtHistogram: public QwtBarCurve
{
public:
	QwtHistogram(QwtPlot *parent, const char *name=0);

	void copy(const QwtHistogram *h);

	virtual void draw(QPainter *painter,const QwtScaleMap &xMap, 
		const QwtScaleMap &yMap, int from, int to);

	QwtDoubleRect boundingRect() const;

	void setBinning(bool autoBin, double begin, double end, double size);
	bool autoBinning(){return d_autoBin;};
	double begin(){return d_begin;};
	double end(){return d_end;};
	double binSize(){return d_binSize;};

private:
	bool d_autoBin;
	double d_binSize, d_begin, d_end;
};

