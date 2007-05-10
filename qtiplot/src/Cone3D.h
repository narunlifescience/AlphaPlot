/***************************************************************************
    File                 : Cone3D.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : 3D cone class (code from Cone class  in QwtPlot3D library with modified destructor)
                           
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
#ifndef MYCONES_H
#define MYCONES_H

#include <qwt3d_plot.h>

//! 3D cone class (based on QwtPlot3D)
class Cone3D : public Qwt3D::VertexEnrichment
{
public:
  Cone3D();
  Cone3D(double rad, unsigned quality);
  ~Cone3D();

  Qwt3D::Enrichment* clone() const {return new Cone3D(*this);}
  
  void configure(double rad, unsigned quality);
  void draw(Qwt3D::Triple const&);

private:
  	GLUquadricObj *hat;
	GLUquadricObj *disk;
	unsigned quality_;
	double radius_;
	GLboolean oldstate_;
};

#endif
