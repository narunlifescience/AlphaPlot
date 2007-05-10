/***************************************************************************
    File                 : PlotWizard.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : A wizard type dialog to create new plots

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


#ifndef PLOTWIZARD_H
#define PLOTWIZARD_H

#include <QDialog>
class QGroupBox;
class QPushButton;
class QListWidget;
class QComboBox;

//! A wizard type dialog class to create new plots
class PlotWizard : public QDialog
{
    Q_OBJECT

public:
	//! Constructor
	/**
	 * \param parent parent widget
	 * \param fl Qt window flags
	 */
    PlotWizard( QWidget* parent = 0, Qt::WFlags fl = 0 );
	 //! Destructor
    ~PlotWizard();

private:
				//! Button "Plot"
    QPushButton* buttonOk,
				//! Button "<->xErr"
				*buttonXErr,
				//! Button "<->yErr"
				*buttonYErr,
				//! Button "Delete curve"
				*buttonDelete;
				//! Button "cancel"
	QPushButton* buttonCancel,
				//! Button "<->X"
				*buttonX,
				//! Button "<->Y"
				*buttonY,
				//! Button "New curve"
				*buttonNew,
				//! Button "<->Z"
				*buttonZ;
				//! Button group defining the layout
    QGroupBox*  groupBox1,
				//! Button group defining the layout
				*groupBox2,
				//! Button group defining the layout
				*groupBox3;
				//! Combo box to select the table
    QComboBox* boxTables;
				//! List of the columns in the selected table
	QListWidget *columnsList,
				//! List of the plots to generate
			 *plotAssociations;

	//! Internal list of columns (contains all columns in all tables)
	QStringList columns;

public slots:
	//! Update the columns list box to contain the columns of 'table'
	void changeColumnsList(const QString &table);
	//! Insert a list of tables into the tables combo box
	void insertTablesList(const QStringList& tables);
	//! Set the contents of the columns list box to the strings in 'cols'
	void setColumnsListBoxContents(const QStringList& cols);
	//! Set the internal columns list to 'cols'
	void setColumnsList(const QStringList& cols);
	//! Add new curve
	void addCurve();
	//! Delete selected curve
	void removeCurve();
	//! Add column as X
	void addXCol();
	//! Add column as Y
	void addYCol();
	//! Add column as X error
	void addXErrCol();
	//! Add column as Y error
	void addYErrCol();
	//! Accept settings, close the dialog
	void accept();
	//! Add column as Z
	void addZCol();

private:
    void plot3D(const QStringList& lst);
    void plot3DRibbon(const QStringList& lst);
	//! Display a warning that a new curve must be added first before the selection of the columns
	bool noCurves();
    QSize sizeHint() const;

signals:
	//! Emitted when new plots need to be created
	void plot (const QStringList&);
};

#endif //  PLOTWIZARD_H
