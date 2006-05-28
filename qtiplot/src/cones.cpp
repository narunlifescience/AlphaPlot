/***************************************************************************
    File                 : cones.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
#include <math.h>
#include "qwt3d_color.h"
#include "qwt3d_plot.h"
#include "cones.h"

using namespace Qwt3D;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   Cone3D (code from Cone class  in QwtPlot3D library with modified destructor)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Cone3D::Cone3D()
{
	hat      = gluNewQuadric();
	disk     = gluNewQuadric();

  configure(0, 3);
}

Cone3D::Cone3D(double rad, unsigned quality)
{
	hat      = gluNewQuadric();
	disk     = gluNewQuadric();

  configure(rad, quality);
}

Cone3D::~Cone3D()
{
}

void Cone3D::configure(double rad, unsigned quality)
{
  plot = 0;
  radius_ = rad;
  quality_ = quality;
  oldstate_ = GL_FALSE;

	gluQuadricDrawStyle(hat,GLU_FILL);
	gluQuadricNormals(hat,GLU_SMOOTH);
	gluQuadricOrientation(hat,GLU_OUTSIDE);
	gluQuadricDrawStyle(disk,GLU_FILL);
	gluQuadricNormals(disk,GLU_SMOOTH);
	gluQuadricOrientation(disk,GLU_OUTSIDE);
}

void Cone3D::draw(Qwt3D::Triple const& pos)
{  
	RGBA rgba = (*plot->dataColor())(pos);
  glColor4d(rgba.r,rgba.g,rgba.b,rgba.a);

  GLint mode;
	glGetIntegerv(GL_MATRIX_MODE, &mode);
	glMatrixMode( GL_MODELVIEW );
  glPushMatrix();

  glTranslatef(pos.x, pos.y, pos.z);

  gluCylinder(hat, 0.0, radius_, radius_*2, quality_, 1);
  glTranslatef(0, 0, radius_*2);
	gluDisk(disk, 0.0, radius_, quality_, 1);

  glPopMatrix();
	glMatrixMode(mode);
}
