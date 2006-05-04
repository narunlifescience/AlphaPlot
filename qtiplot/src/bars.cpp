#include <qbitmap.h>

#include <qwt3d_color.h>
#include <qwt3d_plot.h>
#include "bars.h"

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
