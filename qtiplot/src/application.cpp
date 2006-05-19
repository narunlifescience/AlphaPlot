#include "application.h"
#include "pixmaps.h"
#include "curvesDialog.h"
#include "plotDialog.h"
#include "axesDialog.h"
#include "lineDlg.h"
#include "textDialog.h"
#include "exportDialog.h"
#include "tableDialog.h"
#include "valuesDialog.h"
#include "errDlg.h"
#include "LegendMarker.h"
#include "LineMarker.h"
#include "ImageMarker.h"
#include "importDialog.h"
#include "graph.h"
#include "plot.h"
#include "pieDialog.h"
#include "plotWizard.h"
#include "polynomFitDialog.h"
#include "expDecayDialog.h"
#include "functionDialog.h"
#include "fitDialog.h"
#include "surfaceDialog.h"
#include "graph3D.h"
#include "plot3DDialog.h"
#include "imageDialog.h"
#include "multilayer.h"
#include "layerDialog.h"
#include "analysisDialog.h"
#include "intDialog.h"
#include "configDialog.h"
#include "imageExportDialog.h"
#include "matrixDialog.h"
#include "matrixSizeDialog.h"
#include "matrixValuesDialog.h"
#include "importOPJ.h"
#include "associationsDialog.h"
#include "renameWindowDialog.h"
#include "ErrorBar.h"
#include "interpolationDialog.h"
#include "fileDialogs.h"
#include "smoothCurveDialog.h"
#include "filterDialog.h"
#include "fftDialog.h"
#include "epsExportDialog.h"
#include "note.h"
#include "folder.h"
#include "findDialog.h"

#include <stdio.h>
#include <stdlib.h>

#include <qworkspace.h>
#include <qimage.h>
#include <qpixmap.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qkeycode.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qmessagebox.h>
#include <qprinter.h>
#include <qaccel.h>
#include <qtextstream.h>
#include <qinputdialog.h>
#include <qregexp.h>
#include <qtextview.h>
#include <qlistview.h>
#include <qcursor.h>
#include <qtextbrowser.h>
#include <qevent.h>
#include <qaction.h>
#include <qprogressdialog.h>
#include <qpixmapcache.h>
#include <qsettings.h>
#include <qstylefactory.h>
#include <qdragobject.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <qprocess.h>
#include <qtranslator.h>
#include <qsplitter.h>
#include <qobjectlist.h>

#include <zlib.h>

using namespace Qwt3D;

extern "C" 
{
void file_compress(char  *file, char  *mode);
void file_uncompress(char  *file);
}

ApplicationWindow::ApplicationWindow()
    : QMainWindow( 0, "main window", WDestructiveClose )
{
init();
}

ApplicationWindow::ApplicationWindow(const QStringList& l)
    : QMainWindow( 0, "main window", WDestructiveClose )
{
int args = (int)l.size();
if (args > 2)
	{
	QMessageBox::critical(0, tr("QtiPlot - Error"),
		tr("Too many command line options (maximum accepted is 2)!"));
	exit(1);
	}
else if (args == 2)
	{
	if (QFile::exists(l[1]))
		{
		ApplicationWindow *app = open(l[1]);
		if (app)
			app->parseCommandLineArgument(l[0], 2);
		}
	else
		{
		QMessageBox::critical(0, tr("QtiPlot - Error"),
		tr("<b> %1 </b>: Unknown command line option or the file doesn't exist!").arg(l[1]));
		exit(1);
		}
	}
else if (args == 1)
	{
	if (QFile::exists(l[0]))
		open(l[0]);
	else
		parseCommandLineArgument(l[0], 1);
	}
}

void ApplicationWindow::init()
{
initGlobalConstants();			
QPixmapCache::setCacheLimit(20*QPixmapCache::cacheLimit ());

tablesDepend = new QPopupMenu(this, 0);

createActions();
initToolBars();
initPlot3DToolBar();
initMainMenu();

explorerWindow = new QDockWindow (this, "explorerWindow", 0 );
explorerWindow->setResizeEnabled (true);
explorerWindow->setCloseMode(QDockWindow::Always);
explorerWindow->setFixedExtentHeight(150);
addDockWindow (explorerWindow, Qt::DockBottom);

QSplitter *splitter = new QSplitter( Qt::Horizontal, explorerWindow );

folders = new FolderListView( splitter );
folders->header()->setClickEnabled( FALSE );
folders->addColumn( tr("Folder") );
folders->setRootIsDecorated( TRUE );
folders->setResizeMode(QListView::LastColumn);
folders->header()->hide();
folders->setSelectionMode(QListView::Single);

connect(folders, SIGNAL(currentChanged(QListViewItem *)), 
		this, SLOT(folderItemChanged(QListViewItem *)));
connect(folders, SIGNAL(itemRenamed(QListViewItem *, int, const QString &)), 
		this, SLOT(renameFolder(QListViewItem *, int, const QString &)));
connect(folders, SIGNAL(contextMenuRequested(QListViewItem *, const QPoint &, int)), 
		this, SLOT(showFolderPopupMenu(QListViewItem *, const QPoint &, int)));
connect(folders, SIGNAL(dragItems(QPtrList<QListViewItem>)), 
		this, SLOT(dragFolderItems(QPtrList<QListViewItem>)));
connect(folders, SIGNAL(dropItems(QListViewItem *)), 
		this, SLOT(dropFolderItems(QListViewItem *)));
connect(folders, SIGNAL(renameItem(QListViewItem *)), 
		this, SLOT(startRenameFolder(QListViewItem *)));
connect(folders, SIGNAL(addFolderItem()), this, SLOT(addFolder()));
connect(folders, SIGNAL(deleteSelection()), this, SLOT(deleteSelectedItems()));

current_folder = new Folder( 0, tr("UNTITLED"));
FolderListItem *fli = new FolderListItem(folders, current_folder);
current_folder->setFolderListItem(fli);
fli->setOpen( TRUE );

lv = new FolderListView( splitter );
lv->addColumn (tr("Name"),-1 );
lv->addColumn (tr("Type"),-1 );
lv->addColumn (tr("View"),-1 );
lv->addColumn (tr("Size"),-1 );
lv->addColumn (tr("Created"),-1);
lv->addColumn (tr("Label"),-1);
lv->setResizeMode(QListView::LastColumn);
lv->setMinimumHeight(80);
lv->setSelectionMode(QListView::Extended);

explorerWindow->setWidget(splitter);
explorerWindow->hide();

logWindow = new QDockWindow (this, 0, 0 );
logWindow->setResizeEnabled (true);
logWindow->setCloseMode(QDockWindow::Always);
addDockWindow (logWindow, Qt::DockTop, true);

results=new QTextEdit(logWindow,"results");
results->setReadOnly (TRUE);

logWindow->setWidget(results);
logWindow->hide();

ws = new QWorkspace( this );
ws->setScrollBarsEnabled (TRUE);
setCentralWidget( ws );
setAcceptDrops(true);

hiddenWindows = new QWidgetList();
outWindows = new QWidgetList();

readSettings();
createLanguagesList();
insertTranslatedStrings();

QAccel *accel = new QAccel(this);
accel->connectItem( accel->insertItem( Key_F5 ), ws, SLOT(activateNextWindow()) );
accel->connectItem( accel->insertItem( Key_F6 ), ws, SLOT(activatePrevWindow()) );
accel->connectItem( accel->insertItem( Key_Delete ), this, SLOT(clearSelection()) );


connect(actionShowLog, SIGNAL(toggled(bool)), this, SLOT(showResults(bool)));
connect(logWindow,SIGNAL(visibilityChanged(bool)),actionShowLog,SLOT(setOn(bool)));
connect(explorerWindow,SIGNAL(visibilityChanged(bool)),actionShowExplorer,SLOT(setOn(bool)));
connect(tablesDepend, SIGNAL(activated(int)), this, SLOT(showTable(int)));

connect(this, SIGNAL(modified()),this, SLOT(modifiedProject()));
connect(this, SIGNAL(windowClosed(const QString&)), 
		this, SLOT(updateListView(const QString&)));
connect(ws, SIGNAL(windowActivated (QWidget*)),this, SLOT(windowActivated(QWidget*)));
connect(lv, SIGNAL(doubleClicked(QListViewItem *)),
		this, SLOT(maximizeWindow(QListViewItem *)));
connect(lv, SIGNAL(doubleClicked(QListViewItem *)), 
		this, SLOT(folderItemDoubleClicked(QListViewItem *)));
connect(lv, SIGNAL(contextMenuRequested(QListViewItem *, const QPoint &, int)), 
		this, SLOT(showWindowPopupMenu(QListViewItem *, const QPoint &, int)));
connect(lv, SIGNAL(dragItems(QPtrList<QListViewItem>)), 
		this, SLOT(dragFolderItems(QPtrList<QListViewItem>)));
connect(lv, SIGNAL(dropItems(QListViewItem *)), 
		this, SLOT(dropFolderItems(QListViewItem *)));
connect(lv, SIGNAL(renameItem(QListViewItem *)), 
		this, SLOT(startRenameFolder(QListViewItem *)));
connect(lv, SIGNAL(addFolderItem()), this, SLOT(addFolder()));
connect(lv, SIGNAL(deleteSelection()), this, SLOT(deleteSelectedItems()));
connect(lv, SIGNAL(itemRenamed(QListViewItem *, int, const QString &)), 
		this, SLOT(renameWindow(QListViewItem *, int, const QString &)));

connect(recent, SIGNAL(activated(int)),this, SLOT(openRecentProject(int)));
}

void ApplicationWindow::initGlobalConstants()
{
#ifdef Q_OS_MACX // Mac 
	 appStyle = "Macintosh (Aqua)";
#else
	 appStyle = "Windows";
#endif

askForSupport = true;
majVersion = 0; minVersion = 8; patchVersion = 4;
graphs=0; tables=0; matrixes = 0; notes = 0; fitNumber=0;
projectname="untitled";
ignoredLines=0;
lastModified=0;
activeGraph=0;
lastCopiedLayer=0;
copiedLayer=FALSE;
copiedMarkerType=Graph::None;
aw=0;
logInfo=QString::null;
savingTimerId=0;
renameColumns = true;
strip_spaces = false;
simplify_spaces = false;
show_windows_policy = ActiveFolder;

appFont = QFont();
QString family = appFont.family();
int pointSize = appFont.pointSize();
tableTextFont=appFont;
tableHeaderFont=appFont;
plotAxesFont=QFont(family, pointSize, QFont::Bold, FALSE);
plotNumbersFont=QFont(family, pointSize );
plotLegendFont=appFont;
plotTitleFont=QFont(family, pointSize + 2, QFont::Bold,FALSE);

plot3DAxesFont=QFont(family, pointSize, QFont::Bold, FALSE );
plot3DNumbersFont=QFont(family, pointSize);
plot3DTitleFont=QFont(family, pointSize + 2, QFont::Bold,FALSE);
}

void ApplicationWindow::applyUserSettings()
{
//set user defined colors
lv->setPaletteForegroundColor (panelsTextColor);
lv->setPaletteBackgroundColor (panelsColor);

results->setPaper (QBrush(panelsColor, SolidPattern));
results->setPaletteForegroundColor (panelsTextColor);

ws->setPaletteBackgroundColor (workspaceColor);
	
QPalette pal = qApp->palette();
pal.setColor (QPalette::Active, QColorGroup::Base, QColor(panelsColor));
qApp->setPalette(pal, true, 0);
	
updateAppFonts();

QColorGroup cg;
cg.setColor(QColorGroup::Text, QColor(green) );
cg.setColor(QColorGroup::HighlightedText, QColor(darkGreen) );
cg.setColor(QColorGroup::Background, QColor(black) );
info->setPalette(QPalette(cg, cg, cg));
}

void ApplicationWindow::initToolBars()
{
	setIcon(QPixmap(logo_xpm));
    QPixmap openIcon, saveIcon;

    fileTools = new QToolBar( this, "file operations" );
    addToolBar( fileTools, tr( "File" ), Top, TRUE );

	actionNewProject->addTo(fileTools);
	actionNewTable->addTo(fileTools);
	actionNewMatrix->addTo(fileTools);
	actionNewNote->addTo(fileTools);
	actionNewGraph->addTo(fileTools);
	actionNewFunctionPlot->addTo(fileTools);
	actionNewSurfacePlot->addTo(fileTools);
 
	fileTools->addSeparator ();

	actionOpen->addTo(fileTools);
	actionOpenTemplate->addTo(fileTools);
	actionSaveProject->addTo(fileTools);
	actionSaveTemplate->addTo(fileTools);

	fileTools->addSeparator ();

	actionLoad->addTo(fileTools);
	actionLoadMultiple->addTo(fileTools);

	fileTools->addSeparator ();

	actionCopyWindow->addTo(fileTools);
	actionPrint->addTo(fileTools);

	fileTools->addSeparator();

	actionShowExplorer->addTo(fileTools);
	actionShowLog->addTo(fileTools);

	editTools = new QToolBar( this, "edit operations" );
    addToolBar( editTools, tr( "Edit" ));

	actionUndo->addTo(editTools);
	actionRedo->addTo(editTools);
	actionCutSelection->addTo(editTools);
	actionCopySelection->addTo(editTools);
	actionPasteSelection->addTo(editTools);
	actionClearSelection->addTo(editTools);

	plotTools = new QToolBar( this, "file operations" );
    addToolBar( plotTools, tr( "Plot" ));

	actionAddLayer->addTo(plotTools);
	actionShowLayerDialog->addTo(plotTools);

	plotTools->addSeparator();

	actionShowCurvesDialog->addTo(plotTools);
	actionAddErrorBars->addTo(plotTools);
	actionAddFunctionCurve->addTo(plotTools);
	actionNewLegend->addTo(plotTools);

	plotTools->addSeparator ();

	actionUnzoom->addTo(plotTools);

	dataTools = new QActionGroup( this, "dataTools" );
    dataTools->setExclusive( TRUE );
    dataTools->setUsesDropDown( FALSE );

	btnPointer = new QAction(tr("Disable &tools"), QString::null, dataTools, "btnPointer" );
    btnPointer->setToggleAction( TRUE );
    btnPointer->setIconSet(QPixmap(pointer_xpm) );
	btnPointer->setOn(TRUE);
	btnPointer->addTo(plotTools);

	btnZoom = new QAction(tr("&Zoom"), tr("ALT+Z"), dataTools, "btnZoom" );
    btnZoom->setToggleAction( TRUE );
    btnZoom->setIconSet(QPixmap(zoom_xpm) );
	btnZoom->addTo(plotTools);

    btnCursor = new QAction(tr("&Data reader"), tr("CTRL+D"), dataTools, "btnCursor" );
    btnCursor->setToggleAction( TRUE );
    btnCursor->setIconSet(QPixmap(select_xpm) );
	btnCursor->addTo(plotTools);

	btnSelect = new QAction(tr("&Select data range"), tr("ALT+S"), dataTools, "btnSelect" );
    btnSelect->setToggleAction( TRUE );
    btnSelect->setIconSet(QPixmap(cursors_xpm) );
	btnSelect->addTo(plotTools);

	btnPicker = new QAction(tr("S&creen reader"), QString::null, dataTools, "btnPicker" );
    btnPicker->setToggleAction( TRUE );
    btnPicker->setIconSet(QPixmap(cursor_16) );
	btnPicker->addTo(plotTools);

	btnMovePoints = new QAction(tr("&Move Data Points..."), tr("Ctrl+ALT+M"), dataTools, "btnMovePoints" );
    btnMovePoints->setToggleAction( TRUE );
    btnMovePoints->setIconSet(QPixmap(hand_xpm) );
	btnMovePoints->addTo(plotTools);

	btnRemovePoints = new QAction(tr("Remove &Bad Data Points..."), tr("Alt+B"),dataTools, "btnRemovePoints" );
    btnRemovePoints->setToggleAction( TRUE );
    btnRemovePoints->setIconSet(QPixmap(gomme_xpm));
	btnRemovePoints->addTo(plotTools);

	connect( dataTools, SIGNAL( selected( QAction* ) ), this, SLOT( pickDataTool( QAction* ) ) );
	plotTools->addSeparator ();

	actionAddText = new QAction(QPixmap(text_xpm), tr("Add &Text"), tr("ALT+T"), this);
	actionAddText->setToggleAction(TRUE);
	connect(actionAddText, SIGNAL(activated()), this, SLOT(addText()));
	actionAddText->addTo(plotTools);

	btnLine = new QAction(tr("Draw &Arrow/Line"), tr("CTRL+ALT+L"), dataTools, "btnLine" );
    btnLine->setToggleAction( TRUE );
    btnLine->setIconSet(QPixmap(arrow_xpm) );
	btnLine->addTo(plotTools);

	actionTimeStamp->addTo(plotTools);
	actionAddImage->addTo(plotTools);

	tableTools = new QToolBar( this, "table operations" );
    addToolBar( tableTools, tr( "Table" ), Top,FALSE );

	actionPlotL->addTo(tableTools);
	actionPlotP->addTo(tableTools);
	actionPlotLP->addTo(tableTools);
	actionPlotVerticalBars->addTo(tableTools);
	actionPlotHorizontalBars->addTo(tableTools);
	actionPlotArea->addTo(tableTools);
	actionPlotPie->addTo(tableTools);
	actionPlotHistogram->addTo(tableTools);
	actionBoxPlot->addTo(tableTools);
	actionPlotVectXYXY->addTo(tableTools);
	actionPlotVectXYAM->addTo(tableTools);

	tableTools->addSeparator ();

	actionPlot3DRibbon->addTo(tableTools);
	actionPlot3DBars->addTo(tableTools);
	actionPlot3DScatter->addTo(tableTools);
	actionPlot3DTrajectory->addTo(tableTools);

	tableTools->addSeparator ();

	actionAddColToTable->addTo(tableTools);
	actionShowColStatistics->addTo(tableTools);
	actionShowRowStatistics->addTo(tableTools);

	plotTools->hide();
	tableTools->hide();
	
	displayBar = new QToolBar( this, "display operations" );
	info=new QLineEdit(displayBar,"test");
	info->setReadOnly(TRUE);

	displayBar->setStretchableWidget(info);
	displayBar->setResizeEnabled (true);
	displayBar->setMaximumHeight(2*displayBar->sizeHint().height());
	displayBar->setCloseMode(QDockWindow::Undocked);

    addToolBar(displayBar, tr( "Data Display" ), Top, FALSE );
	setDockEnabled(displayBar, DockLeft, false);
	setDockEnabled(displayBar, DockRight, false);
	displayBar->hide();
}

void ApplicationWindow::insertTranslatedStrings()
{
if (projectname == "untitled")
	setCaption(tr("QtiPlot - untitled"));

lv->setColumnText (0, tr("Name"));
lv->setColumnText (1, tr("Type"));
lv->setColumnText (2, tr("View"));
lv->setColumnText (3, tr("Size"));
lv->setColumnText (4, tr("Created"));
lv->setColumnText (5, tr("Label"));

explorerWindow->setCaption(tr("Project Explorer"));
logWindow->setCaption(tr("Results Log"));
displayBar->setLabel(tr("Data Display"));
tableTools->setLabel(tr("Table"));
plotTools->setLabel(tr("Plot"));
fileTools->setLabel(tr("File"));
editTools->setLabel(tr("Edit"));
plot3DTools->setLabel(tr("Surface 3D"));

file->changeItem(newMenuID, tr("&New"));
file->changeItem(recentMenuID, tr("&Recent projects"));
file->changeItem(exportID, tr("&Export Graph"));
file->changeItem(importMenuID, tr("&Import ASCII"));

plot2D->changeItem(specialPlotMenuID, tr("Special Line/Symb&ol"));
plot2D->changeItem(statMenuID, tr("Statistical &Graphs"));
plot2D->changeItem(panelMenuID, tr("Pa&nel"));
plot2D->changeItem(plot3dID, tr("Plot 3&D"));

dataMenu->changeItem(normMenuID, tr("&Normalize"));

tableMenu->changeItem(setAsMenuID, tr("Set columns &as"));
tableMenu->changeItem(fillMenuID, tr("&Fill columns with"));

calcul->changeItem(translateMenuID, tr("&Translate"));
calcul->changeItem(smoothMenuID, tr("&Smooth"));
calcul->changeItem(filterMenuID, tr("&FFT filter"));
calcul->changeItem(fitExpMenuID, tr("Fit E&xponential Decay"));
calcul->changeItem(multiPeakMenuID, tr("Fit &Multi-peak"));

translateActionsStrings();
customMenu(ws->activeWindow());
}

void ApplicationWindow::initMainMenu()
{
    file = new QPopupMenu( this );
	file->setFont(appFont);
	
	type = new QPopupMenu(this);
	type->setFont(appFont);
	actionNewProject->addTo(type);
	actionNewTable->addTo(type);
	actionNewMatrix->addTo(type);
	actionNewNote->addTo(type);
	actionNewGraph->addTo(type);
	actionNewFunctionPlot->addTo(type);
	actionNewSurfacePlot->addTo(type);

	newMenuID = file->insertItem(tr("&New"),type);
	actionOpen->addTo(file);

	recent = new QPopupMenu(this);
	recent->setFont(appFont);
	recentMenuID = file->insertItem(tr("&Recent projects"), recent);

	file->insertSeparator();

	actionLoadImage->addTo(file);
	actionImportImage->addTo(file);

	file->insertSeparator();

	actionSaveProject->addTo(file);
	actionSaveProjectAs->addTo(file);

    file->insertSeparator();
	actionOpenTemplate->addTo(file);
	actionSaveTemplate->addTo(file);
	file->insertSeparator();
	
	exportPlot = new QPopupMenu(this);
	actionExportGraph->addTo(exportPlot);
	actionExportAllGraphs->addTo(exportPlot);
	exportID=file->insertItem(tr("&Export Graph"), exportPlot);
			   
	actionPrint->addTo(file);
	actionPrintAllPlots->addTo(file);

	file->insertSeparator();

	actionShowExportASCIIDialog->addTo(file);

	import = new QPopupMenu(this);
	import->setFont(appFont);
	actionLoad->addTo(import);
	actionLoadMultiple->addTo(import);

	import->insertSeparator();

	actionShowImportDialog->addTo(import);
	importMenuID = file->insertItem(tr("&Import ASCII"),import);

	file->insertSeparator();

	actionCloseAllWindows->addTo(file);

	edit = new QPopupMenu(this);
	edit->setFont(appFont);
	actionUndo->addTo(edit);
	actionRedo->addTo(edit);

	edit->insertSeparator();

	actionCutSelection->addTo(edit);
	actionCopySelection->addTo(edit);
	actionPasteSelection->addTo(edit);
	actionClearSelection->addTo(edit);

	edit->insertSeparator();

	actionDeleteFitTables->addTo(edit);
	actionClearLogInfo->addTo(edit);

	view = new QPopupMenu(this);
	view->setFont(appFont);
	view->setCheckable(TRUE);
	actionShowPlotWizard->addTo(view);
	actionShowExplorer->addTo(view);
	actionShowLog->addTo(view);
	actionShowConfigureDialog->addTo(view);

	graph = new QPopupMenu(this);
	graph->setFont(appFont);
	graph->setCheckable(TRUE);
	actionShowCurvesDialog->addTo(graph);
	actionAddErrorBars->addTo(graph);
	actionAddFunctionCurve->addTo(graph);
	actionNewLegend->addTo(graph);

	graph->insertSeparator();

	actionAddText->addTo(graph);
	btnLine->addTo(graph);
	actionTimeStamp->addTo(graph);
	actionAddImage->addTo(graph);

	graph->insertSeparator();//layers section
	actionAddLayer->addTo(graph);
	actionDeleteLayer->addTo(graph);
	actionShowLayerDialog->addTo(graph);

	plot3DMenu = new QPopupMenu(this);
	plot3DMenu->setFont(appFont);

	actionPlot3DWireFrame->addTo(plot3DMenu);
	actionPlot3DHiddenLine->addTo(plot3DMenu);

	actionPlot3DPolygons->addTo(plot3DMenu);
	actionPlot3DWireSurface->addTo(plot3DMenu);

	plot3DMenu->insertSeparator();

	actionPlot3DBars->addTo(plot3DMenu);
	actionPlot3DScatter->addTo(plot3DMenu);

	matrixMenu = new QPopupMenu(this);
	matrixMenu->setFont(appFont);

	actionSetMatrixProperties->addTo(matrixMenu);
	actionSetMatrixDimensions->addTo(matrixMenu);
	actionSetMatrixValues->addTo(matrixMenu);

	matrixMenu->insertSeparator();

	actionTransposeMatrix->addTo(matrixMenu);
	actionInvertMatrix->addTo(matrixMenu);
	actionMatrixDeterminant->addTo(matrixMenu);

	matrixMenu->insertSeparator();
	actionConvertMatrix->addTo(matrixMenu);

	initPlotMenu();
	initTableAnalysisMenu();
	initTableMenu();
	initPlotDataMenu();

	calcul = new QPopupMenu( this );
	calcul->setFont(appFont);

	translateMenu = new QPopupMenu(this);
	translateMenu->setFont(appFont);
	actionTranslateVert->addTo(translateMenu);
	actionTranslateHor->addTo(translateMenu);
	translateMenuID = calcul->insertItem(tr("&Translate"),translateMenu);
	calcul->insertSeparator();

	actionDifferentiate->addTo(calcul);
	actionShowIntDialog->addTo(calcul);

	calcul->insertSeparator();

	smooth = new QPopupMenu(this);
	smooth->setFont(appFont);
	actionSmoothSavGol->addTo(smooth);
	actionSmoothAverage->addTo(smooth);
	actionSmoothFFT->addTo(smooth);
	smoothMenuID = calcul->insertItem(tr("&Smooth"),smooth);

	filter = new QPopupMenu(this);
	filter->setFont(appFont);
	actionLowPassFilter->addTo(filter);
	actionHighPassFilter->addTo(filter);
	actionBandPassFilter->addTo(filter);
	actionBandBlockFilter->addTo(filter);
	filterMenuID = calcul->insertItem(tr("&FFT filter"),filter);

	calcul->insertSeparator();
	actionInterpolate->addTo(calcul);
	actionFFT->addTo(calcul);
	calcul->insertSeparator();
	actionFitLinear->addTo(calcul);
	actionShowFitPolynomDialog->addTo(calcul);

	calcul->insertSeparator();

	decay = new QPopupMenu(this);
	decay->setFont(appFont);
	actionShowExpDecayDialog->addTo(decay);
	actionShowTwoExpDecayDialog->addTo(decay);
	actionShowExpDecay3Dialog->addTo(decay);
	fitExpMenuID = calcul->insertItem(tr("Fit E&xponential Decay"), decay);

	actionFitExpGrowth->addTo(calcul);
	actionFitSigmoidal->addTo(calcul);
	actionFitGauss->addTo(calcul);
	actionFitLorentz->addTo(calcul);

	multiPeakMenu = new QPopupMenu(this);
	multiPeakMenu->setFont(appFont);
	actionMultiPeakGauss->addTo(multiPeakMenu);
	actionMultiPeakLorentz->addTo(multiPeakMenu);
	multiPeakMenuID = calcul->insertItem(tr("Fit &Multi-peak"), multiPeakMenu);

	calcul->insertSeparator();

	actionShowFitDialog->addTo(calcul);

	format = new QPopupMenu(this);
	format->setFont(appFont);

	windowsMenu = new QPopupMenu( this );
	windowsMenu->setFont(appFont);
    windowsMenu->setCheckable( TRUE );
    connect( windowsMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( windowsMenuAboutToShow() ) );

    help = new QPopupMenu( this );
	help->setFont(appFont);

	actionShowHelp->addTo(help);
	actionChooseHelpFolder->addTo(help);
	help->insertSeparator();
	actionHomePage->addTo(help);
	//actionCheckUpdates->addTo(help);
	actionDownloadManual->addTo(help);
	actionTranslations->addTo(help);
	help->insertSeparator();
	actionTechnicalSupport->addTo(help);
	actionDonate->addTo(help);
	help->insertSeparator();
	actionAbout->addTo(help);

	disableActions();
}

void ApplicationWindow::initTableMenu()
{
tableMenu = new QPopupMenu(this);
tableMenu->setFont(appFont);

setAsMenu = new QPopupMenu(this);
setAsMenu->setFont(appFont);

actionSetXCol->addTo(setAsMenu);
actionSetYCol->addTo(setAsMenu);
actionSetZCol->addTo(setAsMenu);
actionDisregardCol->addTo(setAsMenu);
setAsMenuID = tableMenu->insertItem(tr("Set columns &as"), setAsMenu);

actionShowColumnOptionsDialog->addTo(tableMenu);
tableMenu->insertSeparator();

actionShowColumnValuesDialog->addTo(tableMenu);

fillMenu = new QPopupMenu(this);
fillMenu->setFont(appFont);
actionSetAscValues->addTo(fillMenu);
actionSetRandomValues->addTo(fillMenu);
fillMenuID = tableMenu->insertItem(tr("&Fill columns with"),fillMenu);

tableMenu->insertSeparator();
actionAddColToTable->addTo(tableMenu);
actionShowColsDialog->addTo(tableMenu);
actionShowRowsDialog->addTo(tableMenu);

tableMenu->insertSeparator();
actionConvertTable->addTo(tableMenu);
}

void ApplicationWindow::initPlotDataMenu()
{
	plotDataMenu = new QPopupMenu(this);
	plotDataMenu->setFont(appFont);
	plotDataMenu->setCheckable(TRUE);

	btnPointer->addTo(plotDataMenu);
	btnZoom->addTo(plotDataMenu);
	actionUnzoom->addTo(plotDataMenu);
	plotDataMenu->insertSeparator();

	btnCursor->addTo(plotDataMenu);
	btnSelect->addTo(plotDataMenu);
	btnPicker->addTo(plotDataMenu);

	plotDataMenu->insertSeparator();

	btnMovePoints->addTo(plotDataMenu);
	btnRemovePoints->addTo(plotDataMenu);
}

void ApplicationWindow::initPlotMenu()
{	  
	plot2D = new QPopupMenu(this);
	plot2D->setFont(appFont);
	specialPlot = new QPopupMenu(this);
	specialPlot->setFont(appFont);
	panels = new QPopupMenu(this);
	panels->setFont(appFont);
	stat = new QPopupMenu(this);
	stat->setFont(appFont);
	
	actionPlotL->addTo(plot2D);
	actionPlotP->addTo(plot2D);
	actionPlotLP->addTo(plot2D);

	actionPlotVerticalDropLines->addTo(specialPlot);
	actionPlotSpline->addTo(specialPlot);
	actionPlotSteps->addTo(specialPlot);
	specialPlotMenuID = plot2D->insertItem(tr("Special Line/Symb&ol"), specialPlot);

	plot2D->insertSeparator();

    actionPlotVerticalBars->addTo(plot2D);
    actionPlotHorizontalBars->addTo(plot2D);
    actionPlotArea->addTo(plot2D);
    actionPlotPie->addTo(plot2D);
    actionPlotVectXYXY->addTo(plot2D);
	actionPlotVectXYAM->addTo(plot2D);

	plot2D->insertSeparator();

	actionBoxPlot->addTo(stat);
	actionPlotHistogram->addTo(stat);
	actionPlotStackedHistograms->addTo(stat);
	statMenuID = plot2D->insertItem(tr("Statistical &Graphs"),stat);
	
	actionPlot2VerticalLayers->addTo(panels);
	actionPlot2HorizontalLayers->addTo(panels);
	actionPlot4Layers->addTo(panels);
	actionPlotStackedLayers->addTo(panels);
	panelMenuID = plot2D->insertItem(tr("Pa&nel"),panels);

	plot3D = new QPopupMenu(this);
	plot3D->setFont(appFont);
	actionPlot3DRibbon->addTo(plot3D);
	actionPlot3DBars->addTo(plot3D);
	actionPlot3DScatter->addTo(plot3D);
	actionPlot3DTrajectory->addTo(plot3D);

	plot2D->insertSeparator();
	plot3dID = plot2D->insertItem(tr("Plot 3&D"), plot3D);	
}

void ApplicationWindow::initTableAnalysisMenu()
{
	dataMenu = new QPopupMenu(this);
	dataMenu->setFont(appFont);

	actionShowColStatistics->addTo(dataMenu);
	actionShowRowStatistics->addTo(dataMenu);

	dataMenu->insertSeparator();

	actionSortSelection->addTo(dataMenu);
	actionSortTable->addTo(dataMenu);

	normMenu = new QPopupMenu(this);
	normMenu->setFont(appFont);
	normMenu->insertItem(tr("&Columns"), this, SLOT(normalizeSelection()));
	actionNormalizeTable->addTo(normMenu);
	normMenuID = dataMenu->insertItem(tr("&Normalize"), normMenu);

	dataMenu->insertSeparator();

	actionFFT->addTo(dataMenu);
	actionCorrelate->addTo(dataMenu);
	actionConvolute->addTo(dataMenu);
	actionDeconvolute->addTo(dataMenu);

	dataMenu->insertSeparator();
	actionShowFitDialog->addTo(dataMenu);
}

void ApplicationWindow::customMenu(QWidget* w)
{
menuBar()->clear();
menuBar()->insertItem(tr("&File"), file);
menuBar()->insertItem(tr("&Edit"), edit);
menuBar()->insertItem(tr("&View"), view);

if(w)
{
if ((int)plotWindows.count() > 0)
	actionPrintAllPlots->setEnabled(true);
else
	actionPrintAllPlots->setEnabled(false);

actionPrint->setEnabled(true);
actionCutSelection->setEnabled(true);
actionCopySelection->setEnabled(true);
actionPasteSelection->setEnabled(true);
actionClearSelection->setEnabled(true);
actionSaveTemplate->setEnabled(true);

if (w->isA("MultiLayer"))
	{
	menuBar()->insertItem(tr("&Graph"), graph);
	menuBar()->insertItem(tr("&Data"), plotDataMenu);
	menuBar()->insertItem(tr("&Analysis"), calcul);
	menuBar()->insertItem(tr("For&mat"), format);

	file->setItemEnabled (exportID,TRUE);
	actionShowExportASCIIDialog->setEnabled(FALSE);
	file->setItemEnabled (closeID,TRUE);

	format->clear();
	actionShowPlotDialog->addTo(format);
	actionShowLayoutDialog->addTo(format);
	Graph *g = ((MultiLayer*)w)->activeGraph();
	if (g && !g->isPiePlot())
		{
		format->insertSeparator();
		actionShowScaleDialog->addTo(format);
		actionShowAxisDialog->addTo(format);
		actionShowAxisDialog->setEnabled(true);
		format->insertSeparator();
		actionShowGridDialog->addTo(format);
		}
	actionShowTitleDialog->addTo(format);
	}
else if (w->isA("Graph3D"))
	{
	disableActions();

	menuBar()->insertItem(tr("For&mat"), format);

	actionPrint->setEnabled(TRUE);
	actionSaveTemplate->setEnabled(true);
	file->setItemEnabled (exportID,TRUE);
	file->setItemEnabled (closeID,TRUE);

	format->clear();
	actionShowPlotDialog->addTo(format);
	actionShowScaleDialog->addTo(format);
	actionShowAxisDialog->addTo(format);
	actionShowTitleDialog->addTo(format);
	if (((Graph3D*)w)->coordStyle() == Qwt3D::NOCOORD)
		actionShowAxisDialog->setEnabled(FALSE);
	}
else if (w->isA("Table"))
	{
	menuBar()->insertItem(tr("&Plot"), plot2D);	
	menuBar()->insertItem(tr("&Analysis"), dataMenu);	
	menuBar()->insertItem(tr("&Table"), tableMenu);

	actionShowExportASCIIDialog->setEnabled(TRUE);
	file->setItemEnabled (exportID,FALSE);
	file->setItemEnabled (closeID,TRUE);
	}
else if (w->isA("Matrix"))
	{
	menuBar()->insertItem(tr("&Plot3D"), plot3DMenu);
	menuBar()->insertItem(tr("&Matrix"), matrixMenu);
	}
else if (w->isA("Note"))
	actionSaveTemplate->setEnabled(false);
else
	disableActions();

menuBar()->insertItem(tr("&Windows"), windowsMenu );
}
else 
	disableActions();

menuBar()->insertItem(tr("&Help"), help );
}

void ApplicationWindow::disableActions()
{
actionSaveTemplate->setEnabled(false);
actionPrintAllPlots->setEnabled(false);
actionPrint->setEnabled(FALSE);
actionShowExportASCIIDialog->setEnabled(FALSE);
file->setItemEnabled (exportID,FALSE);
file->setItemEnabled (closeID,FALSE);

actionUndo->setEnabled(FALSE);
actionRedo->setEnabled(FALSE);

actionCutSelection->setEnabled(FALSE);
actionCopySelection->setEnabled(FALSE);
actionPasteSelection->setEnabled(FALSE);
actionClearSelection->setEnabled(FALSE);
}

void ApplicationWindow::customToolBars(QWidget* w)
{
if (w)
{
	if ((int)plot3DWindows.count()<=0)
		plot3DTools->hide();
	if ((int)plotWindows.count()<=0)
		plotTools->hide();
	if ((int)tableWindows.count()<=0)
		tableTools->hide();

    if (w->isA("MultiLayer"))
	{
	if (plotTools->isHidden())
		plotTools->show();
	
	plotTools->setEnabled (TRUE);
	plot3DTools->setEnabled (FALSE);
	tableTools->setEnabled(FALSE);
	}
	else if (w->isA("Table"))
	{
	if (tableTools->isHidden())
		tableTools->show();
	
	plotTools->setEnabled (FALSE);
	plot3DTools->setEnabled (FALSE);
	tableTools->setEnabled (TRUE);
	}
	else if (w->isA("Matrix"))
	{
	plotTools->setEnabled (FALSE);
	plot3DTools->setEnabled (FALSE);
	tableTools->setEnabled (FALSE);
	}
	else if (w->isA("Graph3D"))
	{
	plotTools->setEnabled (FALSE);
	tableTools->setEnabled (FALSE);

	if (plot3DTools->isHidden())
		plot3DTools->show();

	Graph3D* plot= (Graph3D*)w;
	if (plot->plotStyle() == Qwt3D::NOPLOT)
		plot3DTools->setEnabled (FALSE);
	else
		plot3DTools->setEnabled (TRUE);

	custom3DActions(w);
	}
	else if (w->isA("Note"))
	{	
	plotTools->setEnabled (FALSE);
	plot3DTools->setEnabled (FALSE);
	tableTools->setEnabled (FALSE);
	}

}
else
	hideToolbars();
}

void ApplicationWindow::hideToolbars()
{
	plot3DTools->hide();
	plotTools->hide();
	tableTools->hide();
		
	plotTools->setEnabled (FALSE);
	tableTools->setEnabled (FALSE);
	plot3DTools->setEnabled (FALSE);

}

void ApplicationWindow::showExplorer()
{
if (!explorerWindow->isVisible())
	explorerWindow->show();
else
	explorerWindow->hide();
}

void ApplicationWindow::plot3DRibbon()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	{
	if(int(w->selectedColumns().count())==1)
		w->plot3DRibbon();
	else
		QMessageBox::warning(0,tr("QtiPlot - Plot error"),tr("You must select exactly one column for plotting!"));
	}
}

void ApplicationWindow::plot3DWireframe()
{
plot3DMatrix (Qwt3D::WIREFRAME);
}

void ApplicationWindow::plot3DHiddenLine()
{
plot3DMatrix (Qwt3D::HIDDENLINE);
}

void ApplicationWindow::plot3DPolygons()
{
plot3DMatrix (Qwt3D::FILLED);
}

void ApplicationWindow::plot3DWireSurface()
{
plot3DMatrix (Qwt3D::FILLEDMESH);
}

void ApplicationWindow::plot3DBars()
{
QWidget* w = ws->activeWindow();
if (!w)
	return;

if (tableWindows.contains(w->name()))
	{
	Table* t = (Table*)w;

	if(int(t->selectedColumns().count())==1)
		t->plot3DBars();
	else
		QMessageBox::warning(0,tr("QtiPlot - Plot error"),tr("You must select exactly one column for plotting!"));
	}
else
	plot3DMatrix (Qwt3D::USER);
}

void ApplicationWindow::plot3DScatter()
{
QWidget* w = ws->activeWindow();
if (!w)
	return;

if (tableWindows.contains(w->name()))
	{
	Table* t = (Table*)w;

	if(int(t->selectedColumns().count())==1)
		t->plot3DScatter();
	else
		QMessageBox::warning(0,tr("QtiPlot - Plot error"),tr("You must select exactly one column for plotting!"));
	}
else if (matrixWindows.contains(w->name()))
	plot3DMatrix (Qwt3D::POINTS);
}

void ApplicationWindow::plot3DTrajectory()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	{
	if(int(w->selectedColumns().count())==1)
		w->plot3DTrajectory();
	else
		QMessageBox::warning(0, tr("QtiPlot - Plot error"),
							tr("You must select exactly one column for plotting!"));
	}
}

void ApplicationWindow::plotVerticalBars()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotVB();
}

void ApplicationWindow::plotHorizontalBars()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotHB();
}

void ApplicationWindow::plotHistogram()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotHistogram();
}

void ApplicationWindow::plotArea()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotArea();
}

void ApplicationWindow::plotPie()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	{
	if(int(w->selectedColumns().count())==1)
		w->plotPie();
	else
		QMessageBox::warning(0, tr("QtiPlot - Plot error"), 
			tr("You must select exactly one column for plotting!"));
	}
}

void ApplicationWindow::plotL()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotL();
}

void ApplicationWindow::plotP()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotP();
}

void ApplicationWindow::plotLP()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotLP();
}

void ApplicationWindow::plotVerticalDropLines()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotVerticalDropLines();
}

void ApplicationWindow::plotSpline()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotSpline();
}

void ApplicationWindow::plotSteps()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotSteps();
}

void ApplicationWindow::plotVectXYXY()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotVectXYXY();
}

void ApplicationWindow::plotVectXYAM()
{
Table* w = (Table*)ws->activeWindow();
if (w &&  tableWindows.contains(w->name()))
	w->plotVectXYAM();
}

void ApplicationWindow::updateTable(const QString& caption,int row,const QString& text)
{
Table* w = table(caption);
if (!w)
	return;

QStringList cvs=QStringList::split(",",caption,FALSE);
int pos=cvs[0].findRev("(");
QString colName=cvs[0].left(pos);
int xcol=w->colIndex(colName);
pos=cvs[1].findRev("(");
colName=cvs[1].left(pos);
int ycol=w->colIndex(colName);

if (w->columnType(xcol) == Table::Numeric && w->columnType(ycol) == Table::Numeric)
	{
	QStringList values=QStringList::split ("\t",text,FALSE);
	w->setText(row,xcol,values[0]);
	w->setText(row,ycol,values[1]);
	updateCurves(colName);
	emit modified();
	}
else
	QMessageBox::warning(this, tr("QtiPlot - Warning"),
	tr("This operation can not be performed on curves plotted from columns having a non-numerical format."));
}

void ApplicationWindow::updateTableColumn(const QString& colName, double *dat, int rows)
{
Table* w = table(colName);
if (!w)
	return;

int col=w->colIndex(colName);
if (w->columnType(col) == Table::Numeric)
	{
	int prec;
	char f;
	w->columnNumericFormat(col, f, prec);
	int i=0, j=0;
	while(i<rows && j<w->tableRows())
		{
		if(!w->text(j, col).isEmpty())
			{
			w->setText(j,col, QString::number(dat[i], f, prec));
			i++;
			}
		j++;
		}
	
	updateCurves(colName);
	delete[] dat;
	emit modified();
	}
else
	QMessageBox::warning(this, tr("QtiPlot - Warning"),
	tr("This operation can not be performed on curves plotted from columns having a non-numerical format."));
}

void ApplicationWindow::clearCellFromTable(const QString& name, double value)
{
Table* w = table(name);
if (w)
	{
	int col = w->colIndex(name);
	if (w->columnType(col) == Table::Numeric)
		{
		int row = w->atRow(col, value);
		w->clearCell(row, col);
		}
	else
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
	tr("This operation can not be performed on curves plotted from columns having a non-numerical format."));
	}
}

void ApplicationWindow::updateListView(const QString& caption)
{
QListViewItem *it=lv->findItem (caption,0, Qt::ExactMatch | Qt::CaseSensitive );
if (it)
	lv->takeItem(it);
}

void ApplicationWindow::renameListViewItem(const QString& oldName,const QString& newName)
{
QListViewItem *it=lv->findItem (oldName,0, Qt::ExactMatch | Qt::CaseSensitive );
if (it)
	it->setText(0,newName);
}

void ApplicationWindow::setListViewLabel(const QString& caption,const QString& label)
{
QListViewItem *it=lv->findItem ( caption, 0, Qt::ExactMatch | Qt::CaseSensitive );
if (it)
	it->setText(5,label);
}

void ApplicationWindow::setListViewDate(const QString& caption,const QString& date)
{
QListViewItem *it=lv->findItem ( caption, 0, Qt::ExactMatch | Qt::CaseSensitive );
if (it)
	it->setText(4,date);
}

void ApplicationWindow::setListView(const QString& caption,const QString& view)
{
QListViewItem *it=lv->findItem ( caption,0, Qt::ExactMatch | Qt::CaseSensitive );
if (it)
	it->setText(2,view);
}

void ApplicationWindow::setListViewSize(const QString& caption,const QString& size)
{
QListViewItem *it=lv->findItem ( caption,0, Qt::ExactMatch | Qt::CaseSensitive );
if (it)
	it->setText(3,size);
}

QString ApplicationWindow::listViewDate(const QString& caption)
{
QListViewItem *it=lv->findItem (caption,0,ExactMatch | CaseSensitive );
if (it)
	return it->text(4);
else
	return "";
}

void ApplicationWindow::updateTableNames(const QString& oldName, const QString& newName)
{
QWidgetList windows = ws->windowList();
int k, l, c=int(windows.count());
QStringList onPlot, cols, lst;
QString s, colName, endString;
for (int i=0;i<c;i++)
	{
	if (plotWindows.contains(windows.at(i)->name()))
		{
		MultiLayer *plot=(MultiLayer*)windows.at(i);
		if (!plot)
			return;

		QWidgetList *graphsList=plot->graphPtrs();
		for (int j=0;j<(int)graphsList->count();j++)
			{
			Graph *g=(Graph*)graphsList->at(j);

			//update plotted curves list
			onPlot=g->curvesList();
			for (k=0; k<(int)onPlot.count(); k++)
				{
				cols = QStringList::split("_", onPlot[k], false);
				if (cols[0] == oldName)
					onPlot[k] = newName + "_" + cols[1];
				}
			g->insertPlottedList(onPlot);

			//update plot associations
			onPlot=g->plotAssociations();
			for (k=0; k<(int)onPlot.count(); k++)
				{
				cols = QStringList::split (",", onPlot[k], false);
				for (l=0; l<(int)cols.count(); l++)
					{
					lst = QStringList::split ("_", cols[l], false);
					if (lst[0] == oldName)
						cols[l] = newName + "_" + lst[1];
					}
				onPlot[k] = cols.join (",");
				}
			g->setPlotAssociations(onPlot);

			//update legend
			QString legend=g->getLegendText();
			onPlot=QStringList::split ("\n",legend,FALSE );
			onPlot.gres (oldName,newName,TRUE);
			legend=onPlot.join("\n");
			g->setLegendText(legend);
			}
		}
	else if (plot3DWindows.contains(windows.at(i)->name()))
		{
		Graph3D* g = (Graph3D*)windows.at(i);
		QString name=g->formula();
		if (name.contains(oldName,TRUE))
			{
			name.replace(oldName,newName);
			g->setPlotAssociation(name);
			}
		}
	}
}

void ApplicationWindow::updateColNames(const QString& oldName, const QString& newName)
{
QWidgetList windows = ws->windowList();
int k, l, pos, c=int(windows.count());
QStringList onPlot, cols;
QString s, colName, endString;
for (int i=0;i<c;i++)
	{
	if (plotWindows.contains(windows.at(i)->name()))
		{
		MultiLayer *plot=(MultiLayer*)windows.at(i);
		if (!plot)
			return;

		QWidgetList *graphsList=plot->graphPtrs();
		for (int j=0;j<(int)graphsList->count();j++)
			{
			Graph *g=(Graph*)graphsList->at(j);

			//update plotted curves list
			onPlot=g->curvesList();
			for (k=0; k<(int)onPlot.count(); k++)
				{
				if (onPlot[k] == oldName)
					onPlot[k] = newName;
				}
			g->insertPlottedList(onPlot);

			//update plot associations
			onPlot=g->plotAssociations();
			for (k=0; k<(int)onPlot.count(); k++)
				{
				cols = QStringList::split (",", onPlot[k], false);
				for (l=0; l<(int)cols.count(); l++)
					{
					s = cols[l];
					pos = s.findRev("(");
					colName = s.left(pos); 
					endString = s.right(s.length()-pos);

					if (colName == oldName)
						cols[l] = newName + endString;
					}
				onPlot[k] = cols.join (",");
				}
			g->setPlotAssociations(onPlot);

			//update legend
			QString legend=g->getLegendText();
			onPlot=QStringList::split ("\n",legend,FALSE );
			onPlot.gres (oldName,newName,TRUE);
			legend=onPlot.join("\n");
			g->setLegendText(legend);
			}
		}
	else if (plot3DWindows.contains(windows.at(i)->name()))
		{
		Graph3D* g = (Graph3D*)windows.at(i);
		QString name=g->formula();
		if (name.contains(oldName))
			{
			name.replace(oldName,newName);
			g->setPlotAssociation(name);
			}
		}
	}
}

void ApplicationWindow::changeMatrixName(const QString& oldName, const QString& newName)
{
int index = matrixWindows.findIndex(oldName);
matrixWindows[index] = newName;

QWidget *w;
QWidgetList *lst = windowsList();
for (w = lst->first(); w; w = lst->next() )
	{
	if (w->isA("Graph3D"))
		{
		QString s = ((Graph3D*)w)->formula();
		if (s.contains(oldName))
			{
			s.replace(oldName, newName);
			((Graph3D*)w)->setPlotAssociation(s);
			}
		}
	}
delete lst;
}

void ApplicationWindow::remove3DMatrixPlots(Matrix *m)
{
QWidgetList windows = ws->windowList();
for (int i=0;i<(int)windows.count();i++)
	{
	Graph3D *plot=(Graph3D*)windows.at(i);
	if (plot && plot3DWindows.contains(plot->name()) && plot->getMatrix() == m)
		plot->clearData();
	}
}

void ApplicationWindow::update3DMatrixPlots(QWidget *w)
{
Matrix *m = (Matrix*)w;
if (!m)
	return;

QApplication::setOverrideCursor(waitCursor);

QWidgetList windows = ws->windowList();
for (int i=0;i<(int)windows.count();i++)
	{
	Graph3D *plot=(Graph3D*)windows.at(i);
	if (plot && plot3DWindows.contains(plot->name()) && plot->getMatrix() == m)
		plot->updateMatrixData(m);
	}

QApplication::restoreOverrideCursor();
}

void ApplicationWindow::add3DData()
{
if (tableWindows.count() <= 0)
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no tables available in this project.</h4>"
					  "<p><h4>Please create a table and try again!</h4>"));
	return;
	}

QStringList zColumns = columnsList(Table::Z);
if ((int)zColumns.count() <= 0)
	{
	QMessageBox::critical(this,tr("QtiPlot - Warning"),
				tr("There are no available columns with plot designation set to Z!"));
	return;
	}

analysisDialog *ad=new analysisDialog(0,"Column :", "analysisDialog",TRUE, WDestructiveClose);
connect (ad,SIGNAL(options(const QString&)), this, SLOT(insertNew3DData(const QString&)));
ad->setCaption(tr("QtiPlot - Choose data set"));
ad->setCurveNames(zColumns);
ad->showNormal();
ad->setActiveWindow();
}

void ApplicationWindow::change3DData()
{
analysisDialog *ad=new analysisDialog(0,"Column :", "analysisDialog",TRUE, WDestructiveClose);
connect (ad,SIGNAL(options(const QString&)), this, SLOT(change3DData(const QString&)));

ad->setCaption(tr("QtiPlot - Choose data set"));
ad->setCurveNames(columnsList(Table::Z));
ad->showNormal();
ad->setActiveWindow();
}

void ApplicationWindow::change3DMatrix()
{
analysisDialog *ad=new analysisDialog(0,"Matrix :", "analysisDialog",TRUE, WDestructiveClose);
connect (ad,SIGNAL(options(const QString&)), this, SLOT(change3DMatrix(const QString&)));

ad->setCaption(tr("QtiPlot - Choose matrix to plot"));
ad->setCurveNames(matrixWindows);
ad->showNormal();
ad->setActiveWindow();
}

void ApplicationWindow::change3DMatrix(const QString& matrix_name)
{
Graph3D* g = (Graph3D*)ws->activeWindow();
if ( g && plot3DWindows.contains(g->name()))
	{
	Matrix* w = matrix(matrix_name);
	g->changeMatrix(w);
	emit modified();
	}
}

void ApplicationWindow::add3DMatrixPlot()
{
if (matrixWindows.count() <= 0)
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no matrixes available in this project.</h4>"
					  "<p><h4>Please create a matrix and try again!</h4>"));
	return;
	}

analysisDialog *ad=new analysisDialog(0,"Matrix :", "analysisDialog",TRUE, WDestructiveClose);
connect (ad,SIGNAL(options(const QString&)), this, SLOT(insert3DMatrixPlot(const QString&)));

ad->setCaption(tr("QtiPlot - Choose matrix to plot"));
ad->setCurveNames(matrixWindows);
ad->showNormal();
ad->setActiveWindow();
}

void ApplicationWindow::insert3DMatrixPlot(const QString& matrix_name)
{
Graph3D* g = (Graph3D*)ws->activeWindow();
if ( g && plot3DWindows.contains(g->name()))
	{
	Matrix* w = matrix(matrix_name);
	g->addMatrixData(w);
	emit modified();
	}
}

void ApplicationWindow::insertNew3DData(const QString& colName)
{
Graph3D* g = (Graph3D*)ws->activeWindow();
if ( g && plot3DWindows.contains(g->name()))
	{
	Table* w=table(colName);
	g->insertNewData(w,colName);
	emit modified();
	}
}

void ApplicationWindow::change3DData(const QString& colName)
{
Graph3D* g = (Graph3D*)ws->activeWindow();
if ( g && plot3DWindows.contains(g->name()))
	{
	Table* w=table(colName);
	g->changeDataColumn(w,colName);
	emit modified();
	}
}

void ApplicationWindow::editSurfacePlot()
{
Graph3D* g = (Graph3D*)ws->activeWindow();
if ( g && plot3DWindows.contains(g->name()))
	{
	sDialog* sd= new sDialog(this,"fDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (sd,SIGNAL(options(const QString&,double,double,double,double,double,double)),
		g,SLOT(insertFunction(const QString&,double,double,double,double,double,double)));
	connect (sd,SIGNAL(clearFunctionsList()),this,SLOT(clearSurfaceFunctionsList()));

	sd->insertFunctionsList(surfaceFunc);
	if (g->hasData())
		{
		sd->setFunction(g->formula());
		sd->setLimits(g->xStart(), g->xStop(), g->yStart(), 
					g->yStop(), g->zStart(), g->zStop());
		}
	sd->show();
	sd->setActiveWindow();
	}
}

void ApplicationWindow::newSurfacePlot()
{
sDialog* sd= new sDialog(this,"fDialog",TRUE,WStyle_Tool|WDestructiveClose);
connect (sd,SIGNAL(options(const QString&,double,double,double,double,double,double)),
		this,SLOT(newPlot3D(const QString&,double,double,double,double,double,double)));
connect (sd,SIGNAL(clearFunctionsList()),this,SLOT(clearSurfaceFunctionsList()));

sd->insertFunctionsList(surfaceFunc);
sd->show();
sd->setActiveWindow();
}

Graph3D* ApplicationWindow::newPlot3D(const QString& formula, double xl, double xr,
									  double yl, double yr, double zl, double zr)
{
graphs++;
QString label="graph"+QString::number(graphs);
while(alreadyUsedName(label))
	{
	graphs++;
	label="graph"+QString::number(graphs);
	}

Graph3D *plot=new Graph3D("",ws,0,WDestructiveClose);
plot->addFunction(formula, xl, xr, yl, yr, zl, zr);
plot->resize(500,400);
plot->setCaption(label);
plot->setName(label);
customPlot3D(plot);
plot->update();	

initPlot3D(plot);

emit modified();
return plot;
}

void ApplicationWindow::updateSurfaceFuncList(const QString& s)
{
surfaceFunc.remove(s);
surfaceFunc.push_front(s);
while ((int)surfaceFunc.size() > 10)
	surfaceFunc.pop_back();
}

Graph3D* ApplicationWindow::newPlot3D(const QString& caption,const QString& formula,
									  double xl, double xr,double yl, double yr,
									  double zl, double zr)
{
Graph3D *plot=new Graph3D("",ws,0,WDestructiveClose);
plot->addFunction(formula, xl, xr, yl, yr, zl, zr);
plot->update();
	
QString label=caption;
while(alreadyUsedName(label))
	{
	graphs++;
	label="graph"+QString::number(graphs);
	}

plot->setCaption(label);
plot->setName(label);
initPlot3D(plot);
return plot;
}

Graph3D* ApplicationWindow::dataPlot3D(Table* table, const QString& colName)
{
QApplication::setOverrideCursor(waitCursor);

graphs++;
QString label="graph"+QString::number(graphs);
while(alreadyUsedName(label))
	{
	graphs++;
	label="graph"+QString::number(graphs);
	}

Graph3D *plot=new Graph3D("", ws, 0, WDestructiveClose);
plot->addData(table, colName);
plot->resize(500,400);
plot->setCaption(label);
plot->setName(label);

customPlot3D(plot);
plot->update();
initPlot3D(plot);

emit modified();
QApplication::restoreOverrideCursor();
return plot;
}

Graph3D* ApplicationWindow::dataPlot3D(const QString& caption,const QString& formula,
									double xl, double xr, double yl, double yr, double zl, double zr)
{
int pos=formula.find("_",0);
QString wCaption=formula.left(pos);

Table* w=table(wCaption);
if (!w)
	return 0;

int posX=formula.find("(",pos);
QString xCol=formula.mid(pos+1,posX-pos-1);

pos=formula.find(",",posX);
posX=formula.find("(",pos);
QString yCol=formula.mid(pos+1,posX-pos-1);

Graph3D *plot=new Graph3D("", ws, 0, WDestructiveClose);
plot->addData(w, xCol, yCol, xl, xr, yl, yr, zl, zr);
plot->update();

QString label=caption;
while(alreadyUsedName(label))
	{
	graphs++;
	label="graph"+QString::number(graphs);
	}

plot->setCaption(label);
plot->setName(label);
initPlot3D(plot);

return plot;
}

//plot ribbon from the plot wizard
Graph3D* ApplicationWindow::dataPlot3D(const QString& formula)
{
	QApplication::setOverrideCursor(waitCursor);

	int pos=formula.find(":",0);
	QString caption=formula.left(pos)+"_";
	Table *w=(Table *)table(caption);
	if (!w)
		{
		QApplication::restoreOverrideCursor();
		return 0;
		}

	int posX=formula.find("(",pos);
	QString xColName=caption+formula.mid(pos+2,posX-pos-2);

	posX=formula.find(",",posX);
	int posY=formula.find("(",posX);
	QString yColName=caption+formula.mid(posX+2,posY-posX-2);

	QString label="graph"+QString::number(++graphs);
	while(alreadyUsedName(label))
		{
		label="graph"+QString::number(++graphs);
		}

Graph3D *plot=new Graph3D("", ws, 0, WDestructiveClose);
plot->addData(w, xColName, yColName);
plot->resize(500,400);
plot->setCaption(label);
plot->setName(label);

customPlot3D(plot);
plot->update();
initPlot3D(plot);

emit modified();
QApplication::restoreOverrideCursor();
return plot;
}

Graph3D* ApplicationWindow::dataPlotXYZ(Table* table, const QString& zColName, int type)
{
QApplication::setOverrideCursor(waitCursor);
graphs++;
QString label="graph"+QString::number(graphs);
while(alreadyUsedName(label))
	{
	graphs++;
	label="graph"+QString::number(graphs);
	}

int zCol=table->colIndex(zColName);
int yCol=table->colY(zCol);
int xCol=table->colX(zCol);

Graph3D *plot=new Graph3D("", ws,0,WDestructiveClose);
plot->addData(table, xCol, yCol, zCol, type);
plot->resize(500,400);
plot->setCaption(label);
plot->setName(label);

customPlot3D(plot);
plot->update();
initPlot3D(plot);

emit modified();
QApplication::restoreOverrideCursor();
return plot;
}

Graph3D* ApplicationWindow::dataPlotXYZ(const QString& caption,const QString& formula,
 										double xl, double xr, double yl, double yr, double zl, double zr)
{
	int pos=formula.find("_",0);
	QString wCaption=formula.left(pos);

	Table* w=table(wCaption);
	if (!w)
		return 0;

	int posX=formula.find("(X)",pos);
	QString xColName=formula.mid(pos+1,posX-pos-1);

	pos=formula.find(",",posX);

	posX=formula.find("(Y)",pos);
	QString yColName=formula.mid(pos+1,posX-pos-1);

	pos=formula.find(",",posX);
	posX=formula.find("(Z)",pos);
	QString zColName=formula.mid(pos+1,posX-pos-1);

	int xCol=w->colIndex(xColName);
	int yCol=w->colIndex(yColName);
	int zCol=w->colIndex(zColName);

Graph3D *plot=new Graph3D("", ws, 0, WDestructiveClose);
plot->addData(w, xCol, yCol, zCol, xl, xr, yl, yr, zl, zr);
plot->update();

QString label=caption;
while(alreadyUsedName(label))
	{
	graphs++;
	label="graph"+QString::number(graphs);
	}

plot->setCaption(label);
plot->setName(label);
initPlot3D(plot);
return plot;
}

//plot 3D data from plot wizard string
Graph3D* ApplicationWindow::dataPlotXYZ(const QString& formula)
{
	QApplication::setOverrideCursor(waitCursor);

	int pos=formula.find(":",0);
	QString caption=formula.left(pos)+"_";
	Table *w=(Table *)table(caption);
	if (!w)
		{
		QApplication::restoreOverrideCursor();
		return 0;
		}

	int posX=formula.find("(",pos);
	QString xColName=caption+formula.mid(pos+2,posX-pos-2);

	posX=formula.find(",",posX);
	int posY=formula.find("(",posX);
	QString yColName=caption+formula.mid(posX+2,posY-posX-2);

	posY=formula.find(",",posY);
	int posZ=formula.find("(",posY);
	QString zColName=caption+formula.mid(posY+2,posZ-posY-2);

	int xCol=w->colIndex(xColName);
	int yCol=w->colIndex(yColName);
	int zCol=w->colIndex(zColName);

Graph3D *plot=new Graph3D("", ws,0,WDestructiveClose);
plot->addData(w, xCol, yCol, zCol, 1);
plot->resize(500,400);

QString label="graph"+QString::number(++graphs);
while(alreadyUsedName(label))
	{
	label="graph"+QString::number(++graphs);
	}
plot->setCaption(label);
plot->setName(label);
customPlot3D(plot);
plot->update();
initPlot3D(plot);

QApplication::restoreOverrideCursor();
return plot;
}

void ApplicationWindow::customPlot3D(Graph3D *plot)
{
plot->setDataColors(QColor(plot3DColors[4]), QColor(plot3DColors[0]));
plot->updateColors(QColor(plot3DColors[2]), QColor(plot3DColors[6]),
				   QColor(plot3DColors[5]), QColor(plot3DColors[1]),
				   QColor(plot3DColors[7]), QColor(plot3DColors[3]));

plot->setResolution(plot3DResolution);
plot->showColorLegend(showPlot3DLegend);
plot->setSmoothMesh(smooth3DMesh);
if (showPlot3DProjection)
	plot->setFloorData();

plot->setNumbersFont(plot3DNumbersFont);
plot->setXAxisLabelFont(plot3DAxesFont);
plot->setYAxisLabelFont(plot3DAxesFont);
plot->setZAxisLabelFont(plot3DAxesFont);
plot->setTitleFont(plot3DTitleFont);
}

void ApplicationWindow::initPlot3D(Graph3D *plot)
{
connectSurfacePlot(plot);

plot->setIcon(QPixmap(trajectory_xpm));
plot->show();
plot->setFocus();

addListViewItem(plot);
current_folder->addWindow(plot);

plot3DWindows << plot->name();

if (!plot3DTools->isVisible())
	plot3DTools->show();

if (!plot3DTools->isEnabled())
	plot3DTools->setEnabled(TRUE);

customMenu((QWidget*)plot);
customToolBars((QWidget*)plot);
}

void ApplicationWindow::importImage()
{
QStringList list=QImage::inputFormatList ();
QString filter="Images (*.jpg *.JPG ",aux;
int i;
for (i=0;i<(int)list.count();i++)
	{
	aux="*."+(list[i]).lower()+" *."+list[i]+" ";
	filter+=aux;
	}
filter+=");;";

aux = "*.jpg *.JPG;; ";
filter+=aux;
for (i=0;i<(int)list.count();i++)
	{
	aux="*."+(list[i]).lower()+" *."+list[i]+";;";
	filter+=aux;
	}

QString fn = QFileDialog::getOpenFileName(workingDir, filter, this, 0,
			tr("QtiPlot - Import image from file"), 0, TRUE);
if ( !fn.isEmpty() )
	{
	QApplication::setOverrideCursor(waitCursor);
	QPixmap photo;
	if ( fn.contains(".jpg", false))
		photo.load(fn,"JPEG",QPixmap::Auto);
	else
		{
		for (i=0;i<(int)list.count();i++)
			{
			if (fn.contains("." + list[i], false))
				{
				photo.load(fn,list[i],QPixmap::Auto);
				break;
				}
			}
		}

	Matrix* m = createIntensityMatrix(photo);
	m->setWindowLabel(fn);
	m->setCaptionPolicy(myWidget::Both);
	setListViewLabel(m->name(), fn);

	QFileInfo fi(fn);
	workingDir = fi.dirPath(true);

	QApplication::restoreOverrideCursor();
	}
}

void ApplicationWindow::loadImage()
{
QStringList list=QImage::inputFormatList ();
QString filter="Images (*.jpg *.JPG ",aux;
int i;
for (i=0;i<(int)list.count();i++)
	{
	aux="*."+(list[i]).lower()+" *."+list[i]+" ";
	filter+=aux;
	}
filter+=");;";

aux = "*.jpg *.JPG;; ";
filter+=aux;
for (i=0;i<(int)list.count();i++)
	{
	aux="*."+(list[i]).lower()+" *."+list[i]+";;";
	filter+=aux;
	}

QString fn = QFileDialog::getOpenFileName(workingDir, filter, this, 0,
			tr("QtiPlot - Load image from file"), 0, TRUE);
if ( !fn.isEmpty() )
	{
	loadImage(fn);
	QFileInfo fi(fn);
	workingDir = fi.dirPath(true);
	}
}

void ApplicationWindow::loadImage(const QString& fn)
{
QApplication::setOverrideCursor(waitCursor);
QPixmap photo;
if ( fn.contains(".jpg", false))
	photo.load(fn,"JPEG",QPixmap::Auto);
else
	{
	QStringList lst=QImage::inputFormatList();
	for (int i=0; i<(int)lst.count(); i++)
		{
		if (fn.contains("." + lst[i], false))
			{
			photo.load(fn, lst[i], QPixmap::Auto);
			break;
			}
		}
	}

MultiLayer *plot = multilayerPlot("graph" + QString::number(++graphs));
plot->setWindowLabel(fn);
plot->setCaptionPolicy(myWidget::Both);
setListViewLabel(plot->name(), fn);

if (plot->height()-20>photo.height())
	plot->setGeometry(0,0, plot->width(), photo.height()+20);
	
plot->showNormal();
Graph *g=plot->addLayer(0,0, plot->width(), plot->height()-20);

g->setTitle("");
QMemArray<bool> axesOn(4);
for (int j=0;j<4;j++)
	axesOn[j]=FALSE;
g->enableAxes(axesOn);
g->removeLegend();
g->insertImageMarker(photo,fn);
plot->connectLayer(g);
QApplication::restoreOverrideCursor();
}

void ApplicationWindow::polishGraph(Graph *g, int style)
{
if (style == Graph::VerticalBars || style == Graph::HorizontalBars ||style == Graph::Histogram)
	{
	QValueList<int> ticksList;
	int ticksStyle = Plot::Out;
	ticksList<<ticksStyle<<ticksStyle<<ticksStyle<<ticksStyle;
	g->setTicksType(ticksList);
	}
if (style == Graph::HorizontalBars)
	{
	g->setAxisTitle(0, tr("Y Axis Title"));
	g->setAxisTitle(1, tr("X Axis Title"));
	}
}

MultiLayer* ApplicationWindow::multilayerPlot(const QString& caption)
{
MultiLayer* g = new MultiLayer("", ws,0,WDestructiveClose);
QString label=caption;
initMultilayerPlot(g, label.replace(QRegExp("_"),"-"));
return g;
}

/*
*creates a new empty multilayer plot
*/
void ApplicationWindow::newGraph()
{
MultiLayer* g = multilayerPlot(QString("graph1"));
if (g)
	{
	g->showNormal();
	activeGraph = g->addLayer();
	customGraph(activeGraph);
	activeGraph->replot();
	}
}

MultiLayer* ApplicationWindow::multilayerPlot(Table* w, const QStringList& colList, int style)
{//used when plotting selected columns
QApplication::setOverrideCursor(waitCursor);

MultiLayer* g = new MultiLayer("",ws,0,WDestructiveClose);
g->askOnCloseEvent(confirmClosePlot2D);

activeGraph=g->insertFirstLayer();
if (!activeGraph)
	return 0;

activeGraph->insertCurvesList(w, colList, style, defaultCurveLineWidth, defaultSymbolSize);

customGraph(activeGraph);
polishGraph(activeGraph, style);
initMultilayerPlot(g, "graph"+QString::number(++graphs));

//the following function must be called last in order to avoid resizing problems
activeGraph->setIgnoreResizeEvents(!autoResizeLayers);
emit modified();
QApplication::restoreOverrideCursor();
return g;
}

MultiLayer* ApplicationWindow::multilayerPlot(int c, int r, int style)
{//used when plotting with the panel menu
Table* w = (Table*)ws->activeWindow();
if (!w || !tableWindows.contains(w->name()) || !w->valid2DPlot())
	return 0;

QStringList list=w->selectedYColumns();
if((int)list.count() < 1)
	{
	QMessageBox::warning(0, tr("QtiPlot - Plot error"), 
				tr("Please select a Y column to plot!"));
	return 0;
	}

int curves= (int)list.count();
if (r<0)
	r = curves;

MultiLayer* g = new MultiLayer("", ws,0,WDestructiveClose);
g->askOnCloseEvent(confirmClosePlot2D);
initMultilayerPlot(g, "graph"+QString::number(++graphs));
int layers=c*r;
if (curves<layers)
	{
	for (int i=0; i<curves; i++)
		{
		activeGraph=g->addLayer();
		if (activeGraph)
			{
			activeGraph->insertCurvesList(w, list[i], style, defaultCurveLineWidth, defaultSymbolSize);
			customGraph(activeGraph);
			activeGraph->setAutoscaleFonts(false);//in order to avoid to small fonts
			activeGraph->setIgnoreResizeEvents(!autoResizeLayers);
			polishGraph(activeGraph, style);
			}
		}
	}
else
	{
	for (int i=0; i<layers;i++)
		{
		activeGraph=g->addLayer();
		if (activeGraph)
			{
			activeGraph->insertCurvesList(w, list[i], style, defaultCurveLineWidth, defaultSymbolSize);
			customGraph(activeGraph);
			activeGraph->setAutoscaleFonts(false);//in order to avoid to small fonts
			activeGraph->setIgnoreResizeEvents(!autoResizeLayers);
			polishGraph(activeGraph, style);
			}
		}
	}
g->setRows(r);
g->setCols(c);
g->arrangeLayers(false, false);

QWidgetList *lst = g->graphPtrs();
for (int i=0; i<g->graphsNumber();i++)
	{
	Graph *ag = (Graph *)lst->at(i);
	ag->setAutoscaleFonts(autoScaleFonts);//restore user defined fonts behaviour
	}
emit modified();
return g;
}

MultiLayer* ApplicationWindow::multilayerPlot(const QStringList& colList)
{//used when plotting from wizard
QApplication::setOverrideCursor(waitCursor);
MultiLayer* g = new MultiLayer("", ws,0,WDestructiveClose);
Graph *ag=g->insertFirstLayer();
customGraph(ag);
polishGraph(ag, defaultCurveStyle);
int curves = (int)colList.count();
for (int i=0;i<(int)colList.count();i++)
	{
	QString s=colList[i];
	int pos=s.find(":",0);
	QString caption=s.left(pos)+"_";
	Table *w=(Table *)table(caption);

	int posX=s.find("(X)",pos);
	QString xColName=caption+s.mid(pos+2,posX-pos-2);
	int xCol=w->colIndex(xColName);

	posX=s.find(",",posX);
	int posY=s.find("(Y)",posX);
	QString yColName=caption+s.mid(posX+2,posY-posX-2);

	if (s.contains("(yErr)") || s.contains("(xErr)"))
		{
		curves--;
		posY=s.find(",",posY);
		int posErr, errType;
		if (s.contains("(yErr)"))
			{
			errType = QwtErrorPlotCurve::Vertical;
			posErr=s.find("(yErr)",posY);
			}
		else
			{
			errType = QwtErrorPlotCurve::Horizontal;
			posErr=s.find("(xErr)",posY);
			}

		QString errColName=caption+s.mid(posY+2,posErr-posY-2);	
		ag->addErrorBars(w,xColName,yColName,w,errColName,errType,2,5,QColor(black),false,true,true);
		}
	else
		{
		if (ag->insertCurve(w, xCol, yColName, defaultCurveStyle))
			{
			curveLayout cl = ag->initCurveLayout(i, curves, defaultCurveStyle);
			cl.lWidth = defaultCurveLineWidth;
			cl.sSize = defaultSymbolSize;

			ag->updateCurveLayout(i,&cl);
			}
		}
	}
ag->updatePlot();
initMultilayerPlot(g, "graph"+QString::number(++graphs));
ag->setIgnoreResizeEvents(!autoResizeLayers);
emit modified();
QApplication::restoreOverrideCursor();
return g;
}

void ApplicationWindow::initMultilayerPlot(MultiLayer* g, const QString& name)
{
connectMultilayerPlot(g);
	
QString label = name;
while(alreadyUsedName(label))
	{
	label="graph"+QString::number(++graphs);
	}

plotWindows<<label;

g->setCaption(label);
g->setName(label);
g->setIcon(QPixmap(graph_xpm));
g->showNormal();
g->setFocus();

addListViewItem(g);
current_folder->addWindow(g);
}

void ApplicationWindow::customizeTables(const QColor& bgColor,const QColor& textColor,
										const QColor& headerColor,const QFont& textFont,
										const QFont& headerFont)
{
if (tableBkgdColor == bgColor && tableTextColor == textColor &&
	tableHeaderColor == headerColor && tableTextFont == textFont &&
	tableHeaderFont == headerFont)
return;

tableBkgdColor = bgColor;
tableTextColor = textColor;
tableHeaderColor = headerColor;
tableTextFont = textFont;
tableHeaderFont = headerFont;

QWidgetList *windows = windowsList(); 
for (int i = 0; i < int(windows->count());i++ )
	{
	if (tableWindows.contains(windows->at(i)->name()))
		customTable((Table*)windows->at(i));
	}
delete windows;
}

void ApplicationWindow::customTable(Table* w)
{
w->setBackgroundColor(tableBkgdColor);
w->setTextColor (tableTextColor);
w->setHeaderColor (tableHeaderColor);
w->setTextFont(tableTextFont);
w->setHeaderFont(tableHeaderFont);
}

void ApplicationWindow::customGraph(Graph* g)
{
if (!g->isPiePlot())
	{
	if (allAxesOn)
		{
		QMemArray<bool> axesOn(QwtPlot::axisCnt);
		axesOn.fill (true);
		g->enableAxes(axesOn);
		
		QStringList lst;
		lst<<"1"<<"0"<<"1"<<"0";
		g->setEnabledTickLabels(lst);

		g->updateSecondaryAxis(QwtPlot::xTop);
		g->updateSecondaryAxis(QwtPlot::yRight);
		}
	
	QValueList<int> ticksList;
	ticksList<<ticksStyle<<ticksStyle<<ticksStyle<<ticksStyle;
	g->setTicksType(ticksList);
	g->setTicksLength (minTicksLength, majTicksLength);
	g->setAxesLinewidth(axesLineWidth);
	g->drawAxesBackbones(drawBackbones);
	}

LegendMarker* legend = g->legend();
if (legend)
	{
	legend->setBackground(legendFrameStyle);
	legend->setFont(plotLegendFont);
	}
g->initFonts(plotAxesFont, plotNumbersFont, plotLegendFont);
g->setTextMarkerDefaultFrame(legendFrameStyle);
g->initTitleFont(plotTitleFont);
g->initTitle(titleOn);
g->drawCanvasFrame(canvasFrameOn, canvasFrameWidth);
g->plotWidget()->setMargin(defaultPlotMargin);
g->enableAutoscaling(autoscale2DPlots);
g->setAutoscaleFonts(autoScaleFonts);
}

void ApplicationWindow::newWrksheetPlot(const QString& caption, int r, int c, const QString& text)
{
Table* w =newTable(caption, r, c, text);
MultiLayer* plot=multilayerPlot(w, QStringList(QString(w->name())+"_intensity"), 0);
Graph *g=(Graph*)plot->activeGraph();
if (g)
	{
	g->setTitle("");
	g->setXAxisTitle("pixels");
	g->setYAxisTitle("pixel intensity (a.u.)");
	}
}

/*
*used when importing an ASCII file
*/
Table* ApplicationWindow::newTable(const QString& fname, const QString &sep, 
								   int lines, bool renameCols, bool stripSpaces, 
								   bool simplifySpaces)
{
Table* w = new Table(fname, sep, lines, renameCols, stripSpaces, 
					 simplifySpaces, fname, ws, 0, WDestructiveClose);	
initTable(w, "table"+QString::number(++tables));
w->show();
return w;
}

/*
*creates a new empty table
*/
Table* ApplicationWindow::newTable()
{
Table* w = new Table(30, 2, "", ws, 0, WDestructiveClose);
initTable(w, "table"+QString::number(++tables));
w->showNormal();	
return w;
}

/*
*used when opening a project file
*/
Table* ApplicationWindow::newTable(const QString& caption, int r, int c)
{
Table* w = new Table(r, c, "", ws,0,WDestructiveClose);
initTable(w, caption);
if (w->name() != caption)//the table was renamed
	{
	renamedTables << caption << w->name();

	QApplication::restoreOverrideCursor();
	QMessageBox:: warning(this, "QtiPlot - Renamed Window", 
	tr("The table '%1' already exists. It has been renamed '%2'.").arg(caption).arg(w->name()));
	QApplication::setOverrideCursor(waitCursor);
	}
return w;
}


Table* ApplicationWindow::newTable(const QString& caption, int r, int c, const QString& text)
{
QStringList lst = QStringList::split("\t", caption, false);
Table* w = new Table(r, c, lst[1], ws, 0, WDestructiveClose);
	
QStringList rows=QStringList::split ("\n",text,FALSE);
QString rlist=rows[0];
QStringList list=QStringList::split ("\t",rlist,TRUE);
w->setHeader(list);

for (int i=0; i<r; i++)
	{
	rlist=rows[i+1];
	list=QStringList::split ("\t",rlist,TRUE);
	for (int j=0; j<c; j++)
		w->setText(i, j, list[j]);
	}
	
initTable(w, lst[0]);
w->setCaptionPolicy(myWidget::Both);
w->showNormal();
return w;
}

/*
*used to return the result of an analysis operation
*/
Table* ApplicationWindow::newHiddenTable(const QString& caption, int r, int c, const QString& text)
{
QStringList lst = QStringList::split("\t", caption, false);
Table* w = new Table(r, c, lst[1], 0, 0, WDestructiveClose);
	
QStringList rows=QStringList::split ("\n",text,FALSE);
QString rlist=rows[0];
QStringList list=QStringList::split ("\t",rlist,TRUE);
w->setHeader(list);

for (int i=0; i<r; i++)
	{
	rlist=rows[i+1];
	list=QStringList::split ("\t",rlist,TRUE);
	for (int j=0; j<c; j++)
		w->setText(i, j, list[j]);
	}
	
initTable(w, lst[0]);
w->setCaptionPolicy(myWidget::Both);
outWindows->append(w);
w->setHidden();
return w;
}

void ApplicationWindow::initTable(Table* w, const QString& caption)
{
connectTable(w);
customTable(w);

QString name=caption;
name=name.replace ("_","-");

while(alreadyUsedName(name)){
	name="table"+QString::number(++tables);}

tableWindows<<name;
w->setCaption(name);
w->setName(name);
w->setIcon( QPixmap(worksheet_xpm) );
	
addListViewItem(w);
current_folder->addWindow(w);
	
emit modified();
}

void ApplicationWindow::showHistogramTable(const QString& caption, int r, int c, const QString& text)
{
Table* w = newTable(caption, r, c,text);
w->showMaximized();
}

/*
*creates a new empty note window
*/
Note* ApplicationWindow::newNote(const QString& caption)
{
Note* m = new Note("", ws, 0, WDestructiveClose);
if (caption.isEmpty())
	initNote(m,"Note" + QString::number(++notes));
else
	initNote(m, caption);
m->showNormal();	
return m;
}

void ApplicationWindow::initNote(Note* m, const QString& caption)
{
QString name=caption;
while(alreadyUsedName(name))
	name = "Note"+QString::number(++notes);

noteWindows<<name;
	
m->setCaption(name);
m->setName(name);
m->setIcon( QPixmap(note_xpm) );
m->askOnCloseEvent(confirmCloseNotes);

addListViewItem(m);
current_folder->addWindow(m);

connect(m->textWidget(), SIGNAL(undoAvailable(bool)), actionUndo, SLOT(setEnabled(bool)));
connect(m->textWidget(), SIGNAL(redoAvailable(bool)), actionRedo, SLOT(setEnabled(bool)));
connect(m, SIGNAL(modifiedWindow(QWidget*)), this, SLOT(modifiedProject(QWidget*)));
connect(m, SIGNAL(closedWindow(QWidget*)), this, SLOT(closeWindow(QWidget*)));
connect(m, SIGNAL(hiddenWindow(myWidget*)), this, SLOT(hideWindow(myWidget*)));
connect(m,SIGNAL(statusChanged(myWidget*)),this, SLOT(updateWindowStatus(myWidget*)));
		
emit modified();
}

/*
*creates a new empty matrix
*/
Matrix* ApplicationWindow::newMatrix()
{
Matrix* m = new Matrix(32, 32, "", ws, 0, WDestructiveClose);
matrixes++;
QString caption="Matrix" + QString::number(matrixes);
initMatrix(m, caption);
m->showNormal();	
return m;
}

/*
*used when opening a project file
*/
Matrix* ApplicationWindow::newMatrix(const QString& caption, int r, int c)
{
Matrix* w = new Matrix(r, c, "", ws,0,WDestructiveClose);
initMatrix(w, caption);
if (w->name() != caption)//the matrix was renamed
	{
	renamedTables << caption << w->name();

	QApplication::restoreOverrideCursor();
	QMessageBox:: warning(this, "QtiPlot - Renamed Window", 
	tr("The matrix '%1' already exists. It has been renamed '%2'.").arg(caption).arg(w->name()));
	QApplication::setOverrideCursor(waitCursor);
	}
return w;
}

void ApplicationWindow::transposeMatrix()
{
Matrix* m = (Matrix*)ws->activeWindow();
if (!m)
	return;

QApplication::setOverrideCursor(waitCursor);
m->transpose();
QApplication::restoreOverrideCursor();
}

void ApplicationWindow::matrixDeterminant()
{
Matrix* m = (Matrix*)ws->activeWindow();
if (!m)
	return;

QDateTime dt = QDateTime::currentDateTime ();
QString info=dt.toString(Qt::LocalDate);
info+= "\n" + tr("Determinant of ") + QString(m->name()) + ":\t"; 
info+= "det = " + QString::number(m->determinant()) + "\n";
info+="-------------------------------------------------------------\n";

logInfo+=info;

showResults(true);
}

void ApplicationWindow::invertMatrix()
{
Matrix* m = (Matrix*)ws->activeWindow();
if (!m)
	return;

m->invert();
}

Table* ApplicationWindow::convertMatrixToTable()
{
Matrix* m = (Matrix*)ws->activeWindow();
if (!m)
	return 0;

QApplication::setOverrideCursor(waitCursor);

int rows = m->numRows();
int cols = m->numCols();

Table* w = new Table(rows, cols, "", ws, 0, WDestructiveClose);
for (int i = 0; i<rows; i++)
	{
	for (int j = 0; j<cols; j++)
		w->setText(i, j, m->text(i,j));
	}

tables++;
QString caption="table"+QString::number(tables);
initTable(w, caption);

w->setWindowLabel(m->windowLabel());
w->setCaptionPolicy(m->captionPolicy());
w->resize(m->size());
w->showNormal();

QApplication::restoreOverrideCursor();

return w;
}

void ApplicationWindow::initMatrix(Matrix* m, const QString& caption)
{
QString name=caption;
while(alreadyUsedName(name))
	{
	name = "Matrix"+QString::number(++matrixes);
	}

matrixWindows<<name;
	
m->setCaption(name);
m->setName(name);
m->setIcon( QPixmap(matrix_xpm) );
m->askOnCloseEvent(confirmCloseMatrix);

addListViewItem(m);
current_folder->addWindow(m);

connect(m, SIGNAL(modifiedWindow(QWidget*)), this, SLOT(modifiedProject()));
connect(m, SIGNAL(modifiedWindow(QWidget*)), this, SLOT(update3DMatrixPlots(QWidget *)));
connect(m, SIGNAL(closedWindow(QWidget*)), this, SLOT(closeWindow(QWidget*)));
connect(m, SIGNAL(hiddenWindow(myWidget*)), this, SLOT(hideWindow(myWidget*)));
connect(m, SIGNAL(statusChanged(myWidget*)),this, SLOT(updateWindowStatus(myWidget*)));
connect(m, SIGNAL(showContextMenu()), this, SLOT(showWindowContextMenu()));
		
emit modified();
}

Matrix* ApplicationWindow::convertTableToMatrix()
{
Table* m = (Table*)ws->activeWindow();
if (!m)
	return 0;

QApplication::setOverrideCursor(waitCursor);

int rows = m->tableRows();
int cols = m->tableCols();

Matrix* w = new Matrix(rows, cols, "", ws, 0, WDestructiveClose);
for (int i = 0; i<rows; i++)
	{
	for (int j = 0; j<cols; j++)
		w->setText(i, j, m->text(i,j));
	}

matrixes++;
QString caption="Matrix"+QString::number(matrixes);
initMatrix(w, caption);

w->setWindowLabel(m->windowLabel());
w->setCaptionPolicy(m->captionPolicy());
w->resize(m->size());
w->showNormal();

QApplication::restoreOverrideCursor();
return w;
}

Graph3D* ApplicationWindow::surfacePlot(const QString& name)
{
Graph3D* w=0;
QWidgetList *windows = windowsList();
for (int i = 0; i < int(windows->count());i++ )
	{
	if (plot3DWindows.contains(name) && windows->at(i)->name()==name)
		{
		w=(Graph3D*)windows->at(i);
		break;
		}
	}
delete windows;
return  w;
}

MultiLayer* ApplicationWindow::plot(const QString& name)
{
int pos=name.find("_",0);
QString caption=name.left(pos);
MultiLayer* w=0;
QWidgetList *windows = windowsList();
for (int i = 0; i < int(windows->count());i++ )
	{
	if (plotWindows.contains(caption) && windows->at(i)->name()==caption)
		{
		w=(MultiLayer*)windows->at(i);
		break;
		}
	}
delete windows;
return  w;
}

Table* ApplicationWindow::table(const QString& name)
{
int pos=name.find("_",0);
QString caption=name.left(pos);

QWidget *w;
QWidgetList *lst = windowsList();
for (w = lst->first(); w; w = lst->next() )
	{
	if (w->isA("Table") && w->name() == caption)
		{
		delete lst;
		return (Table*)w;
		}
	}
delete lst;
return  0;
}

Matrix* ApplicationWindow::matrix(const QString& name)
{
QString caption = name;
if (!renamedTables.isEmpty() && renamedTables.contains(caption))
	{
	int index = renamedTables.findIndex (caption);
	caption = renamedTables[index+1];	
	}

QWidget *w;
QWidgetList *lst = windowsList();
for (w = lst->first(); w; w = lst->next() )
	{
	if (w->isA("Matrix") && w->name() == caption)
		{
		delete lst;
		return (Matrix*)w;
		}
	}
delete lst;
return  0;
}

void ApplicationWindow::windowActivated(QWidget *w)
{
customToolBars(w);
customMenu(w);
emit modified();
}

void ApplicationWindow::addErrorBars()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if ( g )
	{
	if (g->isPiePlot())
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));
	else
		{
		activeGraph=g;
		errDialog* ed= new errDialog(this,"errDialog",TRUE,WStyle_Tool|WDestructiveClose);
		connect (ed,SIGNAL(options(const QString&,int,const QString&,int)),this,SLOT(defineErrorBars(const QString&,int,const QString&,int)));
		connect (ed,SIGNAL(options(const QString&,const QString&,int)),this,SLOT(defineErrorBars(const QString&,const QString&,int)));

		QStringList curvesOnPlot=activeGraph->curvesList();

		ed->setCurveNames(curvesOnPlot);
		ed->setExistingColumns(columnsList(Table::All));
		ed->showNormal();
		ed->setActiveWindow();
		}
	}
}

void ApplicationWindow::defineErrorBars(const QString& name, int type,
										const QString& percent, int direction)
{
	Table *w=table(name);
	if (!w)
		{ //user defined function
		QMessageBox::critical(this,tr("QtiPlot - Error bars error"),
				tr("This feature is not available for user defined function curves!"));
		return;
		}

	QString xColName = activeGraph->curveXColName(name);
	if (xColName.isEmpty())
		return;

	w->addCol();
	int r=w->tableRows();
	int c=w->tableCols()-1;
	int ycol=w->colIndex(name);
	if (!direction)
		ycol=w->colIndex(xColName);

	QMemArray<double> Y(r);
	Y=w->col(ycol);
	QString errColName=w->colName(c);

	double prc=percent.toDouble();
	double moyenne=0.0;
	if (type==0)
		{
		for (int i=0;i<r;i++)
			{
			if (!w->table()->text(i,ycol).isEmpty())
				w->setText(i,c,QString::number(Y[i]*prc/100.0,'g',15));
			}
		}
	else if (type==1)
		{
		int i;
		double dev=0.0;
		for (i=0;i<r;i++)
			moyenne+=Y[i];
		moyenne/=r;
		for (i=0;i<r;i++)
			dev+=(Y[i]-moyenne)*(Y[i]-moyenne);
		dev=sqrt(dev/(r-1));
		for (i=0;i<r;i++)
			{
			if (!w->table()->text(i,ycol).isEmpty())
				w->setText(i,c,QString::number(dev,'g',15));
			}
		}		
activeGraph->addErrorBars(w, xColName, name, w, errColName, 
						   direction, 2, 5, QColor(black), false, true, true);
}

void ApplicationWindow::defineErrorBars(const QString& curveName, 
										const QString& errColumnName, int direction)
{
Table *w=table(curveName);
if (!w) 
	{//user defined function --> no worksheet available
	QMessageBox::critical(this,tr("QtiPlot - Error"),
				tr("This feature is not available for user defined function curves!"));
	return;
	}

Table *errTable=table(errColumnName);
if (w->tableRows() != errTable->tableRows())
	{
	QMessageBox::critical(this,tr("QtiPlot - Error"),
				tr("The selected columns have different numbers of rows!"));

	addErrorBars();
	return;
	}

int errCol=errTable->colIndex(errColumnName);
if (errTable->isEmptyColumn(errCol))
	{
	QMessageBox::critical(this,tr("QtiPlot - Error"),
				tr("The selected error column is empty!"));
	addErrorBars();
	return;
	}

activeGraph->addErrorBars(w, curveName, errTable, errColumnName,
						  direction, 2, 5, QColor(black), false, true, true);
	
emit modified();
}

void ApplicationWindow::removeCurves(const QString& name)
{
QApplication::setOverrideCursor(waitCursor);

QWidgetList windows = ws->windowList();
int c=windows.count();
for (int i=0; i<c; i++)
	{
	if (plotWindows.contains(windows.at(i)->name())>=1)
		{
		MultiLayer* plot = (MultiLayer*)windows.at(i);
		QWidgetList *graphsList=plot->graphPtrs();
		for (int j=0; j<(int)graphsList->count(); j++)
			{
			Graph* g=(Graph*)graphsList->at(j);				
			QStringList associations=g->plotAssociations();		
			for (int k=0; k<int(associations.count()); k++)
				{
				QString ass = associations[k];
				if (ass.contains(name))
					g->removeCurve(ass);
				}			
			}
		}
	 else if (plot3DWindows.contains(windows.at(i)->name()))
		{
		Graph3D* g = (Graph3D*)windows.at(i);
		if ((g->formula()).contains(name))
			g->clearData();
		}
	}

QApplication::restoreOverrideCursor();
}

void ApplicationWindow::updateCurves(const QString& name)
{
QApplication::setOverrideCursor(waitCursor);
QWidgetList windows = ws->windowList();
int c=(int)windows.count();
Table *W=table(name);
if (!W)
	return;

for (int i=0; i<c; i++)
	{
	QString caption=windows.at(i)->name();
	if (plotWindows.contains(caption))
		{
		MultiLayer* plot = (MultiLayer*)windows.at(i);
		QWidgetList *graphsList=plot->graphPtrs();
		for (int k=0; k<(int)graphsList->count(); k++)
			{
			Graph* g=(Graph*)graphsList->at(k);
			if (g && g->curves() > 0)
				{
				QStringList as=g->plotAssociations();
				for (int j=0; j<g->curves(); j++)
					{
					if (as[j].contains(name, TRUE))
						g->updateCurveData(W, name, j);
					}
				 g->updatePlot();
				 }
			}
		}
	else if (plot3DWindows.contains(caption))
		{
		Graph3D* g = (Graph3D*)windows.at(i);
		if ((g->formula()).contains(name,TRUE))
			g->updateData(W);
		}
	}
QApplication::restoreOverrideCursor();
}

void ApplicationWindow::showPreferencesDialog()
{
configDialog* cd= new configDialog(this,"configDialog",TRUE,WStyle_Tool|WDestructiveClose);
cd->setColumnSeparator(separator);
cd->initCurvesOptions(defaultCurveStyle, defaultCurveLineWidth, defaultSymbolSize);	
cd->showNormal();
cd->setActiveWindow();
}

void ApplicationWindow::setSaveSettings(bool autoSaving, int min)
{
if (autoSave==autoSaving && autoSaveTime==min)
	return;

autoSave=autoSaving;
autoSaveTime=min;	
	
killTimer(savingTimerId);

if (autoSave)
	savingTimerId=startTimer(autoSaveTime*60000);
else
	savingTimerId=0;
}

void ApplicationWindow::changeAppStyle(const QString& s)
{
if (appStyle == s)
	return;

if (QStyle *style = QStyleFactory::create(s))
    {			
	qApp->setStyle(style);
    appStyle = s;

	QPalette pal = qApp->palette();
	pal.setColor (QPalette::Active, QColorGroup::Base, QColor(panelsColor));
	qApp->setPalette(pal, true, 0);
    }
}

void ApplicationWindow::changeAppFont(const QFont& f)
{
if (appFont == f)
	return;

appFont=f;
updateAppFonts();
}

void ApplicationWindow::updateAppFonts()
{
qApp->setFont (appFont);
this->setFont(appFont);
windowsMenu->setFont(appFont);
view->setFont(appFont);
graph->setFont(appFont);
file->setFont(appFont);
format->setFont(appFont);
calcul->setFont(appFont);
edit->setFont(appFont);
dataMenu->setFont(appFont);
recent->setFont(appFont);
help->setFont(appFont);
type->setFont(appFont);
import->setFont(appFont);
plot2D->setFont(appFont);
plot3D->setFont(appFont);
plot3DMenu->setFont(appFont);
matrixMenu->setFont(appFont);
specialPlot->setFont(appFont);
panels->setFont(appFont);
stat->setFont(appFont);
smooth->setFont(appFont);
filter->setFont(appFont);
decay->setFont(appFont);
plotDataMenu->setFont(appFont);
tablesDepend->setFont(appFont);
tableMenu->setFont(appFont);
exportPlot->setFont(appFont);
normMenu->setFont(appFont);
translateMenu->setFont(appFont);
fillMenu->setFont(appFont);
setAsMenu->setFont(appFont);
multiPeakMenu->setFont(appFont);
info->setFont(QFont(appFont.family(),2+appFont.pointSize(),QFont::Bold,FALSE));
}

void ApplicationWindow::updateConfirmOptions(bool askTables, bool askMatrixes, bool askPlots2D,
											 bool askPlots3D, bool askNotes)
{
QWidgetList *windows = windowsList();
if (confirmCloseTable != askTables)
	{
	confirmCloseTable=askTables;
	for (int i = 0; i < int(windows->count());i++ )
		{
		if (windows->at(i)->isA("Table"))
			((myWidget*)windows->at(i))->askOnCloseEvent(confirmCloseTable);
		}
	}

if (confirmCloseMatrix != askMatrixes)
	{
	confirmCloseMatrix = askMatrixes;
	for (int i = 0; i < int(windows->count());i++ )
		{
		if (windows->at(i)->isA("Matrix"))
			((myWidget*)windows->at(i))->askOnCloseEvent(confirmCloseMatrix);
		}
	}

if (confirmClosePlot2D != askPlots2D)
	{
	confirmClosePlot2D=askPlots2D;
	for (int i = 0; i < int(windows->count());i++ )
		{
		if (windows->at(i)->isA("MultiLayer"))
			((myWidget*)windows->at(i))->askOnCloseEvent(confirmClosePlot2D);
		}
	}

if (confirmClosePlot3D != askPlots3D)
	{
	confirmClosePlot3D=askPlots3D;
	for (int i = 0; i < int(windows->count());i++ )
		{
		if (windows->at(i)->isA("Graph3D"))
			((myWidget*)windows->at(i))->askOnCloseEvent(confirmClosePlot3D);
		}
	}

if (confirmCloseNotes != askNotes)
	{
	confirmCloseNotes = askNotes;
	for (int i = 0; i < int(windows->count());i++ )
		{
		if (windows->at(i)->isA("Note"))
			((myWidget*)windows->at(i))->askOnCloseEvent(confirmCloseNotes);
		}
	}

delete windows;
}

void ApplicationWindow::setGraphDefaultSettings(bool autoscale,bool scaleFonts,bool resizeLayers)
{	
if (autoscale2DPlots == autoscale && 
	autoScaleFonts == scaleFonts &&
	autoResizeLayers != resizeLayers)
	return;

autoscale2DPlots = autoscale; 
autoScaleFonts = scaleFonts;
autoResizeLayers = !resizeLayers;

QWidgetList windows = ws->windowList();
for (int i=0; i<(int)windows.count(); i++)
	{
	if (plotWindows.contains(windows.at(i)->name()))
		{
		MultiLayer* plot = (MultiLayer*)windows.at(i);
		QWidgetList *graphsList=plot->graphPtrs();
		for (int k=0; k<(int)graphsList->count(); k++)
			{
			Graph* g=(Graph*)graphsList->at(k);
			if (g)
				{
				g->enableAutoscaling(autoscale2DPlots);
				g->updateScale();
				g->setIgnoreResizeEvents(!autoResizeLayers);
				g->setAutoscaleFonts(autoScaleFonts);
				}
			}
		}
	}
}

ApplicationWindow * ApplicationWindow::plotFile(const QString& fn)
{	
QApplication::setOverrideCursor(waitCursor);
ApplicationWindow *app= new ApplicationWindow();
app->applyUserSettings();
app->showMaximized();

Table* t = app->newTable(fn, app->separator, 0, true, app->strip_spaces, app->simplify_spaces);
t->setCaptionPolicy(myWidget::Both);	
app->multilayerPlot(t, t->YColumns(),Graph::LineSymbols);
QApplication::restoreOverrideCursor();
return 0;
}

void ApplicationWindow::showImportDialog()
{
importDialog* id= new importDialog(this,"importDialog",false, WStyle_ContextHelp|WDestructiveClose);
connect (id, SIGNAL(options(const QString&, int, bool, bool, bool)),
			this, SLOT(setImportOptions(const QString&, int, bool, bool, bool)));
	
id->setSeparator(separator);
id->setLines(ignoredLines);
id->renameCols(renameColumns);
id->setWhiteSpaceOptions(strip_spaces, simplify_spaces);
id->showNormal();
id->setActiveWindow();
}

void ApplicationWindow::setImportOptions(const QString& sep, int lines, bool rename,
										 bool strip, bool simplify)
{
separator = sep;
ignoredLines = lines;
renameColumns = rename;
strip_spaces = strip;
simplify_spaces = simplify;
}

void ApplicationWindow::loadASCII()
{
QString filter="All files *;;Text (*.TXT *.txt);;Data (*DAT *.dat);;";
QString fn = QFileDialog::getOpenFileName(workingDir, filter, this, 0,
			"QtiPlot - Import ASCII File", 0, TRUE);
if (!fn.isEmpty())
	{
	Table* t = (Table*)ws->activeWindow();
	if ( t && tableWindows.contains(t->name()))
		{
		t->importASCII(fn, separator, ignoredLines, renameColumns, 
					  strip_spaces, simplify_spaces, false);
		t->setWindowLabel(fn);
		}
	else
		t = newTable(fn, separator, ignoredLines, renameColumns, 
					 strip_spaces, simplify_spaces);

	t->setCaptionPolicy(myWidget::Both);
	setListViewLabel(t->name(), fn);
	QFileInfo fi(fn);
	workingDir = fi.dirPath(true);
	}
}

void ApplicationWindow::loadMultiple()
{
Table* t = (Table*)ws->activeWindow();
if ( t && tableWindows.contains(t->name()))
	{
	ImportFilesDialog *fd = new ImportFilesDialog(true, this, 0);
	fd->setDir(workingDir);
	if ( fd->exec() == QDialog::Accepted )
		{
		workingDir = fd->dirPath();
		loadMultipleASCIIFiles(fd->selectedFiles(), fd->importFileAs());
		}
	}
else
	{
	ImportFilesDialog *fd = new ImportFilesDialog(false, this, 0);
	fd->setDir(workingDir);
	if ( fd->exec() == QDialog::Accepted )
		{
		workingDir = fd->dirPath();
		loadMultipleASCIIFiles(fd->selectedFiles(), 0);
		}
	}
}

void ApplicationWindow::loadMultipleASCIIFiles(const QStringList& fileNames, int importFileAs)
{
int files = fileNames.count();
if (!files)
	return;

if (!importFileAs)
	{
	QString fn  = fileNames[0];
	Table *firstTable=newTable(fn, separator, ignoredLines, renameColumns, 
							   strip_spaces, simplify_spaces);
	if (!firstTable)
		return;

	firstTable->setCaptionPolicy(myWidget::Both);
	setListViewLabel(firstTable->name(), fn);

	int dx=firstTable->verticalHeaderWidth();
	int dy=firstTable->parentWidget()->frameGeometry().height() - firstTable->height();
	firstTable->parentWidget()->move(QPoint(0,0));

	for (int i=1;i<files;i++)
		{
		fn  = fileNames[i];
		Table *w = newTable(fn, separator, ignoredLines, renameColumns, 
							strip_spaces, simplify_spaces);
		if (w)
			{
			w->setCaptionPolicy(myWidget::Both);
			setListViewLabel(w->name(), fn);
			w->parentWidget()->move(QPoint(i*dx,i*dy));
			}
		}

	emit modifiedProject();
	}
else
	{
	Table* t = (Table*)ws->activeWindow();
	if ( t && tableWindows.contains(t->name()))
		{
		for (int i=0; i<files; i++)
			t->importMultipleASCIIFiles(fileNames[i], separator, ignoredLines, renameColumns, 
										strip_spaces, simplify_spaces, importFileAs);
		t->setWindowLabel(fileNames.join("; "));
		t->setCaptionPolicy(myWidget::Name);
		emit modifiedProject(t);
		}
	}
}

ApplicationWindow* ApplicationWindow::open(const QString& fn)
{
if (fn.contains(".opj", false))
	return importOPJ(fn);
else if (!fn.contains(".qti"))
	return plotFile(fn);

QString fname = fn;
if (fn.contains(".qti.gz"))
	{//decompress using zlib
	file_uncompress((char *)fname.ascii());
	fname.remove(".gz");
	}
	
QFile f(fname);
QTextStream t( &f );
f.open(IO_ReadOnly);
QString s = t.readLine();
QStringList list=QStringList::split (QRegExp("\\s"),s,false);

QString fileType=list[0], version=list[1];
if (fileType != "QtiPlot")
	{
   	f.close();
	if (QFile::exists(fname+"~"))
      {
	   int choice = QMessageBox::question(this, tr("QtiPlot - File opening error"),
		tr("The file <b>%1</b> is corrupted, but there exists a backup copy.<br>Do you want to open the backup instead?").arg(fn),
		QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape);
	    if (choice==QMessageBox::Yes)
	      return open(fname+"~");
	  } 
	  else
	    QMessageBox::critical(this,tr("QtiPlot - File opening error"),  tr("The file: <b> %1 </b> was not created using QtiPlot!").arg(fn));
 	return 0;
	}

QStringList vl = QStringList::split (".", version, false);
fileVersion =100*(vl[0]).toInt()+10*(vl[1]).toInt()+(vl[2]).toInt();

ApplicationWindow* app = openProject(fname);

f.close();
return app;
}

ApplicationWindow* ApplicationWindow::openProject(const QString& fn)
{
ApplicationWindow *app = new ApplicationWindow();
app->applyUserSettings();
app->projectname = fn;
app->setCaption(tr("QtiPlot") + " - " + fn);
app->showMaximized();

QFile f(fn);
QTextStream t( &f );
t.setEncoding(QTextStream::UnicodeUTF8);
f.open(IO_ReadOnly);

QFileInfo fi(fn);
QString baseName = fi.fileName();

t.readLine(); 
if (fileVersion < 73)
	t.readLine();

QString s = t.readLine();
QStringList list=QStringList::split("\t",s,FALSE);
int aux=0,widgets=list[1].toInt();

QString titleBase = "Window: ";
QString title = titleBase + "1/"+QString::number(widgets)+"  ";

QProgressDialog progress(0, "progress", true, WStyle_StaysOnTop|WStyle_Tool);
progress.setMinimumWidth(app->width()/2);
progress.setCaption(tr("QtiPlot - Opening file") + ": " + baseName);
progress.setLabelText(title);
progress.setTotalSteps(widgets);
progress.setActiveWindow();
progress.setMinimumDuration(10000);

Folder *cf = app->projectFolder();
app->folders->blockSignals (true);
app->blockSignals (true);
//rename project folder item
FolderListItem *item = (FolderListItem *)app->folders->firstChild();
item->setText(0, fi.baseName());
item->folder()->setFolderName(fi.baseName());

//process tables and matrix information
while ( !t.eof() && !progress.wasCanceled())
	{
	s = t.readLine();
	list.clear();
	if  (s.left(8) == "<folder>")
		{
		list = QStringList::split ("\t",s,TRUE);
		Folder *f = new Folder(app->current_folder, list[1]);
		f->setBirthDate(list[2]);
		f->setModificationDate(list[3]);
		if (list[4] == "current")
			cf = f;

		FolderListItem *fli = new FolderListItem(app->current_folder->folderListItem(), f);
		fli->setText(0, list[1]);
		f->setFolderListItem(fli);
	
		app->current_folder = f;
		}
	else if  (s == "<table>")
		{
		title = titleBase + QString::number(++aux)+"/"+QString::number(widgets);
		progress.setLabelText(title);
		if (fileVersion < 69)
			{
			while ( s!="</table>" )
				{
				s=t.readLine();
				list<<s;
				}
			 openTable(app,list);
			}
		else
			{
			while ( s != "<data>" )
				{
				s=t.readLine();
				list<<s;
				}
			Table *w = openTable(app,list);
			int cols = w->tableCols();				
			s = t.readLine();
			while ( s != "</data>" )
				{
				w->addDataRow(s, cols);
				s = t.readLine();
				}				
			}
		progress.setProgress(aux);
		}
	else if  (s == "<matrix>")
		{
		title= titleBase + QString::number(++aux)+"/"+QString::number(widgets);
		progress.setLabelText(title);
		while ( s != "<data>" )
			{
			s=t.readLine();
			list<<s;
			}
		Matrix *w = openMatrix(app,list);
		int cols = w->numCols();				
		s = t.readLine();
		while ( s != "</data>" )
			{
			w->addDataRow(s, cols);
			s = t.readLine();
			}
		progress.setProgress(aux);
		}
	else if  (s == "<note>")
		{
		title= titleBase + QString::number(++aux)+"/"+QString::number(widgets);
		progress.setLabelText(title);
		for (int i=0; i<3; i++)
			{
			s = t.readLine();
			list << s;
			}
		Note* m = openNote(app,list);
		QString text = QString::null;
		while ( s != "</note>" )
			{
			s=t.readLine();
			text += s+"\n";
			}
		m->setText(text.remove("</note>\n"));
		progress.setProgress(aux);
		}
	else if  (s == "</folder>")
		{
		Folder *parent = (Folder *)app->current_folder->parent();
		if (!parent)
			app->current_folder = projectFolder();
		else
			app->current_folder = parent;
		}
	}
f.close();

if (progress.wasCanceled())
	{
	app->saved = true;
	app->close();
	return 0;
	}

//process the rest
f.open(IO_ReadOnly);

MultiLayer *plot=0;
while ( !t.eof() && !progress.wasCanceled())
	{
	s=t.readLine();
	if  (s.left(8) == "<folder>")
		{
		list = QStringList::split ("\t",s,TRUE);
		app->current_folder = app->current_folder->findSubfolder(list[1]);
		}
	else if  (s == "<multiLayer>")
		{//process multilayers information
		title = titleBase + QString::number(++aux)+"/"+QString::number(widgets);
		progress.setLabelText(title);

		s=t.readLine();
		QStringList graph=QStringList::split ("\t",s,TRUE);
		QString caption=graph[0];
		plot=app->multilayerPlot(caption);
		plot->setCols(graph[1].toInt());
		plot->setRows(graph[2].toInt());
		QString date=QString::null;
		if (fileVersion < 63)
			date = graph[5];
		else
			date = graph[3];

		app->setListViewDate(caption,date);
		plot->setBirthDate(date);
		plot->blockSignals(true);	

		restoreWindowGeometry(app, plot, t.readLine());
		
		if (fileVersion > 71)
			{
			QStringList lst=QStringList::split ("\t", t.readLine(), true);
			plot->setWindowLabel(lst[1]);
			app->setListViewLabel(plot->name(),lst[1]);
			plot->setCaptionPolicy((myWidget::CaptionPolicy)lst[2].toInt());
			}

		if (caption.contains ("graph",TRUE))
			{
			bool ok;
			int gr=caption.remove("graph").toInt(&ok);
			if (gr > app->graphs && ok) 
				app->graphs = gr;
			}

		if (fileVersion > 83)
			{
			QStringList lst=QStringList::split ("\t", t.readLine(), false);
			plot->setMargins(lst[1].toInt(),lst[2].toInt(),lst[3].toInt(),lst[4].toInt());
			lst=QStringList::split ("\t", t.readLine(), false);
			plot->setSpacing(lst[1].toInt(),lst[2].toInt());
			lst=QStringList::split ("\t", t.readLine(), false);
			plot->setLayerCanvasSize(lst[1].toInt(),lst[2].toInt());
			lst=QStringList::split ("\t", t.readLine(), false);
			plot->setAlignement(lst[1].toInt(),lst[2].toInt());
			}

		while ( s!="</multiLayer>" )
			{//open layers
			s=t.readLine();
			if (s.left(7)=="<graph>")
				{
				list.clear();
				while ( s!="</graph>" )
					{
					s=t.readLine();
					list<<s;
					}
				openGraph(app,plot,list);
				}
			}
		plot->blockSignals(false);
		progress.setProgress(aux);
		}
	else if  (s == "<SurfacePlot>")
		{//process 3D plots information
		list.clear();
		title = titleBase + QString::number(++aux)+"/"+QString::number(widgets);
		progress.setLabelText(title);
		while ( s!="</SurfacePlot>" )
			{
			s=t.readLine();
			list<<s;
			}
		openSurfacePlot(app,list);
		progress.setProgress(aux);
		}
	else if  (s == "</folder>")
		{
		Folder *parent = (Folder *)app->current_folder->parent();
		if (!parent)
			app->current_folder = projectFolder();
		else
			app->current_folder = parent;
		}
	else if  (s.left(5)=="<log>")
		{//process analysis information
		s = t.readLine();
		while ( s != "</log>" )
			{
			app->logInfo+= s+"\n";
			s = t.readLine();
			}
		app->results->setText(app->logInfo);
		}
	}
f.close();

if (progress.wasCanceled())
	{
	app->saved = true;
	app->close();
	return 0;
	}
			
app->logInfo=app->logInfo.remove ("</log>\n", FALSE);
			
QFileInfo fi2(f);
QString fileName = fi2.absFilePath();

app->recentProjects.remove(fileName);
app->recentProjects.push_front(fileName);
app->updateRecentProjectsList();
	
if (app->aw)
	{
	app->aw->setFocus();
	app->customMenu(app->aw);
	app->customToolBars(app->aw);
	}

app->saved=TRUE;
app->actionSaveProject->setEnabled(false);

if (app->show_windows_policy == HideAll)
	app->hideFolderWindows(app->projectFolder());

app->folders->setCurrentItem(cf->folderListItem());
app->folders->blockSignals (false);
//change folder to user defined current folder
app->changeFolder(cf);
app->blockSignals (false);
app->renamedTables.clear();
return app;
}

void ApplicationWindow::openTemplate()
{
QString filter = "QtiPlot 2D Plot Template (*.qpt);;";
filter += "QtiPlot 3D Surface Template (*.qst);;";
filter += "QtiPlot Table Template (*.qtt);;";
filter += "QtiPlot Matrix Template (*.qmt);;";

QString fn = QFileDialog::getOpenFileName(workingDir, filter, this, 0,
			tr("QtiPlot - Open Template File"), 0, TRUE);
if (!fn.isEmpty())
	{
	QFileInfo fi(fn);
	workingDir = fi.dirPath(true); 		
	if (fn.contains(".qmt",TRUE) || fn.contains(".qpt",true) ||
		fn.contains(".qtt",true) || fn.contains(".qst",true))
		{
		if (!fi.exists())
			{
			QMessageBox::critical(this, tr("QtiPlot - File openning error"),
				tr("The file: <b>%1</b> doesn't exist!").arg(fn));
			return;
			}
		QFile f(fn);
		QTextStream t(&f);
		t.setEncoding(QTextStream::UnicodeUTF8);
		f.open(IO_ReadOnly);
		QStringList l=QStringList::split(QRegExp("\\s"), t.readLine(), false);
		QString fileType=l[0];
		if (fileType != "QtiPlot")
			{
			QMessageBox::critical(this,tr("QtiPlot - File openning error"),
				tr("The file: <b> %1 </b> was not created using QtiPlot!").arg(fn));
			return;
			}
		QStringList vl = QStringList::split (".", l[1], false);
		fileVersion = 100*(vl[0]).toInt()+10*(vl[1]).toInt()+(vl[2]).toInt();

		QApplication::setOverrideCursor(waitCursor);
		myWidget *w = 0;
		QString templateType;
		t>>templateType;

		if (templateType == "<SurfacePlot>")
			{
			t.skipWhiteSpace();
			QStringList lst;
			while (!t.atEnd())
				lst << t.readLine();
			w = openSurfacePlot(this,lst);
			if (w)
				((Graph3D *)w)->clearData();
			}
		else
			{
			int rows, cols;
			t>>rows; t>>cols; 
			t.skipWhiteSpace();
			QString geometry = t.readLine();
				
			if (templateType == "<multiLayer>")
				{
				w = multilayerPlot(tr("graph1"));
				if (w)
					{
					((MultiLayer*)w)->setCols(cols);
					((MultiLayer*)w)->setRows(rows);
					restoreWindowGeometry(this, (QWidget *)w, geometry);
					if (fileVersion > 83)
						{
						QStringList lst=QStringList::split ("\t", t.readLine(), false);
						((MultiLayer*)w)->setMargins(lst[1].toInt(),lst[2].toInt(),lst[3].toInt(),lst[4].toInt());
						lst=QStringList::split ("\t", t.readLine(), false);
						((MultiLayer*)w)->setSpacing(lst[1].toInt(),lst[2].toInt());
						lst=QStringList::split ("\t", t.readLine(), false);
						((MultiLayer*)w)->setLayerCanvasSize(lst[1].toInt(),lst[2].toInt());
						lst=QStringList::split ("\t", t.readLine(), false);
						((MultiLayer*)w)->setAlignement(lst[1].toInt(),lst[2].toInt());
						}
					while (!t.atEnd())
						{//open layers
						QString s=t.readLine();
						if (s.left(7)=="<graph>")
							{
							QStringList lst;
							while ( s!="</graph>" )
								{
								s = t.readLine();
								lst << s;
								}
							openGraph(this, (MultiLayer*)w, lst);
							}
						}
					}
				}
			else 
				{
				if (templateType == "<table>")
					w = newTable(tr("table1"), rows, cols);
				else if (templateType == "<matrix>")
					w = newMatrix(tr("Matrix1"), rows, cols);
				if (w)
					{
					QStringList lst;
					while (!t.atEnd())
						lst << t.readLine();
					w->restore(lst);
					restoreWindowGeometry(this, (QWidget *)w, geometry);
					}
				}
			}

		f.close();
		if (w)
			{
			customMenu((QWidget*)w);
			customToolBars((QWidget*)w);
			}
		QApplication::restoreOverrideCursor();
		}
	else
		{
		QMessageBox::critical(this,tr("QtiPlot - File openning error"),
				tr("The file: <b>%1</b> is not a QtiPlot template file!").arg(fn));
		return;
		}
	}
}

void ApplicationWindow::updatePlotsTransparency()
{
QApplication::setOverrideCursor(waitCursor);
QWidgetList windows = ws->windowList();
int c=(int)windows.count();
for (int i=0; i<c; i++)
	{
	QString caption=windows.at(i)->name();
	if (plotWindows.contains(caption))
		{
		MultiLayer* plot = (MultiLayer*)windows.at(i);
		if (plot->hasOverlapingLayers())
			plot->updateTransparency();
		}
	}
QApplication::restoreOverrideCursor();
}

void ApplicationWindow::open()
{
QString filter = tr("QtiPlot project") + " (*.qti);;";
filter += tr("Compressed QtiPlot project") + " (*.qti.gz);;";
filter += tr("Origin project") + " (*.opj);;";
filter += tr("All files") + " (*);;";

QString fn = QFileDialog::getOpenFileName(workingDir, filter, this, 0,
			tr("QtiPlot - Open Project"), 0, TRUE);
if (!fn.isEmpty())
	{
	QFileInfo fi(fn);
	workingDir = fi.dirPath(true);

	if (projectname != "untitled")
		{
		QFileInfo fi(projectname);
		QString pn = fi.absFilePath();
		if (fn == pn)
			{
			QMessageBox::warning(this,tr("QtiPlot - File openning error"),
				tr("The file: <b>%1</b> is the current file!").arg(fn));
			return;
			}
		}
		
	if (fn.contains(".qti",TRUE) || fn.contains(".opj",false))
		{
		QFileInfo f(fn);
		if (!f.exists ())
			{
			QMessageBox::critical(this, tr("QtiPlot - File openning error"),
				tr("The file: <b>%1</b> doesn't exist!").arg(fn));
			return;
			}

		saveSettings();
		ApplicationWindow *a = open (fn);
		if (a)
			{
			a->updatePlotsTransparency();
			a->workingDir = workingDir;
			this->close();
			}
		}
	else
		{
		QMessageBox::critical(this,tr("QtiPlot - File openning error"),
				tr("The file: <b>%1</b> is not a QtiPlot or Origin project file!").arg(fn));
		return;
		}
	}
}

void ApplicationWindow::openRecentProject(int index)
{
QString fn = recent->text(index);
int pos = fn.find(" ",0);
fn=fn.right(fn.length()-pos-1);

QFile f(fn);
if (!f.exists())
	{
	QMessageBox::critical(this,tr("QtiPlot - File Open Error"),
			   tr("The file: <b> %1 </b> <p>does not exist anymore!"
			   "<p>It will be removed from the list.").arg(fn));

	recentProjects.remove(fn);
	updateRecentProjectsList();
	return;
	}

if (projectname != "untitled")
	{
	QFileInfo fi(projectname);
	QString pn = fi.absFilePath();
	if (fn == pn)
		{
		QMessageBox::warning(this, tr("QtiPlot - File openning error"),
				tr("The file: <b> %1 </b> is the current file!").arg(fn));
		return;
		}
	}

if ( !fn.isEmpty())
	{
	saveSettings();
	ApplicationWindow * a = open (fn);
	if (a)
		{
		a->updatePlotsTransparency();
		this->close();
		}
	}
}

void ApplicationWindow::readSettings()
{
helpFilePath="/usr/share/doc/qtiplot/index.html";
#ifdef Q_OS_WIN // Windows systems
	helpFilePath=qApp->applicationDirPath()+"/index.html";
#endif
	
QSettings settings;
#ifdef Q_OS_MAC // Mac 
	settings = QSettings(QSettings::Ini);
	settings.setPath("Ion Vasilief", "QtiPlot", QSettings::User);
#else
	settings.setPath("Ion Vasilief", "QtiPlot");
#endif

settings.beginGroup("/QtiPlot");
askForSupport = settings.readBoolEntry ("/askForSupport", true, 0);
appLanguage = settings.readEntry("/appLanguage", "en");
workingDir=settings.readEntry("/workingDir", qApp->applicationDirPath());
helpFilePath=settings.readEntry("/helpFilePath", helpFilePath);
show_windows_policy = (ShowWindowsPolicy)settings.readNumEntry("/ShowWindowsPolicy", ActiveFolder);

recentProjects=settings.readListEntry("/recentProjects");
updateRecentProjectsList();

functions=settings.readListEntry("/functions");
fitFunctions=settings.readListEntry("/fitFunctions");
surfaceFunc=settings.readListEntry("/surfaceFunctions");
xFunctions=settings.readListEntry("/xFunctions");
yFunctions=settings.readListEntry("/yFunctions");
rFunctions=settings.readListEntry("/rFunctions");
tetaFunctions=settings.readListEntry("/tetaFunctions");

separator=settings.readEntry("/defaultColumnSeparator", "\t");
QStringList tableColors=settings.readListEntry("/tableColors");
QStringList tableFonts=settings.readListEntry("/tableFonts");

//2D plots settings
titleOn=settings.readBoolEntry ("/titleOn", true, 0);
allAxesOn=settings.readBoolEntry ("/allAxesOn", false, 0);
canvasFrameOn=settings.readBoolEntry ("/canvasFrameOn", false, 0);
canvasFrameWidth=settings.readNumEntry ("/canvasFrameWidth", 0, 0);
defaultPlotMargin=settings.readNumEntry ("/defaultPlotMargin", 0, 0);
drawBackbones=settings.readBoolEntry ("/drawBackbones", true, 0);
axesLineWidth=settings.readNumEntry ("/axesLineWidth", 1, 0);
autoscale2DPlots = settings.readBoolEntry ("/autoscale2DPlots", true, 0);
autoScaleFonts = settings.readBoolEntry ("/autoScaleFonts", true, 0);
autoResizeLayers = settings.readBoolEntry ("/autoResizeLayers", true, 0);

//2D curves settings
defaultCurveStyle = settings.readNumEntry ("/defaultCurveStyle", Graph::LineSymbols, 0);
defaultCurveLineWidth = settings.readNumEntry("/defaultCurveLineWidth", 1, 0);
defaultSymbolSize = settings.readNumEntry("/defaultSymbolSize", 7, 0);

ticksStyle=settings.readNumEntry ("/ticksStyle", Plot::Out, 0);
minTicksLength=settings.readNumEntry ("/minTicksLength", 5, 0);
majTicksLength=settings.readNumEntry ("/majTicksLength", 9, 0);

legendFrameStyle=settings.readNumEntry ("/legendFrameStyle", LegendMarker::Line, 0);
QStringList graphFonts=settings.readListEntry("/graphFonts");
confirmCloseFolder=settings.readBoolEntry ("/confirmCloseFolder", true, 0);
confirmCloseTable=settings.readBoolEntry ("/confirmCloseTable", true, 0);
confirmCloseMatrix=settings.readBoolEntry ("/confirmCloseMatrix", true, 0);
confirmClosePlot2D=settings.readBoolEntry ("/confirmClosePlot2D", true, 0);
confirmClosePlot3D=settings.readBoolEntry ("/confirmClosePlot3D", true, 0);
confirmCloseNotes=settings.readBoolEntry ("/confirmCloseNotes", true, 0);

QStringList applicationFont=settings.readListEntry("/appFont");

//set user style
changeAppStyle(settings.readEntry("/appStyle", appStyle));

autoSave=settings.readBoolEntry("/autoSave",true);
autoSaveTime=settings.readNumEntry ("/autoSaveTime",15);
QStringList appColors=settings.readListEntry("/appColors");

//3D plots settings
showPlot3DLegend=settings.readBoolEntry("/showPlot3DLegend",true);
showPlot3DProjection=settings.readBoolEntry("/showPlot3DProjection", false);
smooth3DMesh = settings.readBoolEntry("/smooth3DMesh", true);
plot3DResolution=settings.readNumEntry ("/plot3DResolution", 1);

QStringList aux = settings.readListEntry("/plot3DColors");
QStringList plot3DFonts = settings.readListEntry("/plot3DFonts");

fitPluginsPath = settings.readEntry("/fitPluginsPath", "fitPlugins");
settings.endGroup();

if (aux.size() == 8)
	plot3DColors = aux;
else
	{
	plot3DColors << QColor(blue).name();
	plot3DColors << QColor(black).name() << QColor(black).name() << QColor(black).name();
	plot3DColors << QColor(red).name() << QColor(black).name() << QColor(black).name();
	plot3DColors << QColor(255, 255, 255).name();
	}

if (appColors.size() == 3)
	{
	workspaceColor=QColor(appColors[0]);
	panelsColor=QColor(appColors[1]);
	panelsTextColor=QColor(appColors[2]);
	}
else
	{
	workspaceColor=QColor(darkGray);
	panelsColor=QColor(255, 255, 255);
	panelsTextColor=QColor(black);
	}
	
if (tableColors.size () == 3)
	{
	tableBkgdColor=QColor(tableColors[0]);
	tableTextColor=QColor(tableColors[1]);
	tableHeaderColor=QColor(tableColors[2]);
	}
else
	{
	tableBkgdColor=QColor(255, 255, 255);
	tableTextColor=QColor(black);
	tableHeaderColor=QColor(black);
	}

if (tableFonts.size() == 8)
	{
	tableTextFont=QFont (tableFonts[0],tableFonts[1].toInt(),tableFonts[2].toInt(),tableFonts[3].toInt());
	tableHeaderFont=QFont (tableFonts[4],tableFonts[5].toInt(),tableFonts[6].toInt(),tableFonts[7].toInt());
	}

if (graphFonts.size() == 16)
	{
	plotAxesFont=QFont (graphFonts[0],graphFonts[1].toInt(),graphFonts[2].toInt(),graphFonts[3].toInt());
	plotNumbersFont=QFont (graphFonts[4],graphFonts[5].toInt(),graphFonts[6].toInt(),graphFonts[7].toInt());
	plotLegendFont=QFont (graphFonts[8],graphFonts[9].toInt(),graphFonts[10].toInt(),graphFonts[11].toInt());
	plotTitleFont=QFont (graphFonts[12],graphFonts[13].toInt(),graphFonts[14].toInt(),graphFonts[15].toInt());
	}

if (plot3DFonts.size() == 12)
	{
	plot3DTitleFont=QFont (plot3DFonts[0],plot3DFonts[1].toInt(),plot3DFonts[2].toInt(),plot3DFonts[3].toInt());
	plot3DNumbersFont=QFont (plot3DFonts[4],plot3DFonts[5].toInt(),plot3DFonts[6].toInt(),plot3DFonts[7].toInt());
	plot3DAxesFont=QFont (plot3DFonts[8],plot3DFonts[9].toInt(),plot3DFonts[10].toInt(),plot3DFonts[11].toInt());
	}

if (applicationFont.size() == 4)	
	appFont=QFont (applicationFont[0],applicationFont[1].toInt(),applicationFont[2].toInt(),applicationFont[3].toInt());

bool ok = true;
settings.beginGroup("/ProjectExplorer");
int edock = settings.readNumEntry("/dock", (int)Qt::DockBottom, &ok);
int index = settings.readNumEntry("/index", 0, &ok);
bool newLine = settings.readBoolEntry("/newLine", true, &ok);
int offset = settings.readNumEntry("/offset", 0, &ok);
int x = settings.readNumEntry("/x", 0, &ok);
int y = settings.readNumEntry("/y", 0, &ok);
int ewidth = settings.readNumEntry("/width", 0, &ok);
int eheight = settings.readNumEntry("/height", 0, &ok);
bool visible = settings.readBoolEntry("/visible", false, &ok);
settings.endGroup();

if (ok)
	{
	if (edock == Qt::DockTornOff)
		{
		moveDockWindow(explorerWindow, (Qt::Dock)edock);
		explorerWindow->setGeometry(QRect(x, y, ewidth, eheight));
		}
	else
		moveDockWindow(explorerWindow, (Qt::Dock)edock, newLine, index, offset);

	if (visible)
		{
		actionShowExplorer->setOn(true);
		explorerWindow->show();
		}
	}

settings.beginGroup("/ResultsLog");
int rdock = settings.readNumEntry("/dock", (int)Qt::DockBottom, &ok);
index = settings.readNumEntry("/index", 0, &ok);
newLine = settings.readBoolEntry("/newLine", true, &ok);
offset = settings.readNumEntry("/offset", 0, &ok);
x = settings.readNumEntry("/x", 0, &ok);
y = settings.readNumEntry("/y", 0, &ok);
int rwidth = settings.readNumEntry("/width", 0, &ok);
int rheight = settings.readNumEntry("/height", 0, &ok);
visible = settings.readBoolEntry("/visible", false, &ok);
settings.endGroup();

if (ok)
	{
	if (rdock == Qt::DockTornOff)
		{
		moveDockWindow(logWindow, (Qt::Dock)rdock);
		logWindow->setGeometry(QRect(x, y, rwidth, rheight));
		}
	else
		{
		moveDockWindow(logWindow, (Qt::Dock)rdock, newLine, index, offset);
		logWindow->setFixedExtentWidth(rwidth);
		logWindow->setFixedExtentHeight(rheight);
		}

	explorerWindow->setFixedExtentWidth(ewidth);
	explorerWindow->setFixedExtentHeight(eheight);

	showResults(visible);
	actionShowLog->setOn(visible);
	}
}

void ApplicationWindow::saveSettings()
{
QStringList tableColors, appColors;
tableColors<<tableBkgdColor.name()<<tableTextColor.name()<<tableHeaderColor.name();
appColors<<workspaceColor.name()<<panelsColor.name()<<panelsTextColor.name();

QStringList tableFonts;
tableFonts<<tableTextFont.family();
tableFonts<<QString::number(tableTextFont.pointSize());
tableFonts<<QString::number(tableTextFont.weight());
tableFonts<<QString::number(tableTextFont.italic());
tableFonts<<tableHeaderFont.family();
tableFonts<<QString::number(tableHeaderFont.pointSize());
tableFonts<<QString::number(tableHeaderFont.weight());
tableFonts<<QString::number(tableHeaderFont.italic());

QStringList graphFonts;
graphFonts<<plotAxesFont.family();
graphFonts<<QString::number(plotAxesFont.pointSize());
graphFonts<<QString::number(plotAxesFont.weight());
graphFonts<<QString::number(plotAxesFont.italic());
graphFonts<<plotNumbersFont.family();
graphFonts<<QString::number(plotNumbersFont.pointSize());
graphFonts<<QString::number(plotNumbersFont.weight());
graphFonts<<QString::number(plotNumbersFont.italic());
graphFonts<<plotLegendFont.family();
graphFonts<<QString::number(plotLegendFont.pointSize());
graphFonts<<QString::number(plotLegendFont.weight());
graphFonts<<QString::number(plotLegendFont.italic());
graphFonts<<plotTitleFont.family();
graphFonts<<QString::number(plotTitleFont.pointSize());
graphFonts<<QString::number(plotTitleFont.weight());
graphFonts<<QString::number(plotTitleFont.italic());

QStringList applicationFont;
applicationFont<<appFont.family();
applicationFont<<QString::number(appFont.pointSize());
applicationFont<<QString::number(appFont.weight());
applicationFont<<QString::number(appFont.italic());

QStringList plot3DFonts;
plot3DFonts<<plot3DTitleFont.family();
plot3DFonts<<QString::number(plot3DTitleFont.pointSize());
plot3DFonts<<QString::number(plot3DTitleFont.weight());
plot3DFonts<<QString::number(plot3DTitleFont.italic());
plot3DFonts<<plot3DNumbersFont.family();
plot3DFonts<<QString::number(plot3DNumbersFont.pointSize());
plot3DFonts<<QString::number(plot3DNumbersFont.weight());
plot3DFonts<<QString::number(plot3DNumbersFont.italic());
plot3DFonts<<plot3DAxesFont.family();
plot3DFonts<<QString::number(plot3DAxesFont.pointSize());
plot3DFonts<<QString::number(plot3DAxesFont.weight());
plot3DFonts<<QString::number(plot3DAxesFont.italic());

QSettings settings;
#ifdef Q_OS_MAC // Mac 
	settings = QSettings(QSettings::Ini);
	settings.setPath("Ion Vasilief", "QtiPlot", QSettings::User);
#else
	settings.setPath("Ion Vasilief", "QtiPlot");
#endif

settings.beginGroup("/QtiPlot");
settings.writeEntry("/askForSupport", askForSupport);
settings.writeEntry("/appLanguage", appLanguage);
settings.writeEntry("/workingDir", workingDir);
settings.writeEntry("/helpFilePath", helpFilePath);
settings.writeEntry("/ShowWindowsPolicy", show_windows_policy);
settings.writeEntry("/recentProjects", recentProjects);
settings.writeEntry("/functions", functions);
settings.writeEntry("/fitFunctions", fitFunctions);
settings.writeEntry("/surfaceFunctions", surfaceFunc);
settings.writeEntry("/xFunctions", xFunctions);
settings.writeEntry("/yFunctions", yFunctions);
settings.writeEntry("/rFunctions", rFunctions);
settings.writeEntry("/tetaFunctions", tetaFunctions);
settings.writeEntry("/defaultColumnSeparator", separator);
settings.writeEntry("/tableColors", tableColors);
settings.writeEntry("/tableFonts", tableFonts);
settings.writeEntry("/titleOn", titleOn);
settings.writeEntry("/allAxesOn", allAxesOn);
settings.writeEntry("/canvasFrameOn", canvasFrameOn);
settings.writeEntry("/canvasFrameWidth", canvasFrameWidth);
settings.writeEntry("/defaultPlotMargin", defaultPlotMargin);
settings.writeEntry("/drawBackbones", drawBackbones);
settings.writeEntry("/axesLineWidth", axesLineWidth);
settings.writeEntry("/autoscale2DPlots", autoscale2DPlots);
settings.writeEntry("/autoScaleFonts", autoScaleFonts);
settings.writeEntry("/autoResizeLayers", autoResizeLayers);

settings.writeEntry("/defaultCurveStyle", defaultCurveStyle);
settings.writeEntry("/defaultCurveLineWidth", defaultCurveLineWidth);
settings.writeEntry("/defaultSymbolSize", defaultSymbolSize);

settings.writeEntry("/ticksStyle", ticksStyle);
settings.writeEntry("/minTicksLength", minTicksLength);
settings.writeEntry("/majTicksLength", majTicksLength);

settings.writeEntry("/legendFrameStyle", legendFrameStyle);
settings.writeEntry("/graphFonts", graphFonts);
settings.writeEntry("/confirmCloseFolder", confirmCloseFolder);
settings.writeEntry("/confirmCloseTable", confirmCloseTable);
settings.writeEntry("/confirmCloseMatrix", confirmCloseMatrix);
settings.writeEntry("/confirmClosePlot2D", confirmClosePlot2D);
settings.writeEntry("/confirmClosePlot3D", confirmClosePlot3D);
settings.writeEntry("/confirmCloseNotes", confirmCloseNotes);
settings.writeEntry("/appFont", applicationFont);
settings.writeEntry("/appStyle", appStyle);
settings.writeEntry("/autoSave", autoSave);
settings.writeEntry("/autoSaveTime", autoSaveTime);
settings.writeEntry("/appColors", appColors);

settings.writeEntry("/showPlot3DLegend", showPlot3DLegend);
settings.writeEntry("/showPlot3DProjection", showPlot3DProjection);
settings.writeEntry("/smooth3DMesh", smooth3DMesh);
settings.writeEntry("/plot3DResolution", plot3DResolution);
settings.writeEntry("/plot3DColors", plot3DColors);
settings.writeEntry("/plot3DFonts", plot3DFonts);
settings.writeEntry("/fitPluginsPath", fitPluginsPath);
settings.endGroup();

Qt::Dock dock;
int index, offset;
bool nl;

settings.beginGroup("/ProjectExplorer");
getLocation(explorerWindow, dock, index, nl, offset);
settings.writeEntry("/dock", (int)dock);
settings.writeEntry("/index", index);
settings.writeEntry("/newLine", nl);
settings.writeEntry("/offset", offset);
settings.writeEntry("/x", explorerWindow->x());
settings.writeEntry("/y", explorerWindow->y());
settings.writeEntry("/width", explorerWindow->width());
settings.writeEntry("/height", explorerWindow->height());
settings.writeEntry("/visible", explorerWindow->isVisible());
settings.endGroup();

settings.beginGroup("/ResultsLog");
getLocation(logWindow, dock, index, nl, offset);
settings.writeEntry("/dock", (int)dock);
settings.writeEntry("/index", index);
settings.writeEntry("/newLine", nl);
settings.writeEntry("/offset", offset);
settings.writeEntry("/x", logWindow->x());
settings.writeEntry("/y", logWindow->y());
settings.writeEntry("/width", logWindow->width());
settings.writeEntry("/height", logWindow->height());
settings.writeEntry("/visible", logWindow->isVisible());
settings.endGroup();
}

void ApplicationWindow::exportGraph()
{
QWidget *w=ws->activeWindow();
if (!w)
	return;

if(plotWindows.contains(w->name()))
	{
	MultiLayer *plot = (MultiLayer*)w;
	if (plot->isEmpty())
		{
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window!</h4>"));
		return;
		}
	
	ImageExportDialog *ied = new ImageExportDialog(this, 0);
	ied->setDir(workingDir);
	if ( ied->exec() == QDialog::Accepted )
	{
	workingDir = ied->dirPath();
	QString fname = ied->selectedFile(); 
	QString selectedFilter = ied->selectedFilter();

	QFileInfo fi(fname);
	QString baseName = fi.fileName();
	
	if (baseName.contains(".")==0)
		fname.append(selectedFilter.remove("*"));		
		
	if ( QFile::exists(fname) &&
        QMessageBox::question(0, tr("QtiPlot - Overwrite File?"),
          tr("A file called: <p><b>%1</b><p>already exists. "
                "Do you want to overwrite it?")
                .arg(fname),
            tr("&Yes"), tr("&No"),
            QString::null, 0, 1 ) )
        			return ;
	else
		{
		QFile f(fname);
		if ( !f.open( IO_WriteOnly ) ) 
			{
			QMessageBox::critical(0, tr("QtiPlot - Export Error"),
				tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that you have the right to write to this location!").arg(fname));
   			 return;
			}
			
		if (selectedFilter.contains(".eps"))
			{
			if (ied->showExportOptions())
				{
				epsExportDialog *ed= new epsExportDialog (fname, this, "exportDialog", TRUE, WStyle_Tool|WDestructiveClose);
				connect (ed, SIGNAL(exportToEPS(const QString&, int, QPrinter::Orientation, QPrinter::PageSize, QPrinter::ColorMode)), 
						     plot, SLOT(exportToEPS(const QString&, int, QPrinter::Orientation, QPrinter::PageSize, QPrinter::ColorMode)));
	
				ed->showNormal();
				ed->setActiveWindow();		
				}
			else
				plot->exportToEPS(fname);
			return;
			}
			
		QStringList list=QImage::outputFormatList ();
		for (int i=0; i<(int)list.count(); i++)
			{
			if (selectedFilter.contains("."+(list[i]).lower()))
				{
				if (ied->showExportOptions())
					{
					imageExportDialog* ed= new imageExportDialog(false, this,"exportDialog",TRUE,WStyle_Tool|WDestructiveClose);
					connect (ed, SIGNAL(options(const QString&, const QString&, int, bool)), 
							 plot, SLOT(exportImage(const QString&, const QString&, int, bool)));
	
					ed->setExportPath(fname, list[i]);
					ed->enableTransparency();
					ed->showNormal();
					ed->setActiveWindow();		
					}
				else
					plot->exportImage(fname, list[i], 100, true);
				return;
				}
			}
		}		
    }
	}

else if(plot3DWindows.contains(w->name()))
	((Graph3D*)w)->saveImage();
}

void ApplicationWindow::exportLayer()
{
QWidget *w=ws->activeWindow();
if (!w || !plotWindows.contains(w->name()))
	return;

MultiLayer *plot = (MultiLayer*)w;
Graph* g = (Graph*)plot->activeGraph();
if (!g)
	return;
	
ImageExportDialog *ied = new ImageExportDialog(this, 0);
ied->setDir(workingDir);
if ( ied->exec() == QDialog::Accepted )
	{
	workingDir = ied->dirPath();
	QString fname = ied->selectedFile(); 
	QString selectedFilter = ied->selectedFilter();

	QFileInfo fi(fname);
	QString baseName = fi.fileName();
	
	if (baseName.contains(".")==0)
		fname.append(selectedFilter.remove("*"));		
		
	if ( QFile::exists(fname) &&
        QMessageBox::question(0, tr("QtiPlot - Overwrite File?"),
          tr("A file called: <p><b>%1</b><p>already exists. "
                "Do you want to overwrite it?")
                .arg(fname),
            tr("&Yes"), tr("&No"),
            QString::null, 0, 1 ) )
        			return ;
	else
		{
		QFile f(fname);
		if ( !f.open( IO_WriteOnly ) ) 
			{
			QMessageBox::critical(0, tr("QtiPlot - Export Error"),
				tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that you have the right to write to this location!").arg(fname));
   			 return;
			}
			
		if (selectedFilter.contains(".eps"))
			{
			if (ied->showExportOptions())
				{
				epsExportDialog *ed= new epsExportDialog (fname, this, "exportDialog", TRUE, WStyle_Tool|WDestructiveClose);
				connect (ed, SIGNAL(exportToEPS(const QString&, int, QPrinter::Orientation, QPrinter::PageSize, QPrinter::ColorMode)), 
						  g, SLOT(exportToEPS(const QString&, int, QPrinter::Orientation, QPrinter::PageSize, QPrinter::ColorMode)));
	
				ed->showNormal();
				ed->setActiveWindow();		
				}
			else
				g->exportToEPS(fname);

			if (plot->hasOverlapingLayers())
				plot->updateTransparency();
			return;
			}			
		/*else if (selectedFilter.contains(".wmf"))
			{
			g->exportToWmf(fname);
			return;
			}*/
		QStringList list=QImage::outputFormatList ();
		for (int i=0; i<(int)list.count(); i++)
			{
			if (selectedFilter.contains("."+(list[i]).lower()))
				{
				if (ied->showExportOptions())
					{
					imageExportDialog* ed= new imageExportDialog(false, this,"exportDialog",TRUE,WStyle_Tool|WDestructiveClose);
					connect (ed, SIGNAL(options(const QString&, const QString&, int, bool)), 
							g, SLOT(exportImage(const QString&, const QString&, int, bool)));
	
					ed->setExportPath(fname, list[i]);
					ed->enableTransparency();
					ed->showNormal();
					ed->setActiveWindow();
					}
				else
					g->exportImage(fname, list[i], 100, true);

				if (plot->hasOverlapingLayers())
					plot->updateTransparency();
				return;
				}
			}
		}		
    }
}

void ApplicationWindow::exportAllGraphs()
{
QString dir = QFileDialog::getExistingDirectory(workingDir, this, "get existing directory", 
					"Choose a directory to export the graphs to", true, true);
if (!dir.isEmpty())
	{
	imageExportDialog* ed= new imageExportDialog(true, this,"exportDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (ed, SIGNAL(exportAll(const QString&, const QString&, int, bool)),
				  this, SLOT(exportAllGraphs(const QString&, const QString&, int, bool)));
		
	workingDir = dir;
	ed->setExportDirPath(dir);
	ed->enableTransparency();
	ed->showNormal();
	ed->setActiveWindow();
	}
}

void ApplicationWindow::exportAllGraphs(const QString& dir, const QString& format, 
										int quality, bool transparency)
{
QApplication::setOverrideCursor(waitCursor);
	
QWidgetList windows = ws->windowList();
QString fileType = format;
fileType.lower();
fileType.prepend(".");

bool confirmOverwrite = true;	
for (int i = 0; i<int(windows.count()); i++ )
	{
	QString caption = windows.at(i)->name();
	MultiLayer *plot = 0;
	Graph3D *splot =0;
		
	if (plotWindows.contains(caption))
		plot = (MultiLayer*)windows.at(i);
	else if (plot3DWindows.contains(caption))
		splot = (Graph3D*)windows.at(i);
	
	if (plot || splot)
		{
		QString fileName = dir + "/" + caption + fileType;
		QFile f(fileName);
		if (f.exists(fileName) && confirmOverwrite)
			{
			QApplication::restoreOverrideCursor();
        	switch(QMessageBox::question(0, tr("QtiPlot - Overwrite File?"),
           		 tr("A file called: <p><b>%1</b><p>already exists. "
                "Do you want to overwrite it?") .arg(fileName), tr("&Yes"), tr("&All"), tr("&Cancel"), 0, 1))
				{
				case 0:
					if (plot)
						export2DPlotToFile(plot, fileName, format, quality, transparency);
					else if (splot)
						export3DPlotToFile(splot, fileName, format);
				break;
			
				case 1:
        			confirmOverwrite = false;
					if (plot)
						export2DPlotToFile(plot, fileName, format, quality, transparency);
					else if (splot)
						export3DPlotToFile(splot, fileName, format);
				break;
				
				case 2:
					return;
				break;
				}
			}
		else
			{
			if (plot)
				export2DPlotToFile(plot, fileName, format, quality, transparency);
			else if (splot)
				export3DPlotToFile(splot, fileName, format);
			}
		}
	}
	
QApplication::restoreOverrideCursor();
}

void ApplicationWindow::export2DPlotToFile(MultiLayer *plot, const QString& fileName, 
										const QString& format, int quality, bool transparency)
{
if (plot->isEmpty())
	{
	QApplication::restoreOverrideCursor();
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("There are no plot layers available in window <b>"+
				QString(plot->name()) + "</b>.<br>Plot window not exported!"));
	return;
	}

QFile f(fileName);
if ( !f.open( IO_WriteOnly ) ) 
	{
	QApplication::restoreOverrideCursor();
	QMessageBox::critical(0, tr("QtiPlot - Export Error"), 
		tr("Could not write to file: <br><h4>%1</h4><p>Please verify that you have the right to write to this location!").arg(fileName));
   				
	return;
	}
				
if (format == "EPS")
	plot->exportToEPS(fileName);
else
	plot->exportImage(fileName, format, quality, transparency);	
}

void ApplicationWindow::export3DPlotToFile(Graph3D *plot, const QString& fileName, 
																	const QString& format)
{
QFile f(fileName);
if ( !f.open( IO_WriteOnly ) ) 
	{
	QMessageBox::critical(0, tr("QtiPlot - Export Error"), tr("Could not write to file: <br><h4>%1</h4><p>Please verify that you have the right to write to this location!").arg(fileName));
   				
	QApplication::restoreOverrideCursor();
	return;
	}
				
plot->saveImageToFile(fileName, format);	
}

QString ApplicationWindow::windowGeometryInfo(QWidget *w)
{
QString s = "geometry\t";

if (((myWidget *)w)->status() == myWidget::Minimized)
	s+="minimized\n";
else if (((myWidget *)w)->status() == myWidget::Maximized)
	s+="maximized\n";
else
	{
	if (!w->parent())
		s+="0\t0\t500\t400\t";	
	else
		{
		QPoint p = w->parentWidget()->pos();// store position
		s+=QString::number(p.x())+"\t";
		s+=QString::number(p.y())+"\t";
		s+=QString::number(w->parentWidget()->frameGeometry().width())+"\t";
		s+=QString::number(w->parentWidget()->frameGeometry().height())+"\t";
		}

	bool hide = hidden(w);
	if (w == ws->activeWindow() && !hide)
		s+="active\n";
	else if(hide)
		s+="hidden\n";
	else
		s+="\n";
	}
return s;
}

Folder* ApplicationWindow::projectFolder()
{
return ((FolderListItem *)folders->firstChild())->folder();
}

bool ApplicationWindow::saveProject()
{
if (projectname == "untitled" || projectname.contains(".opj", false))
	{
	saveProjectAs();
	return false;
	}

saveFolder(projectFolder(), projectname);

setCaption("QtiPlot - "+projectname);
saved=TRUE;
actionSaveProject->setEnabled(false);
actionUndo->setEnabled(false);
actionRedo->setEnabled(false);

if (autoSave)
	{	
	if (savingTimerId)
		killTimer(savingTimerId);
	savingTimerId=startTimer(autoSaveTime*60000);
	}
else
	savingTimerId=0;

QApplication::restoreOverrideCursor();
return true;
}

void ApplicationWindow::saveProjectAs()
{
QString filter = tr("QtiPlot project")+" (*.qti);;";
filter += tr("Compressed QtiPlot project")+" (*.qti.gz)";

QString selectedFilter;
QString fn = QFileDialog::getSaveFileName(workingDir, filter, this, "project",
			tr("Save Project As"), &selectedFilter, false);
if ( !fn.isEmpty() )
	{
	QFileInfo fi(fn);
	workingDir = fi.dirPath(true);
	QString baseName = fi.fileName();	
	if (!baseName.contains("."))
		fn.append(".qti");
	
	if ( QFile::exists(fn) && !selectedFilter.contains(".gz") &&
        QMessageBox::question(this, tr("QtiPlot -- Overwrite File? "),
            tr("A file called: <p><b>%1</b><p>already exists.\n"
                "Do you want to overwrite it?")
                .arg(fn), tr("&Yes"), tr("&No"),QString::null, 0, 1 ) )
        return ;
	else
		{
		projectname=fn;
		if (saveProject())
			{
			recentProjects.remove(projectname);
			recentProjects.push_front(projectname);
			updateRecentProjectsList();

			QFileInfo fi(fn);
			QString baseName = fi.baseName();
			FolderListItem *item = (FolderListItem *)folders->firstChild();
			item->setText(0, baseName);
			item->folder()->setFolderName(baseName);
			}
		if (selectedFilter.contains(".gz"))
			file_compress((char *)fn.ascii(), "wb9");
		}
    }
}

void ApplicationWindow::saveAsTemplate()
{
myWidget* w = (myWidget*)ws->activeWindow();
if (!w)
	return;

QString filter;
if (matrixWindows.contains(w->name()))
	filter = tr("QtiPlot Matrix Template")+" (*.qmt)";
else if (plotWindows.contains(w->name()))
	filter = tr("QtiPlot 2D Plot Template")+" (*.qpt)";
else if (tableWindows.contains(w->name()))
	filter = tr("QtiPlot Table Template")+" (*.qtt)";
else if (plot3DWindows.contains(w->name()))
	filter = tr("QtiPlot 3D Surface Template")+" (*.qst)";

QString selectedFilter;
QString fn = QFileDialog::getSaveFileName(workingDir, filter, this, "template",
			tr("Save Window As Template"), &selectedFilter, false);
if ( !fn.isEmpty() )
	{
	QFileInfo fi(fn);
	workingDir = fi.dirPath(true);
	QString baseName = fi.fileName();	
	if (!baseName.contains("."))
		{
		selectedFilter = selectedFilter.right(5).left(4);
		fn.append(selectedFilter);	
		}

	if ( QFile::exists(fn) &&
        QMessageBox::question(this, tr("QtiPlot -- Overwrite File? "),
            tr("A file called: <p><b>%1</b><p>already exists.\n"
                "Do you want to overwrite it?")
                .arg(fn), tr("&Yes"), tr("&No"),QString::null, 0, 1 ) )
        return ;
	else
		{
		QFile f(fn);
		if ( !f.open( IO_WriteOnly ) )
			{
			QMessageBox::critical(0, tr("QtiPlot - Export Error"),
			tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that you have the right to write to this location!").arg(fn));
			return;
			}
		QApplication::setOverrideCursor(waitCursor);
		QString text="QtiPlot " + QString::number(majVersion)+"."+ QString::number(minVersion)+"."+
		QString::number(patchVersion)+" template file\n";
		text += w->saveAsTemplate(windowGeometryInfo(w));
		QTextStream t( &f );
		t.setEncoding(QTextStream::UnicodeUTF8);
		t << text;
		f.close();
		QApplication::restoreOverrideCursor();
		}
    }
}

void ApplicationWindow::rename()
{
myWidget* m = (myWidget*)ws->activeWindow();
if (!m)
	return;

renameWindowDialog *rwd = new renameWindowDialog(this,"polyDialog",TRUE,WStyle_Tool|WDestructiveClose);
rwd->setWidget(m);
rwd->showNormal();
rwd->setActiveWindow();
}

void ApplicationWindow::renameWindow()
{
WindowListItem *it = (WindowListItem *)lv->currentItem();
myWidget *w= it->window();
if (!w)
	return;

renameWindowDialog *rwd = new renameWindowDialog(this,"polyDialog",TRUE,WStyle_Tool|WDestructiveClose);
rwd->setWidget(w);
rwd->showNormal();
rwd->setActiveWindow();
}

void ApplicationWindow::renameWindow(QListViewItem *item, int, const QString &text)
{
if (!item)
	return;

myWidget *w = ((WindowListItem *)item)->window();
if (!w || text == w->name())
	return;

while(!renameWindow(w, text))
	{
	item->setRenameEnabled (0, true);
	item->startRename (0);
	return;
	}
}

bool ApplicationWindow::renameWindow(myWidget *w, const QString &text)
{
if (!w)
	return false;

QString name = w->name();

if (text.isEmpty())
	{
	QMessageBox::critical(0, tr("QtiPlot - Error"), tr("Please enter a valid name!"));
	return false;
	}
else if (text.contains(QRegExp("\\W")))
	{
	QMessageBox::critical(0, tr("QtiPlot - Error"),
			   tr("The name you chose is not valid: only letters and digits are allowed!")+
			   "<p>" + tr("Please choose another name!"));
	return false;
	}

while(alreadyUsedName(text))
	{
	QMessageBox::critical(this,tr("QtiPlot - Error"),
				tr("Name already exists!")+"\n"+tr("Please choose another name!"));
	return false;
	}

if (w->isA("Graph"))
	{
	int id=plotWindows.findIndex(name);
	plotWindows[id]=text;
	}
else if (w->isA("Graph3D"))
	{
	int id=plot3DWindows.findIndex(name);
	plot3DWindows[id]=text;
	}
else if (w->isA("Table"))
	{
	QStringList labels=((Table *)w)->colNames();
	if (labels.contains(text)>0)
		{
		QMessageBox::critical(0,tr("QtiPlot - Error"),
		tr("The table name must be different from the names of its columns!")+"<p>"+tr("Please choose another name!"));
		return false;
		}

	int id=tableWindows.findIndex(name);
	tableWindows[id]=text;
	updateTableNames(name,text);
	}
else if (w->isA("Matrix"))
	changeMatrixName(name, text);
else if (w->isA("Note"))
	{
	int id=noteWindows.findIndex(name);
	noteWindows[id]=text;
	}

w->setName(text);
w->setCaptionPolicy(w->captionPolicy());
return true;
}

QStringList ApplicationWindow::columnsList(Table::PlotDesignation plotType)
{
QWidgetList *windows = windowsList();
QStringList list;
for (int i=0;i<(int)windows->count();i++)
	{
	Table *w=(Table*)windows->at(i);
	if (w && tableWindows.contains(w->name()))
		{
		int n=w->tableCols();
		for (int j=0;j<n;j++)
			{
			if (plotType == Table::All || w->colPlotDesignation(j) == plotType)
				list<<QString(w->name())+"_"+w->colLabel(j);
			}
		}
	}
delete windows;
return list;
}

void ApplicationWindow::showCurvesDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Error"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if (!g)
	return;

if (g->isPiePlot())
	{
	QMessageBox::warning(this,tr("QtiPlot - Error"),
				tr("This functionality is not available for pie plots!"));
	}
else
	{
	curvesDialog* crvDialog=new curvesDialog(this,"curves",TRUE,WStyle_StaysOnTop|WStyle_Tool|WDestructiveClose);
	connect (crvDialog,SIGNAL(showPlotAssociations(int)), this, SLOT(showPlotAssociations(int)));
	connect (crvDialog,SIGNAL(showFunctionDialog(const QString&, int)), 
				  this, SLOT(showFunctionDialog(const QString&, int)));

	crvDialog->insertCurvesToDialog(columnsList(Table::Y));
	crvDialog->setCurveDefaultSettings(defaultCurveStyle, defaultCurveLineWidth, defaultSymbolSize);
	crvDialog->setGraph(g);
	crvDialog->initTablesList(tableList());
	crvDialog->showNormal();
	crvDialog->setActiveWindow();

	activeGraph = g;
	}
}

QWidgetList* ApplicationWindow::tableList()
{
QWidgetList* lst = new QWidgetList();
QWidgetList *windows = windowsList();
for (int i = 0; i < int(windows->count());i++ )
	{
	if (tableWindows.contains(windows->at(i)->name()))
		lst->append(windows->at(i));
	}
delete windows;
return lst;
}

void ApplicationWindow::showPlotAssociations(int curve)
{
if (!activeGraph)
	return;

associationsDialog* ad=new associationsDialog(this, "curves", true, WStyle_StaysOnTop|WStyle_Tool|WDestructiveClose);
ad->setGraph(activeGraph);
ad->initTablesList(tableList(), curve);
ad->showNormal();
ad->setActiveWindow();
}

void ApplicationWindow::showTitleDialog()
{
QWidget *w = ws->activeWindow();
if (!w)
	return;
QString caption=w->name();

if (plotWindows.contains(caption))
{
MultiLayer* plot = (MultiLayer*)w;
if (!plot)
	return;
Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,"TextDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setTitleFont(const QFont &)));
	connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setTitle(const QString &)));
	connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setTitleColor(const QColor &)));
	connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setTitleAlignment(int)));

	td->setText(g->title());
	td->setFont(g->titleFont());
	td->setTextColor(g->titleColor());
	td->setAlignment(g->titleAlignment());
	td->showNormal();
	td->setActiveWindow();
	g->setTitleSelected(FALSE);
	}
}
else if (plot3DWindows.contains(caption)>0)
	{
	plot3DDialog* pd = (plot3DDialog*)showPlot3dDialog();
	if (pd)
		pd->showTitleTab();
	}
}

void ApplicationWindow::showXAxisTitleDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,"axesDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setXAxisTitleFont(const QFont &)));
	connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setXAxisTitle(const QString &)));
	connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setXAxisTitleColor(const QColor &)));
	connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setXAxisTitleAlignment(int)));

	QStringList t=g->scalesTitles();
	td->setText(t[0]);
	td->setFont(g->axisTitleFont(2));
	td->setTextColor(g->axisTitleColor(2));
	td->setAlignment(g->axisTitleAlignment(2));
	td->setCaption(tr("QtiPlot - X Axis Title"));
	td->showNormal();
	td->setActiveWindow();
	}
}

void ApplicationWindow::showYAxisTitleDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,"axesDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setYAxisTitleFont(const QFont &)));
	connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setYAxisTitle(const QString &)));
	connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setYAxisTitleColor(const QColor &)));
	connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setYAxisTitleAlignment(int)));

	QStringList t=g->scalesTitles();
	td->setText(t[1]);
	td->setFont(g->axisTitleFont(0));
	td->setTextColor(g->axisTitleColor(0));
	td->setAlignment(g->axisTitleAlignment(0));
	td->setCaption(tr("QtiPlot - Y Axis Title"));
	td->showNormal();
	td->setActiveWindow();
	}
}

void ApplicationWindow::showRightAxisTitleDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,"axesDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setRightAxisTitleFont(const QFont &)));
	connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setRightAxisTitle(const QString &)));
	connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setRightAxisTitleColor(const QColor &)));
	connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setRightAxisTitleAlignment(int)));

	QStringList t=g->scalesTitles();
	td->setText(t[3]);
	td->setFont(g->axisTitleFont(1));
	td->setTextColor(g->axisTitleColor(1));
	td->setAlignment(g->axisTitleAlignment(1));
	td->setCaption(tr("QtiPlot - Right Axis Title"));
	td->showNormal();
	td->setActiveWindow();
	}
}

void ApplicationWindow::showTopAxisTitleDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,"axesDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setTopAxisTitleFont(const QFont &)));
	connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setTopAxisTitle(const QString &)));
	connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setTopAxisTitleColor(const QColor &)));
	connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setTopAxisTitleAlignment(int)));

	QStringList t=g->scalesTitles();
	td->setText(t[2]);
	td->setFont(g->axisTitleFont(3));
	td->setTextColor(g->axisTitleColor(3));
	td->setAlignment(g->axisTitleAlignment(3));
	td->setCaption(tr("QtiPLot - Top Axis Title"));
	td->showNormal();
	td->setActiveWindow();
	}
}

void ApplicationWindow::showExportASCIIDialog()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	{
	exportDialog* ed= new exportDialog(this,"exportDialog",TRUE,WStyle_ContextHelp|WDestructiveClose);
	connect (ed, SIGNAL(exportTable(const QString&, const QString&, bool, bool)), 
			 this, SLOT(exportASCII (const QString&, const QString&, bool, bool)));
	connect (ed, SIGNAL(exportAllTables(const QString&, bool, bool)), 
			 this, SLOT(exportAllTables(const QString&, bool, bool)));

	ed->setTableNames(tableWindows);
	ed->setActiveTableName(w->name());
	ed->setColumnSeparator(separator);
	ed->showNormal();
	ed->setActiveWindow();
	}
}

void ApplicationWindow::exportAllTables(const QString& sep, bool colNames, bool expSelection)
{
QString dir = QFileDialog::getExistingDirectory(workingDir, this, "get existing directory",
                    tr("Choose a directory to export the tables to"), true, true);
if (!dir.isEmpty())
{
QApplication::setOverrideCursor(waitCursor);
QWidgetList windows = ws->windowList();
workingDir = dir;

bool confirmOverwrite = true;
bool success = true;	
for (int i = 0; i<int(windows.count()); i++ )
	{
	QString caption = windows.at(i)->name();
	Table *t = (Table*)windows.at(i);		
	if (t && tableWindows.contains(caption))
		{
		QString fileName = dir + "/" + caption + ".txt";
		QFile f(fileName);
		if (f.exists(fileName) && confirmOverwrite)
			{
			QApplication::restoreOverrideCursor();
        	switch(QMessageBox::question(0, tr("QtiPlot - Overwrite File?"),
           		 tr("A file called: <p><b>%1</b><p>already exists. "
                "Do you want to overwrite it?").arg(fileName), tr("&Yes"), tr("&All"), tr("&Cancel"), 0, 1))
				{
				case 0:
					success = t->exportToASCIIFile(fileName, sep, colNames, expSelection);
				break;
			
				case 1:
        			confirmOverwrite = false;
					success = t->exportToASCIIFile(fileName, sep, colNames, expSelection);
				break;
				
				case 2:
					return;
				break;
				}
			}
		else
			success = t->exportToASCIIFile(fileName, sep, colNames, expSelection);

		if (!success)
			break;
		}
	}
	
QApplication::restoreOverrideCursor();
}
}

void ApplicationWindow::exportASCII(const QString& tableName, const QString& sep, 
									bool colNames, bool expSelection)
{
Table* t = table(tableName);
if (!t)
	return;

QString selectedFilter;
QString fname = QFileDialog::getSaveFileName(workingDir,"*.txt;;*.dat;;*.DAT", this,"file dialog",
	tr("Choose a filename to save under"),&selectedFilter,TRUE);
	
if (!fname.isEmpty() ) 
	{ // the user gave a file name	
	QFileInfo fi(fname);
	QString baseName = fi.fileName();	
	if (baseName.contains(".")==0)
			fname.append(selectedFilter.remove("*"));	
	
	workingDir = fi.dirPath(true);

	if ( QFile::exists(fname) &&
        	QMessageBox::question(
            0,
            tr("QtiPlot - Overwrite File?"),
            tr("A file called: <p><b>%1</b><p>already exists. "
                "Do you want to overwrite it?")
                .arg(fname),
            tr("&Yes"), tr("&No"),
            QString::null, 0, 1 ) )
        			return ;
	else
		{	
		QApplication::setOverrideCursor(waitCursor);
		t->exportToASCIIFile(fname, sep, colNames, expSelection);
		QApplication::restoreOverrideCursor();
		}
	}
}

void ApplicationWindow::showRowsDialog()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	{
	bool ok;
    int rows = QInputDialog::getInteger(
            tr("QtiPlot - Enter rows number"), tr("Rows"), w->tableRows(), 0, 1000000, 1,
            &ok, this );
    if ( ok ) 
        w->resizeRows(rows);
	}
}

void ApplicationWindow::showColsDialog()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	{
	bool ok;
    int cols = QInputDialog::getInteger(
            tr("QtiPlot - Enter columns number"), tr("Columns"), w->tableCols(), 0, 1000000, 1,
            &ok, this );
    if ( ok ) 
        w->resizeCols(cols);
	}
}

void ApplicationWindow::showColumnValuesDialog()
{
Table* w = (Table*)ws->activeWindow();

if ( w && tableWindows.contains(w->name()))
	{
	if (int(w->selectedColumns().count())>0)
		{
		setColValuesDialog* vd= new setColValuesDialog(this,"valuesDialog",TRUE,WDestructiveClose);
		vd->setTable(w);
		vd->showNormal();
		vd->setActiveWindow();
		}
	else
		QMessageBox::warning(this, tr("QtiPlot - Column selection error"),
		tr("Please select a column first!"));
	}
}

void ApplicationWindow::sortActiveTable()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	{
	if (int(w->selectedColumns().count())>0)
		w->sortTableDialog();
	else
		QMessageBox::warning(this, "QtiPlot - Column selection error","Please select a column first!");
	}
}

void ApplicationWindow::sortSelection()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	w->sortColumnsDialog();
}

void ApplicationWindow::normalizeActiveTable()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	{
	if (int(w->selectedColumns().count())>0)
		w->normalizeTable();
	else
		QMessageBox::warning(this, "QtiPlot - Column selection error","Please select a column first!");
	}
}

void ApplicationWindow::normalizeSelection()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	{
	if (int(w->selectedColumns().count())>0)
		w->normalizeSelection();
	else
		QMessageBox::warning(this, "QtiPlot - Column selection error","Please select a column first!");
	}
}

void ApplicationWindow::correlate()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	w->correlate();
}

void ApplicationWindow::convolute()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	w->convolute(1);
}

void ApplicationWindow::deconvolute()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	w->convolute(-1);
}

void ApplicationWindow::showColStatistics()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	{
	if (int(w->selectedColumns().count())>0)
		w->showColStatistics();
	else
		QMessageBox::warning(this, "QtiPlot - Column selection error","Please select a column first!");
	}
}

void ApplicationWindow::showRowStatistics()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	{
	if (w->selectedRows()>0)
		w->showRowStatistics();
	else
		QMessageBox::warning(this ,"QtiPlot - Row selection error","Please select a row first!");
	}
}

void ApplicationWindow::showColMenu(int c)
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	{
	QPopupMenu Legend(this);
	QPopupMenu plot(this);
	QPopupMenu specialPlot(this);
	QPopupMenu fill(this);
	QPopupMenu sorting(this);
	QPopupMenu colType(this);
	colType.setCheckable ( true );
	QPopupMenu panels(this);
	QPopupMenu stat(this);
	QPopupMenu norm(this);
		
	if ((int)w->selectedColumns().count()==1)
		{
		w->setSelectedCol(c);
		plot.insertItem(QPixmap(lPlot_xpm),tr("&Line"),w, SLOT(plotL()));
		plot.insertItem(QPixmap(pPlot_xpm),tr("&Scatter"),w, SLOT(plotP()));
		plot.insertItem(QPixmap(lpPlot_xpm),tr("Line + s&ymbol"),w,SLOT(plotLP()));

		specialPlot.insertItem(QPixmap(dropLines_xpm),tr("Vertical &drop lines"),w,SLOT(plotVerticalDropLines()));
		specialPlot.insertItem(QPixmap(spline_xpm),tr("&Spline"),w,SLOT(plotSpline()));
		specialPlot.insertItem(QPixmap(steps_xpm),tr("&Vertical Steps"),w,SLOT(plotSteps()));
		plot.insertItem(tr("Special Line/Symb&ol"),&specialPlot);
		plot.insertSeparator();

		plot.insertItem(QPixmap(vertBars_xpm),tr("&Columns"),w,SLOT(plotVB()));
		plot.insertItem(QPixmap(hBars_xpm),tr("&Rows"),w,SLOT(plotHB()));
		plot.insertItem(QPixmap(area_xpm),tr("&Area"),w,SLOT(plotArea()));

		plot.insertItem(QPixmap(pie_xpm),tr("&Pie"),w,SLOT(plotPie()));
		plot.insertSeparator();

		plot.insertItem(QPixmap(ribbon_xpm),tr("3D Ribbo&n"),w,SLOT(plot3DRibbon()));
		plot.insertItem(QPixmap(bars_xpm),tr("3D &Bars"),w,SLOT(plot3DBars()));
		plot.insertItem(QPixmap(scatter_xpm),tr("3&D Scatter"),w,SLOT(plot3DScatter()));
		plot.insertItem(QPixmap(trajectory_xpm),tr("3D &Trajectory"),w,SLOT(plot3DTrajectory()));

		plot.insertSeparator();

		actionBoxPlot->addTo(&stat);
		stat.insertItem(QPixmap(histogram_xpm),tr("&Histogram"),w,SLOT(plotHistogram()));
		stat.insertItem(QPixmap(stacked_hist_xpm),tr("&Stacked Histograms"),this,SLOT(plotStackedHistograms()));
		plot.insertItem(tr("Statistical &Graphs"),&stat);

		Legend.insertItem(tr("&Plot"),&plot);
		Legend.insertSeparator();

		Legend.insertItem(QPixmap(cut_xpm),tr("Cu&t"), w, SLOT(cutSelection()));
		Legend.insertItem(QPixmap(copy_xpm),tr("&Copy"), w, SLOT(copySelection()));
		Legend.insertItem(QPixmap(paste_xpm),tr("Past&e"), w, SLOT(pasteSelection()));
		Legend.insertSeparator();

		int xColID=colType.insertItem("X", w, SLOT(setXCol()));
		int yColID=colType.insertItem("Y", w, SLOT(setYCol()));
		int zColID=colType.insertItem("Z", w, SLOT(setZCol()));
		int noneID=colType.insertItem(tr("None"), w, SLOT(disregardCol()));
		
		if (w->colPlotDesignation(c) == Table::X)
			colType.setItemChecked (xColID,true);
		else if (w->colPlotDesignation(c) == Table::Y)
			colType.setItemChecked (yColID,true);
		else if (w->colPlotDesignation(c) == Table::Z)
			colType.setItemChecked (zColID,true);
		else
			colType.setItemChecked (noneID,true);

		Legend.insertItem(tr("Set as"),&colType);
		Legend.insertSeparator();

		Legend.insertItem(tr("Set column &values..."),w,SIGNAL(colValuesDialog()));
		actionSetAscValues->addTo(&fill);
		actionSetRandomValues->addTo(&fill);
		Legend.insertItem(tr("&Fill column with"),&fill);
		
		norm.insertItem(tr("&Column"), w, SLOT(normalizeSelection()));
		actionNormalizeTable->addTo(&norm);
		Legend.insertItem(tr("&Normalize"), &norm);

		Legend.insertSeparator();
		actionShowColStatistics->addTo(&Legend);

		Legend.insertSeparator();

		Legend.insertItem(QPixmap(erase_xpm), tr("Clea&r"), w, SLOT(clearCol()));
		Legend.insertItem(QPixmap(close_xpm), tr("&Delete"), w, SLOT(removeCol()));
		Legend.insertItem(tr("&Insert"), w, SLOT(insertCol()));
		Legend.insertItem(tr("&Add column"),w, SLOT(addCol()));
		Legend.insertSeparator();

		sorting.insertItem(tr("&Ascending"),w, SLOT(sortColAsc()));
		sorting.insertItem(tr("&Descending"),w, SLOT(sortColDesc()));
		Legend.insertItem(tr("Sort Colu&mn"),&sorting);

		actionSortTable->addTo(&Legend);

		Legend.insertSeparator();
		actionShowColumnOptionsDialog->addTo(&Legend);
		}
	else if ((int)w->selectedColumns().count()>1)
		{
		plot.insertItem(QPixmap(lPlot_xpm),tr("&Line"),w, SLOT(plotL()));
		plot.insertItem(QPixmap(pPlot_xpm),tr("&Scatter"),w, SLOT(plotP()));
		plot.insertItem(QPixmap(lpPlot_xpm),tr("Line + s&ymbol"),w,SLOT(plotLP()));

		specialPlot.insertItem(QPixmap(dropLines_xpm),tr("Vertical &drop lines"),w,SLOT(plotVerticalDropLines()));
		specialPlot.insertItem(QPixmap(spline_xpm),tr("&Spline"),w,SLOT(plotSpline()));
		specialPlot.insertItem(QPixmap(steps_xpm),tr("&Vertical Steps"),w,SLOT(plotSteps()));
		plot.insertItem(tr("Special Line/Symb&ol"),&specialPlot);
		plot.insertSeparator();

		plot.insertItem(QPixmap(vertBars_xpm),tr("&Columns"),w,SLOT(plotVB()));
		plot.insertItem(QPixmap(hBars_xpm),tr("&Rows"),w,SLOT(plotHB()));
		plot.insertItem(QPixmap(area_xpm),tr("&Area"),w,SLOT(plotArea()));
		plot.insertItem(QPixmap(vectXYXY_xpm),tr("Vectors &XYXY"), w, SLOT(plotVectXYXY()));
		plot.insertSeparator();

		actionBoxPlot->addTo(&stat);
		stat.insertItem(QPixmap(histogram_xpm),tr("&Histogram"),w,SLOT(plotHistogram()));
		stat.insertItem(QPixmap(stacked_hist_xpm),tr("&Stacked Histograms"),this,SLOT(plotStackedHistograms()));
		plot.insertItem(tr("Statistical &Graphs"),&stat);

		panels.insertItem(QPixmap(panel_v2_xpm),tr("&Vertical 2 Layers"),this, SLOT(plot2VerticalLayers()));
		panels.insertItem(QPixmap(panel_h2_xpm),tr("&Horizontal 2 Layers"),this, SLOT(plot2HorizontalLayers()));
		panels.insertItem(QPixmap(panel_4_xpm),tr("&4 Layers"),this, SLOT(plot4Layers()));
		panels.insertItem(QPixmap(stacked_xpm),tr("&Stacked Layers"),this, SLOT(plotStackedLayers()));
		plot.insertItem(tr("Pa&nel"),&panels);
		
		Legend.insertItem(tr("&Plot"),&plot);
		Legend.insertSeparator();
		Legend.insertItem(QPixmap(cut_xpm),tr("Cu&t"), w, SLOT(cutSelection()));
		Legend.insertItem(QPixmap(copy_xpm),tr("&Copy"), w, SLOT(copySelection()));
		Legend.insertItem(QPixmap(paste_xpm),tr("Past&e"), w, SLOT(pasteSelection()));
		Legend.insertSeparator();

		Legend.insertItem(QPixmap(erase_xpm),tr("Clea&r"), w, SLOT(clearSelection()));
		Legend.insertItem(QPixmap(close_xpm),tr("&Delete"), w, SLOT(removeCol()));
		Legend.insertSeparator();
		Legend.insertItem(tr("&Insert"), w, SLOT(insertCol()));
		Legend.insertItem(tr("&Add column"),w, SLOT(addCol()));
		Legend.insertSeparator();

		actionSetXCol->addTo(&colType);
		actionSetYCol->addTo(&colType);
		actionSetZCol->addTo(&colType);
		actionDisregardCol->addTo(&colType);
		Legend.insertItem(tr("Set as"),&colType);
		Legend.insertSeparator();

		actionSetAscValues->addTo(&fill);
		actionSetRandomValues->addTo(&fill);
		Legend.insertItem(tr("&Fill columns with"),&fill);
		
		actionNormalizeSelection->addTo(&norm);
		actionNormalizeTable->addTo(&norm);
		Legend.insertItem(tr("&Normalize"), &norm);

		Legend.insertSeparator();
		actionSortSelection->addTo(&Legend);
		actionSortTable->addTo(&Legend);
		Legend.insertSeparator();
		actionShowColStatistics->addTo(&Legend);
		}

	QPoint posMouse=QCursor::pos();
    Legend.exec(posMouse);
	}
}

void ApplicationWindow::plot2VerticalLayers()
{
multilayerPlot(1, 2, defaultCurveStyle);
}

void ApplicationWindow::plot2HorizontalLayers()
{
multilayerPlot(2, 1, defaultCurveStyle);
}

void ApplicationWindow::plot4Layers()
{
multilayerPlot(2, 2, defaultCurveStyle);
}

void ApplicationWindow::plotStackedLayers()
{
multilayerPlot(1, -1, defaultCurveStyle);
}

void ApplicationWindow::plotStackedHistograms()
{
multilayerPlot(1, -1, Graph::Histogram);
}

void ApplicationWindow::showMatrixDialog()
{
Matrix* w = (Matrix*)ws->activeWindow();
if ( w && matrixWindows.contains(w->name()))
	{
	matrixDialog* md= new matrixDialog(this,"matrixDialog", false, WDestructiveClose);
	connect (md, SIGNAL(changeColumnsWidth(int)), w, SLOT(setColumnsWidth(int)));
	connect (md, SIGNAL(changeTextFormat(const QChar&, int)), 
			 w, SLOT(setNumericFormat(const QChar&, int)));
	connect (md, SLOT(close()), w, SLOT(freeMemory()));

	w->storeCellsToMemory();
	md->setTextFormat(w->textFormat(), w->precision());
	md->setColumnsWidth(w->columnsWidth());
	md->showNormal();
	}
}

void ApplicationWindow::showMatrixSizeDialog()
{
Matrix* w = (Matrix*)ws->activeWindow();
if ( w && matrixWindows.contains(w->name()))
	{
	matrixSizeDialog* md= new matrixSizeDialog(this,"matrixDialog", false, WDestructiveClose);
	connect (md, SIGNAL(changeDimensions(int, int)), w, SLOT(setMatrixDimensions(int, int)));
	connect (md, SIGNAL(changeCoordinates(double, double, double, double)), 
			 w, SLOT(setCoordinates(double, double, double, double)));

	md->setCoordinates(w->xStart(), w->xEnd(), w->yStart(), w->yEnd());
	md->setColumns(w->numCols());
	md->setRows(w->numRows());
	md->showNormal();
	}
}

void ApplicationWindow::showMatrixValuesDialog()
{
Matrix* w = (Matrix*)ws->activeWindow();
if ( w && matrixWindows.contains(w->name()))
	{
	matrixValuesDialog* md= new matrixValuesDialog(this,"matrixValuesDialog", false, WDestructiveClose);
	connect (md, SIGNAL(setValues (const QString&, const QString&, const QStringList&, 
									const QStringList&, int, int, int, int)), 
			w, SLOT(setValues (const QString&, const QString&, const QStringList&,
							  const QStringList&, int, int, int, int)));

	md->setFormula(w->formula());
	md->setColumns(w->numCols());
	md->setRows(w->numRows());
	md->showNormal();
	}
}

void ApplicationWindow::showColumnOptionsDialog()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	{
	if(	int(w->selectedColumns().count())>0)
		{
		tableDialog* td= new tableDialog(this,"optionsDialog", false, WDestructiveClose);
		td->setWorksheet(w);
		td->showNormal();
		}
	else
		QMessageBox::warning(this, "QtiPlot","Please select a column first!");
	}
}

void ApplicationWindow::showAxis(int axis, int type, const QString& labelsColName, bool axisOn, 
								 int ticksType, bool labelsOn, const QColor& c, int format, 
								 int prec, int rotation, int baselineDist, const QString& formula)
{
Table *w = table(labelsColName);
if ((type == Graph::Txt || type == Graph::ColHeader) && !w)
	return;

activeGraph->showAxis(axis, type, labelsColName, w, axisOn, ticksType, labelsOn, 
					  c, format, prec, rotation, baselineDist, formula);
}

void ApplicationWindow::showGeneralPlotDialog()
{
QWidget* plot = ws->activeWindow();
if (!plot)
	return;

QDialog* gd = showScaleDialog();
if (gd && plotWindows.contains(plot->name()) && ((MultiLayer*)plot)->graphsNumber())
	{
	Graph* g = ((MultiLayer*)plot)->activeGraph();
	if (!g->isPiePlot())
		((axesDialog*)gd)->showGeneralPage();
	else
		((pieDialog*)gd)->showGeneralPage();
	}
else if (gd && plot3DWindows.contains(plot->name()))
	((plot3DDialog*)gd)->showGeneralTab();
}

void ApplicationWindow::showAxisDialog()
{
QWidget* plot = (QWidget*)ws->activeWindow();
if (!plot)
	return;

QDialog* gd = showScaleDialog();
if (gd && plotWindows.contains(plot->name()) && ((MultiLayer*)plot)->graphsNumber())
	((axesDialog*)gd)->showAxesPage();
else if (gd && plot3DWindows.contains(plot->name()))
	((plot3DDialog*)gd)->showAxisTab();
}

void ApplicationWindow::showGridDialog()
{
axesDialog* gd = (axesDialog*)showScaleDialog();
if (gd)
	gd->showGridPage();
}

QDialog* ApplicationWindow::showScaleDialog()
{
QWidget *w = ws->activeWindow();
if (!w)
	return 0;

if (plotWindows.contains(w->name()))
{
MultiLayer* plot = (MultiLayer*)w;
if (!plot || !plot->graphsNumber())
	return 0;

Graph* g = (Graph*)plot->activeGraph();
if (!g->isPiePlot())
	{
	activeGraph = g;
		
	axesDialog* ad= new axesDialog(this,"ad",TRUE,WStyle_Tool|WDestructiveClose);
	connect (ad,SIGNAL(updateAxisTitle(int,const QString&)),g,SLOT(setAxisTitle(int,const QString&)));
	connect (ad,SIGNAL(changeAxisFont(int, const QFont &)),g,SLOT(setAxisFont(int,const QFont &)));
	connect (ad,SIGNAL(showAxis(int, int, const QString&, bool,int, bool,const QColor&, int, int, int, int, const QString&)),
			this, SLOT(showAxis(int,int, const QString&, bool, int,bool,const QColor&, int, int, int, int, const QString&)));

	ad->setMultiLayerPlot(plot);
	ad->setLabelsNumericFormat(g->labelsNumericFormat());
	ad->insertColList(columnsList(Table::All));
	ad->insertTablesList(tableWindows);
	ad->setAxesLabelsFormatInfo(g->axesLabelsFormatInfo());
	ad->setEnabledAxes(g->enabledAxes());
	ad->setAxesType(g->axesType());
	ad->setAxesBaseline(g->axesBaseline());
	ad->setScaleLimits(g->plotLimits());
	ad->initAxisFonts(g->axisFont(2), g->axisFont(0),g->axisFont(3),g->axisFont(1));
	ad->setAxisTitles(g->scalesTitles());
	ad->updateTitleBox(0);
	ad->putGridOptions(g->getGridOptions());
	ad->setAxesColors(g->axesColors());
	ad->setTicksType(g->ticksType());
	ad->setEnabledTickLabels(g->enabledTickLabels());
	ad->initLabelsRotation(g->labelsRotation(QwtPlot::xBottom), g->labelsRotation(QwtPlot::xTop));
	ad->showNormal();
	ad->setActiveWindow();
	return ad;
	}
else if (g->isPiePlot())
	return showPieDialog();
}
else if (plot3DWindows.contains(w->name()))
	return showPlot3dDialog();

return 0;
}

axesDialog* ApplicationWindow::showScalePageFromAxisDialog(int axisPos)
{
axesDialog* gd = (axesDialog*)showScaleDialog();
if (gd)
	gd->setCurrentScale(axisPos);
	
return gd;
}

axesDialog* ApplicationWindow::showAxisPageFromAxisDialog(int axisPos)
{
axesDialog* gd = (axesDialog*)showScaleDialog();
if (gd)
	{
	gd->showAxesPage();
	gd->setCurrentScale(axisPos);
	}
return gd;
}

QDialog* ApplicationWindow::showPlot3dDialog()
{
Graph3D* g = (Graph3D*)ws->activeWindow();

if ( g && plot3DWindows.contains(g->name())>0)
	{
	if (!g->hasData())
		{
		QApplication::restoreOverrideCursor();
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				      tr("Not available for empty 3D surface plots!"));
		return 0;
		}

	plot3DDialog* pd= new plot3DDialog(this,"plot3DDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (pd,SIGNAL(updateColors(const QColor&,const QColor&,const QColor&,const QColor&,const QColor&,const QColor&)),
		g,SLOT(updateColors(const QColor&,const QColor&,const QColor&,const QColor&,const QColor&,const QColor&)));

	connect (pd,SIGNAL(updateDataColors(const QColor&,const QColor&)),
		g,SLOT(setDataColors(const QColor&,const QColor&)));

	connect (pd,SIGNAL(updateTitle(const QString&,const QColor&,const QFont&)),
		g,SLOT(updateTitle(const QString&,const QColor&,const QFont&)));
	connect (pd,SIGNAL(updateResolution(int)),g,SLOT(setResolution(int)));
	connect (pd,SIGNAL(showColorLegend(bool)),g,SLOT(showColorLegend(bool)));
	connect (pd,SIGNAL(updateLabel(int,const QString&, const QFont&)),
		g,SLOT(updateLabel(int,const QString&, const QFont&)));
	connect (pd,SIGNAL(updateScale(int,const QStringList&)),
		g,SLOT(updateScale(int,const QStringList&)));
	connect (pd,SIGNAL(adjustLabels(int)),
		g,SLOT(adjustLabels(int)));
	connect (pd,SIGNAL(updateTickLength(int, double, double)),
		g,SLOT(updateTickLength(int, double, double)));
	connect (pd,SIGNAL(setNumbersFont(const QFont&)),
		g,SLOT(setNumbersFont(const QFont&)));
	connect (pd,SIGNAL(updateMeshLineWidth(int)),
		g,SLOT(setMeshLineWidth(int)));
	connect (pd,SIGNAL(updateBars(double)),g,SLOT(updateBars(double)));
	connect (pd,SIGNAL(updatePoints(double, bool)),g, SLOT(updatePoints(double, bool)));
	connect (pd,SIGNAL(updateTransparency(double)),g, SLOT(changeTransparency(double)));
	connect (pd,SIGNAL(showWorksheet()),g,SLOT(showWorksheet()));
	connect (pd,SIGNAL(updateZoom(double)),g,SLOT(updateZoom(double)));
	connect (pd,SIGNAL(updateScaling(double,double,double)),
			g,SLOT(updateScaling(double,double,double)));
	connect (pd,SIGNAL(updateCones(double, int)),g,SLOT(updateCones(double, int)));
	connect (pd,SIGNAL(updateCross(double, double, bool, bool)),
					g,SLOT(updateCross(double, double, bool, bool)));

	pd->setMeshLineWidth(g->meshLineWidth());
	pd->setTransparency(g->transparency());
	pd->setDataColors(g->minDataColor(),g->maxDataColor());
	pd->setColors(g->titleColor(),g->meshColor(),g->axesColor(),g->numColor(),
				g->labelColor(), g->bgColor(),g->gridColor());

	pd->setTitle(g->plotTitle());
	pd->setTitleFont(g->titleFont());

	pd->setZoom(g->zoom());
	pd->setScaling(g->xScale(),g->yScale(),g->zScale());
	pd->setResolution(g->resolution());
	pd->showLegend(g->isLegendOn());
	pd->setAxesLabels(g->axesLabels());
	pd->setAxesTickLengths(g->axisTickLengths());
	pd->setAxesFonts(g->xAxisLabelFont(),g->yAxisLabelFont(),g->zAxisLabelFont());
	pd->setScales(g->scaleLimits());
	pd->setLabelsDistance(g->labelsDistance());
	pd->setNumbersFonts(g->numbersFont());

	if (g->coordStyle() == Qwt3D::NOCOORD)
		pd->disableAxesOptions();

	Qwt3D::PLOTSTYLE style= g->plotStyle();
	Graph3D::PointStyle pt=g->pointType();

	if ( style == Qwt3D::USER )
		{
		switch (pt)
			{
			case Graph3D::None :
			break;

			case Graph3D::Dots :
				pd->disableMeshOptions();
				pd->initPointsOptionsStack();
				pd->showPointsTab (g->pointsSize(), g->smoothPoints());
			break;

			case Graph3D::VerticalBars :
				pd->showBarsTab(g->barsRadius());
			break;

			case Graph3D::HairCross :
				pd->disableMeshOptions();
				pd->initPointsOptionsStack();
				pd->showCrossHairTab (g->crossHairRadius(), g->crossHairLinewidth(),
															g->smoothCrossHair(), g->boxedCrossHair());
			break;

			case Graph3D::Cones :
				pd->disableMeshOptions();
				pd->initPointsOptionsStack();
				pd->showConesTab(g->coneRadius(), g->coneQuality());
			break;
			}
		}
	else if ( style == Qwt3D::FILLED )
		pd->disableMeshOptions();
	else if (style == Qwt3D::HIDDENLINE || style == Qwt3D::WIREFRAME)
		pd->disableLegend();

	if (g->grids() == 0)
		pd->disableGridOptions();

	if (g->userFunction())
		pd->customWorksheetBtn(QString::null);
	else if (g->getTable())
		pd->customWorksheetBtn(tr("&Worksheet"));
	else if (g->getMatrix())
		pd->customWorksheetBtn(tr("&Matrix"));

	pd->showNormal();
	pd->setActiveWindow();
	return pd;
	}
else return 0;
}

QDialog* ApplicationWindow::showPieDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return 0;

Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	activeGraph = g;

	pieDialog* pd= new pieDialog(this,"plotDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (pd,SIGNAL(drawFrame(bool,int,const QColor&)),g,SLOT(drawCanvasFrame(bool,int,const QColor& )));
	connect (pd,SIGNAL(toggleCurve()),g,SLOT(removePie()));
	connect (pd,SIGNAL(updatePie(const QPen&, const Qt::BrushStyle &,int,int)),g,SLOT(updatePie(const QPen&, const Qt::BrushStyle &,int,int)));
	connect (pd,SIGNAL(worksheet(const QString&)),this,SLOT(showTable(const QString&)));

	QString curve=(g->curvesList())[0];
	pd->insertCurveName(curve);
	QPen piePen=g->pieCurvePen();

	pd->setBorderWidth(piePen.width());
	pd->setBorderColor(piePen.color());
	pd->setBorderStyle(piePen.style());
	pd->setFirstColor(g->pieFirstColor());
	pd->setPattern(g->pieBrushStyle());
	pd->setPieSize(g->pieSize());

	pd->setMultiLayerPlot(plot);
	pd->showNormal();
	pd->setActiveWindow();
	return pd;
	}
return 0;
}

void ApplicationWindow::showPlotDialog()
{
QWidget *w = ws->activeWindow();
if (!w)
	return;
QString caption=w->name();
if (plotWindows.contains(caption))
{
MultiLayer* plot = (MultiLayer*)w;
if (!plot)
	return;
Graph *g=plot->activeGraph();
if (!g)
	return;
if (g->curves()>0)
	{
	if (!g->isPiePlot())
		{
		plotDialog* pd= new plotDialog(this,"plotDialog",FALSE,WStyle_Tool|WDestructiveClose);
		pd->insertColumnsList(columnsList(Table::All));
		pd->setGraph(g);
		pd->selectCurve(0);
		pd->showNormal();
		pd->setActiveWindow();

		activeGraph = g;
		}
	else
		showPieDialog();
	}
else if (g->curves() == 0)
			QMessageBox::warning(this, tr("QtiPlot - Empty plot"),
			tr("There are actually no curves on the active layer!"));
}
else if (plot3DWindows.contains(caption))
	showPlot3dDialog();
}

void ApplicationWindow::showPlotDialog(long curveKey)
{
QWidget *w = ws->activeWindow();
if (!w)
	return;
if (plotWindows.contains(w->name()))
	{
	MultiLayer* plot = (MultiLayer*)w;
	if (!plot)
		return;
	Graph *g=plot->activeGraph();
	if (!g || g->curves() <= 0)
		return;

	if (!g->isPiePlot())
		{
		plotDialog* pd= new plotDialog(this,"plotDialog",FALSE,WStyle_Tool|WDestructiveClose);
		pd->insertColumnsList(columnsList(Table::All));
		pd->setGraph(g);
		pd->selectCurve(g->curveIndex(curveKey));
		pd->showNormal();
		pd->setActiveWindow();

		activeGraph = g;
		}
	else
		showPieDialog();
	}
}

void ApplicationWindow::zoom()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;
	
if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	btnPointer->setOn(true);
	return;
	}

Graph* g = (Graph*)plot->activeGraph();
if (!g)
	return;

if (g->isPiePlot())
	{
	if (btnZoom->isOn())
			QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));
	btnPointer->setOn(true);
	return;
	}
else
	g->zoom(true);
}

void ApplicationWindow::removePoints()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	btnPointer->setOn(true);
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if (!g)
	{
	btnPointer->setOn(true);
	return;
	}

if (g->isPiePlot())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));
	btnPointer->setOn(true);
	return;
	}
else
	{
	switch(QMessageBox::warning (this,tr("QtiPlot"),
				tr("This will modify the data in the worksheets!\nAre you sure you want to continue?"),
			tr("Continue"),tr("Cancel"),0,1))
		{
		case 0:
			g->removePoints(true);
			info->setText("Select point and double click to remove it!");
			displayBar->show();
		break;

		case 1:
			btnPointer->setOn(TRUE);
		break;
		}
	}
}

void ApplicationWindow::movePoints()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	btnPointer->setOn(true);
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if (!g)
	return;

if (g->isPiePlot())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));

	btnPointer->setOn(true);
	return;
	}
else
	{
	switch(QMessageBox::warning (this,"QtiPlot",
	"This will modify the data in the worksheets! \
	\nAre you sure you want to continue?",
	"Continue","Cancel",0,1))
		{
		case 0:
		if (g)
			{
			g->movePoints(true);
			info->setText("Please, click on plot and move cursor!");
			displayBar->show();
			}
		break;

		case 1:
			btnPointer->setOn(TRUE);
		break;
		}
	}
}		

void ApplicationWindow::print(QWidget* w)
{
if (w->isA("MultiLayer") && ((MultiLayer*)w)->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"));
	return;
	}	

((myWidget*)w)->print();
}

//print active window
void ApplicationWindow::print()
{
QWidget* w = ws->activeWindow();
if (!w)
	return;

print(w);
}

// print window from project explorer
void ApplicationWindow::printWindow()
{
WindowListItem *it = (WindowListItem *)lv->currentItem();
myWidget *w= it->window();
if (!w)
	return;

print(w);
}

void ApplicationWindow::printAllPlots()
{
QPrinter printer;
printer.setResolution(84);
printer.setOrientation(QPrinter::Landscape);
printer.setColorMode (QPrinter::Color);
printer.setFullPage(TRUE);

int plots = int(plotWindows.count());
printer.setMinMax (0, plots);
printer.setFromTo (0, plots);
	
if (printer.setup())
	{ 
	QPainter *paint = new QPainter (&printer, false);
	QWidgetList windows = ws->windowList();
	for (int i=0;i<(int)windows.count();i++)
		{
		MultiLayer *plot=(MultiLayer*)windows.at(i);
		if (plot && plotWindows.contains(plot->name()) && printer.newPage())
			plot->printAllLayers(paint);
		}
	paint->end();
	delete paint;
	}
}

void ApplicationWindow::showExpGrowthDialog()
{
showExpDecayDialog(-1);
}

void ApplicationWindow::showExpDecayDialog()
{
showExpDecayDialog(1);
}

void ApplicationWindow::showExpDecayDialog(int type)
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph* g = (Graph*)plot->activeGraph();
if (!g || !g->validCurvesDataSize())
	return;

activeGraph=g;
aw= (QWidget *)plot;

expDecayDialog *edd = new expDecayDialog(type, this,"polyDialog", false, WStyle_Tool|WDestructiveClose);
connect (plot, SIGNAL(closedWindow(QWidget*)), edd, SLOT(close()));
	
edd->setGraph(g);
edd->showNormal();
edd->setActiveWindow();
}

void ApplicationWindow::showTwoExpDecayDialog()
{
showExpDecayDialog(2);
}

void ApplicationWindow::showExpDecay3Dialog()
{
showExpDecayDialog(3);
}

void ApplicationWindow::showFitDialog()
{
QWidget *w = ws->activeWindow();
if (!w)
	return;

MultiLayer* plot = 0;
if(plotWindows.contains(w->name()))
	plot = (MultiLayer*)w;
else if(tableWindows.contains(w->name()))
	{
	Table *t = (Table *)w;
	plot = multilayerPlot(t, t->selectedColumns(), Graph::LineSymbols);
	}

if (!plot)
	return;

Graph* g = (Graph*)plot->activeGraph();
if (!g || !g->validCurvesDataSize())
	return;

activeGraph=g;
aw=(QWidget*) plot;

fitDialog *fd=new fitDialog(this,"fitDialog", false, WDestructiveClose);
connect (fd, SIGNAL(clearFunctionsList()), this, SLOT(clearFitFunctionsList()));
connect (fd, SIGNAL(saveFunctionsList(const QStringList&)), 
		 this, SLOT(saveFitFunctionsList(const QStringList&)));
connect (plot, SIGNAL(closedWindow(QWidget*)), fd, SLOT(close()));
	
fd->insertFunctionsList(fitFunctions);
fd->setGraph(g);
fd->showNormal();
fd->setActiveWindow();
}

void ApplicationWindow::lowPassFilterDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;

Graph* g = (Graph*)plot->activeGraph();
if ( g )
	{
	if (!g->curves())
		{
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("There are no curves available on this plot!"));
		return;
		}

	filterDialog *fd=new filterDialog(filterDialog::LowPass, this,"filterDialog",
										TRUE,WStyle_Tool|WDestructiveClose);	
	fd->setGraph(g);
	fd->show();
	fd->setActiveWindow();
	}
}

void ApplicationWindow::highPassFilterDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;

Graph* g = (Graph*)plot->activeGraph();
if ( g )
	{
	if (!g->curves())
		{
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("There are no curves available on this plot!"));
		return;
		}

	filterDialog *fd=new filterDialog(filterDialog::HighPass, this,"filterDialog",
										TRUE,WStyle_Tool|WDestructiveClose);	
	fd->setGraph(g);
	fd->show();
	fd->setActiveWindow();
	}
}

void ApplicationWindow::bandPassFilterDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;

Graph* g = (Graph*)plot->activeGraph();
if ( g )
	{
	if (!g->curves())
		{
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("There are no curves available on this plot!"));
		return;
		}

	filterDialog *fd=new filterDialog(filterDialog::BandPass, this,"filterDialog",
										TRUE,WStyle_Tool|WDestructiveClose);	
	fd->setGraph(g);
	fd->show();
	fd->setActiveWindow();
	}
}

void ApplicationWindow::bandBlockFilterDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;

Graph* g = (Graph*)plot->activeGraph();
if ( g )
	{
	if (!g->curves())
		{
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("There are no curves available on this plot!"));
		return;
		}

	filterDialog *fd=new filterDialog(filterDialog::BandBlock, this,"filterDialog",
										TRUE,WStyle_Tool|WDestructiveClose);	
	fd->setGraph(g);
	fd->show();
	fd->setActiveWindow();
	}
}

void ApplicationWindow::showFFTDialog()
{
QWidget *w = ws->activeWindow();
FFTDialog *sd = 0;
if (plotWindows.contains(w->name()))
	{
	MultiLayer* plot = (MultiLayer*)w;
	if (!plot)
		return;

	Graph* g = (Graph*)plot->activeGraph();
	if ( g )
		{
		if (!g->curves())
			{
			QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("There are no curves available on this plot!"));
			return;
			}

		sd=new FFTDialog(FFTDialog::onGraph, this,"smoothDialog",TRUE,WStyle_Tool|WDestructiveClose);	
		sd->setGraph(g);
		}
	}
else if (tableWindows.contains(w->name()))
	{
	Table* t = (Table*)w;
	sd=new FFTDialog(FFTDialog::onTable, this,"smoothDialog",TRUE,WStyle_Tool|WDestructiveClose);	
	sd->setTable(t);
	}

if (sd)
	{
	sd->show();
	sd->setActiveWindow();
	}
}

void ApplicationWindow::showSmoothSavGolDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;

Graph* g = (Graph*)plot->activeGraph();
if (!g || !g->validCurvesDataSize())
	return;

smoothCurveDialog *sd=new smoothCurveDialog(smoothCurveDialog::SavitzkyGolay, 
						  this,"smoothDialog",TRUE,WStyle_Tool|WDestructiveClose);	
sd->setGraph(g);
sd->show();
sd->setActiveWindow();
}

void ApplicationWindow::showSmoothFFTDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;

Graph* g = (Graph*)plot->activeGraph();
if (!g || !g->validCurvesDataSize())
	return;

smoothCurveDialog *sd=new smoothCurveDialog(smoothCurveDialog::FFT, this,"smoothDialog",TRUE,WStyle_Tool|WDestructiveClose);	
sd->setGraph(g);
sd->show();
sd->setActiveWindow();
}

void ApplicationWindow::showSmoothAverageDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;

Graph* g = (Graph*)plot->activeGraph();
if (!g || !g->validCurvesDataSize())
	return;

smoothCurveDialog *sd=new smoothCurveDialog(smoothCurveDialog::Average, this,"smoothDialog",TRUE,WStyle_Tool|WDestructiveClose);	
sd->setGraph(g);
sd->show();
sd->setActiveWindow();
}

void ApplicationWindow::showInterpolationDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;
Graph* g = (Graph*)plot->activeGraph();
if (!g || !g->validCurvesDataSize())
	return;

interpolationDialog *id=new interpolationDialog(this,"interpolationDialog",false,WStyle_Tool|WDestructiveClose);	
connect (plot, SIGNAL(closedWindow(QWidget*)), id, SLOT(close()));
id->setGraph(g);
id->show();
id->setActiveWindow();
}

void ApplicationWindow::showFitPolynomDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;
Graph* g = (Graph*)plot->activeGraph();
if (!g || !g->validCurvesDataSize())
	return;

activeGraph=g;
aw=(QWidget*)plot;

polynomFitDialog *pfd=new polynomFitDialog(this,"polyDialog",false,WStyle_Tool|WDestructiveClose);	
connect (plot, SIGNAL(closedWindow(QWidget*)), pfd, SLOT(close()));
pfd->setGraph(g);
pfd->showNormal();
pfd->setActiveWindow();
}

void ApplicationWindow::fitLinear()
{
analysis("fitLinear");
}

void ApplicationWindow::updateLog(const QString& result)
{
if ( !result.isEmpty() )
	{
	logInfo+=result;
	showResults(true);
	emit modified();

	if (! aw)
		return;

	((MultiLayer*)aw)->updateTransparency();
	aw->setFocus();
	}
}

void ApplicationWindow::showIntDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;
Graph* g = (Graph*)plot->activeGraph();
if (!g || !g->validCurvesDataSize())
	return;
	
intDialog *id=new intDialog(this,"intDialog",false,WStyle_Tool|WDestructiveClose);
connect (plot, SIGNAL(closedWindow(QWidget*)), id, SLOT(close()));
id->setGraph(g);
id->showNormal();
id->setActiveWindow();
activeGraph=g;
}

void ApplicationWindow::fitSigmoidal()
{
  analysis("fitSigmoidal");
}

void ApplicationWindow::fitGauss()
{
  analysis("fitGauss");
}

void ApplicationWindow::fitLorentz()

{
  analysis("fitLorentz");
}

void ApplicationWindow::differentiate()
{
analysis("differentiate");
}

void ApplicationWindow::showResults(bool ok)
{
if (ok)
	{
	if (!logInfo.isEmpty())
		results->setText(logInfo);
	else
		results->setText(tr("Sorry, there are no results to display!"));

	logWindow->show();
	results->scrollToBottom ();
	}
else
	logWindow->hide();
}

void ApplicationWindow::showResults(const QString& s)
{
logInfo+=s;
showResults(true);
}

void ApplicationWindow::showScreenReader()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	btnPointer->setOn(true);
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if (!g)
	{
	btnPointer->setOn(true);
	return;
	}

if (g->isPiePlot())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));
	btnPointer->setOn(true);
	return;
	}
else
	{
	activeGraph=g;
	g->showPlotPicker(true);
	info->setText(tr("Click on plot or move cursor to display coordinates!"));
	displayBar->show();
	}
}

void ApplicationWindow::showRangeSelectors()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0 )
	return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("There are no plot layers available in this window!"));
	btnPointer->setOn(true);
	return;
	}

Graph* g = (Graph*)plot->activeGraph();
if (!g)
	return;

if (!g->curves())
	{
	QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("There are no curves available on this plot!"));
	btnPointer->setOn(true);
	return;
	}
else if (g->isPiePlot())
	{
	QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));
	btnPointer->setOn(true);
	return;
	}

	
activeGraph=g;			
if (g->enableRangeSelectors(true))	
	{
	info->setText("Click or use Ctrl+arrow key to select range (arrows select active cursor)!");
	displayBar->show();
	}
}

void ApplicationWindow::showCursor()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	btnPointer->setOn(true);
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if (!g)
	return;

if (g->isPiePlot())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));

	btnPointer->setOn(true);
	return;
	}
else
	{	
	activeGraph=g;
	g->enableCursor(true);
	info->setText(tr("Click on plot to display information!"));
	displayBar->show();
	}
}

void ApplicationWindow::unzoom()
{
    MultiLayer* plot = (MultiLayer*)ws->activeWindow();
	if (!plot || plotWindows.contains(plot->name())<=0)
		return;

	if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"));
	return;
	}
	
    Graph* g = (Graph*)plot->activeGraph();
    if ( g )
		{
		g->setAutoScale();
		emit modified();
		}
}

void ApplicationWindow::newLegend()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if ( g )
	g->newLegend(plotLegendFont, legendFrameStyle);
}

void ApplicationWindow::addTimeStamp()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if ( g )
	g->addTimeStamp(plotLegendFont, legendFrameStyle);
}

void ApplicationWindow::disableAddText()
{
actionAddText->setOn(false);
showTextDialog();
}

void ApplicationWindow::addText()
{
if (!btnPointer->isOn())
	btnPointer->setOn(TRUE);

MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;
	
switch(QMessageBox::information(this,
            tr("QtiPlot - Add new layer?"),
            tr("Do you want to add the text on a new layer or on the active layer?"),
            tr("On &New Layer"), tr("On &Active Layer"), tr("&Cancel"),
            0, 2 ) )
	{
	case 0:
		plot->addTextLayer();
	break;

	case 1:
		{
		if (plot->isEmpty())
			{
			QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
				
			actionAddText->setOn(false);
			return;
			}
			
		Graph *g = (Graph*)plot->activeGraph();
		if (g)
			g->drawText(true);
		}
	break;

	case 2:
		actionAddText->setOn(false);
		return;
	break;
	}
}

void ApplicationWindow::addImage()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	QStringList list=QImage::inputFormatList ();
	QString filter="Images (*.jpg *JPG ",aux;
	int i;
	for (i=0;i<(int)list.count();i++)
		{
		aux="*."+(list[i]).lower()+" *. "+list[i] + " ";
		filter+=aux;
		}
	filter+=");;";
	for (i=0;i<(int)list.count();i++)
		{
		aux="*."+(list[i]).lower()+" *. "+list[i] +";;";
		filter+=aux;
		}

	QString fn = QFileDialog::getOpenFileName(workingDir, filter, this, 0,
			"QtiPlot - Insert image from file", 0, TRUE);
	if ( !fn.isEmpty() )
		{
		QApplication::setOverrideCursor(waitCursor);
		QPixmap photo;
		for (i=0;i<(int)list.count();i++)
				{
				if (fn.contains("."+list[i], false))
					{
					photo.load(fn,list[i],QPixmap::Color);
					break;
					}
				}

		if (fn.contains(".jpg", false))
			photo.load(fn,"JPEG",QPixmap::Color);

		g->insertImageMarker(photo,fn);
		QApplication::restoreOverrideCursor();
		}
	}
}

void ApplicationWindow::drawLine()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
		
	btnPointer->setOn(true);
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	g->drawLine(true);
	emit modified();
	}
}

void ApplicationWindow::showImageDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	ImageMarker *im= (ImageMarker *) g->selectedMarkerPtr();
	if (!im)
		return;
	
	imageDialog *id=new imageDialog(0,"imageDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (id,SIGNAL(options(int,int,int,int)),g,SLOT(updateImageMarker(int,int,int,int)));
	id->setIcon(QPixmap(logo_xpm));
	id->setOrigin(im->getOrigin());
	id->setSize(im->size());
	id->showNormal();
	id->setFocus();
	}
}

void ApplicationWindow::showLayerDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

if(plot->isEmpty())
	{
	QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("There are no plot layers available in this window."));
	return;
	}

layerDialog *id=new layerDialog(this,"layerDialog",TRUE,WStyle_Tool|WDestructiveClose);
id->setMultiLayer(plot);
id->initFonts(plotTitleFont, plotAxesFont, plotNumbersFont, plotLegendFont);
id->showNormal();
id->setActiveWindow();
}

void ApplicationWindow::showPlotGeometryDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	imageDialog *id=new imageDialog(0,"imageDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (id,SIGNAL(options(int,int,int,int)),plot,SLOT(setGraphGeometry(int,int,int,int)));
	id->setIcon(QPixmap(logo_xpm));
	id->setCaption(tr("QtiPlot - Layer Geometry"));
	id->setOrigin(g->pos());
	id->setSize(g->plotWidget()->size());
	id->showNormal();
	id->setActiveWindow();
	}
}

void ApplicationWindow::showTextDialog()
{
MultiLayer* plot= (MultiLayer*)ws->activeWindow();
if (plotWindows.contains(plot->name())<=0)
		return;

Graph *g=(Graph *)plot->activeGraph();
if ( g )
	{
	LegendMarker *m= (LegendMarker *) g->selectedMarkerPtr();
	if (!m)
		return;
	
	TextDialog *td=new TextDialog(TextDialog::TextMarker, this,"txtDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (td,SIGNAL(values(const QString&,int,int,const QFont&, const QColor&, const QColor&)),
		 g,SLOT(updateTextMarker(const QString&,int,int,const QFont&, const QColor&, const QColor&)));

	td->setIcon(QPixmap(logo_xpm));
	td->setText(m->getText());
	td->setFont(m->getFont());
	td->setTextColor(m->getTextColor());
	td->setBackgroundColor(m->backgroundColor());
	td->setBackgroundType(m->getBkgType());
	td->setAngle(m->getAngle());
	td->showNormal();
	td->setFocus();
	}
}

void ApplicationWindow::showLineDialog()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	LineMarker* lm=(LineMarker*)g->selectedMarkerPtr();
	if (!lm)
		return;

	lineDialog *ld=new lineDialog(0,"lineDialog",TRUE,WStyle_Tool|WDestructiveClose);
	connect (ld,SIGNAL(values(const QColor&,int,Qt::PenStyle,bool,bool)),
		 g,SLOT(updateLineMarker(const QColor&,int,Qt::PenStyle,bool, bool)));

	connect (ld,SIGNAL(setLineGeometry(const QPoint&,const QPoint&)),
		 g,SLOT(updateLineMarkerGeometry(const QPoint&,const QPoint&)));

	connect (ld,SIGNAL(setHeadGeometry(int, int, bool)),
		 g,SLOT(setArrowHeadGeometry(int, int, bool)));

	ld->setIcon(QPixmap(logo_xpm));
	ld->setStartPoint(lm->startPoint());
	ld->setEndPoint(lm->endPoint());
	ld->setColor(lm->color());
	ld->setWidth(lm->width());
	ld->setStyle(lm->style());
	ld->setEndArrow(lm->getEndArrow());
	ld->setStartArrow(lm->getStartArrow());
	ld->initHeadGeometry(lm->headLength(), lm->headAngle(), lm->filledArrowHead());
	ld->enableHeadTab();
	ld->showNormal();
	ld->setFocus();
	}
}

void ApplicationWindow::addColToTable()
{
Table* m = (Table*)ws->activeWindow();
if ( m )
	m->addCol();
}

void ApplicationWindow::clearSelection()
{
if(lv->hasFocus())
  {
  deleteSelectedItems();
  return;
  }

QWidget* m = (QWidget*)ws->activeWindow();
if (!m)
	return;

if (m->isA("Table"))
	((Table*)m)->clearSelection();
else if (m->isA("Matrix"))
	((Matrix*)m)->clearSelection();
else if (m->isA("MultiLayer"))
	{
	MultiLayer* plot = (MultiLayer*)m;
	if (!plot)
		return;

	Graph* g = (Graph*)plot->activeGraph();
	if (g->titleSelected())
		g->removeTitle();
	else if (g->markerSelected())
		g->removeMarker();
	}
else if (m->isA("Note"))
	((Note*)m)->textWidget()->clear();
emit modified();
}

void ApplicationWindow::copySelection()
{
if(results->hasFocus())
  {
  results->copy();
  return;
  }

QWidget* m = (QWidget*)ws->activeWindow();
if (!m)
	return;

if (m->isA("Table"))
	((Table*)m)->copySelection();
else if (m->isA("Matrix"))
	((Matrix*)m)->copySelection();
else if (m->isA("MultiLayer"))
	{
	MultiLayer* plot = (MultiLayer*)m;
	if (!plot || plot->graphsNumber() == 0)
		return;

	plot->copyAllLayers();
	Graph* g = (Graph*)plot->activeGraph();
	if (g && g->markerSelected())
		copyMarker();
	else
		copyActiveLayer();
	}
else if (m->isA("Note"))
	((Note*)m)->textWidget()->copy();
}

void ApplicationWindow::cutSelection()
{
QWidget* m = (QWidget*)ws->activeWindow();
if (!m)
	return;

if (m->isA("Table"))
	((Table*)m)->cutSelection();
else if (m->isA("Matrix"))
	((Matrix*)m)->cutSelection();
else if(m->isA("MultiLayer"))
	{
	MultiLayer* plot = (MultiLayer*)m;
	if (!plot || plot->graphsNumber() == 0)
		return;

	Graph* g = (Graph*)plot->activeGraph();
	copyMarker();
	g->removeMarker();
	}
else if (m->isA("Note"))
	((Note*)m)->textWidget()->cut();

emit modified();
}

void ApplicationWindow::copyMarker()
{
QWidget* m = (QWidget*)ws->activeWindow();
MultiLayer* plot = (MultiLayer*)m;
Graph* g = (Graph*)plot->activeGraph();
if (g && g->markerSelected())
	{
	g->copyMarker();
	copiedMarkerType=g->copiedMarkerType();
	QRect rect=g->copiedMarkerRect();
	auxMrkStart=rect.topLeft();
	auxMrkEnd=rect.bottomRight();

	if (copiedMarkerType == Graph::Text)
		{
		LegendMarker *m= (LegendMarker *) g->selectedMarkerPtr();
		auxMrkText=m->getText();
		auxMrkColor=m->getTextColor();
		auxMrkFont=m->getFont();
		auxMrkBkg=m->getBkgType();
		auxMrkBkgColor=m->backgroundColor();
		}
	else if (copiedMarkerType == Graph::Arrow)
		{
		LineMarker *m=(LineMarker *) g->selectedMarkerPtr();
		auxMrkWidth=m->width();
		auxMrkColor=m->color();
		auxMrkStyle=m->style();
		startArrowOn=m->getStartArrow();
		endArrowOn=m->getEndArrow();
		arrowHeadLength=m->headLength();
		arrowHeadAngle=m->headAngle();
		fillArrowHead=m->filledArrowHead();
		}
	else if (copiedMarkerType == Graph::Image)
		{
		ImageMarker *im= (ImageMarker *) g->selectedMarkerPtr();
		auxMrkFileName=im->getFileName();
		}
	}
copiedLayer=FALSE;
}

void ApplicationWindow::pasteSelection()
{	
    QWidget* m = (QWidget*)ws->activeWindow();
	if (!m)
		return;

    if (m->isA("Table"))
		((Table*)m)->pasteSelection();
	else if (m->isA("Matrix"))
		((Matrix*)m)->pasteSelection();
	else if (m->isA("Note"))
		((Note*)m)->textWidget()->paste();
	else if (m->isA("MultiLayer"))
		{
		MultiLayer* plot = (MultiLayer*)m;
		if (!plot)
			return;
		if (copiedLayer)
			{
			QApplication::setOverrideCursor(waitCursor);

			Graph* g=plot->addLayer();
			g->copy(lastCopiedLayer);
			QPoint pos=plot->mapFromGlobal(QCursor::pos());
			plot->setGraphGeometry(pos.x(),pos.y()-20,lastCopiedLayer->width(),lastCopiedLayer->height());
			plot->connectLayer(g);

			QApplication::restoreOverrideCursor();
			}
		else
			{
			if (plot->graphsNumber() == 0)
				return;

			Graph* g = (Graph*)plot->activeGraph();
			if (!g)
				return;
	
			g->setCopiedMarkerType(copiedMarkerType);
			g->setCopiedMarkerEnds(auxMrkStart,auxMrkEnd);

			if (copiedMarkerType == Graph::Text)
				g->setCopiedTextOptions(auxMrkBkg,auxMrkText,auxMrkFont,auxMrkColor, auxMrkBkgColor);
			if (copiedMarkerType == Graph::Arrow)
				g->setCopiedArrowOptions(auxMrkWidth,auxMrkStyle,auxMrkColor,startArrowOn,
										endArrowOn, arrowHeadLength,arrowHeadAngle, fillArrowHead);
			if (copiedMarkerType == Graph::Image)
				g->setCopiedImageName(auxMrkFileName);
			g->pasteMarker();
			}
		}
emit modified();
}

Table* ApplicationWindow::copyTable()
{
Table *w = 0, *m = (Table*)ws->activeWindow();
if (m)
	{
	QString caption="table"+QString::number(++tables);
	while (alreadyUsedName(caption))
		{
		tables++;
		caption="table"+QString::number(tables);
		}
	w=newTable(caption, m->tableRows(), m->tableCols());
	w->copy(m);

	QString spec=m->saveToString("geometry\n");
	w->setSpecifications(spec.replace(m->name(),caption));

	w->showNormal();
	setListViewSize(caption, m->sizeToString());
	emit modified();
	}
return w;
}

Matrix* ApplicationWindow::cloneMatrix()
{
Matrix *w = 0, *m = (Matrix*)ws->activeWindow();
if (m)
	{
	QString caption="Matrix"+QString::number(++matrixes);
	while(alreadyUsedName(caption))
		caption = "Matrix"+QString::number(++matrixes);

	int c=m->numCols();
	int r=m->numRows();
	w = newMatrix(caption,r,c);
	for (int i=0;i<r;i++)
		for (int j=0;j<c;j++)
			{
			w->setText(i, j, m->text(i,j));
			}

	w->setColumnsWidth(m->columnsWidth());
	w->setFormula(m->formula());
	w->setTextFormat(m->textFormat(), m->precision());
	w->showNormal();
	setListViewSize(caption, m->sizeToString());
	emit modified();
	}
return w;
}

Graph3D* ApplicationWindow::copySurfacePlot()
{
Graph3D* g = (Graph3D*)ws->activeWindow();
if (g && plot3DWindows.contains(g->name()))
	{
	if (!g->hasData())
		{
		QApplication::restoreOverrideCursor();
		QMessageBox::warning(this, tr("QtiPlot - Duplicate error"),
				      tr("Empty 3D surface plots can not be duplicated!"));
		return 0;
		}

	QString caption="graph"+QString::number(++graphs);
	while(alreadyUsedName(caption))
		caption="graph"+QString::number(++graphs);

	Graph3D *g2=0;
	QString s = g->formula();
	if (g->userFunction())
		{
		g2 = newPlot3D(caption,g->formula(),g->xStart(),g->xStop(),
								g->yStart(),g->yStop(),
								g->zStart(),g->zStop());
		}
	else if (s.endsWith("(Z)",TRUE))
		g2 = dataPlotXYZ(caption,s,g->xStart(),g->xStop(),
					  g->yStart(),g->yStop(),g->zStart(),g->zStop());
	else if (s.endsWith("(Y)",TRUE))
		g2 = dataPlot3D(caption, s, g->xStart(),g->xStop(),
					 g->yStart(),g->yStop(),g->zStart(),g->zStop());//Ribbon plot
	else
		g2 = openMatrixPlot3D(caption, s, g->xStart(), g->xStop(),
					 g->yStart(), g->yStop(),g->zStart(),g->zStop());

	if (!g2)
		return 0;

	Graph3D::PointStyle pt=g->pointType();
	if (g->plotStyle() == Qwt3D::USER )
		{
		switch (pt)
				{
				case Graph3D::None :
				break;

				case Graph3D::Dots :
					g2->setPointOptions(g->pointsSize(), g->smoothPoints());
				break;

				case Graph3D::VerticalBars :
					g2->setBarsRadius(g->barsRadius());
				break;

				case Graph3D::HairCross :
					g2->setCrossOptions(g->crossHairRadius(), g->crossHairLinewidth(),
										g->smoothCrossHair(), g->boxedCrossHair());
				break;

				case Graph3D::Cones :
					g2->setConesOptions(g->coneRadius(), g->coneQuality());
				break;
				}
			}
	g2->setStyle(g->coordStyle(),g->floorStyle(),g->plotStyle(),pt);
	g2->setGrid(g->grids());
	g2->setTitle(g->plotTitle(),g->titleColor(),g->titleFont());
	g2->setTransparency(g->transparency());
	g2->setDataColors(g->minDataColor(),g->maxDataColor());
	g2->setColors(g->meshColor(),g->axesColor(),g->numColor(),
				g->labelColor(), g->bgColor(),g->gridColor());
	g2->setAxesLabels(g->axesLabels());
	g2->setTicks(g->scaleTicks());
	g2->setTickLengths(g->axisTickLengths());
	g2->setOptions(g->isLegendOn(), g->resolution(),g->labelsDistance());
	g2->setNumbersFont(g->numbersFont());
	g2->setXAxisLabelFont(g->xAxisLabelFont());
	g2->setYAxisLabelFont(g->yAxisLabelFont());
	g2->setZAxisLabelFont(g->zAxisLabelFont());
	g2->setRotation(g->xRotation(),g->yRotation(),g->zRotation());
	g2->setZoom(g->zoom());
	g2->setScale(g->xScale(),g->yScale(),g->zScale());
	g2->setShift(g->xShift(),g->yShift(),g->zShift());
	g2->setMeshLineWidth((int)g->meshLineWidth());
	g2->update();
	customToolBars((QWidget*)g2);

	setListViewSize(caption, g->sizeToString());
	return g2;
	}
else
	return 0;
}

MultiLayer* ApplicationWindow::copyGraph()
{
MultiLayer* plot2=0;
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (plot &&  plotWindows.contains(plot->name()))
	{
	QString caption="graph"+QString::number(++graphs);
	while(alreadyUsedName(caption))
		caption="graph"+QString::number(++graphs);

	plot2=multilayerPlot(caption);
	plot2->showNormal();
	plot2->resize(plot->size());
	plot2->setSpacing(plot->rowsSpacing(), plot->colsSpacing());
	plot2->setAlignement(plot->horizontalAlignement(), plot->verticalAlignement());
	plot2->setMargins(plot->leftMargin(), plot->rightMargin(), 
					  plot->topMargin(), plot->bottomMargin());

	QWidgetList *graphsList=plot->graphPtrs();
	for (int j=0;j<(int)graphsList->count();j++)
		{
		Graph* g=(Graph*)graphsList->at(j);
		Graph* g2=plot2->addLayer(g->pos().x(), g->pos().y(), g->width(), g->height());
		g2->setIgnoreResizeEvents(true);
		g2->setAutoscaleFonts(false);
		g2->copy(g);
		g2->updateScale();
		plot2->connectLayer(g2);
		g2->setIgnoreResizeEvents(!autoResizeLayers);
		g2->setAutoscaleFonts(autoScaleFonts);
		}

	setListViewSize(caption, plot->sizeToString());
	}
return plot2;
}

myWidget* ApplicationWindow::copyWindow()
{
myWidget* w=0;
myWidget* g = (myWidget*)ws->activeWindow();
if (!g)
	{
	QMessageBox::critical(this,tr("QtiPlot - Duplicate window error"),
		tr("There are no windows available in this project!"));
	return w;
	}

QApplication::setOverrideCursor(waitCursor);

if (g->isA("MultiLayer"))
	w = copyGraph();
else if (g->isA("Table"))
	w = copyTable();
else if (g->isA("Graph3D"))
	w = copySurfacePlot();
else if (g->isA("Matrix"))
	w = cloneMatrix();
else if (g->isA("Note"))
	{
	w = newNote();
	if (w)
		((Note*)w)->setText(((Note*)g)->text());
	}

if (w)
	{
    if (g->isA("MultiLayer"))
		{
 		((MultiLayer*)w)->updateTransparency();
		if (g->status() == myWidget::Maximized)
			w->showMaximized();
		}
	else if (g->isA("Graph3D"))
		{
 		((Graph3D*)w)->setIgnoreFonts(true);
		if (g->status() == myWidget::Maximized)
			{
			g->showNormal();
			g->resize(500,400);
			w->resize(g->size());
			w->showMaximized();
			}
		else
			w->resize(g->size());
		((Graph3D*)w)->setIgnoreFonts(false);
		}
	else
		w->resize(g->size());

	w->setWindowLabel(g->windowLabel());
	w->setCaptionPolicy(g->captionPolicy());
	setListViewLabel(w->name(), g->windowLabel());
	}

QApplication::restoreOverrideCursor();
return w;
}

void ApplicationWindow::undo()
{
if (!lastModified)
	return;

QApplication::setOverrideCursor(waitCursor);

if (lastModified->isA("Table"))
	{
	Table *t= (Table *)lastModified;
	t->setNewSpecifications();
	QString newCaption=t->oldCaption();
	QString name=lastModified->name();
	if (newCaption != name)
		{
		int id=tableWindows.findIndex(name);
		tableWindows[id]=newCaption;
		updateTableNames(name,newCaption);
		renameListViewItem(name,newCaption);
		}

	t->restore(t->getSpecifications());
	actionUndo->setEnabled(FALSE);
	actionRedo->setEnabled(TRUE);
	}
else if (lastModified->isA("Note"))
	{
	((Note*)lastModified)->textWidget()->undo();
	actionUndo->setEnabled(FALSE);
	actionRedo->setEnabled(TRUE);
	}

QApplication::restoreOverrideCursor();
}

void ApplicationWindow::redo()
{
if (!lastModified)
	return;

QApplication::setOverrideCursor(waitCursor);
if (lastModified->isA("Table"))
	{
	Table *t= (Table *)lastModified;
	QString newCaption=t->newCaption();
	QString name=lastModified->name();
	if (newCaption != name)
		{
		int id=tableWindows.findIndex(name);
		tableWindows[id]=newCaption;
		updateTableNames(name,newCaption);
		renameListViewItem(name,newCaption);
		}
	t->restore(t->getNewSpecifications());
	actionUndo->setEnabled(TRUE);
	actionRedo->setEnabled(FALSE);
	}
else if (lastModified->isA("Note"))
	{
	((Note*)lastModified)->textWidget()->redo();
	actionUndo->setEnabled(TRUE);
	actionRedo->setEnabled(FALSE);
	}
QApplication::restoreOverrideCursor();
}

bool ApplicationWindow::hidden(QWidget* window)
{
if (hiddenWindows->containsRef (window) || outWindows->containsRef (window))
	return TRUE;

return FALSE;
}

void ApplicationWindow::updateWindowStatus(myWidget* w)
{
setListView(w->name(), w->aspect());

if (w->status() == myWidget::Maximized)
	{//set any other window having status = Maximized to status = Normal
	QPtrList <myWidget> lst = current_folder->windowsList();
	if (!lst.containsRef (w))
		return;

	myWidget *aw;
	for (aw = lst.first(); aw ; aw = lst.next())
		{
		if (aw != w && aw->status() == myWidget::Maximized)
			{
			aw->setNormal();
			return;
			}
		}
	}
}

void ApplicationWindow::resizeActiveWindow()
{
QWidget *w=(QWidget *)ws->activeWindow();
if (!w)
	return;

imageDialog *id=new imageDialog(this,"imageDialog",TRUE,WStyle_Tool|WDestructiveClose);
connect (id,SIGNAL(options(int,int,int,int)),w->parentWidget(),SLOT(setGeometry(int,int,int,int)));

id->setCaption(tr("QtiPlot - Window Geometry"));
id->setOrigin(w->parentWidget()->pos());
id->setSize(w->parentWidget()->size());
id->showNormal();
id->setActiveWindow();
}

void ApplicationWindow::hideActiveWindow()
{
myWidget *w=(myWidget *)ws->activeWindow();
if (!w)
	return;

hideWindow(w);
}

void ApplicationWindow::hideWindow(myWidget* w)
{
hiddenWindows->append(w);
w->setHidden();
emit modified();
}

void ApplicationWindow::hideWindow()
{
WindowListItem *it = (WindowListItem *)lv->currentItem();
myWidget *w= it->window();
if (!w)
	return;

hideWindow(w);
}

void ApplicationWindow::resizeWindow()
{
WindowListItem *it = (WindowListItem *)lv->currentItem();
myWidget *w= it->window();
if (!w)
	return;

imageDialog *id=new imageDialog(this,"imageDialog",TRUE,WStyle_Tool|WDestructiveClose);
connect (id,SIGNAL(options(int,int,int,int)),w->parentWidget(),SLOT(setGeometry(int,int,int,int)));

id->setCaption(tr("QtiPlot - Window Geometry"));
id->setOrigin(w->parentWidget()->pos());
id->setSize(w->parentWidget()->size());
id->showNormal();
id->setActiveWindow();
}

void ApplicationWindow::activateWindow()
{
WindowListItem *it = (WindowListItem *)lv->currentItem();
activateWindow(it->window());
}

void ApplicationWindow::activateWindow(QWidget *w)
{
if (!w)
	return;

updateWindowLists(w);

w->showNormal();
w->setActiveWindow();
emit modified();
}

void ApplicationWindow::maximizeWindow(QListViewItem * lbi)
{
if (!lbi || lbi->rtti() == FolderListItem::ListItemType)
	return;

QWidget *w = ((WindowListItem*)lbi)->window();
if (!w)
	return;

updateWindowLists(w);
w->showMaximized();
emit modified();
}

void ApplicationWindow::maximizeWindow()
{
maximizeWindow(lv->currentItem());
}

void ApplicationWindow::minimizeWindow()
{
WindowListItem *it = (WindowListItem *)lv->currentItem();
myWidget *w= it->window();
if (!w)
	return;

updateWindowLists(w);
w->showMinimized();
emit modified();
}

void ApplicationWindow::updateWindowLists(QWidget *w)
{
if (!w)
	return;

if (hiddenWindows->containsRef(w))
	hiddenWindows->take(hiddenWindows->find (w));
else if (outWindows->containsRef(w))
	{
	outWindows->take(outWindows->find (w));		
	w->reparent (ws, WDestructiveClose, w->pos(), false);
	}
}

void ApplicationWindow::closeActiveWindow()
{
QWidget *w=(QWidget *)ws->activeWindow();
if (w)
	w->close();
}

void ApplicationWindow::removeWindowFromLists(QWidget* w)
{
QString caption = w->name();
if (w->isA("Table"))
	{
	Table* m=(Table*)w;		
	for (int i=0; i<m->tableCols(); i++)
		{
		QString name=m->colName(i);
		removeCurves(name);
		}
	tableWindows.remove(caption);
	if (w == lastModified)
		{
		actionUndo->setEnabled(FALSE);
		actionRedo->setEnabled(FALSE);
		}
	}
else if (w->isA("MultiLayer"))
	{
	MultiLayer *ml =  (MultiLayer*)w;
	Graph *g = ml->activeGraph();
		
	if (g && (g->selectorsEnabled() || g->zoomOn() || g->removePointActivated() ||
		g->movePointsActivated() || g->enabledCursor()|| g->pickerActivated()))
		{
		btnPointer->setOn(true);
		activeGraph = 0;
		}	
	plotWindows.remove(caption);
	}	
else if (w->isA("Graph3D"))
	plot3DWindows.remove(caption);
else if (w->isA("Matrix"))
	{
	remove3DMatrixPlots((Matrix*)w);
	matrixWindows.remove(caption);
	}
else if (w->isA("Note"))
	noteWindows.remove(caption);

if (hiddenWindows->containsRef(w))
	hiddenWindows->take(hiddenWindows->find(w));
else if (outWindows->containsRef(w))
	outWindows->take(outWindows->find(w));
}

void ApplicationWindow::closeWindow(QWidget* window)
{
if (!window)
	return;
	
removeWindowFromLists(window);
current_folder->removeWindow((myWidget*)window);

emit modified();
emit windowClosed(window->name());
delete window;
}

void ApplicationWindow::about()
{
QString version = "QtiPlot " + QString::number(majVersion) + "." +
				  QString::number(minVersion) + "." + QString::number(patchVersion);

QMessageBox::about(this,tr("About QtiPlot"),
			 tr("<h2>"+ version + "</h2>"
			 "<p><h3>Copyright(C): Ion Vasilief</h3>"
			 "<p><h3>Released: 17/05/2006</h3>"));
}

void ApplicationWindow::windowsMenuAboutToShow()
{
	QWidgetList windows = ws->windowList();
	int n=int(windows.count());	
	if (!n )
		return;

	windowsMenu->clear();
    windowsMenu->insertItem(tr("&Cascade"), ws, SLOT(cascade() ) );
    windowsMenu->insertItem(tr("&Tile"), ws, SLOT(tile() ) );
    windowsMenu->insertSeparator();
	windowsMenu->insertItem(QPixmap(next_xpm),tr("&Next"),
			   ws, SLOT(activateNextWindow()), Key_F5);
	windowsMenu->insertItem(QPixmap(prev_xpm),tr("&Previous"),
			   ws, SLOT(activatePrevWindow()), Key_F6);
	windowsMenu->insertSeparator();
	actionRename->addTo(windowsMenu);
	actionCopyWindow->addTo(windowsMenu);
	windowsMenu->insertSeparator();
	actionResizeActiveWindow->addTo(windowsMenu);
	windowsMenu->insertItem(tr("&Hide Window"),
			   this, SLOT(hideActiveWindow()));
 	windowsMenu->insertItem(QPixmap(close_xpm), tr("Close &Window"),
			   this, SLOT(closeActiveWindow()), CTRL+Key_W );

	if (n>0 && n<10)
		{
		windowsMenu->insertSeparator();
		for (int i = 0; i<n; ++i )
			{
			int id = windowsMenu->insertItem(windows.at(i)->name(),
					 this, SLOT( windowsMenuActivated( int ) ) );
			windowsMenu->setItemParameter( id, i );
			windowsMenu->setItemChecked( id, ws->activeWindow() == windows.at(i) );
			}
		}
	else if (n>=10)
		{
		windowsMenu->insertSeparator();
		for ( int i = 0; i<9; ++i )
			{
			int id = windowsMenu->insertItem(windows.at(i)->name(),
					 this, SLOT( windowsMenuActivated( int ) ) );
			windowsMenu->setItemParameter( id, i );
			windowsMenu->setItemChecked( id, ws->activeWindow() == windows.at(i) );
			}
		windowsMenu->insertSeparator();
		windowsMenu->insertItem(tr("More windows..."),this, SLOT(showMoreWindows()));
		}
}

void ApplicationWindow::showMarkerPopupMenu()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph* g = (Graph*)plot->activeGraph();

QPopupMenu markerMenu(this);

if (g->imageMarkerSelected())
	{
	markerMenu.insertItem(QPixmap(pixelProfile_xpm),tr("&View pixel line profile"),this, SLOT(pixelLineProfile()));
	markerMenu.insertItem(tr("&Intensity Matrix"),this, SLOT(intensityTable()));
	markerMenu.insertSeparator();
	}

markerMenu.insertItem(QPixmap(cut_xpm),tr("&Cut"),this, SLOT(cutSelection()));
markerMenu.insertItem(QPixmap(copy_xpm), tr("&Copy"),this, SLOT(copySelection()));
markerMenu.insertItem(QPixmap(erase_xpm), tr("&Delete"),this, SLOT(clearSelection()));
markerMenu.insertSeparator();
if (g->arrowMarkerSelected())
	markerMenu.insertItem(tr("&Properties..."),this, SLOT(showLineDialog()));
else if (g->imageMarkerSelected())
	markerMenu.insertItem(tr("&Properties..."),this, SLOT(showImageDialog()));
else
	markerMenu.insertItem(tr("&Properties..."),this, SLOT(showTextDialog()));

markerMenu.exec(QCursor::pos());
}

void ApplicationWindow::showMoreWindows()
{
if (explorerWindow->isVisible())
	QMessageBox::information(this, "QtiPlot",tr("Please use the project explorer to select a window!"));
else
	showExplorer();
}

void ApplicationWindow::windowsMenuActivated( int id )
{
QWidgetList windows = ws->windowList();
QWidget* w = windows.at( id );
    if ( w )
	{
	w->showNormal();
	w->setFocus();
	if(hidden(w))
		{
		hiddenWindows->take(hiddenWindows->find (w));
		setListView(w->name(),tr("Normal"));
		}
    }
}

void ApplicationWindow::newProject()
{
	saveSettings();

	ApplicationWindow *ed = new ApplicationWindow();
	ed->applyUserSettings();
	ed->newTable();

	if (this->isMaximized())
		ed->showMaximized();
	else
		ed->show();

	ed->saved=TRUE;
	ed->actionSaveProject->setEnabled(false);

	this->close();
}

void ApplicationWindow::modifiedProject()
{
actionSaveProject->setEnabled(TRUE);
saved=FALSE;
}

void ApplicationWindow::modifiedProject(QWidget *w)
{
actionSaveProject->setEnabled(TRUE);
saved=FALSE;

actionUndo->setEnabled(TRUE);
lastModified=w;
}

void ApplicationWindow::timerEvent ( QTimerEvent *e)
{
if (e->timerId() == savingTimerId)
	saveProject();
else
	QWidget::timerEvent(e);
}

void ApplicationWindow::dropEvent( QDropEvent* e )
{
QStringList fileNames;
if (QUriDrag::decodeLocalFiles(e, fileNames))
	{
	QStringList lst=QImage::inputFormatList();
	QStringList asciiFiles;

	for(int i = 0; i<(int)fileNames.count(); i++)
		{
		QString fn = fileNames[i];
		QFileInfo fi (fn);
		QString ext = fi.extension().lower();
		QStringList l = lst.grep(ext, false);
		if (l.count())
			loadImage(fn);
		else if ( ext == "opj" || ext == "qti")
			open(fn);
		else 
			asciiFiles << fn;
		}

	loadMultipleASCIIFiles(asciiFiles, 0);
	}
}

void ApplicationWindow::dragEnterEvent( QDragEnterEvent* e )
{
if (e->source())
	{
	e->ignore();
	return;
	}

e->accept(QUriDrag::canDecode(e));
}

void ApplicationWindow::closeEvent( QCloseEvent* ce )
{
if (!saved)
	{
	QString s= tr("Save changes to project: <p><b> %1 </b> ?").arg(projectname);
    switch( QMessageBox::information(this,"QtiPlot", s, tr("Yes"), tr("No"), 
			tr("Cancel"), 0, 1 ) )
		{
		case 0:
			saveProject();
			saveSettings();
			ce->accept();
		break;
	
		case 1:
			default:
			saveSettings();
			ce->accept();
		break;
	
		case 2:
			ce->ignore();
		break;
		}
	}
else
	{
	saveSettings();
	ce->accept();
	}
}

void ApplicationWindow::deleteSelectedItems()
{
if (folders->hasFocus() && folders->currentItem() != folders->firstChild())
	{//we never allow the user to delete the project folder item
	deleteFolder();
	return;
	}

QListViewItem *item;
QPtrList<QListViewItem> lst;
for (item = lv->firstChild(); item; item = item->nextSibling())
	{
	if (item->isSelected())
		lst.append(item);
	}

folders->blockSignals(true);
for (item = lst.first(); item; item = lst.next())
	{
	if (item->rtti() == FolderListItem::ListItemType)
		{
		Folder *f = ((FolderListItem *)item)->folder();
		if (deleteFolder(f))
			delete item; 
		}
	else
		((WindowListItem *)item)->window()->close();
	}
folders->blockSignals(false);
}

void ApplicationWindow::showListViewSelectionMenu(const QPoint &p)
{
QPopupMenu cm(this);
cm.insertItem(tr("&Delete Selection"), this, SLOT(deleteSelectedItems()), Key_F8);
cm.exec(p);
}

void ApplicationWindow::showListViewPopupMenu(const QPoint &p)
{
QPopupMenu cm(this);
QPopupMenu window(this);

actionNewTable->addTo(&window);
actionNewMatrix->addTo(&window);
actionNewNote->addTo(&window);
actionNewGraph->addTo(&window);
actionNewFunctionPlot->addTo(&window);
actionNewSurfacePlot->addTo(&window);
cm.insertItem(tr("New &Window"), &window);

cm.insertItem(QPixmap(newfolder_xpm), tr("New F&older"), this, SLOT(addFolder()), Key_F7);
cm.insertSeparator();
cm.insertItem(tr("Auto &Column Width"), lv, SLOT(adjustColumns()));
cm.exec(p);
}

void ApplicationWindow::showWindowPopupMenu(QListViewItem *it, const QPoint &p, int)
{
if (folders->isRenaming())
	return;

if (!it) 
	{
	showListViewPopupMenu(p);
	return;
	}

QListViewItem *item;
int selected = 0;
for (item = lv->firstChild(); item; item = item->nextSibling())
	{
	if (item->isSelected())
		selected++;
	
	if (selected>1)
		{
		showListViewSelectionMenu(p);
		return;
		}
	}

if (it->rtti() == FolderListItem::ListItemType)
	{
	current_folder = ((FolderListItem *)it)->folder();
	showFolderPopupMenu(it, p, false);
	return;
	}

myWidget *w= ((WindowListItem *)it)->window();
if (w)
	{
	QPopupMenu cm(this);
	QPopupMenu plots(this);
	
	actionActivateWindow->addTo(&cm);
	actionMinimizeWindow->addTo(&cm);
	actionMaximizeWindow->addTo(&cm);
	cm.insertSeparator();
	if (!hidden(w))
		actionHideWindow->addTo(&cm);
	cm.insertItem(QPixmap(close_xpm), tr("&Delete Window"), w, SLOT(close()), Key_F8);
	cm.insertSeparator();
	cm.insertItem(tr("&Rename Window"), this, SLOT(renameWindow()), Key_F2);
	actionResizeWindow->addTo(&cm);
	cm.insertSeparator();
	actionPrintWindow->addTo(&cm);
	cm.insertSeparator();
	cm.insertItem(tr("&Properties..."), this, SLOT(windowProperties()));

	if (w->isA("Table"))
		{
		QStringList graphs=dependingPlots(w->name());
		if (int(graphs.count())>0)
			{
			cm.insertSeparator();
			for (int i=0;i<int(graphs.count());i++)
				{
				if (plotWindows.contains(graphs[i]))
					plots.insertItem(graphs[i],plot(graphs[i]), SLOT(showMaximized()));
				else
					plots.insertItem(graphs[i],surfacePlot(graphs[i]), SLOT(showMaximized()));
				}
			cm.insertItem(tr("D&epending Plots"),&plots);
			}
		}
	else if (w->isA("Matrix"))
		{
		QStringList graphs=depending3DPlots((Matrix*)w);
		if (int(graphs.count())>0)
			{
			cm.insertSeparator();
			for (int i=0;i<int(graphs.count());i++)
				{
				if (plot3DWindows.contains(graphs[i]))
					plots.insertItem(graphs[i], surfacePlot(graphs[i]), SLOT(showMaximized()));
				}
			cm.insertItem(tr("D&epending 3D Plots"),&plots);
			}
		}
	else if (w->isA("MultiLayer"))
		{
		tablesDepend->clear();
		QStringList tbls=multilayerDependencies(w);
		int n = int(tbls.count());
		if (n > 0)
			{
			cm.insertSeparator();
			for (int i=0; i<n; i++)
				tablesDepend->insertItem(tbls[i], i, -1);

			cm.insertItem(tr("D&epends on"), tablesDepend);
			}
		}
	else if (w->isA("Graph3D"))
		{
		Graph3D *sp=(Graph3D*)w;
		Matrix *m = sp->getMatrix();
		QString formula = sp->formula();
		if (!formula.isEmpty())
			{
			cm.insertSeparator();
			if (formula.contains("_"))	
				{
				QStringList tl = QStringList::split("_", formula, false);
				tablesDepend->clear();
				tablesDepend->insertItem(tl[0], 0, -1);
				cm.insertItem(tr("D&epends on"), tablesDepend);
				}
			else if (m)	
				{
				plots.insertItem(m->name(), m, SLOT(showNormal()));
				cm.insertItem(tr("D&epends on"),&plots);
				}
			else
				{
				plots.insertItem(formula, w, SLOT(showNormal()));
				cm.insertItem(tr("Function"), &plots);
				}
			}
		}
	cm.exec(p);
	}
}

void ApplicationWindow::showTable(int i)
{
Table *t = table(tablesDepend->text(i));
if (!t)
	return;

updateWindowLists(t);

t->showMaximized();
QListViewItem *it=lv->findItem (t->name(), 0, ExactMatch | CaseSensitive );
if (it)
	it->setText(2,tr("Maximized"));
}

void ApplicationWindow::showTable(const QString& curve)
{
Table* w=table(curve);
if (!w)
	return;

updateWindowLists(w);
w->showMaximized();
QListViewItem *it=lv->findItem (w->name(), 0, ExactMatch | CaseSensitive );
if (it)
	it->setText(2,tr("Maximized"));
emit modified();
}

QStringList ApplicationWindow::depending3DPlots(Matrix *m)
{
QWidgetList windows = ws->windowList();
int c=int(windows.count());
QStringList plots;

for (int i=0; i<c; i++)
	{
	if (plot3DWindows.contains(windows.at(i)->name()))
		{
		Graph3D *g = (Graph3D*)windows.at(i);
		if (g->getMatrix() == m)
		  	plots<<g->name();
		}
	}
return plots;
}

QStringList ApplicationWindow::dependingPlots(const QString& name)
{
QWidgetList windows = ws->windowList();
int c=int(windows.count());
QStringList onPlot, plots;

for (int i=0;i<c;i++)
	{
	if (windows.at(i)->isA("MultiLayer"))
		{
		MultiLayer *g=(MultiLayer*)windows.at(i);
		QWidgetList *graphsList=g->graphPtrs();
		for (int j=0;j<(int)graphsList->count();j++)
			{
			Graph* ag=(Graph*)graphsList->at(j);
			onPlot=ag->curvesList();
			onPlot=onPlot.grep (name,TRUE);
			if (int(onPlot.count()) && plots.contains(g->name())<=0)
		  		 plots<<g->name();
			}
		}
	else if (windows.at(i)->isA("Graph3D"))
		{
		Graph3D *g=(Graph3D*)windows.at(i);
		if ((g->formula()).contains(name,TRUE) && plots.contains(g->name())<=0)
		  	plots<<g->name();
		}
	}
return plots;
}

QStringList ApplicationWindow::multilayerDependencies(QWidget *w)
{
QStringList tables;
MultiLayer *g=(MultiLayer*)w;
QWidgetList *graphsList=g->graphPtrs();
for (int i=0; i<(int)graphsList->count(); i++)
	{
	Graph* ag=(Graph*)graphsList->at(i);
	QStringList onPlot=ag->curvesList();
	for (int j=0; j<(int)onPlot.count(); j++)
		{
		QStringList tl = QStringList::split("_", onPlot[j], false);
		if (tables.contains(tl[0])<=0)
		  	tables << tl[0];
		}
	}
return tables;
}

void ApplicationWindow::showGraphContextMenu()
{
QWidget* w = (QWidget*)ws->activeWindow();
if (!w)
	return;

if (w->isA("MultiLayer"))
	{
	MultiLayer *plot=(MultiLayer*)w;
	QPopupMenu cm(this);
	QPopupMenu exports(this);
	QPopupMenu copy(this);
	QPopupMenu prints(this);	
	QPopupMenu calcul(this);
	QPopupMenu smooth(this);
	QPopupMenu filter(this);
	QPopupMenu decay(this);
	QPopupMenu translate(this);
	QPopupMenu multiPeakMenu(this);
	
	Graph* ag = (Graph*)plot->activeGraph();

	if (ag->isPiePlot())
		cm.insertItem(tr("Re&move pie curve"),ag, SLOT(removePie()));
	else
		{
		actionShowCurvesDialog->addTo(&cm);
		
		actionTranslateVert->addTo(&translate);
		actionTranslateHor->addTo(&translate);
		calcul.insertItem(tr("&Translate"),&translate);
		calcul.insertSeparator();

		actionDifferentiate->addTo(&calcul);
		actionShowIntDialog->addTo(&calcul);
		calcul.insertSeparator();
		actionSmoothSavGol->addTo(&smooth);
		actionSmoothFFT->addTo(&smooth);
		actionSmoothAverage->addTo(&smooth);
		calcul.insertItem(tr("&Smooth"), &smooth);

		actionLowPassFilter->addTo(&filter);
		actionHighPassFilter->addTo(&filter);
		actionBandPassFilter->addTo(&filter);
		actionBandBlockFilter->addTo(&filter);
		calcul.insertItem(tr("&FFT filter"),&filter);
		calcul.insertSeparator();
		actionInterpolate->addTo(&calcul);
		actionFFT->addTo(&calcul);
		calcul.insertSeparator();
		actionFitLinear->addTo(&calcul);
		actionShowFitPolynomDialog->addTo(&calcul);
		calcul.insertSeparator();
		actionShowExpDecayDialog->addTo(&decay);
		actionShowTwoExpDecayDialog->addTo(&decay);
		actionShowExpDecay3Dialog->addTo(&decay);
		calcul.insertItem(tr("Fit E&xponential Decay"), &decay);
		actionFitExpGrowth->addTo(&calcul);
		actionFitSigmoidal->addTo(&calcul);
		actionFitGauss->addTo(&calcul);
		actionFitLorentz->addTo(&calcul);

		actionMultiPeakGauss->addTo(&multiPeakMenu);
		actionMultiPeakLorentz->addTo(&multiPeakMenu);
		calcul.insertItem(tr("Fit &Multi-peak"), &multiPeakMenu);
		calcul.insertSeparator();
		actionShowFitDialog->addTo(&calcul);
		cm.insertItem(tr("Anal&yse"), &calcul);
		}
	
	if (copiedLayer)
		{
		cm.insertSeparator();
		cm.insertItem(QPixmap(paste_xpm), tr("&Paste layer"),this, SLOT(pasteSelection()));
		}
	else if (copiedMarkerType >=0 )
		{
		cm.insertSeparator();
		if (copiedMarkerType == Graph::Text )
			cm.insertItem(QPixmap(paste_xpm),tr("&Paste text"),plot, SIGNAL(pasteMarker()));
		else if (copiedMarkerType == Graph::Arrow )
			cm.insertItem(QPixmap(paste_xpm),tr("&Paste line/arrow"),plot, SIGNAL(pasteMarker()));
		else if (copiedMarkerType == Graph::Image )
			cm.insertItem(QPixmap(paste_xpm),tr("&Paste image"),plot, SIGNAL(pasteMarker()));
		}
	cm.insertSeparator();
	copy.insertItem(tr("&Layer"), this, SLOT(copyActiveLayer()));
	copy.insertItem(tr("&Window"),plot, SLOT(copyAllLayers()));
	cm.insertItem(QPixmap(copy_xpm),tr("&Copy"), &copy);

	exports.insertItem(tr("&Layer"), this, SLOT(exportLayer()));
	exports.insertItem(tr("&Window"), this, SLOT(exportGraph()));
	cm.insertItem(tr("E&xport"),&exports);

	prints.insertItem(tr("&Layer"), plot, SLOT(printActiveLayer()));
	prints.insertItem(tr("&Window"),plot, SLOT(print()));
	cm.insertItem(QPixmap(fileprint_xpm),tr("&Print"),&prints);
	cm.insertSeparator();
	cm.insertItem(QPixmap(resize_xpm), tr("&Geometry..."), plot, SIGNAL(showGeometryDialog()));
	cm.insertItem(tr("P&roperties..."), this, SLOT(showGeneralPlotDialog()));
	cm.insertSeparator();
	cm.insertItem(QPixmap(close_xpm), tr("&Delete Layer"), plot, SLOT(confirmRemoveLayer()));
	cm.exec(QCursor::pos());
	}
}

void ApplicationWindow::showWindowContextMenu()
{
QWidget* w = (QWidget*)ws->activeWindow();
if (!w)
	return;

QPopupMenu cm(this);
QPopupMenu plot3D(this);
if (plotWindows.contains(w->name()))
	{
	MultiLayer *g=(MultiLayer*)w;
	if (copiedLayer)
		{
		cm.insertItem(QPixmap(paste_xpm),tr("&Paste Layer"),this, SLOT(pasteSelection()));
		cm.insertSeparator();
		}
	
	actionAddLayer->addTo(&cm);	
	cm.insertSeparator();
	if (g->graphsNumber() != 0)
		{
		actionDeleteLayer->addTo(&cm);
		cm.insertSeparator();
		actionShowPlotGeometryDialog->addTo(&cm);
		actionShowLayerDialog->addTo(&cm);
		cm.insertSeparator();
		}
	actionRename->addTo(&cm);
	actionCopyWindow->addTo(&cm);
	cm.insertSeparator();
	cm.insertItem(QPixmap(copy_xpm),tr("&Copy Page"), g, SLOT(copyAllLayers()));
	cm.insertItem(tr("E&xport Page"), this, SLOT(exportGraph()));
	actionPrint->addTo(&cm);
	cm.insertSeparator();
	actionCloseWindow->addTo(&cm);
	}
else if (tableWindows.contains(w->name()))
	{
	Table *t=(Table *)w;
	if (t->singleRowSelected())
		{
		cm.insertItem(QPixmap(cut_xpm),tr("Cu&t"), w, SLOT(cutSelection()));
		cm.insertItem(QPixmap(copy_xpm),tr("&Copy"), w, SLOT(copySelection()));
		cm.insertItem(QPixmap(paste_xpm),tr("&Paste"), w, SLOT(pasteSelection()));
		cm.insertSeparator();
		cm.insertItem(tr("&Insert Row"), w, SLOT(insertRow()));
		cm.insertItem(QPixmap(close_xpm), tr("&Delete Row"), w, SLOT(deleteSelectedRows()));
		cm.insertItem(QPixmap(erase_xpm),tr("Clea&r Row"), w, SLOT(clearSelection()));
		cm.insertSeparator();
		actionShowRowStatistics->addTo(&cm);
		}
	else if (t->multipleRowsSelected())
		{
		cm.insertItem(QPixmap(cut_xpm),tr("Cu&t"), w, SLOT(cutSelection()));
		cm.insertItem(QPixmap(copy_xpm),tr("&Copy"), w, SLOT(copySelection()));
		cm.insertItem(QPixmap(paste_xpm),tr("&Paste"), w, SLOT(pasteSelection()));
		cm.insertSeparator();
		cm.insertItem(QPixmap(close_xpm), tr("&Delete Rows"), w, SLOT(deleteSelectedRows()));
		cm.insertItem(QPixmap(erase_xpm),tr("Clea&r Rows"), w, SLOT(clearSelection()));
		cm.insertSeparator();
		actionShowRowStatistics->addTo(&cm);
		}
	else if (!t->singleCellSelected())
		{
		cm.insertItem(QPixmap(cut_xpm),tr("Cu&t"), w, SLOT(cutSelection()));
		cm.insertItem(QPixmap(copy_xpm),tr("&Copy"), w, SLOT(copySelection()));
		cm.insertItem(QPixmap(paste_xpm),tr("&Paste"), w, SLOT(pasteSelection()));
		cm.insertSeparator();
		cm.insertItem(QPixmap(erase_xpm),tr("Clea&r"), w, SLOT(clearSelection()));
		}
	else
		{
		actionRename->addTo(&cm);
		actionCopyWindow->addTo(&cm);
		cm.insertSeparator();
		actionShowExportASCIIDialog->addTo(&cm);
		actionPrint->addTo(&cm);
		cm.insertSeparator();
		actionCloseWindow->addTo(&cm);
		}
	}
else if (plot3DWindows.contains(w->name()))
	{
	Graph3D *g=(Graph3D*)w;
	if (!g->hasData())
		{
		cm.insertItem(tr("&Plot 3D"), &plot3D);
		actionAdd3DData->addTo(&plot3D);
		plot3D.insertItem(tr("&Matrix..."), this, SLOT(add3DMatrixPlot()));
		actionEditSurfacePlot->addTo(&plot3D);
		}
	else
		{
		if (g->getTable())
			cm.insertItem(tr("Choose &Data Set..."), this, SLOT(change3DData()));
		else if (g->getMatrix())
			cm.insertItem(tr("Choose &Matrix..."), this, SLOT(change3DMatrix()));
		else if (g->userFunction())
			actionEditSurfacePlot->addTo(&cm);
		cm.insertItem(QPixmap(erase_xpm), tr("C&lear"), g, SLOT(clearData()));
		}

	cm.insertSeparator();
	actionRename->addTo(&cm);
	actionCopyWindow->addTo(&cm);
	cm.insertSeparator();
	cm.insertItem(tr("&Copy Graph"), g, SLOT(copyImage()));
	cm.insertItem(tr("&Export"), g, SLOT(saveImage()));
	actionPrint->addTo(&cm);
	cm.insertSeparator();
	actionCloseWindow->addTo(&cm);
	}
else if (matrixWindows.contains(w->name()))
	{
	Matrix *t=(Matrix *)w;
	cm.insertItem(QPixmap(cut_xpm),tr("Cu&t"), t, SLOT(cutSelection()));
	cm.insertItem(QPixmap(copy_xpm),tr("&Copy"), t, SLOT(copySelection()));
	cm.insertItem(QPixmap(paste_xpm),tr("&Paste"), t, SLOT(pasteSelection()));
	cm.insertSeparator();
	if (t->rowsSelected())
		{
		cm.insertItem(tr("&Insert Row"), t, SLOT(insertRow()));
		cm.insertItem(QPixmap(close_xpm), tr("&Delete Rows"), t, SLOT(deleteSelectedRows()));
		}
	else if (t->columnsSelected())
		{
		cm.insertItem(tr("&Insert Column"), t, SLOT(insertColumn()));
		cm.insertItem(QPixmap(close_xpm), tr("&Delete Columns"), t, SLOT(deleteSelectedColumns()));
		}
	cm.insertItem(QPixmap(erase_xpm),tr("Clea&r"), t, SLOT(clearSelection()));
	}
cm.exec(QCursor::pos());
}

void ApplicationWindow::chooseHelpFolder()
{
QString dir = QFileDialog::getExistingDirectory(
                    qApp->applicationDirPath(), this,
                    "get help directory",
                    "Choose the location of the QtiPlot help folder!",
                    true, true );

if (!dir.isEmpty())
	{
	helpFilePath = dir + "/index.html";

	QFile helpFile(helpFilePath);
	if (!helpFile.exists())
		{
		QMessageBox::critical(this, tr("QtiPlot - index.html File Not Found!"),
			   tr("There is no file called <b>index.html</b> in this folder.<br>Please choose another folder!"));
		}
	}
}

void ApplicationWindow::showHelp()
{
	QMainWindow *helpWindow= new QMainWindow(0, "browser",WDestructiveClose);
	browser=new QTextBrowser (helpWindow,"helpBrowse");
	helpWindow->setFocus();
	helpWindow->setCentralWidget(browser);

	QToolBar* toolbar = new QToolBar( helpWindow );
    helpWindow->addToolBar( toolbar, "Toolbar");
    QToolButton* button;

	button = new QToolButton(QPixmap(fileprint_xpm), tr("Print"), "", this, SLOT(printHelp()), toolbar );
    button = new QToolButton(QPixmap(back_xpm), tr("Backward"), "", browser, SLOT(backward()), toolbar );
    connect( browser, SIGNAL( backwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
	button = new QToolButton(QPixmap(forward_xpm), tr("Forward"), "", browser, SLOT(forward()), toolbar );
    connect( browser, SIGNAL( forwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
    button = new QToolButton(QPixmap(home_xpm), tr("Home"), "", browser, SLOT(home()), toolbar );

	QString s=QDir::currentDirPath();
    browser->mimeSourceFactory()->setFilePath(s);
    browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );

	QFile helpFile(helpFilePath);
	if (!helpFile.exists())
		{
		QMessageBox::critical(this,tr("QtiPlot - Help Files Not Found!"),
			   tr("Please indicate the location of the help file!<br><br>"
			   "<p>The manual can be downloaded from the following internet address:</p>"
			   "<p><font color=blue>'http://soft.proindependent.com/manuals.html'</font></p>"));
		QString fn = QFileDialog::getOpenFileName(QDir::currentDirPath(),QString::null, this );
		if (!fn.isEmpty())
			{
			QFileInfo fi(fn);
			helpFilePath=fi.absFilePath();
			}
		else
			return;
		}		
	browser->setSource (helpFilePath);
	helpWindow->setCaption(tr("QtiPlot - Help Browser"));
	helpWindow->showMaximized();
}

void ApplicationWindow::printHelp()
{
#ifndef QT_NO_PRINTER
    QPrinter printer( QPrinter::HighResolution );
    printer.setFullPage(TRUE);
    if ( printer.setup( this ) ) {
	QPainter p( &printer );
	if( !p.isActive() ) // starting printing failed
	    return;
	QPaintDeviceMetrics metrics(p.device());
	int dpiy = metrics.logicalDpiY();
	int margin = (int) ( (2/2.54)*dpiy ); // 2 cm margins
	QRect body( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
	QSimpleRichText richText( browser->text(),
				  QFont(),
				  browser->context(),
				  browser->styleSheet(),
				  browser->mimeSourceFactory(),
				  body.height() );
	richText.setWidth( &p, body.width() );
	QRect view( body );
	int page = 1;
	do {
	    richText.draw( &p, body.left(), body.top(), view, colorGroup() );
	    view.moveBy( 0, body.height() );
	    p.translate( 0 , -body.height() );
	    p.drawText( view.right() - p.fontMetrics().width( QString::number(page) ),
			view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page) );
	    if ( view.top()  >= richText.height() )
		break;
	    printer.newPage();
	    page++;
	} while (TRUE);
    }
#endif
}

void ApplicationWindow::showPlotWizard()
{
if (tableWindows.count()>0)
	{
	plotWizard* pw = new plotWizard(this,"plotWizard",TRUE,WStyle_Tool|WDestructiveClose);
	connect (pw,SIGNAL(plot(const QStringList&)),this,SLOT(multilayerPlot(const QStringList&)));
	connect (pw,SIGNAL(plot3D(const QString&)),this,SLOT(dataPlotXYZ(const QString&)));
	connect (pw,SIGNAL(plot3DRibbon(const QString&)),this,SLOT(dataPlot3D(const QString&)));

	pw->insertTablesList(tableWindows);
	pw->setColumnsList(columnsList(Table::All));
	pw->changeColumnsList(tableWindows[0]);
	pw->show();
	pw->setActiveWindow();
	}
else
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no tables available in this project.</h4>"
					  "<p><h4>Please create a table and try again!</h4>"));
}

void ApplicationWindow::showFunctionDialog(const QString& function, int curve)
{	
if ( !activeGraph )
	return;

fDialog* fd= functionDialog();
fd->setCaption(tr("QtiPlot - Edit function"));
fd->setGraph(activeGraph);
fd->setCurveToModify(function, curve);
}

fDialog* ApplicationWindow::functionDialog()
{
fDialog* fd= new fDialog(this,"fDialog",TRUE,WStyle_Tool|WDestructiveClose);
connect (fd,SIGNAL(clearFunctionsList()),this,SLOT(clearFunctionsList()));
connect (fd,SIGNAL(clearParamFunctionsList()),this,SLOT(clearParamFunctionsList()));
connect (fd,SIGNAL(clearPolarFunctionsList()),this,SLOT(clearPolarFunctionsList()));

fd->insertFunctionsList(functions);
fd->insertParamFunctionsList(xFunctions, yFunctions);
fd->insertPolarFunctionsList(rFunctions, tetaFunctions);
fd->show();
fd->setActiveWindow();

return fd;
}

void ApplicationWindow::addFunctionCurve()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;

if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if ( g )
	{
	activeGraph=g;
	fDialog* fd = functionDialog();
	if (fd)
		fd->setGraph(g);
	}
}

void ApplicationWindow::updateFunctionLists(QString& type,QStringList &formulas)
{
int maxListSize = 10;
if (type == "Polar plot")
	{
	rFunctions.remove(formulas[0]);
	rFunctions.push_front(formulas[0]);

	tetaFunctions.remove(formulas[1]);
	tetaFunctions.push_front(formulas[1]);

	while ((int)rFunctions.size() > maxListSize)
		rFunctions.pop_back();
	while ((int)tetaFunctions.size() > maxListSize)
		tetaFunctions.pop_back();
	}
else if  (type == "Parametric plot")
	{
	xFunctions.remove(formulas[0]);
	xFunctions.push_front(formulas[0]);

	yFunctions.remove(formulas[1]);
	yFunctions.push_front(formulas[1]);

	while ((int)xFunctions.size() > maxListSize)
		xFunctions.pop_back();
	while ((int)yFunctions.size() > maxListSize)
		yFunctions.pop_back();
	}
else if (type == "Function")
	{
	functions.remove(formulas[0]);
	functions.push_front(formulas[0]);

	while ((int)functions.size() > maxListSize)
		functions.pop_back();
	}
}

void ApplicationWindow::newFunctionPlot()
{
fDialog* fd = functionDialog();
if (fd)
	connect (fd,SIGNAL(newFunctionPlot(QString&,QStringList &,QStringList &,QValueList<double> &,QValueList<int> &)),
		this,SLOT(newFunctionPlot(QString&,QStringList &,QStringList &,QValueList<double> &,QValueList<int> &)));

}

void ApplicationWindow::newFunctionPlot(QString& type,QStringList &formulas,QStringList &vars,QValueList<double> &ranges,QValueList<int> &points)
{
QString label="graph"+QString::number(++graphs);
while(alreadyUsedName(label)){
	label="graph"+QString::number(++graphs);}

MultiLayer* plot = multilayerPlot(label);
Graph* g=plot->addLayer();
customGraph(g);
g->addFunctionCurve(type,formulas,vars,ranges,points);
g->newLegend(plotLegendFont, legendFrameStyle);

plot->showNormal();
setListViewSize(plot->name(), plot->sizeToString());

updateFunctionLists(type, formulas);
}

void ApplicationWindow::clearLogInfo()
{
if (!logInfo.isEmpty())
	{
	logInfo="";
	results->setText(logInfo);
	emit modified();
	}
}

void ApplicationWindow::clearParamFunctionsList()
{
xFunctions.clear();
yFunctions.clear();
}

void ApplicationWindow::clearPolarFunctionsList()
{
rFunctions.clear();
tetaFunctions.clear();
}

void ApplicationWindow::clearFunctionsList()
{
functions.clear();
}

void ApplicationWindow::clearFitFunctionsList()
{
fitFunctions.clear();
}

void ApplicationWindow::saveFitFunctionsList(const QStringList& l)
{
fitFunctions = l;
}

void ApplicationWindow::clearSurfaceFunctionsList()
{
surfaceFunc.clear();
}

void ApplicationWindow::setFramed3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setFramed();
	actionShowAxisDialog->setEnabled(TRUE);
	}
}

void ApplicationWindow::setBoxed3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setBoxed();
	actionShowAxisDialog->setEnabled(TRUE);
	}
}

void ApplicationWindow::removeAxes3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setNoAxes();
	actionShowAxisDialog->setEnabled(FALSE);
	}
}

void ApplicationWindow::removeGrid3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setNoGrid();
	}
}

void ApplicationWindow::setHiddenLineGrid3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setHiddenLineGrid();
	}
}

void ApplicationWindow::setPoints3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setPointsMesh();
	}
}

void ApplicationWindow::setCones3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setConesMesh();
	}
}

void ApplicationWindow::setCrosses3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setCrossMesh();
	}
}

void ApplicationWindow::setBars3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setBarsPlot();
	}
}

void ApplicationWindow::setLineGrid3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setLineGrid();
	}
}

void ApplicationWindow::setFilledMesh3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setFilledMesh();
	}
}

void ApplicationWindow::setFloorData3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setFloorData();
	}
}

void ApplicationWindow::setFloorIso3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setFloorIsolines();
	}
}

void ApplicationWindow::setEmptyFloor3DPlot()
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setEmptyFloor();
	}
}

void ApplicationWindow::setFrontGrid3DPlot(bool on)
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setFrontGrid(on);

	}
}

void ApplicationWindow::setBackGrid3DPlot(bool on)
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setBackGrid(on);
	}
}

void ApplicationWindow::setFloorGrid3DPlot(bool on)
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setFloorGrid(on);
	}
}

void ApplicationWindow::setCeilGrid3DPlot(bool on)
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setCeilGrid(on);
	}
}

void ApplicationWindow::setRightGrid3DPlot(bool on)
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setRightGrid(on);
	}
}

void ApplicationWindow::setLeftGrid3DPlot(bool on)
{
QWidget* g = (QWidget*)ws->activeWindow();
if (g &&  plot3DWindows.contains(g->name()))
	{
	Graph3D* plot= (Graph3D*)g;
	plot->setLeftGrid(on);
	}
}

void ApplicationWindow::pickPlotStyle( QAction* action )
{
	if (!action )
		return;

	if (action == polygon)
	{
		removeGrid3DPlot();
	}
	else if (action == filledmesh)
	{
		setFilledMesh3DPlot();
	}
	else if (action == wireframe)
	{
		setLineGrid3DPlot();
	}
	else if (action == hiddenline)
	{
		setHiddenLineGrid3DPlot();
	}
	else if (action == pointstyle)
	{
		setPoints3DPlot();
	}
	else if (action == conestyle)
	{
		setCones3DPlot();
	}
	else if (action == crossHairStyle)
	{
		setCrosses3DPlot();
	}
	else if (action == barstyle)
	{
		setBars3DPlot();
	}
	emit modified();
}


void ApplicationWindow::pickCoordSystem( QAction* action)
{
	if (!action)
		return;

	if (action == Box || action == Frame)
	{
		if (action == Box)
			setBoxed3DPlot();
		if (action == Frame)
			setFramed3DPlot();
		grids->setEnabled(true);
	}
	else if (action == None)
	{
		removeAxes3DPlot();
		grids->setEnabled(false);
	}

	emit modified();
}

void ApplicationWindow::pickFloorStyle( QAction* action )
{
	if (!action)
		return;

	if (action == floordata)
	{
	setFloorData3DPlot();
	}
	else if (action == flooriso)
	{
	setFloorIso3DPlot();
	}
	else
	{
	setEmptyFloor3DPlot();
	}

	emit modified();
}

void ApplicationWindow::custom3DActions(QWidget *w)
{
	if (w &&  plot3DWindows.contains(w->name()))
	{
	Graph3D* plot= (Graph3D*)w;
	switch(plot->plotStyle())
		{
		case FILLEDMESH:
			wireframe->setOn( FALSE );
			hiddenline->setOn( FALSE );
			polygon->setOn( FALSE );
			filledmesh->setOn( TRUE );
			pointstyle->setOn( FALSE );
			barstyle->setOn( FALSE );
			conestyle->setOn( FALSE );
			crossHairStyle->setOn( FALSE );
		break;

		case FILLED:
			wireframe->setOn( FALSE );
			hiddenline->setOn( FALSE );
			polygon->setOn( TRUE );
			filledmesh->setOn( FALSE );
			pointstyle->setOn( FALSE );
			barstyle->setOn( FALSE );
			conestyle->setOn( FALSE );
			crossHairStyle->setOn( FALSE );
		break;

		case Qwt3D::USER:
			wireframe->setOn( FALSE );
			hiddenline->setOn( FALSE );
			polygon->setOn( FALSE );
			filledmesh->setOn( FALSE );

			if (plot->pointType() == Graph3D::VerticalBars)
				{
				pointstyle->setOn( FALSE );
				conestyle->setOn( FALSE );
				crossHairStyle->setOn( FALSE );
				barstyle->setOn( TRUE );
				}
			else if (plot->pointType() == Graph3D::Dots)
				{
				pointstyle->setOn( TRUE );
				barstyle->setOn( FALSE );
				conestyle->setOn( FALSE );
				crossHairStyle->setOn( FALSE );
				}
			else if (plot->pointType() == Graph3D::HairCross)
				{
				pointstyle->setOn( FALSE );
				barstyle->setOn( FALSE );
				conestyle->setOn( FALSE );
				crossHairStyle->setOn( true );
				}
			else if (plot->pointType() == Graph3D::Cones)
				{
				pointstyle->setOn( FALSE );
				barstyle->setOn( FALSE );
				conestyle->setOn( true );
				crossHairStyle->setOn( FALSE );
				}
		break;

		case WIREFRAME:
			wireframe->setOn( TRUE );
			hiddenline->setOn( FALSE );
			polygon->setOn( FALSE );
			filledmesh->setOn( FALSE );
			pointstyle->setOn( FALSE );
			barstyle->setOn( FALSE );
			conestyle->setOn( FALSE );
			crossHairStyle->setOn( FALSE );
		break;

		case HIDDENLINE:
			wireframe->setOn( FALSE );
			hiddenline->setOn( TRUE );
			polygon->setOn( FALSE );
			filledmesh->setOn( FALSE );
			pointstyle->setOn( FALSE );
			barstyle->setOn( FALSE );
			conestyle->setOn( FALSE );
			crossHairStyle->setOn( FALSE );
		break;

		default:
			break;
		}

	switch(plot->coordStyle())
		{
		case Qwt3D::NOCOORD:
			None->setOn( TRUE );
			Box->setOn( FALSE );
			Frame->setOn( FALSE );
		break;

		case Qwt3D::BOX:
			None->setOn( FALSE );
			Box->setOn( TRUE );
			Frame->setOn( FALSE );
		break;

		case Qwt3D::FRAME:
			None->setOn(FALSE );
			Box->setOn( FALSE );
			Frame->setOn(TRUE );
		break;
		}

	switch(plot->floorStyle())
		{
		case NOFLOOR:
			floornone->setOn( TRUE );
			flooriso->setOn( FALSE );
			floordata->setOn( FALSE );
		break;

		case FLOORISO:
			floornone->setOn( FALSE );
			flooriso->setOn( TRUE );
			floordata->setOn( FALSE );
		break;

		case FLOORDATA:
			floornone->setOn(FALSE );
			flooriso->setOn( FALSE );
			floordata->setOn(TRUE );
		break;
		}
	custom3DGrids(plot->grids());
	}
}

void ApplicationWindow::custom3DGrids(int grids)
{
	if (Qwt3D::BACK & grids)
		back->setOn(TRUE);
	else
		back->setOn(FALSE);

	if (Qwt3D::FRONT & grids)
		front->setOn(TRUE);
	else
		front->setOn(FALSE);

	if (Qwt3D::CEIL & grids)
		ceil->setOn(TRUE);
	else
		ceil->setOn(FALSE);

	if (Qwt3D::FLOOR & grids)
		floor->setOn(TRUE);
	else
		floor->setOn(FALSE);

	if (Qwt3D::RIGHT & grids)
		right->setOn(TRUE);
	else
		right->setOn(FALSE);

	if (Qwt3D::LEFT & grids)
		left->setOn(TRUE);
	else
		left->setOn(FALSE);
}

void ApplicationWindow::initPlot3DToolBar()
{
	plot3DTools = new QToolBar( this, "plot3d operations" );
    addToolBar( plot3DTools, tr( "Surface 3D" ),Top, FALSE );

	coord = new QActionGroup( this, "coord" );
    coord->setUsesDropDown( FALSE );
    Box = new QAction( coord, "Box" );
    Box->setToggleAction( TRUE );
    Box->setIconSet(QPixmap(box_xpm));
	Box->setOn( TRUE );

    Frame = new QAction( coord, "Frame" );
    Frame->setToggleAction( TRUE );
    Frame->setIconSet(QPixmap(free_axes_xpm) );
    None = new QAction( coord, "None" );
    None->setToggleAction( TRUE );
    None->setIconSet( QPixmap(no_axes_xpm) );

	Frame->addTo( plot3DTools );
    Box->addTo( plot3DTools );
    None->addTo( plot3DTools );

	plot3DTools->addSeparator();

	// grid actions
	grids = new QActionGroup( this, "grids" );
    grids->setEnabled( TRUE );
    grids->setExclusive( FALSE );
    grids->setUsesDropDown( FALSE );
    front = new QAction( grids, "front" );
    front->setToggleAction( TRUE );
    front->setIconSet(QPixmap(frontGrid_xpm) );
    back = new QAction( grids, "back" );
    back->setToggleAction( TRUE );
    back->setIconSet(QPixmap(backGrid_xpm));
    right = new QAction( grids, "right" );
    right->setToggleAction( TRUE );
    right->setIconSet(QPixmap(leftGrid_xpm) );
    left = new QAction( grids, "left" );
    left->setToggleAction( TRUE );
    left->setIconSet( QPixmap(rightGrid_xpm));
    ceil = new QAction( grids, "ceil" );
    ceil->setToggleAction( TRUE );
    ceil->setIconSet( QPixmap(ceilGrid_xpm) );
    floor = new QAction( grids, "floor" );
    floor->setToggleAction( TRUE );
    floor->setIconSet( QPixmap(floorGrid_xpm) );

	front->addTo(plot3DTools );
    back->addTo(plot3DTools );
    right->addTo(plot3DTools );
    left->addTo(plot3DTools );
    ceil->addTo(plot3DTools );
    floor->addTo(plot3DTools );

	plot3DTools->addSeparator();

	//plot style actions
	plotstyle = new QActionGroup( this, "plotstyle" );
    plotstyle->setUsesDropDown( FALSE );
    wireframe = new QAction( plotstyle, "wireframe" );
    wireframe->setToggleAction( TRUE );
    wireframe->setEnabled( TRUE );
    wireframe->setIconSet(QPixmap(lineMesh_xpm) );
    hiddenline = new QAction( plotstyle, "hiddenline" );
    hiddenline->setToggleAction( TRUE );
    hiddenline->setEnabled( TRUE );
    hiddenline->setIconSet(QPixmap(grid_only_xpm) );
    polygon = new QAction( plotstyle, "polygon" );
    polygon->setToggleAction( TRUE );
    polygon->setEnabled( TRUE );
    polygon->setIconSet(QPixmap(no_grid_xpm));
    filledmesh = new QAction( plotstyle, "filledmesh" );
    filledmesh->setToggleAction( TRUE );
    filledmesh->setOn( TRUE );
    filledmesh->setIconSet(QPixmap(grid_poly_xpm) );
    pointstyle = new QAction( plotstyle, "pointstyle" );
    pointstyle->setToggleAction( TRUE );
    pointstyle->setIconSet( QPixmap(pointsMesh_xpm) );

 	conestyle = new QAction( plotstyle, "conestyle" );
    conestyle->setToggleAction( TRUE );
    conestyle->setIconSet( QPixmap(cones_xpm) );

	crossHairStyle = new QAction( plotstyle, "crossHairStyle" );
    crossHairStyle->setToggleAction( TRUE );
    crossHairStyle->setIconSet( QPixmap(crosses_xpm) );

	barstyle = new QAction( plotstyle, "barstyle" );
    barstyle->setToggleAction( TRUE );
    barstyle->setIconSet( QPixmap(plot_bars_xpm) );

	barstyle->addTo( plot3DTools );
	plot3DTools->addSeparator();
	pointstyle->addTo( plot3DTools );

	conestyle->addTo( plot3DTools );
	crossHairStyle->addTo( plot3DTools );
	plot3DTools->addSeparator();

    wireframe->addTo( plot3DTools );
    hiddenline->addTo( plot3DTools );
    polygon->addTo( plot3DTools );
    filledmesh->addTo( plot3DTools );

	plot3DTools->addSeparator();

	//floor actions
	floorstyle = new QActionGroup( this, "floorstyle" );
    floorstyle->setUsesDropDown( FALSE );
    floordata = new QAction( floorstyle, "floordata" );
    floordata->setToggleAction( TRUE );
    floordata->setIconSet(QPixmap(floor_xpm) );
    flooriso = new QAction( floorstyle, "flooriso" );
    flooriso->setToggleAction( TRUE );
    flooriso->setIconSet(QPixmap(isolines_xpm) );
    floornone = new QAction( floorstyle, "floornone" );
    floornone->setToggleAction( TRUE );
    floornone->setOn( TRUE );
    floornone->setIconSet(QPixmap(no_floor_xpm));

	floordata->addTo( plot3DTools );
    flooriso->addTo( plot3DTools );
    floornone->addTo( plot3DTools );

	plot3DTools->hide();

connect( coord, SIGNAL( selected( QAction* ) ), this, SLOT( pickCoordSystem( QAction* ) ) );
connect( floorstyle, SIGNAL( selected( QAction* ) ), this, SLOT( pickFloorStyle( QAction* ) ) );
connect( plotstyle, SIGNAL( selected( QAction* ) ), this, SLOT( pickPlotStyle( QAction* ) ) );

connect( left, SIGNAL( toggled( bool ) ), this, SLOT( setLeftGrid3DPlot(bool) ));
connect( right, SIGNAL( toggled( bool ) ), this, SLOT( setRightGrid3DPlot( bool ) ) );
connect( ceil, SIGNAL( toggled( bool ) ), this, SLOT( setCeilGrid3DPlot( bool ) ) );
connect( floor, SIGNAL( toggled( bool ) ), this, SLOT(setFloorGrid3DPlot( bool ) ) );
connect( back, SIGNAL( toggled( bool ) ), this, SLOT(setBackGrid3DPlot( bool ) ) );
connect( front, SIGNAL( toggled( bool ) ), this, SLOT( setFrontGrid3DPlot( bool ) ) );
}

void ApplicationWindow::pixelLineProfile()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph* g = (Graph*)plot->activeGraph();
if (g)
	{
	 bool ok;
    int res = QInputDialog::getInteger(
            "QtiPlot - Set number of pixels to average", "Number of average pixels",1, 1, 2000, 2,
            &ok, this );
    if ( ok )
		{
		QApplication::setOverrideCursor(waitCursor);
		g->calculateProfile(res,TRUE);
		QApplication::restoreOverrideCursor();
		}
	else
       return;
	}
}

void ApplicationWindow::intensityTable()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph* g = (Graph*)plot->activeGraph();
if (g)
	g->showIntensityTable();
}

Matrix* ApplicationWindow::createIntensityMatrix(const QPixmap& pic)
{
QImage image=pic.convertToImage();
QSize size=pic.size();
int cols=size.width();
int rows=size.height();

QApplication::setOverrideCursor(waitCursor);
Matrix* w = newMatrix("Matrix1", rows, cols);
for (int i=0; i<rows; i++ )
 	{
	for (int j=0; j<cols; j++)
 		{
		QRgb pixel = image.pixel (j,i);
		w->setText (i, j, QString::number(qGray(pixel)));
		}
	}

w->show();
QApplication::restoreOverrideCursor();
return w;
}

void ApplicationWindow::addLayer()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (plot &&  plotWindows.contains(plot->name())>0)
	{
	switch(QMessageBox::information(this,
            tr("QtiPlot - Guess best origin for the new layer?"),
            tr("Do you want QtiPlot to guess the best position for the new layer?\n Warning: this will rearrange existing layers!"),
            tr("&Guess"), tr("&Top-left corner"), tr("&Cancel"),
            0, 2 ) )
			{
			case 0:
				{
				customGraph(plot->addLayer());
				plot->arrangeLayers(true, false);
				}
			break;

			case 1:
				customGraph(plot->addLayerToOrigin());
				plot->updateTransparency();
			break;

			case 2:
		   		return;
			break;
			}
	}
}

void ApplicationWindow::deleteLayer()
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (plot &&  plotWindows.contains(plot->name())>0)
	plot->confirmRemoveLayer();

plot->setActiveWindow();
}

void ApplicationWindow::restoreWindowGeometry(ApplicationWindow *app, QWidget *w, const QString s)
{
QString caption = w->name();
if (s.contains ("minimized"))
	{
	w->setGeometry(0, 0, 500, 400);
	w->showMinimized();
	((myWidget *)w)->setStatus(myWidget::Minimized);
	app->setListView(caption, tr("Minimized"));
	}
else if (s.contains ("maximized"))
	{
	w->setGeometry(0, 0, 500, 400);
	if (w->isA("Graph3D"))
		((Graph3D*)w)->setIgnoreFonts(true);

	w->hide();//trick used in order to avoid a resize event
	w->showMaximized();

	if (w->isA("Graph3D"))
		((Graph3D*)w)->setIgnoreFonts(false);

	((myWidget *)w)->setStatus(myWidget::Maximized);
	app->setListView(caption, tr("Maximized"));
	}
else
	{
	QStringList lst=QStringList::split ("\t",s,TRUE);
	w->parentWidget()->setGeometry(lst[1].toInt(),lst[2].toInt(),lst[3].toInt(),lst[4].toInt());
	w->showNormal();
	((myWidget *)w)->setStatus(myWidget::Normal);

	if (lst[5] == "active")
		app->aw=(QWidget*)w;
	else if (lst[5] == "hidden")
		{
		app->hiddenWindows->append(w);
		w->hide();
		app->setListView(caption, tr("Hidden"));
		}
	else
		app->setListView(caption, tr("Normal"));
	}
}

Note* ApplicationWindow::openNote(ApplicationWindow* app, const QStringList &flist)
{
QStringList lst=QStringList::split ("\t",flist[0],false);
QString caption=lst[0];
Note* w = app->newNote(caption);
app->setListViewDate(caption, lst[1]);
w->setBirthDate(lst[1]);
if (caption.contains ("Note"))
	{
	bool ok;
	int tb=caption.remove("Note").toInt(&ok);
	if (tb > app->notes && ok) 
		app->notes = tb;
	}
restoreWindowGeometry(app, (QWidget *)w, flist[1]);

lst=QStringList::split ("\t", flist[2], true);
w->setWindowLabel(lst[1]);
w->setCaptionPolicy((myWidget::CaptionPolicy)lst[2].toInt());
app->setListViewLabel(w->name(), lst[1]);
return w;
}

Matrix* ApplicationWindow::openMatrix(ApplicationWindow* app, const QStringList &flist)
{
QStringList lst=QStringList::split ("\t",flist[0],TRUE);
QString caption=lst[0];
QString rows=lst[1];
QString cols=lst[2];

Matrix* w = app->newMatrix(caption, rows.toInt(), cols.toInt());
app->setListViewDate(caption,lst[3]);
w->setBirthDate(lst[3]);

if (caption.contains ("Matrix"))
	{
	bool ok;
	int tb=caption.remove("Matrix").toInt(&ok);
	if (tb > app->matrixes && ok) 
		app->matrixes = tb;
	}

restoreWindowGeometry(app, (QWidget *)w, flist[1]);

lst=QStringList::split ("\t",flist[2],TRUE);
w->setColumnsWidth((lst[1]).toInt());

lst=QStringList::split ("\t",flist[3],TRUE);
w->setFormula(lst[1]);

lst=QStringList::split ("\t",flist[4],TRUE);
if (lst[1] == "f")
	w->setTextFormat('f', lst[2].toInt());
else
	w->setTextFormat('e', lst[2].toInt());

if (fileVersion > 71)
	{
	lst=QStringList::split ("\t", flist[5], true);
	w->setWindowLabel(lst[1]);
	w->setCaptionPolicy((myWidget::CaptionPolicy)lst[2].toInt());
	app->setListViewLabel(w->name(), lst[1]);
	}

if (fileVersion > 81)
	{
	lst=QStringList::split ("\t", flist[6], false);
	w->setCoordinates(lst[1].toDouble(), lst[2].toDouble(), 
					  lst[3].toDouble(), lst[4].toDouble());
	}
return w;
}

Table* ApplicationWindow::openTable(ApplicationWindow* app, const QStringList &flist)
{
QStringList list=QStringList::split ("\t",flist[0],TRUE);
QString caption=list[0];
int cols=list[2].toInt();

Table* w = app->newTable(caption, list[1].toInt(),cols);
app->setListViewDate(caption,list[3]);
w->setBirthDate(list[3]);

if (caption.contains ("table"))
	{
	bool ok;
	int tb = caption.remove("table").toInt(&ok);
	if (tb > app->tables && ok) 
		app->tables = tb;	
	}
else if (caption.contains ("Fit"))
	{
	bool ok;
	int tb=caption.remove("Fit").toInt(&ok);
	if (tb > app->fitNumber && ok) 
		app->fitNumber = tb;
	}

restoreWindowGeometry(app, (QWidget *)w, flist[1]);

QString s=flist[2].right(flist[2].length()-7);
if (fileVersion >= 78)
	w->loadHeader(QStringList::split ("\t",s,FALSE ));
else
	{
	w->setColPlotDesignation(list[4].toInt(), Table::X);
	if (fileVersion > 50)
		w->setColPlotDesignation(list[6].toInt(), Table::Y);
	w->setHeader(QStringList::split ("\t",s,FALSE ));
	}
	
s=flist[3].right(flist[3].length()-9);
w->setColWidths(QStringList::split ("\t",s,FALSE ));
w->setCommandes(flist[4]);

if (fileVersion > 65)
	{
	QString t= flist[5];
	w->setColumnTypes(QStringList::split ("\t", t.remove(0,7), FALSE));
	}

if (fileVersion > 71)
	{
	list=QStringList::split ("\t", flist[6], true);
	list.remove(list.first());
	w->setColComments(list);

	list=QStringList::split ("\t", flist[7], true);
	w->setWindowLabel(list[1]);
	w->setCaptionPolicy((myWidget::CaptionPolicy)list[2].toInt());
	app->setListViewLabel(w->name(), list[1]);
	}

if (fileVersion < 69)
	{//read and set table values
	int startText = 5;
	if (fileVersion > 65)
		startText = 6;

	for (int k=startText; k<(int)flist.count()-1; k++)
		{
		list=QStringList::split("\t",flist[k],TRUE);
		int line=list[0].toInt();
		for (int i=0;i<cols;i++)
			w->setText(line,i,list[i+1]);
		}
	}
return w;
}

void ApplicationWindow::openGraph(ApplicationWindow* app, MultiLayer *plot,
									const QStringList &list)
{
Graph* ag=0;
int curveID=0,i;
Table* w=0;
QStringList fList,curve;
QString caption;
curveLayout cl;

for (int j=0;j<(int)list.count()-1;j++)
	{
	QString s=list[j];
	if (s.contains ("ggeometry"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			int x=fList[1].toInt();
			int y=fList[2].toInt();
			int width=fList[3].toInt();
			int height=fList[4].toInt();
			ag=(Graph*)plot->addLayer(x,y,width,height);
			ag->setIgnoreResizeEvents(true);
			ag->enableAutoscaling(autoscale2DPlots);
			}
	else if (s.contains ("Background"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			if (QColor(fList[1]) != QColor(255, 255, 255))
				ag->setBackgroundColor(QColor(fList[1]));
			}
	else if (s.contains ("Margin"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->plotWidget()->setMargin(fList[1].toInt());
			}
	else if (s.contains ("Border"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->setBorder(fList[1].toInt(), QColor(fList[2]));
			}
	else if (s.contains ("EnabledAxes"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->enableAxes(fList);
			}
	else if (s.contains ("AxesBaseline"))
			{
			fList=QStringList::split ("\t",s,false);
			ag->setAxesBaseline(fList);
			}
	else if (s.contains ("EnabledTicks"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->setTicksType(fList);
			}
	else if (s.contains ("TicksLength"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->setTicksLength(fList[1].toInt(), fList[2].toInt());
			}
	else if (s.contains ("EnabledTickLabels"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			for (i=0;i<(int)fList.count();i++)
				fList[i]=fList[i+1];
			ag->setEnabledTickLabels(fList);
			}
	else if (s.contains ("AxesColors"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			for (i=0;i<(int)fList.count();i++)
				fList[i]=fList[i+1];
			ag->setAxesColors(fList);
			}
	else if (s.left(5)=="grid\t")
			{
			QStringList grid=QStringList::split ("\t",s,TRUE);
			gridOptions gr;
			gr.majorOnX=grid[1].toInt();
			gr.minorOnX=grid[2].toInt();
			gr.majorOnY=grid[3].toInt();
			gr.minorOnY=grid[4].toInt();
			gr.majorCol=grid[5].toInt();
			gr.majorStyle=grid[6].toInt();
			gr.majorWidth=grid[7].toInt();
			gr.minorCol=grid[8].toInt();
			gr.minorStyle=grid[9].toInt();
			gr.minorWidth=grid[10].toInt();
			gr.xZeroOn=grid[11].toInt();
			gr.yZeroOn=grid[12].toInt();

			ag->setGridOptions(gr);
			}
	else if (s.contains ("PieCurve"))
			{
			curve=QStringList::split ("\t",s,TRUE);
			QPen pen=QPen(QColor(curve[3]),curve[2].toInt(),Graph::getPenStyle(curve[4]));
			ag->plotPie(app->table(curve[1]),curve[1],pen,curve[5].toInt(),
						curve[6].toInt(),curve[7].toInt());
			}
	else if (s.left(6)=="curve\t")
			{
			curve = QStringList::split ("\t",s,TRUE);
			if (!app->renamedTables.isEmpty())
				{
				QString caption = (curve[2]).left((curve[2]).find("_",0));
				if (app->renamedTables.contains(caption))
					{//modify the name of the curve according to the new table name
					int index = app->renamedTables.findIndex (caption);
					QString newCaption = app->renamedTables[++index];
					curve.gres(caption+"_", newCaption+"_", true);
					}
				}

			if (fileVersion <= 60)
				cl.connectType=curve[4].toInt()+1;
			else
				cl.connectType=curve[4].toInt();
			cl.lCol=curve[5].toInt();
			cl.lStyle=curve[6].toInt();
			cl.lWidth=curve[7].toInt();
			cl.sSize=curve[8].toInt();
			if (fileVersion <= 78)
				cl.sType=Graph::obsoleteSymbolStyle(curve[9].toInt());
			else
				cl.sType=curve[9].toInt();

			cl.symCol=curve[10].toInt();
			cl.fillCol=curve[11].toInt();
			cl.filledArea=curve[12].toInt();
			cl.aCol=curve[13].toInt();
			cl.aStyle=curve[14].toInt();
			if (fileVersion <= 77)
				cl.penWidth = cl.lWidth;
			else
				cl.penWidth = curve[15].toInt();

			w = app->table(curve[2]);
			if (w)
				{
				int plotType = curve[3].toInt();
				if(plotType == Graph::VectXYXY || plotType == Graph::VectXYAM)
					{
					QStringList colsList;
					colsList<<curve[2]; colsList<<curve[20]; colsList<<curve[21];
					if (fileVersion < 72)
						colsList.prepend(w->colName(curve[1].toInt()));
					else
						colsList.prepend(curve[1]);
					ag->plotVectorCurve(w, colsList, plotType);
					if (fileVersion <= 77)
						ag->updateVectorsLayout(w, curveID, curve[15].toInt(), curve[16].toInt(), curve[17].toInt(),
												curve[18].toInt(), curve[19].toInt(), 0, curve[20], curve[21]);
					else
						{
						if(plotType == Graph::VectXYXY)
							ag->setVectorsLook(curveID, QColor(curve[15]), curve[16].toInt(),
										   curve[17].toInt(), curve[18].toInt(), curve[19].toInt(),0);
						else
							ag->setVectorsLook(curveID, QColor(curve[15]), curve[16].toInt(), curve[17].toInt(),
											   curve[18].toInt(), curve[19].toInt(), curve[22].toInt());
						}
					}
				else if(plotType == Graph::Box)
					ag->openBoxDiagram(w, curve);
				else
					{
					if (fileVersion < 72)
						ag->insertCurve(w, curve[1].toInt(), curve[2], plotType);
					else
						ag->insertCurve(w, curve[1], curve[2], plotType);
					}

				if(plotType == Graph::Histogram)
					{
					if (fileVersion <= 76)
						ag->updateHistogram(w,curve[2],curveID,curve[16].toInt(),curve[17].toDouble(),curve[18].toDouble(),curve[19].toDouble());
					else
						ag->updateHistogram(w,curve[2],curveID,curve[17].toInt(),curve[18].toDouble(),curve[19].toDouble(),curve[20].toDouble());
					}

				if(plotType == Graph::VerticalBars || 
				   plotType == Graph::HorizontalBars || 
				   plotType == Graph::Histogram)
					{
					if (fileVersion <= 76)
						ag->setBarsGap(curveID, curve[15].toInt(), 0);
					else
						ag->setBarsGap(curveID, curve[15].toInt(), curve[16].toInt());
					}
				ag->updateCurveLayout(curveID,&cl);
				}
			curveID++;
			}
		else if (s.contains ("FunctionCurve"))
			{
			curve=QStringList::split ("\t",s,TRUE);

			cl.connectType=curve[6].toInt();
			cl.lCol=curve[7].toInt();
			cl.lStyle=curve[8].toInt();
			cl.lWidth=curve[9].toInt();
			cl.sSize=curve[10].toInt();
			cl.sType=curve[11].toInt();
			cl.symCol=curve[12].toInt();
			cl.fillCol=curve[13].toInt();
			cl.filledArea=curve[14].toInt();
			cl.aCol=curve[15].toInt();
			cl.aStyle=curve[16].toInt();
			if (fileVersion <= 77)
				cl.penWidth = cl.lWidth;
			else
				cl.penWidth = curve[17].toInt();

			ag->insertFunctionCurve(curve[1], curve[3].toDouble(),curve[4].toDouble(),curve[2].toInt());
			ag->setCurveType(curveID, curve[5].toInt());
			ag->updateCurveLayout(curveID, &cl);
			curveID++;
			}
		else if (s.contains ("ErrorBars"))
			{
			curve=QStringList::split ("\t",s,FALSE);
			bool plus=TRUE,minus=TRUE,through=TRUE;
			if (!curve[8].toInt()) through=FALSE;
			if (!curve[9].toInt()) plus=FALSE;
			if (!curve[10].toInt()) minus=FALSE;
			w=app->table(curve[3]);
			Table *errTable=app->table(curve[4]);
			if (w && errTable)
				ag->addErrorBars(w,curve[2],curve[3],errTable, curve[4],
							 curve[1].toInt(),curve[5].toInt(),curve[6].toInt(),
							 QColor(curve[7]),through,minus,plus);
			}
		else if (s.left(6)=="scale\t")
			{
			QStringList scale=QStringList::split ("\t",s,TRUE);
			
			if (fileVersion < 64)
				{//ensure backwards compatibility	
				QStringList newScale; 
				for (i=1; i<7; i++)
					newScale<<scale[i];
				
				QString sclType = scale[7];	
				if ( sclType == "2")
					{
					newScale<<"0";//linear scale
					newScale<<"1";//inverted scale
					}
				else if ( sclType == "3")
					{
					newScale<<"1";//log scale
					newScale<<"1";//inverted scale
					}
				else
					{
					newScale<<sclType;
					newScale<<"0";//not inverted scale
					}
					
				for (i=8; i<14; i++)
					newScale<<scale[i];	
				
				sclType = scale[14];	
				if ( sclType == "2")
					{
					newScale<<"0";//linear scale
					newScale<<"1";//inverted scale
					}
				else if ( sclType == "3")
					{
					newScale<<"1";//log scale
					newScale<<"1";//inverted scale
					}
				else
					{
					newScale<<sclType;
					newScale<<"0";//not inverted scale
					}
				ag->setScales(newScale);
				}
			else
				{
				for (i=0; i<(int)scale.count(); i++)
					scale[i]=scale[i+1];				
				ag->setScales(scale);
				}
			}
		else if (s.contains ("PlotTitle"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->setTitle(fList[1]);
			ag->setTitleColor(QColor(fList[2]));
			ag->setTitleAlignment(fList[3].toInt());
			}
		else if (s.contains ("TitleFont"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			QFont fnt=QFont (fList[1],fList[2].toInt(),fList[3].toInt(),fList[4].toInt());
			fnt.setUnderline(fList[5].toInt());
			fnt.setStrikeOut(fList[6].toInt());
			ag->setTitleFont(fnt);
			}
		else if (s.contains ("AxesTitles"))
			{
			QStringList legend=QStringList::split ("\t",s,TRUE);
			for (i=0;i<4;i++)
				ag->setAxisTitle(i,legend[i+1]);
			}
		else if (s.contains ("AxesTitleColors"))
			{
			QStringList colors=QStringList::split ("\t",s,FALSE);
			ag->setAxesTitleColor(colors);
			}
		else if (s.contains ("AxesTitleAlignment"))
			{
			QStringList align=QStringList::split ("\t",s,FALSE);
			ag->setAxesTitlesAlignment(align);
			}
		else if (s.contains ("ScaleFont"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			QFont fnt=QFont (fList[1],fList[2].toInt(),fList[3].toInt(),fList[4].toInt());
			fnt.setUnderline(fList[5].toInt());
			fnt.setStrikeOut(fList[6].toInt());

			int axis=(fList[0].right(1)).toInt();
			ag->setAxisTitleFont(axis,fnt);
			}
		else if (s.contains ("AxisFont"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			QFont fnt=QFont (fList[1],fList[2].toInt(),fList[3].toInt(),fList[4].toInt());
			fnt.setUnderline(fList[5].toInt());
			fnt.setStrikeOut(fList[6].toInt());

			int axis=(fList[0].right(1)).toInt();
			ag->setAxisFont(axis,fnt);
			}
		else if (s.contains ("AxesFormulas"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			fList.remove(fList.first());
			ag->setAxesFormulas(fList);
			}
		else if (s.contains ("LabelsFormat"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			if (fileVersion < 64)
				{//insure backwards compatibility
				for (i=0; i<4; i++)
					{			
					QString fmt = fList[2*i + 1];	
					if ( fmt == "0")
						fList[2*i + 1] = "1";
					else if ( fmt == "1")
						fList[2*i  + 1] = "2";
					else if ( fmt == "2")
						fList[2*i + 1] = "0";
					}					
				}
			fList.remove(fList.first());
			ag->setLabelsNumericFormat(fList);
			}
		else if (s.contains ("LabelsRotation"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->setAxisLabelRotation(QwtPlot::xBottom, fList[1].toInt());
			ag->setAxisLabelRotation(QwtPlot::xTop, fList[2].toInt());
			}
		else if (s.contains ("DrawAxesBackbone"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->loadAxesOptions(fList[1]);
			}
		else if (s.contains ("AxesLineWidth"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->loadAxesLinewidth(fList[1].toInt());
			}
		else if (s.contains ("CanvasFrame"))
			{
			QStringList list=QStringList::split ("\t",s,TRUE);
			ag->drawCanvasFrame(list);
			}
		else if (s.contains ("Legend"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			if (fileVersion < 71)
				ag->insertLegend_obsolete(fList);
			else
				ag->insertLegend(fList);
			}
		else if (s.contains ("textMarker"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			if (fileVersion < 71)
				ag->insertTextMarker_obsolete(fList);
			else
				ag->insertTextMarker(fList);
			}
		else if (s.contains ("lineMarker"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->insertLineMarker(fList);
			}
		else if (s.contains ("ImageMarker"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->insertImageMarker(fList);
			}
		else if (s.contains ("FitID"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			ag->setFitID(fList[1].toInt());
			}
		else if (s.contains("AxisType"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			for (i=0; i<4; i++)
				{
				QStringList lst = QStringList::split(";", fList[i+1], false);
				int format = lst[0].toInt();
				if (format == Graph::Numeric)
					ag->setLabelsNumericFormat(i, ag->labelsNumericFormat());
				else if (format == Graph::Time || format == Graph::Date)
					ag->setLabelsDateTimeFormat(i, format, lst[1]+";"+lst[2]);
				else
					{
					Table *nw = app->table(lst[1]);
					ag->setLabelsTextFormat(i, format, lst[1], nw);
					}
				}
			}
		else if (fileVersion < 69 && s.contains ("AxesTickLabelsCol"))
			{
			fList=QStringList::split ("\t",s,TRUE);
			QValueList<int> axesTypes = ag->axesType();
			for (i=0; i<4; i++)
				{
				QString colName = fList[i+1];
				Table *nw = app->table(colName);
				ag->setLabelsTextFormat(i, axesTypes[i], colName, nw);
				}
			}
	}
ag->replot();
ag->setIgnoreResizeEvents(!autoResizeLayers);
ag->setAutoscaleFonts(autoScaleFonts);
plot->connectLayer(ag);
}

Graph3D* ApplicationWindow::openSurfacePlot(ApplicationWindow* app, const QStringList &lst)
{
QStringList fList=QStringList::split ("\t",lst[0],TRUE);
QString caption=fList[0];
QString date=fList[1];
if (date.isEmpty())
	date = QDateTime::currentDateTime().toString(Qt::LocalDate);

fList=QStringList::split ("\t",lst[2],FALSE );
Graph3D *plot=0;

if (fList[1].endsWith("(Y)",TRUE))//Ribbon plot
	plot=app->dataPlot3D(caption, fList[1],fList[2].toDouble(),fList[3].toDouble(),
						fList[4].toDouble(),fList[5].toDouble(),fList[6].toDouble(),fList[7].toDouble());
else if (fList[1].contains("(Z)",TRUE) > 0)
	plot=app->dataPlotXYZ(caption, fList[1], fList[2].toDouble(),fList[3].toDouble(),
						fList[4].toDouble(),fList[5].toDouble(),fList[6].toDouble(),fList[7].toDouble());
else if (fList[1].startsWith("matrix<",TRUE) && fList[1].endsWith(">",false))
	plot=app->openMatrixPlot3D(caption, fList[1], fList[2].toDouble(),fList[3].toDouble(),
						fList[4].toDouble(),fList[5].toDouble(),fList[6].toDouble(),fList[7].toDouble());
else 
	plot=app->newPlot3D(caption, fList[1],fList[2].toDouble(),fList[3].toDouble(),
						fList[4].toDouble(),fList[5].toDouble(),
						fList[6].toDouble(),fList[7].toDouble());

if (!plot)
	return 0;

app->setListViewDate(caption,date);
plot->setBirthDate(date);
//app->setListViewSize(caption, plot->sizeToString());

if (caption.contains ("graph",TRUE))
	{
	bool ok;
	int gr=caption.remove("graph").toInt(&ok);
	if (gr > app->graphs && ok) 
		app->graphs = gr;
	}

plot->setIgnoreFonts(true);
restoreWindowGeometry(app, (QWidget *)plot, lst[1]);

fList=QStringList::split ("\t",lst[3],FALSE );
plot->setStyle(fList);

fList=QStringList::split ("\t", lst[4],FALSE );
plot->setGrid(fList[1].toInt());

fList=QStringList::split ("\t",lst[5],true );
plot->setTitle(fList);

fList=QStringList::split ("\t",lst[6],FALSE );
plot->setColors(fList);

fList=QStringList::split ("\t",lst[7],FALSE );
fList.remove(fList.first());
plot->setAxesLabels(fList);

fList=QStringList::split ("\t",lst[8],FALSE );
plot->setTicks(fList);

fList=QStringList::split ("\t",lst[9],FALSE );
plot->setTickLengths(fList);

fList=QStringList::split ("\t",lst[10],FALSE );
plot->setOptions(fList);

fList=QStringList::split ("\t",lst[11],FALSE );
plot->setNumbersFont(fList);

fList=QStringList::split ("\t",lst[12],FALSE );
plot->setXAxisLabelFont(fList);

fList=QStringList::split ("\t",lst[13],FALSE );
plot->setYAxisLabelFont(fList);

fList=QStringList::split ("\t",lst[14],FALSE );
plot->setZAxisLabelFont(fList);

fList=QStringList::split ("\t",lst[15],FALSE );
plot->setRotation(fList[1].toDouble(),fList[2].toDouble(),fList[3].toDouble());

fList=QStringList::split ("\t",lst[16],FALSE );
plot->setZoom(fList[1].toDouble());

fList=QStringList::split ("\t",lst[17],FALSE );
plot->setScale(fList[1].toDouble(),fList[2].toDouble(),fList[3].toDouble());

fList=QStringList::split ("\t",lst[18],FALSE );
plot->setShift(fList[1].toDouble(),fList[2].toDouble(),fList[3].toDouble());

if (fileVersion > 50)
	{
	fList=QStringList::split ("\t",lst[19],FALSE );
	plot->setMeshLineWidth(fList[1].toInt());
	}

if (fileVersion > 71)
	{
	fList=QStringList::split ("\t",lst[20],FALSE );
	plot->setWindowLabel(fList[1]);
	plot->setCaptionPolicy((myWidget::CaptionPolicy)fList[2].toInt());
	app->setListViewLabel(plot->name(),fList[1]);
	}

plot->update();
plot->setIgnoreFonts(true);
return plot;
}

void ApplicationWindow::copyActiveLayer()
{
copiedLayer=TRUE;

MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
	return;

Graph *g= (Graph*)plot->activeGraph();
delete lastCopiedLayer;
lastCopiedLayer = new Graph (0, 0, WDestructiveClose);
lastCopiedLayer->setGeometry(0, 0, g->width(), g->height());
lastCopiedLayer->copy(g);
g->copyImage();
}

void ApplicationWindow::showAnalysisDialog(const QString& whichFit)
{
analysisDialog *ad=new analysisDialog(0, tr("Curve:"), "analysisDialog",TRUE, WDestructiveClose);
ad->setCurveNames(activeGraph->curvesList());
ad->setOperationType(whichFit);
ad->showNormal();
ad->setActiveWindow();

connect (ad,SIGNAL(analyse(const QString&, const QString&)),this,SLOT(analyzeCurve(const QString&, const QString& )));
}

void ApplicationWindow::analyzeCurve(const QString& whichFit, const QString& curveTitle)
{
QString result="";
if (whichFit=="fitLinear")
	result=activeGraph->fitLinear(curveTitle);
else if(whichFit=="fitSigmoidal")
	result=activeGraph->fitBoltzmann(curveTitle);
else if(whichFit=="fitGauss")
	result=activeGraph->fitGauss(curveTitle);
else if(whichFit=="fitLorentz")
	result=activeGraph->fitLorentz(curveTitle);
else if(whichFit=="differentiate" && activeGraph->diffCurve(curveTitle))
	{
	Table* w=table(tableWindows.last());
	QStringList list;
	list<<QString(w->name())+"_derivative";
	MultiLayer* d=multilayerPlot(w,list,0);
	d->setFocus();
	}
if (whichFit != "differentiate" && !result.isEmpty())
	{
	logInfo+=result;
	showResults(true);
	activeGraph->setFitID(++fitNumber);
	activeGraph->setFocus();
	emit modified();
	if (!aw)
		return;
	aw->setActiveWindow();
	((MultiLayer*)aw)->updateTransparency();
	}
}

void ApplicationWindow::analysis(const QString& whichFit)
{
MultiLayer* plot = (MultiLayer*)ws->activeWindow();
if (!plot || plotWindows.contains(plot->name())<=0)
		return;
Graph* g = (Graph*)plot->activeGraph();
if (!g || !g->validCurvesDataSize())
	return;

activeGraph=g;
aw=(QWidget*)plot;
		
if (g->selectorsEnabled()) // a curve is selected
	analyzeCurve(whichFit, g->selectedCurveTitle());
else if(g->curves() == 1)
	{
	const QwtPlotCurve *c = g->curve(0);
	if (c)
		analyzeCurve(whichFit,c->title());
	}
else
	showAnalysisDialog(whichFit);
}

void ApplicationWindow::pickPointerCursor()
{
btnPointer->setOn(true);
activeGraph = 0;
}

void ApplicationWindow::disableTools()
{
if (displayBar->isVisible())
	displayBar->hide();

QWidgetList windows = ws->windowList();
for (int i=0; i<(int)windows.count(); i++)
	{
	QString caption=windows.at(i)->name();
	if (plotWindows.contains(caption))
		{
		MultiLayer* plot = (MultiLayer*)windows.at(i);
		QWidgetList *graphsList=plot->graphPtrs();
		for (int k=0; k<(int)graphsList->count(); k++)
			{
			Graph* g=(Graph*)graphsList->at(k);
			if (g)
				{
				if (g->selectorsEnabled())
					{
					g->disableRangeSelectors();
					return;
					}
				else if (g->enabledCursor())
					{
					g->enableCursor(false);
					g->replot();
					return;
					}
				else if (g->pickerActivated())
					{
					g->showPlotPicker(false);
					return;
					}
				else if (g->movePointsActivated())
					{
					g->movePoints(false);
					return;
					}
				else if (g->removePointActivated())
					{
					g->removePoints(false);
					return;
					}
				else if (g->zoomOn())
					{
					g->zoom(false);
					return;
					}
				else if (g->drawLineActive())
					{
					g->drawLine(false);
					return;
					}
				}
			}
		}
	}
}

void ApplicationWindow::pickDataTool( QAction* action )
{
if (!action)
	return;

disableTools();

if (action == btnCursor)
	showCursor();
else if (action == btnSelect)
	showRangeSelectors();
else if (action == btnPicker)
	showScreenReader();
else if (action == btnMovePoints)
	movePoints();
else if (action == btnRemovePoints)
	removePoints();
else if (action == btnZoom)
	zoom();
else if (action == btnLine)
	drawLine();
}

void ApplicationWindow::connectSurfacePlot(Graph3D *plot)
{
connect (plot,SIGNAL(showContextMenu()),this,SLOT(showWindowContextMenu()));
connect (plot,SIGNAL(showOptionsDialog()),this,SLOT(showPlot3dDialog()));
connect (plot,SIGNAL(closedWindow(QWidget*)),this, SLOT(closeWindow(QWidget*)));
connect (plot,SIGNAL(hiddenWindow(myWidget*)),this, SLOT(hideWindow(myWidget*)));
connect (plot,SIGNAL(statusChanged(myWidget*)),this, SLOT(updateWindowStatus(myWidget*)));
connect (plot,SIGNAL(modified()),this, SIGNAL(modified()));
connect (plot,SIGNAL(custom3DActions(QWidget*)),this, SLOT(custom3DActions(QWidget*)));

plot->askOnCloseEvent(confirmClosePlot3D);
}

void ApplicationWindow::connectMultilayerPlot(MultiLayer *g)
{
connect (g,SIGNAL(showTextDialog()),this,SLOT(showTextDialog()));
connect (g,SIGNAL(showPlotDialog(long)),this,SLOT(showPlotDialog(long)));
connect (g,SIGNAL(showScaleDialog(int)), this, SLOT(showScalePageFromAxisDialog(int)));
connect (g,SIGNAL(showAxisDialog(int)), this, SLOT(showAxisPageFromAxisDialog(int)));

connect (g,SIGNAL(showWindowContextMenu()),this,SLOT(showWindowContextMenu()));
connect (g,SIGNAL(showCurvesDialog()),this,SLOT(showCurvesDialog()));

connect (g,SIGNAL(drawTextOff()),this, SLOT(disableAddText()));
connect (g,SIGNAL(showXAxisTitleDialog()),this,SLOT(showXAxisTitleDialog()));
connect (g,SIGNAL(showYAxisTitleDialog()),this,SLOT(showYAxisTitleDialog()));
connect (g,SIGNAL(showRightAxisTitleDialog()),this,SLOT(showRightAxisTitleDialog()));
connect (g,SIGNAL(showTopAxisTitleDialog()),this,SLOT(showTopAxisTitleDialog()));
connect (g,SIGNAL(showMarkerPopupMenu()),this,SLOT(showMarkerPopupMenu()));
connect (g,SIGNAL(closedWindow(QWidget*)),this, SLOT(closeWindow(QWidget*)));
connect (g,SIGNAL(hiddenWindow(myWidget*)),this, SLOT(hideWindow(myWidget*)));
connect (g,SIGNAL(statusChanged(myWidget*)),this, SLOT(updateWindowStatus(myWidget*)));
connect (g,SIGNAL(cursorInfo(const QString&)),info,SLOT(setText(const QString&)));
connect (g,SIGNAL(showImageDialog()),this,SLOT(showImageDialog()));
connect (g,SIGNAL(createTablePlot(const QString&,int,int,const QString&)),this,SLOT(newWrksheetPlot(const QString&,int,int,const QString&)));
connect (g,SIGNAL(createTable(const QString&,int,int,const QString&)),this,SLOT(newHiddenTable(const QString&,int,int,const QString&)));
connect (g,SIGNAL(showPieDialog()),this,SLOT(showPieDialog()));
connect (g,SIGNAL(viewTitleDialog()),this,SLOT(showTitleDialog()));
connect (g,SIGNAL(modifiedPlot()),this,SLOT(modifiedProject()));
connect (g,SIGNAL(showLineDialog()),this,SLOT(showLineDialog()));
connect (g,SIGNAL(updateTable(const QString&,int,const QString&)),this,SLOT(updateTable(const QString&,int,const QString&)));
connect (g,SIGNAL(updateTableColumn(const QString&, double *, int)),
		 this,SLOT(updateTableColumn(const QString&, double *, int)));

connect (g,SIGNAL(clearCell(const QString&,double)),this,SLOT(clearCellFromTable(const QString&,double)));
connect (g,SIGNAL(showGeometryDialog()),this,SLOT(showPlotGeometryDialog()));
connect (g,SIGNAL(pasteMarker()),this,SLOT(pasteSelection()));
connect (g,SIGNAL(showGraphContextMenu()),this,SLOT(showGraphContextMenu()));

connect (g,SIGNAL(createIntensityTable(const QPixmap&)),
			this,SLOT(createIntensityMatrix(const QPixmap&)));
connect (g,SIGNAL(createHistogramTable(const QString&,int,int,const QString&)),
		this,SLOT(showHistogramTable(const QString&,int,int,const QString&)));
connect (g, SIGNAL(setPointerCursor()),this, SLOT(pickPointerCursor()));

g->askOnCloseEvent(confirmClosePlot2D);
}

void ApplicationWindow::connectTable(Table* w)
{
connect (w,SIGNAL(statusChanged(myWidget*)),this, SLOT(updateWindowStatus(myWidget*)));
connect (w,SIGNAL(hiddenWindow(myWidget*)),this, SLOT(hideWindow(myWidget*)));
connect (w,SIGNAL(closedWindow(QWidget*)),this, SLOT(closeWindow(QWidget*)));
connect (w,SIGNAL(removedCol(const QString&)),this,SLOT(removeCurves(const QString&)));
connect (w,SIGNAL(modifiedData(const QString&)),this,SLOT(updateCurves(const QString&)));
connect (w,SIGNAL(plotCol(Table*,const QStringList&, int)),this, SLOT(multilayerPlot(Table*,const QStringList&, int)));
connect (w,SIGNAL(modifiedWindow(QWidget*)),this,SLOT(modifiedProject(QWidget*)));
connect (w,SIGNAL(optionsDialog()),this,SLOT(showColumnOptionsDialog()));
connect (w,SIGNAL(colValuesDialog()),this,SLOT(showColumnValuesDialog()));
connect (w,SIGNAL(colMenu(int)),this,SLOT(showColMenu(int)));
connect (w,SIGNAL(showContextMenu()),this,SLOT(showWindowContextMenu()));
connect (w,SIGNAL(changedColHeader(const QString&,const QString&)),this,SLOT(updateColNames(const QString&,const QString&)));
connect (w,SIGNAL(createTable(const QString&,int,int,const QString&)),this,SLOT(newTable(const QString&,int,int,const QString&)));

//3d plots
connect( w,SIGNAL(plot3DRibbon(Table*,const QString&)),this, SLOT(dataPlot3D(Table*,const QString&)));
connect( w,SIGNAL(plotXYZ(Table*,const QString&, int)),this, SLOT(dataPlotXYZ(Table*,const QString&, int)));

w->askOnCloseEvent(confirmCloseTable);
}

void ApplicationWindow::setAppColors(const QColor& wc,const QColor& pc,const QColor& tpc)
{
if (workspaceColor != wc)
	{
	workspaceColor = wc;
	ws->setPaletteBackgroundColor (wc);
	}
	
if (panelsColor != pc)
	{
	panelsColor = pc;
	lv->setPaletteBackgroundColor (pc);
	results->setPaper ( QBrush (pc, SolidPattern ) );
		
	QPalette pal = qApp->palette();
	pal.setColor (QPalette::Active, QColorGroup::Base, QColor(panelsColor) );
	qApp->setPalette(pal, true, 0);
	}
	
if (panelsTextColor != tpc)
	{
	panelsTextColor = tpc;
	lv->setPaletteForegroundColor (tpc);
	
	results->setPaletteForegroundColor (tpc);
	if (results->isVisible())
		{
		results->hide();
		results->show();
		}
	}
}

void ApplicationWindow::setPlot3DOptions()
{
QWidgetList *windows = windowsList();
for (int i = 0; i<int(windows->count());i++ )
	{
	if (plot3DWindows.contains(windows->at(i)->name()))
		((Graph3D*)windows->at(i))->setSmoothMesh(smooth3DMesh);
	}
}

void ApplicationWindow::createActions()
{
  actionNewProject = new QAction(QPixmap(new_xpm), tr("New &Project"), tr("Ctrl+N"), this);
  connect(actionNewProject, SIGNAL(activated()), this, SLOT(newProject()));

  actionNewGraph = new QAction(QPixmap(new_graph_xpm), tr("New &Graph"), tr("Ctrl+G"), this);
  connect(actionNewGraph, SIGNAL(activated()), this, SLOT(newGraph()));

  actionNewNote = new QAction(QPixmap(new_note_xpm), tr("New &Note"), QString::null, this);
  connect(actionNewNote, SIGNAL(activated()), this, SLOT(newNote()));

  actionNewTable = new QAction(QPixmap(table_xpm), tr("New &Table"), tr("Ctrl+T"), this);
  connect(actionNewTable, SIGNAL(activated()), this, SLOT(newTable()));

  actionNewMatrix = new QAction(QPixmap(new_matrix_xpm), tr("New &Matrix"), tr("Ctrl+M"), this);
  connect(actionNewMatrix, SIGNAL(activated()), this, SLOT(newMatrix()));

  actionNewFunctionPlot = new QAction(QPixmap(newF_xpm), tr("New &Function Plot"), tr("Ctrl+F"), this);
  connect(actionNewFunctionPlot, SIGNAL(activated()), this, SLOT(newFunctionPlot()));

  actionNewSurfacePlot = new QAction(QPixmap(newFxy_xpm), tr("New &Surface 3D Plot"), tr("Ctrl+ALT+Z"), this);
  connect(actionNewSurfacePlot, SIGNAL(activated()), this, SLOT(newSurfacePlot()));

  actionOpen = new QAction(QPixmap(fileopen_xpm), tr("&Open"), tr("Ctrl+O"), this);
  connect(actionOpen, SIGNAL(activated()), this, SLOT(open()));

  actionLoadImage = new QAction(tr("Open image &file"), tr("Ctrl+I"), this);
  connect(actionLoadImage, SIGNAL(activated()), this, SLOT(loadImage()));

  actionImportImage = new QAction(tr("Import i&mage..."), QString::null, this);
  connect(actionImportImage, SIGNAL(activated()), this, SLOT(importImage()));

  actionSaveProject = new QAction(QPixmap(filesave_xpm), tr("&Save Project"), tr("Ctrl+S"), this);
  connect(actionSaveProject, SIGNAL(activated()), this, SLOT(saveProject()));
  actionSaveProject->setEnabled(FALSE);
  saved=TRUE;

  actionSaveProjectAs = new QAction(tr("Save Project &as..."), QString::null, this);
  connect(actionSaveProjectAs, SIGNAL(activated()), this, SLOT(saveProjectAs()));

  actionOpenTemplate = new QAction(QPixmap(open_template_xpm),tr("Open Temp&late..."), QString::null, this);
  connect(actionOpenTemplate, SIGNAL(activated()), this, SLOT(openTemplate()));

  actionSaveTemplate = new QAction(QPixmap(save_template_xpm), tr("Save as &Template..."), QString::null, this);
  connect(actionSaveTemplate, SIGNAL(activated()), this, SLOT(saveAsTemplate()));

  actionLoad = new QAction(QPixmap(import_xpm), tr("&Single file..."), QString::null, this);
  connect(actionLoad, SIGNAL(activated()), this, SLOT(loadASCII()));

  actionLoadMultiple = new QAction(QPixmap(multiload_xpm), tr("&Multiple files..."), QString::null, this);
  connect(actionLoadMultiple, SIGNAL(activated()), this, SLOT(loadMultiple()));

  actionUndo = new QAction(QPixmap(undo_xpm), tr("&Undo"), tr("Ctrl+Z"), this);
  connect(actionUndo, SIGNAL(activated()), this, SLOT(undo()));
  actionUndo->setEnabled(FALSE);

  actionRedo = new QAction(QPixmap(redo_xpm), tr("&Redo"), tr("Ctrl+R"), this);
  connect(actionRedo, SIGNAL(activated()), this, SLOT(redo()));
  actionRedo->setEnabled(FALSE);

  actionCopyWindow = new QAction(QPixmap(duplicate_xpm), tr("&Duplicate"), QString::null, this);
  connect(actionCopyWindow, SIGNAL(activated()), this, SLOT(copyWindow()));

  actionCutSelection = new QAction(QPixmap(cut_xpm), tr("Cu&t selection"), tr("Ctrl+X"), this);
  connect(actionCutSelection, SIGNAL(activated()), this, SLOT(cutSelection()));

  actionCopySelection = new QAction(QPixmap(copy_xpm), tr("&Copy selection"), tr("Ctrl+C"), this);
  connect(actionCopySelection, SIGNAL(activated()), this, SLOT(copySelection()));

  actionPasteSelection = new QAction(QPixmap(paste_xpm), tr("&Paste selection"), tr("Ctrl+V"), this);
  connect(actionPasteSelection, SIGNAL(activated()), this, SLOT(pasteSelection()));

  actionClearSelection = new QAction(QPixmap(erase_xpm), tr("&Delete selection"), tr("Key_Delete"), this);
  connect(actionClearSelection, SIGNAL(activated()), this, SLOT(clearSelection()));

  actionShowExplorer = new QAction(QPixmap(folder_xpm), tr("Project &explorer"), tr("Ctrl+E"), this);
  actionShowExplorer->setToggleAction(TRUE);
  actionShowExplorer->setOn(FALSE);
  connect(actionShowExplorer, SIGNAL(activated()), this, SLOT(showExplorer()));

  actionShowLog = new QAction(QPixmap(log_xpm), tr("Results &Log"), QString::null, this);
  actionShowLog->setToggleAction(TRUE);
  actionShowLog->setOn(FALSE);
  
  actionAddLayer = new QAction(QPixmap(newLayer_xpm), tr("Add La&yer"), tr("ALT+L"), this);
  connect(actionAddLayer, SIGNAL(activated()), this, SLOT(addLayer()));

  actionShowLayerDialog = new QAction(QPixmap(arrangeLayers_xpm), tr("Arran&ge Layers"), tr("ALT+A"), this);
  connect(actionShowLayerDialog, SIGNAL(activated()), this, SLOT(showLayerDialog()));

  actionExportGraph = new QAction(tr("&Current"), tr("Ctrl+G"), this);
  connect(actionExportGraph, SIGNAL(activated()), this, SLOT(exportGraph()));

  actionExportAllGraphs = new QAction(tr("&All"), tr("Alt+X"), this);
  connect(actionExportAllGraphs, SIGNAL(activated()), this, SLOT(exportAllGraphs()));

  actionPrint = new QAction(QPixmap(fileprint_xpm), tr("&Print"), tr("Ctrl+P"), this);
  connect(actionPrint, SIGNAL(activated()), this, SLOT(print()));

  actionPrintAllPlots = new QAction(tr("Print All Plo&ts"), QString::null, this);
  connect(actionPrintAllPlots, SIGNAL(activated()), this, SLOT(printAllPlots()));

  actionShowExportASCIIDialog = new QAction(tr("E&xport ASCII"), QString::null, this);
  connect(actionShowExportASCIIDialog, SIGNAL(activated()), this, SLOT(showExportASCIIDialog()));

  actionShowImportDialog = new QAction(tr("Set import &options"), QString::null, this);
  connect(actionShowImportDialog, SIGNAL(activated()), this, SLOT(showImportDialog()));

  actionCloseAllWindows = new QAction(QPixmap(quit_xpm), tr("&Quit"), tr("Ctrl+Q"), this);
  connect(actionCloseAllWindows, SIGNAL(activated()), qApp, SLOT(closeAllWindows()));

  actionClearLogInfo = new QAction(tr("Clear &log information"), QString::null, this);
  connect(actionClearLogInfo, SIGNAL(activated()), this, SLOT(clearLogInfo()));

  actionDeleteFitTables = new QAction(QPixmap(close_xpm), tr("Delete &fit tables"), QString::null, this);
  connect(actionDeleteFitTables, SIGNAL(activated()), this, SLOT(deleteFitTables()));

  actionShowPlotWizard = new QAction(QPixmap(wizard_xpm), tr("Plot &wizard"), tr("Ctrl+Alt+W"), this);
  connect(actionShowPlotWizard, SIGNAL(activated()), this, SLOT(showPlotWizard()));

  actionShowConfigureDialog = new QAction(tr("&Preferences..."), QString::null, this);
  connect(actionShowConfigureDialog, SIGNAL(activated()), this, SLOT(showPreferencesDialog()));

  actionShowCurvesDialog = new QAction(QPixmap(curves_xpm), tr("Add/Remove &Curve..."), tr("ALT+C"), this);
  connect(actionShowCurvesDialog, SIGNAL(activated()), this, SLOT(showCurvesDialog()));

  actionAddErrorBars = new QAction(QPixmap(errors_xpm), tr("Add &Error Bars..."), tr("Ctrl+B"), this);
  connect(actionAddErrorBars, SIGNAL(activated()), this, SLOT(addErrorBars()));

  actionAddFunctionCurve = new QAction(QPixmap(fx_xpm), tr("Add &Function..."), tr("Ctrl+Alt+F"), this);
  connect(actionAddFunctionCurve, SIGNAL(activated()), this, SLOT(addFunctionCurve()));

  actionUnzoom = new QAction(QPixmap(unzoom_xpm), tr("&Rescale to show all"), tr("Ctrl+R"), this);
  connect(actionUnzoom, SIGNAL(activated()), this, SLOT(unzoom()));

  actionNewLegend = new QAction(QPixmap(legend_xpm), tr("New &Legend"), tr("Ctrl+L"), this);
  connect(actionNewLegend, SIGNAL(activated()), this, SLOT(newLegend()));

  actionTimeStamp = new QAction(QPixmap(clock_xpm), tr("Add time stamp"), tr("Ctrl+ALT+T"), this);
  connect(actionTimeStamp, SIGNAL(activated()), this, SLOT(addTimeStamp()));

  actionAddImage = new QAction(QPixmap(monalisa_xpm), tr("Add &Image"), tr("ALT+I"), this);
  connect(actionAddImage, SIGNAL(activated()), this, SLOT(addImage()));

  actionPlotL = new QAction(QPixmap(lPlot_xpm), tr("&Line"), QString::null, this);
  connect(actionPlotL, SIGNAL(activated()), this, SLOT(plotL()));

  actionPlotP = new QAction(QPixmap(pPlot_xpm), tr("&Scatter"), QString::null, this);
  connect(actionPlotP, SIGNAL(activated()), this, SLOT(plotP()));

  actionPlotLP = new QAction(QPixmap(lpPlot_xpm), tr("Line + S&ymbol"), QString::null, this);
  connect(actionPlotLP, SIGNAL(activated()), this, SLOT(plotLP()));

  actionPlotVerticalDropLines = new QAction(QPixmap(dropLines_xpm), tr("Vertical &drop lines"), QString::null, this);
  connect(actionPlotVerticalDropLines, SIGNAL(activated()), this, SLOT(plotVerticalDropLines()));

  actionPlotSpline = new QAction(QPixmap(spline_xpm), tr("&Spline"), QString::null, this);
  connect(actionPlotSpline, SIGNAL(activated()), this, SLOT(plotSpline()));

  actionPlotSteps = new QAction(QPixmap(steps_xpm), tr("&Vertical Steps"), QString::null, this);
  connect(actionPlotSteps, SIGNAL(activated()), this, SLOT(plotSteps()));

  actionPlotVerticalBars = new QAction(QPixmap(vertBars_xpm), tr("&Columns"), QString::null, this);
  connect(actionPlotVerticalBars, SIGNAL(activated()), this, SLOT(plotVerticalBars()));

  actionPlotHorizontalBars = new QAction(QPixmap(hBars_xpm), tr("&Rows"), QString::null, this);
  connect(actionPlotHorizontalBars, SIGNAL(activated()), this, SLOT(plotHorizontalBars()));

  actionPlotArea = new QAction(QPixmap(area_xpm), tr("&Area"), QString::null, this);
  connect(actionPlotArea, SIGNAL(activated()), this, SLOT(plotArea()));

  actionPlotPie = new QAction(QPixmap(pie_xpm), tr("&Pie"), QString::null, this);
  connect(actionPlotPie, SIGNAL(activated()), this, SLOT(plotPie()));

  actionPlotVectXYAM = new QAction(QPixmap(vectXYAM_xpm), tr("Vectors XY&AM"), QString::null, this);
  connect(actionPlotVectXYAM, SIGNAL(activated()), this, SLOT(plotVectXYAM()));

  actionPlotVectXYXY = new QAction(QPixmap(vectXYXY_xpm), tr("&Vectors &XYXY"), QString::null, this);
  connect(actionPlotVectXYXY, SIGNAL(activated()), this, SLOT(plotVectXYXY()));

  actionPlotHistogram = new QAction(QPixmap(histogram_xpm), tr("&Histogram"), QString::null, this);
  connect(actionPlotHistogram, SIGNAL(activated()), this, SLOT(plotHistogram()));

  actionPlotStackedHistograms = new QAction(QPixmap(stacked_hist_xpm), tr("&Stacked Histogram"), QString::null, this);
  connect(actionPlotStackedHistograms, SIGNAL(activated()), this, SLOT(plotStackedHistograms()));

  actionPlot2VerticalLayers = new QAction(QPixmap(panel_v2_xpm), tr("&Vertical 2 Layers"), QString::null, this);
  connect(actionPlot2VerticalLayers, SIGNAL(activated()), this, SLOT(plot2VerticalLayers()));

  actionPlot2HorizontalLayers = new QAction(QPixmap(panel_h2_xpm), tr("&Horizontal 2 Layers"), QString::null, this);
  connect(actionPlot2HorizontalLayers, SIGNAL(activated()), this, SLOT(plot2HorizontalLayers()));

  actionPlot4Layers = new QAction(QPixmap(panel_4_xpm), tr("&4 Layers"), QString::null, this);
  connect(actionPlot4Layers, SIGNAL(activated()), this, SLOT(plot4Layers()));

  actionPlotStackedLayers = new QAction(QPixmap(stacked_xpm), tr("&Stacked Layers"), QString::null, this);
  connect(actionPlotStackedLayers, SIGNAL(activated()), this, SLOT(plotStackedLayers()));

  actionPlot3DRibbon = new QAction(QPixmap(ribbon_xpm), tr("&Ribbon"), QString::null, this);
  connect(actionPlot3DRibbon, SIGNAL(activated()), this, SLOT(plot3DRibbon()));

  actionPlot3DBars = new QAction(QPixmap(bars_xpm), tr("&Bars"), QString::null, this);
  connect(actionPlot3DBars, SIGNAL(activated()), this, SLOT(plot3DBars()));

  actionPlot3DScatter = new QAction(QPixmap(scatter_xpm), tr("&Scatter"), QString::null, this);
  connect(actionPlot3DScatter, SIGNAL(activated()), this, SLOT(plot3DScatter()));

  actionPlot3DTrajectory = new QAction(QPixmap(trajectory_xpm), tr("&Trajectory"), QString::null, this);
  connect(actionPlot3DTrajectory, SIGNAL(activated()), this, SLOT(plot3DTrajectory()));

  actionShowColStatistics = new QAction(QPixmap(col_stat_xpm), tr("Statistics on &Columns"), QString::null, this);
  connect(actionShowColStatistics, SIGNAL(activated()), this, SLOT(showColStatistics()));

  actionShowRowStatistics = new QAction(QPixmap(stat_rows_xpm), tr("Statistics on &Rows"), QString::null, this);
  connect(actionShowRowStatistics, SIGNAL(activated()), this, SLOT(showRowStatistics()));

  actionShowIntDialog = new QAction(tr("&Integrate ..."), QString::null, this);
  connect(actionShowIntDialog, SIGNAL(activated()), this, SLOT(showIntDialog()));

  actionInterpolate = new QAction(tr("Inte&rpolate ..."), QString::null, this);
  connect(actionInterpolate, SIGNAL(activated()), this, SLOT(showInterpolationDialog()));

  actionLowPassFilter = new QAction(tr("&Low Pass..."), QString::null, this);
  connect(actionLowPassFilter, SIGNAL(activated()), this, SLOT(lowPassFilterDialog()));

  actionHighPassFilter = new QAction(tr("&High Pass..."), QString::null, this);
  connect(actionHighPassFilter, SIGNAL(activated()), this, SLOT(highPassFilterDialog()));

  actionBandPassFilter = new QAction(tr("&Band Pass..."), QString::null, this);
  connect(actionBandPassFilter, SIGNAL(activated()), this, SLOT(bandPassFilterDialog()));

  actionBandBlockFilter = new QAction(tr("&Band Block..."), QString::null, this);
  connect(actionBandBlockFilter, SIGNAL(activated()), this, SLOT(bandBlockFilterDialog()));

  actionFFT = new QAction(tr("&FFT..."), QString::null, this);
  connect(actionFFT, SIGNAL(activated()), this, SLOT(showFFTDialog()));

  actionSmoothSavGol = new QAction(tr("&Savitzky-Golay..."), QString::null, this);
  connect(actionSmoothSavGol, SIGNAL(activated()), this, SLOT(showSmoothSavGolDialog()));

  actionSmoothFFT = new QAction(tr("&FFT Filter..."), QString::null, this);
  connect(actionSmoothFFT, SIGNAL(activated()), this, SLOT(showSmoothFFTDialog()));

  actionSmoothAverage = new QAction(tr("Moving Window &Average..."), QString::null, this);
  connect(actionSmoothAverage, SIGNAL(activated()), this, SLOT(showSmoothAverageDialog()));

  actionDifferentiate = new QAction(tr("&Differentiate"), QString::null, this);
  connect(actionDifferentiate, SIGNAL(activated()), this, SLOT(differentiate()));

  actionFitLinear = new QAction(tr("Fit &Linear"), QString::null, this);
  connect(actionFitLinear, SIGNAL(activated()), this, SLOT(fitLinear()));

  actionShowFitPolynomDialog = new QAction(tr("Fit &Polynomial ..."), QString::null, this);
  connect(actionShowFitPolynomDialog, SIGNAL(activated()), this, SLOT(showFitPolynomDialog()));

  actionShowExpDecayDialog = new QAction(tr("&First Order ..."), QString::null, this);
  connect(actionShowExpDecayDialog, SIGNAL(activated()), this, SLOT(showExpDecayDialog()));

  actionShowTwoExpDecayDialog = new QAction(tr("&Second Order ..."), QString::null, this);
  connect(actionShowTwoExpDecayDialog, SIGNAL(activated()), this, SLOT(showTwoExpDecayDialog()));

  actionShowExpDecay3Dialog = new QAction(tr("&Third Order ..."), QString::null, this);
  connect(actionShowExpDecay3Dialog, SIGNAL(activated()), this, SLOT(showExpDecay3Dialog()));

  actionFitExpGrowth = new QAction(tr("Fit Exponential Gro&wth ..."), QString::null, this);
  connect(actionFitExpGrowth, SIGNAL(activated()), this, SLOT(showExpGrowthDialog()));

  actionFitSigmoidal = new QAction(tr("Fit &Boltzmann (Sigmoidal)"), QString::null, this);
  connect(actionFitSigmoidal, SIGNAL(activated()), this, SLOT(fitSigmoidal()));

  actionFitGauss = new QAction(tr("Fit &Gaussian"), QString::null, this);
  connect(actionFitGauss, SIGNAL(activated()), this, SLOT(fitGauss()));

  actionFitLorentz = new QAction(tr("Fit Lorent&zian"), QString::null, this);
  connect(actionFitLorentz, SIGNAL(activated()), this, SLOT(fitLorentz()));

  actionShowFitDialog = new QAction(tr("&Non-linear Curve Fit ..."), tr("Ctrl+Y"), this);
  connect(actionShowFitDialog, SIGNAL(activated()), this, SLOT(showFitDialog()));

  actionShowPlotDialog = new QAction(tr("&Plot ..."), QString::null, this);
  connect(actionShowPlotDialog, SIGNAL(activated()), this, SLOT(showGeneralPlotDialog()));

  actionShowLayoutDialog = new QAction(tr("&Curves ..."), QString::null, this);
  connect(actionShowLayoutDialog, SIGNAL(activated()), this, SLOT(showPlotDialog()));

  actionShowScaleDialog = new QAction(tr("&Scales..."), QString::null, this);
  connect(actionShowScaleDialog, SIGNAL(activated()), this, SLOT(showScaleDialog()));

  actionShowAxisDialog = new QAction(tr("&Axes..."), QString::null, this);
  connect(actionShowAxisDialog, SIGNAL(activated()), this, SLOT(showAxisDialog()));

  actionShowGridDialog = new QAction(tr("&Grid ..."), QString::null, this);
  connect(actionShowGridDialog, SIGNAL(activated()), this, SLOT(showGridDialog()));

  actionShowTitleDialog = new QAction(tr("&Title ..."), QString::null, this);
  connect(actionShowTitleDialog, SIGNAL(activated()), this, SLOT(showTitleDialog()));

  actionShowColumnOptionsDialog = new QAction(tr("Column &Options ..."), QString::null, this);
  connect(actionShowColumnOptionsDialog, SIGNAL(activated()), this, SLOT(showColumnOptionsDialog()));

  actionShowColumnValuesDialog = new QAction(tr("Set Column &Values ..."), QString::null, this);
  connect(actionShowColumnValuesDialog, SIGNAL(activated()), this, SLOT(showColumnValuesDialog()));

  actionShowColsDialog = new QAction(tr("&Columns..."), QString::null, this);
  connect(actionShowColsDialog, SIGNAL(activated()), this, SLOT(showColsDialog()));

  actionShowRowsDialog = new QAction(tr("&Rows..."), QString::null, this);
  connect(actionShowRowsDialog, SIGNAL(activated()), this, SLOT(showRowsDialog()));

  actionAbout = new QAction(tr("&About QtiPlot"), tr("F1"), this);
  connect(actionAbout, SIGNAL(activated()), this, SLOT(about()));

  actionShowHelp = new QAction(tr("&Help"), tr("Ctrl+H"), this);
  connect(actionShowHelp, SIGNAL(activated()), this, SLOT(showHelp()));

  actionChooseHelpFolder = new QAction(tr("&Choose help folder..."), QString::null, this);
  connect(actionChooseHelpFolder, SIGNAL(activated()), this, SLOT(chooseHelpFolder()));

  actionRename = new QAction(tr("&Rename Window"), QString::null, this);
  connect(actionRename, SIGNAL(activated()), this, SLOT(rename()));

  actionCloseWindow = new QAction(QPixmap(close_xpm), tr("Close &Window"), tr("Ctrl+W"), this);
  connect(actionCloseWindow, SIGNAL(activated()), this, SLOT(closeActiveWindow()));

  actionAddColToTable = new QAction(QPixmap(addCol_xpm), tr("Add column"), QString::null, this);
  connect(actionAddColToTable, SIGNAL(activated()), this, SLOT(addColToTable()));

  actionDeleteLayer = new QAction(QPixmap(erase_xpm), tr("&Remove Layer"), tr("Alt+R"), this);
  connect(actionDeleteLayer, SIGNAL(activated()), this, SLOT(deleteLayer()));

  actionPrintHelp = new QAction(QPixmap(fileprint_xpm), tr("Print"), QString::null, this);
  connect(actionPrintHelp, SIGNAL(activated()), this, SLOT(printHelp()));

  actionResizeActiveWindow = new QAction(QPixmap(resize_xpm), tr("Window &Geometry..."), QString::null, this);
  connect(actionResizeActiveWindow, SIGNAL(activated()), this, SLOT(resizeActiveWindow()));

  actionHideActiveWindow = new QAction(tr("&Hide Window"), QString::null, this);
  connect(actionHideActiveWindow, SIGNAL(activated()), this, SLOT(hideActiveWindow()));

  actionShowMoreWindows = new QAction(tr("More windows..."), QString::null, this);
  connect(actionShowMoreWindows, SIGNAL(activated()), this, SLOT(showMoreWindows()));

  actionPixelLineProfile = new QAction(QPixmap(pixelProfile_xpm), tr("&View pixel line profile"), QString::null, this);
  connect(actionPixelLineProfile, SIGNAL(activated()), this, SLOT(pixelLineProfile()));

  actionIntensityTable = new QAction(tr("&Intensity table"), QString::null, this);
  connect(actionIntensityTable, SIGNAL(activated()), this, SLOT(intensityTable()));

  actionShowLineDialog = new QAction(tr("&Properties"), QString::null, this);
  connect(actionShowLineDialog, SIGNAL(activated()), this, SLOT(showLineDialog()));

  actionShowImageDialog = new QAction(tr("&Properties"), QString::null, this);
  connect(actionShowImageDialog, SIGNAL(activated()), this, SLOT(showImageDialog()));

  actionShowTextDialog = new QAction(tr("&Properties"), QString::null, this);
  connect(actionShowTextDialog, SIGNAL(activated()), this, SLOT(showTextDialog()));

  actionActivateWindow = new QAction(tr("&Activate Window"), QString::null, this);
  connect(actionActivateWindow, SIGNAL(activated()), this, SLOT(activateWindow()));

  actionMinimizeWindow = new QAction(tr("Mi&nimize Window"), QString::null, this);
  connect(actionMinimizeWindow, SIGNAL(activated()), this, SLOT(minimizeWindow()));

  actionMaximizeWindow = new QAction(tr("Ma&ximize Window"), QString::null, this);
  connect(actionMaximizeWindow, SIGNAL(activated()), this, SLOT(maximizeWindow()));

  actionHideWindow = new QAction(tr("&Hide Window"), QString::null, this);
  connect(actionHideWindow, SIGNAL(activated()), this, SLOT(hideWindow()));

  actionResizeWindow = new QAction(QPixmap(resize_xpm), tr("Re&size Window..."), QString::null, this);
  connect(actionResizeWindow, SIGNAL(activated()), this, SLOT(resizeWindow()));

  actionPrintWindow = new QAction(QPixmap(fileprint_xpm),tr("&Print Window"), QString::null, this);
  connect(actionPrintWindow, SIGNAL(activated()), this, SLOT(printWindow()));

  actionShowPlotGeometryDialog = new QAction(QPixmap(resize_xpm), tr("&Layer geometry"), QString::null, this);
  connect(actionShowPlotGeometryDialog, SIGNAL(activated()), this, SLOT(showPlotGeometryDialog()));

  actionEditSurfacePlot = new QAction(tr("&Surface..."), QString::null, this);
  connect(actionEditSurfacePlot, SIGNAL(activated()), this, SLOT(editSurfacePlot()));

  actionAdd3DData = new QAction(tr("&Data Set..."), QString::null, this);
  connect(actionAdd3DData, SIGNAL(activated()), this, SLOT(add3DData()));

  actionSetMatrixProperties = new QAction(tr("Set &Properties..."), QString::null, this);
  connect(actionSetMatrixProperties, SIGNAL(activated()), this, SLOT(showMatrixDialog()));

  actionSetMatrixDimensions = new QAction(tr("Set &Dimensions..."), QString::null, this);
  connect(actionSetMatrixDimensions, SIGNAL(activated()), this, SLOT(showMatrixSizeDialog()));

  actionSetMatrixValues = new QAction(tr("Set &Values..."), QString::null, this);
  connect(actionSetMatrixValues, SIGNAL(activated()), this, SLOT(showMatrixValuesDialog()));

  actionTransposeMatrix = new QAction(tr("&Transpose"), QString::null, this);
  connect(actionTransposeMatrix, SIGNAL(activated()), this, SLOT(transposeMatrix()));

  actionInvertMatrix = new QAction(tr("&Invert"), QString::null, this);
  connect(actionInvertMatrix, SIGNAL(activated()), this, SLOT(invertMatrix()));

  actionMatrixDeterminant = new QAction(tr("&Determinant"), QString::null, this);
  connect(actionMatrixDeterminant, SIGNAL(activated()), this, SLOT(matrixDeterminant()));

  actionConvertMatrix = new QAction(tr("&Convert to spreadsheet"), QString::null, this);
  connect(actionConvertMatrix, SIGNAL(activated()), this, SLOT(convertMatrixToTable()));

  actionConvertTable= new QAction(tr("Convert to &matrix"), QString::null, this);
  connect(actionConvertTable, SIGNAL(activated()), this, SLOT(convertTableToMatrix()));

  actionPlot3DWireFrame = new QAction(QPixmap(lineMesh_xpm), tr("3D &Wire Frame"), QString::null, this);
  connect(actionPlot3DWireFrame, SIGNAL(activated()), this, SLOT(plot3DWireframe()));

  actionPlot3DHiddenLine = new QAction(QPixmap(grid_only_xpm), tr("3D &Hidden Line"), QString::null, this);
  connect(actionPlot3DHiddenLine, SIGNAL(activated()), this, SLOT(plot3DHiddenLine()));

  actionPlot3DPolygons = new QAction(QPixmap(no_grid_xpm), tr("3D &Polygons"), QString::null, this);
  connect(actionPlot3DPolygons, SIGNAL(activated()), this, SLOT(plot3DPolygons()));

  actionPlot3DWireSurface = new QAction(QPixmap(grid_poly_xpm), tr("3D Wire &Surface"), QString::null, this);
  connect(actionPlot3DWireSurface, SIGNAL(activated()), this, SLOT(plot3DWireSurface()));

  actionSortTable = new QAction(tr("Sort Ta&ble"), QString::null, this);
  connect(actionSortTable, SIGNAL(activated()), this, SLOT(sortActiveTable()));

  actionSortSelection = new QAction(tr("Sort Columns"), QString::null, this);
  connect(actionSortSelection, SIGNAL(activated()), this, SLOT(sortSelection()));

  actionNormalizeTable = new QAction(tr("&Table"), QString::null, this);
  connect(actionNormalizeTable, SIGNAL(activated()), this, SLOT(normalizeActiveTable()));

  actionNormalizeSelection = new QAction(tr("&Columns"), QString::null, this);
  connect(actionNormalizeSelection, SIGNAL(activated()), this, SLOT(normalizeSelection()));

  actionCorrelate = new QAction(tr("Co&rrelate"), QString::null, this);
  connect(actionCorrelate, SIGNAL(activated()), this, SLOT(correlate()));

  actionConvolute = new QAction(tr("&Convolute"), QString::null, this);
  connect(actionConvolute, SIGNAL(activated()), this, SLOT(convolute()));

  actionDeconvolute = new QAction(tr("&Deconvolute"), QString::null, this);
  connect(actionDeconvolute, SIGNAL(activated()), this, SLOT(deconvolute()));

  actionTranslateHor = new QAction(tr("&Horizontal"), QString::null, this);
  connect(actionTranslateHor, SIGNAL(activated()), this, SLOT(translateCurveHor()));

  actionTranslateVert = new QAction(tr("&Vertical"), QString::null, this);
  connect(actionTranslateVert, SIGNAL(activated()), this, SLOT(translateCurveVert()));

  actionSetAscValues = new QAction(QPixmap(rowNumbers_xpm),tr("Ro&w Numbers"), QString::null, this);
  connect(actionSetAscValues, SIGNAL(activated()), this, SLOT(setAscValues()));

  actionSetRandomValues = new QAction(QPixmap(randomNumbers_xpm),tr("&Random values"), QString::null, this);
  connect(actionSetRandomValues, SIGNAL(activated()), this, SLOT(setRandomValues()));

  actionSetXCol = new QAction("&X", QString::null, this);
  connect(actionSetXCol, SIGNAL(activated()), this, SLOT(setXCol()));

  actionSetYCol = new QAction("&Y", QString::null, this);
  connect(actionSetYCol, SIGNAL(activated()), this, SLOT(setYCol()));

  actionSetZCol = new QAction("&Z", QString::null, this);
  connect(actionSetZCol, SIGNAL(activated()), this, SLOT(setZCol()));

  actionDisregardCol = new QAction(tr("&None"), QString::null, this);
  connect(actionDisregardCol, SIGNAL(activated()), this, SLOT(disregardCol()));

  actionBoxPlot = new QAction(QPixmap(boxPlot_xpm),tr("&Box Plot"), QString::null, this);
  connect(actionBoxPlot, SIGNAL(activated()), this, SLOT(plotBoxDiagram()));

  actionMultiPeakGauss = new QAction(tr("&Gaussian..."), QString::null, this);
  connect(actionMultiPeakGauss, SIGNAL(activated()), this, SLOT(fitMultiPeakGauss()));

  actionMultiPeakLorentz = new QAction(tr("&Lorentzian..."), QString::null, this);
  connect(actionMultiPeakLorentz, SIGNAL(activated()), this, SLOT(fitMultiPeakLorentz()));

  actionCheckUpdates = new QAction(tr("&Search for Updates"), QString::null, this);
  connect(actionCheckUpdates, SIGNAL(activated()), this, SLOT(checkUpdates()));

  actionHomePage = new QAction(tr("&QtiPlot Homepage"), QString::null, this);
  connect(actionHomePage, SIGNAL(activated()), this, SLOT(showHomePage()));

  actionDownloadManual = new QAction(tr("Download &manual"), QString::null, this);
  connect(actionDownloadManual, SIGNAL(activated()), this, SLOT(downloadManual()));

  actionTranslations = new QAction(tr("&Translations"), QString::null, this);
  connect(actionTranslations, SIGNAL(activated()), this, SLOT(downloadTranslation()));

  actionDonate = new QAction(tr("Make a &donation"), QString::null, this);
  connect(actionDonate, SIGNAL(activated()), this, SLOT(showDonationsPage()));

  actionTechnicalSupport = new QAction(tr("Technical &support"), QString::null, this);
  connect(actionTechnicalSupport, SIGNAL(activated()), this, SLOT(showSupportPage()));
}

void ApplicationWindow::translateActionsStrings()
{
  actionNewProject->setMenuText(tr("New &Project"));
  actionNewProject->setToolTip(tr("Open a new project"));
  actionNewProject->setAccel(tr("Ctrl+N"));

  actionNewGraph->setMenuText(tr("New &Graph"));
  actionNewGraph->setToolTip(tr("Create an empty 2D plot"));
  actionNewGraph->setAccel(tr("Ctrl+G"));	

  actionNewNote->setMenuText(tr("New &Note"));
  actionNewNote->setToolTip(tr("Create an empty note window"));

  actionNewTable->setMenuText(tr("New &Table"));
  actionNewTable->setAccel(tr("Ctrl+T"));
  actionNewTable->setToolTip(tr("New Table"));

  actionNewMatrix->setMenuText(tr("New &Matrix"));
  actionNewMatrix->setAccel(tr("Ctrl+M"));
  actionNewMatrix->setToolTip(tr("New matrix"));

  actionNewFunctionPlot->setMenuText(tr("New &Function Plot"));
  actionNewFunctionPlot->setToolTip(tr("Create a new 2D function plot"));
  actionNewFunctionPlot->setAccel(tr("Ctrl+F"));

  actionNewSurfacePlot->setMenuText(tr("New &Surface 3D Plot"));
  actionNewSurfacePlot->setToolTip(tr("Create a new 3D surface plot"));
  actionNewSurfacePlot->setAccel(tr("Ctrl+ALT+Z"));

  actionOpen->setMenuText(tr("&Open"));
  actionOpen->setAccel(tr("Ctrl+O"));
  actionOpen->setToolTip(tr("Open project"));

  actionLoadImage->setMenuText(tr("Open image &file"));
  actionLoadImage->setAccel(tr("Ctrl+I"));

  actionImportImage->setMenuText(tr("Import i&mage..."));

  actionSaveProject->setMenuText(tr("&Save Project"));
  actionSaveProject->setToolTip(tr("Save Project"));
  actionSaveProject->setAccel(tr("Ctrl+S"));

  actionSaveProjectAs->setMenuText(tr("Save Project &as..."));

  actionOpenTemplate->setMenuText(tr("Open Te&mplate..."));
  actionOpenTemplate->setToolTip(tr("Open Template"));

  actionSaveTemplate->setMenuText(tr("Save as &Template..."));
  actionSaveTemplate->setToolTip(tr("Save Window as Template"));

  actionLoad->setMenuText(tr("&Single file..."));
  actionLoad->setToolTip(tr("Import data file"));

  actionLoadMultiple->setMenuText(tr("&Multiple files..."));
  actionLoadMultiple->setToolTip(tr("Import multiple data files"));

  actionUndo->setMenuText(tr("&Undo"));
  actionUndo->setToolTip(tr("Undo changes"));
  actionUndo->setAccel(tr("Ctrl+Z"));

  actionRedo->setMenuText(tr("&Redo"));
  actionRedo->setToolTip(tr("Redo changes"));
  actionRedo->setAccel(tr("Ctrl+R"));

  actionCopyWindow->setMenuText(tr("&Duplicate"));
  actionCopyWindow->setToolTip(tr("Duplicate window"));

  actionCutSelection->setMenuText(tr("Cu&t selection"));
  actionCutSelection->setToolTip(tr("Cut selection"));
  actionCutSelection->setAccel(tr("Ctrl+X"));

  actionCopySelection->setMenuText(tr("&Copy selection"));
  actionCopySelection->setToolTip(tr("Copy selection"));
  actionCopySelection->setAccel(tr("Ctrl+C"));

  actionPasteSelection->setMenuText(tr("&Paste selection"));
  actionPasteSelection->setToolTip(tr("Paste selection"));
  actionPasteSelection->setAccel(tr("Ctrl+V"));

  actionClearSelection->setMenuText(tr("&Delete selection"));
  actionClearSelection->setToolTip(tr("Delete selection"));
  actionClearSelection->setAccel(tr("Key_Delete"));

  actionShowExplorer->setMenuText(tr("Project &explorer"));
  actionShowExplorer->setAccel(tr("Ctrl+E"));
  actionShowExplorer->setToolTip(tr("Show project explorer"));

  actionShowLog->setMenuText(tr("Results &Log"));
  actionShowLog->setToolTip(tr("Show calculus results"));
  
  actionAddLayer->setMenuText(tr("Add La&yer"));
  actionAddLayer->setAccel(tr("ALT+L"));

  actionShowLayerDialog->setMenuText(tr("Arran&ge Layers"));
  actionShowLayerDialog->setAccel(tr("ALT+A"));

  actionExportGraph->setMenuText(tr("&Current"));
  actionExportGraph->setAccel(tr("Ctrl+G"));
  actionExportGraph->setToolTip(tr("Export current graph"));

  actionExportAllGraphs->setMenuText(tr("&All")); 
  actionExportAllGraphs->setAccel(tr("Alt+X"));
  actionExportAllGraphs->setToolTip(tr("Export all graphs"));

  actionPrint->setMenuText(tr("&Print")); 
  actionPrint->setAccel(tr("Ctrl+P"));
  actionPrint->setToolTip(tr("Print window"));

  actionPrintAllPlots->setMenuText(tr("Print All Plo&ts"));
  actionShowExportASCIIDialog->setMenuText(tr("E&xport ASCII"));
  actionShowImportDialog->setMenuText(tr("Set import &options"));

  actionCloseAllWindows->setMenuText(tr("&Quit")); 
  actionCloseAllWindows->setAccel(tr("Ctrl+Q"));

  actionClearLogInfo->setMenuText(tr("Clear &log information"));
  actionDeleteFitTables->setMenuText(tr("Delete &fit tables"));

  actionShowPlotWizard->setMenuText(tr("Plot &wizard")); 
  actionShowPlotWizard->setAccel(tr("Ctrl+Alt+W"));

  actionShowConfigureDialog->setMenuText(tr("&Preferences..."));

  actionShowCurvesDialog->setMenuText(tr("Add/Remove &Curve...")); 
  actionShowCurvesDialog->setAccel(tr("ALT+C"));
  actionShowCurvesDialog->setToolTip(tr("Add curve to graph"));

  actionAddErrorBars->setMenuText(tr("Add &Error Bars...")); 
  actionAddErrorBars->setAccel(tr("Ctrl+B"));

  actionAddFunctionCurve->setMenuText(tr("Add &Function...")); 
  actionAddFunctionCurve->setAccel(tr("Ctrl+Alt+F"));

  actionUnzoom->setMenuText(tr("&Rescale to show all")); 
  actionUnzoom->setAccel(tr("Ctrl+R"));
  actionUnzoom->setToolTip(tr("Best fit"));

  actionNewLegend->setMenuText( tr("New &Legend")); 
  actionNewLegend->setAccel(tr("Ctrl+L"));
  actionNewLegend->setToolTip(tr("Add new legend"));

  actionTimeStamp->setMenuText(tr("Add time stamp")); 
  actionTimeStamp->setAccel(tr("Ctrl+ALT+T"));
  actionTimeStamp->setToolTip(tr("Date & Time "));

  actionAddImage->setMenuText(tr("Add &Image")); 
  actionAddImage->setAccel(tr("ALT+I"));

  actionPlotL->setMenuText(tr("&Line"));
  actionPlotL->setToolTip(tr("Plot with line"));

  actionPlotP->setMenuText(tr("&Scatter"));
  actionPlotP->setToolTip(tr("Plot with symbols"));

  actionPlotLP->setMenuText(tr("Line + S&ymbol"));
  actionPlotLP->setToolTip(tr("Plot with line + symbols"));

  actionPlotVerticalDropLines->setMenuText(tr("Vertical &drop lines"));

  actionPlotSpline->setMenuText(tr("&Spline"));
  actionPlotSteps->setMenuText(tr("&Vertical Steps"));

  actionPlotVerticalBars->setMenuText(tr("&Columns"));
  actionPlotVerticalBars->setToolTip(tr("Plot with vertical bars"));

  actionPlotHorizontalBars->setMenuText(tr("&Rows"));
  actionPlotHorizontalBars->setToolTip(tr("Plot with horizontal bars"));

  actionPlotArea->setMenuText(tr("&Area"));
  actionPlotArea->setToolTip(tr("Plot area"));

  actionPlotPie->setMenuText(tr("&Pie"));
  actionPlotPie->setToolTip(tr("Plot pie"));

  actionPlotVectXYXY->setMenuText(tr("&Vectors XYXY"));
  actionPlotVectXYXY->setToolTip(tr("Vectors XYXY"));

  actionPlotVectXYAM->setMenuText(tr("Vectors XY&AM"));
  actionPlotVectXYAM->setToolTip(tr("Vectors XYAM"));

  actionPlotHistogram->setMenuText( tr("&Histogram"));
  actionPlotStackedHistograms->setMenuText(tr("&Stacked Histogram"));
  actionPlot2VerticalLayers->setMenuText(tr("&Vertical 2 Layers"));
  actionPlot2HorizontalLayers->setMenuText(tr("&Horizontal 2 Layers"));
  actionPlot4Layers->setMenuText(tr("&4 Layers"));
  actionPlotStackedLayers->setMenuText(tr("&Stacked Layers"));

  actionPlot3DRibbon->setMenuText(tr("&Ribbon"));
  actionPlot3DRibbon->setToolTip(tr("Plot 3D Ribbon"));

  actionPlot3DBars->setMenuText(tr("&Bars"));
  actionPlot3DBars->setToolTip(tr("Plot 3D Bars"));

  actionPlot3DScatter->setMenuText(tr("&Scatter"));
  actionPlot3DScatter->setToolTip(tr("Plot 3D Scatter"));

  actionPlot3DTrajectory->setMenuText(tr("&Trajectory"));
  actionPlot3DTrajectory->setToolTip(tr("Plot 3D Trajectory"));

  actionShowColStatistics->setMenuText(tr("Statistics on &Columns"));
  actionShowColStatistics->setToolTip(tr("Selected columns statistics"));

  actionShowRowStatistics->setMenuText(tr("Statistics on &Rows"));
  actionShowRowStatistics->setToolTip(tr("Selected rows statistics"));
  actionShowIntDialog->setMenuText(tr("&Integrate ..."));
  actionInterpolate->setMenuText(tr("Inte&rpolate ..."));
  actionLowPassFilter->setMenuText(tr("&Low Pass..."));
  actionHighPassFilter->setMenuText(tr("&High Pass..."));
  actionBandPassFilter->setMenuText(tr("&Band Pass..."));
  actionBandBlockFilter->setMenuText(tr("&Band Block..."));
  actionFFT->setMenuText(tr("&FFT..."));
  actionSmoothSavGol->setMenuText(tr("&Savitzky-Golay..."));
  actionSmoothFFT->setMenuText(tr("&FFT Filter..."));
  actionSmoothAverage->setMenuText(tr("Moving Window &Average..."));
  actionDifferentiate->setMenuText(tr("&Differentiate"));
  actionFitLinear->setMenuText(tr("Fit &Linear"));
  actionShowFitPolynomDialog->setMenuText(tr("Fit &Polynomial ..."));
  actionShowExpDecayDialog->setMenuText(tr("&First Order ..."));
  actionShowTwoExpDecayDialog->setMenuText(tr("&Second Order ..."));
  actionShowExpDecay3Dialog->setMenuText(tr("&Third Order ..."));
  actionFitExpGrowth->setMenuText(tr("Fit Exponential Gro&wth ..."));
  actionFitSigmoidal->setMenuText(tr("Fit &Boltzmann (Sigmoidal)"));
  actionFitGauss->setMenuText(tr("Fit &Gaussian"));
  actionFitLorentz->setMenuText(tr("Fit Lorent&zian"));

  actionShowFitDialog->setMenuText(tr("&Non-linear Curve Fit ...")); 
  actionShowFitDialog->setAccel(tr("Ctrl+Y"));

  actionShowPlotDialog->setMenuText(tr("&Plot ..."));
  actionShowScaleDialog->setMenuText(tr("&Scales..."));
  actionShowLayoutDialog->setMenuText(tr("&Curves..."));
  actionShowAxisDialog->setMenuText(tr("&Axes..."));
  actionShowGridDialog->setMenuText(tr("&Grid ..."));
  actionShowTitleDialog->setMenuText(tr("&Title ..."));
  actionShowColumnOptionsDialog->setMenuText(tr("Column &Options ..."));
  actionShowColumnValuesDialog->setMenuText(tr("Set Column &Values ..."));
  actionShowColsDialog->setMenuText(tr("&Columns..."));
  actionShowRowsDialog->setMenuText(tr("&Rows..."));

  actionAbout->setMenuText(tr("&About QtiPlot")); 
  actionAbout->setAccel(tr("F1"));

  actionShowHelp->setMenuText(tr("&Help")); 
  actionShowHelp->setAccel(tr("Ctrl+H"));

  actionChooseHelpFolder->setMenuText(tr("&Choose help folder..."));
  actionRename->setMenuText(tr("&Rename Window"));

  actionCloseWindow->setMenuText(tr("Close &Window"));
  actionCloseWindow->setAccel(tr("Ctrl+W"));

  actionAddColToTable->setMenuText(tr("Add column"));

  actionDeleteLayer->setMenuText(tr("&Remove Layer"));
  actionDeleteLayer->setAccel(tr("Alt+R"));

  actionPrintHelp->setMenuText(tr("Print"));
  actionResizeActiveWindow->setMenuText(tr("Window &Geometry..."));
  actionHideActiveWindow->setMenuText(tr("&Hide Window"));
  actionShowMoreWindows->setMenuText(tr("More windows..."));
  actionPixelLineProfile->setMenuText(tr("&View pixel line profile"));
  actionIntensityTable->setMenuText(tr("&Intensity table"));
  actionShowLineDialog->setMenuText(tr("&Properties"));
  actionShowImageDialog->setMenuText(tr("&Properties"));
  actionShowTextDialog->setMenuText(tr("&Properties"));
  actionActivateWindow->setMenuText(tr("&Activate Window"));
  actionMinimizeWindow->setMenuText(tr("Mi&nimize Window"));
  actionMaximizeWindow->setMenuText(tr("Ma&ximize Window"));
  actionHideWindow->setMenuText(tr("&Hide Window"));
  actionResizeWindow->setMenuText(tr("Re&size Window..."));
  actionPrintWindow->setMenuText(tr("&Print Window"));
  actionShowPlotGeometryDialog->setMenuText(tr("&Layer geometry"));
  actionEditSurfacePlot->setMenuText(tr("&Surface..."));
  actionAdd3DData->setMenuText(tr("&Data Set..."));
  actionSetMatrixProperties->setMenuText(tr("Set &Properties..."));
  actionSetMatrixDimensions->setMenuText(tr("Set &Dimensions..."));
  actionSetMatrixValues->setMenuText(tr("Set &Values..."));
  actionTransposeMatrix->setMenuText(tr("&Transpose"));
  actionInvertMatrix->setMenuText(tr("&Invert"));
  actionMatrixDeterminant->setMenuText(tr("&Determinant"));
  actionConvertMatrix->setMenuText(tr("&Convert to spreadsheet"));
  actionConvertTable->setMenuText(tr("Convert to &matrix"));
  actionPlot3DWireFrame->setMenuText(tr("3D &Wire Frame"));
  actionPlot3DHiddenLine->setMenuText(tr("3D &Hidden Line"));
  actionPlot3DPolygons->setMenuText(tr("3D &Polygons"));
  actionPlot3DWireSurface->setMenuText(tr("3D Wire &Surface"));
  actionSortTable->setMenuText(tr("Sort Ta&ble"));
  actionSortSelection->setMenuText(tr("Sort Columns"));
  actionNormalizeTable->setMenuText(tr("&Table"));
  actionNormalizeSelection->setMenuText(tr("&Columns"));
  actionCorrelate->setMenuText(tr("Co&rrelate"));
  actionConvolute->setMenuText(tr("&Convolute"));
  actionDeconvolute->setMenuText(tr("&Deconvolute"));
  actionTranslateHor->setMenuText(tr("&Horizontal"));
  actionTranslateVert->setMenuText(tr("&Vertical"));
  actionSetAscValues->setMenuText(tr("Ro&w Numbers"));
  actionSetRandomValues->setMenuText(tr("&Random values"));
  actionSetXCol->setMenuText("&X");
  actionSetYCol->setMenuText("&Y");
  actionSetZCol->setMenuText("&Z");
  actionDisregardCol->setMenuText(tr("&None"));

  actionBoxPlot->setMenuText(tr("&Box Plot"));
  actionBoxPlot->setToolTip(tr("Box and Whiskers Plot"));

  actionMultiPeakGauss->setMenuText(tr("&Gaussian..."));
  actionMultiPeakLorentz->setMenuText(tr("&Lorentzian..."));
  actionHomePage->setMenuText(tr("&QtiPlot Homepage"));
  actionCheckUpdates->setMenuText(tr("&Search for Updates"));
  actionDownloadManual->setMenuText(tr("Download &manual"));
  actionTranslations->setMenuText(tr("&Translations"));
  actionDonate->setMenuText(tr("Make a &donation"));
  actionTechnicalSupport->setMenuText(tr("Technical &support"));

  	btnPointer->setMenuText(tr("Disable &tools"));
	btnPointer->setToolTip( tr( "Pointer" ) );

	btnZoom->setMenuText(tr("&Zoom"));
	btnZoom->setAccel(tr("ALT+Z"));
	btnZoom->setToolTip(tr("Zoom"));

    btnCursor->setMenuText(tr("&Data reader"));
	btnCursor->setAccel(tr("CTRL+D"));
	btnCursor->setToolTip(tr("Data reader"));

	btnSelect->setMenuText(tr("&Select data range"));
	btnSelect->setAccel(tr("ALT+S"));
	btnSelect->setToolTip(tr("Select data range"));

	btnPicker->setMenuText(tr("S&creen reader"));
	btnPicker->setToolTip(tr("Screen reader"));

	btnMovePoints->setMenuText(tr("&Move Data Points..."));
	btnMovePoints->setAccel(tr("Ctrl+ALT+M"));
	btnMovePoints->setToolTip(tr("Move data points"));

	btnRemovePoints->setMenuText(tr("Remove &Bad Data Points..."));
	btnRemovePoints->setAccel(tr("Alt+B"));
	btnRemovePoints->setToolTip(tr("Remove data points"));

	actionAddText->setMenuText(tr("Add &Text"));
	actionAddText->setAccel(tr("ALT+T"));

	btnLine->setMenuText(tr("Draw &Arrow/Line"));
	btnLine->setAccel(tr("CTRL+ALT+L"));
	btnLine->setToolTip(tr("Draw line"));

	coord->setText( tr( "Coordinates" ) );
    coord->setMenuText( tr( "&Coord" ) );
    coord->setStatusTip( tr( "Coordinates" ) );
    Box->setText( tr( "Box" ) );
    Box->setMenuText( tr( "Box" ) );
    Box->setToolTip( tr( "Box" ) );
    Box->setStatusTip( tr( "Box" ) );
    Frame->setText( tr( "Frame" ) );
    Frame->setMenuText( tr( "&Frame" ) );
    Frame->setToolTip( tr( "Frame" ) );
    Frame->setStatusTip( tr( "Frame" ) );
    None->setText( tr( "No Axes" ) );
    None->setMenuText( tr( "No Axes" ) );
    None->setToolTip( tr( "No Axes" ) );
    None->setStatusTip( tr( "No Axes" ) );

	grids->setText( tr( "grid" ) );
    grids->setMenuText( tr( "grid" ) );
    grids->setStatusTip( tr( "grid" ) );
    front->setToolTip( tr( "Front Grid" ) );
    back->setToolTip( tr( "Back Grid" ) );
    right->setToolTip( tr( "Right Grid" ) );
    left->setToolTip( tr( "Left Grid" ) );
    ceil->setToolTip( tr( "Ceiling Grid" ) );
    floor->setToolTip( tr( "Floor Grid" ) );

	plotstyle->setText( tr( "Plot Style" ) );
    plotstyle->setMenuText( tr( "Plot Style" ) );
    plotstyle->setStatusTip( tr( "Plot Style" ) );
    wireframe->setText( tr( "Wireframe" ) );
    wireframe->setMenuText( tr( "Wireframe" ) );
    wireframe->setToolTip( tr( "Wireframe" ) );
    wireframe->setStatusTip( tr( "Wireframe" ) );
    hiddenline->setText( tr( "Hidden Line" ) );
    hiddenline->setMenuText( tr( "Hidden Line" ) );
    hiddenline->setToolTip( tr( "Hidden Line" ) );
    hiddenline->setStatusTip( tr( "Hidden Line" ) );
    polygon->setText( tr( "Polygon only" ) );
    polygon->setMenuText( tr( "Polygon only" ) );
    polygon->setToolTip( tr( "Polygon only" ) );
    polygon->setStatusTip( tr( "Polygon only" ) );
    filledmesh->setText( tr( "Mesh & filled Polygons" ) );
    filledmesh->setMenuText( tr( "Mesh & filled Polygons" ) );
    filledmesh->setToolTip( tr( "Mesh & filled Polygons" ) );
    filledmesh->setStatusTip( tr( "Mesh & filled Polygons" ) );
	pointstyle->setText( tr( "Dots" ) );
    pointstyle->setMenuText( tr( "Dots" ) );
    pointstyle->setToolTip( tr( "Dots" ) );
    pointstyle->setStatusTip( tr( "Dots" ) );
	barstyle->setText( tr( "Bars" ) );
    barstyle->setMenuText( tr( "Bars" ) );
    barstyle->setToolTip( tr( "Bars" ) );
    barstyle->setStatusTip( tr( "Bars" ) );
	conestyle->setText( tr( "Cones" ) );
    conestyle->setMenuText( tr( "Cones" ) );
    conestyle->setToolTip( tr( "Cones" ) );
    conestyle->setStatusTip( tr( "Cones" ) );
	crossHairStyle->setText( tr( "Cross Hairs" ) );
    crossHairStyle->setMenuText( tr( "Cross Hairs" ) );
    crossHairStyle->setToolTip( tr( "Cross Hairs" ) );
    crossHairStyle->setStatusTip( tr( "Cross Hairs" ) );

	floorstyle->setText( tr( "Floor Style" ) );
    floorstyle->setMenuText( tr( "Floor Style" ) );
    floorstyle->setStatusTip( tr( "Floor Style" ) );
    floordata->setText( tr( "Floor Data Projection" ) );
    floordata->setMenuText( tr( "Floor Data Projection" ) );
    floordata->setToolTip( tr( "Floor Data Projection" ) );
    floordata->setStatusTip( tr( "Floor Data Projection" ) );
    flooriso->setText( tr( "Floor Isolines" ) );
    flooriso->setMenuText( tr( "Floor Isolines" ) );
    flooriso->setToolTip( tr( "Floor Isolines" ) );
    flooriso->setStatusTip( tr( "Floor Isolines" ) );
    floornone->setText( tr( "Empty Floor" ) );
    floornone->setMenuText( tr( "Empty Floor" ) );
    floornone->setToolTip( tr( "Empty Floor" ) );
    floornone->setStatusTip( tr( "Empty Floor" ) );
}

Graph3D * ApplicationWindow::openMatrixPlot3D(const QString& caption, const QString& matrix_name,
											  double xl,double xr,double yl,double yr,double zl,double zr)
{
QString name = matrix_name;
name.remove("matrix<", true);
name.remove(">");
Matrix* m = matrix(name);
if (!m)
	return 0;

Graph3D *plot=new Graph3D("", ws, 0, WDestructiveClose);
plot->setCaption(caption);
plot->setName(caption);
plot->addMatrixData(m, xl, xr, yl, yr, zl, zr);
plot->update();

initPlot3D(plot);
return plot;
}

void ApplicationWindow::plot3DMatrix(int style)
{
Matrix* w = (Matrix*)ws->activeWindow();
if (!w || matrixWindows.contains(w->name())<=0)
	return;

QApplication::setOverrideCursor(waitCursor);
graphs++;
QString label="graph"+QString::number(graphs);
while (plotWindows.contains(label) || plot3DWindows.contains(label) || 
		tableWindows.contains(label) || matrixWindows.contains(label))
	{
	graphs++;
	label="graph"+QString::number(graphs);
	}

Graph3D *plot=new Graph3D("", ws, 0, WDestructiveClose);
plot->addMatrixData(w);
plot->customPlotStyle(style);
customPlot3D(plot);
plot->update();
	
plot->resize(500,400);
plot->setCaption(label);
plot->setName(label);
initPlot3D(plot);

emit modified();
QApplication::restoreOverrideCursor();
}

ApplicationWindow* ApplicationWindow::importOPJ(const QString& filename)
{
QApplication::setOverrideCursor(waitCursor);

ApplicationWindow *app = new ApplicationWindow();
app->applyUserSettings();
app->setCaption("QtiPlot - " + filename);
app->showMaximized();
app->projectname = filename;
app->recentProjects.remove(filename);
app->recentProjects.push_front(filename);
app->updateRecentProjectsList();

ImportOPJ(app, filename);

QApplication::restoreOverrideCursor();
return app;
}

void ApplicationWindow::deleteFitTables()
{
QWidgetList *windows = windowsList(); 
for (int i = 0; i < int(windows->count());i++ )
	{
	QString caption = windows->at(i)->name();
	if (tableWindows.contains(caption) && 
		(caption.startsWith("Fit") || caption.startsWith("LinearFit")))
		{
		Table* t = (Table*)windows->at(i);
		if (t)
			{
			t->askOnCloseEvent(false);
			t->close();
			}
		}
	}
delete windows;
}

QWidgetList* ApplicationWindow::windowsList()
{
QWidgetList *lst = new QWidgetList;

QWidgetList windows = ws->windowList(QWorkspace::StackingOrder);
int i, n=(int)windows.count();
for (i = 0; i<n; i++ )
	lst->append(windows.at(i));

n=(int)outWindows->count();
for (i = 0; i<n; i++ )
	lst->append(outWindows->at(i));

return lst;
}

void ApplicationWindow::updateRecentProjectsList()
{
while ((int)recentProjects.size() > MaxRecentProjects)
	recentProjects.pop_back();

recent->clear();

for (int i = 0; i<(int)recentProjects.size(); i++ )
	recent->insertItem("&"+QString::number(i+1)+" "+recentProjects[i]);
}

void ApplicationWindow::translateCurveHor()
{
QWidget *w=ws->activeWindow();
if (!w || !plotWindows.contains(w->name()))
	return;

MultiLayer *plot = (MultiLayer*)w;
if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	btnPointer->setOn(true);
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if (!g)
	return;

if (g->isPiePlot())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));

	btnPointer->setOn(true);
	return;
	}
else
	{	
	activeGraph=g;
	btnPointer->setOn(true);
	g->translateCurve(0);
	info->setText(tr("Double-click on plot to select a data point!"));
	displayBar->show();
	}
}

void ApplicationWindow::translateCurveVert()
{
QWidget *w=ws->activeWindow();
if (!w || !plotWindows.contains(w->name()))
	return;

MultiLayer *plot = (MultiLayer*)w;
if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	btnPointer->setOn(true);
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if (!g)
	return;

if (g->isPiePlot())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));

	btnPointer->setOn(true);
	return;
	}
else
	{	
	activeGraph=g;
	btnPointer->setOn(true);
	g->translateCurve(1);
	info->setText(tr("Double-click on plot to select a data point!"));
	displayBar->show();
	}
}

void ApplicationWindow::setAscValues()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	w->setAscValues();
}

void ApplicationWindow::setRandomValues()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	w->setRandomValues();
}

void ApplicationWindow::setXCol()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	w->setPlotDesignation(Table::X);
}

void ApplicationWindow::setYCol()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	w->setPlotDesignation(Table::Y);
}

void ApplicationWindow::setZCol()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	w->setPlotDesignation(Table::Z);
}

void ApplicationWindow::disregardCol()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	w->setPlotDesignation(Table::None);
}

void ApplicationWindow::plotBoxDiagram()
{
Table* w = (Table*)ws->activeWindow();
if ( w && tableWindows.contains(w->name()))
	w->plotBoxDiagram();
}

void ApplicationWindow::fitMultiPeakGauss()
{
QWidget *w=ws->activeWindow();
if (!w || !plotWindows.contains(w->name()))
	return;

MultiLayer *plot = (MultiLayer*)w;
if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	btnPointer->setOn(true);
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if (!g || !g->validCurvesDataSize())
	return;

if (g->isPiePlot())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));
	return;
	}
else
	{	
	activeGraph=g;
	bool ok;
	int peaks = QInputDialog::getInteger(
            tr("QtiPlot - Enter the number of peaks"), tr("Peaks"), 2, 0, 1000000, 1,
            &ok, this );
	if (ok && peaks) 
		{
		g->multiPeakFit(0, peaks);
		info->setText(tr("Move cursor and click to select a point and double-click/press 'Enter' to set the position of a peak!"));
		displayBar->show();
		connect (g,SIGNAL(showFitResults(const QString&)), this, SLOT(showResults(const QString&)));
		}
	}
}

void ApplicationWindow::fitMultiPeakLorentz()
{
QWidget *w=ws->activeWindow();
if (!w || !plotWindows.contains(w->name()))
	return;

MultiLayer *plot = (MultiLayer*)w;
if (plot->isEmpty())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					  "<p><h4>Please add a layer and try again!</h4>"));
	btnPointer->setOn(true);
	return;
	}
	
Graph* g = (Graph*)plot->activeGraph();
if (!g || !g->validCurvesDataSize())
	return;

if (g->isPiePlot())
	{
	QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));
	return;
	}
else
	{	
	activeGraph=g;
	bool ok;
	int peaks = QInputDialog::getInteger(tr("QtiPlot - Enter the number of peaks"), 
				tr("Peaks"), 2, 0, 1000000, 1, &ok, this);
	if (ok && peaks) 
		{
		g->multiPeakFit(1, peaks);
		info->setText(tr("Move cursor and click to select a point and double-click/press 'Enter' to set the position of a peak!"));
		displayBar->show();
		connect (g,SIGNAL(showFitResults(const QString&)), this, SLOT(showResults(const QString&)));
		}
	}
}

void ApplicationWindow::showSupportPage()
{
open_browser(this, "http://soft.proindependent.com/contracts.html");
}


void ApplicationWindow::showDonationsPage()
{
open_browser(this, "http://soft.proindependent.com/why_donate.html");
}

void ApplicationWindow::downloadManual()
{
open_browser(this, "http://soft.proindependent.com/manuals.html");
}

void ApplicationWindow::downloadTranslation()
{
open_browser(this, "http://soft.proindependent.com/translations.html");
}

void ApplicationWindow::checkUpdates()
{
QUrlOperator *op = new QUrlOperator();
connect(op, SIGNAL(finished(QNetworkOperation *)), 
		this, SLOT(checkUpdates(QNetworkOperation *)));
//op.get();
op->copy("http://soft.proindependent.com/version.txt", "./tmp.txt", false, true);
}

void ApplicationWindow::checkUpdates(QNetworkOperation *op)
{
QMessageBox::information(this, "", QString::number(op->errorCode()));
}

void ApplicationWindow::showHomePage()
{
open_browser(this, "http://soft.proindependent.com/qtiplot.html");
}

bool ApplicationWindow::open_browser(QWidget* parent, const QString& rUrl)
{
bool result = false;
QApplication::setOverrideCursor(Qt::WaitCursor);
#ifdef Q_WS_WIN
	#if defined(_MSC_VER) //MSVC compiler
		result = int(ShellExecuteW(parent->winId(), 0, rUrl.ucs2(), 0, 0, SW_SHOWNORMAL)) > 32;
	#else //MinGW compiler
		QApplication::restoreOverrideCursor();

		QMessageBox::information(this, "QtiPlot - Error", 
		tr("Sorry, QtiPlot couldn't start the default browser! Please start a browser manually and visit the following link")+":\n"+rUrl);
	#endif
#else
    Q_UNUSED(parent);
    //Try a range of browsers available on UNIX, until we (hopefully) find one that works.  
	//Start with the most popular first.
    QProcess process;
    bool process_started = false;
    process.setArguments(QStringList() << "firefox" << rUrl);
    process_started = process.start();
    if (!process_started)
		{
        process.setArguments(QStringList() << "mozilla" << rUrl);
        process_started = process.start();
        }
	if (!process_started)
		{
		process.setArguments(QStringList() << "netscape" << rUrl);
		process_started = process.start();
		}
	if (!process_started)
		{
		process.setArguments(QStringList() << "konqueror" << rUrl);
		process_started = process.start();
		}
	result = process_started;
#endif
		
QApplication::restoreOverrideCursor();
return result;
} 
	  
void ApplicationWindow::showDonationDialog()
{
if (askForSupport)
	{
	QString s= tr("<font size=+2, color = darkBlue><b>QtiPlot is open-source software and its development required hundreds of hours of work.<br><br>\
				   If you like it, you're using it in your work and you would like to see it \
				   constantly improved,<br> please support its author by making a donation.<br><br> \
				   Would you like to make a donation for QtiPlot now?</b></font>");
    switch( QMessageBox::information(this, tr("Please support QtiPlot!"), s,
		 tr("Yes, I'd love to!"), tr("Ask me again later!"), tr("No, stop bothering me!"), 0, 1 ) )
		{
		case 0:
			showDonationsPage();
		break;
	
		case 1:
		break;

		case 2:
			askForSupport = false;
		break;
		}
	}
}

void ApplicationWindow::parseCommandLineArgument(const QString& s, int args)
{
if (s == "-v" || s == "--version")
	{
	initGlobalConstants();
	about();
	exit(0);
	}
else if (s == "-h" || s == "--help")
	{
	QMessageBox::warning(0, tr("QtiPlot - Warning"), tr("Help is not available on command line.\
	In order to download the user manual for QtiPlot, please visit:<br><br>\
    <font color = blue> http://soft.proindependent.com/help.html</font>"));
	exit(0);
	}
else if (s.contains("-lang=") || s.contains("-l="))
	{
	if (args == 1)
		{
		init();
		applyUserSettings();
		}

	QString locale = s.mid(s.find('=')+1);
	if (locales.contains(locale))
		switchToLanguage(locale);

	if (args == 1)
		{
		newTable();
		setCaption(tr("QtiPlot - untitled"));
		showMaximized();
		saved=TRUE;
		actionSaveProject->setEnabled(false);
		}

	if (!locales.contains(locale))
		QMessageBox::critical(0, tr("QtiPlot - Error"),
		tr("<b> %1 </b>: Wrong locale option or no translation available!").arg(locale));
	}
else
	{
	QMessageBox::critical(0, tr("QtiPlot - Error"),
				tr("<b> %1 </b>: Unknown command line option or the file doesn't exist!").arg(s));
	exit(1);
	}
}

void ApplicationWindow::createLanguagesList()
{
appTranslator = new QTranslator(this);
qtTranslator = new QTranslator(this);
qApp->installTranslator(appTranslator);
qApp->installTranslator(qtTranslator);

QString qmPath = qApp->applicationDirPath() + "/translations";
QDir dir(qmPath);
QStringList fileNames = dir.entryList("qtiplot_*.qm");
for (int i=0; i < (int)fileNames.size(); i++)
	{
	QString locale = fileNames[i];
	locale = locale.mid(locale.find('_')+1);
	locale.truncate(locale.find('.'));
	locales.push_back(locale);
	}
locales.push_back("en");
locales.sort();

if (appLanguage != "en")
	{
	appTranslator->load("qtiplot_" + appLanguage, qmPath);
	qtTranslator->load("qt_" + appLanguage, qmPath+"/qt");
	}
}

void ApplicationWindow::switchToLanguage(int param)
{
if (param < (int)locales.size())
	switchToLanguage(locales[param]);
}

void ApplicationWindow::switchToLanguage(const QString& locale)
{
if (!locales.contains(locale) || appLanguage == locale)
	return;

appLanguage = locale;
if (locale == "en")
	{
	appTranslator->clear();
	qtTranslator->clear();
	}
else
	{
	QString qmPath = qApp->applicationDirPath() + "/translations";
	appTranslator->load("qtiplot_" + locale, qmPath);
	qtTranslator->load("qt_" + locale, qmPath+"/qt");
	}
insertTranslatedStrings();
}

bool ApplicationWindow::alreadyUsedName(const QString& label)
{
if (plotWindows.contains(label) || plot3DWindows.contains(label) ||
	tableWindows.contains(label) || matrixWindows.contains(label) || 
	noteWindows.contains(label))
	return true;

return false;
}

void ApplicationWindow::appendProject()
{
QString filter = tr("QtiPlot project") + " (*.qti);;";
filter += tr("Compressed QtiPlot project") + " (*.qti.gz);;";

QString fn = QFileDialog::getOpenFileName(workingDir, filter, this, 0,
			tr("QtiPlot - Open Project"), 0, TRUE);

if (fn.isEmpty())
	return;

QFileInfo fi(fn);
workingDir = fi.dirPath(true);
		
if (fn.contains(".qti",TRUE))
	{
	QFileInfo f(fn);
	if (!f.exists ())
		{
		QMessageBox::critical(this, tr("QtiPlot - File openning error"),
				tr("The file: <b>%1</b> doesn't exist!").arg(fn));
		return;
		}
	}
else
	{
	QMessageBox::critical(this,tr("QtiPlot - File openning error"),
				tr("The file: <b>%1</b> is not a QtiPlot project file!").arg(fn));
	return;
	}

QApplication::setOverrideCursor(waitCursor);

QString fname = fn;
if (fn.contains(".qti.gz"))
	{//decompress using zlib
	file_uncompress((char *)fname.ascii());
	fname.remove(".gz");
	}

QFile f(fname);
QTextStream t( &f );
t.setEncoding(QTextStream::UnicodeUTF8);
f.open(IO_ReadOnly);

QString s = t.readLine();
QStringList lst = QStringList::split (QRegExp("\\s"),s,false);
QString version = lst[1];
lst = QStringList::split (".", version, false);
fileVersion =100*(lst[0]).toInt()+10*(lst[1]).toInt()+(lst[2]).toInt();

t.readLine(); 
if (fileVersion < 73)
	t.readLine();

Folder *cf = current_folder;
FolderListItem *item = (FolderListItem *)current_folder->folderListItem();
folders->blockSignals (true);
blockSignals (true);

QString baseName = fi.baseName();
lst = current_folder->subfolders();
int n = (int)lst.contains(baseName);
if (n)
	{//avoid identical subfolder names
	while ((int)lst.contains(baseName + QString::number(n)))
			n++;
	baseName += QString::number(n);
	}

current_folder = new Folder(current_folder, baseName);
FolderListItem *fli = new FolderListItem(item, current_folder);
current_folder->setFolderListItem(fli);

//process tables and matrix information
while ( !t.eof())
	{
	s = t.readLine();
	lst.clear();
	if  (s.left(8) == "<folder>")
		{
		lst = QStringList::split ("\t",s,TRUE);
		Folder *f = new Folder(current_folder, lst[1]);
		f->setBirthDate(lst[2]);
		f->setModificationDate(lst[3]);
		if (lst[4] == "current")
			cf = f;

		FolderListItem *fli = new FolderListItem(current_folder->folderListItem(), f);
		fli->setText(0, lst[1]);
		f->setFolderListItem(fli);
	
		current_folder = f;
		}
	else if  (s == "<table>")
		{
		if (fileVersion < 69)
			{
			while ( s!="</table>" )
				{
				s=t.readLine();
				lst<<s;
				}
			 openTable(this,lst);
			}
		else
			{
			while ( s != "<data>" )
				{
				s=t.readLine();
				lst<<s;
				}
			Table *w = openTable(this, lst);
			int cols = w->tableCols();				
			s = t.readLine();
			while ( s != "</data>" )
				{
				w->addDataRow(s, cols);
				s = t.readLine();
				}				
			}
		}
	else if  (s == "<matrix>")
		{
		while ( s != "<data>" )
			{
			s=t.readLine();
			lst<<s;
			}
		Matrix *w = openMatrix(this, lst);
		int cols = w->numCols();				
		s = t.readLine();
		while ( s != "</data>" )
			{
			w->addDataRow(s, cols);
			s = t.readLine();
			}
		}
	else if  (s == "<note>")
		{
		for (int i=0; i<3; i++)
			{
			s = t.readLine();
			lst << s;
			}
		Note* m = openNote(this, lst);
		QString text = QString::null;
		while ( s != "</note>" )
			{
			s=t.readLine();
			text += s+"\n";
			}
		m->setText(text.remove("</note>\n"));
		}
	else if  (s == "</folder>")
		{
		Folder *parent = (Folder *)current_folder->parent();
		if (!parent)
			current_folder = projectFolder();
		else
			current_folder = parent;
		}
	}
f.close();

//process the rest
f.open(IO_ReadOnly);

MultiLayer *plot=0;
while ( !t.eof())
	{
	s=t.readLine();
	if  (s.left(8) == "<folder>")
		{
		lst = QStringList::split ("\t",s,TRUE);
		current_folder = current_folder->findSubfolder(lst[1]);
		}
	else if  (s == "<multiLayer>")
		{//process multilayers information
		s=t.readLine();
		QStringList graph=QStringList::split ("\t",s,TRUE);
		QString caption=graph[0];
		plot=multilayerPlot(caption);
		plot->setCols(graph[1].toInt());
		plot->setRows(graph[2].toInt());
		QString date=QString::null;
		if (fileVersion < 63)
			date = graph[5];
		else
			date = graph[3];

		setListViewDate(caption,date);
		plot->setBirthDate(date);
		plot->blockSignals(true);	

		restoreWindowGeometry(this, plot, t.readLine());
		
		if (fileVersion > 71)
			{
			QStringList lst=QStringList::split ("\t", t.readLine(), true);
			plot->setWindowLabel(lst[1]);
			setListViewLabel(plot->name(),lst[1]);
			plot->setCaptionPolicy((myWidget::CaptionPolicy)lst[2].toInt());
			}

		if (caption.contains ("graph",TRUE))
			{
			bool ok;
			int gr=caption.remove("graph").toInt(&ok);
			if (gr > graphs && ok) 
				graphs = gr;
			}

		if (fileVersion > 83)
			{
			QStringList lst=QStringList::split ("\t", t.readLine(), false);
			plot->setMargins(lst[1].toInt(),lst[2].toInt(),lst[3].toInt(),lst[4].toInt());
			lst=QStringList::split ("\t", t.readLine(), false);
			plot->setSpacing(lst[1].toInt(),lst[2].toInt());
			lst=QStringList::split ("\t", t.readLine(), false);
			plot->setLayerCanvasSize(lst[1].toInt(),lst[2].toInt());
			lst=QStringList::split ("\t", t.readLine(), false);
			plot->setAlignement(lst[1].toInt(),lst[2].toInt());
			}

		while ( s!="</multiLayer>" )
			{//open layers
			s=t.readLine();
			if (s.left(7)=="<graph>")
				{
				lst.clear();
				while ( s!="</graph>" )
					{
					s=t.readLine();
					lst<<s;
					}
				openGraph(this,plot, lst);
				}
			}
		plot->blockSignals(false);
		}
	else if  (s == "<SurfacePlot>")
		{//process 3D plots information
		lst.clear();
		while ( s!="</SurfacePlot>" )
			{
			s=t.readLine();
			lst<<s;
			}
		openSurfacePlot(this,lst);
		}
	else if  (s == "</folder>")
		{
		Folder *parent = (Folder *)current_folder->parent();
		if (!parent)
			current_folder = projectFolder();
		else
			current_folder = parent;
		}
	}
f.close();
			
folders->blockSignals (false);
//change folder to user defined current folder
changeFolder(cf);
blockSignals (false);
renamedTables = QStringList();
QApplication::restoreOverrideCursor();
}

void ApplicationWindow::saveFolder(Folder *folder, const QString& fn)
{
QFile f( fn );
if (f.exists())
	{// make byte-copy of current file so that there's always a copy of the data on disk
	QFile backup(fn + "~");
    while (!f.open(IO_ReadOnly) || !backup.open(IO_WriteOnly))
      {
       if (f.isOpen()) 
		   f.close();
       if (backup.isOpen()) 
		   backup.close();
       int choice = QMessageBox::warning(this, tr("QtiPlot - File Backup Error"),
	   tr("Cannot make a backup copy of <b>%1</b> (to %2).<br>If you ignore this, you run the risk of <b>data loss</b>.").arg(projectname).arg(projectname+"~"),
	  QMessageBox::Retry|QMessageBox::Default, QMessageBox::Abort|QMessageBox::Escape, QMessageBox::Ignore);
      if (choice == QMessageBox::Abort) 
		  return;
      if (choice == QMessageBox::Ignore) 
		  break;
      }

   if (f.isOpen() && backup.isOpen())
    {
     while (!f.atEnd())
        backup.putch(f.getch());

     backup.close();
     f.close();
    }
  }

if ( !f.open( IO_WriteOnly ) )
	{
	QMessageBox::about(this, tr("QtiPlot - File Save Error"), tr("The file: <br><b>%1</b> is opened in read-only mode").arg(fn));
	return;
	}
QApplication::setOverrideCursor(waitCursor);

QPtrList<myWidget> lst = folder->windowsList();
myWidget *w;
int windows = 0;
QString text;
for (w = lst.first(); w; w = lst.next())
	{
	text += w->saveToString(windowGeometryInfo(w));
	windows++;
	}

FolderListItem *fi = folder->folderListItem();
FolderListItem *item = (FolderListItem *)fi->firstChild();
int opened_folders = 0;
int initial_depth = fi->depth();
while (item && item->depth() > initial_depth)
	{
	Folder *dir = (Folder *)item->folder();
	text += "<folder>\t"+dir->folderName()+"\t"+dir->birthDate()+"\t"+dir->modificationDate();
	if (dir == current_folder)
		text += "\tcurrent\n";
	else
		text += "\n";

	lst = dir->windowsList();
	for (w = lst.first(); w; w = lst.next())
		{
		text += w->saveToString(windowGeometryInfo(w));
		windows++;
		}

	if (!dir->children())
		text += "</folder>\n";
	else
		opened_folders++;

	int depth = item->depth();
	item = (FolderListItem *)item->itemBelow();
	if (item && item->depth() < depth && item->depth() > initial_depth)
		{
		text += "</folder>\n";
		opened_folders--;
		}
	else if (!item)
		{
		for (int i = 0; i<opened_folders; i++)
			text += "</folder>\n";
		opened_folders = 0;
		}
	}
text += "<log>\n"+logInfo+"</log>";
text.prepend("<windows>\t"+QString::number(windows)+"\n");
text.prepend("QtiPlot " + QString::number(majVersion)+"."+ QString::number(minVersion)+"."+
			QString::number(patchVersion)+" project file\n");

QTextStream t( &f );
t.setEncoding(QTextStream::UnicodeUTF8);
t << text;
f.close();

QApplication::restoreOverrideCursor();
}

void ApplicationWindow::saveAsProject()
{
saveFolderAsProject(current_folder);
}

void ApplicationWindow::saveFolderAsProject(Folder *f)
{
QString filter = tr("QtiPlot project")+" (*.qti);;";
filter += tr("Compressed QtiPlot project")+" (*.qti.gz)";

QString selectedFilter;
QString fn = QFileDialog::getSaveFileName(workingDir, filter, this, "project",
			tr("Save Project As"), &selectedFilter, false);
if ( !fn.isEmpty() )
	{
	QFileInfo fi(fn);
	workingDir = fi.dirPath(true);
	QString baseName = fi.fileName();	
	if (!baseName.contains("."))
		fn.append(".qti");
	
	if ( QFile::exists(fn) && !selectedFilter.contains(".gz") &&
        QMessageBox::question(this, tr("QtiPlot -- Overwrite File? "),
            tr("A file called: <p><b>%1</b><p>already exists.\n"
                "Do you want to overwrite it?")
                .arg(fn), tr("&Yes"), tr("&No"),QString::null, 0, 1 ) )
        return ;
	else
		{
		saveFolder(f, fn);
		if (selectedFilter.contains(".gz"))
			file_compress((char *)fn.ascii(), "wb9");
		}
    }
}

void ApplicationWindow::showFolderPopupMenu(QListViewItem *it, const QPoint &p, int)
{
showFolderPopupMenu(it, p, true);
}

//! fromFolders = TRUE means the user clicked right mouse buttom on a list iten from QListView "folders"
void ApplicationWindow::showFolderPopupMenu(QListViewItem *it, const QPoint &p, bool fromFolders)
{
if (!it || folders->isRenaming())
	return;

QPopupMenu cm(this);
QPopupMenu window(this);
QPopupMenu viewWindowsMenu(this);
viewWindowsMenu.setCheckable ( true );

cm.insertItem(tr("&Find..."), this, SLOT(showFindDialogue()));	
cm.insertSeparator();
cm.insertItem(tr("App&end Project..."), this, SLOT(appendProject()));
if (((FolderListItem *)it)->folder()->parent())
	cm.insertItem(tr("Save &As Project..."), this, SLOT(saveAsProject()));
else
	cm.insertItem(tr("Save Project &As..."), this, SLOT(saveProjectAs()));
cm.insertSeparator();

if (fromFolders && show_windows_policy != HideAll)
	{
	cm.insertItem(tr("&Show All Windows"), this, SLOT(showAllFolderWindows()));
	cm.insertItem(tr("&Hide All Windows"), this, SLOT(hideAllFolderWindows()));
	cm.insertSeparator();
	}

if (((FolderListItem *)it)->folder()->parent())
	{
	cm.insertItem(QPixmap(close_xpm), tr("&Delete Folder"), this, SLOT(deleteFolder()), Key_F8);
	cm.insertItem(tr("&Rename"), this, SLOT(startRenameFolder()), Key_F2);
	cm.insertSeparator();
	}

if (fromFolders)
	{
	actionNewTable->addTo(&window);
	actionNewMatrix->addTo(&window);
	actionNewNote->addTo(&window);
	actionNewGraph->addTo(&window);
	actionNewFunctionPlot->addTo(&window);
	actionNewSurfacePlot->addTo(&window);
	cm.insertItem(tr("New &Window"), &window);
	}

cm.insertItem(QPixmap(newfolder_xpm), tr("New F&older"), this, SLOT(addFolder()), Key_F7);
cm.insertSeparator();

QStringList lst;
lst << tr("&None") << tr("&Windows in Active Folder") << tr("Windows in &Active Folder && Subfolders");
for (int i = 0; i < 3; ++i) 
	{
    int id = viewWindowsMenu.insertItem(lst[i],this, SLOT( setShowWindowsPolicy( int ) ) );
    viewWindowsMenu.setItemParameter( id, i );
    viewWindowsMenu.setItemChecked( id, show_windows_policy == i );
    }
cm.insertItem(tr("&View Windows"), &viewWindowsMenu);
cm.insertSeparator();
cm.insertItem(tr("&Properties..."), this, SLOT(folderProperties()));
cm.exec(p);
}

void ApplicationWindow::setShowWindowsPolicy(int p)
{
if (show_windows_policy == (ShowWindowsPolicy)p)
	return;

show_windows_policy = (ShowWindowsPolicy)p;
if (show_windows_policy == HideAll)
	{
	QWidgetList *lst = windowsList(); 
	QWidget *w;
	for (w = lst->first(); w; w = lst->next())
		{
		hiddenWindows->append(w);
		w->hide();
		setListView(w->name(),tr("Hidden"));
		}
	delete lst;
	}
else
	showAllFolderWindows();		
}

void ApplicationWindow::showFindDialogue()
{
findDialog *fd = new findDialog(this, 0, TRUE, WStyle_Tool|WDestructiveClose);
fd->showNormal();
fd->setActiveWindow();
}

void ApplicationWindow::startRenameFolder()
{
FolderListItem *fi = current_folder->folderListItem();
if (!fi)
	return;

disconnect(folders, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(folderItemChanged(QListViewItem *)));
fi->setRenameEnabled (0, true);
fi->startRename (0);
}

void ApplicationWindow::startRenameFolder(QListViewItem *item)
{
if (!item || item == folders->firstChild())
	return;

disconnect(folders, SIGNAL(currentChanged(QListViewItem *)), this, SLOT(folderItemChanged(QListViewItem *)));

if (item->listView() == lv && item->rtti() == FolderListItem::ListItemType)
	{
	current_folder = ((FolderListItem *)item)->folder();
	FolderListItem *it = current_folder->folderListItem();
	it->setRenameEnabled (0, true);
	it->startRename (0);
	}
else
	{
	item->setRenameEnabled (0, true);
	item->startRename (0);
	}
}

void ApplicationWindow::renameFolder(QListViewItem *it, int col, const QString &text)
{
if (!it)
	return;

Folder *parent = (Folder *)current_folder->parent();
if (!parent)//the parent folder is the project folder (it always exists)
	parent = projectFolder();

while(text.isEmpty())
	{
	QMessageBox::critical(this,tr("QtiPlot - Error"), tr("Please enter a valid name!"));
	it->setRenameEnabled (0, true);
	it->startRename (0);
	return;
	}

QStringList lst = parent->subfolders();
lst.remove(current_folder->folderName());
while(lst.contains(text))
	{
	QMessageBox::critical(this,tr("QtiPlot - Error"),
				tr("Name already exists!")+"\n"+tr("Please choose another name!"));

	it->setRenameEnabled (0, true);
	it->startRename (0);
	return;
	}

current_folder->setFolderName(text);
it->setRenameEnabled (0, false);
connect(folders, SIGNAL(currentChanged(QListViewItem *)), 
		this, SLOT(folderItemChanged(QListViewItem *)));
folders->setCurrentItem(parent->folderListItem());//update the list views
}

void ApplicationWindow::showAllFolderWindows()
{
QPtrList<myWidget> lst = current_folder->windowsList();
myWidget *w;
for (w = lst.first(); w; w = lst.next())
	{//force show all windows in current folder
	if (w)
		{
		updateWindowLists(w);
		switch (w->status())
			{
			case myWidget::Hidden:
				w->showNormal();
			break;

			case myWidget::Normal:
				w->showNormal();
			break;

			case myWidget::Minimized:
				w->showMinimized();
			break;

			case myWidget::Maximized:
				w->showMaximized();
			break;
			}
		}
	}

if (!current_folder->children())
	return;

FolderListItem *fi = current_folder->folderListItem();
FolderListItem *item = (FolderListItem *)fi->firstChild();
int initial_depth = item->depth();
while (item && item->depth() >= initial_depth)
	{// show/hide windows in all subfolders
	lst = ((Folder *)item->folder())->windowsList();
	for (w = lst.first(); w; w = lst.next())
		{
		if (w && show_windows_policy == SubFolders)
			{
			updateWindowLists(w);
			switch (w->status())
				{
				case myWidget::Hidden:
					w->showNormal();
				break;

				case myWidget::Normal:
					w->showNormal();
				break;

				case myWidget::Minimized:
					w->showMinimized();
				break;

				case myWidget::Maximized:
					w->showMaximized();
				break;
				}
			}
		else
			w->hide();
		}

	item = (FolderListItem *)item->itemBelow();
	}
}

void ApplicationWindow::hideAllFolderWindows()
{
QPtrList<myWidget> lst = current_folder->windowsList();
myWidget *w;
for (w = lst.first(); w; w = lst.next())
	hideWindow(w);

if (!current_folder->children())
	return;

if (show_windows_policy == SubFolders)
	{
	FolderListItem *fi = current_folder->folderListItem();
	FolderListItem *item = (FolderListItem *)fi->firstChild();
	int initial_depth = item->depth();
	while (item && item->depth() >= initial_depth)
		{
		lst = item->folder()->windowsList();
		for (w = lst.first(); w; w = lst.next())
			hideWindow(w);

		item = (FolderListItem *)item->itemBelow();
		}
	}
}

void ApplicationWindow::projectProperties()
{
QString s = current_folder->folderName() + "\n\n";
s += "\n\n\n";
s += tr("Type") + ": " + tr("Project")+"\n\n";
if (projectname != "untitled")
	{
	s += tr("Path") + ": " + projectname + "\n\n";

	QFileInfo fi(projectname);
	s += tr("Size") + ": " + QString::number(fi.size()) + " " + tr("bytes")+ "\n\n";
	}

QWidgetList *lst = windowsList();
s += tr("Contents") + ": " + QString::number(lst->count()) + " " + tr("Windows");
delete lst;

s += ", " + QString::number(current_folder->subfolders().count()) + " " + tr("Folders") + "\n\n";
s += "\n\n\n";

if (projectname != "untitled")
	{
	QFileInfo fi(projectname);
	s += tr("Created") + ": " + fi.created().toString(Qt::LocalDate) + "\n\n";
	s += tr("Modified") + ": " + fi.lastModified().toString(Qt::LocalDate) + "\n\n";
	}
else
	s += tr("Created") + ": " + current_folder->birthDate() + "\n\n";

QMessageBox *mbox = new QMessageBox ( tr("Properties"), s, QMessageBox::NoIcon, 
						QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, this);

mbox->setIconPixmap(QPixmap( qtiplot_logo_xpm ));
mbox->show();
}

void ApplicationWindow::folderProperties()
{
if (!current_folder->parent())
	{
	projectProperties();
	return;
	}

QString s = current_folder->folderName() + "\n\n";
s += "\n\n\n";
s += tr("Type") + ": " + tr("Folder")+"\n\n";
s += tr("Path") + ": " + current_folder->path() + "\n\n";
s += tr("Size") + ": " + current_folder->sizeToString() + "\n\n";
s += tr("Contents") + ": " + QString::number(current_folder->windowsList().count()) + " " + tr("Windows");
s += ", " + QString::number(current_folder->subfolders().count()) + " " + tr("Folders") + "\n\n";
//s += "\n\n\n";
s += tr("Created") + ": " + current_folder->birthDate() + "\n\n";
//s += tr("Modified") + ": " + current_folder->modificationDate() + "\n\n";

QMessageBox *mbox = new QMessageBox ( tr("Properties"), s, QMessageBox::NoIcon, 
						QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, this);

mbox->setIconPixmap(QPixmap( folder_open_xpm ));
mbox->show();
}

void ApplicationWindow::addFolder()
{
QStringList lst = current_folder->subfolders();
QString name =  tr("New Folder");
lst = lst.grep( name );
if (!lst.isEmpty())
	name += " ("+ QString::number(lst.size()+1)+")";

Folder *f = new Folder(current_folder, name);
addFolderListViewItem(f);

FolderListItem *fi = new FolderListItem(current_folder->folderListItem(), f);
if (fi)
	{
	f->setFolderListItem(fi);
	fi->setRenameEnabled (0, true);
	fi->startRename(0);
	}
}

bool ApplicationWindow::deleteFolder(Folder *f)
{
if (confirmCloseFolder && QMessageBox::information(this, tr("QtiPlot - Delete folder?"),
	tr("Delete folder '%1' and all the windows it contains?").arg(f->folderName()),
	tr("Yes"), tr("No"), 0, 0))
	return false;
else
	{
	FolderListItem *fi = f->folderListItem();
	QPtrList<myWidget> lst = f->windowsList();
	myWidget *w;
	for (w = lst.first(); w; w = lst.next())
		removeWindowFromLists(w);

	if (f->children())
		{
		FolderListItem *item = (FolderListItem *)fi->firstChild();
		int initial_depth = item->depth();
		while (item && item->depth() >= initial_depth)
			{
			lst = ((Folder *)item->folder())->windowsList();
			for (w = lst.first(); w; w = lst.next())
				removeWindowFromLists(w);

			item = (FolderListItem *)item->itemBelow();
			}
		}

	delete f;
	delete fi;
	return true;
	}
}

void ApplicationWindow::deleteFolder()
{
Folder *parent = (Folder *)current_folder->parent();
if (!parent)
	parent = projectFolder();

folders->blockSignals(true);

if (deleteFolder(current_folder))
	{
	current_folder = parent;
	folders->setCurrentItem(parent->folderListItem());
	changeFolder(parent, true);
	}

folders->blockSignals(false);
folders->setFocus();
}

void ApplicationWindow::folderItemDoubleClicked(QListViewItem *it)
{
if (!it || it->rtti() != FolderListItem::ListItemType)
	return;

FolderListItem *item = ((FolderListItem *)it)->folder()->folderListItem();
folders->setCurrentItem(item);
}

void ApplicationWindow::folderItemChanged(QListViewItem *it)
{
if (!it)
	return;

it->setOpen(true);
changeFolder (((FolderListItem *)it)->folder());
folders->setFocus();
}

void ApplicationWindow::hideFolderWindows(Folder *f)
{
QPtrList<myWidget> lst = f->windowsList();
myWidget *w;
for (w = lst.first(); w; w = lst.next())
	{
	if (w && !w->isHidden())
		w->hide();
	}

if (!f->children())
	return;

FolderListItem *fi = f->folderListItem();
FolderListItem *item = (FolderListItem *)fi->firstChild();
int initial_depth = item->depth();
while (item && item->depth() >= initial_depth)
	{
	lst = item->folder()->windowsList();
	for (w = lst.first(); w; w = lst.next())
		{
		if (w && w->isVisible())
			w->hide();
		}
	item = (FolderListItem *)item->itemBelow();
	}
}

void ApplicationWindow::changeFolder(Folder *newFolder, bool force)
{
desactivateFolders();
newFolder->folderListItem()->setActive(true);

if (current_folder == newFolder && !force)
	return;

hideFolderWindows(current_folder);
current_folder = newFolder;

lv->clear();

QObjectList* folderLst = (QObjectList*)newFolder->children();
if (folderLst)
	{
	Folder *f;
	for (f = (Folder*)folderLst->first(); f; f = (Folder*)folderLst->next())
		addFolderListViewItem(f);
	}

QPtrList<myWidget> lst = newFolder->windowsList();
myWidget *w;
for (w = lst.first(); w; w = lst.next())
	{//show only windows in the current folder which are not hidden by the user
	if (w)
		{
		if (!hiddenWindows->containsRef(w) && !outWindows->containsRef(w) &&
			show_windows_policy != HideAll)
			{
			switch (w->status())
				{
				case myWidget::Normal:
					w->showNormal();
				break;
				case myWidget::Minimized:
					w->showMinimized();
				break;
				case myWidget::Maximized:
					{
					if (w->isA("Graph3D"))
						((Graph3D *)w)->setIgnoreFonts(true);

					w->showMaximized();

					if (w->isA("Graph3D"))
						((Graph3D *)w)->setIgnoreFonts(false);
					}
				break;
				}
			}
		else
			w->setStatus(myWidget::Hidden);

		addListViewItem(w);
		}
	}

if (!newFolder->children())
	return;

FolderListItem *fi = newFolder->folderListItem();
FolderListItem *item = (FolderListItem *)fi->firstChild();
int initial_depth = item->depth();
while (item && item->depth() >= initial_depth)
	{//show/hide windows in subfolders
	lst = ((Folder *)item->folder())->windowsList();
	for (w = lst.first(); w; w = lst.next())
		{
		if (w &&!hiddenWindows->containsRef(w) && !outWindows->containsRef(w))
			{
			if (show_windows_policy == SubFolders)
				{
				switch (w->status())
					{
					case myWidget::Normal:
						w->showNormal();
					break;
					case myWidget::Minimized:
						w->showMinimized();
					break;
					case myWidget::Maximized:
						if (w->isA("Graph3D"))
							((Graph3D*)w)->setIgnoreFonts(true);
							
						w->showMaximized();

						if (w->isA("Graph3D"))
							((Graph3D*)w)->setIgnoreFonts(false);
					break;
					}
				}
			else if (w->isVisible())
				w->hide();
			}
		}

	item = (FolderListItem *)item->itemBelow();
	}
}

void ApplicationWindow::desactivateFolders()
{
FolderListItem *item = (FolderListItem *)folders->firstChild();
while (item)
	{
	item->setActive(false);
	item = (FolderListItem *)item->itemBelow();
	}
}

void ApplicationWindow::addListViewItem(myWidget *w)
{
if (!w)
	return;

WindowListItem* it = new WindowListItem(lv, w);

if (w->isA("Matrix"))
	{
	it->setPixmap(0, QPixmap(matrix_xpm));
	it->setText(1, tr("Matrix"));
	}
else if (w->isA("Table"))
	{
	it->setPixmap(0, QPixmap(worksheet_xpm));
	it->setText(1, tr("Table"));
	}
else if (w->isA("Note"))
	{
	it->setPixmap(0, QPixmap(note_xpm));
	it->setText(1, tr("Note"));
	}
else if (w->isA("MultiLayer"))
	{
	it->setPixmap(0, QPixmap(graph_xpm));
	it->setText(1, tr("Plot"));
	}
else if (w->isA("Graph3D"))
	{
	it->setPixmap(0, QPixmap(trajectory_xpm));
	it->setText(1, tr("Plot 3D"));
	}

it->setText(0, w->name());
if (w->isHidden())
	it->setText(2, tr("Hidden"));
else
	it->setText(2, w->aspect());

it->setText(3, w->sizeToString());
it->setText(4, w->birthDate());
it->setText(5, w->windowLabel());
}

void ApplicationWindow::windowProperties()
{
WindowListItem *it = (WindowListItem *)lv->currentItem();
myWidget *w = it->window();
if (!w)
	return;

QMessageBox *mbox = new QMessageBox ( tr("Properties"), QString::null, QMessageBox::NoIcon, 
						QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, this);

QString s = QString(w->name()) + "\n\n";
s += "\n\n\n";

s += tr("Label") + ": " + ((myWidget *)w)->windowLabel() + "\n\n";

if (w->isA("Matrix"))
	{
	mbox->setIconPixmap(QPixmap(matrix_xpm));
	s +=  tr("Type") + ": " + tr("Matrix") + "\n\n";
	}
else if (w->isA("Table"))
	{
	mbox->setIconPixmap(QPixmap(worksheet_xpm));
	s +=  tr("Type") + ": " + tr("Table") + "\n\n";
	}
else if (w->isA("Note"))
	{
	mbox->setIconPixmap(QPixmap(note_xpm));
	s +=  tr("Type") + ": " + tr("Note") + "\n\n";
	}
else if (w->isA("MultiLayer"))
	{
	mbox->setIconPixmap(QPixmap(graph_xpm));
	s +=  tr("Type") + ": " + tr("Plot") + "\n\n";
	}
else if (w->isA("Graph3D"))
	{
	mbox->setIconPixmap(QPixmap(trajectory_xpm));
	s +=  tr("Type") + ": " + tr("Plot 3D") + "\n\n";
	}
s += tr("Path") + ": " + current_folder->path() + "\n\n";
s += tr("Size") + ": " + w->sizeToString() + "\n\n";
s += tr("Created") + ": " + w->birthDate() + "\n\n";
s += tr("Status") + ": " + it->text(2) + "\n\n";
mbox->setText(s);
mbox->show();
}

void ApplicationWindow::addFolderListViewItem(Folder *f)
{
if (!f)
	return;

FolderListItem* it = new FolderListItem(lv, f);
it->setActive(false);
it->setText(0, f->folderName());
it->setText(1, tr("Folder"));
it->setText(3, f->sizeToString());
it->setText(4, f->birthDate());
}

void ApplicationWindow::find(const QString& s, bool windowNames, bool labels, 
							 bool folderNames, bool caseSensitive, bool partialMatch, 
							 bool subfolders)
{
if (windowNames || labels)
	{
	myWidget *w = current_folder->findWindow(s,windowNames,labels,caseSensitive,partialMatch);
	if (w)
		{
		activateWindow(w);
		return;
		}

	if (subfolders)
		{
		FolderListItem *item = (FolderListItem *)folders->currentItem()->firstChild();
		while (item)
			{
			Folder *f = item->folder();
			myWidget *w = f->findWindow(s,windowNames,labels,caseSensitive,partialMatch);
			if (w)
				{
				folders->setCurrentItem(f->folderListItem());
				activateWindow(w);
				return;
				} 
			item = (FolderListItem *)item->itemBelow();
			}
		}
	}

if (folderNames)
	{
	Folder *f = current_folder->findSubfolder(s, caseSensitive, partialMatch);
	if (f)
		{
		folders->setCurrentItem(f->folderListItem());
		return;
		} 

	if (subfolders)
		{
		FolderListItem *item = (FolderListItem *)folders->currentItem()->firstChild();
		while (item)
			{
			Folder *f = item->folder()->findSubfolder(s, caseSensitive, partialMatch);
			if (f)
				{
				folders->setCurrentItem(f->folderListItem());
				return;
				}
			
			item = (FolderListItem *)item->itemBelow();
			}
		}
	}

QMessageBox::warning(this, tr("QtiPlot - No match found"),
					 tr("Sorry, no match found for string: '%1'").arg(s));
}

void ApplicationWindow::dropFolderItems(QListViewItem *dest)
{
if (!dest || draggedItems.isEmpty ())
	return;

Folder *dest_f = ((FolderListItem *)dest)->folder();

QListViewItem *it;
QStringList subfolders = dest_f->subfolders();

for (it = draggedItems.first(); it; it = draggedItems.next())
	{
	if (it->rtti() == FolderListItem::ListItemType)
		{
		Folder *f = ((FolderListItem *)it)->folder();
		FolderListItem *src = f->folderListItem();
		if (dest_f == f)
			{
			QMessageBox::critical(this, "QtiPlot - Error", tr("Cannot move an object to itself!"));
			return;
			}

		if (((FolderListItem *)dest)->isChildOf(src))
			{
			QMessageBox::critical(this,"QtiPlot - Error",tr("Cannot move a parent folder into a child folder!"));
			draggedItems.clear();
			folders->setCurrentItem(current_folder->folderListItem());
			return;
			}

		Folder *parent = (Folder *)f->parent();
		if (!parent)
			parent = projectFolder();
		if (dest_f == parent)
			return;

		if (subfolders.contains(f->folderName()))
			{
			QMessageBox::critical(this, tr("QtiPlot") +" - " + tr("Skipped Moving Folder"), 
			tr("The destination folder already contains a folder called '%1'! Folder skipped!").arg(f->folderName()));
			}
		else
			moveFolder(src, (FolderListItem *)dest);
		}
	else
		{
		if (dest_f == current_folder)
			return;

		myWidget *w = ((WindowListItem *)it)->window();
		if (w)
			{
			current_folder->removeWindow(w);
			w->hide();
			dest_f->addWindow(w);
			delete it;
			}
		}
	}

draggedItems.clear();
current_folder = dest_f;
folders->setCurrentItem(dest_f->folderListItem());
changeFolder(dest_f, true);
folders->setFocus();
}

void ApplicationWindow::moveFolder(FolderListItem *src, FolderListItem *dest)
{
folders->blockSignals(true);

Folder *dest_f = dest->folder();
Folder *src_f = src->folder();

dest_f = new Folder(dest_f, src_f->folderName());
dest_f->setBirthDate(src_f->birthDate());
dest_f->setModificationDate(src_f->modificationDate());

FolderListItem *copy_item = new FolderListItem(dest, dest_f);
copy_item->setText(0, src_f->folderName());
dest_f->setFolderListItem(copy_item);

QPtrList <myWidget> lst = QPtrList<myWidget>(src_f->windowsList());
myWidget *w;
for (w = lst.first(); w; w = lst.next())
	{
	src_f->removeWindow(w);
	w->hide();
	dest_f->addWindow(w);
	}	

if (src_f->children())
	{
	FolderListItem *item = (FolderListItem *)src->firstChild();
	int initial_depth = item->depth();
	while (item && item->depth() >= initial_depth)
		{
		src_f = (Folder *)item->folder();

		dest_f = new Folder(dest_f, src_f->folderName());
		dest_f->setBirthDate(src_f->birthDate());
		dest_f->setModificationDate(src_f->modificationDate());

		copy_item = new FolderListItem(copy_item, dest_f);
		copy_item->setText(0, src_f->folderName());
		dest_f->setFolderListItem(copy_item);

		lst = QPtrList<myWidget>(src_f->windowsList());
		for (w = lst.first(); w; w = lst.next())
			{
			src_f->removeWindow(w);
			w->hide();
			dest_f->addWindow(w);
			}

		item = (FolderListItem *)item->itemBelow();
		}
	}

src_f = src->folder();
delete src_f;
delete src;
folders->blockSignals(false);
}

ApplicationWindow::~ApplicationWindow()
{
if (lastCopiedLayer)
	delete lastCopiedLayer;

delete hiddenWindows;
delete outWindows;

QApplication::clipboard()->clear(QClipboard::Clipboard);
}
