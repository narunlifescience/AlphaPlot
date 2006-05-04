#include "epsExportDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qimage.h>
#include <qprinter.h>

epsExportDialog::epsExportDialog(const QString& fileName, QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
	f_name = fileName;
	
    if ( !name )
	setName( "epsExportDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setSizeGripEnabled( true );
	
	GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),this,"GroupBox1" );

	new QLabel( tr( "Orientation" ), GroupBox1, "TextLabel111",0 );
	boxOrientation= new QComboBox(GroupBox1, "Orientation" );

	new QLabel( tr( "Page Size" ), GroupBox1, "TextLabel11",0 );
	boxPageSize= new QComboBox(GroupBox1, "boxPageSize" );
		
	new QLabel( tr( "Resolution" ), GroupBox1, "TextLabel1",0 );
	boxResolution= new QSpinBox(0, 1000, 1, GroupBox1, "boxQuality" );
	boxResolution->setValue(84);
	
    boxColor= new QCheckBox(GroupBox1, "boxColor" );
	boxColor->setText( tr("&Print in color if available") );
	boxColor->setChecked(true);

	GroupBox2 = new QButtonGroup(1,QGroupBox::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	GroupBox2->setLineWidth (0);
	
	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
	
	QHBoxLayout* hlayout = new QHBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(GroupBox2);

    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void epsExportDialog::languageChange()
{
    setCaption( tr( "QtiPlot - EPS Export options" ) );
    buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
	
	boxOrientation->insertItem(tr("Landscape"));
	boxOrientation->insertItem(tr("Portrait"));
	
	boxPageSize->insertItem("A0 - 841 x 1189 mm");
	boxPageSize->insertItem("A1 - 594 x 841 mm");
	boxPageSize->insertItem("A2 - 420 x 594 mm");
	boxPageSize->insertItem("A3 - 297 x 420 mm");
	boxPageSize->insertItem("A4 - 210 x 297 mm");
	boxPageSize->insertItem("A5 - 148 x 210 mm");
	boxPageSize->insertItem("A6 - 105 x 148 mm");
	boxPageSize->insertItem("A7 - 74 x 105 mm");
	boxPageSize->insertItem("A8 - 52 x 74 mm");
	boxPageSize->insertItem("A9 - 37 x 52 mm");
	boxPageSize->insertItem("B0 - 1030 x 1456 mm");
	boxPageSize->insertItem("B1 - 728 x 1030 mm");
	boxPageSize->insertItem("B2 - 515 x 728 mm");
	boxPageSize->insertItem("B3 - 364 x 515 mm");
	boxPageSize->insertItem("B4 - 257 x 364 mm");
	boxPageSize->insertItem("B5 - 182 x 257 mm");
	boxPageSize->insertItem("B6 - 128 x 182 mm");
	boxPageSize->insertItem("B7 - 91 x 128 mm");
	boxPageSize->insertItem("B8 - 64 x 91 mm");
	boxPageSize->insertItem("B9 - 45 x 64 mm");

	boxPageSize->setCurrentItem(5);
}

void epsExportDialog::accept()
{
QPrinter::Orientation o;
if (boxOrientation->currentItem() == 1)
	o = QPrinter::Portrait;
else
	o = QPrinter::Landscape;
	
QPrinter::ColorMode col = QPrinter::Color;
if (!boxColor->isChecked())
	col = QPrinter::GrayScale;

QPrinter::PageSize size = pageSize();

emit exportToEPS(f_name, boxResolution->value(), o, size, col);
close();
}

QPrinter::PageSize epsExportDialog::pageSize()
{
QPrinter::PageSize size;
switch (boxPageSize->currentItem())
	{
	case 0:
		size = QPrinter::A0;
	break;

	case 1:
		size = QPrinter::A1;
	break;

	case 2:
		size = QPrinter::A2;
	break;

	case 3:
		size = QPrinter::A3;
	break;

	case 4:
		size = QPrinter::A4;
	break;

	case 5:
		size = QPrinter::A5;
	break;

	case 6:
		size = QPrinter::A6;
	break;

	case 7:
		size = QPrinter::A7;
	break;

	case 8:
		size = QPrinter::A8;
	break;

	case 9:
		size = QPrinter::A9;
	break;

	case 10:
		size = QPrinter::B0;
	break;

	case 11:
		size = QPrinter::B1;
	break;

	case 12:
		size = QPrinter::B2;
	break;

	case 13:
		size = QPrinter::B3;
	break;

	case 14:
		size = QPrinter::B4;
	break;

	case 15:
		size = QPrinter::B5;
	break;

	case 16:
		size = QPrinter::B6;
	break;

	case 17:
		size = QPrinter::B7;
	break;

	case 18:
		size = QPrinter::B8;
	break;

	case 19:
		size = QPrinter::B9;
	break;
	}
return size;
}

epsExportDialog::~epsExportDialog()
{
}
