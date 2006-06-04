/***************************************************************************
    File                 : expDecayDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Fit exponential decay dialog
                           
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
#ifndef EXPDECAYDIALOG_H
#define EXPDECAYDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QLabel>

class Q3ButtonGroup;
class QPushButton;
class QLineEdit;
class QComboBox;
class QLabel;
class Graph;
class ColorBox;
	
//! Fit exponential decay dialog
class expDecayDialog : public QDialog
{
    Q_OBJECT

public:
    expDecayDialog( int type, QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~expDecayDialog();

	QPushButton* buttonFit;
	QPushButton* buttonCancel;
    Q3ButtonGroup* GroupBox1, *GroupBox2;
	QComboBox* boxName;
	QLineEdit* boxFirst;
	QLineEdit* boxSecond;
	QLineEdit* boxThird;
	QLineEdit* boxStart;
	QLineEdit* boxYOffset;
	QLabel* thirdLabel, *dampingLabel;
	ColorBox* boxColor;

public slots:
    virtual void languageChange();
	void fit();
	void setGraph(Graph *g);

signals:
	void options(const QString&,double,double,double,double,int);
	void options(const QString&, double,double,double,int);
	void options3(const QString&,double,double,double,double,double,int);

private:
	Graph *graph;
	int slopes;
};

#endif // EXPORTDIALOG_H
