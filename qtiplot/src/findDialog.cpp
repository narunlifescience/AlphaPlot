#include "findDialog.h"
#include "application.h"
#include "folder.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qbuttongroup.h>
#include <qlabel.h>

findDialog::findDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "findDialog" );

	setCaption (tr("QtiPlot") + " - " + tr("Find"));
	setSizeGripEnabled( true );

	QButtonGroup *GroupBox0 = new QButtonGroup(2, QGroupBox::Horizontal, QString::null, this);
	GroupBox0->setFlat(true);
	GroupBox0->setLineWidth(0);

	new QLabel( tr( "Start From" ), GroupBox0 );
	labelStart = new QLabel( GroupBox0 );
	labelStart->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	labelStart->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

	new QLabel( tr( "Find" ), GroupBox0 );
	boxFind = new QComboBox( TRUE, GroupBox0);
	boxFind->setDuplicatesEnabled(FALSE);
	boxFind->setInsertionPolicy ( QComboBox::AtTop );
	boxFind->setAutoCompletion(true);
	boxFind->setMaxCount ( 10 );
	boxFind->setSizeLimit ( 10 );
	boxFind->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

	QHBox *hbox = new QHBox(this);
	hbox->setSpacing (5);
	hbox->setMargin (5);

	QButtonGroup *GroupBox1 = new QButtonGroup(1, QGroupBox::Horizontal, tr("Search In"), hbox);

	boxWindowNames = new QCheckBox(tr("&Window Names"), GroupBox1);
    boxWindowNames->setChecked(true);

    boxWindowLabels = new QCheckBox(tr("Window &Labels"), GroupBox1);
    boxWindowLabels->setChecked( false );
	
    boxFolderNames = new QCheckBox(tr("Folder &Names"), GroupBox1);
    boxFolderNames->setChecked( false );

	QVBox *vbox = new QVBox(hbox);
	vbox->setSpacing (5);
	vbox->setMargin (5);

	boxCaseSensitive = new QCheckBox(tr("Case &Sensitive"), vbox);
    boxCaseSensitive->setChecked(false);

    boxPartialMatch = new QCheckBox(tr("&Partial Match Allowed"), vbox);
    boxPartialMatch->setChecked(true);
	
	boxSubfolders = new QCheckBox(tr("&Include Subfolders"), vbox);
    boxSubfolders->setChecked(true);
	
	QVBox *vbox2 = new QVBox(hbox);
	vbox2->setSpacing (5);
	
	buttonFind = new QPushButton(tr("&Find"), vbox2);
    buttonFind->setDefault( TRUE );
   
	buttonReset = new QPushButton(tr("&Reset Start From"), vbox2);
    buttonCancel = new QPushButton(tr("&Close"), vbox2);
	
	QVBoxLayout* hlayout = new QVBoxLayout(this, 5, 5);
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
