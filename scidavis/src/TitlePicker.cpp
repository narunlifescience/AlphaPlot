/***************************************************************************
    File                 : TitlePicker.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Title picker
                           
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
#include "TitlePicker.h"

#include <qwt_plot.h>
#include <qwt_text_label.h>

#include <QMouseEvent>

TitlePicker::TitlePicker(QwtPlot *plot):
	QObject(plot)
{
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
		 emit clicked();

		 if (me->button()==Qt::RightButton)
			 emit showTitleMenu();
		 return !(me->modifiers() & Qt::ShiftModifier);
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
