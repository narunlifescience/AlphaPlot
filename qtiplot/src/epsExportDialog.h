/***************************************************************************
    File                 : epsExportDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
#include <QPrinter>

class QGroupBox;
class QPushButton;
class QCheckBox;
class QSpinBox;
class QComboBox;
	
//! EPS export dialog
class EpsExportDialog : public QDialog
{
    Q_OBJECT

public:
    EpsExportDialog(const QString& fileName, QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~EpsExportDialog();

    QPushButton *buttonOk;
	QPushButton *buttonCancel;
    QGroupBox *groupBox1;
    QCheckBox *boxColor;
	QSpinBox *boxResolution;
	QComboBox *boxPageSize, *boxOrientation;
	
public slots:
	QPrinter::PageSize pageSize();

protected slots:
    virtual void languageChange();
	void accept();

signals:
	void exportToEPS(const QString&, int, QPrinter::Orientation, QPrinter::PageSize, QPrinter::ColorMode);
    void exportPDF(const QString&, int, QPrinter::Orientation, QPrinter::PageSize, QPrinter::ColorMode);

private:
	QString f_name;
};

#endif // EPSEXPORTDIALOG_H
