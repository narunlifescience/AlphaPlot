#include "analysisDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qhbuttongroup.h>

analysisDialog::analysisDialog( QWidget* parent, const QString& text, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "analysisDialog" );
	setCaption(tr("QtiPlot - Analysis Option"));
	
	operation = QString::null;

	GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),this,"GroupBox1" );

	new QLabel( tr(text), GroupBox1, "TextLabel1",0 );
	boxName = new QComboBox(GroupBox1, "boxShow" );
	
	GroupBox2 = new QHButtonGroup(this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	GroupBox2->setLineWidth (0);
	
	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
	
    QVBoxLayout* vlayout = new QVBoxLayout(this,5,5, "vlayout");
	vlayout->addWidget(GroupBox1);
	vlayout->addWidget(GroupBox2);
	
    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

analysisDialog::~analysisDialog()
{
}

void analysisDialog::languageChange()
{
    buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
}

void analysisDialog::accept()
{
if (operation.isEmpty())
	emit options(boxName->currentText());
else
	emit analyse(operation, boxName->currentText());
close();
}

void analysisDialog::setCurveNames(const QStringList& names)
{
boxName->insertStringList (names,-1);
}
