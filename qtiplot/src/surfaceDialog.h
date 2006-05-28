/***************************************************************************
    File                 : surfaceDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Define surface plot dialog
                           
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
#ifndef SURFACEDIALOG_H
#define SURFACEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class Q3ButtonGroup;
class QPushButton;
class QLineEdit;
class QComboBox;
	
//! Define surface plot dialog
class sDialog : public QDialog
{
    Q_OBJECT

public:
    sDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~sDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonClear;
    Q3ButtonGroup* GroupBox1, *GroupBox2, *GroupBox3, *GroupBox4, *GroupBox5, *GroupBox6;
	QComboBox* boxFunction;
	QLineEdit* boxXFrom;
	QLineEdit* boxXTo;
	QLineEdit* boxYFrom;
	QLineEdit* boxYTo;
	QLineEdit* boxZFrom;
	QLineEdit* boxZTo;

public slots:
	void accept();
	void insertFunctionsList(const QStringList& list);
	void clearList();
	void setFunction(const QString& s);
	void setLimits(double xs, double xe, double ys, double ye, double zs, double ze);

signals:
	void options(const QString&,double,double,double,double,double,double);
	void clearFunctionsList();
	void custom3DToolBar();
};

#endif // EXPORTDIALOG_H
