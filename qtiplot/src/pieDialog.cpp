#include "pieDialog.h"
#include "textDialog.h"
#include "colorBox.h"
#include "colorButton.h"
#include "patternBox.h"
#include "graph.h"
#include "multilayer.h"
#include "plot.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qbuttongroup.h>
#include <qcolordialog.h>
#include <qwidgetlist.h>
#include <qpopupmenu.h>

#include <qwt_plot.h>


pieDialog::pieDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "pieDialog" );
	setFixedWidth(521);
	setFixedHeight(260);
    setCaption( tr( "QtiPlot - Pie Options" ) );
    setSizeGripEnabled( FALSE );

    generalDialog = new QTabWidget( this, "generalDialog" );
	
	initPiePage();
	initBorderPage();
	
	QButtonGroup *GroupBox1 = new QButtonGroup(4,QGroupBox::Horizontal,tr(""),this, "GroupBox1" );
	GroupBox1->setLineWidth(0);
	GroupBox1->setFlat (TRUE);
	
	buttonWrk = new QPushButton( GroupBox1, "buttonWrk" );
    buttonWrk->setText( tr( "&Worksheet" ) );
	
	buttonApply = new QPushButton( GroupBox1, "buttonApply" );
    buttonApply->setText( tr( "&Apply" ) );
	
    buttonOk = new QPushButton(GroupBox1, "buttonOk" );
    buttonOk->setText( tr( "&OK" ) );
    buttonOk->setDefault( TRUE );
	
    buttonCancel = new QPushButton(GroupBox1, "buttonCancel" );
    buttonCancel->setText( tr( "&Cancel" ) );
	
	QVBoxLayout* vl = new QVBoxLayout(this,5,5, "vl");
	vl->addWidget(generalDialog);
    vl->addWidget(GroupBox1);
   
    // signals and slots connections
	connect( buttonWrk, SIGNAL(clicked()), this, SLOT(showWorksheet()));
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT(updatePlot() ) );
}

void pieDialog::initPiePage()
{
	pieOptions = new QWidget( generalDialog, "pieOptions" );
	curvesList = new QListBox(pieOptions, "listBox" );
	
	QButtonGroup *GroupBox3 = new QButtonGroup(2,QGroupBox::Horizontal,tr( "Border" ),pieOptions, "GroupBox3" );

    new QLabel( tr( "Color" ), GroupBox3, "TextLabel4",0 );

    boxLineColor = new ColorBox( FALSE, GroupBox3);
	
	new QLabel(tr( "Style" ), GroupBox3, "TextLabel31",0 );  
    boxLineStyle = new QComboBox( FALSE, GroupBox3, "boxLineStyle" );
    boxLineStyle->insertItem("_____");
	boxLineStyle->insertItem("- - -");
	boxLineStyle->insertItem(".....");
	boxLineStyle->insertItem("_._._");
	boxLineStyle->insertItem("_.._..");
	
	new QLabel(tr( "Width" ), GroupBox3, "TextLabel3",0 );  
    boxLineWidth = new QSpinBox( GroupBox3, "boxLineWidth" );
	
	QButtonGroup *GroupBox2 = new QButtonGroup(2,QGroupBox::Horizontal,tr( "Fill" ),pieOptions, "GroupBox2" );

    new QLabel( tr( "First color" ), GroupBox2, "TextLabel4",0 ); 
    boxFirstColor = new ColorBox( FALSE, GroupBox2);
	
	new QLabel( tr( "Pattern" ), GroupBox2, "TextLabel41",0 );
	boxPattern = new PatternBox( FALSE, GroupBox2);
	
	new QLabel(tr( "Pie ray" ), GroupBox2, "rayLabel",0 );  
    boxRay= new QSpinBox(0,2000,10,GroupBox2, "boxRay");
	
	QHBoxLayout* hlayout1 = new QHBoxLayout(pieOptions,5,5, "hlayout1");
	hlayout1->addWidget(curvesList);
    hlayout1->addWidget(GroupBox3);
	hlayout1->addWidget(GroupBox2);
	
	generalDialog->insertTab(pieOptions, tr( "Pie" ) );

connect(curvesList, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)), this, SLOT(showPopupMenu(QListBoxItem *, const QPoint &)));
}

void pieDialog::initBorderPage()
{
frame = new QWidget( generalDialog, "frame" );	

QButtonGroup *GroupBox1 = new QButtonGroup(2,QGroupBox::Horizontal, tr("Background"), frame, "GroupBox5" );
	
new QLabel(tr("Color"), GroupBox1, "TextLabel1_53",0 );
boxBackgroundColor= new ColorButton(GroupBox1);
	
new QLabel(tr("Border Width"),GroupBox1, "TextLabel1_54",0 );
boxBorderWidth= new QSpinBox(GroupBox1);

new QLabel(tr("Border Color" ),GroupBox1, "TextLabel1_53",0 );
boxBorderColor= new ColorButton(GroupBox1);

QButtonGroup *GroupBox2 = new QButtonGroup(2,QGroupBox::Horizontal,tr("Options"), frame, "GroupBox2" );
new QLabel(tr( "Margin" ),GroupBox2, "TextLabel1_541",0 );
boxMargin= new QSpinBox(0, 1000, 5, GroupBox2);

boxAll = new QCheckBox(tr("Apply to all layers"), GroupBox2, "boxShowAxis" );

QHBoxLayout* hlayout = new QHBoxLayout(frame, 5, 5, "hlayout");
hlayout->addWidget(GroupBox1);
hlayout->addWidget(GroupBox2);

generalDialog->insertTab(frame, tr( "General" ) );
connect(boxMargin, SIGNAL(valueChanged (int)), this, SLOT(changeMargin(int)));
connect(boxBorderColor, SIGNAL(clicked()), this, SLOT(pickBorderColor()));
connect(boxBackgroundColor, SIGNAL(clicked()), this, SLOT(pickBackgroundColor()));
connect(boxBorderWidth,SIGNAL(valueChanged (int)), this, SLOT(updateBorder(int)));
}

void pieDialog::setMultiLayerPlot(MultiLayer *m)
{
mPlot = m;
Graph* g = (Graph*)mPlot->activeGraph();
Plot *p = g->plotWidget();
	
boxMargin->setValue (p->margin());
boxBorderWidth->setValue(p->lineWidth());
boxBorderColor->setColor(p->frameColor());
boxBackgroundColor->setColor(p->paletteBackgroundColor());
}

void pieDialog::pickBackgroundColor()
{
QColor c = QColorDialog::getColor(boxBackgroundColor->color(), this);
if ( !c.isValid() || c == boxBackgroundColor->color() )
	return;

boxBackgroundColor->setColor ( c ) ;

if (boxAll->isChecked())
	{
	QWidgetList* allPlots = mPlot->graphPtrs();
	for (int i=0; i<(int)allPlots->count();i++)
		{
		Graph* g=(Graph*)allPlots->at(i);
		if (g)
			g->setBackgroundColor(c);
		}
	}
else
	{
	Graph* g = (Graph*)mPlot->activeGraph();
	if (g)
		g->setBackgroundColor(c);
	}

if (c == QColor(white) && mPlot->hasOverlapingLayers())
	mPlot->updateTransparency();
}

void pieDialog::pickBorderColor()
{
QColor c = QColorDialog::getColor(boxBorderColor->color(), this);
if ( !c.isValid() || c == boxBorderColor->color() )
	return;

boxBorderColor->setColor ( c ) ;

if (boxAll->isChecked())
	{
	QWidgetList* allPlots = mPlot->graphPtrs();
	for (int i=0; i<(int)allPlots->count();i++)
		{
		Graph* g=(Graph*)allPlots->at(i);
		if (g)
			g->drawBorder(boxBorderWidth->value(), c);
		}
	}
else
	{
	Graph* g = (Graph*)mPlot->activeGraph();
	if (g)
		g->drawBorder(boxBorderWidth->value(), c);
	}
}

void pieDialog::updateBorder(int width)
{
if (generalDialog->currentPage() != frame)
	return;

if (boxAll->isChecked())
	{
	QWidgetList* allPlots = mPlot->graphPtrs();
	for (int i=0; i<(int)allPlots->count();i++)
		{
		Graph* g=(Graph*)allPlots->at(i);
		if (g)
			g->drawBorder(width, boxBorderColor->color());
		}
	}
else
	{
	Graph* g = (Graph*)mPlot->activeGraph();
	if (g)
		g->drawBorder(width, boxBorderColor->color());
	}
}

void pieDialog::changeMargin(int width)
{
if (generalDialog->currentPage() != frame)
	return;

if (boxAll->isChecked())
	{
	QWidgetList* allPlots = mPlot->graphPtrs();
	for (int i=0; i<(int)allPlots->count();i++)
		{
		Graph* g=(Graph*)allPlots->at(i);
		if (g)
			g->changeMargin(width);
		}
	}
else
	{
	Graph* g = (Graph*)mPlot->activeGraph();
	if (g)
		g->changeMargin(width);
	}
}

void pieDialog::showWorksheet()
{
emit worksheet(curvesList->currentText ());
close();
}

void pieDialog::showPopupMenu(QListBoxItem *, const QPoint &point)
{
QPopupMenu contextMenu(this);
contextMenu.insertItem("&Delete", this, SLOT(removeCurve()));
contextMenu.exec(point);
}

void pieDialog::removeCurve()
{
emit toggleCurve();
curvesList->removeItem (0);
}

void pieDialog::setPieSize(int size)
{
boxRay->setValue(size);
}

void pieDialog::setFramed(bool ok)
{
boxFramed->setChecked(ok);
}

void pieDialog::drawFrame(bool framed)
{
boxFrameWidth->setEnabled(framed);
boxFrameColor->setEnabled(framed);
	
emit drawFrame(framed,boxFrameWidth->text().toInt(),boxFrameColor->color());
}

void pieDialog::setFrameWidth(int w)
{
boxFrameWidth->setValue(w);
}

void pieDialog::setFrameColor(const QColor& c)
{
  boxFrameColor->setColor(c);
}

void pieDialog::insertCurveName(const QString& name)
{
curvesList->clear();
curvesList->insertItem(name,-1);
curvesList->setCurrentItem (0);
}

void pieDialog::accept()
{
updatePlot();
close();
}

void pieDialog::updatePlot()
{
if (generalDialog->currentPage()==(QWidget *)pieOptions)
	{
	QPen pen=QPen(boxLineColor->color(),boxLineWidth->value(), style());
	emit updatePie(pen, pattern(), boxRay->value(), boxFirstColor->currentItem());	
	}
	
if (generalDialog->currentPage()==(QWidget*)frame)
	{
	if (!boxAll->isChecked())
		return;
	
	QColor c = boxBackgroundColor->color();
	QWidgetList* allPlots = mPlot->graphPtrs();
	for (int i=0; i<(int)allPlots->count();i++)
		{
		Graph* g=(Graph*)allPlots->at(i);
		if (g)
			{
			g->drawBorder(boxBorderWidth->value(), boxBorderColor->color());
			g->changeMargin(boxMargin->value());
			g->setBackgroundColor(c);
			}
		}
	if (c == QColor(white) && mPlot->hasOverlapingLayers())
		mPlot->updateTransparency();
	}
}

void pieDialog::setBorderWidth(int width)
{
boxLineWidth->setValue(width);
}

void pieDialog::setFirstColor(int c)
{
boxFirstColor->setCurrentItem(c);	
}

void pieDialog::setBorderColor(const QColor& c)
{
  boxLineColor->setColor(c);
}

Qt::PenStyle pieDialog::style()
{
Qt::PenStyle style;
switch (boxLineStyle->currentItem())
	{
	case 0:
		style=Qt::SolidLine;
	break;
	case 1:
		style=Qt::DashLine;
	break;
	case 2:
		style=Qt::DotLine;
	break;
	case 3:
		style=Qt::DashDotLine;
	break;
	case 4:
		style=Qt::DashDotDotLine;
	break;
	}
return style;
}

void pieDialog::setBorderStyle(const Qt::PenStyle& style)
{
if(style == Qt::SolidLine)
	boxLineStyle->setCurrentItem(0);
if(style == Qt::DashLine)
	boxLineStyle->setCurrentItem(1);
if(style == Qt::DotLine)
	boxLineStyle->setCurrentItem(2);
if(style == Qt::DashDotLine)
	boxLineStyle->setCurrentItem(3);
if(style == Qt::DashDotDotLine)
	boxLineStyle->setCurrentItem(4);
}

void pieDialog::setPattern(const Qt::BrushStyle& style)
{
  boxPattern->setPattern(style);
}


Qt::BrushStyle pieDialog::pattern()
{
  return boxPattern->getSelectedPattern();
}

void pieDialog::showGeneralPage()
{
generalDialog->showPage (frame);
}

pieDialog::~pieDialog()
{
}
