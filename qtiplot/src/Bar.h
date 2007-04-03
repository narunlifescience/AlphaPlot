/***************************************************************************
    File                 : Bar.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : 3D bars (modifed enrichment from QwtPlot3D)
                           
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
#ifndef BARS_H
#define BARS_H

#include <qwt3d_plot.h>

//! 3D bars (modifed enrichment from QwtPlot3D)
class Bar : public Qwt3D::VertexEnrichment
{
public:
  Bar();
  Bar(double rad);

  Qwt3D::Enrichment* clone() const {return new Bar(*this);}
  
  void configure(double rad);
  void drawBegin();
  void drawEnd();
  void draw(Qwt3D::Triple const&);

private:
  double radius_;
  double diag_;
};


#endif
