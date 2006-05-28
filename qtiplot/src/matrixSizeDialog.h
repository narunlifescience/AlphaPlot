/***************************************************************************
    File                 : matrixSizeDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Matrix dimensions dialog
                           
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
#ifndef matrixSizeDialog_H
#define matrixSizeDialog_H

#include <qvariant.h>
#include <qdialog.h>

class Q3ButtonGroup;
class QPushButton;
class QSpinBox;
class QLineEdit;
	
//! Matrix dimensions dialog
class matrixSizeDialog : public QDialog
{
    Q_OBJECT

public:
    matrixSizeDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~matrixSizeDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
    Q3ButtonGroup* GroupBox1;
	QSpinBox *boxCols, *boxRows;
	QLineEdit *boxXStart, *boxYStart, *boxXEnd, *boxYEnd;

public slots:
    virtual void languageChange();
	void accept();
	void setColumns(int c);
	void setRows(int r);
	void setCoordinates(double xs, double xe, double ys, double ye);

signals:
	void changeDimensions(int, int);
	void changeCoordinates(double, double, double, double);
};

#endif // 
