/***************************************************************************
    File                 : scaleDraws.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Extension to QwtScaleDraw
                           
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
#ifndef SCALEDRAWS_H
#define SCALEDRAWS_H

#include "parser.h"

#include <qpainter.h>
#include <qdatetime.h>
#include <qmessagebox.h>

#include <qwt_painter.h>
#include <qwt_scale_draw.h>


//! Extension to QwtScaleDraw
class ScaleDraw: public QwtScaleDraw
{
public:		
	ScaleDraw(const QString& s = QString::null):
	d_fmt('g'),
    d_prec(4),
	formula_string (s)
	{};

	virtual ~ScaleDraw(){};
		
	QString formulaString() {return formula_string;};
	void setFormulaString(const QString& formula) {formula_string = formula;};

	double transformValue(double value) const
	{
	if (!formula_string.isEmpty())
		{
		double lbl=0.0;
		try
			{
			MyParser parser;
			if (formula_string.contains("x"))
				parser.DefineVar("x", &value);
			else if (formula_string.contains("y"))
				parser.DefineVar("y", &value);

			parser.SetExpr(formula_string.ascii());
			lbl=parser.Eval();
			}
		catch(mu::ParserError &)
			{
			return 0;
			}

		return lbl;
		}
	else
		return value;
	};

	virtual QwtText label(double value) const
	{
	return QwtText(QString::number(transformValue(value), d_fmt, d_prec));
	};

/*!
  \brief Set the number format for the major scale labels

  Format character and precision have the same meaning as for
  sprintf().
  \param f format character 'e', 'f', 'g' 
  \param prec
    - for 'e', 'f': the number of digits after the radix character (point)
    - for 'g': the maximum number of significant digits

  \sa labelFormat()
*/
void setLabelFormat(char f, int prec)
{
d_fmt = f;
d_prec = prec;
}

/*!
  \brief Set the number precision for the major scale labels

  Precision has the same meaning as for
  sprintf().
  \param prec
    - for 'e', 'f': the number of digits after the radix character (point)
    - for 'g': the maximum number of significant digits

  \sa labelFormat()
*/
void setLabelPrecision(int prec)
{
d_prec = prec;
}

/*!
  \brief Return the number format for the major scale labels

  Format character and precision have the same meaning as for
  sprintf().
  \param f format character 'e', 'f' or 'g' 
  \param prec
    - for 'e', 'f': the number of digits after the radix character (point)
    - for 'g': the maximum number of significant digits

  \sa setLabelFormat()
*/
void labelFormat(char &f, int &prec) const
{
    f = d_fmt;
    prec = d_prec;
}
	
	/*void drawTick(QPainter *p, double val, int len) const
	{
	bool print = p->device()->isExtDev();
		
	bool hasBackbone = hasComponent(QwtAbstractScaleDraw::Backbone);
	if ( hasBackbone && !print)
		QwtScaleDraw::drawTick(p, val, len);
	else if (!hasBackbone)
		{
    	if ( len <= 0 )
       		return;

		const QwtScaleMap map = this->map();
    	const int tval = map.transform(val);
		const int xorg = x();
		const int yorg = y();	
	
    	switch(orientation())
    		{
        	case LeftScale:
				QwtPainter::drawLine(p, xorg + 1, tval, xorg - len, tval);
            break;

        	case RightScale:
				QwtPainter::drawLine(p, xorg, tval, xorg + len, tval);
            break;

        	case BottomScale: 
                 QwtPainter::drawLine(p, tval, yorg, tval, yorg + len);
            break;

        	case TopScale:
				QwtPainter::drawLine(p, tval, yorg + 1, tval, yorg - len);
            break;

        	default:
            	break;
    		}
		}
	else if (print)
		{
    	if ( len <= 0 )
       		return;

    	const QwtScaleMap map = this->map();
    	const int tval = map.transform(val);
		const int xorg = x();
		const int yorg = y();	
	
    	switch(orientation())
    		{
        	case LeftScale:
				{
				if (!hasBackbone)
					QwtPainter::drawLine(p, xorg + 1, tval, xorg - len, tval);
				else
					QwtPainter::drawLine(p, xorg, tval, xorg - len, tval);
           	    break;
				}

        	case RightScale:
				{
				if (!hasBackbone)
            		QwtPainter::drawLine(p, xorg, tval, xorg + len, tval);
				else
					{
					const int bw = QMAX (p->pen().width() / 2, 1);
					QwtPainter::drawLine(p, xorg + bw, tval, xorg + len, tval);
					}
            	break;
				}

        	case BottomScale: 
				{
				const int bw = p->pen().width() / 2;
					QwtPainter::drawLine(p, tval, yorg + bw, tval, yorg + len);
            	break;
				}

        	case TopScale:
				QwtPainter::drawLine(p, tval, yorg, tval, yorg - len);
            break;

        	default:
            	break;
    		}
		}
	};
	
	void drawBackbone(QPainter *p) const
	{   
	const int pw = p->pen().width();
	const int pw2 = p->pen().width() % 2;
		
	if (pw ==1 || !pw2)
		{
		const int xorg = x();
		const int yorg = y();
		const int l = length();
			
    	switch(orientation())
    		{
        	case LeftScale:
				{
				const int bw2 = (pw - 1) / 2;
            	QwtPainter::drawLine(p, xorg - bw2, yorg, xorg - bw2, yorg + l - 1);
            	break;
				}
			
        	case RightScale:
				{
				int bw2 = pw / 2;
				if (p->device()->isExtDev() && pw == 1)
					bw2 = 1;
					
				QwtPainter::drawLine(p, xorg + bw2, yorg, xorg + bw2, yorg + l - 1);
            	break;
				}
			
        	case TopScale:
				{
				const int bw2 = (pw - 1) / 2;
            	QwtPainter::drawLine(p, xorg, yorg - bw2, xorg + l - 1, yorg - bw2);
            	break;
				}
			
        	case BottomScale:
				{
				const int bw2 = pw / 2;
            	QwtPainter::drawLine(p, xorg, yorg + bw2, xorg + l - 1, yorg + bw2);
            	break;
				}
    		}
		}
	else
		QwtScaleDraw::drawBackbone(p);
	};*/
	
private:
	QString formula_string;

	char d_fmt;
    int d_prec;
};

class QwtTextScaleDraw: public ScaleDraw
{
public:
	QwtTextScaleDraw(const QStringList& list)
		{
		labels = list; 
		};
		
	~QwtTextScaleDraw(){};
		
	virtual QwtText label(double value) const
		{
		int index=qRound(value);
		if (floor(value) == value && index > 0 && index <= (int)labels.count())
			return QwtText(labels[index - 1]);
		else
			return QwtText();
		};

	QStringList labelsList(){return labels;};

private:
	QStringList labels;
};

class TimeScaleDraw: public ScaleDraw
{
public:
	TimeScaleDraw(const QTime& t, const QString& format)
		{
		t_origin = t; 
		t_format = format;
		};
		
	~TimeScaleDraw(){};
	
	QString origin() 
	{
		return t_origin.toString ( "hh:mm:ss.zzz" );
	};
	
	QString timeFormat() {return t_format;};
		
	virtual QwtText label(double value) const
	{
		QTime t = t_origin.addMSecs ( (int)value );		
		return QwtText(t.toString ( t_format ));
	};
	
private:
	QTime t_origin;
	QString t_format;
};

class DateScaleDraw: public ScaleDraw
{
public:
	DateScaleDraw(const QDate& t, const QString& format)
		{
		t_origin = t; 
		t_format = format;
		};
		
	~DateScaleDraw(){};
	
	QString origin() 
	{
		return t_origin.toString ( Qt::ISODate );
	};
	
	QString format() {return t_format;};
		
	virtual QwtText label(double value) const
	{
		QDate t = t_origin.addDays ( (int)value );
		return QwtText(t.toString ( t_format ));
	};
	
private:
	QDate t_origin;
	QString t_format;
};

class WeekDayScaleDraw: public ScaleDraw
{
public:
	enum NameFormat{ShortName, LongName, Initial};

	WeekDayScaleDraw(NameFormat format = ShortName):
		d_format(format)
		{};
		
	~WeekDayScaleDraw(){};
		
	NameFormat format() {return d_format;};
		
	virtual QwtText label(double value) const
	{
		int val = int(transformValue(value))%7;

		if (val < 0)
			val = 7 - abs(val);
		else if (val == 0)
			val = 7;

		QString day;
		switch(d_format)
			{
			case  ShortName:
				day = QDate::shortDayName (val);
			break;

			case  LongName:
				day = QDate::longDayName (val);
			break;

			case  Initial:
				day = (QDate::shortDayName (val)).left(1);
			break;
			}

		return QwtText(day);
	};
	
private:
	NameFormat d_format;
};

class MonthScaleDraw: public ScaleDraw
{
public:
	enum NameFormat{ShortName, LongName, Initial};

	MonthScaleDraw(NameFormat format = ShortName):
		d_format(format)
		{};
		
	~MonthScaleDraw(){};
		
	NameFormat format() {return d_format;};
		
	virtual QwtText label(double value) const
	{
		int val = int(transformValue(value))%12;

		if (val < 0)
			val = 12 - abs(val);
		else if (val == 0)
			val = 12;

		QString day;
		switch(d_format)
			{
			case  ShortName:
				day = QDate::shortMonthName (val);
			break;

			case  LongName:
				day = QDate::longMonthName (val);
			break;

			case  Initial:
				day = (QDate::shortMonthName (val)).left(1);
			break;
			}

		return QwtText(day);
	};
	
private:
	NameFormat d_format;
};

class QwtSupersciptsScaleDraw: public ScaleDraw
{
public:
	QwtSupersciptsScaleDraw(const QString& s = QString::null)
	{
	setFormulaString(s);
	};

	~QwtSupersciptsScaleDraw(){};

	virtual QwtText label(double value) const
	{
	char f;
	int prec;
	labelFormat(f, prec);
	
	double val = transformValue(value);
	
	QString txt;
	txt.setNum (val, 'e', prec);

	QStringList list = QStringList::split ( "e", txt, FALSE );
	if (list[0].toDouble() == 0.0)
		return QString("0");
	
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
	
	if (list[0] == "1")
		return QwtText("10<sup>" + s + "</sup>");
	else
		return QwtText(list[0] + "x10<sup>" + s + "</sup>");
	};
};

#endif

