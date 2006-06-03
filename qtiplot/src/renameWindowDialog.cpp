/***************************************************************************
    File                 : renameWindowDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Rename window dialog
                           
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
#include "renameWindowDialog.h"
#include "application.h"
#include "worksheet.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <q3buttongroup.h>
#include <qlineedit.h>
#include <q3textedit.h>
#include <qradiobutton.h>
#include <qmessagebox.h>
#include <qaction.h>
#include <qregexp.h>
//Added by qt3to4:
#include <Q3HBoxLayout>

renameWindowDialog::renameWindowDialog(QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "renameWindowDialog" );
		
	setWindowTitle(tr("QtiPlot - Rename Window"));
	
	GroupBox1 = new Q3ButtonGroup( 2, Qt::Horizontal,tr("Window Title"),this,"GroupBox1" );

	boxName = new QRadioButton(tr("&Name (single word)"), GroupBox1, "boxName" );
	boxNameLine = new QLineEdit(GroupBox1, "boxNameLine");
	setFocusProxy(boxNameLine);

	boxLabel = new QRadioButton(tr("&Label"), GroupBox1, "boxLabel" );
	boxLabelEdit = new Q3TextEdit(GroupBox1, "boxLabelEdit");
	boxLabelEdit->setMaximumHeight(100);

	boxBoth = new QRadioButton(tr("&Both Name and Label"), GroupBox1, "boxBoth" );
	
	GroupBox2 = new Q3ButtonGroup(1,Qt::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	
	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
	
	Q3HBoxLayout* hlayout = new Q3HBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(GroupBox2);

    languageChange();

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void renameWindowDialog::setWidget(MyWidget *w)
{
window = w;
boxNameLine->setText(w->name());
boxLabelEdit->setText(w->windowLabel());
switch (w->captionPolicy())
	{
	case MyWidget::Name:
		boxName->setChecked(true);
	break;

	case MyWidget::Label:
		boxLabel->setChecked(true);
	break;

	case MyWidget::Both:
		boxBoth->setChecked(true);
	break;
	}
}

MyWidget::CaptionPolicy renameWindowDialog::getCaptionPolicy()
{
MyWidget::CaptionPolicy policy = MyWidget::Name;
if (boxLabel->isChecked())
	policy = MyWidget::Label;
else if (boxBoth->isChecked())
	policy = MyWidget::Both;
		
return policy;
}

renameWindowDialog::~renameWindowDialog()
{
}

void renameWindowDialog::languageChange()
{
buttonOk->setText( tr( "&OK" ) );
buttonCancel->setText( tr( "&Cancel" ) );
}

void renameWindowDialog::accept()
{
QString name = window->name();
QString text = boxNameLine->text().remove("_").remove("=").remove(QRegExp("\\s"));
QString label = boxLabelEdit->text();

MyWidget::CaptionPolicy policy = getCaptionPolicy();
if (text == name && label == window->windowLabel() && window->captionPolicy() == policy)
	close();

ApplicationWindow *app = (ApplicationWindow *)parentWidget();
if (!app)
	return;

if (text != name)
	{
	if(!app->renameWindow(window, text))
		return;

	app->renameListViewItem(name,text);
	}

label.replace("\n"," ").replace("\t"," ");
window->setWindowLabel(label);
window->setCaptionPolicy(policy);
app->setListViewLabel(window->name(), label);
app->modifiedProject(window);
close();
}
