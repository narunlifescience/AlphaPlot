#include "functionDialogui.h"

#include <qvariant.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qwidgetstack.h>
#include <qwidget.h>
#include <qlineedit.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qhbox.h>

functionDialogui::functionDialogui( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "functionDialogui" );
	 setMinimumSize( QSize( 610, 440 ) );
	setMaximumSize( QSize( 610, 440 ) );
    setMouseTracking( TRUE );

	QHBox *hbox1=new QHBox(this, "hbox1");
	hbox1->setSpacing(5);
	
	textFunction_2 = new QLabel( hbox1, "textFunction_2" );
	boxType = new QComboBox( FALSE, hbox1, "boxType" );

    optionStack = new QWidgetStack( this, "optionStack" );
    optionStack->setFrameShape( QWidgetStack::GroupBoxPanel );
    optionStack->setFrameShadow( QWidgetStack::Plain );

    functionPage = new QWidget( optionStack, "functionPage" );
	
	GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),functionPage,"GroupBox1" );
	GroupBox1->setLineWidth(0);
	GroupBox1->setFlat(TRUE);

    textFunction = new QLabel( GroupBox1, "textFunction" );
	boxFunction = new QComboBox( FALSE, GroupBox1, "boxFunction" );
    boxFunction->setEditable( TRUE );

	textFrom = new QLabel( GroupBox1, "textFrom" );
    boxFrom = new QLineEdit( GroupBox1, "boxFrom" );
	boxFrom->setText("0");
	
    textTo = new QLabel( GroupBox1, "textTo" );
	boxTo = new QLineEdit( GroupBox1, "boxTo" );
	boxTo->setText("1");

    textPoints = new QLabel( GroupBox1, "textPoints" );
	boxPoints = new QSpinBox(2,1000000,100, GroupBox1, "boxPoints" );
	boxPoints->setValue(100);

	QHBoxLayout* hlayout = new QHBoxLayout(functionPage,5,5, "hlayout");
	hlayout->addWidget(GroupBox1);
    
    optionStack->addWidget( functionPage, 0 );

    parametricPage = new QWidget( optionStack, "parametricPage" );

	GroupBox2 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""), parametricPage,"GroupBox2" );
	GroupBox2->setLineWidth(0);
	GroupBox2->setFlat(TRUE);
	
    textParameter = new QLabel( GroupBox2, "textParameter" );
    boxParameter = new QLineEdit( GroupBox2, "boxParameter" );
	boxParameter->setText("m");
	
	textParameterFrom = new QLabel( GroupBox2, "textParameterFrom" );
	boxParFrom = new QLineEdit( GroupBox2, "boxParFrom" );
	boxParFrom->setText("0");
	
	textParameterTo = new QLabel( GroupBox2, "textParameterTo" );
    boxParTo = new QLineEdit( GroupBox2, "boxParTo" );
	boxParTo->setText("1");

    textXPar = new QLabel( GroupBox2, "textXPar" );
    boxXFunction = new QComboBox(TRUE, GroupBox2, "boxXFunction" );
	
	textYPar = new QLabel( GroupBox2, "textYPar" );
    boxYFunction = new QComboBox(TRUE, GroupBox2, "boxYFunction" );
	
	textParPoints = new QLabel( GroupBox2, "textParPoints" );
	boxParPoints = new QSpinBox(2,1000000,100, GroupBox2, "boxParPoints" );
	boxParPoints->setValue(100);
	
	QHBoxLayout* hlayout2 = new QHBoxLayout(parametricPage, 5, 5, "hlayout");
	hlayout2->addWidget(GroupBox2);
	
    optionStack->addWidget( parametricPage, 1 );

    polarPage = new QWidget( optionStack, "polarPage" );

	GroupBox3 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""), polarPage,"GroupBox3" );
	GroupBox3->setLineWidth(0);
	GroupBox3->setFlat(TRUE);
	
    textPolarParameter = new QLabel( GroupBox3, "textPolarParameter" );
	boxPolarParameter = new QLineEdit( GroupBox3, "boxPolarParameter" );
	boxPolarParameter->setText ("t");
	
    textPolarFrom = new QLabel( GroupBox3, "textPolarFrom" );
	boxPolarFrom = new QLineEdit( GroupBox3, "boxPolarFrom" );
	boxPolarFrom->setText("0");
	
    textPolarTo = new QLabel( GroupBox3, "textPolarTo" );
    boxPolarTo = new QLineEdit( GroupBox3, "boxPolarTo" );
	boxPolarTo->setText("pi");

	textPolarRadius = new QLabel( GroupBox3, "textPolarRadius" );
    boxPolarRadius = new QComboBox(TRUE, GroupBox3, "boxPolarRadius" );
	
    textPolarTheta = new QLabel( GroupBox3, "textPolarTheta" );
    boxPolarTheta = new QComboBox(TRUE, GroupBox3, "boxPolarTheta" );
	
	textPolarPoints = new QLabel( GroupBox3, "textPolarPoints" );
	boxPolarPoints = new QSpinBox(2,1000000,100,GroupBox3, "boxPolarPoints" );
	boxPolarPoints->setValue(100);
	
	QHBoxLayout* hlayout3 = new QHBoxLayout(polarPage, 5, 5, "hlayout");
	hlayout3->addWidget(GroupBox3);
	
    optionStack->addWidget( polarPage, 2 );

    GroupBox4 = new QButtonGroup( 3, QGroupBox::Horizontal,tr(""), this, "buttonGroup3" );
	GroupBox4->setLineWidth(0);
	GroupBox4->setFlat(TRUE);
	
	buttonClear = new QPushButton( GroupBox4, "buttonClear" );
    buttonOk = new QPushButton( GroupBox4, "buttonOk" );
	buttonOk->setDefault(TRUE);
	
	buttonCancel = new QPushButton( GroupBox4, "buttonCancel" );
	
	QVBoxLayout* hlayout1 = new QVBoxLayout(this, 5, 5, "hlayout1");
	hlayout1->addWidget(hbox1);
    hlayout1->addWidget(optionStack);
	hlayout1->addWidget(GroupBox4);
	
    languageChange();

    connect( boxType, SIGNAL( activated(int) ), optionStack, SLOT( raiseWidget(int) ) );
}

functionDialogui::~functionDialogui()
{
}

void functionDialogui::languageChange()
{
    setCaption( tr( "QtiPlot - Add function curve" ) );
    textFunction_2->setText( tr( "Curve type " ) );
    textFunction->setText( tr( "f(x)= " ) );
    textFrom->setText( tr( "From x= " ) );
    textTo->setText( tr( "To x= " ) );
    textPoints->setText( tr( "Points" ) );
    buttonClear->setText( tr( "Clear list" ) );
    textParameter->setText( tr( "Parameter" ) );
    textParPoints->setText( tr( "Points" ) );
    textParameterTo->setText( tr( "To" ) );
    textYPar->setText( tr( "y = " ) );
    textXPar->setText( tr( "x = " ) );
    textParameterFrom->setText( tr( "From" ) );
    textPolarPoints->setText( tr( "Points" ) );
    textPolarParameter->setText( tr( "Parameter" ) );
    textPolarFrom->setText( tr( "From" ) );
    textPolarTo->setText( tr( "To" ) );
    textPolarRadius->setText( tr( "R =" ) );
    textPolarTheta->setText( tr( "Theta =" ) );
    boxType->insertItem( tr( "Function" ) );
    boxType->insertItem( tr( "Parametric plot" ) );
    boxType->insertItem( tr( "Polar plot" ) );
    buttonCancel->setText( tr( "Cancel" ) );
    buttonOk->setText( tr( "Ok" ) );
}
