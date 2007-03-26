/***************************************************************************
    File                 : MyWidget.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
					  Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net,
                           knut.franke*gmx.de
    Description          : MDI window widget
                           
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
#include "MyWidget.h"
#include <QMessageBox>
#include <QEvent>
#include <QCloseEvent>
#include <QString>

MyWidget::MyWidget(const QString& label, QWidget * parent, const char * name, Qt::WFlags f):
		QWidget (parent, f)
{
	d_max_size = QSize();
	w_label = label;
	caption_policy = Both;
	askOnClose = true;
	user_request = false;
	w_status = Normal;
	titleBar = NULL;
	setObjectName(QString(name));
}

void MyWidget::updateCaption()
{
switch (caption_policy)
	{
	case Name:
        setWindowTitle(objectName());
	break;

	case Label:
		if (!w_label.isEmpty())
            setWindowTitle(w_label);
		else
            setWindowTitle(objectName());
	break;

	case Both:
		if (!w_label.isEmpty())
            setWindowTitle(objectName() + " - " + w_label);
		else
            setWindowTitle(objectName());
	break;
	}
};

void MyWidget::closeEvent( QCloseEvent *e )
{
if (askOnClose)
    {
    switch( QMessageBox::information(this,tr("QtiPlot"),
					tr("Do you want to hide or delete") + "<p><b>'" + objectName() + "'</b> ?",
				      tr("Delete"), tr("Hide"), tr("Cancel"), 0,2)) 
		{
		case 0:
			emit closedWindow(this);
			e->accept();
		break;

		case 1:
			e->ignore();
			emit hiddenWindow(this);
		break;

		case 2:
			e->ignore();
		break;
		} 
    }
else 
    {
	emit closedWindow(this);
    e->accept();
    }
}

QString MyWidget::aspect()
{
QString s = tr("Normal");
switch (w_status)
	{
	case Hidden:
		return tr("Hidden");
	break;

	case Normal:
	break;

	case Minimized:
		return tr("Minimized");
	break;

	case Maximized:
		return tr("Maximized");
	break;
	}
return s;
};

bool MyWidget::event( QEvent *e )
{
	bool result = QWidget::event( e );
	if( e->type() == QEvent::WindowStateChange)
	{
		if( windowState() & Qt::WindowMinimized ) 
	    	w_status = Minimized;
		else if ( windowState() & Qt::WindowMaximized )
		{			
	     	w_status = Maximized;
			// When switching from the Qt::WindowMaximized state to Qt::WindowMinimized
			// Qt posts a resize event with invalid oldSize (width = height = -1).
			// This is why we must keep track of the size of the window in the 
			// Qt::WindowMaximized state and use it as the real oldSize when processing
			// the resize event for the MultiLayer windows.		
			d_max_size = size();
		}
		else
	    {
	    	user_request = true; 
	    	w_status = Normal; 
	    }
    	emit statusChanged (this);
   		return result;
	}
	return false;
}

void MyWidget::setHidden()
{
w_status = Hidden; 
emit statusChanged (this);
hide();
}

void MyWidget::setNormal()
{
w_status = Normal; 
emit statusChanged (this);
}

void MyWidget::showMaximized()
{
user_request = this->isVisible();
QWidget::showMaximized();
}

QString MyWidget::sizeToString()
{
return QString::number(8*sizeof(this)/1024.0, 'f', 1) + " " + tr("kB");
}

// Modifying the title bar menu is somewhat more complicated in Qt4.
// Apart from the trivial change in how we intercept the reparenting,
// in Qt4 the title bar doesn't exist yet at this point.
// Thus, we now also have to intercept the creation of the title bar
// in MyWidget::eventFilter.
void MyWidget::changeEvent(QEvent *event)
{
	if (event->type() == QEvent::ParentChange) {
		titleBar = 0;
		parent()->installEventFilter(this);
	}
	QWidget::changeEvent(event);
}

bool MyWidget::eventFilter(QObject *object, QEvent *e)
{
	QWidget *tmp;
	if (e->type()==QEvent::ContextMenu && object == titleBar)
	{
		emit showTitleBarMenu();
		((QContextMenuEvent*)e)->accept();
		return true;
	} else if (e->type()==QEvent::ChildAdded && object == parent() && (tmp = qobject_cast<QWidget *>(((QChildEvent*)e)->child()))) {
		(titleBar = tmp)->installEventFilter(this);
		parent()->removeEventFilter(this);
	}
	return QObject::eventFilter(object, e);
}
