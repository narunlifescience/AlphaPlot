/***************************************************************************
    File                 : surfaceDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
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

class QPushButton;
class QLineEdit;
class QComboBox;
	
//! Define surface plot dialog
class SurfaceDialog : public QDialog
{
    Q_OBJECT

public:
    SurfaceDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~SurfaceDialog();

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
	
private:
    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonClear;
	QComboBox* boxFunction;
	QLineEdit* boxXFrom;
	QLineEdit* boxXTo;
	QLineEdit* boxYFrom;
	QLineEdit* boxYTo;
	QLineEdit* boxZFrom;
	QLineEdit* boxZTo;
};

#endif 


