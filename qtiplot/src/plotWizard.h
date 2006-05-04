/***************************************************************************
    File                 : plotWizard.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief
    Email                : ion_vasilief@yahoo.fr
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

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QListBox;
class QComboBox;

//! A wizard type dialog class to create new plots
class plotWizard : public QDialog
{
    Q_OBJECT

public:
	//! Constructor
	/**
	 * \param parent parent widget
	 * \param name widget name
	 * \param modal flag: decides wheather the dialog is model or not
	 * \param fl Qt window flags
	 */
    plotWizard( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
	//! Destructor
    ~plotWizard();

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
    QButtonGroup* GroupBox1, 
				//! Button group defining the layout
				*GroupBox2, 
				//! Button group defining the layout
				*GroupBox3;
				//! Combo box to select the table
    QComboBox* boxTables;
				//! List of the columns in the selected table
	QListBox *columnsList, 
				//! List of the plots to generate
			 *plotAssociations;

public slots:
	//! Slot: Update the columns list box to contain the columns of 'table'
	void changeColumnsList(const QString &table);
	//! Slot: Insert a list of tables into the tables combo box
	void insertTablesList(const QStringList& tables);
	//! Slot: Set the contents of the columns list box to the strings in 'cols' 
	void insertColumnsList(const QStringList& cols);
	//! Slot: Set the internal columns list to 'cols'
	void setColumnsList(const QStringList& cols);
	//! Slot: Add new curve
	void addCurve();
	//! Slot: Delete selected curve
	void toggleCurve();
	//! Slot: Add column as X
	void addXCol();
	//! Slot: Add column as Y
	void addYCol();
	//! Slot: Add column as X error
	void addXErrCol();
	//! Slot: Add column as Y error
	void addYErrCol();
	//! Slot: Accept settings, close the dialog
	void accept();
	//! Slot: Display a warning that a new curve must be added first before the selection of the columns
	bool noCurves();
	//! Slot: Add column as Z
	void addZCol();

signals:
	//! Signal: New plots need to be created
	void plot (const QStringList&);
	//! Signal: A new 3D plot needs to be generated
	void plot3D (const QString&);
	//! Signal: A new 3D ribbon plot needs to be generated
	void plot3DRibbon (const QString&);

private:
	//! Internal list of columns (contains all columns in all tables)
	QStringList columns;
};

#endif //  PLOTWIZARD_H
