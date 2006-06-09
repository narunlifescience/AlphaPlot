/***************************************************************************
    File                 : ImportDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
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
	//! Set the widget texts in the current language
    virtual void languageChange();
	//! Set the column delimiter for ASCII import
	void setSeparator(const QString& sep);
	//! Toggle the white space handling options
	void setWhiteSpaceOptions(bool strip, bool simplify);
	//! Set the number of ignored lines
	void setLines(int lines);
	//! Toggle whether the first column is treated as column titles
	void renameCols(bool rename);

private slots:
	//! Accept changes
	void accept();
	//! Display help
	void help();

signals:
	//! Emits the selected options
	void options(const QString&, int, bool, bool, bool);
};

#endif // IMPORTDIALOG_H
