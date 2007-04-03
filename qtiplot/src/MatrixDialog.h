/***************************************************************************
    File                 : MatrixDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Matrix properties dialog
                           
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
#ifndef MATRIXDIALOG_H
#define MATRIXDIALOG_H

#include <QDialog>

class QPushButton;
class QSpinBox;
class QComboBox;
	
//! Matrix properties dialog
class MatrixDialog : public QDialog
{
    Q_OBJECT

public:
	//! Constructor
	/**
	 * \param parent parent widget
	 * \param fl window flags
	 */
    MatrixDialog( QWidget* parent = 0, Qt::WFlags fl = 0 );
	//! Destructor
    ~MatrixDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel, *buttonApply;
	QSpinBox* boxColWidth, *boxPrecision; 
	QComboBox *boxFormat, *boxNumericDisplay;

public slots:
	//! Set all strings in the current language
	virtual void languageChange();
	//! Set the text format
	/**
	 * \param format format code ("e" or "f")
	 * \param precision number of decimal places
	 */
	void setTextFormat(const QString& format, int precision);
	//! Set the column width
	void setColumnsWidth(int width);

private slots:
	//! Accept changes and quit
	void accept();
	//! Apply changes
	void apply();
	//! Activate the numeric precision choice box
	void showPrecisionBox(int item);
	//! Change the numeric precision (emits changeTextFormat)
	void changePrecision(int precision);

signals:
	//! Emit new column width
	void changeColumnsWidth(int width);
	//! Emit new text format
	/**
	 * \param format format code ('e' or 'f')
	 * \param precision number of decimal places
	 */
	void changeTextFormat(const QChar& format, int precision);
};

#endif // MATRIXDIALOG_H
