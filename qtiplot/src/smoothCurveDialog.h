/***************************************************************************
    File                 : smoothCurveDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Smoothing options dialog
                           
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
#ifndef SMOOTHDIALOG_H
#define SMOOTHDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QComboBox;
class QSpinBox;
class Graph;
class ColorBox;
	
//! Smoothing options dialog
class smoothCurveDialog : public QDialog
{
    Q_OBJECT

public:
    smoothCurveDialog(int method, QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~smoothCurveDialog();

	enum SmoothType{SavitzkyGolay = 1, FFT = 2, Average = 3};

	QPushButton* btnSmooth;
	QPushButton* buttonCancel;
	QComboBox* boxName;
	QSpinBox *boxPointsLeft, *boxPointsRight, *boxOrder;
	ColorBox* boxColor;

public slots:
	void setGraph(Graph *g);
    virtual void languageChange();
	void smooth();
	void activateCurve(int index);

private:
	Graph *graph;
	int smooth_type;
};

#endif



