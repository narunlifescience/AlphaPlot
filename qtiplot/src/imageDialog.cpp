#include "imageDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qbuttongroup.h>

imageDialog::imageDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "imageDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 310, 140 ) );
	setMaximumSize( QSize( 310, 140 ) );
    setMouseTracking( TRUE );
    setSizeGripEnabled( FALSE );
	
	GroupBox4 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),this,"GroupBox4" );
	GroupBox4->setFlat (TRUE);
	GroupBox4->setLineWidth (0);
	
	GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr("Origin"),GroupBox4,"GroupBox1" );

    new QLabel( tr( "X= " ), GroupBox1, "TextLabel1",0 );
	boxX = new QSpinBox(0,2000,1,GroupBox1, "boxX" );

    new QLabel( tr( "Y= " ), GroupBox1, "TextLabel2",0 );
	boxY = new QSpinBox(0,2000,1,GroupBox1, "boxY" );

	GroupBox3 = new QButtonGroup( 2,QGroupBox::Horizontal,tr("Size"),GroupBox4,"GroupBox3" );

    new QLabel( tr( "width= " ), GroupBox3, "TextLabel3",0 );
	boxWidth = new QSpinBox(0,2000,1,GroupBox3, "boxX" );

    new QLabel( tr( "height= " ), GroupBox3, "TextLabel4",0 );
	boxHeight = new QSpinBox(0,2000,1,GroupBox3, "boxY" );
	
	GroupBox2 = new QButtonGroup(3,QGroupBox::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	GroupBox2->setLineWidth (0);
	
	buttonApply = new QPushButton(GroupBox2, "buttonApply" );
	
	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
	
	QVBoxLayout* hlayout = new QVBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox4);
	hlayout->addWidget(GroupBox2);

    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT(update() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

imageDialog::~imageDialog()
{
}

void imageDialog::languageChange()
{
    setCaption( tr( "QtiPlot - Image Geometry" ) );
	buttonApply->setText( tr( "&Apply" ) );
    buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
}

void imageDialog::setOrigin(const QPoint& o)
{
boxX->setValue(o.x());
boxY->setValue(o.y());
}

void imageDialog::setSize(const QSize& size)
{
boxWidth->setValue(size.width());
boxHeight->setValue(size.height());
}

void imageDialog::update()
{
emit options(boxX->value(),boxY->value(),boxWidth->value(),boxHeight->value());
}

void imageDialog::accept()
{
emit options(boxX->value(),boxY->value(),boxWidth->value(),boxHeight->value());
close();
}
