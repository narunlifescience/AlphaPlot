/***************************************************************************
    File                 : ImportDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : ASCII import options dialog
                           
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
#ifndef IMPORTDIALOG_H
#define IMPORTDIALOG_H

#include <QDialog>
class QLabel;
class QPushButton;
class QComboBox;
class QSpinBox;
class QCheckBox;


//! ASCII import options dialog
class ImportDialog : public QDialog
{
    Q_OBJECT

public:
	//! Constructor
	/**
	 * Parameters: see QDialog constructor
	 */
    ImportDialog( QWidget* parent = 0 , Qt::WFlags fl = 0 );
	//! Destructor
    ~ImportDialog();

private:
	QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonHelp;
    QComboBox* boxSeparator;
	QSpinBox* boxLines;
	QCheckBox *boxRenameCols, *boxSimplifySpaces, *boxStripSpaces;
	QLabel *ignoreLabel,*sepText;

public slots:
	//! Set the column delimiter for ASCII import
	void setSeparator(const QString& sep);

private slots:
	void quit();
	//! Accept changes
	void accept();
	//! Display help
	void help();

	void enableApplyButton(int);
	void enableApplyButton(const QString &);
};

#endif // IMPORTDIALOG_H
