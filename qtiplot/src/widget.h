/***************************************************************************
    File                 : widget.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Extension to QWidget
                           
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
#ifndef WIDGET_H
#define WIDGET_H

#include <qwidget.h>
//Added by qt3to4:
#include <QEvent>
#include <QCloseEvent>

//! Extension to QWidget
class myWidget: public QWidget
{
	Q_OBJECT

public:	
	
	myWidget(const QString& label, QWidget * parent, const char * name, Qt::WFlags f);
	~myWidget(){};

	enum CaptionPolicy{Name = 0, Label = 1, Both = 2};
	enum Status{Hidden = -1, Normal = 0, Minimized = 1, Maximized = 2};

	QString windowLabel(){return QString(w_label);};
	void setWindowLabel(const QString& s){w_label = s;};

	CaptionPolicy captionPolicy(){return caption_policy;};
	void setCaptionPolicy(CaptionPolicy policy);

	QString birthDate(){return birthdate;};
	void setBirthDate(const QString& s){birthdate = s;};

	QString aspect();
	Status status(){return w_status;};
	void setStatus(Status s){w_status = s;};

	virtual QString saveAsTemplate(const QString& ){return QString::null;};
	virtual void restore(const QStringList& ){};

	virtual void print(){};
	virtual QString saveToString(const QString &){return QString::null;};
	
	//! Size of the widget as a string
	virtual QString sizeToString();

	//!Notifies a change in the status of a former maximized window after it was shown as normal as a result of an indirect user action (e.g.: another window was maximized)
	void setNormal();

	//!Notifies that a window was hidden by a direct user action
	void setHidden();

	//event handlers
	void closeEvent( QCloseEvent *);
	void askOnCloseEvent(bool ask){askOnClose = ask;};
	bool event( QEvent *e );

	void showMaximized();
	//! Tells if a resize event was requested by the user or generated programatically
	bool userRequested(){return user_request;};

signals:  
	void closedWindow(QWidget *);
	void hiddenWindow(myWidget *);
	void modifiedWindow(QWidget *);
	void resizedWindow(QWidget *);
	void statusChanged(myWidget *);

private:
	QString w_label, birthdate;
	Status w_status;
	CaptionPolicy caption_policy;
	bool askOnClose;
	//! Tells if the showMaximized action was requested by the user or generated programatically
	bool user_request;
};

#endif
