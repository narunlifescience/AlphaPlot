/***************************************************************************
    File                 : Bar.cpp
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
#include <qbitmap.h>

#include <qwt3d_color.h>
#include <qwt3d_plot.h>
#include "Bar.h"

using namespace Qwt3D;

/////////////////////////////////////////////////////////////////
//
//  3D Bars  (modified enrichement example from QwtPlot3D archive)
//
/////////////////////////////////////////////////////////////////

Bar::Bar()
{
  configure(0);
}

Bar::Bar(double rad)
{
  configure(rad);
}

void Bar::configure(double rad)
{
  plot = 0;
  radius_ = rad;
}

void Bar::drawBegin()
{  
  diag_ = (plot->hull().maxVertex-plot->hull().minVertex).length() * radius_;
  glLineWidth( plot->meshLineWidth() );
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1,1);
}

void Bar::drawEnd()
{
}

void Bar::draw(Qwt3D::Triple const& pos)
{
  GLdouble minz = plot->hull().minVertex.z;

  RGBA rgbat = (*plot->dataColor())(pos);
	RGBA rgbab = (*plot->dataColor())(pos.x, pos.y, minz);
	
  glBegin(GL_QUADS);
    glColor4d(rgbab.r,rgbab.g,rgbab.b,rgbab.a);
    glVertex3d(pos.x-diag_,pos.y-diag_,minz);
    glVertex3d(pos.x+diag_,pos.y-diag_,minz);
    glVertex3d(pos.x+diag_,pos.y+diag_,minz);
    glVertex3d(pos.x-diag_,pos.y+diag_,minz);

      glColor4d(rgbat.r,rgbat.g,rgbat.b,rgbat.a);
	
    glVertex3d(pos.x-diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x+diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y+diag_,pos.z);

    glColor4d(rgbab.r,rgbab.g,rgbat.b,rgbab.a);
    glVertex3d(pos.x-diag_,pos.y-diag_,minz);
    glVertex3d(pos.x+diag_,pos.y-diag_,minz);
    glColor4d(rgbat.r,rgbat.g,rgbat.b,rgbat.a);
    glVertex3d(pos.x+diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y-diag_,pos.z);

    glColor4d(rgbab.r,rgbab.g,rgbat.b,rgbab.a);
    glVertex3d(pos.x-diag_,pos.y+diag_,minz);
    glVertex3d(pos.x+diag_,pos.y+diag_,minz);
    glColor4d(rgbat.r,rgbat.g,rgbat.b,rgbat.a);
    glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y+diag_,pos.z);

    glColor4d(rgbab.r,rgbab.g,rgbat.b,rgbab.a);
    glVertex3d(pos.x-diag_,pos.y-diag_,minz);
    glVertex3d(pos.x-diag_,pos.y+diag_,minz);
    glColor4d(rgbat.r,rgbat.g,rgbat.b,rgbat.a);
    glVertex3d(pos.x-diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y-diag_,pos.z);

    glColor4d(rgbab.r,rgbab.g,rgbat.b,rgbab.a);
    glVertex3d(pos.x+diag_,pos.y-diag_,minz);
    glVertex3d(pos.x+diag_,pos.y+diag_,minz);
    glColor4d(rgbat.r,rgbat.g,rgbat.b,rgbat.a);
    glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x+diag_,pos.y-diag_,pos.z);
  glEnd();

	Qwt3D::RGBA meshCol=plot->meshColor();//using mesh color to draw the lines
    glColor3d(meshCol.r,meshCol.g,meshCol.b);

  glBegin(GL_LINES);
    glVertex3d(pos.x-diag_,pos.y-diag_,minz); glVertex3d(pos.x+diag_,pos.y-diag_,minz);
    glVertex3d(pos.x-diag_,pos.y-diag_,pos.z); glVertex3d(pos.x+diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y+diag_,pos.z); glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y+diag_,minz); glVertex3d(pos.x+diag_,pos.y+diag_,minz);

    glVertex3d(pos.x-diag_,pos.y-diag_,minz); glVertex3d(pos.x-diag_,pos.y+diag_,minz);
    glVertex3d(pos.x+diag_,pos.y-diag_,minz); glVertex3d(pos.x+diag_,pos.y+diag_,minz);
    glVertex3d(pos.x+diag_,pos.y-diag_,pos.z); glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y-diag_,pos.z); glVertex3d(pos.x-diag_,pos.y+diag_,pos.z);

    glVertex3d(pos.x-diag_,pos.y-diag_,minz); glVertex3d(pos.x-diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x+diag_,pos.y-diag_,minz); glVertex3d(pos.x+diag_,pos.y-diag_,pos.z);
    glVertex3d(pos.x+diag_,pos.y+diag_,minz); glVertex3d(pos.x+diag_,pos.y+diag_,pos.z);
    glVertex3d(pos.x-diag_,pos.y+diag_,minz); glVertex3d(pos.x-diag_,pos.y+diag_,pos.z);
  glEnd();
}
