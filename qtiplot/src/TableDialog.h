/***************************************************************************
    File                 : TableDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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

#include "Table.h"

class QPushButton;
class QLineEdit;
class QCheckBox;
class QComboBox;
class QLabel;
class QTextEdit;
class QSpinBox;

//! Column options dialog
class TableDialog : public QDialog
{
    Q_OBJECT

public:
    TableDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~TableDialog();

    QPushButton* buttonOk;
    QPushButton* buttonCancel;
	QPushButton* buttonApply;
	QPushButton* buttonPrev;
	QPushButton* buttonNext;
    QLineEdit* colName;
    QCheckBox* enumerateAllBox;
    QCheckBox* applyToRightCols;
    QCheckBox* applyToAllBox;
    QComboBox* formatBox;
	QComboBox* displayBox;
    QComboBox* columnsBox;
    QSpinBox* colWidth, *precisionBox;
	QLabel *labelNumeric, *labelFormat;
	QTextEdit *comments;

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
