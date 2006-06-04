/***************************************************************************
    File                 : interpolationDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Interpolation options dialog
                           
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
#ifndef INTERPOLATIONDIALOG_H
#define INTERPOLATIONDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QLineEdit;
class QComboBox;
class QSpinBox;
class Graph;
class ColorBox;
	
//! Interpolation options dialog
class interpolationDialog : public QDialog
{
    Q_OBJECT

public:
    interpolationDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~interpolationDialog();

	QPushButton* buttonFit;
	QPushButton* buttonCancel;
	QComboBox* boxName;
	QComboBox* boxMethod;
	QSpinBox* boxPoints;
	QLineEdit* boxStart;
	QLineEdit* boxEnd;
	ColorBox* boxColor;

public slots:
	void activateCurve(int index);
	void setGraph(Graph *g);
    virtual void languageChange();
	void interpolate();
	void changeDataRange();

private:
	Graph *graph;
};

#endif



