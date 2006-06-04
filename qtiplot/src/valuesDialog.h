/***************************************************************************
    File                 : valuesDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Set column values dialog
                           
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
#ifndef VALUESDIALOG_H
#define VALUESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <QLabel>
#include <Q3GridLayout>
class Q3VBoxLayout; 
class Q3HBoxLayout; 
class Q3GridLayout; 
class QComboBox;
class Q3TextEdit;
class QSpinBox;
class QPushButton;
class QLabel;
class Table;
	
//! Set column values dialog
class setColValuesDialog : public QDialog
{ 
    Q_OBJECT

public:
    setColValuesDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~setColValuesDialog();
	
	QSize sizeHint() const ;

    QComboBox* functions;
    QComboBox* boxColumn;
    QPushButton* PushButton3; 
    QPushButton* PushButton4;
    QPushButton* btnOk;
    QPushButton* btnCancel;
    Q3TextEdit* commands;
    Q3TextEdit* explain;
	QSpinBox* start, *end;
	QPushButton *buttonPrev, *buttonNext, *addCellButton, *btnApply;
	QLabel *colNameLabel;

public slots:
	void accept();
	bool apply();
	void prevColumn();
	void nextColumn();
	void setFunctions();
	void insertFunction();
	void insertCol();
	void insertCell();
	void insertExplain(int index);
	void setTable(Table* w);
	void updateColumn(int sc);

private:
	Table* table;
};

#endif //
