#include "layerDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qmessagebox.h>
#include <qhbox.h>
#include <qfont.h>
#include <qfontdialog.h>

layerDialog::layerDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "layerDialog" );
    setSizeGripEnabled( true );

	generalDialog = new QTabWidget( this, "generalDialog" );

	layout = new QWidget( generalDialog, "layout" );

	optionsBox=new QHBox(layout, "optionsBox");
	optionsBox->setSpacing(5);

	GroupBox1 = new QButtonGroup( 2,QGroupBox::Horizontal,tr("Grid"),optionsBox,"GroupBox1" );

    new QLabel( tr( "Columns" ), GroupBox1, "TextLabel1",0 );
	boxX = new QSpinBox(1,100,1,GroupBox1, "boxX" );

    new QLabel( tr( "Rows" ), GroupBox1, "TextLabel2",0 );
	boxY = new QSpinBox(1,100,1,GroupBox1, "boxY" );

	GroupBox4 = new QButtonGroup( 2,QGroupBox::Horizontal,tr("Spacing"),optionsBox,"GroupBox4" );

  	new QLabel( tr( "Columns gap" ), GroupBox4, "TextLabel4",0 );
	boxColsGap = new QSpinBox(0,100,1,GroupBox4, "boxColsGap" );

    new QLabel( tr( "Rows gap" ), GroupBox4, "TextLabel5",0 );
	boxRowsGap = new QSpinBox(0,100,1,GroupBox4, "boxRowsGap" );

	fitBox=new QCheckBox(layout,"fit");
	fitBox->setChecked(FALSE);

	QVBoxLayout* hlayout1 = new QVBoxLayout(layout,5,5, "hlayout1");
	hlayout1->addWidget(fitBox);
	hlayout1->addWidget(optionsBox);
   
	generalDialog->insertTab(layout, tr( "Layout" ) );

	fonts = new QWidget( generalDialog, "fonts" );
	GroupBox2 = new QButtonGroup( 2,QGroupBox::Horizontal, QString::null,fonts,"GroupBox2" );

    btnTitle = new QPushButton(GroupBox2, "btnTitle" );
	btnAxisLegend = new QPushButton(GroupBox2, "btnAxisLegend" );
	btnAxisNumbers = new QPushButton(GroupBox2, "btnAxisNumbers" );
	btnLegend = new QPushButton(GroupBox2, "btnLegend" );

	QVBoxLayout* vl2 = new QVBoxLayout(fonts,5,5, "vl2");
	vl2->addWidget(GroupBox2);
 
	generalDialog->insertTab(fonts, tr( "Fonts" ) );

	QHBox *hbox2=new QHBox(this, "hbox2");
	hbox2->setSpacing(5);

	buttonApply = new QPushButton(hbox2, "buttonApply" );

	buttonOk = new QPushButton(hbox2, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );

    buttonCancel = new QPushButton(hbox2, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );

	QVBoxLayout* vl = new QVBoxLayout(this,10, 5, "vl");
	vl->addWidget(generalDialog);
    vl->addWidget(hbox2);

    languageChange();

    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT(update() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( btnTitle, SIGNAL( clicked() ), this, SLOT( setTitlesFont() ) );
	connect( btnAxisLegend, SIGNAL( clicked() ), this, SLOT( setAxisLegendFont() ) );
	connect( btnAxisNumbers, SIGNAL( clicked() ), this, SLOT( setAxisNumbersFont() ) );
    connect( btnLegend, SIGNAL( clicked() ), this, SLOT( setLegendsFont() ) );
	connect( fitBox, SIGNAL( toggled(bool) ), this, SLOT(enableLayoutOptions(bool) ) );

}

void layerDialog::enableLayoutOptions(bool ok)
{
GroupBox1->setEnabled(!ok);
}

layerDialog::~layerDialog()
{
}

void layerDialog::languageChange()
{
    setCaption( tr( "QtiPlot - Arrange Layers" ) );
	buttonApply->setText( tr( "&Apply" ) );
    buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
	fitBox->setText(tr("Automatic &layout"));

	btnTitle->setText("Titles");
	btnAxisLegend->setText("Axis Legends");
	btnAxisNumbers->setText("Axis Numbers");
	btnLegend->setText("Legends");
}

void layerDialog::setGraphsNumber(int g)
{
graphs=g;
};

void layerDialog::setColumns(int c)
{
boxX->setValue(c);
}

void layerDialog::setRows(int r)
{
boxY->setValue(r);
}

void layerDialog::setSpacing(int colsGap, int spaceGap)
{
boxColsGap->setValue(colsGap);
boxRowsGap->setValue(spaceGap);
}

void layerDialog::update()
{
if (generalDialog->currentPage()==(QWidget *)layout )
	{
	int cols=boxX->value();
	int rows=boxY->value();
		
	if (cols>graphs && !fitBox->isChecked())
		{
		QMessageBox::about(0,tr("QtiPlot - Columns input error"),
			tr("The number of columns you've entered is greater than the number of graphs ("+QString::number(graphs)+")!"));
		boxX->setFocus();
		return;
		}

	if (rows>graphs && !fitBox->isChecked())
		{
		QMessageBox::about(0,tr("QtiPlot - Rows input error"),
tr("The number of rows you've entered is greater than the number of graphs ("+QString::number(graphs)+")!"));
		boxY->setFocus();
		return;
		}

	emit options(cols, rows, boxColsGap->value(), boxRowsGap->value(), fitBox->isChecked());
	}

if (generalDialog->currentPage()==(QWidget *)fonts)
	{
	emit setFonts(titleFont, axesFont, numbersFont, legendFont);
	}
}

void layerDialog::accept()
{
update();
close();
}

void layerDialog::initFonts(const QFont& titlefont, const QFont& axesfont, const QFont& numbersfont, const QFont& legendfont)
{
    axesFont = axesfont;
    titleFont = titlefont;
    numbersFont = numbersfont;
    legendFont = legendfont;
}

void layerDialog::setTitlesFont()
{
bool ok;
QFont font = QFontDialog::getFont(&ok,titleFont,this);
    if ( ok ) {
        titleFont = font;
    } else {
     return;
    }
}

void layerDialog::setAxisLegendFont()
{
bool ok;
QFont font = QFontDialog::getFont(&ok,axesFont,this);
    if ( ok ) {
        axesFont = font;
    } else {
     return;
    }
}

void layerDialog::setAxisNumbersFont()
{
bool ok;
QFont font = QFontDialog::getFont(&ok,numbersFont,this);
    if ( ok ) {
        numbersFont = font;
    } else {
     return;
    }
}

void layerDialog::setLegendsFont()
{
bool ok;
QFont font = QFontDialog::getFont(&ok, legendFont,this);
    if ( ok ) {
        legendFont = font;
    } else {
     return;
    }
}
