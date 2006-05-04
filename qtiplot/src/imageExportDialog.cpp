#include "imageExportDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qimage.h>

imageExportDialog::imageExportDialog( bool exportAllPlots, QWidget* parent, 
																const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "imageExportDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 310, 140 ) );
	setMaximumSize( QSize( 310, 140 ) );
    setMouseTracking( TRUE );
    setSizeGripEnabled( FALSE );
	
	GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr(""),this,"GroupBox1" );

	expAll = exportAllPlots;
	if (expAll)
		{
		formatLabel = new QLabel( tr( "Image format" ), GroupBox1, "TextLabel11",0 );
	
		QStringList outputFormatList = QImage::outputFormatList ();

		#ifdef Q_OS_WIN
			;
 		#else
			outputFormatList.prepend("EPS");
		#endif
				
		boxFormat= new QComboBox(GroupBox1, "boxFormat" );
		boxFormat->insertStringList (outputFormatList);
		connect( boxFormat, SIGNAL( activated(int) ), this, SLOT( enableTransparency(int) ) );
		}
	
	new QLabel( tr( "Image quality" ), GroupBox1, "TextLabel1",0 );
	boxQuality= new QSpinBox(0, 100, 1, GroupBox1, "boxQuality" );
	boxQuality->setValue(100);
	
    boxTransparency = new QCheckBox(GroupBox1, "boxTransparency" );
	boxTransparency->setText( "Save transparency" );
    boxTransparency->setChecked( false );
	boxTransparency->setEnabled(false);
	
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

void imageExportDialog::languageChange()
{
    setCaption( tr( "QtiPlot - Export options" ) );
    buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
}

void imageExportDialog::enableTransparency(int)
{
QString type = boxFormat->currentText().lower();
	
if (type == "tif" || type == "tiff" || type == "png" || type == "xpm")
	boxTransparency->setEnabled(true);
}

void imageExportDialog::enableTransparency()
{
QString type = f_type.lower();
	
if (type == "tif" || type == "tiff" || type == "png" || type == "xpm")
	boxTransparency->setEnabled(true);
}

void imageExportDialog::accept()
{
if (expAll)
	emit exportAll(f_dir, boxFormat->currentText(), boxQuality->value(), boxTransparency->isChecked());
else
	emit options(f_name, f_type, boxQuality->value(), boxTransparency->isChecked());
close();
}


imageExportDialog::~imageExportDialog()
{
}
