/***************************************************************************
    File                 : tableDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Column options dialog
                           
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
#ifndef TABLEDIALOG_H
#define TABLEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QLabel>
#include <QCloseEvent>
#include "worksheet.h"

class QPushButton;
class QLineEdit;
class QCheckBox;
class Q3ButtonGroup;
class QComboBox;
class QLabel;
class Q3TextEdit;
class QSpinBox;

//! Column options dialog
class tableDialog : public QDialog
{
    Q_OBJECT

public:
    tableDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~tableDialog();

    QPushButton* buttonCancel;
	QPushButton* buttonApply;
	QPushButton* buttonPrev;
	QPushButton* buttonNext;
    QLineEdit* colName;
    QCheckBox* enumerateAllBox;
    QPushButton* buttonOk;
 	Q3ButtonGroup* GroupBox2;
    QCheckBox* applyToRightCols;
    QComboBox* formatBox;
	QComboBox* displayBox;
    QComboBox* columnsBox;
    QSpinBox* colWidth, *precisionBox;
    QCheckBox* applyToAllBox;
	QLabel *labelNumeric, *labelFormat;
	Q3TextEdit *comments;

protected slots:
    virtual void languageChange();

public slots:
	void prevColumn();
	void nextColumn();
	void updateColumn(int);
	void changeColWidth(int width);
	void setWorksheet(Table * table);
	void showPrecisionBox(int item);
	void updateFormat();
	void updatePrecision(int prec);
	void setPlotDesignation(int i);
	void accept();
	void apply();
	void closeEvent( QCloseEvent *);
	void updateDisplay(int item);
	void enablePrecision(int f);

signals:
	void nameChanged(const QString&);
	void enumRightCols(bool);
	void changeWidth(const QString&, bool);

protected:
	Table * w;
};

#endif // TABLEDIALOG_H
