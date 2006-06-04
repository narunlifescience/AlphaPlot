/***************************************************************************
    File                 : intDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Integration options dialog
                           
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
#ifndef INTDIALOG_H
#define INTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class Q3ButtonGroup;
class QPushButton;
class QCheckBox;
class QLineEdit;
class QComboBox;
class QSpinBox;
class Graph;
	
//! Integration options dialog
class intDialog : public QDialog
{
    Q_OBJECT

public:
    intDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~intDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonHelp;
    Q3ButtonGroup* GroupBox1, *GroupBox2;
    QCheckBox* boxShowFormula;
	QComboBox* boxName;
	QSpinBox* boxOrder;
	QSpinBox* boxSteps;
	QLineEdit* boxStart;
	QLineEdit* boxEnd;
	QLineEdit* boxTol;	

public slots:
    virtual void languageChange();
	void accept();
	void setGraph(Graph *g);
	void activateCurve(int index);
	void changeCurve(int index);
	void help();
	void changeDataRange();

signals:
	void integrate(int,int,int,double,double,double);

private:
	Graph *graph;
};

#endif 


