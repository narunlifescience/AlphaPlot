/***************************************************************************
    File                 : PolynomFitDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Fit polynomial dialog
                           
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
#ifndef POLINOMFITDIALOG_H
#define POLINOMFITDIALOG_H

#include <QDialog>

class QCheckBox;
class QSpinBox;
class QPushButton;
class QLineEdit;
class QComboBox;
class Graph;
class ColorBox;

//! Fit polynomial dialog
class PolynomFitDialog : public QDialog
{
    Q_OBJECT

public:
    PolynomFitDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~PolynomFitDialog();

public slots:
	void fit();
	void setGraph(Graph *g);
	void activateCurve(int index);
	void changeCurve(int index);
	void changeDataRange();

private:
	Graph *graph;

    QPushButton* buttonFit;
	QPushButton* buttonCancel;
    QCheckBox* boxShowFormula;
	QComboBox* boxName;
	QSpinBox* boxOrder;
	QSpinBox* boxPoints;
	QLineEdit* boxStart;
	QLineEdit* boxEnd;
	ColorBox* boxColor;
};

#endif // EXPORTDIALOG_H
