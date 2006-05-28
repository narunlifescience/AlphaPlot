/***************************************************************************
    File                 : findDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Find dialog
                           
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
#include "findDialog.h"
#include "application.h"
#include "folder.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qregexp.h>
#include <q3vbox.h>
#include <q3hbox.h>
#include <q3buttongroup.h>
#include <qlabel.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3Frame>

findDialog::findDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "findDialog" );

	setWindowTitle (tr("QtiPlot") + " - " + tr("Find"));
	setSizeGripEnabled( true );

	Q3ButtonGroup *GroupBox0 = new Q3ButtonGroup(2, Qt::Horizontal, QString::null, this);
	GroupBox0->setFlat(true);

	new QLabel( tr( "Start From" ), GroupBox0 );
	labelStart = new QLabel( GroupBox0 );
	labelStart->setFrameStyle(Q3Frame::Panel | Q3Frame::Sunken);
	labelStart->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

	new QLabel( tr( "Find" ), GroupBox0 );
	boxFind = new QComboBox( TRUE, GroupBox0);
	boxFind->setDuplicatesEnabled(FALSE);
	boxFind->setInsertionPolicy ( QComboBox::InsertAtTop );
	boxFind->setAutoCompletion(true);
	boxFind->setMaxCount ( 10 );
	boxFind->setMaxVisibleItems ( 10 );
	boxFind->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

	Q3HBox *hbox = new Q3HBox(this);
	hbox->setSpacing (5);
	hbox->setMargin (5);

	Q3ButtonGroup *GroupBox1 = new Q3ButtonGroup(1, Qt::Horizontal, tr("Search In"), hbox);

	boxWindowNames = new QCheckBox(tr("&Window Names"), GroupBox1);
    boxWindowNames->setChecked(true);

    boxWindowLabels = new QCheckBox(tr("Window &Labels"), GroupBox1);
    boxWindowLabels->setChecked( false );
	
    boxFolderNames = new QCheckBox(tr("Folder &Names"), GroupBox1);
    boxFolderNames->setChecked( false );

	Q3VBox *vbox = new Q3VBox(hbox);
	vbox->setSpacing (5);
	vbox->setMargin (5);

	boxCaseSensitive = new QCheckBox(tr("Case &Sensitive"), vbox);
    boxCaseSensitive->setChecked(false);

    boxPartialMatch = new QCheckBox(tr("&Partial Match Allowed"), vbox);
    boxPartialMatch->setChecked(true);
	
	boxSubfolders = new QCheckBox(tr("&Include Subfolders"), vbox);
    boxSubfolders->setChecked(true);
	
	Q3VBox *vbox2 = new Q3VBox(hbox);
	vbox2->setSpacing (5);
	
	buttonFind = new QPushButton(tr("&Find"), vbox2);
    buttonFind->setDefault( TRUE );
   
	buttonReset = new QPushButton(tr("&Reset Start From"), vbox2);
    buttonCancel = new QPushButton(tr("&Close"), vbox2);
	
	Q3VBoxLayout* hlayout = new Q3VBoxLayout(this, 5, 5);
	hlayout->addWidget(GroupBox0);
	hlayout->addWidget(hbox);

	setStartPath();
   
    // signals and slots connections
    connect( buttonFind, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonReset, SIGNAL( clicked() ), this, SLOT( setStartPath() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void findDialog::setStartPath()
{
ApplicationWindow *app = (ApplicationWindow *)this->parent();
labelStart->setText(app->current_folder->path());
}

void findDialog::accept()
{
ApplicationWindow *app = (ApplicationWindow *)this->parent();
app->find(boxFind->currentText(), boxWindowNames->isChecked(), boxWindowLabels->isChecked(),
		  boxFolderNames->isChecked(), boxCaseSensitive->isChecked(), boxPartialMatch->isChecked(),
		  boxSubfolders->isChecked());
}

findDialog::~findDialog()
{
}
