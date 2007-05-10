/***************************************************************************
    File                 : EpsExportDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : EPS export dialog
                           
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
#ifndef EPSEXPORTDIALOG_H
#define EPSEXPORTDIALOG_H

#include <QDialog>

class QPushButton;
class QCheckBox;
class QSpinBox;
	
//! EPS export dialog
class EpsExportDialog : public QDialog
{
    Q_OBJECT

public:
    EpsExportDialog(const QString& fileName, QWidget* parent = 0, Qt::WFlags fl = 0 );

    QPushButton *buttonOk;
	QPushButton *buttonCancel;
    QCheckBox *boxColor;
	QSpinBox *boxResolution;
	
protected slots:
	void accept();

signals:
	void exportVector(const QString&, int, bool);

private:
	QString f_name;
};

#endif // EPSEXPORTDIALOG_H
