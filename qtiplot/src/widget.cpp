/***************************************************************************
    File                 : widget.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
					  Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net,
                           knut.franke@gmx.de
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
#include "widget.h"
#include <QMessageBox>
#include <QEvent>
#include <QCloseEvent>
#include <QString>

MyWidget::MyWidget(const QString& label, QWidget * parent, const char * name, Qt::WFlags f):
		QWidget (parent, f)
{
	w_label = label;
	caption_policy = Both;
	askOnClose = true;
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
    switch( QMessageBox::information(0,tr("QtiPlot"),
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
	w_status = Maximized;
		else
	{
	user_request = true; 
	w_status = Normal; 
	}

emit statusChanged (this);
	}
	return result;
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

void MyWidget::reparent(QWidget * parent, Qt::WFlags f, const QPoint & p, bool showIt)
{
	titleBar = (QWidget*) parent->child("qt_ws_titlebar","QWidget",false);
	if(titleBar) titleBar->installEventFilter(this);
	QWidget::reparent(parent, f, p, showIt);
}

bool MyWidget::eventFilter(QObject *object, QEvent *e)
{
	if (e->type()==QEvent::ContextMenu && object == titleBar)
	{
		emit showTitleBarMenu();
		((QContextMenuEvent*)e)->accept();
		return true;
	}
	return QObject::eventFilter(object, e);
}

