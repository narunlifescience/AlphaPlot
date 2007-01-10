/***************************************************************************
    File                 : functionDialogui.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Function dialog user interface
                           
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
#ifndef FUNCTIONDIALOGUI_H
#define FUNCTIONDIALOGUI_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QLabel>


class QLabel;

//! Function dialog user interface
class FunctionDialogUi : public QDialog
{
    Q_OBJECT

public:
    FunctionDialogUi( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );

    QLabel* textFunction_2;
    Q3WidgetStack* optionStack;
    QWidget* functionPage;
    QLabel* textFunction;
    QLineEdit* boxFrom;
    QLabel* textFrom;
    QLabel* textTo;
    QLabel* textPoints;
    QLineEdit* boxTo;
    Q3TextEdit* boxFunction;
    QPushButton* buttonClear;
    QWidget* parametricPage;
    QLabel* textParameter;
    QLineEdit* boxParameter;
    QLabel* textParPoints;
    QLineEdit* boxParFrom;
    QLineEdit* boxParTo;
    QLabel* textParameterTo;
    QLabel* textYPar;
    QLabel* textXPar;
    QComboBox* boxXFunction;
    QComboBox* boxYFunction;
    QLabel* textParameterFrom;
    QWidget* polarPage;
    QLabel* textPolarPoints;
    QLabel* textPolarParameter;
    QLabel* textPolarFrom;
    QLabel* textPolarTo;
    QLabel* textPolarRadius;
    QLineEdit* boxPolarParameter;
    QLineEdit* boxPolarFrom;
    QLineEdit* boxPolarTo;
    QComboBox* boxPolarRadius;
    QLabel* textPolarTheta;
    QComboBox* boxPolarTheta;
    QComboBox* boxType;
    Q3ButtonGroup  *GroupBox1, *GroupBox2, *GroupBox3, *GroupBox4;
    QPushButton* buttonCancel;
    QPushButton* buttonOk;
	QSpinBox *boxPoints, *boxParPoints, *boxPolarPoints;

protected slots:
    void languageChange();
	void raiseWidget(int index);

};

#endif // FUNCTIONDIALOGUI_H
