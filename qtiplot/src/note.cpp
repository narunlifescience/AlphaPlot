/***************************************************************************
    File                 : note.cpp
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
#include "note.h"

#include <QDateTime>
#include <QLayout>
#include <QApplication>
#include <QPrinter>
#include <QPainter>
#include <QPaintDevice>
#include <QVBoxLayout>
#include <QPrintDialog>

#include <math.h>

Note::Note(const QString& label, QWidget* parent, const char *name, Qt::WFlags f)
				: MyWidget(label, parent, name, f)
{
	init();	
}

void Note::init()
{
	QDateTime dt = QDateTime::currentDateTime();
	setBirthDate(dt.toString(Qt::LocalDate));

	te = new QTextEdit();
	QVBoxLayout * layout = new QVBoxLayout( this );
	layout->addWidget(te);
	layout->setMargin(0);

	setGeometry(0, 0, 500, 200);
	connect(te, SIGNAL(textChanged()), this, SLOT(modifiedNote()));
}

void Note::modifiedNote()
{
	emit modifiedWindow(this);
}

QString Note::saveToString(const QString &info)
{
	QString s= "<note>\n";
	s+= QString(name()) + "\t" + birthDate() + "\n";
	s+= info;
	s+= "WindowLabel\t" + windowLabel() + "\t" + QString::number(captionPolicy()) + "\n";
	s+= te->text().stripWhiteSpace()+"\n";
	s+="</note>\n";
	return s;
}

void Note::print()
{
    QTextDocument *document = te->document();
	QPrinter printer;
	printer.setColorMode(QPrinter::GrayScale);
	QPrintDialog printDialog(&printer);
	// TODO: Write a dialog to use more features of Qt4's QPrinter class
    if (printDialog.exec() == QDialog::Accepted) 
	{
        document->print(&printer);
	}
}



