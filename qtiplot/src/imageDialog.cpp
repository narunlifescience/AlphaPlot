#include "imageDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qvbox.h>

/* XPM */
static const char * up_xpm[] = {
"6 4 3 1",
" 	c None",
".	c #8F8B6D",
"+	c #EFEEE4",
"..... ",
"+++++.",
"    +.",
"    +."};

/* XPM */
static const char * down_xpm[] = {
"6 5 3 1",
" 	c None",
".	c #EFEEE4",
"+	c #8F8B6D",
"    .+",
"    .+",
"    .+",
".....+",
"+++++ "};

/* XPM */
static const char * chain_xpm[] = {
"7 20 9 1",
" 	c None",
".	c #020204",
"+	c #6E6E6E",
"@	c #D0D0D1",
"#	c #B5B5B6",
"$	c #5A5A5C",
"%	c #9A9A98",
"&	c #E8E8E9",
"*	c #8F8F91",
" ..... ",
".+@@#$.",
".%...&.",
".@. .@.",
".@. .@.",
".#. .@.",
".%...@.",
".*.+.*.",
" ..#.. ",
"  .@.  ",
"  .@.  ",
" ..@.. ",
".+.#.*.",
".#...#.",
".#. .&.",
".#. .&.",
".#. .&.",
".@...&.",
".+&@&#.",
" ..... "};

/* XPM */
static const char * unchain_xpm[] = {
"7 24 9 1",
" 	c None",
".	c #020204",
"+	c #6E6E6E",
"@	c #D0D0D1",
"#	c #B5B5B6",
"$	c #5A5A5C",
"%	c #9A9A98",
"&	c #E8E8E9",
"*	c #8F8F91",
" ..... ",
".+@@#$.",
".%...&.",
".@. .@.",
".@. .@.",
".#. .@.",
".%...@.",
".*.+.*.",
" ..#.. ",
"  .@.  ",
"       ",
"       ",
"       ",
"       ",
"  .@.  ",
" ..@.. ",
".+.#.*.",
".#...#.",
".#. .&.",
".#. .&.",
".#. .&.",
".@...&.",
".+&@&#.",
" ..... "};

imageDialog::imageDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "imageDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setSizeGripEnabled( FALSE );
	
	GroupBox4 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),this,"GroupBox4" );
	GroupBox4->setFlat (TRUE);
	GroupBox4->setLineWidth (0);
	
	GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr("Origin"),GroupBox4,"GroupBox1" );

    new QLabel( tr( "X= " ), GroupBox1, "TextLabel1",0 );
	boxX = new QSpinBox(0,2000,1,GroupBox1, "boxX" );
	boxX->setSuffix(tr(" pixels"));

    new QLabel( tr( "Y= " ), GroupBox1, "TextLabel2",0 );
	boxY = new QSpinBox(0,2000,1,GroupBox1, "boxY" );
	boxY->setSuffix(tr(" pixels"));

	GroupBox3 = new QButtonGroup(2,QGroupBox::Horizontal,tr("Size"),GroupBox4,"GroupBox3" );
	QGroupBox *box1 = new QGroupBox ( 2, QGroupBox::Horizontal, GroupBox3);
	box1->setFlat (true);
	box1->setLineWidth (0);
	box1->setInsideMargin (0);

    new QLabel( tr( "width= " ), box1, "TextLabel3",0 );
	boxWidth = new QSpinBox(0,2000,1,box1, "boxX" );
	boxWidth->setSuffix(tr(" pixels"));

	new QLabel( tr( "height= " ), box1, "TextLabel4",0 );
	boxHeight = new QSpinBox(0,2000,1,box1, "boxY" );
	boxHeight->setSuffix(tr(" pixels"));

	QVBox *box2 = new QVBox (GroupBox3);
	QLabel *up = new QLabel(box2, 0,0 );
	up->setPixmap(QPixmap(up_xpm));

	linkButton = new ChainButton(box2);

	QLabel *down = new QLabel(box2, 0,0 );
	down->setPixmap(QPixmap(down_xpm));

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
aspect_ratio = (double)size.width()/(double)size.height();

connect( boxWidth, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustHeight(int) ) );
connect( boxHeight, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustWidth(int) ) );
}

void imageDialog::adjustHeight(int width)
{
if (linkButton->isLocked())
	{
	disconnect( boxHeight, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustWidth(int) ) );
	boxHeight->setValue(int(width/aspect_ratio));
	connect( boxHeight, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustWidth(int) ) );
	}
else
	aspect_ratio = (double)width/double(boxHeight->value());
}

void imageDialog::adjustWidth(int height)
{
if (linkButton->isLocked())
	{
	disconnect( boxWidth, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustHeight(int) ) );
	boxWidth->setValue(int(height*aspect_ratio));
	connect( boxWidth, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustHeight(int) ) );
	}
else
	aspect_ratio = double(boxWidth->value())/(double)height;
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

ChainButton::ChainButton(QWidget *parent) : QPushButton(parent)
{
locked = true;
setFlat (true);
setAutoDefault (false);
setIconSet (QPixmap(chain_xpm));
setMaximumWidth(20);

connect (this, SIGNAL(clicked()), this, SLOT(changeLock()));
}

void ChainButton::changeLock() 
{
locked = !locked;
if (locked)
	setIconSet (QPixmap(chain_xpm));
else
	setIconSet (QPixmap(unchain_xpm));
}
