/***************************************************************************
    File                 : configDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Configuration dialog
                           
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
#include "configDialog.h"
#include "application.h"
#include "graph.h"
#include "colorButton.h"


#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QGroupBox>
#include <QFont>
#include <QFontDialog> 
#include <QColorDialog>
#include <QTabWidget>
#include <QStackedWidget>
#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QStyleFactory>
#include <QRegExp>
#include <QMessageBox>
#include <QTranslator>
#include <QApplication>
#include <QDir>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidget>
#include <QFontMetrics>

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

ConfigDialog::ConfigDialog( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
	// get current values from app window
	ApplicationWindow *app = (ApplicationWindow *)parentWidget();
	plot3DColors = app->plot3DColors;
	plot3DTitleFont = app->plot3DTitleFont;
	plot3DNumbersFont = app->plot3DNumbersFont;
	plot3DAxesFont = app->plot3DAxesFont;
	textFont = app->tableTextFont;
	headerFont = app->tableHeaderFont;	
	appFont = app->appFont;
	axesFont = app->plotAxesFont;
	numbersFont = app->plotNumbersFont;
	legendFont = app->plotLegendFont;	
	titleFont = app->plotTitleFont;

	// create the GUI
#ifdef TABBED_CONFIG_DIALOG
	generalDialog = new QTabWidget();
#else
	generalDialog = new QStackedWidget();
	itemsList = new QListWidget();
#endif
	initAppPage();
	initConfirmationsPage();
	initTablesPage();
	initPlotsPage();
	initCurvesPage();
	initPlots3DPage();
#ifdef TABBED_CONFIG_DIALOG
	generalDialog->addTab(application,QString()); // the titles will be set in languageChange()
	generalDialog->addTab(confirm,QString());
	generalDialog->addTab(tables,QString());
	generalDialog->addTab(plots,QString());
	generalDialog->addTab(curves,QString());
	generalDialog->addTab(plots3D,QString());
	lastTab = application;
#else
	generalDialog->addWidget(application);
	generalDialog->addWidget(confirm);
	generalDialog->addWidget(tables);
	generalDialog->addWidget(plots);
	generalDialog->addWidget(curves);
	generalDialog->addWidget(plots3D);

	QHBoxLayout * topLayout = new QHBoxLayout();
	topLayout->setSpacing(5);
	topLayout->setMargin(5);
	topLayout->addWidget( itemsList );
	topLayout->addWidget( generalDialog );
#endif

	QHBoxLayout * bottomButtons = new QHBoxLayout();

	buttonApply = new QPushButton();
    buttonApply->setAutoDefault( true );
	bottomButtons->addWidget( buttonApply );
	
	buttonOk = new QPushButton();
    buttonOk->setAutoDefault( true );
    buttonOk->setDefault( true );
	bottomButtons->addWidget( buttonOk );
   
    buttonCancel = new QPushButton();
    buttonCancel->setAutoDefault( true );
	bottomButtons->addWidget( buttonCancel );

	QVBoxLayout * mainLayout = new QVBoxLayout( this ); 
#ifdef TABBED_CONFIG_DIALOG
    mainLayout->addWidget(generalDialog);
#else
    mainLayout->addLayout(topLayout);
#endif
	mainLayout->addLayout(bottomButtons);

    languageChange();
   
    // signals and slots connections
#ifdef TABBED_CONFIG_DIALOG
	connect( generalDialog, SIGNAL(currentChanged(int)), this, SLOT(update()));
#else
	connect( itemsList, SIGNAL(currentRowChanged(int)), this, SLOT(update()));
	connect( itemsList, SIGNAL(currentRowChanged(int)), generalDialog, SLOT(setCurrentIndex(int)));
#endif
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT( update() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( buttonBackground, SIGNAL( clicked() ), this, SLOT( pickBgColor() ) );
	connect( buttonText, SIGNAL( clicked() ), this, SLOT( pickTextColor() ) );
	connect( buttonHeader, SIGNAL( clicked() ), this, SLOT( pickHeaderColor() ) );
	connect( buttonTextFont, SIGNAL( clicked() ), this, SLOT( pickTextFont() ) );
	connect( buttonHeaderFont, SIGNAL( clicked() ), this, SLOT( pickHeaderFont() ) );
}

void ConfigDialog::initTablesPage()
{
	ApplicationWindow *app = (ApplicationWindow *)parentWidget();
	tables = new QWidget();

	QHBoxLayout * topLayout = new QHBoxLayout(); 
	topLayout->setSpacing(5);

	lblSeparator = new QLabel();
	topLayout->addWidget( lblSeparator );
	boxSeparator = new QComboBox();
	topLayout->addWidget( boxSeparator );

	groupBoxTableCol = new QGroupBox();
	QGridLayout * colorsLayout = new QGridLayout(groupBoxTableCol);

	lblTableBackground = new QLabel(); 
	colorsLayout->addWidget( lblTableBackground, 0, 0 );
	buttonBackground= new ColorButton();
	buttonBackground->setColor(app->tableBkgdColor);
	colorsLayout->addWidget( buttonBackground, 0, 1 );

	lblTextColor = new QLabel(); 
	colorsLayout->addWidget( lblTextColor, 1, 0 );
	buttonText= new ColorButton();
	buttonText->setColor(app->tableTextColor);
	colorsLayout->addWidget( buttonText, 1, 1 );

	lblHeaderColor = new QLabel(); 
	colorsLayout->addWidget( lblHeaderColor, 2, 0 );
	buttonHeader= new ColorButton();
	buttonHeader->setColor(app->tableHeaderColor);
	colorsLayout->addWidget( buttonHeader, 2, 1 );

	groupBoxTableFonts = new QGroupBox();
	QHBoxLayout * bottomLayout = new QHBoxLayout( groupBoxTableFonts );

	buttonTextFont= new QPushButton();
	bottomLayout->addWidget( buttonTextFont );
	buttonHeaderFont= new QPushButton();
	bottomLayout->addWidget( buttonHeaderFont );

	QVBoxLayout * tablesPageLayout = new QVBoxLayout( tables );
	tablesPageLayout->addLayout(topLayout,1);
	tablesPageLayout->addWidget(groupBoxTableCol);
	tablesPageLayout->addWidget(groupBoxTableFonts);
}

void ConfigDialog::initPlotsPage()
{
	ApplicationWindow *app = (ApplicationWindow *)parentWidget();
	plots = new QWidget();

	QHBoxLayout * topLayout = new QHBoxLayout(); 
	topLayout->setSpacing(5);

	groupBoxOptions = new QGroupBox();	
	topLayout->addWidget( groupBoxOptions );

	QGridLayout * optionsLayout = new QGridLayout( groupBoxOptions );

	boxAutoscaling = new QCheckBox();
	boxAutoscaling->setChecked(app->autoscale2DPlots);
	optionsLayout->addWidget( boxAutoscaling, 0, 0 );

	boxScaleFonts = new QCheckBox();
	boxScaleFonts->setChecked(app->autoScaleFonts);
	optionsLayout->addWidget( boxScaleFonts, 0, 1 );

	boxTitle = new QCheckBox();
	boxTitle->setChecked(app->titleOn);
	optionsLayout->addWidget( boxTitle, 1, 0 );

	boxAllAxes = new QCheckBox();
	boxAllAxes->setChecked (app->allAxesOn);
	optionsLayout->addWidget( boxAllAxes, 1, 1 );

	boxFrame = new QCheckBox();
	boxFrame->setChecked(app->canvasFrameOn);
	optionsLayout->addWidget( boxFrame, 2, 0 );

	boxBackbones= new QCheckBox();
	boxBackbones->setChecked(app->drawBackbones);
	optionsLayout->addWidget( boxBackbones, 2, 1 );

	labelFrameWidth = new QLabel();
	optionsLayout->addWidget( labelFrameWidth, 3, 0 );
	boxFrameWidth= new QSpinBox();
	optionsLayout->addWidget( boxFrameWidth, 3, 1 );
	boxFrameWidth->setRange(1, 100);
	boxFrameWidth->setValue(app->canvasFrameWidth);
	if (!app->canvasFrameOn)
	{
		labelFrameWidth->hide();
		boxFrameWidth->hide();
	}

	lblLegend = new QLabel(); 
	optionsLayout->addWidget( lblLegend, 4, 0 );
	boxLegend = new QComboBox();
	optionsLayout->addWidget( boxLegend, 4, 1 );

	lblMargin = new QLabel(); 
	optionsLayout->addWidget( lblMargin, 5, 0 );
	boxMargin= new QSpinBox();
	boxMargin->setRange(0, 1000);
	boxMargin->setSingleStep(5);
	boxMargin->setValue(app->defaultPlotMargin);
	optionsLayout->addWidget( boxMargin, 5, 1 );

	optionsLayout->setRowStretch( 6, 1 );

	groupBox2DFonts = new QGroupBox();
	topLayout->addWidget( groupBox2DFonts );
	QVBoxLayout * fontsLayout = new QVBoxLayout( groupBox2DFonts );
	buttonTitleFont= new QPushButton();  
	fontsLayout->addWidget( buttonTitleFont );
	buttonLegendFont= new QPushButton();	 
	fontsLayout->addWidget( buttonLegendFont );
	buttonAxesFont= new QPushButton();   
	fontsLayout->addWidget( buttonAxesFont );
	buttonNumbersFont= new QPushButton();
	fontsLayout->addWidget( buttonNumbersFont );
	fontsLayout->addStretch();

	QGroupBox * ticksGroupBox = new QGroupBox();
	QGridLayout * ticksLayout = new QGridLayout( ticksGroupBox );

	lblTicks = new QLabel(); 
	ticksLayout->addWidget( lblTicks, 0, 0 );
	boxTicks = new QComboBox();
	ticksLayout->addWidget( boxTicks, 0, 1 );

	lblMajTicks = new QLabel();  
	ticksLayout->addWidget( lblMajTicks, 0, 2 );
	boxMajorTicks= new QSpinBox();
	boxMajorTicks->setRange(0, 100);
	boxMajorTicks->setValue(app->majTicksLength);
	ticksLayout->addWidget( boxMajorTicks, 0, 3 );

	lblAxesLineWidth = new QLabel();  
	ticksLayout->addWidget( lblAxesLineWidth, 1, 0 );
	boxLineWidth= new QSpinBox();
	boxLineWidth->setRange(0, 100);
	boxLineWidth->setValue(app->axesLineWidth);
	ticksLayout->addWidget( boxLineWidth, 1, 1 );

	lblMinTicks = new QLabel();  
	ticksLayout->addWidget( lblMinTicks, 1, 2 );
	boxMinorTicks= new QSpinBox();
	boxMinorTicks->setRange(0, 100);
	boxMinorTicks->setValue(app->minTicksLength);
	ticksLayout->addWidget( boxMinorTicks, 1, 4 );

	boxResize = new QCheckBox();
	boxResize->setChecked(!app->autoResizeLayers);
	if(boxResize->isChecked())
		boxScaleFonts->setEnabled(false);

	QVBoxLayout * plotsPageLayout = new QVBoxLayout( plots );
	plotsPageLayout->addLayout(topLayout);
	plotsPageLayout->addWidget(ticksGroupBox);
	plotsPageLayout->addWidget(boxResize);

	connect( boxResize, SIGNAL( clicked() ), this, SLOT( enableScaleFonts() ) );
	connect( boxFrame, SIGNAL( toggled(bool) ), this, SLOT( showFrameWidth(bool) ) );
	connect( buttonAxesFont, SIGNAL( clicked() ), this, SLOT( pickAxesFont() ) );
	connect( buttonNumbersFont, SIGNAL( clicked() ), this, SLOT( pickNumbersFont() ) );
	connect( buttonLegendFont, SIGNAL( clicked() ), this, SLOT( pickLegendFont() ) );
	connect( buttonTitleFont, SIGNAL( clicked() ), this, SLOT( pickTitleFont() ) );
}

void ConfigDialog::enableScaleFonts()
{
	if(boxResize->isChecked())
		boxScaleFonts->setEnabled(false);
	else
		boxScaleFonts->setEnabled(true);
}

void ConfigDialog::showFrameWidth(bool ok)
{
	if (!ok)
	{
		boxFrameWidth->hide();
		labelFrameWidth->hide();
	}
	else
	{
		boxFrameWidth->show();
		labelFrameWidth->show();
	}	
}

void ConfigDialog::initPlots3DPage()
{
	ApplicationWindow *app = (ApplicationWindow *)parentWidget();
	plots3D = new QWidget();

	QGroupBox * topBox = new QGroupBox();
	QGridLayout * topLayout = new QGridLayout( topBox );
	topLayout->setSpacing(5);

	lblResolution = new QLabel(); 
	topLayout->addWidget( lblResolution, 0, 0 );
	boxResolution = new QSpinBox();
	boxResolution->setRange(1, 100);
	boxResolution->setValue(app->plot3DResolution);
	topLayout->addWidget( boxResolution, 0, 1 );

	boxShowLegend = new QCheckBox();
	boxShowLegend->setChecked(app->showPlot3DLegend);
	topLayout->addWidget( boxShowLegend, 1, 0 );

	boxShowProjection = new QCheckBox();
	boxShowProjection->setChecked(app->showPlot3DProjection);
	topLayout->addWidget( boxShowProjection, 1, 1 );

	boxSmoothMesh = new QCheckBox();
	boxSmoothMesh->setChecked(app->smooth3DMesh);
	topLayout->addWidget( boxSmoothMesh, 2, 0 );

	groupBox3DCol = new QGroupBox();
	QGridLayout * middleLayout = new QGridLayout( groupBox3DCol );

	btnFromColor = new QPushButton();
	middleLayout->addWidget( btnFromColor, 0, 0 );
	btnLabels = new QPushButton();
	middleLayout->addWidget( btnLabels, 0, 1 );
	btnMesh = new QPushButton();	
	middleLayout->addWidget( btnMesh, 0, 2 );
	btnGrid = new QPushButton();
	middleLayout->addWidget( btnGrid, 0, 3 );
	btnToColor = new QPushButton();
	middleLayout->addWidget( btnToColor, 1, 0 );
	btnNumbers = new QPushButton();
	middleLayout->addWidget( btnNumbers, 1, 1 );
	btnAxes = new QPushButton();
	middleLayout->addWidget( btnAxes, 1, 2 );
	btnBackground3D = new QPushButton();
	middleLayout->addWidget( btnBackground3D, 1, 3 );

	groupBox3DFonts = new QGroupBox();
	QHBoxLayout * bottomLayout = new QHBoxLayout( groupBox3DFonts );

	btnTitleFnt = new QPushButton();
	bottomLayout->addWidget( btnTitleFnt );
	btnLabelsFnt = new QPushButton();
	bottomLayout->addWidget( btnLabelsFnt );
	btnNumFnt = new QPushButton();
	bottomLayout->addWidget( btnNumFnt );

	QVBoxLayout * plots3DPageLayout = new QVBoxLayout( plots3D );
	plots3DPageLayout->addWidget(topBox);
	plots3DPageLayout->addWidget(groupBox3DCol);
	plots3DPageLayout->addWidget(groupBox3DFonts);
	plots3DPageLayout->addStretch();

	connect( btnAxes, SIGNAL( clicked() ), this, SLOT(pick3DAxesColor() ) );
	connect( btnLabels, SIGNAL( clicked() ), this, SLOT(pick3DLabelsColor() ) );
	connect( btnNumbers, SIGNAL( clicked() ), this, SLOT(pick3DNumbersColor() ) );
	connect( btnBackground3D, SIGNAL( clicked() ), this, SLOT(pick3DBackgroundColor()));
	connect( btnFromColor, SIGNAL( clicked() ), this, SLOT(pickDataMinColor() ) );
	connect( btnToColor, SIGNAL( clicked() ), this, SLOT(pickDataMaxColor() ) );
	connect( btnGrid, SIGNAL( clicked() ), this, SLOT(pickGridColor() ) );
	connect( btnMesh, SIGNAL( clicked() ), this, SLOT(pickMeshColor() ) );

	connect( btnNumFnt, SIGNAL( clicked() ), this, SLOT(pick3DNumbersFont() ) );
	connect( btnTitleFnt, SIGNAL( clicked() ), this, SLOT(pick3DTitleFont() ) );
	connect( btnLabelsFnt, SIGNAL( clicked() ), this, SLOT(pick3DAxesFont() ) );
}

void ConfigDialog::initAppPage()
{
	ApplicationWindow *app = (ApplicationWindow *)parentWidget();

	application = new QWidget();
	groupBoxApp = new QGroupBox();
	QGridLayout * topBoxLayout = new QGridLayout( groupBoxApp );

	lblLanguage = new QLabel(); 
	topBoxLayout->addWidget( lblLanguage, 0, 0 );
	boxLanguage = new QComboBox();
	insertLanguagesList();
	topBoxLayout->addWidget( boxLanguage, 0, 1 );

	lblStyle = new QLabel(); 
	topBoxLayout->addWidget( lblStyle, 1, 0 );
	boxStyle = new QComboBox();
	topBoxLayout->addWidget( boxStyle, 1, 1 );
	QStringList styles = QStyleFactory::keys();
	styles.sort();
	boxStyle->addItems(styles);
	boxStyle->setCurrentIndex(boxStyle->findText(app->appStyle,Qt::MatchWildcard));

#ifdef Q_OS_MACX // Mac 
	if (QApplication::macVersion() == QSysInfo::MV_10_3)
	{
		lblStyle->hide();
		boxStyle->hide();
	}
#endif

	lblFonts = new QLabel(); 
	topBoxLayout->addWidget( lblFonts, 2, 0 );
	fontsBtn= new QPushButton();
	topBoxLayout->addWidget( fontsBtn, 2, 1 );

	boxSave= new QCheckBox();
	boxSave->setChecked(app->autoSave);
	topBoxLayout->addWidget( boxSave, 3, 0 );

	boxMinutes = new QSpinBox();
	boxMinutes->setRange(1,100);
	boxMinutes->setValue(app->autoSaveTime);
	boxMinutes->setEnabled(app->autoSave);
	topBoxLayout->addWidget( boxMinutes, 3, 1 );

	topBoxLayout->setRowStretch( 4, 1 );

	groupBoxAppCol = new QGroupBox();
	QGridLayout * bottomBoxLayout = new QGridLayout( groupBoxAppCol );

	lblWorkspace = new QLabel(); 
	bottomBoxLayout->addWidget( lblWorkspace, 0, 0 );
	btnWorkspace = new ColorButton();
	btnWorkspace->setColor(app->workspaceColor);
	bottomBoxLayout->addWidget( btnWorkspace, 0, 1 );

	lblPanels = new QLabel(); 
	bottomBoxLayout->addWidget( lblPanels, 1, 0 );
	btnPanels = new ColorButton();
	bottomBoxLayout->addWidget( btnPanels, 1, 1 );
	btnPanels->setColor(app->panelsColor);

	lblPanelsText = new QLabel(); 
	bottomBoxLayout->addWidget( lblPanelsText, 2, 0 );
	btnPanelsText = new ColorButton();
	bottomBoxLayout->addWidget( btnPanelsText, 2, 1 );
	btnPanelsText->setColor(app->panelsTextColor);

	topBoxLayout->setRowStretch( 3, 1 );

	QVBoxLayout* appPageLayout = new QVBoxLayout( application );
	appPageLayout->addWidget(groupBoxApp);
	appPageLayout->addWidget(groupBoxAppCol);

	connect( boxLanguage, SIGNAL( activated(int) ), this, SLOT( switchToLanguage(int) ) );
	connect( fontsBtn, SIGNAL( clicked() ), this, SLOT( pickApplicationFont() ) );
	connect( boxSave, SIGNAL( toggled(bool) ), boxMinutes, SLOT( setEnabled(bool) ) );
	connect( btnWorkspace, SIGNAL( clicked() ), this, SLOT( pickWorkspaceColor() ) );
	connect( btnPanels, SIGNAL( clicked() ), this, SLOT( pickPanelsColor() ) );
	connect( btnPanelsText, SIGNAL( clicked() ), this, SLOT( pickPanelsTextColor() ) );
}

void ConfigDialog::initCurvesPage()
{
	curves = new QWidget();

	QGroupBox * curvesGroupBox = new QGroupBox();
	QGridLayout * curvesBoxLayout = new QGridLayout( curvesGroupBox );

	lblCurveStyle = new QLabel(); 	
	curvesBoxLayout->addWidget( lblCurveStyle, 0, 0 );
	boxCurveStyle = new QComboBox();
	curvesBoxLayout->addWidget( boxCurveStyle, 0, 1 );

	lblLineWidth = new QLabel(); 
	curvesBoxLayout->addWidget( lblLineWidth, 1, 0 );
	boxCurveLineWidth = new QSpinBox();
	boxCurveLineWidth->setRange(1,100);
	curvesBoxLayout->addWidget( boxCurveLineWidth, 1, 1 );

	lblSymbSize = new QLabel(); 
	curvesBoxLayout->addWidget( lblSymbSize, 2, 0 );
	boxSymbolSize = new QSpinBox();
	boxSymbolSize->setRange(1,100);
	curvesBoxLayout->addWidget( boxSymbolSize, 2, 1 );

	curvesBoxLayout->setRowStretch( 3, 1 );

	QHBoxLayout * curvesPageLayout = new QHBoxLayout( curves );
	curvesPageLayout->addWidget( curvesGroupBox );
}

void ConfigDialog::initConfirmationsPage()
{
	ApplicationWindow *app = (ApplicationWindow *)parentWidget();
	confirm = new QWidget();

	groupBoxConfirm = new QGroupBox();
	QVBoxLayout * layout = new QVBoxLayout( groupBoxConfirm );

	boxFolders = new QCheckBox();
	boxFolders->setChecked(app->confirmCloseFolder);
	layout->addWidget( boxFolders );

	boxTables = new QCheckBox();
	boxTables->setChecked(app->confirmCloseTable);
	layout->addWidget( boxTables );

	boxMatrices = new QCheckBox();
	boxMatrices->setChecked(app->confirmCloseMatrix);
	layout->addWidget( boxMatrices );

	boxPlots2D = new QCheckBox();
	boxPlots2D->setChecked(app->confirmClosePlot2D);
	layout->addWidget( boxPlots2D );

	boxPlots3D = new QCheckBox();
	boxPlots3D->setChecked(app->confirmClosePlot3D);
	layout->addWidget( boxPlots3D );

	boxNotes = new QCheckBox();
	boxNotes->setChecked(app->confirmCloseNotes);
	layout->addWidget( boxNotes );

	layout->addStretch();

	QHBoxLayout * confirmPageLayout = new QHBoxLayout( confirm );
	confirmPageLayout->addWidget(groupBoxConfirm);
}

ConfigDialog::~ConfigDialog()
{
}

void ConfigDialog::languageChange()
{
	setWindowTitle( tr( "QtiPlot - Choose default settings" ) );
	ApplicationWindow *app = (ApplicationWindow *)parentWidget();

#ifdef TABBED_CONFIG_DIALOG
	generalDialog->setTabText( 0, tr( "Application" ) );
	generalDialog->setTabText( 1, tr( "Confirmations" ) );
	generalDialog->setTabText( 2, tr( "Tables" ) );
	generalDialog->setTabText( 3, tr( "2D Plots" ) );
	generalDialog->setTabText( 4, tr( "2D Curves" ) );
	generalDialog->setTabText( 5, tr( "3D Plots" ) );
#else
	itemsList->clear();
	itemsList->addItem( tr( "Application" ) );
	itemsList->addItem( tr( "Confirmations" ) );
	itemsList->addItem( tr( "Tables" ) );
	itemsList->addItem( tr( "2D Plots" ) );
	itemsList->addItem( tr( "2D Curves" ) );
	itemsList->addItem( tr( "3D Plots" ) );
#endif

	//plots 2D page
	groupBoxOptions->setTitle(tr("Options"));	
	groupBox2DFonts->setTitle(tr("Fonts"));
	boxResize->setText(tr("Do not &resize layers when window size changes"));
	lblMinTicks->setText(tr("Minor ticks length"));  
	lblAxesLineWidth->setText(tr("Axes linewidth" ));  
	lblMajTicks->setText(tr("Major ticks length" ));  
	lblTicks->setText(tr("Ticks" )); 
	lblMargin->setText(tr("Margin" )); 
	lblLegend->setText(tr("Legend frame" )); 
	labelFrameWidth->setText(tr("Frame width" )); 
	boxBackbones->setText(tr("Axes &backbones"));
	boxFrame->setText(tr("Canvas Fra&me"));
	boxAllAxes->setText(tr("Sho&w all axes"));
	boxTitle->setText(tr("Show &Title"));
	boxScaleFonts->setText(tr("Scale &Fonts"));
	boxAutoscaling->setText(tr("Auto&scaling"));
	boxLegend->clear();
	boxLegend->addItem( tr( "None" ) );
	boxLegend->addItem( tr( "Rectangle" ) );
	boxLegend->addItem( tr( "Shadow" ) );
	boxLegend->addItem( tr( "White out" ) );
	boxLegend->addItem( tr( "Black out" ) );
	boxLegend->setCurrentItem(app->legendFrameStyle);

	boxTicks->clear();
	boxTicks->addItem(tr("In"));
	boxTicks->addItem(tr("Out"));
	boxTicks->addItem(tr("Both"));
	boxTicks->addItem(tr("None"));
	switch (app->ticksStyle)
	{
		case -1:
			boxTicks->setCurrentItem(0);
			break;

		case 1:
			boxTicks->setCurrentItem(1);
			break;

		case 2:
			boxTicks->setCurrentItem(2);
			break;

		case 0:
			boxTicks->setCurrentItem(3);
			break;	
	}

	//confirmations page
	groupBoxConfirm->setTitle(tr("Prompt on closing"));
	boxFolders->setText(tr("Folders"));
	boxTables->setText(tr("Tables"));
	boxPlots3D->setText(tr("3D Plots"));
	boxPlots2D->setText(tr("2D Plots"));
	boxMatrices->setText(tr("Matrices"));
	boxNotes->setText(tr("&Notes"));

	buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Close" ) );
	buttonApply->setText( tr( "&Apply" ) );
	buttonTextFont->setText( tr( "&Text Font" ) );
	buttonHeaderFont->setText( tr( "&Labels Font" ) );
	buttonAxesFont->setText( tr( "A&xes Labels" ) );
	buttonNumbersFont->setText( tr( "Axes &Numbers" ) );
	buttonLegendFont->setText( tr( "&Legend" ) );
	buttonTitleFont->setText( tr( "T&itle" ) );

	//application page
	groupBoxApp->setTitle(tr("General"));
	groupBoxAppCol->setTitle(tr("Colors"));
	lblLanguage->setText(tr("Language"));
	lblStyle->setText(tr("Style")); 
	lblFonts->setText(tr("Main Font")); 
	fontsBtn->setText(tr("Choose &font"));
	lblWorkspace->setText(tr("Workspace")); 
	lblPanelsText->setText(tr("Panels text")); 
	lblPanels->setText(tr("Panels"));
	boxSave->setText(tr("Save every"));
	boxMinutes->setSuffix(tr(" minutes"));

	//tables page
	groupBoxTableCol->setTitle(tr("Colors"));
	lblSeparator->setText(tr("Default Column Separator"));
	boxSeparator->clear();
	boxSeparator->addItem(tr("TAB"));
	boxSeparator->addItem(tr("SPACE"));
	boxSeparator->addItem(";" + tr("TAB"));
	boxSeparator->addItem("," + tr("TAB"));
	boxSeparator->addItem(";" + tr("SPACE"));
	boxSeparator->addItem("," + tr("SPACE"));
	boxSeparator->addItem(";");
	boxSeparator->addItem(",");
	setColumnSeparator(app->separator);

	lblTableBackground->setText(tr( "Background" )); 
	lblTextColor->setText(tr( "Text" )); 
	lblHeaderColor->setText(tr("Labels")); 
	groupBoxTableFonts->setTitle(tr("Fonts"));

	//curves page
	lblCurveStyle->setText(tr( "Default curve style" )); 	
	lblLineWidth->setText(tr( "Line width" )); 
	lblSymbSize->setText(tr( "Symbol size" )); 

	boxCurveStyle->clear();
	boxCurveStyle->addItem( QPixmap(lPlot_xpm), tr( " Line" ) );
	boxCurveStyle->addItem( QPixmap(pPlot_xpm), tr( " Scatter" ) );
	boxCurveStyle->addItem( QPixmap(lpPlot_xpm), tr( " Line + Symbol" ) );
	boxCurveStyle->addItem( QPixmap(dropLines_xpm), tr( " Vertical drop lines" ) );
	boxCurveStyle->addItem( QPixmap(spline_xpm), tr( " Spline" ) );
	boxCurveStyle->addItem( QPixmap(steps_xpm), tr( " Vertical steps" ) );
	boxCurveStyle->addItem( QPixmap(area_xpm), tr( " Area" ) );
	boxCurveStyle->addItem( QPixmap(vertBars_xpm), tr( " Vertical Bars" ) );
	boxCurveStyle->addItem( QPixmap(hBars_xpm), tr( " Horizontal Bars" ) );

	//plots 3D
	lblResolution->setText(tr("Resolution")); 
	boxResolution->setSpecialValueText( "1 " + tr("(all data shown)") );
	boxShowLegend->setText(tr( "&Show Legend" ));
	boxShowProjection->setText(tr( "Show &Projection" ));
	btnFromColor->setText( tr( "&Data Max" ) );
	boxSmoothMesh->setText(tr( "Smoot&h Line" ));
	btnLabels->setText( tr( "Lab&els" ) );
	btnMesh->setText( tr( "Mesh &Line" ) );
	btnGrid->setText( tr( "&Grid" ) );
	btnToColor->setText( tr( "Data &Min" ) );
	btnNumbers->setText( tr( "&Numbers" ) );
	btnAxes->setText( tr( "A&xes" ) );
	btnBackground3D->setText( tr( "&Background" ) );
	groupBox3DCol->setTitle(tr("Colors" ));
	groupBox3DFonts->setTitle(tr("Fonts" ));
	btnTitleFnt->setText( tr( "&Title" ) );
	btnLabelsFnt->setText( tr( "&Axes Labels" ) );
	btnNumFnt->setText( tr( "&Numbers" ) );
}

void ConfigDialog::accept()
{
	update();
	close();
}

void ConfigDialog::update()
{
	ApplicationWindow *app = (ApplicationWindow *)parentWidget();
	if (!app)
		return;
#ifdef TABBED_CONFIG_DIALOG
	QWidget * curWidget = lastTab;
	lastTab = generalDialog->currentWidget();
#else
	QWidget * curWidget = generalDialog->currentWidget();
#endif

	if (curWidget==tables)
	{
		QString sep = boxSeparator->currentText();
		sep.replace(tr("TAB"), "\t", false);
		sep.replace("\\t", "\t");
		sep.replace(tr("SPACE"), " ");
		sep.replace("\\s", " ");

		if (sep.contains(QRegExp("[0-9.eE+-]"))!=0)
		{
			QMessageBox::warning(0, tr("QtiPlot - Import options error"),
					tr("The separator must not contain the following characters: 0-9eE.+-"));
			return;
		}

		app->separator = sep;
		app->customizeTables(buttonBackground->color(), buttonText->color(), 
				buttonHeader->color(), textFont, headerFont);
	}
	else if (curWidget==plots)
	{
		app->ticksStyle=ticksType();
		app->legendFrameStyle=boxLegend->currentItem();
		app->titleOn=boxTitle->isChecked();
		app->allAxesOn = boxAllAxes->isChecked();
		app->canvasFrameOn=boxFrame->isChecked();
		app->canvasFrameWidth = boxFrameWidth->value();
		app->drawBackbones = boxBackbones->isChecked();
		app->axesLineWidth = boxLineWidth->value();
		app->majTicksLength = boxMajorTicks->value();
		app->minTicksLength = boxMinorTicks->value();
		app->defaultPlotMargin = boxMargin->value();
		app->plotAxesFont=axesFont;
		app->plotNumbersFont=numbersFont;
		app->plotLegendFont=legendFont;
		app->plotTitleFont=titleFont;
		app->setGraphDefaultSettings(boxAutoscaling->isChecked(),boxScaleFonts->isChecked(),boxResize->isChecked());
	}
	else if (curWidget==application)
	{
		app->changeAppFont(appFont);
		setFont(appFont);
		app->changeAppStyle(boxStyle->currentText());
		app->setSaveSettings(boxSave->isChecked(), boxMinutes->value());
		app->setAppColors(btnWorkspace->color(), btnPanels->color(), btnPanelsText->color());
	}
	else if (curWidget==confirm)
	{
		app->confirmCloseFolder = boxFolders->isChecked();
		app->updateConfirmOptions(boxTables->isChecked(), boxMatrices->isChecked(),
				boxPlots2D->isChecked(), boxPlots3D->isChecked(),
				boxNotes->isChecked());
	}
	else if (curWidget==plots3D)
	{
		app->plot3DColors = plot3DColors;
		app->showPlot3DLegend = boxShowLegend->isChecked();
		app->showPlot3DProjection = boxShowProjection->isChecked();
		app->plot3DResolution = boxResolution->value();
		app->plot3DTitleFont = plot3DTitleFont;
		app->plot3DNumbersFont = plot3DNumbersFont;
		app->plot3DAxesFont = plot3DAxesFont;

		if (app->smooth3DMesh != boxSmoothMesh->isChecked())
		{
			app->smooth3DMesh = boxSmoothMesh->isChecked();
			app->setPlot3DOptions();
		}
	}
	else if (curWidget==curves)
	{
		app->defaultCurveStyle = curveStyle();
		app->defaultCurveLineWidth = boxCurveLineWidth->value();
		app->defaultSymbolSize = boxSymbolSize->value();
	}
}

int ConfigDialog::curveStyle()
{
	int style = 0;
	switch (boxCurveStyle->currentItem())
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

void ConfigDialog::pickBgColor()
{
	QColor c = QColorDialog::getColor(buttonBackground->color(), this);
	if ( !c.isValid() || c == buttonBackground->color())
		return;

	buttonBackground->setColor(c);
}

void ConfigDialog::pickTextColor()
{
	QColor c = QColorDialog::getColor(buttonText->color(), this);
	if ( !c.isValid() || c == buttonText->color())
		return;

	buttonText->setColor(c);
}

void ConfigDialog::pickHeaderColor()
{
	QColor c = QColorDialog::getColor(buttonHeader->color(), this);
	if ( !c.isValid() || c == buttonHeader->color())
		return;

	buttonHeader->setColor(c);
}

void ConfigDialog::pickTextFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,textFont,this);
	if ( ok ) {
		textFont = font;
	} else {
		return;
	}
}

void ConfigDialog::pickHeaderFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,headerFont,this);
	if ( ok ) {
		headerFont = font;
	} else {
		return;
	}
}

int ConfigDialog::ticksType()
{
	int ticks=-1;
	switch (boxTicks->currentItem())
	{
		case 0:
			break;

		case 1:
			ticks=1;
			break;

		case 2:
			ticks=2;
			break;

		case 3:
			ticks=0;
			break;	
	}
	return ticks;
}

void ConfigDialog::pickLegendFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,legendFont,this);
	if ( ok ) {
		legendFont = font;
	} else {
		return;
	}
}

void ConfigDialog::pickAxesFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,axesFont,this);
	if ( ok ) {
		axesFont = font;
	} else {
		return;
	}
}

void ConfigDialog::pickNumbersFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,numbersFont,this);
	if ( ok ) {
		numbersFont = font;
	} else {
		return;
	}
}

void ConfigDialog::pickTitleFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,titleFont,this);
	if ( ok ) 
		titleFont = font;
	else 
		return;
}

void ConfigDialog::pickApplicationFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,appFont,this);
	if ( ok ) 
		appFont = font;
	else 
		return;
}

void ConfigDialog::initCurvesOptions(int style, int width, int symbolSize)
{
	if (style == Graph::Line)
		boxCurveStyle->setCurrentIndex(0);
	else if (style == Graph::Scatter)
		boxCurveStyle->setCurrentIndex(1);
	else if (style == Graph::LineSymbols)
		boxCurveStyle->setCurrentIndex(2);
	else if (style == Graph::VerticalDropLines)
		boxCurveStyle->setCurrentIndex(3);
	else if (style == Graph::Spline)
		boxCurveStyle->setCurrentIndex(4);
	else if (style == Graph::Steps)
		boxCurveStyle->setCurrentIndex(5);
	else if (style == Graph::Area)
		boxCurveStyle->setCurrentIndex(6);
	else if (style == Graph::VerticalBars)
		boxCurveStyle->setCurrentIndex(7);
	else if (style == Graph::HorizontalBars)
		boxCurveStyle->setCurrentIndex(8);

	boxCurveLineWidth->setValue(width);
	boxSymbolSize->setValue(symbolSize);
}

void ConfigDialog::pickPanelsTextColor()
{
	QColor c = QColorDialog::getColor(btnPanelsText->color(), this);
	if ( !c.isValid() || c == btnPanelsText->color())
		return;

	btnPanelsText->setColor(c);
}

void ConfigDialog::pickPanelsColor()
{
	QColor c = QColorDialog::getColor(btnPanels->color(), this);
	if ( !c.isValid() || c == btnPanels->color())
		return;

	btnPanels->setColor(c);
}

void ConfigDialog::pickWorkspaceColor()
{
	QColor c = QColorDialog::getColor(btnWorkspace->color(), this);
	if ( !c.isValid() || c == btnWorkspace->color())
		return;

	btnWorkspace->setColor(c);
}

void ConfigDialog::pickDataMaxColor()
{
	QColor c = QColorDialog::getColor(QColor(plot3DColors[0]), this );
	if ( !c.isValid() )
		return;

	plot3DColors[0] = c.name();
}

void ConfigDialog::pickDataMinColor()
{
	QColor c = QColorDialog::getColor(QColor(plot3DColors[4]), this );
	if ( !c.isValid() )
		return;

	plot3DColors[4] = c.name();
}

void ConfigDialog::pick3DBackgroundColor()
{
	QColor c = QColorDialog::getColor(QColor(plot3DColors[7]), this );
	if ( !c.isValid() )
		return;

	plot3DColors[7] = c.name();
}

void ConfigDialog::pickMeshColor()
{
	QColor c = QColorDialog::getColor(QColor(plot3DColors[2]), this );
	if ( !c.isValid() )
		return;

	plot3DColors[2] = c.name();
}

void ConfigDialog::pickGridColor()
{
	QColor c = QColorDialog::getColor(QColor(plot3DColors[3]), this );
	if ( !c.isValid() )
		return;

	plot3DColors[3] = c.name();
}

void ConfigDialog::pick3DAxesColor()
{
	QColor c = QColorDialog::getColor(QColor(plot3DColors[6]), this );
	if ( !c.isValid() )
		return;

	plot3DColors[6] = c.name();
}

void ConfigDialog::pick3DNumbersColor()
{
	QColor c = QColorDialog::getColor(QColor(plot3DColors[5]), this );
	if ( !c.isValid() )
		return;

	plot3DColors[5] = c.name();
}

void ConfigDialog::pick3DLabelsColor()
{
	QColor c = QColorDialog::getColor(QColor(plot3DColors[1]), this );
	if ( !c.isValid() )
		return;

	plot3DColors[1] = c.name();
}

void ConfigDialog::pick3DTitleFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, plot3DTitleFont,this);
	if ( ok ) 
		plot3DTitleFont = font;
	else 
		return;
}

void ConfigDialog::pick3DNumbersFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, plot3DNumbersFont,this);
	if ( ok ) 
		plot3DNumbersFont = font;
	else 
		return;
}

void ConfigDialog::pick3DAxesFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok, plot3DAxesFont,this);
	if ( ok ) 
		plot3DAxesFont = font;
	else 
		return;
}

void ConfigDialog::setColumnSeparator(const QString& sep)
{
	if (sep=="\t")
		boxSeparator->setCurrentIndex(0);
	else if (sep==" ")
		boxSeparator->setCurrentIndex(1);
	else if (sep==";\t")
		boxSeparator->setCurrentIndex(2);
	else if (sep==",\t")
		boxSeparator->setCurrentIndex(3);
	else if (sep=="; ")
		boxSeparator->setCurrentIndex(4);
	else if (sep==", ")
		boxSeparator->setCurrentIndex(5);
	else if (sep==";")
		boxSeparator->setCurrentIndex(6);
	else if (sep==",")
		boxSeparator->setCurrentIndex(7);
	else
	{
		QString separator = sep;
		boxSeparator->setCurrentText(separator.replace(" ","\\s").replace("\t","\\t"));
	}
}

void ConfigDialog::switchToLanguage(int param)
{
	ApplicationWindow *app = (ApplicationWindow *)parentWidget();
	app->switchToLanguage(param);
	languageChange();
}

void ConfigDialog::insertLanguagesList()
{
	ApplicationWindow *app = (ApplicationWindow *)parentWidget();
	QString qmPath = qApp->applicationDirPath() + "/translations";
	QDir dir(qmPath);
	QStringList locales = app->locales;
	QStringList languages;
	int lang = 0;
	for (int i=0; i < (int)locales.size(); i++)
	{
		if (locales[i] == "en")
			languages.push_back("English");
		else
		{
			QTranslator translator;
			translator.load("qtiplot_"+locales[i], qmPath);

			QString language = translator.translate("ApplicationWindow", "English");
			if (!language.isEmpty())
				languages.push_back(language);
			else
				languages.push_back(locales[i]);
		}

		if (locales[i] == app->appLanguage)
			lang = i;
	}
	boxLanguage->addItems(languages);
	boxLanguage->setCurrentIndex(lang);
}
