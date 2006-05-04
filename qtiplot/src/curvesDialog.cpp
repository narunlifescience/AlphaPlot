#include "curvesDialog.h"
#include "graph.h"
#include "worksheet.h"

#include <qlabel.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qvbox.h>
#include <qpopupmenu.h>
#include <qaccel.h>
#include <qwidgetlist.h>

static const char * remove_xpm[] = {
"16 17 27 1",
" 	c None",
".	c #000000",
"+	c #BDD3BD",
"@	c #B5CBB5",
"#	c #B5C7B5",
"$	c #B5CFB5",
"%	c #BDCFB5",
"&	c #BDCFBD",
"*	c #7BAA7B",
"=	c #ADC7AD",
"-	c #B5CBAD",
";	c #ADC7A5",
">	c #6B8263",
",	c #292C29",
"'	c #ADCBAD",
")	c #B5C7AD",
"!	c #638E63",
"~	c #9CAE9C",
"{	c #84AE84",
"]	c #84B284",
"^	c #8CAE84",
"/	c #8CB284",
"(	c #527152",
"_	c #395131",
":	c #5A7952",
"<	c #4A6142",
"[	c #425539",
"        .       ",
"       ..       ",
"      .+.       ",
"     .@#.       ",
"    .@@#........",
"   .@@$$%+%&+@*.",
"  .@=@@-=====;>.",
" ,@')=='===)='!.",
".~{{{{{{]^{]/{(.",
" ._:::::::::::<.",
"  ._::::::::::<.",
"   ._::<______[.",
"    ._:<........",
"     ._<.       ",
"      .<.       ",
"       ..       ",
"        .       "};

static const char * add_xpm[] = {
"16 17 26 1",
" 	c None",
".	c #000000",
"+	c #8CA684",
"@	c #BAD4B8",
"#	c #73A26B",
"$	c #BAD4BA",
"%	c #ADCBAD",
"&	c #739A63",
"*	c #BDD7BD",
"=	c #BAD3B8",
"-	c #BAD3BA",
";	c #ADC7AD",
">	c #081008",
",	c #ADC7A5",
"'	c #A5C7A5",
")	c #7B9E73",
"!	c #A5BEA5",
"~	c #A5C3A5",
"{	c #6B9263",
"]	c #738E6B",
"^	c #6B8E63",
"/	c #425929",
"(	c #6B8663",
"_	c #5A7952",
":	c #63825A",
"<	c #526942",
"       .        ",
"       ..       ",
"       .+.      ",
"       .@#.     ",
"........$%&.    ",
".*******=-;&>   ",
".;%;;;,,;%;').  ",
".!;,',,;%;;%~{. ",
".]^{{{{{{{{{{^/.",
".(___________/. ",
".(__________/.  ",
".:<<<<<<___/.   ",
"........<_/.    ",
"       .</.     ",
"       ./.      ",
"       ..       ",
"       .        "};

static const char * dropLines_xpm[] = {
"16 16 3 1",
" 	c None",
".	c #000000",
"+	c #CC3737",
"                ",
"  .     +++     ",
"  .     +++     ",
" ..     +++     ",
"  .      .      ",
"  . +++  .      ",
" .. +++  .  +++ ",
"  . +++  .  +++ ",
"  .  .   .  +++ ",
" ..  .   .   .  ",
"  .  .   .   .  ",
"  .  .   .   .  ",
" ..  .   .   .  ",
"  .  .   .   .  ",
"  ..............",
"    .  .  .  .  "};

static const char * spline_xpm[] = {
"17 13 2 1",
" 	c None",
".	c #000000",
"                 ",
"           ..    ",
"            ..   ",
"          .   .  ",
"  ..      .   .  ",
"   .     .    .  ",
"  .      ..    . ",
"  .      .    .. ",
"  .     .        ",
"   .    .        ",
"   ..  ..        ",
"     ..          ",
"                 "};

static const char * steps_xpm[] = {
"16 16 2 1",
" 	c None",
".	c #050404",
"                ",
"                ",
"     ....       ",
"     .  .     ..",
" ..  .  .     . ",
"  .  .  . ... . ",
"  .  .  . . . . ",
"  .  .  . . . . ",
"  ....  . . ... ",
"        . .     ",
"        . .     ",
"        . .     ",
"        . .     ",
"        ...     ",
"                ",
"                "};

static const char *pPlot_xpm[]={
"13 13 2 1",
". c None",
"# c #000000",
"..........###",
"..........###",
"..........###",
".............",
".............",
".....###.....",
".....###.....",
".....###.....",
".............",
".............",
"###..........",
"###..........",
"###.........."};


static const char *lpPlot_xpm[]={
"13 13 2 1",
". c None",
"# c #000000",
"..........###",
"..........###",
"..........###",
".........#...",
"........#....",
".....###.....",
".....###.....",
".....###.....",
"....#........",
"...#.........",
"###..........",
"###..........",
"###.........."};

static const char *lPlot_xpm[]={
"14 14 2 1",
". c None",
"# c #000000",
".............#",
"............#.",
"...........#..",
"..........#...",
".........#....",
"........#.....",
".......#......",
"......#.......",
".....#........",
"....#.........",
"...#..........",
"..#...........",
".#............",
"#............."};

static const char * area_xpm[] = {
"16 15 4 1",
" 	c None",
".	c #00FF00",
"+	c #FF0000",
"@	c #000000",
"   .        .   ",
"  ...      ...  ",
" .....    ..... ",
" ......  .......",
"................",
"++.....++++....+",
"+++...++++++..++",
"+++++++++@@+++++",
"+++@@+++@@@@++++",
"++@@@@+@@@@@@+++",
"+@@@@@@@@@@@@+++",
"@@@@@@@@@@@@@@@@",
"@@@@@@@@@@@@@@@@",
"@@@@@@@@@@@@@@@@",
"@@@@@@@@@@@@@@@@"};

static const char * hBars_xpm[] = {
"15 15 3 1",
" 	c None",
".	c #000000",
"+	c #FF0000",
"...........    ",
"++++++++++.    ",
"++++++++++.    ",
"++++++++++.    ",
"...........    ",
"...............",
"++++++++++++++.",
"++++++++++++++.",
"++++++++++++++.",
"...............",
"...........    ",
"++++++++++.    ",
"++++++++++.    ",
"++++++++++.    ",
"...........    "};

static const char * vertBars_xpm[] = {
"15 15 3 1",
" 	c None",
".	c #000000",
"+	c #FF0000",
"     .....     ",
"     .+++.     ",
"     .+++.     ",
"     .+++.     ",
"......+++......",
".+++..+++..+++.",
".+++..+++..+++.",
".+++..+++..+++.",
".+++..+++..+++.",
".+++..+++..+++.",
".+++..+++..+++.",
".+++..+++..+++.",
".+++..+++..+++.",
".+++..+++..+++.",
".+++..+++..+++."};

curvesDialog::curvesDialog( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "curvesDialog" );
	setMinimumSize(QSize(400,200));
    setCaption( tr( "QtiPlot - Add/Remove curves" ) );
	setFocus();
	
	QHBox *box5 = new QHBox (this, "box5"); 
	box5->setSpacing (5);
	box5->setMargin(5);

	new QLabel("New curves style", box5, "label0");
	boxStyle = new QComboBox (box5, "boxStyle");
	boxStyle->insertItem( QPixmap(lPlot_xpm), tr( " Line" ) );
    boxStyle->insertItem( QPixmap(pPlot_xpm), tr( " Scatter" ) );
    boxStyle->insertItem( QPixmap(lpPlot_xpm), tr( " Line + Symbol" ) );
    boxStyle->insertItem( QPixmap(dropLines_xpm), tr( " Vertical drop lines" ) );
	boxStyle->insertItem( QPixmap(spline_xpm), tr( " Spline" ) );
	boxStyle->insertItem( QPixmap(steps_xpm), tr( " Vertical steps" ) );
	boxStyle->insertItem( QPixmap(area_xpm), tr( " Area" ) );
	boxStyle->insertItem( QPixmap(vertBars_xpm), tr( " Vertical Bars" ) );
	boxStyle->insertItem( QPixmap(hBars_xpm), tr( " Horizontal Bars" ) );

	QHBox  *box0 = new QHBox (this, "box0"); 
	box0->setSpacing (5);

	QVBox  *box1=new QVBox (box0, "box1"); 
	box1->setMargin(5);
	box1->setSpacing (5);
	
	TextLabel1 = new QLabel(box1, "TextLabel1" );
    TextLabel1->setText( tr( "Available data" ) );

    available = new QListBox( box1, "available" );
	available->setSelectionMode (QListBox::Multi);
	
	QVBox  *box2=new QVBox (box0, "box2"); 
	box2->setMargin(5);
	box2->setSpacing (5);

    btnAdd = new QPushButton(box2, "btnAdd" );
    btnAdd->setPixmap( QPixmap(add_xpm) );
	btnAdd->setFixedWidth (35);
	btnAdd->setFixedHeight (30);
	
    btnRemove = new QPushButton(box2, "btnRemove" );
    btnRemove->setPixmap( QPixmap(remove_xpm) );
	btnRemove->setFixedWidth (35);
	btnRemove->setFixedHeight(30);
		
	QVBox  *box3=new QVBox (box0, "box3"); 
	box3->setMargin(5);
	box3->setSpacing (5);
	
	TextLabel2 = new QLabel(box3, "TextLabel2" );
    TextLabel2->setText( tr( "Graph contents" ) );

    contents = new QListBox( box3, "contents" );
	contents->setSelectionMode (QListBox::Multi);

	QVBox  *box4=new QVBox (box0, "box4"); 
	box4->setMargin(5);
	box4->setSpacing (5);

	btnAssociations = new QPushButton(box4, "btnAssociations" );
    btnAssociations->setText( tr( "&Plot Associations..." ) );
	btnAssociations->setEnabled(false);
	
	btnEditFunction = new QPushButton(box4, "btnEditFunction" );
    btnEditFunction->setText( tr( "&Edit Function..." ) );
	btnEditFunction->setEnabled(false);
	
    btnOK = new QPushButton(box4, "btnOK" );
    btnOK->setText( tr( "OK" ) );
	btnOK->setDefault( TRUE );
	
    btnCancel = new QPushButton(box4, "btnCancel" );
    btnCancel->setText( tr( "Close" ) );

	QVBoxLayout* layout = new QVBoxLayout(this,5,5, "hlayout3");
    layout->addWidget(box5);
	layout->addWidget(box0);

connect(btnAssociations, SIGNAL(clicked()),this, SLOT(showPlotAssociations()));
connect(btnEditFunction, SIGNAL(clicked()),this, SLOT(showFunctionDialog()));

connect(btnAdd, SIGNAL(clicked()),this, SLOT(addCurve()));
connect(btnRemove, SIGNAL(clicked()),this, SLOT(removeCurve()));
connect(btnOK, SIGNAL(clicked()),this, SLOT(close()));
connect(btnCancel, SIGNAL(clicked()),this, SLOT(close()));
connect(btnAdd, SIGNAL(clicked()),this, SLOT(enableRemoveBtn()));
connect(btnRemove, SIGNAL(clicked()),this, SLOT(enableRemoveBtn()));

connect(contents, SIGNAL(highlighted (int)), this, SLOT(showCurveBtn(int)));
connect(contents, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)), this, SLOT(deletePopupMenu(QListBoxItem *, const QPoint &)));
connect(available, SIGNAL(rightButtonClicked(QListBoxItem *, const QPoint &)), this, SLOT(addPopupMenu(QListBoxItem *, const QPoint &)));

QAccel *accel = new QAccel(this);
accel->connectItem( accel->insertItem( Key_Delete ), this, SLOT(removeCurve()) );
}

void curvesDialog::showCurveBtn(int) 
{
QString txt= contents->currentText();
if (txt.contains("="))
	{
	btnAssociations->setEnabled(false);
	btnEditFunction->setEnabled(true);
	}
else
	{
	btnAssociations->setEnabled(true);
	btnEditFunction->setEnabled(false);
	}
}

void curvesDialog::showPlotAssociations() 
{
int curve = contents->currentItem();
if (curve < 0)
	curve = 0;
emit showPlotAssociations(curve);
close();
}

void curvesDialog::showFunctionDialog() 
{
emit showFunctionDialog(contents->currentText(), contents->currentItem());
close();
}

QSize curvesDialog::sizeHint() const 
{
return QSize(400, 200 );
}

void curvesDialog::deletePopupMenu(QListBoxItem *it, const QPoint &point)
{
selectedCurve=contents->index (it);
	
QPopupMenu contextMenu(this);
contextMenu.insertItem(tr("&Delete"), this, SLOT(removeSelectedCurve()));
contextMenu.exec(point);
}

void curvesDialog::addPopupMenu(QListBoxItem *it, const QPoint &point)
{
selectedCurve=available->index (it);
	
QPopupMenu contextMenu(this);
contextMenu.insertItem(tr("&Plot"), this, SLOT(addSelectedCurve()));
contextMenu.exec(point);
}

void curvesDialog::insertCurvesToDialog(const QStringList& names)
{
available->clear();
int i,n=names.count();	
for (i=0;i<n;i++)
	available->insertItem(names[i],i);

if (n==0)
	btnAdd->setDisabled(true);
}

void curvesDialog::setGraph(Graph *graph)
{
g = graph;
contents->insertStringList(g->curvesList(), -1);
enableRemoveBtn();
}

void curvesDialog::addSelectedCurve()
{
QStringList emptyColumns;
QString text=available->text(selectedCurve);
if (!contents->findItem(text, Qt::ExactMatch))
	{
	if (!addCurve(text))
		emptyColumns << text;
	else
		g->updatePlot();
	}
Graph::showPlotErrorMessage(this, emptyColumns);
}

void curvesDialog::addCurve()
{
QStringList emptyColumns;
for (int i=0;i<int(available->count());i++)
	{
	if (available->isSelected(i))
		{
		QString text=available->text(i);
		if (!contents->findItem(text, Qt::ExactMatch))
			{
			if (!addCurve(text))
				emptyColumns << text;
			}
		}
	}
g->updatePlot();
Graph::showPlotErrorMessage(this, emptyColumns);
}

bool curvesDialog::addCurve(const QString& name)
{
int style = curveStyle();
Table* t = findTable(name);
if (t && g->insertCurve(t, name, style))
	{
	curveLayout cl = Graph::initCurveLayout();

	int curve = g->curves() - 1;
	long key = g->curveKey(curve);	
	if (key == (long) curve)
		key++;
	
	int color = key%16;
	if (color == 13) //avoid white invisible curves
		color = 0;
			
	cl.lCol=color;
	cl.symCol=color;
	cl.fillCol=color;	
	cl.lWidth = defaultCurveLineWidth;
	cl.sSize = defaultSymbolSize;
	cl.sType=key%9;

	if (style==Graph::VerticalBars || style==Graph::HorizontalBars )
		{
		cl.filledArea=1;
		cl.lCol=0;
		cl.aCol=color;
		}
	else if (style==Graph::Area )
		{
		cl.filledArea=1;
		cl.aCol=color;
		}
	else if (style == Graph::VerticalDropLines)
		cl.connectType=2;
	else if (style == Graph::Steps)
		cl.connectType=3;
	else if (style == Graph::Spline)
		cl.connectType=5;

	g->updateCurveLayout(curve, &cl);

	contents->insertItem(name,-1);
	return true;
	}
return false;
}


void curvesDialog::setCurveDefaultSettings(int style, int width, int size)
{
defaultCurveLineWidth = width;
defaultSymbolSize = size;

if (style == Graph::Line)
	boxStyle->setCurrentItem(0);
else if (style == Graph::Scatter)
	boxStyle->setCurrentItem(1);
else if (style == Graph::LineSymbols)
	boxStyle->setCurrentItem(2);
else if (style == Graph::VerticalDropLines)
	boxStyle->setCurrentItem(3);
else if (style == Graph::Spline)
	boxStyle->setCurrentItem(4);
else if (style == Graph::Steps)
	boxStyle->setCurrentItem(5);
else if (style == Graph::Area)
	boxStyle->setCurrentItem(6);
else if (style == Graph::VerticalBars)
	boxStyle->setCurrentItem(7);
else if (style == Graph::HorizontalBars)
	boxStyle->setCurrentItem(8);	
}

Table * curvesDialog::findTable(const QString& text)
{
int pos = text.find("_", 0);
for (int i=0; i < (int)tables->count(); i++ )
	{
	if (tables->at(i)->name() == text.left(pos))
		return (Table *)tables->at(i);
	}
return 0;
}

void curvesDialog::removeSelectedCurve()
{
g->removeCurve(selectedCurve);
contents->removeItem (selectedCurve);
}

void curvesDialog::removeCurve()
{
int i;
QStringList texts;	
for (i=0;i<int(contents->count());i++)
	{
	if (contents->isSelected(i))
		texts<<contents->text(i);	
	}

for (i=0;i<int(texts.count());i++)
	{
	QListBoxItem *it=contents->findItem (texts[i],Qt::ExactMatch);
	
	int index=contents->index(it);		
	g->removeCurve(index);
	contents->removeItem(index);	
	}	
}

void curvesDialog::clear()
{
contents->clear();
btnRemove->setDisabled (TRUE);
}

void curvesDialog::enableRemoveBtn()
{
if (contents->count()>0)
	btnRemove->setEnabled (TRUE);
else
	btnRemove->setDisabled (TRUE);
}

int curvesDialog::curveStyle()
{
int style = 0;
switch (boxStyle->currentItem())
	{
	case 0:
		style = Graph::Line;
	break;
	case 1:
		style = Graph::Scatter;
	break;
	case 2:
		style = Graph::LineSymbols;
	break;
	case 3:
		style = Graph::VerticalDropLines;
	break;
	case 4:
		style = Graph::Spline;
	break;
	case 5:
		style = Graph::Steps;
	break;
	case 6:
		style = Graph::Area;
	break;
	case 7:
		style = Graph::VerticalBars;
	break;
	case 8:
		style = Graph::HorizontalBars;
	break;
	}
return style;
}

curvesDialog::~curvesDialog()
{
delete tables;
}
