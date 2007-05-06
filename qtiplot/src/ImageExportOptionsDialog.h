/***************************************************************************
    File                 : ImageExportOptionsDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Image export options dialog

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
#ifndef IMEXPORTOPTIONSDIALOG_H
#define IMEXPORTOPTIONSDIALOG_H

#include <QDialog>

class QGroupBox;
class QPushButton;
class QCheckBox;
class QSpinBox;
class QComboBox;
class QLabel;

//! Image export options dialog
class ImageExportOptionsDialog : public QDialog
{
    Q_OBJECT

public:
    ImageExportOptionsDialog( bool exportAllPlots, QWidget* parent = 0, Qt::WFlags fl = 0 );
    ~ImageExportOptionsDialog(){};

    QPushButton * buttonOk;
	QPushButton * buttonCancel;
    QGroupBox * groupBox1;
    QCheckBox * boxTransparency;
	QSpinBox * boxQuality;
	QLabel *formatLabel, *labelQuality;
	QComboBox * boxFormat;

public slots:
	void enableTransparency();
	void enableTransparency(int);
	void setExportPath(const QString& fname, const QString& fileType)
			{f_name = fname; f_type = fileType;};

	void setExportDirPath(const QString& dir) {f_dir = dir;};

protected slots:
	void accept();

signals:
	void options(const QString&, int, bool);
	void exportAll(const QString&, const QString&, int, bool);

private:
	QString f_name, f_type, f_dir;
	bool expAll;
};

#endif // IMAGEEXPORTOPTIONSDIALOG_H
