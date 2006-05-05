#include "scalePicker.h"
#include <qpainter.h>
#include <qwt_plot.h>
#include <qwt_scale.h>

#include <qmessagebox.h>

ScalePicker::ScalePicker(QwtPlot *plot):
    QObject(plot)
{
	movedGraph=FALSE;
	
    for ( uint i = 0; i < QwtPlot::axisCnt; i++ )
		{
        QwtScale *scale = (QwtScale *)plot->axis(i);
        if ( scale )
            scale->installEventFilter(this);
		}
}

bool ScalePicker::eventFilter(QObject *object, QEvent *e)
{  
	if ( object->inherits("QwtScale") && e->type() == QEvent::MouseButtonDblClick)
    	{
		mouseDblClicked((const QwtScale *)object, ((QMouseEvent *)e)->pos());
        return TRUE;
    	}

	if ( object->inherits("QwtScale") && e->type() == QEvent::MouseButtonPress)
    	{
		const QMouseEvent *me = (const QMouseEvent *)e;	
		if (me->button()==QEvent::LeftButton)
			{
			emit clicked();	

			if (plot()->margin() < 2 && plot()->lineWidth() < 2)
				{
				QRect r = ((const QwtScale *)object)->rect();
				r.addCoords(2, 2, -2, -2);
				if (!r.contains(me->pos()))
					emit highlightGraph();
				}
			return TRUE;
			}
		else if (me->button() == QEvent::RightButton)
			{
			mouseRightClicked((const QwtScale *)object, me->pos());
			return TRUE;
			}
    	}
	
	if ( object->inherits("QwtScale") && e->type() == QEvent::MouseMove)
    	{	
		const QMouseEvent *me = (const QMouseEvent *)e;			

		movedGraph=TRUE;
		emit moveGraph(me->pos());

        return TRUE;
   	 }
	
	if ( object->inherits("QwtScale") && e->type() == QEvent::MouseButtonRelease)
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

void ScalePicker::mouseDblClicked(const QwtScale *scale, const QPoint &pos) 
{
QRect rect = scaleRect(scale);

int margin = 2; // pixels tolerance
rect.setRect(rect.x() - margin, rect.y() - margin, rect.width() + 2 * margin, rect.height() +  2 * margin);

if ( rect.contains(pos) ) 
	{
	emit axisDblClicked(scale->position());
	}
else
	{// Click on the title
    switch(scale->position())   
        {
        case QwtScale::Left:
            {
			emit yAxisTitleDblClicked();
            break;
            }
        case QwtScale::Right:
            {
			emit rightAxisTitleDblClicked();
            break;
            }
        case QwtScale::Bottom:
            {
			emit xAxisTitleDblClicked();
            break;
            }
        case QwtScale::Top:
            {
			emit topAxisTitleDblClicked();
            break;
            }
		}
	}
}

void ScalePicker::mouseRightClicked(const QwtScale *scale, const QPoint &pos) 
{
QRect rect = scaleRect(scale);

int margin = 2; // pixels tolerance
rect.setRect(rect.x() - margin, rect.y() - margin, rect.width() + 2 * margin, rect.height() +  2 * margin);

if (rect.contains(pos)) 
	{
	if (scale->position() == QwtScale::Left || scale->position() == QwtScale::Right)
			emit axisRightClicked(scale->position());
	else if (scale->position() == QwtScale::Top)
		emit axisRightClicked(QwtScale::Bottom);
	else if (scale->position() == QwtScale::Bottom)
		emit axisRightClicked(QwtScale::Top);
	}
else
	{
	if (scale->position() == QwtScale::Left || scale->position() == QwtScale::Right)
		emit axisTitleRightClicked(scale->position());
	else if (scale->position() == QwtScale::Top)
		emit axisTitleRightClicked(QwtScale::Bottom);
	else if (scale->position() == QwtScale::Bottom)
		emit axisTitleRightClicked(QwtScale::Top);
	}
}

// The rect of a scale without the title
QRect ScalePicker::scaleRect(const QwtScale *scale) const
{
    const int bld = scale->baseLineDist();
    const int mjt = scale->scaleDraw()->majTickLength();
    const int sbd = scale->startBorderDist();
    const int ebd = scale->endBorderDist();
	
	int mlw, mlh;

    QRect rect;
    switch(scale->position())   
    {
        case QwtScale::Left:
        {
			mlw=maxLabelWidth(scale);
			
			rect.setRect(scale->width() - bld - mjt-mlw, sbd,
                mjt+mlw, scale->height() - sbd - ebd);
			
            break;
        }
        case QwtScale::Right:
        {
			mlw=maxLabelWidth(scale);
			rect.setRect(bld, sbd,
                 mjt+mlw, scale->height() - sbd - ebd);
            break;
        }
        case QwtScale::Bottom:
        {
			mlh=maxLabelHeight(scale);
			rect.setRect(sbd, bld, 
                scale->width() - sbd - ebd, mjt+mlh);
	        break;
        }
        case QwtScale::Top:
        {
			mlh=maxLabelHeight(scale);
			rect.setRect(sbd, scale->height() - bld - mjt-mlh, 
                scale->width() - sbd - ebd, mjt+mlh);
            break;
        }
    }
    return rect;
}

int ScalePicker::maxLabelWidth(const QwtScale *scale) const
{
	QFontMetrics fm(scale->font());
	const QwtScaleDraw * scaleDraw=scale->scaleDraw ();
	const QwtScaleDiv 	scaleDiv=scaleDraw->scaleDiv();
	const double step_eps = 1.0e-6;
    int maxWidth = 0;

    for (uint i = 0; i < scaleDiv.majCnt(); i++)
    {
        double val = scaleDiv.majMark(i);

        // correct rounding errors if val = 0
        if ((!scaleDiv.logScale()) 
            && (qwtAbs(val) < step_eps * qwtAbs(scaleDiv.majStep())))
        {
            val = 0.0;
        }

        const int w = fm.boundingRect(scaleDraw->label(val)).width();
        if ( w > maxWidth )
            maxWidth = w;
    }

    return maxWidth;
}

int ScalePicker::maxLabelHeight(const QwtScale *scale) const
{
	QFontMetrics fm(scale->font());
	const QwtScaleDraw * scaleDraw=scale->scaleDraw ();
	const QwtScaleDiv 	scaleDiv=scaleDraw->scaleDiv();
   
	int maxHeight = 0;

    for (uint i = 0; i < scaleDiv.majCnt(); i++)
    {
        double val = scaleDiv.majMark(i);

        const int w = fm.boundingRect(scaleDraw->label(val)).height();
        if ( w > maxHeight )
            maxHeight = w;
    }
	
    return maxHeight;
}

void ScalePicker::refresh()
{	
    for ( uint i = 0; i < QwtPlot::axisCnt; i++ )
    {
        QwtScale *scale = (QwtScale *)plot()->axis(i);
        if ( scale )
            scale->installEventFilter(this);
    }
}

TitlePicker::TitlePicker(QwtPlot *plot):
    QObject(plot)
{
movedGraph=FALSE;

title = (QLabel *)plot->titleLabel();
title->setFocusPolicy(QWidget::StrongFocus);
if (title)
	title->installEventFilter(this);
}

bool TitlePicker::eventFilter(QObject *object, QEvent *e)
{
	if (object != (QObject *)title)
		return FALSE;
	
    if ( object->inherits("QLabel") && e->type() == QEvent::MouseButtonDblClick)
		{
		emit doubleClicked();
        return TRUE;
		}

	if ( object->inherits("QLabel") &&  e->type() == QEvent::MouseButtonPress )
		{
		emit clicked();

		const QMouseEvent *me = (const QMouseEvent *)e;	
		if (me->button()==QEvent::RightButton)
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

	if ( object->inherits("QLabel") &&  e->type() == QEvent::MouseMove)
		{	
		const QMouseEvent *me = (const QMouseEvent *)e;			
		movedGraph=TRUE;
		emit moveGraph(me->pos());

        return TRUE;
		}
	
	if ( object->inherits("QLabel") && e->type() == QEvent::MouseButtonRelease)
		{
		const QMouseEvent *me = (const QMouseEvent *)e;
		if (me->button()== QEvent::LeftButton)
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

	if ( object->inherits("QLabel") && 
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
