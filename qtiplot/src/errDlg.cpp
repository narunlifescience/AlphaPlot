#include "errDlg.h"
#include "worksheet.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qradiobutton.h>
#include <qlineedit.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qbuttongroup.h>
#include <qwidgetlist.h>

errDialog::errDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "errDialog" );

    setFocusPolicy( QDialog::StrongFocus );
    setSizeGripEnabled( true );

	QVBox *vbox1=new QVBox (this,"vbox1");
	vbox1->setSpacing (5);
	
	QHBox *hbox1=new QHBox (vbox1,"hbox1");
    TextLabel1 = new QLabel(hbox1, "TextLabel1" );
    nameLabel = new QComboBox( FALSE,hbox1, "nameLabel" );

    GroupBox1 = new QButtonGroup(2,QGroupBox::Horizontal,tr(""), vbox1 , "GroupBox1" );
	GroupBox1->setRadioButtonExclusive ( TRUE );
	
	columnBox = new QRadioButton( GroupBox1, "columnBox" );
    columnBox->setChecked( TRUE );
	
    QHBox *hbox2 = new QHBox(GroupBox1, "hbox2");
    tableNamesBox = new QComboBox(false, hbox2, "tableNamesBox");
    colNamesBox = new QComboBox(false, hbox2, "colNamesBox" );
		
    percentBox = new QRadioButton( GroupBox1, "percentBox" );
    percentBox->setChecked( FALSE );

    valueBox = new QLineEdit( GroupBox1, "valueBox" );
    valueBox->setAlignment( int( QLineEdit::AlignHCenter ) );
	valueBox->setEnabled(false);

	standardBox = new QRadioButton( GroupBox1, "standardBox" );

	GroupBox3 = new QButtonGroup(2,QGroupBox::Horizontal,tr(""), vbox1 , "GroupBox3" );
	GroupBox3->setRadioButtonExclusive ( TRUE );
	
    xErrBox = new QRadioButton(GroupBox3, "xErrBox" );

    yErrBox = new QRadioButton(GroupBox3, "yErrBox" );
    yErrBox->setChecked( TRUE );

    GroupBox2 = new QGroupBox(3, QGroupBox::Vertical,tr(""), this , "GroupBox2" );
	GroupBox2->setLineWidth(0);
	GroupBox2->setFlat(TRUE);
	
	buttonAdd = new QPushButton(GroupBox2 , "buttonAdd" );
    buttonAdd->setDefault( TRUE );

    buttonCancel = new QPushButton( GroupBox2, "buttonCancel" );
	
	QHBoxLayout* hlayout1 = new QHBoxLayout(this,5,5, "hlayout1");
	hlayout1->addWidget(vbox1);
    hlayout1->addWidget(GroupBox2);
	
    languageChange();

  // signals and slots connections
	connect( buttonAdd, SIGNAL( clicked() ), this, SLOT( add() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( percentBox, SIGNAL( toggled(bool) ), valueBox, SLOT( setEnabled(bool) ) );
	connect( columnBox, SIGNAL( toggled(bool) ), tableNamesBox, SLOT( setEnabled(bool) ) );
	connect( columnBox, SIGNAL( toggled(bool) ), colNamesBox, SLOT( setEnabled(bool) ) );
	connect( tableNamesBox, SIGNAL( activated(int) ), this, SLOT( selectSrcTable(int) ));
}

void errDialog::setCurveNames(const QStringList& names)
{
nameLabel->insertStringList (names,-1);
}

void errDialog::setSrcTables(QWidgetList* tables)
{
srcTables = tables;
tableNamesBox->clear();
for (QWidget *i=srcTables->first(); i; i=srcTables->next())
	tableNamesBox->insertItem(i->name());
tableNamesBox->setCurrentText(QStringList::split("_",nameLabel->currentText())[0]);
selectSrcTable(tableNamesBox->currentItem());
}

void errDialog::selectSrcTable(int tabnr)
{
colNamesBox->clear();
colNamesBox->insertStringList(((Table*)srcTables->at(tabnr))->colNames());
}

void errDialog::add()
{
int direction=-1;
if (xErrBox->isChecked()) direction=0; 
else direction=1;
	
if (columnBox->isChecked())
	emit options(nameLabel->currentText(), tableNamesBox->currentText()+"_"+colNamesBox->currentText(), direction);	
else
	{
	int type;
	if (percentBox->isChecked()) type=0; 
		else type=1;

	emit options(nameLabel->currentText(),type, valueBox->text(), direction);
	}
}

errDialog::~errDialog()
{
}

void errDialog::languageChange()
{
    setCaption( tr( "QtiPlot - Error Bars" ) );
    xErrBox->setText( tr( "&X Error Bars" ) );
	buttonAdd->setText( tr( "&Add" ) );
    TextLabel1->setText( tr( "Add Error Bars to" ) );
    GroupBox1->setTitle( tr( "Source of errors" ) );
    percentBox->setText( tr( "Percent of data (%)" ) );
    valueBox->setText( tr( "5" ) );
    standardBox->setText( tr( "Standard Deviation of Data" ) );
    yErrBox->setText( tr( "&Y Error Bars" ) );
    buttonCancel->setText( tr( "&Close" ) );
	columnBox->setText("Existing column");
}
