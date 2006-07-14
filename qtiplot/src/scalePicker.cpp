/***************************************************************************
    File                 : scalePicker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Scale picker
                           
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
#include "scalePicker.h"

#include <qapplication.h>

#include <qwt_plot.h>
#include <qwt_scale_widget.h>
#include <qwt_text_label.h>
#include <QMouseEvent>

ScalePicker::ScalePicker(QwtPlot *plot):
    QObject(plot)
{
	movedGraph=FALSE;
	
    for ( uint i = 0; i < QwtPlot::axisCnt; i++ )
		{
        QwtScaleWidget *scale = (QwtScaleWidget *)plot->axisWidget(i);
        if ( scale )
            scale->installEventFilter(this);
		}
}

bool ScalePicker::eventFilter(QObject *object, QEvent *e)
{  
	if (!object->inherits("QwtScaleWidget"))
		return QObject::eventFilter(object, e);

	if ( e->type() == QEvent::MouseButtonDblClick )
    	{
		mouseDblClicked((const QwtScaleWidget *)object, ((QMouseEvent *)e)->pos());
        return TRUE;
    	}

	if ( e->type() == QEvent::MouseButtonPress)
    	{
		const QMouseEvent *me = (const QMouseEvent *)e;	
		if (me->button()==Qt::LeftButton)
			{
			presspos = me->pos();
			((QwtScaleWidget *)object)->setFocus();
			emit clicked();	

			if (plot()->margin() < 2 && plot()->lineWidth() < 2)
				{
				QRect r = ((const QwtScaleWidget *)object)->rect();
				r.addCoords(2, 2, -2, -2);
				if (!r.contains(me->pos()))
					emit highlightGraph();
				}
			return TRUE;
			}
		else if (me->button() == Qt::RightButton)
			{
			mouseRightClicked((const QwtScaleWidget *)object, me->pos());
			return TRUE;
			}
    	}
	
	if ( e->type() == QEvent::MouseMove)
    	{	
		const QMouseEvent *me = (const QMouseEvent *)e;			

		if ((presspos - me->pos()).manhattanLength() > QApplication::startDragDistance())
			{
			movedGraph=TRUE;
			emit moveGraph(me->pos());
			}

        return TRUE;
   	 }
	
	if ( e->type() == QEvent::MouseButtonRelease)
    	{
		if (movedGraph)
			{
			emit releasedGraph();
			movedGraph=FALSE;
			}
				
        return TRUE;
    	}
		
return QObject::eventFilter(object, e);
}

void ScalePicker::mouseDblClicked(const QwtScaleWidget *scale, const QPoint &pos) 
{
QRect rect = scaleRect(scale);

int margin = 1; // pixels tolerance
rect.setRect(rect.x() - margin, rect.y() - margin, rect.width() + 2 * margin, rect.height() +  2 * margin);

if ( rect.contains(pos) ) 
	emit axisDblClicked(scale->alignment());
else
	{// Click on the title
    switch(scale->alignment())   
        {
        case QwtScaleDraw::LeftScale:
            {
			emit yAxisTitleDblClicked();
            break;
            }
        case QwtScaleDraw::RightScale:
            {
			emit rightAxisTitleDblClicked();
            break;
            }
        case QwtScaleDraw::BottomScale:
            {
			emit xAxisTitleDblClicked();
            break;
            }
        case QwtScaleDraw::TopScale:
            {
			emit topAxisTitleDblClicked();
            break;
            }
		}
	}
}

void ScalePicker::mouseRightClicked(const QwtScaleWidget *scale, const QPoint &pos) 
{
QRect rect = scaleRect(scale);

int margin = 2; // pixels tolerance
rect.setRect(rect.x() - margin, rect.y() - margin, rect.width() + 2 * margin, rect.height() +  2 * margin);

if (rect.contains(pos)) 
	emit axisRightClicked(scale->alignment());
else
	emit axisTitleRightClicked(scale->alignment());
}

// The rect of a scale without the title
QRect ScalePicker::scaleRect(const QwtScaleWidget *scale) const
{
    const int bld = scale->margin();
    const int mjt = scale->scaleDraw()->majTickLength();
    const int sbd = scale->startBorderDist();
    const int ebd = scale->endBorderDist();
	
	int mlw, mlh;

    QRect rect;
    switch(scale->alignment())   
    {
        case QwtScaleDraw::LeftScale:
        {
			mlw=maxLabelWidth(scale);
			
			rect.setRect(scale->width() - bld - mjt-mlw, sbd,
                mjt+mlw, scale->height() - sbd - ebd);
			
            break;
        }
        case QwtScaleDraw::RightScale:
        {
			mlw=maxLabelWidth(scale);
			rect.setRect(bld, sbd,
                 mjt+mlw, scale->height() - sbd - ebd);
            break;
        }
        case QwtScaleDraw::BottomScale:
        {
			mlh=maxLabelHeight(scale);
			rect.setRect(sbd, bld, 
                scale->width() - sbd - ebd, mjt+mlh);
	        break;
        }
        case QwtScaleDraw::TopScale:
        {
			mlh=maxLabelHeight(scale);
			rect.setRect(sbd, scale->height() - bld - mjt-mlh, 
                scale->width() - sbd - ebd, mjt+mlh);
            break;
        }
    }
    return rect;
}

int ScalePicker::maxLabelWidth(const QwtScaleWidget *scale) const
{
const QwtScaleDraw *sd=scale->scaleDraw ();
return sd->maxLabelWidth (scale->font());
}

int ScalePicker::maxLabelHeight(const QwtScaleWidget *scale) const
{
const QwtScaleDraw *sd=scale->scaleDraw ();
return sd->maxLabelHeight (scale->font());
}

void ScalePicker::refresh()
{	
    for ( uint i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScaleWidget *scale = (QwtScaleWidget *)plot()->axisWidget(i);
        if ( scale )
            scale->installEventFilter(this);
    }
}

TitlePicker::TitlePicker(QwtPlot *plot):
    QObject(plot)
{
movedGraph=FALSE;

title = (QwtTextLabel *)plot->titleLabel();
title->setFocusPolicy(Qt::StrongFocus);
if (title)
	title->installEventFilter(this);
}

bool TitlePicker::eventFilter(QObject *object, QEvent *e)
{
	if (object != (QObject *)title)
		return FALSE;
	
    if ( object->inherits("QwtTextLabel") && e->type() == QEvent::MouseButtonDblClick)
		{
		emit doubleClicked();
        return TRUE;
		}

	if ( object->inherits("QwtTextLabel") &&  e->type() == QEvent::MouseButtonPress )
		{
		const QMouseEvent *me = (const QMouseEvent *)e;	
		presspos = me->pos();
		emit clicked();

		if (me->button()==Qt::RightButton)
			emit showTitleMenu();

		QwtPlot *plot = (QwtPlot *)title->parent();
		if (plot->margin() < 2 && plot->lineWidth() < 2)
			{
			QRect r = title->rect();
			r.addCoords(2, 2, -2, -2);
			if (!r.contains(me->pos()))
				emit highlightGraph();
			}

		return TRUE;
		}

	if ( object->inherits("QwtTextLabel") &&  e->type() == QEvent::MouseMove)
		{	
		const QMouseEvent *me = (const QMouseEvent *)e;		
		if ((presspos - me->pos()).manhattanLength() > QApplication::startDragDistance())
			{			
			movedGraph=TRUE;
			emit moveGraph(me->pos());
			}
        return TRUE;
		}
	
	if ( object->inherits("QwtTextLabel") && e->type() == QEvent::MouseButtonRelease)
		{
		const QMouseEvent *me = (const QMouseEvent *)e;
		if (me->button()== Qt::LeftButton)
			{
			emit clicked();
			if (movedGraph)
				{
				emit releasedGraph();
				movedGraph=FALSE;
				}
        	return TRUE;
			}
		}

	if ( object->inherits("QwtTextLabel") && 
        e->type() == QEvent::KeyPress)
		{
		switch (((const QKeyEvent *)e)->key()) 
			{
			case Qt::Key_Delete: 
			emit removeTitle();	
            return TRUE;
			}
		}

    return QObject::eventFilter(object, e);
}
