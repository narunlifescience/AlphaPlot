/***************************************************************************
    File                 : matrixDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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

#include <qvariant.h>
#include <qdialog.h>

class QPushButton;
class QSpinBox;
class QComboBox;
	
//! Matrix properties dialog
class matrixDialog : public QDialog
{
    Q_OBJECT

public:
    matrixDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~matrixDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel, *buttonApply;
	QSpinBox* boxColWidth, *boxPrecision; 
	QComboBox *boxFormat, *boxNumericDisplay;

public slots:
    virtual void languageChange();
	void accept();
	void apply();
	void setColumnsWidth(int width);
	void showPrecisionBox(int item);
	void setTextFormat(const QString& format, int precision);
	void changePrecision(int precision);

signals:
	void changeColumnsWidth(int);
	void changeTextFormat(const QChar&, int);
};

#endif // IMPORTDIALOG_H
