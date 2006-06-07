/***************************************************************************
    File                 : note.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Notes window class
                           
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
#ifndef NOTE_H
#define NOTE_H

#include "widget.h"	
#include <QTextEdit>

//! Notes window class
class Note: public MyWidget
{
    Q_OBJECT

public:

	Note(const QString& label, QWidget* parent=0, const char *name=0, Qt::WFlags f=0);
	~Note(){};

	QTextEdit *te;
		
	void init();

public slots:
	QString saveToString(const QString &info);

	QTextEdit* textWidget(){return te;};
	QString text(){return te->text();};
	void setText(const QString &s){te->setText(s);};
	void modifiedNote();
	void print();	
};
   
#endif
