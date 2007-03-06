/***************************************************************************
    File                 : QwtSupersciptsScaleDraw.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Superscript extension of QwtScaleDraw
                           
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
#include <stdio.h>
#include <qpainter.h>
#include <qfontmetrics.h>

#include <qwt_text.h>
#include <qwt_scldraw.h>
#include <qwt_layout_metrics.h>

//! Superscript extension of QwtScaleDraw
class QwtSupersciptsScaleDraw: public QwtScaleDraw
{
public:
	QwtSupersciptsScaleDraw(const QFont& fnt, const QColor& col){d_font = fnt; d_color = col;};
	~QwtSupersciptsScaleDraw(){};

	virtual QString label(double value) const
	{
	char f;
	int prec, fieldwidth;
	labelFormat  	(f, prec, fieldwidth);
		
	QString txt;
	txt.setNum ( value, 'e', prec);
		
	QStringList list = txt.split( "e", QString::SkipEmptyParts);
	if (list[0].toDouble() == 0.0)
		return "0";
	
	QString s= list[1];
	int l = s.length();
	QChar sign = s[0];
	
	s.remove (sign);
		
	while (l>1 && s.startsWith ("0", false))
		{
		s.remove ( 0, 1 );
		l = s.length();
		}
		
	if (sign == '-')
		s.prepend(sign);
	
	return list[0] + "x10<sup>" + s + "</sup>";
	//return list[0]+"x10<sup><font size=+1>"+s+"</font></sup>";
	};
	
//! Draws the number label for a major scale tick
void drawLabel(QPainter *p, double val) const
{
    QPoint pos;
    int alignment;
    double rotation;
    labelPlacement(QFontMetrics(d_font), val, pos, alignment, rotation);

	if ( alignment )
    {
    const QString txt = label(val);
    if ( !txt.isEmpty() )
        {
            QWMatrix m = labelWorldMatrix(d_font, pos, alignment, rotation, txt);
			
            p->save();
#ifndef QT_NO_TRANSFORMATIONS
            p->setWorldMatrix(m, true);
#else
            p->translate(m.dx(), m.dy());
#endif
           
			QwtText *ltxt = labelText(val);
			QRect br = ltxt->boundingRect();
			QRect rect = QRect (0, 0, br.width(), br.height());
			
			ltxt->draw (p, rect);
			delete ltxt;
			
			p->restore();
        }
	}
	};

	QwtText *labelText(double value) const
	{
	const QString lbl = label(value);
	return QwtText::makeText(lbl, labelAlignment(), d_font, d_color);
	};
	
	QRect labelBoundingRect(const QFontMetrics &fm, double val) const
	{
	char f;
	int prec, fieldwidth;
	labelFormat  	(f, prec, fieldwidth);
		
    QString zeroString;
    if ( fieldwidth > 0 )
        zeroString.fill('0', fieldwidth);

    const QString lbl = label(val);

    const QString &txt = fm.width(zeroString) > fm.width(lbl) 
        ? zeroString : lbl;
    if ( txt.isEmpty() )
        return QRect(0, 0, 0, 0);

	QwtText *ltxt = labelText(val);
	
    QRect br;
    QPoint pos;
    int alignment;
    double rotation;

    labelPlacement(fm, val, pos, alignment, rotation);
    if ( alignment )
    {
       const int w = ltxt->boundingRect().width();
       const int h = ltxt->boundingRect().height();
				
        QWMatrix m = labelWorldMatrix(fm, pos, alignment, rotation, lbl);
        br = QwtMetricsMap::translate(m, QRect(0, 0, w, h));
        br.moveBy(-pos.x(), -pos.y());
    }
	
	delete ltxt;
    return br;
	};
	
/*void labelPlacement( const QFontMetrics &fm, double val, 
    QPoint &pos, int &alignment, double &rotation) const
{

    QwtScaleDraw::labelPlacement(fm, val, pos, alignment, rotation);
   
	int align = 0;
    switch(orientation ())
    {
        case Right:
        {
            align = Qt::AlignLeft | Qt::AlignVCenter;
            break;
        }
        case Left:
        {
            align = Qt::AlignRight | Qt::AlignVCenter;
            break;
        }
        case Bottom:
        {
            align = Qt::AlignHCenter | Qt::AlignBottom;
            break;
        }
        case Top:
        {
            align = Qt::AlignHCenter | Qt::AlignTop;
            break;
        }
        case Round:
        {
               break;
        }
	}
			
    alignment = align;
};*/

//! Return the world matrix for painting the label 
 QWMatrix labelWorldMatrix(const QFontMetrics &,
    const QPoint &pos, int alignment, 
#ifdef QT_NO_TRANSFORMATIONS
    double,
#else
    double rotation, 
#endif
    const QString &txt) const
	{
    QwtText *ltxt = QwtText::makeText(txt, alignment, d_font, d_color);
    QRect br = ltxt->boundingRect();
			
	const int w = br.width();
    const int h = br.height();
    	
    int x, y;
    if ( alignment & Qt::AlignLeft )
        x = -w;
    else if ( alignment & Qt::AlignRight )
        x = 0 - w % 2;
    else // Qt::AlignHCenter
        x = -(w / 2);
	
	if ( alignment & Qt::AlignTop )
        y =  - h ;
    else if ( alignment & Qt::AlignBottom )
        y = 0;
    else // Qt::AlignVCenter
        y = - (h/2);
	
    QWMatrix m;
    m.translate(pos.x(), pos.y());
#ifndef QT_NO_TRANSFORMATIONS
    m.rotate(rotation);
#endif
    m.translate(x, y);

	delete ltxt;
    return m;
 };
	
private:
	QFont d_font;
	QColor d_color;
};
