/***************************************************************************
    File                 : plotDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Custom curves dialog
                           
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#include "application.h"
#include "plotDialog.h"
#include "colorBox.h"
#include "colorButton.h"
#include "patternBox.h"
#include "symbolBox.h"
#include "graph.h"
#include "worksheet.h"
#include "parser.h"
#include "Histogram.h"
#include "VectorCurve.h"
#include "ErrorBar.h"
#include "BoxCurve.h"
#include "FunctionCurve.h"

#include <q3accel.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <q3groupbox.h>
#include <q3buttongroup.h>
#include <q3hbox.h>
#include <q3vbox.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <qregexp.h>
#include <qcolordialog.h>
#include <q3popupmenu.h>
#include <qwt_counter.h>
//Added by qt3to4:
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

PlotDialog::PlotDialog( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
: QDialog( parent, name, modal, fl )
{
	if ( !name )
		setName( "PlotDialog" );
	setWindowTitle( tr( "QtiPlot - Custom curves" ) );

	Q3VBox *box1 = new Q3VBox(this);
	box1->setSpacing (5);

	listBox = new Q3ListBox( box1, "listBox" );

	Q3HBox *hbox1 = new Q3HBox ( box1);
	hbox1->setSpacing (5);

	new QLabel(tr( "Plot type" ), hbox1, "TextLabel111",0 );
	boxPlotType = new QComboBox( false, hbox1, "boxPlotType" );

	btnAssociations = new QPushButton(box1, "btnAssociations" );
	btnAssociations->setText( tr( "&Plot Associations..." ) );
	btnAssociations->hide();

	btnEditFunction = new QPushButton(box1, "btnEditFunction" );
	btnEditFunction->setText( tr( "&Edit Function..." ) );
	btnEditFunction->hide();

	Q3VBox  *box2=new Q3VBox (this); 
	box2->setSpacing (5);
	privateTabWidget = new QTabWidget(box2, "privateTabWidget" );

	initAxesPage();
	initLinePage();
	initSymbolsPage();
	initHistogramPage();
	initErrorsPage();
	initSpacingPage();
	initVectPage();
	initBoxPage();
	initPercentilePage();

	clearTabWidget();
	graph = 0;

	Q3HBox *hbox2 = new Q3HBox ( box2);
	hbox2->setSpacing (5);

	btnWorksheet = new QPushButton(hbox2, "btnWorksheet" );
	btnWorksheet->setText( tr( "&Worksheet" ) );
	btnWorksheet->setAutoDefault( true );

	buttonApply = new QPushButton(hbox2, "buttonApply" );
	buttonApply->setText( tr( "&Apply" ) );
	buttonApply->setAutoDefault( true );

	buttonOk = new QPushButton(hbox2, "buttonOk" );
	buttonOk->setText( tr( "&OK" ) );
	buttonOk->setAutoDefault( true );
	buttonOk->setDefault( true );

	buttonCancel = new QPushButton(hbox2, "buttonCancel" );
	buttonCancel->setText( tr( "&Cancel" ) );
	buttonCancel->setAutoDefault( true );

	Q3HBoxLayout* hlayout3 = new Q3HBoxLayout(this,5,5, "hlayout3");
	hlayout3->addWidget(box1);
	hlayout3->addWidget(box2);

	// signals and slots connections
	connect( buttonOk, SIGNAL(clicked()), this, SLOT(quit() ) );
	connect( buttonCancel, SIGNAL(clicked()), this, SLOT(close()));
	connect( buttonApply, SIGNAL(clicked() ), this, SLOT(acceptParams() ) );
	connect( btnWorksheet, SIGNAL(clicked()), this, SLOT(showWorksheet()));
	connect( btnAssociations, SIGNAL(clicked()), this, SLOT(showPlotAssociations()));
	connect( btnEditFunction, SIGNAL(clicked()), this, SLOT(editFunctionCurve()));

	connect(listBox, SIGNAL(doubleClicked (Q3ListBoxItem *)), this, SLOT(showPlotAssociations(Q3ListBoxItem *)));
	connect(listBox, SIGNAL(highlighted(int)), this, SLOT(updateTabWindow(int)));
	connect(listBox, SIGNAL(rightButtonClicked(Q3ListBoxItem *, const QPoint &)), this, SLOT(showPopupMenu(Q3ListBoxItem *, const QPoint &)));
	connect(boxPlotType, SIGNAL(activated(int)), this, SLOT(changePlotType(int)));

	Q3Accel *accel = new Q3Accel(this);
	accel->connectItem( accel->insertItem( Qt::Key_Delete ), this, SLOT(removeSelectedCurve()) );
}

void PlotDialog::showPlotAssociations(Q3ListBoxItem *item)
{
	if (!item)
		return;

	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (!app)
		return;

	QString text = item->text();
	if (text.contains("="))
		app->showFunctionDialog(graph, listBox->index(item));
	else
		app->showPlotAssociations(listBox->index(item));
	close();
}

void PlotDialog::showPlotAssociations()
{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (app)
		app->showPlotAssociations(listBox->currentItem());
}

void PlotDialog::changePlotType(int plotType)
{
	int curve = listBox->currentItem();
	int curveType = graph->curveType(curve);
	if (boxPlotType->count() == 1 || (curveType == plotType))
		return;

	if (curveType == Graph::VectXYAM || curveType == Graph::VectXYXY)
	{
		if ((plotType && curveType == Graph::VectXYAM) || 
				(!plotType && curveType == Graph::VectXYXY))
			return;

		clearTabWidget();
		insertTabs(curveType);

		VectorCurve *v = (VectorCurve*)graph->curve(curve);
		if (plotType)
		{
			graph->setCurveType(curve, Graph::VectXYAM);
			v->setVectorStyle(VectorCurve::XYAM);	
		}
		else
		{
			graph->setCurveType(curve, Graph::VectXYXY);
			v->setVectorStyle(VectorCurve::XYXY);
		}
		customVectorsPage(plotType);
	}
	else
	{
		clearTabWidget();
		insertTabs(plotType);

		lastSelectedCurve = curve;
		graph->setCurveType(lastSelectedCurve, plotType);	
		setActiveCurve(lastSelectedCurve);

		boxConnect->setCurrentItem(1);//show line for Line and LineSymbol plots

		QwtSymbol s = QwtSymbol(QwtSymbol::Ellipse, QBrush(), QPen(), QSize(9,9));
		if (plotType == Graph::Line)
			s.setStyle(QwtSymbol::NoSymbol);
		else if (plotType == Graph::Scatter)
			graph->setCurveStyle(curve, QwtPlotCurve::NoCurve);

		if (plotType)
		{
			boxSymbolStyle->setCurrentItem(1);
			boxFillSymbol->setChecked(true);
			boxFillColor->setEnabled(true);
		}
		graph->setCurveSymbol(curve, s);
	}

	acceptParams();
}
void PlotDialog::initAxesPage()
{
	axesPage = new QWidget( privateTabWidget);

	Q3ButtonGroup *gr = new Q3ButtonGroup(2, Qt::Horizontal, tr( "Attach curve to: " ), axesPage);

	new QLabel( tr( "x Axis" ), gr);

	boxXAxis = new QComboBox( FALSE, gr, "boxXAxis" );
	boxXAxis->insertItem(tr("Bottom"));
	boxXAxis->insertItem(tr("Top"));

	new QLabel(tr( "y Axis" ) , gr);

	boxYAxis = new QComboBox( FALSE, gr, "boxLineStyle" );
	boxYAxis->insertItem(tr("Left"));
	boxYAxis->insertItem(tr("Right"));

	Q3HBoxLayout* hlayout2 = new Q3HBoxLayout(axesPage, 5, 5);
	hlayout2->addWidget(gr);

	privateTabWidget->insertTab(axesPage, tr( "Axes" ) );
}

void PlotDialog::initLinePage()
{
	linePage = new QWidget( privateTabWidget, "linePage" );

	GroupBox3 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "" ),linePage, "GroupBox3" );

	new QLabel( tr( "Connect" ),GroupBox3, "TextLabel1",0 );  

	boxConnect = new QComboBox( false, GroupBox3, "boxConnect" );
	boxConnect->insertItem(tr("No line"));
	boxConnect->insertItem(tr("Lines"));
	boxConnect->insertItem(tr("Sticks"));
	boxConnect->insertItem(tr("Horizontal Steps"));
	boxConnect->insertItem(tr("Dots"));
	boxConnect->insertItem(tr("Spline"));
	boxConnect->insertItem(tr("Vertical Steps"));

	new QLabel(tr( "Style" ) , GroupBox3, "TextLabel2",0 );

	boxLineStyle = new QComboBox( false, GroupBox3, "boxLineStyle" );
	boxLineStyle->insertItem("_____");
	boxLineStyle->insertItem("_ _ _");
	boxLineStyle->insertItem(".....");
	boxLineStyle->insertItem("_._._");
	boxLineStyle->insertItem("_.._..");

	new QLabel(tr( "Width" ), GroupBox3, "TextLabel3",0 );  

	boxLineWidth = new QSpinBox( GroupBox3, "boxLineWidth" );
	boxLineWidth->setMinValue( 1 );
	boxLineWidth->setValue( 1 );

	new QLabel( tr( "Color" ), GroupBox3, "TextLabel4",0 );
	boxLineColor = new ColorBox( false, GroupBox3);

	fillGroupBox = new Q3ButtonGroup(2,Qt::Horizontal, QString(), linePage, "fillGroupBox" );

	boxFill = new QCheckBox(fillGroupBox, 0);
	new QLabel(  tr( "Fill area under curve" ),fillGroupBox, "TextLabel4_3",0 );  

	TextLabel4_3=new QLabel(  tr( "Fill color" ),fillGroupBox, "TextLabel4_3",0 );  
	boxAreaColor = new ColorBox( false,fillGroupBox);

	TextLabel4_4=new QLabel(tr( "Pattern" ), fillGroupBox, "TextLabel4_4",0);
	boxPattern = new PatternBox( false, fillGroupBox);

	Q3HBoxLayout* hlayout2 = new Q3HBoxLayout(linePage,5,5, "hlayout2");
	hlayout2->addWidget(GroupBox3);
	hlayout2->addWidget(fillGroupBox);

	privateTabWidget->insertTab( linePage, tr( "Line" ) );

	//line page connections
	connect(boxLineColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxConnect, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxLineStyle, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxAreaColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxPattern, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxFill, SIGNAL(toggled(bool)), this, SLOT(showAreaColor(bool)));
	connect(boxFill, SIGNAL(clicked()), this, SLOT(acceptParams()));
}

void PlotDialog::initSymbolsPage()
{	
	symbolPage = new QWidget( privateTabWidget, "symbolPage" );

	GroupBox0 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "" ),symbolPage, "GroupBox0" );

	new QLabel(tr( "Style" ), GroupBox0, "TextLabel2_2",0 );
	boxSymbolStyle = new SymbolBox( false, GroupBox0);

	new QLabel(tr( "Size" ), GroupBox0, "TextLabel3_2",0 );
	boxSymbolSize = new QSpinBox(1, 100, 1, GroupBox0, "boxSymbolSize" );
	boxSymbolSize->setValue(5);

	boxFillSymbol = new QCheckBox( tr( "Fill Color" ), GroupBox0,0 );
	boxFillColor = new ColorBox( false, GroupBox0);

	new QLabel(tr( "Edge Color" ), GroupBox0, "TextLabel4_2",0 ); 
	boxSymbolColor = new ColorBox( false, GroupBox0);

	new QLabel(tr( "Edge Width" ), GroupBox0, "TextLabel4_2",0 ); 
	boxPenWidth = new QSpinBox(1, 100, 1, GroupBox0, "boxPenWidth" );

	Q3HBoxLayout* hlayout0 = new Q3HBoxLayout(symbolPage,5,25, "hlayout");
	hlayout0->addWidget(GroupBox0);

	privateTabWidget->insertTab(symbolPage, tr( "Symbol" ) );

	//symbol page connections
	connect(boxSymbolColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxSymbolStyle, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxFillColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxFillSymbol, SIGNAL(clicked()), this, SLOT(fillSymbols()));
}

void PlotDialog::initBoxPage()
{	
	boxPage = new QWidget( privateTabWidget, "boxPage" );

	GroupBox0 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "Box" ),boxPage, "GroupBox0" );

	new QLabel(tr( "Type" ), GroupBox0, "TextLabel2_2",0 );
	boxType = new QComboBox( false, GroupBox0, "boxType" );
	boxType->insertItem(tr("No Box"));
	boxType->insertItem(tr("Rectangle"));
	boxType->insertItem(tr("Diamond"));
	boxType->insertItem(tr("Perc 10, 25, 75, 90"));
	boxType->insertItem(tr("Notch"));

	boxRangeLabel = new QLabel(tr( "Range" ), GroupBox0, "TextLabel2_2",0 );
	boxRange = new QComboBox( false, GroupBox0, "boxRange" );
	boxRange->insertItem(tr("Standard Deviation"));
	boxRange->insertItem(tr("Standard Error"));
	boxRange->insertItem(tr("Perc 25, 75"));
	boxRange->insertItem(tr("Perc 10, 90"));
	boxRange->insertItem(tr("Perc 5, 95"));
	boxRange->insertItem(tr("Perc 1, 99"));
	boxRange->insertItem(tr("Max-Min"));
	boxRange->insertItem(tr("Constant"));

	boxCoeffLabel = new QLabel(tr( "Percentile (%)" ), GroupBox0, "TextLabel3_2",0 );
	boxCoef = new QSpinBox(50, 100, 5, GroupBox0, "boxCoef" );

	boxCntLabel = new QLabel(tr( "Coefficient" ), GroupBox0, "TextLabel3_2",0 );
	boxCnt = new QwtCounter(GroupBox0);
	boxCnt->setRange(0.0, 100.0, 0.01);             // From 0.0 to 100, step 0.1
	boxCnt->setValue(1.0);
	boxCnt->setNumButtons(2);                      // Two buttons each side
	boxCnt->setIncSteps(QwtCounter::Button1, 1);   // Button 1 increments 1 step
	boxCnt->setIncSteps(QwtCounter::Button2, 50);  // Button 2 increments 5 steps

	new QLabel(tr( "Box Width" ), GroupBox0, "TextLabel3_2",0 );
	boxWidth = new QSpinBox(0, 100, 5, GroupBox0, "boxWidth" );

	GroupBox1 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "Whiskers" ),boxPage, "GroupBox0" );

	whiskerRangeLabel = new QLabel(tr( "Range" ), GroupBox1, "TextLabel2_2",0 );
	boxWhiskersRange = new QComboBox( false, GroupBox1, "boxWhiskersRange" );
	boxWhiskersRange->insertItem(tr("No Whiskers"));
	boxWhiskersRange->insertItem(tr("Standard Deviation"));
	boxWhiskersRange->insertItem(tr("Standard Error"));
	boxWhiskersRange->insertItem(tr("75-25"));
	boxWhiskersRange->insertItem(tr("90-10"));
	boxWhiskersRange->insertItem(tr("95-5"));
	boxWhiskersRange->insertItem(tr("99-1"));
	boxWhiskersRange->insertItem(tr("Max-Min"));
	boxWhiskersRange->insertItem(tr("Constant"));

	whiskerCoeffLabel = new QLabel(tr( "Percentile (%)" ), GroupBox1, "TextLabel3_2",0 );
	boxWhiskersCoef = new QSpinBox(50, 100, 5, GroupBox1, "boxWhiskersCoef" );

	whiskerCntLabel = new QLabel(tr( "Coef" ), GroupBox1, "TextLabel3_2",0 );
	whiskerCnt = new QwtCounter(GroupBox1);
	whiskerCnt->setRange(0.0, 100.0, 0.01); 
	whiskerCnt->setValue(1.0);
	whiskerCnt->setNumButtons(2);                      // Two buttons each side
	whiskerCnt->setIncSteps(QwtCounter::Button1, 1);   // Button 1 increments 1 step
	whiskerCnt->setIncSteps(QwtCounter::Button2, 50);  // Button 2 increments 5 steps

	Q3HBoxLayout* hlayout0 = new Q3HBoxLayout(boxPage,5,25, "hlayout");
	hlayout0->addWidget(GroupBox0);
	hlayout0->addWidget(GroupBox1);

	privateTabWidget->insertTab(boxPage, tr( "Box/Whiskers" ) );

	connect(boxType, SIGNAL(activated(int)), this, SLOT(setBoxType(int)));
	connect(boxRange, SIGNAL(activated(int)), this, SLOT(setBoxRangeType(int)));
	connect(boxWhiskersRange, SIGNAL(activated(int)), this, SLOT(setWhiskersRange(int)));
}

void PlotDialog::initPercentilePage()
{	
	percentilePage = new QWidget( privateTabWidget, "percentilePage" );

	Q3ButtonGroup *gb0 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "Type" ),percentilePage, "gb0" );

	new QLabel(tr( "Max" ), gb0, "TextLabel2_2",0 );
	boxMaxStyle = new SymbolBox( false, gb0);

	new QLabel(tr( "99%" ), gb0, "TextLabel2_2",0 );
	box99Style = new SymbolBox( false, gb0);

	new QLabel(tr( "Mean" ), gb0, "TextLabel2_2",0 );
	boxMeanStyle = new SymbolBox( false, gb0);

	new QLabel(tr( "1%" ), gb0, "TextLabel2_2",0 );
	box1Style = new SymbolBox( false, gb0);

	new QLabel(tr( "Min" ), gb0, "TextLabel2_2",0 );
	boxMinStyle = new SymbolBox( false, gb0);

	Q3ButtonGroup *gb1 = new Q3ButtonGroup(2,Qt::Horizontal, tr( "Symbol" ),percentilePage, "GroupBox0" );

	new QLabel(tr( "Size" ), gb1, "TextLabel3_2",0 );
	boxPercSize = new QSpinBox( gb1, "boxPercSize" );
	boxPercSize->setMinValue( 1 );

	boxFillSymbols = new QCheckBox( tr( "Fill Color" ), gb1, 0);
	boxPercFillColor = new ColorBox( false, gb1);

	new QLabel(tr( "Edge Color" ), gb1, "TextLabel4_2",0 ); 
	boxEdgeColor = new ColorBox( false, gb1);

	new QLabel(tr( "Edge Width" ), gb1, "TextLabel4_2",0 ); 
	boxEdgeWidth = new QSpinBox(0, 100, 1, gb1, "boxPenWidth" );

	Q3HBoxLayout* hlayout0 = new Q3HBoxLayout(percentilePage,5,25, "hlayout");
	hlayout0->addWidget(gb0);
	hlayout0->addWidget(gb1);

	privateTabWidget->insertTab(percentilePage, tr( "Percentile" ) );

	connect(boxMeanStyle, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxMinStyle, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxMaxStyle, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(box99Style, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(box1Style, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(box1Style, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxEdgeColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxPercFillColor, SIGNAL(activated(int)), this, SLOT(acceptParams()));
	connect(boxFillSymbols, SIGNAL(clicked()), this, SLOT(fillBoxSymbols()));
}

void PlotDialog::fillBoxSymbols()
{
	boxPercFillColor->setEnabled(boxFillSymbols->isChecked());
	acceptParams();
}

void PlotDialog::fillSymbols()
{
	boxFillColor->setEnabled(boxFillSymbol->isChecked());
	acceptParams();
}

void PlotDialog::initErrorsPage()
{
	errorsPage = new QWidget(privateTabWidget, "errorsPage" );

	GroupBox2 = new Q3ButtonGroup(1,Qt::Horizontal,tr( "Direction" ),errorsPage, "GroupBox2" );

	plusBox = new QCheckBox( GroupBox2, "plusBox" );
	plusBox->setText( tr( "Plus" ) );
	plusBox->setChecked( true );

	minusBox = new QCheckBox( GroupBox2, "minusBox" );
	minusBox->setText( tr( "Minus" ) );
	minusBox->setChecked( true );

	xBox = new QCheckBox( GroupBox2, "xBox" ); 
	xBox->setText( tr( "&X Error Bar" ) );

	GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal,tr( "Style" ),errorsPage, "GroupBox1" );

	new QLabel(tr( "Color" ), GroupBox1, "TextLabel3_3",0 );
	colorBox = new ColorButton(GroupBox1);

	new QLabel(tr( "Line Width" ), GroupBox1, "TextLabel3_3_2",0 );
	widthBox = new QComboBox( false, GroupBox1, "widthBox" );
	widthBox->insertItem( tr( "1" ) );
	widthBox->insertItem( tr( "2" ) );
	widthBox->insertItem( tr( "3" ) );
	widthBox->insertItem( tr( "4" ) );
	widthBox->insertItem( tr( "5" ) );
	widthBox->setEditable (true);
	widthBox->setCurrentItem(1);

	new QLabel(tr( "Cap Width" ) , GroupBox1, "TextLabel3_3_2_2",0 );

	capBox = new QComboBox( false, GroupBox1, "capBox" );
	capBox->insertItem( tr( "8" ) );
	capBox->insertItem( tr( "10" ) );
	capBox->insertItem( tr( "12" ) );
	capBox->insertItem( tr( "16" ) );
	capBox->insertItem( tr( "20" ) );
	capBox->setEditable (true);
	capBox->setCurrentItem(0);

	throughBox = new QCheckBox( GroupBox1, "throughBox" );
	throughBox->setText( tr( "Through Symbol" ) );

	Q3HBoxLayout* hlayout = new Q3HBoxLayout(errorsPage,5,5, "hlayout");
	hlayout->addWidget(GroupBox2);
	hlayout->addWidget(GroupBox1);

	privateTabWidget->insertTab( errorsPage, tr( "Error Bars" ) );

	connect(colorBox, SIGNAL(clicked()), this, SLOT(pickErrorBarsColor()));
	connect(xBox, SIGNAL(clicked()), this, SLOT(changeErrorBarsType()));
	connect(plusBox, SIGNAL(clicked()), this, SLOT(changeErrorBarsPlus()));
	connect(minusBox, SIGNAL(clicked()), this, SLOT(changeErrorBarsMinus()));
	connect(throughBox, SIGNAL(clicked()), this, SLOT(changeErrorBarsThrough()));
}

void PlotDialog::initHistogramPage()
{
	histogramPage = new QWidget(privateTabWidget, "histogramPage" );

	Q3HBox *GroupBoxH1 = new Q3HBox ( histogramPage);
	GroupBoxH1->setSpacing (5);

	automaticBox = new QCheckBox( GroupBoxH1, "automaticBox" );
	automaticBox->setText( tr( "Automatic Binning" ) );

	buttonStatistics = new QPushButton(GroupBoxH1, "buttonStatistics" );
	buttonStatistics->setText( tr( "&Show statistics" ) );

	GroupBoxH = new Q3ButtonGroup(2,Qt::Horizontal,tr( "" ),histogramPage, "GroupBoxH" );

	new QLabel(tr( "Bin Size" ), GroupBoxH, "TextLabel3_3",0 );
	binSizeBox = new QLineEdit(GroupBoxH);	

	new QLabel(tr( "Begin" ), GroupBoxH, "TextLabel3_3_2",0 );
	histogramBeginBox = new QLineEdit(GroupBoxH);	

	new QLabel(tr( "End" ) , GroupBoxH, "TextLabel3_3_2_2",0 );
	histogramEndBox = new QLineEdit(GroupBoxH);

	Q3VBoxLayout* hlayout = new Q3VBoxLayout(histogramPage,5,5, "hlayout");
	hlayout->addWidget(GroupBoxH1);
	hlayout->addWidget(GroupBoxH);

	connect(automaticBox, SIGNAL(clicked()), this, SLOT(setAutomaticBinning()));
	connect(buttonStatistics, SIGNAL(clicked()), this, SLOT(showStatistics() ) );

	privateTabWidget->insertTab( histogramPage, tr( "Histogram Data" ) );
}

void PlotDialog::initSpacingPage()
{
	spacingPage = new QWidget(privateTabWidget, "histogramPage" );

	Q3HBox *hb1 = new Q3HBox ( spacingPage);
	hb1->setSpacing (5);

	new QLabel(tr( "Gap Between Bars (in %)" ) , hb1, "TextLabel_gap",0 );
	gapBox=new QSpinBox(0,100,10,hb1, "gapBox");

	Q3HBox *hb2 = new Q3HBox ( spacingPage);
	hb2->setSpacing (5);

	new QLabel(tr( "Offset (in %)" ) , hb2, "TextLabel_gap",0 );
	offsetBox=new QSpinBox(-1000,1000,50,hb2, "gapBox");

	Q3VBoxLayout* hlayout = new Q3VBoxLayout(spacingPage, 35, 5, "hlayout");
	hlayout->addWidget(hb1);
	hlayout->addWidget(hb2);

	privateTabWidget->insertTab( spacingPage, tr( "Spacing" ) );
}

void PlotDialog::initVectPage()
{
	vectPage = new QWidget(privateTabWidget, "vectPage" );
	Q3VBox *box1 = new Q3VBox(vectPage);

	Q3ButtonGroup *GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal,tr( "" ),box1, "GroupBox1" );

	new QLabel(tr( "Color" ), GroupBox1, "TextLabel3_3",0 );
	vectColorBox = new ColorBox( false, GroupBox1);

	new QLabel(tr( "Line Width" ), GroupBox1, "TextLabel3_3_2",0 );
	vectWidthBox = new QSpinBox( 1,100,1, GroupBox1, "vectWidthBox" );

	GroupBox2 = new Q3ButtonGroup( 2,Qt::Horizontal,tr( "Arrowheads" ),box1, "GroupBox2" );

	new QLabel(tr( "Length" ), GroupBox2, "TextLabel3_3",0 );
	headLengthBox = new QSpinBox( 0,100,1, GroupBox2, "headLengthBox" );

	new QLabel(tr( "Angle" ), GroupBox2, "TextLabel3_3_2",0 );
	headAngleBox = new QSpinBox( 0,85,5, GroupBox2, "headAngleBox" );

	filledHeadBox = new QCheckBox(GroupBox2, "filledBox" ); 
	filledHeadBox->setText( tr( "&Filled" ) );

	Q3VBox *box2 = new Q3VBox(vectPage);
	GroupBoxVectEnd = new Q3ButtonGroup( 2,Qt::Horizontal,tr( "End Point" ),box2, "GroupBox2" );

	labelXEnd = new QLabel(tr( "X End" ), GroupBoxVectEnd, "TextLabel3_3",0 );
	xEndBox = new QComboBox( false, GroupBoxVectEnd, "headLengthBox" );

	labelYEnd = new QLabel(tr( "Y End" ), GroupBoxVectEnd, "TextLabel3_3_2",0 );
	yEndBox = new  QComboBox( false, GroupBoxVectEnd, "headAngleBox" );

	labelPosition = new QLabel(tr( "Position" ), GroupBoxVectEnd, "TextLabel3_3_2",0 );
	vectPosBox = new  QComboBox( false, GroupBoxVectEnd, "headAngleBox" );
	vectPosBox->insertItem(tr("Tail"));
	vectPosBox->insertItem(tr("Middle"));
	vectPosBox->insertItem(tr("Head"));

	Q3HBoxLayout* hlayout = new Q3HBoxLayout(vectPage,5,5, "hlayout");
	hlayout->addWidget(box1);
	hlayout->addWidget(box2);

	privateTabWidget->insertTab( vectPage, tr( "Vector" ) );
}

void PlotDialog::setGraph(Graph *g)
{
	graph = g;
	insertCurvesList();

	connect( graph, SIGNAL(modifiedFunction()), this, SLOT(insertCurvesList() ) );
	connect( graph, SIGNAL( modifiedPlotAssociation() ), this, SLOT( insertCurvesList() ) );
}

void PlotDialog::selectCurve(int index)
{
	insertTabs(graph->curveType(index));
	lastSelectedCurve = index;
	listBox->setSelected (index,true);
	setActiveCurve(index);
}

void PlotDialog::showStatistics()
{
	QString text=listBox->currentText();
	QStringList t=QStringList::split(": ", text, false);
	QStringList list=QStringList::split (",", t[1], false );
	text=t[0] + "_" + list[1].remove("(Y)");

	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (app)
	{
		Table* w=app->table(text);
		if (!w)
			return;
		QString result=graph->showHistogramStats(w, text, listBox->currentItem());
		if (!result.isEmpty())
		{
			app->logInfo+=result;
			app->showResults(true);
		}
	}
	close();
}

void PlotDialog::showPopupMenu(Q3ListBoxItem *it, const QPoint &point)
{
	if (!it)
		return;

	lastSelectedCurve=listBox->index (it);

	Q3PopupMenu contextMenu(this);
	if (listBox->count() > 1)
		contextMenu.insertItem(tr("&Delete"), this, SLOT(removeCurve()));
	QwtPlotCurve *c = graph->curve(lastSelectedCurve);
	if (!c)
		return;
	
	if (c->rtti() == FunctionCurve::RTTI)
		contextMenu.insertItem(tr("&Edit..."), this, SLOT(editFunctionCurve()));
	else
		contextMenu.insertItem(tr("&Plot Associations..."), this, SLOT(showPlotAssociations()));

	contextMenu.exec(point);
}

void PlotDialog::editFunctionCurve()
{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (app)
		app->showFunctionDialog(graph, listBox->currentItem());
}

void PlotDialog::removeSelectedCurve()
{
	int curve=listBox->currentItem();
	graph->removeCurve(curve);
	listBox->removeItem (curve);

	if (listBox->count() == 0)
		close();
}

void PlotDialog::changeErrorBarsPlus()
{
	graph->updateErrorBars(listBox->currentItem(),xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(),colorBox->color(), plusBox->isChecked(),minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::changeErrorBarsMinus()
{
	graph->updateErrorBars(listBox->currentItem(),xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(), colorBox->color(),plusBox->isChecked(),minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::changeErrorBarsThrough()
{
	graph->updateErrorBars(listBox->currentItem(),xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(), colorBox->color(),plusBox->isChecked(),minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::changeErrorBarsType()
{
	graph->updateErrorBars(listBox->currentItem(), xBox->isChecked(), widthBox->currentText().toInt(),
			capBox->currentText().toInt(), colorBox->color(), plusBox->isChecked(), minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::pickErrorBarsColor()
{
	QColor color = QColorDialog::getColor(colorBox->color(), this);
	if ( !color.isValid() || color == colorBox->color() )
		return;

	colorBox->setColor (color) ;

	graph->updateErrorBars(listBox->currentItem(),xBox->isChecked(),widthBox->currentText().toInt(),
			capBox->currentText().toInt(), color, plusBox->isChecked(),minusBox->isChecked(),
			throughBox->isChecked());
}

void PlotDialog::showAreaColor(bool show)
{
	boxAreaColor->setEnabled(show);
	TextLabel4_3->setEnabled(show);
	TextLabel4_4->setEnabled(show);
	boxPattern->setEnabled(show);
}

void PlotDialog::updateTabWindow(int curveIndex)
{
	int plot_type = setPlotType(curveIndex);
	if (plot_type < 0)
	{
		lastSelectedCurve = -1;
		return;
	}
	else if (graph->curveType(lastSelectedCurve) == plot_type)
	{
		setActiveCurve(curveIndex);
		return;
	}

	clearTabWidget();

	insertTabs(plot_type);
	setActiveCurve(curveIndex);
	lastSelectedCurve = curveIndex;
}

void PlotDialog::insertTabs(int plot_type)
{
	if (plot_type == Graph::Line)
	{
		boxConnect->setEnabled(true);
		privateTabWidget->addTab (linePage, tr("Line"));
		privateTabWidget->showPage(linePage);
	}
	else if (plot_type == Graph::Scatter)
	{
		boxConnect->setEnabled(true);
		privateTabWidget->addTab (symbolPage, tr("Symbol"));
		privateTabWidget->showPage(symbolPage);
	}
	else if (plot_type == Graph::LineSymbols)
	{
		boxConnect->setEnabled(true);
		privateTabWidget->addTab (linePage, tr("Line"));
		privateTabWidget->addTab (symbolPage, tr("Symbol"));
		privateTabWidget->showPage(symbolPage);
	}
	else if (plot_type == Graph::VerticalBars || 
			plot_type == Graph::HorizontalBars || 
			plot_type == Graph::Histogram)
	{
		boxConnect->setEnabled(false);
		privateTabWidget->addTab (linePage, tr("Pattern"));
		privateTabWidget->addTab (spacingPage, tr("Spacing"));

		if (plot_type == Graph::Histogram)
		{
			privateTabWidget->addTab (histogramPage, tr("Histogram Data"));	
			privateTabWidget->showPage(histogramPage);
		}
		else
			privateTabWidget->showPage(linePage);
	}
	else if (plot_type == Graph::VectXYXY || plot_type == Graph::VectXYAM)
	{
		boxConnect->setEnabled(true);
		privateTabWidget->addTab (linePage, tr("Line"));
		privateTabWidget->addTab (vectPage, tr("Vector"));
		customVectorsPage(plot_type == Graph::VectXYAM);
		privateTabWidget->showPage(vectPage);
	}
	else if (plot_type == Graph::ErrorBars)
	{
		privateTabWidget->addTab (errorsPage, "Error Bars");
		privateTabWidget->showPage(errorsPage);
	}
	else if (plot_type == Graph::Box)
	{
		boxConnect->setEnabled(false);
		privateTabWidget->addTab (linePage, tr("Pattern"));
		privateTabWidget->addTab (boxPage, tr("Box/Whiskers"));
		privateTabWidget->addTab (percentilePage, tr("Percentile"));
		privateTabWidget->showPage(linePage);
	}
}

void PlotDialog::clearTabWidget()
{
	privateTabWidget->removePage(linePage);
	privateTabWidget->removePage(symbolPage);
	privateTabWidget->removePage(errorsPage);
	privateTabWidget->removePage(histogramPage);
	privateTabWidget->removePage(spacingPage);
	privateTabWidget->removePage(vectPage);
	privateTabWidget->removePage(boxPage);
	privateTabWidget->removePage(percentilePage);
}

void PlotDialog::quit()
{
	if (acceptParams())
		close();
}

void PlotDialog::showWorksheet()
{
	ApplicationWindow *app = (ApplicationWindow *)this->parent();
	if (!app)
		return;

	app->showCurveWorksheet(graph->curveKey(listBox->currentItem()));
	close();
}

int PlotDialog::setPlotType(int index)
{
	int size=listBox->count();
	int curveType = graph->curveType(index);
	if (size>0 && curveType >= 0)
	{
		boxPlotType->clear();

		if (curveType == Graph::ErrorBars)
			boxPlotType->insertItem( tr( "Error Bars" ) );
		else if (curveType == Graph::VerticalBars)
			boxPlotType->insertItem( tr( "Vertical Bars" ) );
		else if (curveType == Graph::HorizontalBars)
			boxPlotType->insertItem( tr( "Horizontal Bars" ) );
		else if (curveType == Graph::Histogram)
			boxPlotType->insertItem( tr( "Histogram" ) );
		else if (curveType == Graph::VectXYXY || curveType == Graph::VectXYAM)
		{
			boxPlotType->insertItem( tr( "Vector XYXY" ) );
			boxPlotType->insertItem( tr( "Vector XYAM" ) );
			if (curveType == Graph::VectXYAM)
				boxPlotType->setCurrentItem(1);
		}
		else if (curveType == Graph::Box)
			boxPlotType->insertItem( tr( "Box" ) );
		else 
		{
			boxPlotType->insertItem( tr( "Line" ) );
			boxPlotType->insertItem( tr( "Scatter" ) );
			boxPlotType->insertItem( tr( "Line + Symbol" ) ); 

			QwtPlotCurve *c = (QwtPlotCurve*)graph->curve(index);
			if (!c)
				return Graph::Line;

			QwtSymbol s = c->symbol();
			if (s.style() == QwtSymbol::NoSymbol)
			{
				boxPlotType->setCurrentItem(0);
				return Graph::Line;
			}
			else if (c->style() == QwtPlotCurve::NoCurve)
			{
				boxPlotType->setCurrentItem(1);
				return Graph::Scatter;
			}
			else 
			{
				boxPlotType->setCurrentItem(2);
				return Graph::LineSymbols;
			}
		}
	}
	return curveType;
}

void PlotDialog::setActiveCurve(int index)
{//connected to the listBox highlighted signal, displays the current curve parameters
	int size=listBox->count();
	if (size>0)
	{
		QwtPlotCurve *c = (QwtPlotCurve*)graph->curve(index);
		if (!c)
			return;

		if (c->rtti() == FunctionCurve::RTTI)
		{
			btnAssociations->hide();
			btnEditFunction->show();
		}
		else
		{
			btnAssociations->show();
			btnEditFunction->hide();
		}

		int curveType = graph->curveType(index);

		//axes page
		boxXAxis->setCurrentItem(c->xAxis()-2);
		boxYAxis->setCurrentItem(c->yAxis());

		//line page
		int style = c->style();
		if (curveType == Graph::Spline)
			style = 5;
		else if (curveType == Graph::VerticalSteps)
			style = 6;
		boxConnect->setCurrentItem(style);

		setPenStyle(c->pen().style());
		boxLineColor->setColor(c->pen().color());
		boxLineWidth->setValue(c->pen().width());		
		boxFill->setChecked(c->brush().style() != Qt::NoBrush );
		boxAreaColor->setColor(c->brush().color());
		boxPattern->setPattern(c->brush().style());

		//symbol page
		const QwtSymbol s = c->symbol();
		boxSymbolSize->setValue(s.size().width());
		boxSymbolStyle->setStyle(s.style());
		boxSymbolColor->setColor(s.pen().color());
		boxPenWidth->setValue(s.pen().width());
		boxFillSymbol->setChecked(s.brush() != Qt::NoBrush);
		boxFillColor->setEnabled(s.brush() != Qt::NoBrush);
		boxFillColor->setColor(s.brush().color());

		if (curveType == Graph::VerticalBars || curveType == Graph::HorizontalBars || 
				curveType == Graph::Histogram)
		{//spacing page
			QwtBarCurve *b = (QwtBarCurve*)graph->curve(index);
			if (b)
			{
				gapBox->setValue(b->gap());
				offsetBox->setValue(b->offset());
			}
		}

		if (curveType == Graph::Histogram)
		{//Histogram page
			QwtHistogram *h = (QwtHistogram*)graph->curve(index);
			if (h)
			{
				automaticBox->setChecked(h->autoBinning());
				binSizeBox->setText(QString::number(h->binSize()));
				histogramBeginBox->setText(QString::number(h->begin()));
				histogramEndBox->setText(QString::number(h->end()));
				setAutomaticBinning();
			}
		}

		if (curveType == Graph::VectXYXY || curveType == Graph::VectXYAM)
		{//Vector page		
			VectorCurve *v = (VectorCurve*)graph->curve(index);
			if (v)
			{
				vectColorBox->setColor(v->color());
				vectWidthBox->setValue(v->width());
				headLengthBox->setValue(v->headLength());
				headAngleBox->setValue(v->headAngle());
				filledHeadBox->setChecked(v->filledArrowHead());
				vectPosBox->setCurrentItem(v->position());
				updateEndPointColumns(listBox->currentText());
			}
		}

		if (curveType == Graph::ErrorBars)
		{
			QwtErrorPlotCurve *err = (QwtErrorPlotCurve*)graph->curve(index);
			if (err)
			{
				xBox->setChecked(err->xErrors());
				widthBox->setEditText(QString::number(err->width()));
				capBox->setEditText(QString::number(err->capLength()));
				colorBox->setColor(err->color());
				throughBox->setChecked(err->throughSymbol());
				plusBox->setChecked(err->plusSide());
				minusBox->setChecked(err->minusSide());
			}
		}

		if (curveType == Graph::Box)
		{
			BoxCurve *b = (BoxCurve*)graph->curve(index);
			if (b)
			{
				boxMaxStyle->setStyle(b->maxStyle());
				boxMinStyle->setStyle(b->minStyle());
				boxMeanStyle->setStyle(b->meanStyle());
				box99Style->setStyle(b->p99Style());
				box1Style->setStyle(b->p1Style());

				boxPercSize->setValue(s.size().width());
				boxFillSymbols->setChecked(s.brush() != Qt::NoBrush);
				boxPercFillColor->setEnabled(s.brush() != Qt::NoBrush);
				boxPercFillColor->setColor(s.brush().color());
				boxEdgeColor->setColor(s.pen().color());
				boxEdgeWidth->setValue(s.pen().width());

				boxRange->setCurrentItem(b->boxRangeType()-1);
				boxType->setCurrentItem(b->boxStyle());
				boxWidth->setValue(b->boxWidth());
				setBoxRangeType(boxRange->currentItem());
				setBoxType(boxType->currentItem());
				if (b->boxRangeType() == BoxCurve::SD || b->boxRangeType() == BoxCurve::SE)
					boxCnt->setValue(b->boxRange());
				else
					boxCoef->setValue((int)b->boxRange());

				boxWhiskersRange->setCurrentItem(b->whiskersRangeType());
				setWhiskersRange(boxWhiskersRange->currentItem());
				if (b->whiskersRangeType() == BoxCurve::SD || b->whiskersRangeType() == BoxCurve::SE)
					whiskerCnt->setValue(b->whiskersRange());
				else
					boxWhiskersCoef->setValue((int)b->whiskersRange());
			}
		}
	}
}

void PlotDialog::updateEndPointColumns(const QString& text)
{
	QStringList cols=QStringList::split(",", text, false);
	QStringList aux=QStringList::split(":", cols[0], false);
	QString table=aux[0];	
	QStringList list;
	for (int i=0; i<(int)columnNames.count(); i++)	
	{
		QString s=columnNames[i];
		if (s.contains(table))
			list<<s;	
	}

	xEndBox->clear();
	xEndBox->insertStringList(list);
	xEndBox->setCurrentText(table + "_" + cols[2].remove("(X)").remove("(A)"));

	yEndBox->clear();
	yEndBox->insertStringList(list);	
	yEndBox->setCurrentText(table + "_" + cols[3].remove("(Y)").remove("(M)"));
}

bool PlotDialog::acceptParams()
{
	if (privateTabWidget->currentPage() == axesPage)
	{
		QwtPlotCurve *c = graph->curve(listBox->currentItem());
		if (!c)
			return false;

		c->setAxis(boxXAxis->currentItem() + 2, boxYAxis->currentItem());
		graph->setAutoScale();
		return true;
	}
	else if (privateTabWidget->currentPage()==linePage)
	{
		int index=listBox->currentItem();
		graph->setCurveStyle(index, boxConnect->currentItem());
		QBrush br = QBrush(boxAreaColor->color(), boxPattern->getSelectedPattern());
		if (!boxFill->isChecked())
			br = QBrush();
		graph->setCurveBrush(index, br);
		QPen pen = QPen(boxLineColor->color(),boxLineWidth->value(),Graph::getPenStyle(boxLineStyle->currentItem()));
		graph->setCurvePen(index, pen);
	}
	else if (privateTabWidget->currentPage()==symbolPage)
	{
		int size = boxSymbolSize->value();
		QBrush br = QBrush(boxFillColor->color(), Qt::SolidPattern);
		if (!boxFillSymbol->isChecked())
			br = QBrush();
		QPen pen = QPen(boxSymbolColor->color(),boxPenWidth->value(),Qt::SolidLine);
		QwtSymbol s = QwtSymbol(boxSymbolStyle->selectedSymbol(), br, pen, QSize(size, size));
		graph->setCurveSymbol(listBox->currentItem(), s);
	}
	else if (privateTabWidget->currentPage()==histogramPage)
	{
		QString text=listBox->currentText();
		QStringList t=QStringList::split(": ", text, false);
		QStringList list=QStringList::split (",", t[1], false );
		text=t[0] + "_" + list[1].remove("(Y)");
		bool accept=validInput();
		if (accept)
		{
			ApplicationWindow *app = (ApplicationWindow *)this->parent();
			if (!app)
				return false;
			Table* w=app->table(text);
			if (w)
				graph->updateHistogram(w, text, listBox->currentItem(), automaticBox->isChecked(),binSizeBox->text().toDouble(), 
						histogramBeginBox->text().toDouble(), histogramEndBox->text().toDouble());		
		}
		return accept;
	}
	else if (privateTabWidget->currentPage()==spacingPage)
		graph->setBarsGap(listBox->currentItem(),gapBox->value(), offsetBox->value());
	else if (privateTabWidget->currentPage() == vectPage)
	{
		int index=listBox->currentItem();
		ApplicationWindow *app = (ApplicationWindow *)this->parent();
		if (!app)
			return false;

		QString xEndCol = xEndBox->currentText();
		QString yEndCol = yEndBox->currentText(); 
		Table* w=app->table(xEndCol);
		if (!w)
			return false;

		graph->updateVectorsLayout(w, index, vectColorBox->currentItem(), vectWidthBox->value(), 
				headLengthBox->value(), headAngleBox->value(), 
				filledHeadBox->isChecked(),vectPosBox->currentItem(),xEndCol,yEndCol);

		QString text=listBox->currentText();
		QStringList t=QStringList::split(": ", text, false);
		QString table = t[0];

		QStringList cols=QStringList::split(",", t[1], false);
		if (graph->curveType(index) == Graph::VectXYXY)
		{
			xEndCol = xEndCol.remove(table + "_") + "(X)";
			yEndCol = yEndCol.remove(table + "_") + "(Y)";
		}
		else
		{
			xEndCol = xEndCol.remove(table + "_") + "(A)";
			yEndCol = yEndCol.remove(table + "_") + "(M)";
		}

		if (cols[2] != xEndCol || cols[3] != yEndCol)
		{
			cols[2] = xEndCol;
			cols[3] = yEndCol;
			text=table + ": " + cols.join(",");
			listBox->changeItem (text, index);
		}		
		return true;
	}
	else if (privateTabWidget->currentPage() == errorsPage)
	{
		graph->updateErrorBars(listBox->currentItem(), xBox->isChecked(), widthBox->currentText().toInt(),
				capBox->currentText().toInt(), colorBox->color(), plusBox->isChecked(), minusBox->isChecked(),
				throughBox->isChecked());
	}
	else if (privateTabWidget->currentPage() == percentilePage)
	{	
		int size = boxPercSize->value();
		QBrush br = QBrush(boxPercFillColor->color(), Qt::SolidPattern);
		if (!boxFillSymbols->isChecked())
			br = QBrush();
		QwtSymbol s = QwtSymbol(QwtSymbol::NoSymbol, br, QPen(boxEdgeColor->color(),boxEdgeWidth->value(),Qt::SolidLine), QSize(size, size));
		graph->setCurveSymbol(listBox->currentItem(), s);

		BoxCurve *b = (BoxCurve*)graph->curve(listBox->currentItem());
		if (b)
		{
			b->setMaxStyle(boxMaxStyle->selectedSymbol());
			b->setP99Style(box99Style->selectedSymbol());
			b->setMeanStyle(boxMeanStyle->selectedSymbol());
			b->setP1Style(box1Style->selectedSymbol());
			b->setMinStyle(boxMinStyle->selectedSymbol());
		}
	}
	else if (privateTabWidget->currentPage() == boxPage)
	{	
		BoxCurve *b = (BoxCurve*)graph->curve(listBox->currentItem());
		if (b)
		{
			b->setBoxWidth(boxWidth->value());
			b->setBoxStyle(boxType->currentItem());
			if (boxCnt->isVisible())
				b->setBoxRange(boxRange->currentItem()+1, boxCnt->value());
			else
				b->setBoxRange(boxRange->currentItem()+1, (double)boxCoef->value());

			if (whiskerCnt->isVisible())
				b->setWhiskersRange(boxWhiskersRange->currentItem(), whiskerCnt->value());
			else
				b->setWhiskersRange(boxWhiskersRange->currentItem(), (double)boxWhiskersCoef->value());
		}
	}
	graph->replot();
	graph->emitModified();
	return true;
}

void PlotDialog::insertCurvesList()
{
	QStringList names = graph->plotAssociations();
	listBox->clear();
	QStringList newNames;
	int i;
	for (i=0;i<(int)names.count();i++)
	{
		QString s=names[i];
		int pos=s.find("_",0);
		if (pos>0)
		{
			QString table=s.left(pos);	
			QString cols=s.right(s.length()-pos-1);			
			newNames<<table+": "+cols.remove(table+"_",true);
		}
		else
			newNames<<s;
	}
	listBox->insertStringList(newNames,0);
}

void PlotDialog::setAutomaticBinning()
{
	GroupBoxH->setEnabled(!automaticBox->isChecked());
}

bool PlotDialog::validInput()
{
	QString from=histogramBeginBox->text();
	QString to=histogramEndBox->text();
	QString step=binSizeBox->text();
	QRegExp nonDigit("\\D");

	if (histogramBeginBox->text().isEmpty())
	{
		QMessageBox::critical(0,tr("QtiPlot - Input error"),
				tr("Please enter a valid start limit!"));
		histogramBeginBox->setFocus();
		return false;
	}

	if (histogramEndBox->text().isEmpty())
	{
		QMessageBox::critical(0,tr("QtiPlot - Input error"),
				tr("Please enter a valid end limit!"));
		histogramEndBox->setFocus();
		return false;
	}

	if (binSizeBox->text().isEmpty())
	{
		QMessageBox::critical(0,tr("QtiPlot - Input error"),
				tr("Please enter a valid bin size value!"));
		binSizeBox->setFocus();
		return false;
	}	

	from=from.remove(".");
	to=to.remove(".");
	step=step.remove(".");

	int pos=from.find("-",0);
	if(pos==0)
		from=from.replace(pos,1,"");

	pos=to.find("-",0);
	if(pos==0)
		to=to.replace(pos,1,"");

	double start,end, stp;
	bool error=false;	
	if (from.contains(nonDigit))
	{
		try
		{
			MyParser parser;
			parser.SetExpr((histogramBeginBox->text()).ascii());
			start=parser.Eval();
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(0,"QtiPlot - Start limit error",QString::fromStdString(e.GetMsg()));
			histogramBeginBox->setFocus();
			error=true;
			return false;
		}	
	}
	else
		start=histogramBeginBox->text().toDouble();

	if (to.contains(nonDigit))
	{
		try
		{
			MyParser parser;
			parser.SetExpr((histogramEndBox->text()).ascii());
			end=parser.Eval();
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(0,"QtiPlot - End limit error",QString::fromStdString(e.GetMsg()));
			histogramEndBox->setFocus();
			error=true;
			return false;
		}	
	}
	else
		end=histogramEndBox->text().toDouble();

	if (start>=end)
	{
		QMessageBox::critical(0,tr("QtiPlot - Input error"),
				tr("Please enter limits that satisfy: begin < end!"));
		histogramEndBox->setFocus();
		return false;
	}

	if (step.contains(nonDigit))
	{
		try
		{
			MyParser parser;
			parser.SetExpr((binSizeBox->text()).ascii());
			stp=parser.Eval();
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(0,tr("QtiPlot - Bin size input error"),QString::fromStdString(e.GetMsg()));
			binSizeBox->setFocus();
			error=true;
			return false;
		}	
	}
	else
		stp=binSizeBox->text().toDouble();

	if (stp <=0)
	{
		QMessageBox::critical(0,tr("QtiPlot - Bin size input error"),tr("Please enter a positive bin size value!"));
		binSizeBox->setFocus();
		return false;	
	}

	return true;
}

void PlotDialog::setPenStyle(Qt::PenStyle style)
{
	switch (style)
	{
		case Qt::SolidLine:
			boxLineStyle->setCurrentItem(0);
			break;
		case Qt::DashLine:
			boxLineStyle->setCurrentItem(1);
			break;
		case Qt::DotLine:
			boxLineStyle->setCurrentItem(2);
			break;
		case Qt::DashDotLine:
			boxLineStyle->setCurrentItem(3);
			break;
		case Qt::DashDotDotLine:
			boxLineStyle->setCurrentItem(4);
			break;
	}
}

void PlotDialog::setBoxType(int index)
{
	boxCoeffLabel->hide();
	boxRangeLabel->hide();
	boxRange->hide();
	boxCoef->hide();
	boxCntLabel->hide();
	boxCnt->hide();

	if (index != BoxCurve::NoBox && index != BoxCurve::WindBox)
	{
		boxRange->show();
		boxRangeLabel->show();
		int id = boxRange->currentItem() + 1;
		if (id == BoxCurve::UserDef)
		{
			boxCoef->show();
			boxCoeffLabel->show();
		}
		else if (id == BoxCurve::SD || id == BoxCurve::SE)
		{
			boxCntLabel->show();
			boxCnt->show();
		}
	}
}

void PlotDialog::setBoxRangeType(int index)
{
	boxCoeffLabel->hide();
	boxCoef->hide();
	boxCntLabel->hide();
	boxCnt->hide();

	index++;
	if (index == BoxCurve::UserDef)
	{
		boxCoeffLabel->show();
		boxCoef->show();
	}
	else if (index == BoxCurve::SD || index == BoxCurve::SE)
	{
		boxCntLabel->show();
		boxCnt->show();
	}
}

void PlotDialog::setWhiskersRange(int index)
{
	whiskerCoeffLabel->hide();
	boxWhiskersCoef->hide();
	whiskerCntLabel->hide();
	whiskerCnt->hide();

	if (index == BoxCurve::UserDef)
	{
		whiskerCoeffLabel->show();
		boxWhiskersCoef->show();
	}
	else if (index == BoxCurve::SD || index == BoxCurve::SE)
	{
		whiskerCntLabel->show();
		whiskerCnt->show();
	}
}

void PlotDialog::customVectorsPage(bool angleMag)
{
	if (angleMag)
	{
		GroupBoxVectEnd->setTitle(tr("Vector Data"));
		labelXEnd->setText(tr("Angle"));	
		labelYEnd->setText(tr("Magnitude"));
		labelPosition->show();
		vectPosBox->show();
	}
	else
	{
		GroupBoxVectEnd->setTitle(tr("End Point"));
		labelXEnd->setText(tr("X End"));	
		labelYEnd->setText(tr("Y End"));
		labelPosition->hide();
		vectPosBox->hide();
	}
}

PlotDialog::~PlotDialog()
{
}
