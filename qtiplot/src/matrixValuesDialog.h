/***************************************************************************
    File                 : matrixValuesDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Set matrix values dialog
                           
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
#ifndef MVALUESDIALOG_H
#define MVALUESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QComboBox;
class Q3TextEdit;
class QSpinBox;
class QPushButton;
	
//! Set matrix values dialog
class MatrixValuesDialog : public QDialog
{ 
    Q_OBJECT

public:
    MatrixValuesDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~MatrixValuesDialog();
	
	QSize sizeHint() const ;

    QComboBox* functions;
    QPushButton* PushButton3; 
    QPushButton* btnOk, *btnAddCell;
    QPushButton* btnCancel;
    Q3TextEdit* commands;
    Q3TextEdit* explain;
	QSpinBox *startRow, *endRow, *startCol, *endCol;
	QPushButton *btnApply;

public slots:
	void accept();
	bool apply();
	void setFunctions();
	void addCell();
	void insertFunction();
	void insertExplain(int index);
	void setColumns(int c);
	void setRows(int r);
	void setFormula(const QString& s);

signals:
	void setValues (const QString&, const QString&, const QStringList&, 
			   const QStringList&, int, int, int, int);
};

#endif //
