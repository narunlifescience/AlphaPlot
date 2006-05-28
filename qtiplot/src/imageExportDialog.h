/***************************************************************************
    File                 : imageExportDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Image export dialog
                           
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
#ifndef IMEXPORTDIALOG_H
#define IMEXPORTDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
//Added by qt3to4:
#include <QLabel>

class Q3ButtonGroup;
class QPushButton;
class QCheckBox;
class QSpinBox;
class QComboBox;
class QLabel;

//! Image export dialog
class imageExportDialog : public QDialog
{
    Q_OBJECT

public:
    imageExportDialog( bool exportAllPlots, QWidget* parent = 0, 
									const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~imageExportDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
    Q3ButtonGroup* GroupBox1, *GroupBox2;
    QCheckBox* boxTransparency;
	QSpinBox * boxQuality;
	QLabel *formatLabel;
	QComboBox *boxFormat;
	
public slots:
	void enableTransparency();
	void enableTransparency(int);
	void setExportPath(const QString& fname, const QString& fileType)
			{f_name = fname; f_type = fileType;};
			
	void setExportDirPath(const QString& dir) {f_dir = dir;};

protected slots:
    virtual void languageChange();
	void accept();

signals:
	void options(const QString&, const QString&, int, bool);
	void exportAll(const QString&, const QString&, int, bool);

private:
	QString f_name, f_type, f_dir;
	bool expAll;
};

#endif // imageExportDialog_H
