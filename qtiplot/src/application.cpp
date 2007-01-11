/***************************************************************************
	File                 : application.cpp
	Project              : QtiPlot
--------------------------------------------------------------------
	Copyright            : (C) 2006 by Ion Vasilief,
	                       Tilman Hoener zu Siederdissen,
                          Knut Franke
	Email                : ion_vasilief@yahoo.fr, thzs@gmx.net,
	                       knut.franke@gmx.de
	Description          : QtiPlot's main window

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
#include "dataSetDialog.h"
#include "intDialog.h"
#include "configDialog.h"
#include "imageExportOptionsDialog.h"
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
#include "scales.h"
#include "ScriptingLangDialog.h"
#include "TableStatistics.h"
#include "Fitter.h"
#include "FunctionCurve.h"

#include <stdio.h>
#include <stdlib.h>

#include <qworkspace.h>
#include <qimage.h>
#include <qpixmap.h>
#include <q3toolbar.h>
#include <qtoolbutton.h>
#include <q3popupmenu.h>
#include <qmenubar.h>
#include <qnamespace.h>
#include <qfile.h>
#include <q3filedialog.h>
#include <qmessagebox.h>
#include <qprinter.h>
#include <q3accel.h>
#include <qtextstream.h>
#include <qinputdialog.h>
#include <qregexp.h>
#include <q3textview.h>
#include <q3listview.h>
#include <qcursor.h>
#include <q3textbrowser.h>
#include <qevent.h>
#include <qaction.h>
#include <q3progressdialog.h>
#include <qpixmapcache.h>
#include <qsettings.h>
#include <qstylefactory.h>
#include <q3dragobject.h>
#include <qclipboard.h>
#include <qapplication.h>
#include <q3process.h>
#include <qtranslator.h>
#include <qsplitter.h>
#include <qobject.h>
#include <q3simplerichtext.h>
#include <q3ptrlist.h>

#include <QActionGroup>
#include <QAction>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QList>
#include <QTimerEvent>
#include <QCloseEvent>
#include <QToolBar>
#include <QKeySequence>
#include <QImageReader>
#include <QImageWriter>
#include <QList>
#include <QDateTime>
#include <QProcess>
#include <QAction>
#include <QShortcut>
#include <QDockWidget>
#include <QPrintDialog>
#include <QVarLengthArray>

#include <zlib.h>

using namespace Qwt3D;

extern "C" 
{
	void file_compress(char  *file, char  *mode);
	void file_uncompress(char  *file);
}

ApplicationWindow::ApplicationWindow()
: QMainWindow(), scripted(ScriptingLangManager::newEnv(this))
{
	setAttribute(Qt::WA_DeleteOnClose);
	init();
}

ApplicationWindow::ApplicationWindow(const QStringList& l)
: QMainWindow(), scripted(ScriptingLangManager::newEnv(this))
{
	setAttribute(Qt::WA_DeleteOnClose);

	int args = (int)l.size();
	if (args > 2)
	{
		ApplicationWindow *aux = new ApplicationWindow();
		aux->hideActiveWindow();

		QMessageBox::critical(aux, tr("QtiPlot - Error"),
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
			ApplicationWindow *aux = new ApplicationWindow();
			aux->hideActiveWindow();

			QMessageBox::critical(aux, tr("QtiPlot - Error"),
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
	setWindowTitle(tr("QtiPlot - untitled"));
	initGlobalConstants();			
	QPixmapCache::setCacheLimit(20*QPixmapCache::cacheLimit ());

	tablesDepend = new QMenu(this);

	createActions();
	initToolBars();
	initPlot3DToolBar();
	initMainMenu();

	explorerWindow = new QDockWidget( this );
	explorerWindow->setWindowTitle(tr("Project Explorer"));
	explorerWindow->setObjectName("explorerWindow"); // this is needed for QMainWindow::restoreState()
	explorerWindow->setMinimumHeight(150);
	addDockWidget( Qt::BottomDockWidgetArea, explorerWindow );

	explorerSplitter = new QSplitter( Qt::Horizontal, explorerWindow );

	folders = new FolderListView( explorerSplitter );
	folders->header()->setClickEnabled( false );
	folders->addColumn( tr("Folder") );
	folders->setRootIsDecorated( true );
	folders->setResizeMode(Q3ListView::LastColumn);
	folders->header()->hide();
	folders->setSelectionMode(Q3ListView::Single);

	connect(folders, SIGNAL(currentChanged(Q3ListViewItem *)), 
			this, SLOT(folderItemChanged(Q3ListViewItem *)));
	connect(folders, SIGNAL(itemRenamed(Q3ListViewItem *, int, const QString &)), 
			this, SLOT(renameFolder(Q3ListViewItem *, int, const QString &)));
	connect(folders, SIGNAL(contextMenuRequested(Q3ListViewItem *, const QPoint &, int)), 
			this, SLOT(showFolderPopupMenu(Q3ListViewItem *, const QPoint &, int)));
	connect(folders, SIGNAL(dragItems(QList<Q3ListViewItem *>)), 
			this, SLOT(dragFolderItems(QList<Q3ListViewItem *>)));
	connect(folders, SIGNAL(dropItems(Q3ListViewItem *)), 
			this, SLOT(dropFolderItems(Q3ListViewItem *)));
	connect(folders, SIGNAL(renameItem(Q3ListViewItem *)), 
			this, SLOT(startRenameFolder(Q3ListViewItem *)));
	connect(folders, SIGNAL(addFolderItem()), this, SLOT(addFolder()));
	connect(folders, SIGNAL(deleteSelection()), this, SLOT(deleteSelectedItems()));

	current_folder = new Folder( 0, tr("UNTITLED"));
	FolderListItem *fli = new FolderListItem(folders, current_folder);
	current_folder->setFolderListItem(fli);
	fli->setOpen( true );

	lv = new FolderListView( explorerSplitter );
	lv->addColumn (tr("Name"),-1 );
	lv->addColumn (tr("Type"),-1 );
	lv->addColumn (tr("View"),-1 );
	lv->addColumn (tr("Size"),-1 );
	lv->addColumn (tr("Created"),-1);
	lv->addColumn (tr("Label"),-1);
	lv->setResizeMode(Q3ListView::LastColumn);
	lv->setMinimumHeight(80);
	lv->setSelectionMode(Q3ListView::Extended);

	explorerWindow->setWidget(explorerSplitter);
	explorerWindow->hide();

	logWindow = new QDockWidget(this);
	logWindow->setObjectName("logWindow"); // this is needed for QMainWindow::restoreState()
	logWindow->setWindowTitle(tr("Results Log"));
	addDockWidget( Qt::TopDockWidgetArea, logWindow );

	results=new QTextEdit(logWindow);
	results->setReadOnly (true);

	logWindow->setWidget(results);
	logWindow->hide();

#ifdef SCRIPTING_CONSOLE
	consoleWindow = new QDockWidget(this);
	consoleWindow->setObjectName("consoleWindow"); // this is needed for QMainWindow::restoreState()
	consoleWindow->setWindowTitle(tr("Scripting Console"));
	addDockWidget( Qt::TopDockWidgetArea, consoleWindow );
	console = new QTextEdit(consoleWindow);
	console->setReadOnly(true);
	consoleWindow->setWidget(console);
	consoleWindow->hide();
#endif

	ws = new QWorkspace( this );
	ws->setScrollBarsEnabled (true);
	setCentralWidget( ws );
	setAcceptDrops(true);

	hiddenWindows = new QList<QWidget*>();
	outWindows = new QList<QWidget*>();

	readSettings();
	createLanguagesList();
	insertTranslatedStrings();

	actionNextWindow = new QAction(QIcon(QPixmap(next_xpm)), tr("&Next","next window"), this);
	actionNextWindow->setShortcut( tr("F5","next window shortcut") );
	connect(actionNextWindow, SIGNAL(activated()), ws, SLOT(activateNextWindow()));

	actionPrevWindow = new QAction(QIcon(QPixmap(prev_xpm)), tr("&Previous","previous window"), this);
	actionPrevWindow->setShortcut( tr("F6","previous window shortcut") );
	connect(actionPrevWindow, SIGNAL(activated()), ws, SLOT(activatePreviousWindow()));

	connect(actionShowLog, SIGNAL(toggled(bool)), this, SLOT(showResults(bool)));
	//TODO: Find a way to implement this in Qt4
	//	connect(logWindow,SIGNAL(visibilityChanged(bool)),actionShowLog,SLOT(setChecked(bool)));
	//	connect(explorerWindow,SIGNAL(visibilityChanged(bool)),actionShowExplorer,SLOT(setChecked(bool)));
#ifdef SCRIPTING_CONSOLE
	connect(actionShowConsole, SIGNAL(toggled(bool)), consoleWindow, SLOT(setShown(bool)));
	//TODO: Find a way to implement this in Qt4
	// connect(consoleWindow, SIGNAL(visibilityChanged(bool)), actionShowConsole, SLOT(setOn(bool)));
#endif
	connect(tablesDepend, SIGNAL(activated(int)), this, SLOT(showTable(int)));

	connect(this, SIGNAL(modified()),this, SLOT(modifiedProject()));
	connect(ws, SIGNAL(windowActivated (QWidget*)),this, SLOT(windowActivated(QWidget*)));
	connect(lv, SIGNAL(doubleClicked(Q3ListViewItem *)),
			this, SLOT(maximizeWindow(Q3ListViewItem *)));
	connect(lv, SIGNAL(doubleClicked(Q3ListViewItem *)), 
			this, SLOT(folderItemDoubleClicked(Q3ListViewItem *)));
	connect(lv, SIGNAL(contextMenuRequested(Q3ListViewItem *, const QPoint &, int)), 
			this, SLOT(showWindowPopupMenu(Q3ListViewItem *, const QPoint &, int)));
	connect(lv, SIGNAL(dragItems(QList<Q3ListViewItem *>)), 
			this, SLOT(dragFolderItems(QList<Q3ListViewItem *>)));
	connect(lv, SIGNAL(dropItems(Q3ListViewItem *)), 
			this, SLOT(dropFolderItems(Q3ListViewItem *)));
	connect(lv, SIGNAL(renameItem(Q3ListViewItem *)), 
			this, SLOT(startRenameFolder(Q3ListViewItem *)));
	connect(lv, SIGNAL(addFolderItem()), this, SLOT(addFolder()));
	connect(lv, SIGNAL(deleteSelection()), this, SLOT(deleteSelectedItems()));
	connect(lv, SIGNAL(itemRenamed(Q3ListViewItem *, int, const QString &)), 
			this, SLOT(renameWindow(Q3ListViewItem *, int, const QString &)));
	connect(scriptEnv, SIGNAL(error(const QString&,const QString&,int)),
			this, SLOT(scriptError(const QString&,const QString&,int)));
	connect(scriptEnv, SIGNAL(print(const QString&)), this, SLOT(scriptPrint(const QString&)));

	connect(recent, SIGNAL(activated(int)), this, SLOT(openRecentProject(int)));
	connect(&http, SIGNAL(done(bool)), this, SLOT(receivedVersionFile(bool)));

	// this has to be done after connecting scriptEnv
	scriptEnv->initialize();
}

void ApplicationWindow::initGlobalConstants()
{
	appStyle = qApp->style()->objectName();

	majVersion = 0; minVersion = 9; patchVersion = 0;
	versionSuffix = "alpha1";
	graphs=0; tables=0; matrixes = 0; notes = 0;
	projectname="untitled";
	lastModified=0;
	activeGraph=0;
	lastCopiedLayer=0;
	copiedLayer=false;
	copiedMarkerType=Graph::None;
	aw=0;
	logInfo=QString();
	savingTimerId=0;

	autoSearchUpdatesRequest = false;

	show_windows_policy = ActiveFolder;

	appFont = QFont();
	QString family = appFont.family();
	int pointSize = appFont.pointSize();
	tableTextFont=appFont;
	tableHeaderFont=appFont;
	plotAxesFont=QFont(family, pointSize, QFont::Bold, false);
	plotNumbersFont=QFont(family, pointSize );
	plotLegendFont=appFont;
	plotTitleFont=QFont(family, pointSize + 2, QFont::Bold,false);

	plot3DAxesFont=QFont(family, pointSize, QFont::Bold, false );
	plot3DNumbersFont=QFont(family, pointSize);
	plot3DTitleFont=QFont(family, pointSize + 2, QFont::Bold,false);
}

void ApplicationWindow::applyUserSettings()
{
	//set user defined colors
	lv->setPaletteForegroundColor (panelsTextColor);
	lv->setPaletteBackgroundColor (panelsColor);

	QPalette pal = results->palette();
	pal.setBrush(QPalette::Active, QPalette::Window, QBrush(panelsColor, Qt::SolidPattern));
	pal.setColor(QPalette::Active, QPalette::WindowText, panelsTextColor);
	results->setPalette(pal);

	ws->setPaletteBackgroundColor (workspaceColor);

	pal = qApp->palette();
	pal.setColor(QPalette::Active, QPalette::Base, QColor(panelsColor));
	qApp->setPalette(pal);

	updateAppFonts();

	QColorGroup cg;
	cg.setColor(QColorGroup::Text, QColor(Qt::green) );
	cg.setColor(QColorGroup::HighlightedText, QColor(Qt::darkGreen) );
	cg.setColor(QColorGroup::Background, QColor(Qt::black) );
	info->setPalette(QPalette(cg, cg, cg));
	setScriptingLang(defaultScriptingLang);
}

void ApplicationWindow::initToolBars()
{
	setWindowIcon(QIcon(QPixmap(logo_xpm)));
	QPixmap openIcon, saveIcon;

	fileTools = new QToolBar( tr( "File" ), this );
	fileTools->setObjectName("fileTools"); // this is needed for QMainWindow::restoreState()
	fileTools->setIconSize( QSize(18,20) );
	addToolBar( Qt::TopToolBarArea, fileTools );

	fileTools->addAction(actionNewProject);
	fileTools->addAction(actionNewTable);
	fileTools->addAction(actionNewMatrix);
	fileTools->addAction(actionNewNote);
	fileTools->addAction(actionNewGraph);
	fileTools->addAction(actionNewFunctionPlot);
	fileTools->addAction(actionNewSurfacePlot);

	fileTools->addSeparator ();

	fileTools->addAction(actionOpen);
	fileTools->addAction(actionOpenTemplate);
	fileTools->addAction(actionSaveProject);
	fileTools->addAction(actionSaveTemplate);

	fileTools->addSeparator ();

	fileTools->addAction(actionLoad);
	fileTools->addAction(actionLoadMultiple);

	fileTools->addSeparator ();

	fileTools->addAction(actionCopyWindow);
	fileTools->addAction(actionPrint);

	fileTools->addSeparator();

	fileTools->addAction(actionShowExplorer);
	fileTools->addAction(actionShowLog);

	editTools = new QToolBar( tr("Edit"), this);
	editTools->setObjectName("editTools"); // this is needed for QMainWindow::restoreState()
	editTools->setIconSize( QSize(18,20) );
	addToolBar( editTools );

	editTools->addAction(actionUndo);
	editTools->addAction(actionRedo);
	editTools->addAction(actionCutSelection);
	editTools->addAction(actionCopySelection);
	editTools->addAction(actionPasteSelection);
	editTools->addAction(actionClearSelection);

	plotTools = new QToolBar( tr("Graph"), this );
	plotTools->setObjectName("plotTools"); // this is needed for QMainWindow::restoreState()
	plotTools->setIconSize( QSize(16,20) );
	addToolBar( plotTools );

	plotTools->addAction(actionAddLayer);
	plotTools->addAction(actionShowLayerDialog);
	plotTools->addAction(actionAutomaticLayout);

	plotTools->addSeparator();

	plotTools->addAction(actionShowCurvesDialog);
	plotTools->addAction(actionAddErrorBars);
	plotTools->addAction(actionAddFunctionCurve);
	plotTools->addAction(actionNewLegend);

	plotTools->addSeparator ();

	plotTools->addAction(actionUnzoom);

	dataTools = new QActionGroup( this );
	dataTools->setExclusive( true );

	btnPointer = new QAction(tr("Disable &Tools"), this);
	btnPointer->setActionGroup(dataTools);
	btnPointer->setCheckable( true );
	btnPointer->setIcon(QIcon(QPixmap(pointer_xpm)) );
	btnPointer->setChecked(true);
	plotTools->addAction(btnPointer);

	btnZoomIn = new QAction(tr("&Zoom In"), this);
	btnZoomIn->setShortcut( tr("Ctrl++") );
	btnZoomIn->setActionGroup(dataTools);
	btnZoomIn->setCheckable( true );
	btnZoomIn->setIcon(QIcon(QPixmap(zoom_xpm)) );
	plotTools->addAction(btnZoomIn);

	btnZoomOut = new QAction(tr("&Zoom Out"), this);
	btnZoomOut->setShortcut( tr("Ctrl+-") );
	btnZoomOut->setActionGroup(dataTools);
	btnZoomOut->setCheckable( true );
	btnZoomOut->setIcon(QIcon(QPixmap(zoomOut_xpm)) );
	plotTools->addAction(btnZoomOut);

	btnCursor = new QAction(tr("&Data Reader"), this);
	btnCursor->setShortcut( tr("CTRL+D") );
	btnCursor->setActionGroup(dataTools);
	btnCursor->setCheckable( true );
	btnCursor->setIcon(QIcon(QPixmap(select_xpm)) );
	plotTools->addAction(btnCursor);

	btnSelect = new QAction(tr("&Select Data Range"), this);
	btnSelect->setShortcut( tr("ALT+S") );
	btnSelect->setActionGroup(dataTools);
	btnSelect->setCheckable( true );
	btnSelect->setIcon(QIcon(QPixmap(cursors_xpm)) );
	plotTools->addAction(btnSelect);

	btnPicker = new QAction(tr("S&creen Reader"), this);
	btnPicker->setActionGroup(dataTools);
	btnPicker->setCheckable( true );
	btnPicker->setIcon(QIcon(QPixmap(cursor_16)) );
	plotTools->addAction(btnPicker);

	btnMovePoints = new QAction(tr("&Move Data Points..."), this);
	btnMovePoints->setShortcut( tr("Ctrl+ALT+M") );
	btnMovePoints->setActionGroup(dataTools);
	btnMovePoints->setCheckable( true );
	btnMovePoints->setIcon(QIcon(QPixmap(hand_xpm)) );
	plotTools->addAction(btnMovePoints);

	btnRemovePoints = new QAction(tr("Remove &Bad Data Points..."), this);
	btnRemovePoints->setShortcut( tr("Alt+B") );
	btnRemovePoints->setActionGroup(dataTools);
	btnRemovePoints->setCheckable( true );
	btnRemovePoints->setIcon(QIcon(QPixmap(gomme_xpm)));
	plotTools->addAction(btnRemovePoints);

	connect( dataTools, SIGNAL( triggered( QAction* ) ), this, SLOT( pickDataTool( QAction* ) ) );
	plotTools->addSeparator ();

	actionAddText = new QAction(tr("Add &Text"), this);
	actionAddText->setShortcut( tr("ALT+T") );
	actionAddText->setIcon(QIcon(QPixmap(text_xpm)));
	actionAddText->setCheckable(true);
	connect(actionAddText, SIGNAL(activated()), this, SLOT(addText()));
	plotTools->addAction(actionAddText);

	btnArrow = new QAction(tr("Draw &Arrow"), this);
	btnArrow->setShortcut( tr("CTRL+ALT+A") );
	btnArrow->setActionGroup(dataTools);
	btnArrow->setCheckable( true );
	btnArrow->setIcon(QIcon(QPixmap(arrow_xpm)) );
	plotTools->addAction(btnArrow);

	btnLine = new QAction(tr("Draw &Line"), this);
	btnLine->setShortcut( tr("CTRL+ALT+L") );
	btnLine->setActionGroup(dataTools);
	btnLine->setCheckable( true );
	btnLine->setIcon(QIcon(QPixmap(lPlot_xpm)) );
	plotTools->addAction(btnLine);

	plotTools->addAction(actionTimeStamp);
	plotTools->addAction(actionAddImage);

	tableTools = new QToolBar( tr( "Table" ), this );
	tableTools->setObjectName("tableTools"); // this is needed for QMainWindow::restoreState()
	tableTools->setIconSize( QSize(16,20) );
	addToolBar( Qt::TopToolBarArea, tableTools );

	tableTools->addAction(actionPlotL);
	tableTools->addAction(actionPlotP);
	tableTools->addAction(actionPlotLP);
	tableTools->addAction(actionPlotVerticalBars);
	tableTools->addAction(actionPlotHorizontalBars);
	tableTools->addAction(actionPlotArea);
	tableTools->addAction(actionPlotPie);
	tableTools->addAction(actionPlotHistogram);
	tableTools->addAction(actionBoxPlot);
	tableTools->addAction(actionPlotVectXYXY);
	tableTools->addAction(actionPlotVectXYAM);

	tableTools->addSeparator ();

	tableTools->addAction(actionPlot3DRibbon);
	tableTools->addAction(actionPlot3DBars);
	tableTools->addAction(actionPlot3DScatter);
	tableTools->addAction(actionPlot3DTrajectory);

	tableTools->addSeparator ();

	tableTools->addAction(actionAddColToTable);
	tableTools->addAction(actionShowColStatistics);
	tableTools->addAction(actionShowRowStatistics);

	plotTools->hide();
	tableTools->hide();

	displayBar = new QToolBar( tr( "Data Display" ), this );
	displayBar->setObjectName("displayBar"); // this is needed for QMainWindow::restoreState()
	info=new QLineEdit( displayBar );
	info->setReadOnly(true);

	displayBar->setMaximumHeight(2*displayBar->sizeHint().height());

	addToolBar( Qt::TopToolBarArea, displayBar );
	displayBar->setAllowedAreas( Qt::TopToolBarArea | Qt::BottomToolBarArea );
	displayBar->hide();
}

void ApplicationWindow::insertTranslatedStrings()
{
	if (projectname == "untitled")
		setWindowTitle(tr("QtiPlot - untitled"));

	lv->setColumnText (0, tr("Name"));
	lv->setColumnText (1, tr("Type"));
	lv->setColumnText (2, tr("View"));
	lv->setColumnText (3, tr("Size"));
	lv->setColumnText (4, tr("Created"));
	lv->setColumnText (5, tr("Label"));

	explorerWindow->setWindowTitle(tr("Project Explorer"));
	logWindow->setWindowTitle(tr("Results Log"));
#ifdef SCRIPTING_CONSOLE
	consoleWindow->setWindowTitle(tr("Scripting Console"));
#endif
	displayBar->setLabel(tr("Data Display"));
	tableTools->setLabel(tr("Table"));
	plotTools->setLabel(tr("Plot"));
	fileTools->setLabel(tr("File"));
	editTools->setLabel(tr("Edit"));
	plot3DTools->setLabel(tr("3D Surface"));

	file->changeItem(newMenuID, tr("&New"));
	file->changeItem(recentMenuID, tr("&Recent Projects"));
	file->changeItem(exportID, tr("&Export Graph"));
	file->changeItem(importMenuID, tr("&Import ASCII"));

	plot2D->changeItem(specialPlotMenuID, tr("Special Line/Symb&ol"));
	plot2D->changeItem(statMenuID, tr("Statistical &Graphs"));
	plot2D->changeItem(panelMenuID, tr("Pa&nel"));
	plot2D->changeItem(plot3dID, tr("3&D Plot"));

	dataMenu->changeItem(normMenuID, tr("&Normalize"));

	tableMenu->changeItem(setAsMenuID, tr("Set Columns &As"));
	tableMenu->changeItem(fillMenuID, tr("&Fill Columns With"));

	calcul->changeItem(translateMenuID, tr("&Translate"));
	calcul->changeItem(smoothMenuID, tr("&Smooth"));
	calcul->changeItem(filterMenuID, tr("&FFT Filter"));
	calcul->changeItem(fitExpMenuID, tr("Fit E&xponential Decay"));
	calcul->changeItem(multiPeakMenuID, tr("Fit &Multi-Peak"));

	translateActionsStrings();
	customMenu(ws->activeWindow());
}

void ApplicationWindow::initMainMenu()
{
	file = new QMenu( this );
	file->setFont(appFont);

	type = new QMenu(this);
	type->setFont(appFont);
	type->addAction(actionNewProject);
	type->addAction(actionNewTable);
	type->addAction(actionNewMatrix);
	type->addAction(actionNewNote);
	type->addAction(actionNewGraph);
	type->addAction(actionNewFunctionPlot);
	type->addAction(actionNewSurfacePlot);

	newMenuID = file->insertItem(tr("&New"),type);
	file->addAction(actionOpen);

	recent = new QMenu(this);
	recent->setFont(appFont);
	recentMenuID = file->insertItem(tr("&Recent Projects"), recent);

	file->insertSeparator();

	file->addAction(actionLoadImage);
	file->addAction(actionImportImage);

	file->insertSeparator();

	file->addAction(actionSaveProject);
	file->addAction(actionSaveProjectAs);

	file->insertSeparator();
	file->addAction(actionOpenTemplate);
	file->addAction(actionSaveTemplate);
	file->insertSeparator();

	exportPlot = new QMenu(this);
	exportPlot->addAction(actionExportGraph);
	exportPlot->addAction(actionExportAllGraphs);
	exportID=file->insertItem(tr("&Export Graph"), exportPlot);

	file->addAction(actionPrint);
	file->addAction(actionPrintAllPlots);

	file->insertSeparator();

	file->addAction(actionShowExportASCIIDialog);

	import = new QMenu(this);
	import->setFont(appFont);
	import->addAction(actionLoad);
	import->addAction(actionLoadMultiple);

	import->insertSeparator();

	import->addAction(actionShowImportDialog);
	importMenuID = file->insertItem(tr("&Import ASCII"),import);

	file->insertSeparator();

	file->addAction(actionCloseAllWindows);

	edit = new QMenu(this);
	edit->setFont(appFont);
	edit->addAction(actionUndo);
	edit->addAction(actionRedo);

	edit->insertSeparator();

	edit->addAction(actionCutSelection);
	edit->addAction(actionCopySelection);
	edit->addAction(actionPasteSelection);
	edit->addAction(actionClearSelection);

	edit->insertSeparator();

	edit->addAction(actionDeleteFitTables);
	edit->addAction(actionClearLogInfo);

	view = new QMenu(this);
	view->setFont(appFont);
	view->setCheckable(true);
	view->addAction(actionShowPlotWizard);
	view->addAction(actionShowExplorer);
	view->addAction(actionShowLog);
#ifdef SCRIPTING_CONSOLE
	view->addAction(actionShowConsole);
#endif
	view->addAction(actionShowConfigureDialog);

	graph = new QMenu(this);
	graph->setFont(appFont);
	graph->setCheckable(true);
	graph->addAction(actionShowCurvesDialog);
	graph->addAction(actionAddErrorBars);
	graph->addAction(actionAddFunctionCurve);
	graph->addAction(actionNewLegend);

	graph->insertSeparator();

	graph->addAction(actionAddText);
	graph->addAction(btnArrow);
	graph->addAction(btnLine);
	graph->addAction(actionTimeStamp);
	graph->addAction(actionAddImage);

	graph->insertSeparator();//layers section
	graph->addAction(actionAddLayer);
	graph->addAction(actionDeleteLayer);
	graph->addAction(actionShowLayerDialog);

	plot3DMenu = new QMenu(this);
	plot3DMenu->setFont(appFont);

	plot3DMenu->addAction(actionPlot3DWireFrame);
	plot3DMenu->addAction(actionPlot3DHiddenLine);

	plot3DMenu->addAction(actionPlot3DPolygons);
	plot3DMenu->addAction(actionPlot3DWireSurface);

	plot3DMenu->insertSeparator();

	plot3DMenu->addAction(actionPlot3DBars);
	plot3DMenu->addAction(actionPlot3DScatter);

	matrixMenu = new QMenu(this);
	matrixMenu->setFont(appFont);

	matrixMenu->addAction(actionSetMatrixProperties);
	matrixMenu->addAction(actionSetMatrixDimensions);
	matrixMenu->addAction(actionSetMatrixValues);

	matrixMenu->insertSeparator();

	matrixMenu->addAction(actionTransposeMatrix);
	matrixMenu->addAction(actionInvertMatrix);
	matrixMenu->addAction(actionMatrixDeterminant);

	matrixMenu->insertSeparator();
	matrixMenu->addAction(actionConvertMatrix);

	initPlotMenu();
	initTableAnalysisMenu();
	initTableMenu();
	initPlotDataMenu();

	calcul = new QMenu( this );
	calcul->setFont(appFont);

	translateMenu = new QMenu(this);
	translateMenu->setFont(appFont);
	translateMenu->addAction(actionTranslateVert);
	translateMenu->addAction(actionTranslateHor);
	translateMenuID = calcul->insertItem(tr("&Translate"),translateMenu);
	calcul->insertSeparator();

	calcul->addAction(actionDifferentiate);
	calcul->addAction(actionShowIntDialog);

	calcul->insertSeparator();

	smooth = new QMenu(this);
	smooth->setFont(appFont);
	smooth->addAction(actionSmoothSavGol);
	smooth->addAction(actionSmoothAverage);
	smooth->addAction(actionSmoothFFT);
	smoothMenuID = calcul->insertItem(tr("&Smooth"),smooth);

	filter = new QMenu(this);
	filter->setFont(appFont);
	filter->addAction(actionLowPassFilter);
	filter->addAction(actionHighPassFilter);
	filter->addAction(actionBandPassFilter);
	filter->addAction(actionBandBlockFilter);
	filterMenuID = calcul->insertItem(tr("&FFT filter"),filter);

	calcul->insertSeparator();
	calcul->addAction(actionInterpolate);
	calcul->addAction(actionFFT);
	calcul->insertSeparator();
	calcul->addAction(actionFitLinear);
	calcul->addAction(actionShowFitPolynomDialog);

	calcul->insertSeparator();

	decay = new QMenu(this);
	decay->setFont(appFont);
	decay->addAction(actionShowExpDecayDialog);
	decay->addAction(actionShowTwoExpDecayDialog);
	decay->addAction(actionShowExpDecay3Dialog);
	fitExpMenuID = calcul->insertItem(tr("Fit E&xponential Decay"), decay);

	calcul->addAction(actionFitExpGrowth);
	calcul->addAction(actionFitSigmoidal);
	calcul->addAction(actionFitGauss);
	calcul->addAction(actionFitLorentz);

	multiPeakMenu = new QMenu(this);
	multiPeakMenu->setFont(appFont);
	multiPeakMenu->addAction(actionMultiPeakGauss);
	multiPeakMenu->addAction(actionMultiPeakLorentz);
	multiPeakMenuID = calcul->insertItem(tr("Fit &Multi-peak"), multiPeakMenu);

	calcul->insertSeparator();

	calcul->addAction(actionShowFitDialog);

	format = new QMenu(this);
	format->setFont(appFont);

	scriptingMenu = new QMenu(this);
	scriptingMenu->setFont(appFont);

	windowsMenu = new QMenu( this );
	windowsMenu->setFont(appFont);
	windowsMenu->setCheckable( true );
	connect( windowsMenu, SIGNAL( aboutToShow() ),
			this, SLOT( windowsMenuAboutToShow() ) );

	help = new QMenu( this );
	help->setFont(appFont);

	help->addAction(actionShowHelp);
	help->addAction(actionChooseHelpFolder);
	help->insertSeparator();
	help->addAction(actionHomePage);
	help->addAction(actionCheckUpdates);
	help->addAction(actionDownloadManual);
	help->addAction(actionTranslations);
	help->insertSeparator();
	help->addAction(actionTechnicalSupport);
	help->addAction(actionDonate);
	help->addAction(actionHelpForums);
	help->addAction(actionHelpBugReports);
	help->insertSeparator();
	help->addAction(actionAbout);

	disableActions();
}

void ApplicationWindow::initTableMenu()
{
	tableMenu = new QMenu(this);
	tableMenu->setFont(appFont);

	setAsMenu = new QMenu(this);
	setAsMenu->setFont(appFont);

	setAsMenu->addAction(actionSetXCol);
	setAsMenu->addAction(actionSetYCol);
	setAsMenu->addAction(actionSetZCol);
	setAsMenu->insertSeparator();
	setAsMenu->addAction(actionSetYErrCol);
	setAsMenu->addAction(actionSetXErrCol);
	setAsMenu->insertSeparator();

	setAsMenu->addAction(actionDisregardCol);
	setAsMenuID = tableMenu->insertItem(tr("Set Columns &As"), setAsMenu);

	tableMenu->addAction(actionShowColumnOptionsDialog);
	tableMenu->insertSeparator();

	tableMenu->addAction(actionShowColumnValuesDialog);
	tableMenu->addAction(actionTableRecalculate);

	fillMenu = new QMenu(this);
	fillMenu->setFont(appFont);
	fillMenu->addAction(actionSetAscValues);
	fillMenu->addAction(actionSetRandomValues);
	fillMenuID = tableMenu->insertItem(tr("&Fill Columns With"),fillMenu);

	tableMenu->insertSeparator();
	tableMenu->addAction(actionAddColToTable);
	tableMenu->addAction(actionShowColsDialog);
	tableMenu->addAction(actionShowRowsDialog);

	tableMenu->insertSeparator();
	tableMenu->addAction(actionConvertTable);
}

void ApplicationWindow::initPlotDataMenu()
{
	plotDataMenu = new QMenu(this);
	plotDataMenu->setFont(appFont);
	plotDataMenu->setCheckable(true);

	plotDataMenu->addAction(btnPointer);
	plotDataMenu->addAction(btnZoomIn);
	plotDataMenu->addAction(btnZoomOut);
	plotDataMenu->addAction(actionUnzoom);
	plotDataMenu->insertSeparator();

	plotDataMenu->addAction(btnCursor);
	plotDataMenu->addAction(btnSelect);
	plotDataMenu->addAction(btnPicker);

	plotDataMenu->insertSeparator();

	plotDataMenu->addAction(btnMovePoints);
	plotDataMenu->addAction(btnRemovePoints);
}

void ApplicationWindow::initPlotMenu()
{	  
	plot2D = new QMenu(this);
	plot2D->setFont(appFont);
	specialPlot = new QMenu(this);
	specialPlot->setFont(appFont);
	panels = new QMenu(this);
	panels->setFont(appFont);
	stat = new QMenu(this);
	stat->setFont(appFont);

	plot2D->addAction(actionPlotL);
	plot2D->addAction(actionPlotP);
	plot2D->addAction(actionPlotLP);

	specialPlot->addAction(actionPlotVerticalDropLines);
	specialPlot->addAction(actionPlotSpline);
	specialPlot->addAction(actionPlotVertSteps);
	specialPlot->addAction(actionPlotHorSteps);
	specialPlotMenuID = plot2D->insertItem(tr("Special Line/Symb&ol"), specialPlot);

	plot2D->insertSeparator();

	plot2D->addAction(actionPlotVerticalBars);
	plot2D->addAction(actionPlotHorizontalBars);
	plot2D->addAction(actionPlotArea);
	plot2D->addAction(actionPlotPie);
	plot2D->addAction(actionPlotVectXYXY);
	plot2D->addAction(actionPlotVectXYAM);

	plot2D->insertSeparator();

	stat->addAction(actionBoxPlot);
	stat->addAction(actionPlotHistogram);
	stat->addAction(actionPlotStackedHistograms);
	statMenuID = plot2D->insertItem(tr("Statistical &Graphs"),stat);

	panels->addAction(actionPlot2VerticalLayers);
	panels->addAction(actionPlot2HorizontalLayers);
	panels->addAction(actionPlot4Layers);
	panels->addAction(actionPlotStackedLayers);
	panelMenuID = plot2D->insertItem(tr("Pa&nel"),panels);

	plot3D = new QMenu(this);
	plot3D->setFont(appFont);
	plot3D->addAction(actionPlot3DRibbon);
	plot3D->addAction(actionPlot3DBars);
	plot3D->addAction(actionPlot3DScatter);
	plot3D->addAction(actionPlot3DTrajectory);

	plot2D->insertSeparator();
	plot3dID = plot2D->insertItem(tr("3&D Plot"), plot3D);	
}

void ApplicationWindow::initTableAnalysisMenu()
{
	dataMenu = new QMenu(this);
	dataMenu->setFont(appFont);

	dataMenu->addAction(actionShowColStatistics);
	dataMenu->addAction(actionShowRowStatistics);

	dataMenu->insertSeparator();

	dataMenu->addAction(actionSortSelection);
	dataMenu->addAction(actionSortTable);

	normMenu = new QMenu(this);
	normMenu->setFont(appFont);
	normMenu->insertItem(tr("&Columns"), this, SLOT(normalizeSelection()));
	normMenu->addAction(actionNormalizeTable);
	normMenuID = dataMenu->insertItem(tr("&Normalize"), normMenu);

	dataMenu->insertSeparator();

	dataMenu->addAction(actionFFT);
	dataMenu->addAction(actionCorrelate);
	dataMenu->addAction(actionConvolute);
	dataMenu->addAction(actionDeconvolute);

	dataMenu->insertSeparator();
	dataMenu->addAction(actionShowFitDialog);
}

void ApplicationWindow::customMenu(QWidget* w)
{
	menuBar()->clear();
	menuBar()->insertItem(tr("&File"), file);
	menuBar()->insertItem(tr("&Edit"), edit);
	menuBar()->insertItem(tr("&View"), view);
	menuBar()->insertItem(tr("Scripting"), scriptingMenu);

	scriptingMenu->clear();
#ifdef SCRIPTING_DIALOG
	scriptingMenu->addAction(actionScriptingLang);
#endif
	scriptingMenu->addAction(actionRestartScripting);

	// these use the same keyboard shortcut (Ctrl+Return) and should not be enabled at the same time
	actionNoteEvaluate->setEnabled(false);
	actionTableRecalculate->setEnabled(false);

	if(w)
	{
		actionPrintAllPlots->setEnabled(projectHas2DPlots());
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

			file->setItemEnabled (exportID,true);
			actionShowExportASCIIDialog->setEnabled(false);
			file->setItemEnabled (closeID,true);

			format->clear();
			format->addAction(actionShowPlotDialog);
			format->addAction(actionShowCurveFormatDialog);
			Graph *g = ((MultiLayer*)w)->activeGraph();
			if (g && !g->isPiePlot())
			{
				format->insertSeparator();
				format->addAction(actionShowScaleDialog);
				format->addAction(actionShowAxisDialog);
				actionShowAxisDialog->setEnabled(true);
				format->insertSeparator();
				format->addAction(actionShowGridDialog);
			}
			format->addAction(actionShowTitleDialog);
		}
		else if (w->isA("Graph3D"))
		{
			disableActions();

			menuBar()->insertItem(tr("For&mat"), format);

			actionPrint->setEnabled(true);
			actionSaveTemplate->setEnabled(true);
			file->setItemEnabled (exportID,true);
			file->setItemEnabled (closeID,true);

			format->clear();
			format->addAction(actionShowPlotDialog);
			format->addAction(actionShowScaleDialog);
			format->addAction(actionShowAxisDialog);
			format->addAction(actionShowTitleDialog);
			if (((Graph3D*)w)->coordStyle() == Qwt3D::NOCOORD)
				actionShowAxisDialog->setEnabled(false);
		}
		else if (w->inherits("Table"))
		{
			menuBar()->insertItem(tr("&Plot"), plot2D);	
			if (w->isA("Table"))
			{
				menuBar()->insertItem(tr("&Analysis"), dataMenu);
				menuBar()->insertItem(tr("&Table"), tableMenu);
			}

			actionShowExportASCIIDialog->setEnabled(true);
			actionTableRecalculate->setEnabled(true);
			file->setItemEnabled (exportID,false);
			file->setItemEnabled (closeID,true);
		}
		else if (w->isA("Matrix"))
		{
			menuBar()->insertItem(tr("3D &Plot"), plot3DMenu);
			menuBar()->insertItem(tr("&Matrix"), matrixMenu);
		}
		else if (w->isA("Note"))
		{
			actionSaveTemplate->setEnabled(false);
			actionNoteEvaluate->setEnabled(true);
			scriptingMenu->insertSeparator();
			actionNoteExecute->addTo(scriptingMenu);
			actionNoteExecuteAll->addTo(scriptingMenu);
			actionNoteEvaluate->addTo(scriptingMenu);

			actionNoteExecute->disconnect(SIGNAL(activated()));
			actionNoteExecuteAll->disconnect(SIGNAL(activated()));
			actionNoteEvaluate->disconnect(SIGNAL(activated()));
			connect(actionNoteExecute, SIGNAL(activated()), w, SLOT(execute()));
			connect(actionNoteExecuteAll, SIGNAL(activated()), w, SLOT(executeAll()));
			connect(actionNoteEvaluate, SIGNAL(activated()), w, SLOT(evaluate()));
		}
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
	actionPrint->setEnabled(false);
	actionShowExportASCIIDialog->setEnabled(false);
	file->setItemEnabled (exportID,false);
	file->setItemEnabled (closeID,false);

	actionUndo->setEnabled(false);
	actionRedo->setEnabled(false);

	actionCutSelection->setEnabled(false);
	actionCopySelection->setEnabled(false);
	actionPasteSelection->setEnabled(false);
	actionClearSelection->setEnabled(false);
}

void ApplicationWindow::customToolBars(QWidget* w)
{
	if (w)
	{
		if (!projectHas3DPlots())
			plot3DTools->hide();
		if (!projectHas2DPlots())
			plotTools->hide();
		if ((int)tableWindows.count()<=0)
			tableTools->hide();

		if (w->isA("MultiLayer"))
		{
			if (plotTools->isHidden())
				plotTools->show();

			plotTools->setEnabled (true);
			plot3DTools->setEnabled (false);
			tableTools->setEnabled(false);
		}
		else if (w->inherits("Table"))
		{
			if (tableTools->isHidden())
				tableTools->show();

			plotTools->setEnabled (false);
			plot3DTools->setEnabled (false);
			tableTools->setEnabled (true);
		}
		else if (w->isA("Matrix"))
		{
			plotTools->setEnabled (false);
			plot3DTools->setEnabled (false);
			tableTools->setEnabled (false);
		}
		else if (w->isA("Graph3D"))
		{
			plotTools->setEnabled (false);
			tableTools->setEnabled (false);

			if (plot3DTools->isHidden())
				plot3DTools->show();

			Graph3D* plot= (Graph3D*)w;
			if (plot->plotStyle() == Qwt3D::NOPLOT)
				plot3DTools->setEnabled (false);
			else
				plot3DTools->setEnabled (true);

			custom3DActions(w);
		}
		else if (w->isA("Note"))
		{	
			plotTools->setEnabled (false);
			plot3DTools->setEnabled (false);
			tableTools->setEnabled (false);
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

	plotTools->setEnabled (false);
	tableTools->setEnabled (false);
	plot3DTools->setEnabled (false);

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
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	Table* w = (Table*)ws->activeWindow();
	if(int(w->selectedColumns().count())==1)
		w->plot3DRibbon();
	else
		QMessageBox::warning(0,tr("QtiPlot - Plot error"),tr("You must select exactly one column for plotting!"));
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

	if (w->inherits("Table"))
	{
		if(int(((Table*)w)->selectedColumns().count())==1)
			((Table*)w)->plot3DBars();
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

	if (w->inherits("Table"))
	{
		if(int(((Table*)w)->selectedColumns().count())==1)
			((Table*)w)->plot3DScatter();
		else
			QMessageBox::warning(0,tr("QtiPlot - Plot error"),tr("You must select exactly one column for plotting!"));
	}
	else if (w->isA("Matrix"))
		plot3DMatrix (Qwt3D::POINTS);
}

void ApplicationWindow::plot3DTrajectory()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	Table* w = (Table*)ws->activeWindow();
	if(int(w->selectedColumns().count())==1)
		w->plot3DTrajectory();
	else
		QMessageBox::warning(0, tr("QtiPlot - Plot error"),
				tr("You must select exactly one column for plotting!"));
}

void ApplicationWindow::plotVerticalBars()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotVB();
}

void ApplicationWindow::plotHorizontalBars()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotHB();
}

void ApplicationWindow::plotHistogram()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotHistogram();
}

void ApplicationWindow::plotArea()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotArea();
}

void ApplicationWindow::plotPie()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	if(int(((Table*)ws->activeWindow())->selectedColumns().count())==1)
		((Table*)ws->activeWindow())->plotPie();
	else
		QMessageBox::warning(0, tr("QtiPlot - Plot error"), 
				tr("You must select exactly one column for plotting!"));
}

void ApplicationWindow::plotL()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotL();
}

void ApplicationWindow::plotP()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotP();
}

void ApplicationWindow::plotLP()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotLP();
}

void ApplicationWindow::plotVerticalDropLines()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotVerticalDropLines();
}

void ApplicationWindow::plotSpline()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotSpline();
}

void ApplicationWindow::plotVertSteps()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotVertSteps();
}

void ApplicationWindow::plotHorSteps()
{
if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
   return;
  	 
((Table*)ws->activeWindow())->plotHorSteps();
}

void ApplicationWindow::plotVectXYXY()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotVectXYXY();
}

void ApplicationWindow::plotVectXYAM()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table*)ws->activeWindow())->plotVectXYAM();
}

void ApplicationWindow::updateTable(const QString& caption,int row,const QString& text)
{
	Table* w = table(caption);
	if (!w)
		return;

	QStringList cvs=QStringList::split(",",caption,false);
	int pos=cvs[0].findRev("(");
	QString colName=cvs[0].left(pos);
	int xcol=w->colIndex(colName);
	pos=cvs[1].findRev("(");
	colName=cvs[1].left(pos);
	int ycol=w->colIndex(colName);

	if (w->columnType(xcol) == Table::Numeric && w->columnType(ycol) == Table::Numeric)
	{
		QStringList values=QStringList::split ("\t",text,false);
		w->setText(row,xcol,values[0]);
		w->setText(row,ycol,values[1]);
		updateCurves(w, colName);
		emit modified();
	}
	else
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("This operation cannot be performed on curves plotted from columns having a non-numerical format."));
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

		updateCurves(w, colName);
		delete[] dat;
		emit modified();
	}
	else
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("This operation cannot be performed on curves plotted from columns having a non-numerical format."));
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
					tr("This operation cannot be performed on curves plotted from columns having a non-numerical format."));
	}
}

void ApplicationWindow::renameListViewItem(const QString& oldName,const QString& newName)
{
	Q3ListViewItem *it=lv->findItem (oldName,0, Q3ListView::ExactMatch | Qt::CaseSensitive );
	if (it)
		it->setText(0,newName);
}

void ApplicationWindow::setListViewLabel(const QString& caption,const QString& label)
{
	Q3ListViewItem *it=lv->findItem ( caption, 0, Q3ListView::ExactMatch | Qt::CaseSensitive );
	if (it)
		it->setText(5,label);
}

void ApplicationWindow::setListViewDate(const QString& caption,const QString& date)
{
	Q3ListViewItem *it=lv->findItem ( caption, 0, Q3ListView::ExactMatch | Qt::CaseSensitive );
	if (it)
		it->setText(4,date);
}

void ApplicationWindow::setListView(const QString& caption,const QString& view)
{
	Q3ListViewItem *it=lv->findItem ( caption,0, Q3ListView::ExactMatch | Qt::CaseSensitive );
	if (it)
		it->setText(2,view);
}

void ApplicationWindow::setListViewSize(const QString& caption,const QString& size)
{
	Q3ListViewItem *it=lv->findItem ( caption,0, Q3ListView::ExactMatch | Qt::CaseSensitive );
	if (it)
		it->setText(3,size);
}

QString ApplicationWindow::listViewDate(const QString& caption)
{
	Q3ListViewItem *it=lv->findItem (caption,0, Q3ListView::ExactMatch | Qt::CaseSensitive );
	if (it)
		return it->text(4);
	else
		return "";
}

void ApplicationWindow::updateTableNames(const QString& oldName, const QString& newName)
{
	QWidgetList *windows = windowsList();
	foreach (QWidget *w, *windows)
	{
		if (w->isA("MultiLayer"))
		{
			QWidgetList gr_lst= ((MultiLayer*)w)->graphPtrs();
			Graph *g;
			foreach(QWidget *widget, gr_lst)
			{
				g = (Graph *)widget;
				//update plotted curves list
				QStringList onPlot=g->curvesList();
				for (int i=0; i<(int)onPlot.count(); i++)
				{
					QStringList cols = QStringList::split("_", onPlot[i], false);
					if (cols[0] == oldName)
						onPlot[i] = newName + "_" + cols[1];
				}
				g->insertPlottedList(onPlot);

				//update plot associations
				onPlot=g->plotAssociations();
				for (int k=0; k<(int)onPlot.count(); k++)
				{
					QStringList cols = QStringList::split (",", onPlot[k], false);
					for (int l=0; l<(int)cols.count(); l++)
					{
						QStringList lst = QStringList::split ("_", cols[l], false);
						if (lst[0] == oldName)
							cols[l] = newName + "_" + lst[1];
					}
					onPlot[k] = cols.join (",");
				}
				g->setPlotAssociations(onPlot);

				//update legend
				LegendMarker *legendMrk = g->legend();
				if (legendMrk)
				{
					onPlot = QStringList::split ("\n", legendMrk->getText(), FALSE );
					onPlot.gres (oldName,newName,TRUE);
					legendMrk->setText(onPlot.join("\n"));
					g->replot();
				}
			}
		}
		else if (w->isA("Graph3D"))
		{
			QString name = ((Graph3D*)w)->formula();
			if (name.contains(oldName,true))
			{
				name.replace(oldName,newName);
				((Graph3D*)w)->setPlotAssociation(name);
			}
		}
	}
	delete windows;
}

void ApplicationWindow::updateColNames(const QString& oldName, const QString& newName)
{
	QWidgetList *windows = windowsList();
	foreach (QWidget *w, *windows)
	{
		if (w->isA("MultiLayer"))
		{
			QWidgetList gr_lst= ((MultiLayer*)w)->graphPtrs();
			Graph *g;
			foreach (QWidget *widget, gr_lst)
			{
				g = (Graph *)widget;
				//update plotted curves list
				QStringList onPlot=g->curvesList();
				for (int i=0; i<(int)onPlot.count(); i++)
				{
					if (onPlot[i] == oldName)
						onPlot[i] = newName;
				}
				g->insertPlottedList(onPlot);

				//update plot associations
				onPlot=g->plotAssociations();
				for (int k=0; k<(int)onPlot.count(); k++)
				{
					QStringList cols = QStringList::split (",", onPlot[k], false);
					for (int l=0; l<(int)cols.count(); l++)
					{
						QString s = cols[l];
						int pos = s.findRev("(");
						QString colName = s.left(pos); 
						QString endString = s.right(s.length()-pos);

						if (colName == oldName)
							cols[l] = newName + endString;
					}
					onPlot[k] = cols.join (",");
				}
				g->setPlotAssociations(onPlot);

				//update legend
				LegendMarker *legendMrk = g->legend();
				if (legendMrk)
				{
					onPlot = QStringList::split ("\n", legendMrk->getText(), FALSE );
					onPlot.gres (oldName,newName,TRUE);
					legendMrk->setText(onPlot.join("\n"));
					g->replot();
				}
			}
		}
		else if (w->isA("Graph3D"))
		{
			QString name = ((Graph3D*)w)->formula();
			if (name.contains(oldName))
			{
				name.replace(oldName,newName);
				((Graph3D*)w)->setPlotAssociation(name);
			}
		}
	}
	delete windows;
}

void ApplicationWindow::changeMatrixName(const QString& oldName, const QString& newName)
{
	QWidgetList * lst = windowsList();
	foreach(QWidget *w, *lst)
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
	if (!m)
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QWidgetList *windows = windowsList();
	foreach(QWidget *w, *windows)
	{
		if (w->isA("Graph3D") && ((Graph3D*)w)->getMatrix() == m)
			((Graph3D*)w)->clearData();
	}
	delete windows;
	QApplication::restoreOverrideCursor();
}

void ApplicationWindow::update3DMatrixPlots(QWidget *window)
{
	if (!window)
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QWidgetList *windows = windowsList();
	foreach(QWidget *w, *windows)
	{
		if (w->isA("Graph3D") && ((Graph3D*)w)->getMatrix() == window)
			((Graph3D*)w)->updateMatrixData((Matrix *)window);
	}

	delete windows;
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

	DataSetDialog *ad=new DataSetDialog(tr("Column :"));
	ad->setAttribute(Qt::WA_DeleteOnClose);
	connect (ad,SIGNAL(options(const QString&)), this, SLOT(insertNew3DData(const QString&)));
	ad->setWindowTitle(tr("QtiPlot - Choose data set"));
	ad->setCurveNames(zColumns);
	ad->exec();
}

void ApplicationWindow::change3DData()
{
	DataSetDialog *ad=new DataSetDialog(tr("Column :"));
	ad->setAttribute(Qt::WA_DeleteOnClose);
	connect (ad,SIGNAL(options(const QString&)), this, SLOT(change3DData(const QString&)));

	ad->setWindowTitle(tr("QtiPlot - Choose data set"));
	ad->setCurveNames(columnsList(Table::Z));
	ad->exec();
}

void ApplicationWindow::change3DMatrix()
{
	DataSetDialog *ad=new DataSetDialog(tr("Matrix :"));
	ad->setAttribute(Qt::WA_DeleteOnClose);
	connect (ad,SIGNAL(options(const QString&)), this, SLOT(change3DMatrix(const QString&)));

	ad->setWindowTitle(tr("QtiPlot - Choose matrix to plot"));
	ad->setCurveNames(matrixNames());
	ad->exec();
}

void ApplicationWindow::change3DMatrix(const QString& matrix_name)
{
	if ( ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
	{
		((Graph3D*)ws->activeWindow())->changeMatrix(matrix(matrix_name));
		emit modified();
	}
}

void ApplicationWindow::add3DMatrixPlot()
{
	QStringList matrices = matrixNames();
	if ((int)matrices.count() <= 0)
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no matrices available in this project.</h4>"
					"<p><h4>Please create a matrix and try again!</h4>"));
		return;
	}

	DataSetDialog *ad=new DataSetDialog(tr("Matrix :"));
	ad->setAttribute(Qt::WA_DeleteOnClose);
	connect (ad,SIGNAL(options(const QString&)), this, SLOT(insert3DMatrixPlot(const QString&)));

	ad->setWindowTitle(tr("QtiPlot - Choose matrix to plot"));
	ad->setCurveNames(matrices);
	ad->exec();
}

void ApplicationWindow::insert3DMatrixPlot(const QString& matrix_name)
{
	if ( ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
	{
		((Graph3D*)ws->activeWindow())->addMatrixData(matrix(matrix_name));
		emit modified();
	}
}

void ApplicationWindow::insertNew3DData(const QString& colName)
{
	if ( ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
	{
		((Graph3D*)ws->activeWindow())->insertNewData(table(colName),colName);
		emit modified();
	}
}

void ApplicationWindow::change3DData(const QString& colName)
{
	if ( ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
	{
		((Graph3D*)ws->activeWindow())->changeDataColumn(table(colName),colName);
		emit modified();
	}
}

void ApplicationWindow::editSurfacePlot()
{
	if ( ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
	{
		Graph3D* g = (Graph3D*)ws->activeWindow();

		SurfaceDialog* sd= new SurfaceDialog(this,"FunctionDialog",true,0);
		sd->setAttribute(Qt::WA_DeleteOnClose);
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
		sd->exec();
	}
}

void ApplicationWindow::newSurfacePlot()
{
	SurfaceDialog* sd= new SurfaceDialog(this,"FunctionDialog",true,0);
	sd->setAttribute(Qt::WA_DeleteOnClose);
	connect (sd,SIGNAL(options(const QString&,double,double,double,double,double,double)),
			this,SLOT(newPlot3D(const QString&,double,double,double,double,double,double)));
	connect (sd,SIGNAL(clearFunctionsList()),this,SLOT(clearSurfaceFunctionsList()));

	sd->insertFunctionsList(surfaceFunc);
	sd->exec();
}

Graph3D* ApplicationWindow::newPlot3D(const QString& formula, double xl, double xr,
		double yl, double yr, double zl, double zr)
{
	QString label="graph"+QString::number(++graphs);
	while(alreadyUsedName(label)){
		label="graph"+QString::number(++graphs);}

	Graph3D *plot=new Graph3D("",ws,0);
	plot->setAttribute(Qt::WA_DeleteOnClose);
	plot->addFunction(formula, xl, xr, yl, yr, zl, zr);
	plot->resize(500,400);
	plot->setWindowTitle(label);
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
	Graph3D *plot=new Graph3D("",ws,0);
	plot->setAttribute(Qt::WA_DeleteOnClose);
	plot->addFunction(formula, xl, xr, yl, yr, zl, zr);
	plot->update();

	QString label=caption;
	while(alreadyUsedName(label))
	{
		graphs++;
		label="graph"+QString::number(graphs);
	}

	plot->setWindowTitle(label);
	plot->setName(label);
	initPlot3D(plot);
	return plot;
}

Graph3D* ApplicationWindow::dataPlot3D(Table* table, const QString& colName)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	graphs++;
	QString label="graph"+QString::number(graphs);
	while(alreadyUsedName(label))
	{
		graphs++;
		label="graph"+QString::number(graphs);
	}

	Graph3D *plot=new Graph3D("", ws, 0);
	plot->setAttribute(Qt::WA_DeleteOnClose);
	plot->addData(table, colName);
	plot->resize(500,400);
	plot->setWindowTitle(label);
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

	Graph3D *plot=new Graph3D("", ws, 0);
	plot->setAttribute(Qt::WA_DeleteOnClose);
	plot->addData(w, xCol, yCol, xl, xr, yl, yr, zl, zr);
	plot->update();

	QString label=caption;
	while(alreadyUsedName(label))
	{
		graphs++;
		label="graph"+QString::number(graphs);
	}

	plot->setWindowTitle(label);
	plot->setName(label);
	initPlot3D(plot);

	return plot;
}

//plot ribbon from the plot wizard
Graph3D* ApplicationWindow::dataPlot3D(const QString& formula)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

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

	Graph3D *plot=new Graph3D("", ws, 0);
	plot->setAttribute(Qt::WA_DeleteOnClose);
	plot->addData(w, xColName, yColName);
	plot->resize(500,400);
	plot->setWindowTitle(label);
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
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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

	Graph3D *plot=new Graph3D("", ws,0);
	plot->setAttribute(Qt::WA_DeleteOnClose);
	plot->addData(table, xCol, yCol, zCol, type);
	plot->resize(500,400);
	plot->setWindowTitle(label);
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

	Graph3D *plot=new Graph3D("", ws, 0);
	plot->setAttribute(Qt::WA_DeleteOnClose);
	plot->addData(w, xCol, yCol, zCol, xl, xr, yl, yr, zl, zr);
	plot->update();

	QString label=caption;
	while(alreadyUsedName(label))
	{
		graphs++;
		label="graph"+QString::number(graphs);
	}

	plot->setWindowTitle(label);
	plot->setName(label);
	initPlot3D(plot);
	return plot;
}

//plot 3D data from plot wizard string
Graph3D* ApplicationWindow::dataPlotXYZ(const QString& formula)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

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

	Graph3D *plot=new Graph3D("", ws,0);
	plot->setAttribute(Qt::WA_DeleteOnClose);
	plot->addData(w, xCol, yCol, zCol, 1);
	plot->resize(500,400);

	QString label="graph"+QString::number(++graphs);
	while(alreadyUsedName(label))
	{
		label="graph"+QString::number(++graphs);
	}
	plot->setWindowTitle(label);
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
	plot->setOrtho(orthogonal3DPlots);
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
	ws->addWindow(plot);
	connectSurfacePlot(plot);

	plot->setIcon(QPixmap(trajectory_xpm));
	plot->show();
	plot->setFocus();

	addListViewItem(plot);
	current_folder->addWindow(plot);
	plot->setFolder(current_folder);

	if (!plot3DTools->isVisible())
		plot3DTools->show();

	if (!plot3DTools->isEnabled())
		plot3DTools->setEnabled(true);

	customMenu((QWidget*)plot);
	customToolBars((QWidget*)plot);
}

void ApplicationWindow::importImage()
{
	QList<QByteArray> list = QImageReader::supportedImageFormats();
	QString filter = tr("Images") + " (", aux1, aux2;
	for (int i=0; i<(int)list.count(); i++)
	    {
		aux1 = " *."+list[i]+" ";
		aux2 += " *."+list[i]+";;";
		filter += aux1;
	    }
	filter+=");;" + aux2;

	QString fn = Q3FileDialog::getOpenFileName(workingDir, filter, this, 0,
			tr("QtiPlot - Import image from file"), 0, true);
	if ( !fn.isEmpty() )
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QPixmap photo;
		for (int i=0; i<(int)list.count();i++)
			{
			if (fn.contains("." + list[i], false))
				{
				photo.load(fn,list[i],QPixmap::Auto);
				break;
				}
			}
		Matrix* m = createIntensityMatrix(photo);
		m->setWindowLabel(fn);
		m->setCaptionPolicy(MyWidget::Both);
		setListViewLabel(m->name(), fn);

		QFileInfo fi(fn);
		workingDir = fi.dirPath(true);

		QApplication::restoreOverrideCursor();
	}
}

void ApplicationWindow::loadImage()
{
	QList<QByteArray> list = QImageReader::supportedImageFormats();
	QString filter = tr("Images") + " (", aux1, aux2;
	for (int i=0; i<(int)list.count(); i++)
	    {
		aux1 = " *."+list[i]+" ";
		aux2 += " *."+list[i]+";;";
		filter += aux1;
	    }
	filter+=");;" + aux2;

	QString fn = Q3FileDialog::getOpenFileName(workingDir, filter, this, 0,
			tr("QtiPlot - Load image from file"), 0, true);
	if ( !fn.isEmpty() )
	{
		loadImage(fn);
		QFileInfo fi(fn);
		workingDir = fi.dirPath(true);
	}
}

void ApplicationWindow::loadImage(const QString& fn)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QPixmap photo;
	QList<QByteArray> lst = QImageReader::supportedImageFormats();
    for (int i=0; i<(int)lst.count(); i++)
		  {
		  if (fn.contains("." + lst[i], false))
			  {
			  photo.load(fn, lst[i], QPixmap::Auto);
			  break;
			  }
		  }

	  MultiLayer *plot = multilayerPlot("graph" + QString::number(++graphs));
	  plot->setWindowLabel(fn);
	  plot->setCaptionPolicy(MyWidget::Both);
	  setListViewLabel(plot->name(), fn);

	  if (plot->height()-20>photo.height())
		  plot->setGeometry(0,0, plot->width(), photo.height()+20);

	  plot->showNormal();
	  Graph *g=plot->addLayer(0,0, plot->width(), plot->height()-20);

	  g->setTitle("");
	  Q3MemArray<bool> axesOn(4);
	  for (int j=0;j<4;j++)
		axesOn[j]=false;
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
		QList<int> ticksList;
		int ticksStyle = ScaleDraw::Out;
		ticksList<<ticksStyle<<ticksStyle<<ticksStyle<<ticksStyle;
		g->setMajorTicksType(ticksList);
		g->setMinorTicksType(ticksList);
	}
	if (style == Graph::HorizontalBars)
	{
		g->setAxisTitle(0, tr("Y Axis Title"));
		g->setAxisTitle(1, tr("X Axis Title"));
	}
}

MultiLayer* ApplicationWindow::multilayerPlot(const QString& caption)
{
	MultiLayer* g = new MultiLayer("", ws,0);
	g->setAttribute(Qt::WA_DeleteOnClose);
	QString label=caption;
	initMultilayerPlot(g, label.replace(QRegExp("_"),"-"));
	return g;
}

/*
 *creates a new empty multilayer plot
 */
void ApplicationWindow::newGraph()
{
	MultiLayer* g = multilayerPlot(tr("graph1"));
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
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	MultiLayer* g = new MultiLayer("",ws,0);
	g->setAttribute(Qt::WA_DeleteOnClose);
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
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return 0;

	Table* w = (Table*)ws->activeWindow();
	if (!w->valid2DPlot())
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

	MultiLayer* g = new MultiLayer("", ws,0);
	g->setAttribute(Qt::WA_DeleteOnClose);
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
				activeGraph->insertCurvesList(w, QStringList(list[i]), style, defaultCurveLineWidth, defaultSymbolSize);
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
				activeGraph->insertCurvesList(w, QStringList(list[i]), style, defaultCurveLineWidth, defaultSymbolSize);
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

	QWidgetList lst = g->graphPtrs();
	for (int i=0; i<g->graphsNumber();i++)
	{
		Graph *ag = (Graph *)lst.at(i);
		ag->setAutoscaleFonts(autoScaleFonts);//restore user defined fonts behaviour
	}
	emit modified();
	return g;
}

MultiLayer* ApplicationWindow::multilayerPlot(const QStringList& colList)
{//used when plotting from wizard
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	MultiLayer* g = new MultiLayer("", ws,0);
	g->setAttribute(Qt::WA_DeleteOnClose);
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
			ag->addErrorBars(w,xColName,yColName,w,errColName,errType,2,5,QColor(Qt::black),false,true,true);
		}
		else
		{
			if (ag->insertCurve(w, xCol, yColName, defaultCurveStyle))
			{
				CurveLayout cl = ag->initCurveLayout(i, curves, defaultCurveStyle);
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
	ws->addWindow(g);
	connectMultilayerPlot(g);

	QString label = name;
	while(alreadyUsedName(label))
	{
		label="graph"+QString::number(++graphs);
	}

	g->setWindowTitle(label);
	g->setName(label);
	g->setIcon(QPixmap(graph_xpm));
	g->showNormal();
	g->setFocus();

	addListViewItem(g);
	current_folder->addWindow(g);
	g->setFolder(current_folder);
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
	foreach(QWidget *w, *windows)
	{
		if (w->inherits("Table"))
			customTable((Table*)w);
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
			Q3MemArray<bool> axesOn(QwtPlot::axisCnt);
			axesOn.fill (true);
			g->enableAxes(axesOn);
			g->updateSecondaryAxis(QwtPlot::xTop);
			g->updateSecondaryAxis(QwtPlot::yRight);
		}

		QList<int> ticksList;
		ticksList<<majTicksStyle<<majTicksStyle<<majTicksStyle<<majTicksStyle;
		g->setMajorTicksType(ticksList);
		ticksList.clear();
		ticksList<<minTicksStyle<<minTicksStyle<<minTicksStyle<<minTicksStyle;
		g->setMinorTicksType(ticksList);

		g->setTicksLength (minTicksLength, majTicksLength);
		g->setAxesLinewidth(axesLineWidth);
		g->drawAxesBackbones(drawBackbones);
	}

	g->initFonts(plotAxesFont, plotNumbersFont);
	g->setTextMarkerDefaults(legendFrameStyle, plotLegendFont, legendTextColor, legendBackground);
	g->customLegend();

	g->setArrowDefaults(defaultArrowLineWidth, defaultArrowColor, defaultArrowLineStyle,
			defaultArrowHeadLength, defaultArrowHeadAngle, defaultArrowHeadFill);

	g->initTitle(titleOn, plotTitleFont);
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
		g->setXAxisTitle(tr("pixels"));
		g->setYAxisTitle(tr("pixel intensity (a.u.)"));
	}
}

/*
 *used when importing an ASCII file
 */
Table* ApplicationWindow::newTable(const QString& fname, const QString &sep, 
		int lines, bool renameCols, bool stripSpaces, 
		bool simplifySpaces)
{
	Table* w = new Table(scriptEnv, fname, sep, lines, renameCols, stripSpaces, 
			simplifySpaces, fname, ws, 0, 0);
	w->setAttribute(Qt::WA_DeleteOnClose);
	if (w)
	{	
		initTable(w, "table"+QString::number(++tables));
		w->show();
	}
	return w;
}

/*
 *creates a new empty table
 */
Table* ApplicationWindow::newTable()
{
	Table* w = new Table(scriptEnv, 30, 2, "", ws, 0);
	w->setAttribute(Qt::WA_DeleteOnClose);
	initTable(w, "table"+QString::number(++tables));
	w->showNormal();	
	return w;
}

/*
 *used when opening a project file
 */
Table* ApplicationWindow::newTable(const QString& caption, int r, int c)
{
	Table* w = new Table(scriptEnv, r, c, "", ws, 0);
	w->setAttribute(Qt::WA_DeleteOnClose);
	initTable(w, caption);
	if (w->name() != caption)//the table was renamed
	{
		renamedTables << caption << w->name();

		QApplication::restoreOverrideCursor();
		QMessageBox:: warning(this, "QtiPlot - Renamed Window", 
				tr("The table '%1' already exists. It has been renamed '%2'.").arg(caption).arg(w->name()));
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	}
	return w;
}

Table* ApplicationWindow::newTable(const QString& caption, int r, int c, const QString& text)
{
	QStringList lst = QStringList::split("\t", caption, false);
	Table* w = new Table(scriptEnv, r, c, lst[1], ws, 0);
	w->setAttribute(Qt::WA_DeleteOnClose);

	QStringList rows=QStringList::split ("\n",text,false);
	QString rlist=rows[0];
	QStringList list=QStringList::split ("\t",rlist,true);
	w->setHeader(list);

	for (int i=0; i<r; i++)
	{
		rlist=rows[i+1];
		list=QStringList::split ("\t",rlist,true);
		for (int j=0; j<c; j++)
			w->setText(i, j, list[j]);
	}

	initTable(w, lst[0]);
	w->setCaptionPolicy(MyWidget::Both);
	w->showNormal();
	return w;
}

/*
 *used to return the result of an analysis operation
 */
Table* ApplicationWindow::newHiddenTable(const QString& caption, int r, int c, const QString& text)
{
	QStringList lst = QStringList::split("\t", caption, false);
	Table* w = new Table(scriptEnv, r, c, lst[1], 0, 0);
	w->setAttribute(Qt::WA_DeleteOnClose);

	QStringList rows=QStringList::split ("\n",text,false);
	QString rlist=rows[0];
	QStringList list=QStringList::split ("\t",rlist,true);
	w->setHeader(list);

	for (int i=0; i<r; i++)
	{
		rlist=rows[i+1];
		list=QStringList::split ("\t",rlist,true);
		for (int j=0; j<c; j++)
			w->setText(i, j, list[j]);
	}

	initTable(w, lst[0]);
	w->setCaptionPolicy(MyWidget::Both);
	outWindows->append(w);
	w->setHidden();
	return w;
}

Table* ApplicationWindow::newHiddenTable(const QString& name, const QString& legend, int r, int c)
{
	Table* w = new Table(scriptEnv, r, c, legend, 0, 0);	
	w->setAttribute(Qt::WA_DeleteOnClose);
	initTable(w, name);
	w->setCaptionPolicy(MyWidget::Both);
	outWindows->append(w);
	w->setHidden();
	return w;
}


void ApplicationWindow::initTable(Table* w, const QString& caption)
{
	ws->addWindow(w);
	connectTable(w);
	customTable(w);

	QString name=caption;
	name=name.replace ("_","-");

	while(alreadyUsedName(name)){
		name="table"+QString::number(++tables);}

	tableWindows<<name;
	w->setWindowTitle(name);
	w->setName(name);
	w->setIcon( QPixmap(worksheet_xpm) );

	addListViewItem(w);
	current_folder->addWindow(w);
	w->setFolder(current_folder);

	emit modified();
}

void ApplicationWindow::showHistogramTable(const QString& caption, int r, int c, const QString& text)
{
	Table* w = newTable(caption, r, c, text);
	w->showMaximized();
}

/*
 * !creates a new table with type statistics on target columns/rows of table base
 */
TableStatistics *ApplicationWindow::newTableStatistics(Table *base, int type, QList<int> target, const QString &caption)
{
	TableStatistics* s = new TableStatistics(scriptEnv, ws, base, (TableStatistics::Type) type, target);
	if (caption.isEmpty())
		initTable(s, s->name());
	else
		initTable(s, caption);
	connect(base, SIGNAL(modifiedData(Table*,const QString&)), s, SLOT(update(Table*,const QString&)));
	connect(base, SIGNAL(changedColHeader(const QString&, const QString&)), s, SLOT(renameCol(const QString&, const QString&)));
	connect(base, SIGNAL(removedCol(const QString&)), s, SLOT(removeCol(const QString&)));
	s->showNormal();
	return s;
}

/*
 *creates a new empty note window
 */
Note* ApplicationWindow::newNote(const QString& caption)
{
	Note* m = new Note(scriptEnv, "", ws);
	if (caption.isEmpty())
		initNote(m, tr("Note") + QString::number(++notes));
	else
		initNote(m, caption);
	m->showNormal();	
	return m;
}

void ApplicationWindow::initNote(Note* m, const QString& caption)
{
	ws->addWindow(m);
	QString name=caption;
	while(name.isEmpty() || alreadyUsedName(name))
		name = "Note"+QString::number(++notes);

	m->setWindowTitle(name);
	m->setName(name);
	m->setIcon( QPixmap(note_xpm) );
	m->askOnCloseEvent(confirmCloseNotes);

	addListViewItem(m);
	current_folder->addWindow(m);
	m->setFolder(current_folder);

	connect(m->textWidget(), SIGNAL(undoAvailable(bool)), actionUndo, SLOT(setEnabled(bool)));
	connect(m->textWidget(), SIGNAL(redoAvailable(bool)), actionRedo, SLOT(setEnabled(bool)));
	connect(m, SIGNAL(modifiedWindow(QWidget*)), this, SLOT(modifiedProject(QWidget*)));
	connect(m, SIGNAL(closedWindow(MyWidget*)), this, SLOT(closeWindow(MyWidget*)));
	connect(m, SIGNAL(hiddenWindow(MyWidget*)), this, SLOT(hideWindow(MyWidget*)));
	connect(m,SIGNAL(statusChanged(MyWidget*)),this, SLOT(updateWindowStatus(MyWidget*)));

	emit modified();
}

/*
 *creates a new empty matrix
 */
Matrix* ApplicationWindow::newMatrix()
{
	Matrix* m = new Matrix(scriptEnv, 32, 32, "", ws, 0);
	m->setAttribute(Qt::WA_DeleteOnClose);
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
	Matrix* w = new Matrix(scriptEnv, r, c, "", ws,0);
	w->setAttribute(Qt::WA_DeleteOnClose);
	initMatrix(w, caption);
	if (w->name() != caption)//the matrix was renamed
	{
		renamedTables << caption << w->name();

		QApplication::restoreOverrideCursor();
		QMessageBox:: warning(this, "QtiPlot - Renamed Window", 
				tr("The matrix '%1' already exists. It has been renamed '%2'.").arg(caption).arg(w->name()));
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	}
	return w;
}

void ApplicationWindow::transposeMatrix()
{
	Matrix* m = (Matrix*)ws->activeWindow();
	if (!m)
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int rows = m->numRows();
	int cols = m->numCols();

	Table* w = new Table(scriptEnv, rows, cols, "", ws, 0);
	w->setAttribute(Qt::WA_DeleteOnClose);
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
	ws->addWindow(m);
	QString name=caption;
	while(alreadyUsedName(name)){
		name = "Matrix"+QString::number(++matrixes);}

	m->setWindowTitle(name);
	m->setName(name);
	m->setIcon( QPixmap(matrix_xpm) );
	m->askOnCloseEvent(confirmCloseMatrix);

	addListViewItem(m);
	current_folder->addWindow(m);
	m->setFolder(current_folder);

	connect(m, SIGNAL(modifiedWindow(QWidget*)), this, SLOT(modifiedProject()));
	connect(m, SIGNAL(modifiedWindow(QWidget*)), this, SLOT(update3DMatrixPlots(QWidget *)));
	connect(m, SIGNAL(closedWindow(MyWidget*)), this, SLOT(closeWindow(MyWidget*)));
	connect(m, SIGNAL(hiddenWindow(MyWidget*)), this, SLOT(hideWindow(MyWidget*)));
	connect(m, SIGNAL(statusChanged(MyWidget*)),this, SLOT(updateWindowStatus(MyWidget*)));
	connect(m, SIGNAL(showContextMenu()), this, SLOT(showWindowContextMenu()));

	emit modified();
}

Matrix* ApplicationWindow::convertTableToMatrix()
{
	Table* m = (Table*)ws->activeWindow();
	if (!m)
		return 0;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	int rows = m->tableRows();
	int cols = m->tableCols();

	Matrix* w = new Matrix(scriptEnv, rows, cols, "", ws, 0);
	w->setAttribute(Qt::WA_DeleteOnClose);
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

QWidget* ApplicationWindow::window(const QString& name)
{
	QWidget* w=0;
	QWidgetList *windows = windowsList();
	for (int i = 0; i < int(windows->count());i++ )
	{
		if (windows->at(i)->name() == name)
		{
			w = windows->at(i);
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

	QList<QWidget*> *lst = windowsList();
	foreach(QWidget *w, *lst)
	{
		if (w->inherits("Table") && w->name() == caption)
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

	QWidgetList *lst = windowsList();
	foreach(QWidget *w, *lst)
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
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
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
			ErrDialog* ed = new ErrDialog(this ,0);
			ed->setAttribute(Qt::WA_DeleteOnClose);
			connect (ed,SIGNAL(options(const QString&,int,const QString&,int)),this,SLOT(defineErrorBars(const QString&,int,const QString&,int)));
			connect (ed,SIGNAL(options(const QString&,const QString&,int)),this,SLOT(defineErrorBars(const QString&,const QString&,int)));

			QStringList curvesOnPlot=activeGraph->curvesList();

			ed->setCurveNames(curvesOnPlot);
			ed->setSrcTables(tableList());
			ed->exec();
		}
	}
}

void ApplicationWindow::defineErrorBars(const QString& name, int type, const QString& percent, int direction)
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

	if (direction == QwtErrorPlotCurve::Horizontal)
		w->addCol(Table::xErr);
	else
		w->addCol(Table::yErr);

	int r=w->tableRows();
	int c=w->tableCols()-1;
	int ycol=w->colIndex(name);
	if (!direction)
		ycol=w->colIndex(xColName);

	QVarLengthArray<double> Y(r);
	Y = w->col(ycol);
	QString errColName = w->colName(c);

	double prc=percent.toDouble();
	double moyenne=0.0;
	if (type==0)
	{
		for (int i=0;i<r;i++)
		{
			if (!w->text(i,ycol).isEmpty())
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
			if (!w->table()->item(i,ycol)->text().isEmpty())
				w->setText(i,c,QString::number(dev,'g',15));
		}
	}		
	activeGraph->addErrorBars(w, xColName, name, w, errColName, direction);
}

void ApplicationWindow::defineErrorBars(const QString& curveName, const QString& errColumnName, int direction)
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
	activeGraph->addErrorBars(w, curveName, errTable, errColumnName, direction);
	emit modified();
}

void ApplicationWindow::removeCurves(const QString& name)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QWidgetList *windows = windowsList();
	foreach(QWidget *w, *windows)
	{
		if (w->isA("MultiLayer"))
		{
			QWidgetList lst= ((MultiLayer*)w)->graphPtrs();
			Graph *g;
			foreach(QWidget *widget, lst)
			{
				g = (Graph *)widget;
				QStringList associations=g->plotAssociations();		
				for (int i=0; i<int(associations.count()); i++)
				{
					QString ass = associations[i];
					if (ass.contains(name))
						g->removeCurve(ass);
				}			
			}
		}
		else if (w->isA("Graph3D"))
		{
			if ( (((Graph3D*)w)->formula()).contains(name) )
				((Graph3D*)w)->clearData();
		}
	}
	delete windows;
	QApplication::restoreOverrideCursor();
}

void ApplicationWindow::updateCurves(Table *t, const QString& name)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QWidgetList *windows = windowsList();
	foreach(QWidget *w, *windows)
	{
		if (w->isA("MultiLayer"))
		{
			QWidgetList graphsList = ((MultiLayer*)w)->graphPtrs();
			for (int k=0; k<(int)graphsList.count(); k++)
			{
				Graph* g=(Graph*)graphsList.at(k);
				if (g && g->curves() > 0)
				{
					bool modified = false;
					QStringList as=g->plotAssociations();
					for (int j=0; j<g->curves(); j++)
					{
						if (as[j].contains(name))
						{
							modified = true;
							g->updateCurveData(t, name, j);
						}
					}
					if (modified)
						g->updatePlot();
				}
			}
		}
		else if (w->isA("Graph3D"))
		{
			Graph3D* g = (Graph3D*)w;
			if ((g->formula()).contains(name))
				g->updateData(t);
		}
	}
	delete windows;
	QApplication::restoreOverrideCursor();
}

void ApplicationWindow::showPreferencesDialog()
{
	ConfigDialog* cd= new ConfigDialog(this);
	cd->setAttribute(Qt::WA_DeleteOnClose);
	cd->setColumnSeparator(columnSeparator);
	cd->exec();
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
	// style keys are case insensitive
	if (appStyle.toLower() == s.toLower())
		return;

	qApp->setStyle(s);
	appStyle = qApp->style()->objectName();

	QPalette pal = qApp->palette();
	pal.setColor (QPalette::Active, QPalette::Base, QColor(panelsColor));
	qApp->setPalette(pal);

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
	scriptingMenu->setFont(appFont);
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
	info->setFont(QFont(appFont.family(),2+appFont.pointSize(),QFont::Bold,false));
}

void ApplicationWindow::updateConfirmOptions(bool askTables, bool askMatrixes, bool askPlots2D,
		bool askPlots3D, bool askNotes)
{
	QList<QWidget*> *windows = windowsList();
	if (confirmCloseTable != askTables)
	{
		confirmCloseTable=askTables;
		for (int i = 0; i < int(windows->count());i++ )
		{
			if (windows->at(i)->isA("Table"))
				((MyWidget*)windows->at(i))->askOnCloseEvent(confirmCloseTable);
		}
	}

	if (confirmCloseMatrix != askMatrixes)
	{
		confirmCloseMatrix = askMatrixes;
		for (int i = 0; i < int(windows->count());i++ )
		{
			if (windows->at(i)->isA("Matrix"))
				((MyWidget*)windows->at(i))->askOnCloseEvent(confirmCloseMatrix);
		}
	}

	if (confirmClosePlot2D != askPlots2D)
	{
		confirmClosePlot2D=askPlots2D;
		for (int i = 0; i < int(windows->count());i++ )
		{
			if (windows->at(i)->isA("MultiLayer"))
				((MyWidget*)windows->at(i))->askOnCloseEvent(confirmClosePlot2D);
		}
	}

	if (confirmClosePlot3D != askPlots3D)
	{
		confirmClosePlot3D=askPlots3D;
		for (int i = 0; i < int(windows->count());i++ )
		{
			if (windows->at(i)->isA("Graph3D"))
				((MyWidget*)windows->at(i))->askOnCloseEvent(confirmClosePlot3D);
		}
	}

	if (confirmCloseNotes != askNotes)
	{
		confirmCloseNotes = askNotes;
		for (int i = 0; i < int(windows->count());i++ )
		{
			if (windows->at(i)->isA("Note"))
				((MyWidget*)windows->at(i))->askOnCloseEvent(confirmCloseNotes);
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

	QWidgetList *windows = windowsList();
	foreach(QWidget *w, *windows)
	{
		if (w->isA("MultiLayer"))
		{
			QWidgetList lst = ((MultiLayer*)w)->graphPtrs();
			Graph *g;
			foreach(QWidget *widget, lst)
			{
				g = (Graph *)widget;
				g->enableAutoscaling(autoscale2DPlots);
				g->updateScale();
				g->setIgnoreResizeEvents(!autoResizeLayers);
				g->setAutoscaleFonts(autoScaleFonts);
			}
		}
	}
	delete windows;
}

void ApplicationWindow::setLegendDefaultSettings(int frame, const QFont& font, 
		const QColor& textCol, const QColor& backgroundCol)
{
	if (legendFrameStyle == frame && 
			legendTextColor == textCol &&
			legendBackground == backgroundCol &&
			plotLegendFont == font)
		return;

	legendFrameStyle = frame;
	legendTextColor = textCol;
	legendBackground = backgroundCol;
	plotLegendFont = font;

	QWidgetList *windows = windowsList();
	foreach(QWidget *w, *windows)
	{
		if (w->isA("MultiLayer"))
		{
			QWidgetList graphsList = ((MultiLayer*)w)->graphPtrs();
			foreach(QWidget *widget, graphsList)
				((Graph *)widget)->setTextMarkerDefaults(frame, font, textCol, backgroundCol);
		}
	}
	delete windows;
	saveSettings();
}

void ApplicationWindow::setArrowDefaultSettings(int lineWidth,  const QColor& c, Qt::PenStyle style,
		int headLength, int headAngle, bool fillHead)
{
	if (defaultArrowLineWidth == lineWidth && 
			defaultArrowColor == c &&
			defaultArrowLineStyle == style &&
			defaultArrowHeadLength == headLength &&
			defaultArrowHeadAngle == headAngle &&
			defaultArrowHeadFill == fillHead)
		return;

	defaultArrowLineWidth = lineWidth; 
	defaultArrowColor = c;
	defaultArrowLineStyle = style;
	defaultArrowHeadLength = headLength;
	defaultArrowHeadAngle = headAngle;
	defaultArrowHeadFill = fillHead;

	QWidgetList *windows = windowsList();
	foreach(QWidget *w, *windows)
	{
		if (w->isA("MultiLayer"))
		{
			QWidgetList graphsList = ((MultiLayer*)w)->graphPtrs();
			foreach(QWidget *widget, graphsList)
				((Graph *)widget)->setArrowDefaults(defaultArrowLineWidth, defaultArrowColor, 

						defaultArrowLineStyle, defaultArrowHeadLength,
						defaultArrowHeadAngle, defaultArrowHeadFill);
		}
	}
	delete windows;
	saveSettings();
}

ApplicationWindow * ApplicationWindow::plotFile(const QString& fn)
{	
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	ApplicationWindow *app= new ApplicationWindow();
	app->applyUserSettings();
	app->showMaximized();

	Table* t = app->newTable(fn, app->columnSeparator, 0, true, app->strip_spaces, app->simplify_spaces);
	t->setCaptionPolicy(MyWidget::Both);	
	app->multilayerPlot(t, t->YColumns(),Graph::LineSymbols);
	QApplication::restoreOverrideCursor();
	return 0;
}

void ApplicationWindow::showImportDialog()
{
	ImportDialog* id= new ImportDialog(this, Qt::WindowContextHelpButtonHint);
	id->setAttribute(Qt::WA_DeleteOnClose);
	connect (id, SIGNAL(options(const QString&, int, bool, bool, bool)),
			this, SLOT(setImportOptions(const QString&, int, bool, bool, bool)));

	id->setSeparator(columnSeparator);
	id->setLines(ignoredLines);
	id->renameCols(renameColumns);
	id->setWhiteSpaceOptions(strip_spaces, simplify_spaces);
	id->exec();
}

void ApplicationWindow::setImportOptions(const QString& sep, int lines, bool rename,
		bool strip, bool simplify)
{
	columnSeparator = sep;
	ignoredLines = lines;
	renameColumns = rename;
	strip_spaces = strip;
	simplify_spaces = simplify;
}

void ApplicationWindow::loadASCII()
{
	QString filter=tr("All files") + " *;;" + tr("Text") + " (*.TXT *.txt);;" +
		tr("Data")+" (*DAT *.dat);;" + tr("Comma Separated Values") + " (*.CSV *.csv);;";
	QString fn = QFileDialog::getOpenFileName(workingDir, filter, this, 0,
			tr("QtiPlot - Import ASCII File"), 0, true);
	if (!fn.isEmpty())
	{
		Table* t = (Table*)ws->activeWindow();
		if ( t && t->isA("Table"))
		{
			t->importASCII(fn, columnSeparator, ignoredLines, renameColumns, 
					strip_spaces, simplify_spaces, false);
			t->setWindowLabel(fn);
		}
		else
			t = newTable(fn, columnSeparator, ignoredLines, renameColumns, 
					strip_spaces, simplify_spaces);

		t->setCaptionPolicy(MyWidget::Both);
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
			workingDir = fd->directory().path();
			loadMultipleASCIIFiles(fd->selectedFiles(), fd->importFileAs());
		}
	}
	else
	{
		ImportFilesDialog *fd = new ImportFilesDialog(false, this, 0);
		fd->setDir(workingDir);
		if ( fd->exec() == QDialog::Accepted )
		{
			workingDir = fd->directory().path();
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
		Table *firstTable=newTable(fn, columnSeparator, ignoredLines, renameColumns, 
				strip_spaces, simplify_spaces);
		if (!firstTable)
			return;

		firstTable->setCaptionPolicy(MyWidget::Both);
		setListViewLabel(firstTable->name(), fn);

		int dx=firstTable->verticalHeaderWidth();
		int dy=firstTable->parentWidget()->frameGeometry().height() - firstTable->height();
		firstTable->parentWidget()->move(QPoint(0,0));

		for (int i=1;i<files;i++)
		{
			fn  = fileNames[i];
			Table *w = newTable(fn, columnSeparator, ignoredLines, renameColumns, 
					strip_spaces, simplify_spaces);
			if (w)
			{
				w->setCaptionPolicy(MyWidget::Both);
				setListViewLabel(w->name(), fn);
				w->parentWidget()->move(QPoint(i*dx,i*dy));
			}
		}

		modifiedProject();
	}
	else
	{
		if (ws->activeWindow() && ws->activeWindow()->isA("Table"))
		{
			Table* t = (Table*)ws->activeWindow();

			for (int i=0; i<files; i++)
				t->importMultipleASCIIFiles(fileNames[i], columnSeparator, ignoredLines, renameColumns, 
						strip_spaces, simplify_spaces, importFileAs);
			t->setWindowLabel(fileNames.join("; "));
			t->setCaptionPolicy(MyWidget::Name);
			emit modifiedProject(t);
		}
	}
}

void ApplicationWindow::open()
{
	QString filter = tr("QtiPlot project") + " (*.qti);;";
	filter += tr("Compressed QtiPlot project") + " (*.qti.gz);;";
	filter += tr("Origin project") + " (*.opj *.OPJ);;";
	filter += tr("All files") + " (*);;";

	QString fn = QFileDialog::getOpenFileName(workingDir, filter, this, 0,
			tr("QtiPlot - Open Project"), 0, true);
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

		if (fn.contains(".qti",true) || fn.contains(".opj",false))
		{
			QFileInfo f(fn);
			if (!f.exists ())
			{
				QMessageBox::critical(this, tr("QtiPlot - File openning error"),
						tr("The file: <b>%1</b> doesn't exist!").arg(fn));
				return;
			}

			saveSettings();//the recent projects must be saved

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
	f.open(QIODevice::ReadOnly);
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
			QMessageBox::critical(this, tr("QtiPlot - File opening error"),  tr("The file: <b> %1 </b> was not created using QtiPlot!").arg(fn));
		return 0;
	}

	QStringList vl = QStringList::split (".", version, false);
	fileVersion = 100*(vl[0]).toInt()+10*(vl[1]).toInt()+(vl[2]).toInt();

	ApplicationWindow* app = openProject(fname);

	f.close();
	return app;
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
		saveSettings();//the recent projects must be saved 

		ApplicationWindow * a = open (fn);
		if (a)
		{
			a->updatePlotsTransparency();
			this->close();
		}
	}
}

ApplicationWindow* ApplicationWindow::openProject(const QString& fn)
{
	ApplicationWindow *app = new ApplicationWindow();
	app->applyUserSettings();
	app->projectname = fn;
	app->fileVersion = fileVersion;
	app->setWindowTitle(tr("QtiPlot") + " - " + fn);
	app->showMaximized();

	QFile f(fn);
	QTextStream t( &f );
	t.setEncoding(QTextStream::UnicodeUTF8);
	f.open(QIODevice::ReadOnly);

	QFileInfo fi(fn);
	QString baseName = fi.fileName();

	t.readLine(); 
	if (fileVersion < 73)
		t.readLine();

	QString s = t.readLine();
	QStringList list=QStringList::split("\t",s,false);
	if (list[0] == "<scripting-lang>")
	{
		if (!app->setScriptingLang(list[1], true))
			QMessageBox::warning(app, tr("QtiPlot - File opening error"),
					tr("The file \"%1\" was created using \"%2\" as scripting language.\n\n"\
						"Initializing support for this language FAILED; I'm using \"%3\" instead.\n"\
						"Various parts of this file may not be displayed as expected.")\
					.arg(fn).arg(list[1]).arg(scriptEnv->name()));

		s = t.readLine();
		list=QStringList::split("\t",s,false);
	}
	int aux=0,widgets=list[1].toInt();

	QString titleBase = tr("Window") + ": ";
	QString title = titleBase + "1/"+QString::number(widgets)+"  ";

	Q3ProgressDialog progress(0, "progress", true, Qt::WindowStaysOnTopHint);
	progress.setMinimumWidth(app->width()/2);
	progress.setWindowTitle(tr("QtiPlot - Opening file") + ": " + baseName);
	progress.setLabelText(title);
	progress.setTotalSteps(widgets);
	progress.setActiveWindow();
	//progress.setMinimumDuration(10000);
	//progress.move(0,0);

	Folder *cf = app->projectFolder();
	app->folders->blockSignals (true);
	app->blockSignals (true);
	//rename project folder item
	FolderListItem *item = (FolderListItem *)app->folders->firstChild();
	item->setText(0, fi.baseName());
	item->folder()->setFolderName(fi.baseName());

	//process tables and matrix information
	while ( !t.atEnd() && !progress.wasCanceled())
	{
		s = t.readLine();
		list.clear();
		if  (s.left(8) == "<folder>")
		{
			list = QStringList::split ("\t",s,true);
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
			QStringList lst;
			while ( s!="</table>" )
			{
				s=t.readLine();
				lst<<s;
			}
			lst.pop_back();
			openTable(app,lst);
			progress.setProgress(aux);
		}
		else if (s.left(17)=="<TableStatistics>")
		{
			QStringList lst;
			while ( s!="</TableStatistics>" )
			{
				s=t.readLine();
				lst<<s;
			}
			lst.pop_back();
			app->openTableStatistics(lst);
		}
		else if  (s == "<matrix>")
		{
			title= titleBase + QString::number(++aux)+"/"+QString::number(widgets);
			progress.setLabelText(title);
			QStringList lst;
			while ( s != "</matrix>" )
			{
				s=t.readLine();
				lst<<s;
			}
			lst.pop_back();
			openMatrix(app, lst);
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
			QStringList cont;
			while ( s != "</note>" )
			{
				s=t.readLine();
				cont << s;
			}
			cont.pop_back();
			m->restore(cont);
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
	f.open(QIODevice::ReadOnly);

	MultiLayer *plot=0;
	while ( !t.atEnd() && !progress.wasCanceled())
	{
		s=t.readLine();
		if  (s.left(8) == "<folder>")
		{
			list = QStringList::split ("\t",s,true);
			app->current_folder = app->current_folder->findSubfolder(list[1]);
		}
		else if  (s == "<multiLayer>")
		{//process multilayers information
			title = titleBase + QString::number(++aux)+"/"+QString::number(widgets);
			progress.setLabelText(title);

			s=t.readLine();
			QStringList graph=QStringList::split ("\t",s,true);
			QString caption=graph[0];
			plot=app->multilayerPlot(caption);
			plot->setCols(graph[1].toInt());
			plot->setRows(graph[2].toInt());
			
			app->setListViewDate(caption, graph[3]);
			plot->setBirthDate(graph[3]);

			restoreWindowGeometry(app, plot, t.readLine());

			plot->blockSignals(true);	

			if (fileVersion > 71)
			{
				QStringList lst=QStringList::split ("\t", t.readLine(), true);
				plot->setWindowLabel(lst[1]);
				app->setListViewLabel(plot->name(),lst[1]);
				plot->setCaptionPolicy((MyWidget::CaptionPolicy)lst[2].toInt());
			}

			if (caption.contains ("graph",true))
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

	app->logInfo=app->logInfo.remove ("</log>\n", false);

	QFileInfo fi2(f);
	QString fileName = fi2.absFilePath();

	app->recentProjects.remove(fileName);
	app->recentProjects.push_front(fileName);
	app->updateRecentProjectsList();

	if (app->aw)
	{
		app->aw->setFocus();
		if (app->aw->status() == MyWidget::Maximized)
		{
			app->aw->hide();
			app->aw->showMaximized();
		}

		app->customMenu(app->aw);
		app->customToolBars(app->aw);
	}

	app->savedProject();

	if (app->show_windows_policy == HideAll)
		app->hideFolderWindows(app->projectFolder());

	app->folders->setCurrentItem(cf->folderListItem());
	app->folders->blockSignals (false);
	//change folder to user defined current folder
	app->changeFolder(cf);
	app->blockSignals (false);
	app->renamedTables.clear();

	app->executeNotes();
	return app;
}

void ApplicationWindow::executeNotes()
{
	QList<MyWidget *> lst = projectFolder()->windowsList();
	foreach(MyWidget *widget, lst)
		if (widget->isA("Note") && ((Note*)widget)->autoexec())
			((Note*)widget)->executeAll();
}

void ApplicationWindow::scriptError(const QString &message, const QString &scriptName, int lineNumber)
{
	QMessageBox::critical(this, "QtiPlot - Script Error", message);
}

void ApplicationWindow::scriptPrint(const QString &text)
{
#ifdef SCRIPTING_CONSOLE
	if(!text.stripWhiteSpace().isEmpty()) console->append(text);
#else
	printf(text.ascii());
#endif
}

bool ApplicationWindow::setScriptingLang(const QString &lang, bool force)
{
	if (!force && lang == scriptEnv->name()) return true;
	if (lang.isEmpty()) return false;

	ScriptingEnv *newEnv = ScriptingLangManager::newEnv(lang, this);
	if (newEnv)
	{
		connect(newEnv, SIGNAL(error(const QString&,const QString&,int)),
				this, SLOT(scriptError(const QString&,const QString&,int)));
		connect(newEnv, SIGNAL(print(const QString&)), this, SLOT(scriptPrint(const QString&)));
		if (!newEnv->initialize())
		{
			delete newEnv;
			return false;
		}
		ScriptingChangeEvent *sce = new ScriptingChangeEvent(newEnv);
		QApplication::sendEvent(this, sce);
		delete sce;
		foreach(QObject *i, queryList())
			QApplication::postEvent(i, new ScriptingChangeEvent(newEnv));
		return true;
	}
	return false;
}

void ApplicationWindow::showScriptingLangDialog()
{
	ScriptingLangDialog* d = new ScriptingLangDialog(scriptEnv,this,"scriptingLangDialog",true);
	d->showNormal();
	d->setActiveWindow();
}

void ApplicationWindow::restartScriptingEnv()
{
	if (setScriptingLang(scriptEnv->name(), true))
		executeNotes();
	else
		QMessageBox::critical(this, tr("QtiPlot - Scripting Error"),
				tr("Scripting language \"%1\" failed to initialize.").arg(scriptEnv->name()));
}

void ApplicationWindow::openTemplate()
{
	QString filter = "QtiPlot 2D Graph Template (*.qpt);;";
	filter += "QtiPlot 3D Surface Template (*.qst);;";
	filter += "QtiPlot Table Template (*.qtt);;";
	filter += "QtiPlot Matrix Template (*.qmt);;";

	QString fn = Q3FileDialog::getOpenFileName(workingDir, filter, this, 0,
			tr("QtiPlot - Open Template File"), 0, true);
	if (!fn.isEmpty())
	{
		QFileInfo fi(fn);
		workingDir = fi.dirPath(true); 		
		if (fn.contains(".qmt",true) || fn.contains(".qpt",true) ||
				fn.contains(".qtt",true) || fn.contains(".qst",true))
		{
			if (!fi.exists())
			{
				QMessageBox::critical(this, tr("QtiPlot - File opening error"),
						tr("The file: <b>%1</b> doesn't exist!").arg(fn));
				return;
			}
			QFile f(fn);
			QTextStream t(&f);
			t.setEncoding(QTextStream::UnicodeUTF8);
			f.open(QIODevice::ReadOnly);
			QStringList l=QStringList::split(QRegExp("\\s"), t.readLine(), false);
			QString fileType=l[0];
			if (fileType != "QtiPlot")
			{
				QMessageBox::critical(this,tr("QtiPlot - File opening error"),
						tr("The file: <b> %1 </b> was not created using QtiPlot!").arg(fn));
				return;
			}
			QStringList vl = QStringList::split (".", l[1], false);
			fileVersion = 100*(vl[0]).toInt()+10*(vl[1]).toInt()+(vl[2]).toInt();

			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			MyWidget *w = 0;
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
						w = newMatrix(tr("matrix1"), rows, cols);
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
			QMessageBox::critical(this,tr("QtiPlot - File opening error"),
					tr("The file: <b>%1</b> is not a QtiPlot template file!").arg(fn));
			return;
		}
	}
}

void ApplicationWindow::updatePlotsTransparency()
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	QWidgetList *windows = windowsList();
	foreach(QWidget *w, *windows)
	{
		if (w->isA("MultiLayer") && ((MultiLayer*)w)->hasOverlapingLayers())
			((MultiLayer*)w)->updateTransparency();
	}
	delete windows;
	QApplication::restoreOverrideCursor();
}

void ApplicationWindow::readSettings()
{
	helpFilePath="/usr/share/doc/qtiplot/index.html";
#ifdef Q_OS_WIN // Windows systems
	helpFilePath=qApp->applicationDirPath()+"/index.html";
#endif

#ifdef Q_OS_MAC // Mac 
	QSettings settings(QSettings::IniFormat,QSettings::UserScope, "Ion Vasilief", "QtiPlot");
#else
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope, "Ion Vasilief", "QtiPlot");
#endif

	settings.beginGroup("/QtiPlot");
	autoSearchUpdates = settings.value("/autoSearchUpdates", QVariant(true)).toBool();
	askForSupport = settings.value("/askForSupport", QVariant(true)).toBool();
	appLanguage = settings.value("/appLanguage", "en").toString();
	workingDir=settings.value("/workingDir", qApp->applicationDirPath()).toString();
	helpFilePath=settings.value("/helpFilePath", helpFilePath).toString();
	show_windows_policy = (ShowWindowsPolicy)settings.value("/ShowWindowsPolicy", ApplicationWindow::ActiveFolder).toInt();

	recentProjects=variantListToStringList(settings.value("/recentProjects").toList());
	updateRecentProjectsList();

	fitFunctions=variantListToStringList(settings.value("/fitFunctions").toList());
	surfaceFunc=variantListToStringList(settings.value("/surfaceFunctions").toList());
	xFunctions=variantListToStringList(settings.value("/xFunctions").toList());
	yFunctions=variantListToStringList(settings.value("/yFunctions").toList());
	rFunctions=variantListToStringList(settings.value("/rFunctions").toList());
	tetaFunctions=variantListToStringList(settings.value("/tetaFunctions").toList());

	QStringList tableColors=variantListToStringList(settings.value("/tableColors").toList());
	QStringList tableFonts=variantListToStringList(settings.value("/tableFonts").toList());

	//2D plots settings
	titleOn=settings.value("/titleOn", true).toBool();
	allAxesOn=settings.value("/allAxesOn", false).toBool();
	canvasFrameOn=settings.value("/canvasFrameOn", false).toBool();
	canvasFrameWidth=settings.value("/canvasFrameWidth").toInt();
	defaultPlotMargin=settings.value("/defaultPlotMargin").toInt();
	drawBackbones=settings.value("/drawBackbones", true).toBool();
	axesLineWidth=settings.value("/axesLineWidth", 1).toInt();
	autoscale2DPlots = settings.value("/autoscale2DPlots", true).toBool();
	autoScaleFonts = settings.value("/autoScaleFonts", true).toBool();
	autoResizeLayers = settings.value("/autoResizeLayers", true).toBool();

	//2D curves settings
	defaultCurveStyle = settings.value("/defaultCurveStyle", Graph::LineSymbols).toInt();
	defaultCurveLineWidth = settings.value("/defaultCurveLineWidth", 1).toInt();
	defaultSymbolSize = settings.value("/defaultSymbolSize", 7).toInt();

	majTicksStyle=settings.value("/majTicksStyle", ScaleDraw::Out).toInt();
	minTicksStyle=settings.value("/minTicksStyle", ScaleDraw::Out).toInt();
	minTicksLength=settings.value("/minTicksLength", 5).toInt();
	majTicksLength=settings.value("/majTicksLength", 9).toInt();

	legendFrameStyle=settings.value("/legendFrameStyle", LegendMarker::Line).toInt();
	legendTextColor = QColor(settings.value("/legendTextColor", "#000000").toString());//default color Qt::black
	legendBackground = QColor(settings.value("/legendBackground", "#ffffff").toString());//default color Qt::white

	defaultArrowLineWidth = settings.value("/defaultArrowLineWidth", 1).toInt();
	defaultArrowColor = QColor(settings.value("/defaultArrowColor", "#000000").toString());//default color Qt::black
	defaultArrowHeadLength = settings.value("/defaultArrowHeadLength", 4).toInt();
	defaultArrowHeadAngle = settings.value("/defaultArrowHeadAngle", 45).toInt();
	defaultArrowHeadFill = settings.value("/defaultArrowHeadFill", true).toBool();
	defaultArrowLineStyle = Graph::getPenStyle(settings.value("/defaultArrowLineStyle", "SolidLine").toString());

	QStringList graphFonts=variantListToStringList(settings.value("/graphFonts").toList());
	confirmCloseFolder=settings.value("/confirmCloseFolder", true).toBool();
	confirmCloseTable=settings.value("/confirmCloseTable", true).toBool();
	confirmCloseMatrix=settings.value("/confirmCloseMatrix", true).toBool();
	confirmClosePlot2D=settings.value("/confirmClosePlot2D", true).toBool();
	confirmClosePlot3D=settings.value("/confirmClosePlot3D", true).toBool();
	confirmCloseNotes=settings.value("/confirmCloseNotes", true).toBool();

	QStringList applicationFont=variantListToStringList(settings.value("/appFont").toList());

	//set user style
	changeAppStyle(settings.value("/appStyle", appStyle).toString());

	autoSave=settings.value("/autoSave",true).toBool();
	autoSaveTime=settings.value("/autoSaveTime",15).toInt();
	defaultScriptingLang=settings.value("/ScriptingLang","muParser").toString();
	QStringList appColors=variantListToStringList(settings.value("/appColors").toList());

	//3D plots settings
	showPlot3DLegend=settings.value("/showPlot3DLegend",true).toBool();
	showPlot3DProjection=settings.value("/showPlot3DProjection", false).toBool();
	smooth3DMesh = settings.value("/smooth3DMesh", true).toBool();
	plot3DResolution=settings.value("/plot3DResolution", 1).toInt();
    orthogonal3DPlots =settings.value("/Orthogonal", false).toBool();
    
	QStringList aux =variantListToStringList( settings.value("/plot3DColors").toList());
	QStringList plot3DFonts =variantListToStringList( settings.value("/plot3DFonts").toList());

	fitPluginsPath = settings.value("/fitPluginsPath", "fitPlugins").toString();

	if (aux.size() == 8)
		plot3DColors = aux;
	else
	{
		plot3DColors << QColor(Qt::blue).name();
		plot3DColors << QColor(Qt::black).name() << QColor(Qt::black).name() << QColor(Qt::black).name();
		plot3DColors << QColor(Qt::red).name() << QColor(Qt::black).name() << QColor(Qt::black).name();
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
		workspaceColor=QColor(Qt::darkGray);
		panelsColor=QColor(255, 255, 255);
		panelsTextColor=QColor(Qt::black);
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
		tableTextColor=QColor(Qt::black);
		tableHeaderColor=QColor(Qt::black);
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

	//restore dock windows and tool bars
	QByteArray ba = settings.value("/DockWindows").toByteArray();
	restoreState(ba);

	ba = settings.value("/ExplorerSplitter").toByteArray();
	explorerSplitter->restoreState(ba);

	settings.endGroup();

	settings.beginGroup("/Fitting");
	fit_output_precision = settings.value("/fit_output_precision", 15).toInt();
	pasteFitResultsToPlot = settings.value("/pasteFitResultsToPlot", false).toBool();
	writeFitResultsToLog = settings.value("/writeFitResultsToLog", true).toBool();
	generateUniformFitPoints = settings.value("/generateUniformFitPoints", true).toBool();
	fitPoints = settings.value("/fitPoints", 100).toInt();
	generatePeakCurves = settings.value("/generatePeakCurves", true).toBool();
	peakCurvesColor = settings.value("/peakCurvesColor", 2).toInt();//green color
	settings.endGroup();

	settings.beginGroup("/ImportASCII");
	columnSeparator = settings.value("/defaultColumnSeparator", "\t").toString();
	ignoredLines = settings.value("/ignoredLines", 0).toInt();
	renameColumns = settings.value("/renameColumns", true).toBool();
	strip_spaces = settings.value("/stripSpaces", false).toBool();
	simplify_spaces = settings.value("/simplifySpaces", false).toBool();
	settings.endGroup();

}

QStringList ApplicationWindow::variantListToStringList(const QList<QVariant> src)
{
        QStringList dest;
        for(int i=0; i<src.size(); i++)
                dest.append(src[i].toString());
         // remark: copying a QList is fast because of implicit sharing, no need for pointers here
	return dest;
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

#ifdef Q_OS_MAC // Mac 
	QSettings settings(QSettings::IniFormat,QSettings::UserScope, "Ion Vasilief", "QtiPlot");
#else
	QSettings settings(QSettings::NativeFormat,QSettings::UserScope, "Ion Vasilief", "QtiPlot");
#endif

	settings.beginGroup("/QtiPlot");
	settings.setValue("/autoSearchUpdates", autoSearchUpdates);
	settings.setValue("/askForSupport", askForSupport);
	settings.setValue("/appLanguage", appLanguage);
	settings.setValue("/workingDir", workingDir);
	settings.setValue("/helpFilePath", helpFilePath);
	settings.setValue("/ShowWindowsPolicy", show_windows_policy);
	settings.setValue("/recentProjects", recentProjects);
	settings.setValue("/fitFunctions", fitFunctions);
	settings.setValue("/surfaceFunctions", surfaceFunc);
	settings.setValue("/xFunctions", xFunctions);
	settings.setValue("/yFunctions", yFunctions);
	settings.setValue("/rFunctions", rFunctions);
	settings.setValue("/tetaFunctions", tetaFunctions);
	settings.setValue("/tableColors", tableColors);
	settings.setValue("/tableFonts", tableFonts);
	settings.setValue("/titleOn", titleOn);
	settings.setValue("/allAxesOn", allAxesOn);
	settings.setValue("/canvasFrameOn", canvasFrameOn);
	settings.setValue("/canvasFrameWidth", canvasFrameWidth);
	settings.setValue("/defaultPlotMargin", defaultPlotMargin);
	settings.setValue("/drawBackbones", drawBackbones);
	settings.setValue("/axesLineWidth", axesLineWidth);
	settings.setValue("/autoscale2DPlots", autoscale2DPlots);
	settings.setValue("/autoScaleFonts", autoScaleFonts);
	settings.setValue("/autoResizeLayers", autoResizeLayers);

	settings.setValue("/defaultCurveStyle", defaultCurveStyle);
	settings.setValue("/defaultCurveLineWidth", defaultCurveLineWidth);
	settings.setValue("/defaultSymbolSize", defaultSymbolSize);

	//ticks
	settings.setValue("/majTicksStyle", majTicksStyle);
	settings.setValue("/minTicksStyle", minTicksStyle);
	settings.setValue("/minTicksLength", minTicksLength);
	settings.setValue("/majTicksLength", majTicksLength);

	settings.setValue("/legendFrameStyle", legendFrameStyle);
	settings.setValue("/legendTextColor", legendTextColor.name());
	settings.setValue("/legendBackground", legendBackground.name());

	//line/arrow default settings 
	settings.setValue("/defaultArrowLineWidth", defaultArrowLineWidth);
	settings.setValue("/defaultArrowColor", defaultArrowColor.name());
	settings.setValue("/defaultArrowHeadLength", defaultArrowHeadLength);
	settings.setValue("/defaultArrowHeadAngle", defaultArrowHeadAngle);
	settings.setValue("/defaultArrowHeadFill", defaultArrowHeadFill);
	settings.setValue("/defaultArrowLineStyle", Graph::penStyleName(defaultArrowLineStyle));
	settings.setValue("/graphFonts", graphFonts);
	settings.setValue("/confirmCloseFolder", confirmCloseFolder);
	settings.setValue("/confirmCloseTable", confirmCloseTable);
	settings.setValue("/confirmCloseMatrix", confirmCloseMatrix);
	settings.setValue("/confirmClosePlot2D", confirmClosePlot2D);
	settings.setValue("/confirmClosePlot3D", confirmClosePlot3D);
	settings.setValue("/confirmCloseNotes", confirmCloseNotes);
	settings.setValue("/appFont", applicationFont);
	settings.setValue("/appStyle", appStyle);
	settings.setValue("/autoSave", autoSave);
	settings.setValue("/autoSaveTime", autoSaveTime);
	settings.setValue("/ScriptingLang",defaultScriptingLang);
	settings.setValue("/appColors", appColors);

	settings.setValue("/showPlot3DLegend", showPlot3DLegend);
	settings.setValue("/showPlot3DProjection", showPlot3DProjection);
	settings.setValue("/smooth3DMesh", smooth3DMesh);
	settings.setValue("/plot3DResolution", plot3DResolution);
	settings.setValue("/Orthogonal", orthogonal3DPlots);
	
	settings.setValue("/plot3DColors", plot3DColors);
	settings.setValue("/plot3DFonts", plot3DFonts);
	settings.setValue("/fitPluginsPath", fitPluginsPath);

	// Save the state of all dock windows
	QByteArray ba = saveState();
	settings.setValue("/DockWindows", ba);

	ba = explorerSplitter->saveState();
	settings.setValue("/ExplorerSplitter", ba);

	settings.endGroup();

	settings.beginGroup("/Fitting");
	settings.setValue("/fit_output_precision", fit_output_precision);
	settings.setValue("/pasteFitResultsToPlot", pasteFitResultsToPlot);
	settings.setValue("/writeFitResultsToLog", writeFitResultsToLog);
	settings.setValue("/generateUniformFitPoints", generateUniformFitPoints);
	settings.setValue("/generatePeakCurves", generatePeakCurves);
	settings.setValue("/peakCurvesColor", peakCurvesColor);
	settings.setValue("/fitPoints", fitPoints);
	settings.endGroup();

	settings.beginGroup("/ImportASCII");
	settings.setValue("/defaultColumnSeparator", columnSeparator);
	settings.setValue("/ignoredLines", ignoredLines);
	settings.setValue("/renameColumns", renameColumns);
	settings.setValue("/stripSpaces", strip_spaces);
	settings.setValue("/simplifySpaces", simplify_spaces);
	settings.endGroup();

}

void ApplicationWindow::exportGraph()
{
	QWidget *w=ws->activeWindow();
	if (!w)
		return;

	if(w->isA("MultiLayer"))
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
			workingDir = ied->directory().path();
			QString fname = ied->selectedFile(); 
			QString selectedFilter = ied->selectedFilter();

			QFileInfo fi(fname);
			QString baseName = fi.fileName();

			if (baseName.contains(".")==0)
				fname.append(selectedFilter.remove("*"));		

			if ( QFile::exists(fname) &&
					QMessageBox::question(0, tr("QtiPlot - Overwrite file?"),
						tr("A file called: <p><b>%1</b><p>already exists. "
							"Do you want to overwrite it?")
						.arg(fname),
						tr("&Yes"), tr("&No"),
						QString(), 0, 1 ) )
				return ;
			else
			{
				QFile f(fname);
				if ( !f.open( QIODevice::WriteOnly ) ) 
				{
					QMessageBox::critical(0, tr("QtiPlot - Export error"),
							tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that you have the right to write to this location!").arg(fname));
					return;
				}

				if (selectedFilter.contains(".svg"))
				{
					plot->exportToSVG(fname);
					return;
				}
				else if (selectedFilter.contains(".eps"))
				{
					if (ied->showExportOptions())
					{
						EpsExportDialog *ed= new EpsExportDialog (fname, this, 0);
						ed->setAttribute(Qt::WA_DeleteOnClose);
						connect (ed, SIGNAL(exportToEPS(const QString&, int, QPrinter::Orientation, QPrinter::PageSize, QPrinter::ColorMode)), 
								plot, SLOT(exportToEPS(const QString&, int, QPrinter::Orientation, QPrinter::PageSize, QPrinter::ColorMode)));

						ed->exec();
					}
					else
						plot->exportToEPS(fname);
					return;
				}

				QList<QByteArray> list=QImageWriter::supportedImageFormats ();
				for (int i=0; i<(int)list.count(); i++)
				{
					if (selectedFilter.contains("."+(list[i]).lower()))
					{
						if (ied->showExportOptions())
						{
							ImageExportOptionsDialog* ed= new ImageExportOptionsDialog(false, this);
							ed->setAttribute(Qt::WA_DeleteOnClose);
							connect (ed, SIGNAL(options(const QString&, const QString&, int, bool)), 
									plot, SLOT(exportImage(const QString&, const QString&, int, bool)));

							ed->setExportPath(fname, list[i]);
							ed->enableTransparency();
							ed->exec();
						}
						else
							plot->exportImage(fname, list[i], 100, true);
						return;
					}
				}
			}		
		}
	}

	else if(w->isA("Graph3D"))
		((Graph3D*)w)->saveImage();
}

void ApplicationWindow::exportLayer()
{
	QWidget *w=ws->activeWindow();
	if (!w || !w->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)w)->activeGraph();
	if (!g)
		return;

	ImageExportDialog *ied = new ImageExportDialog(this, 0);
	ied->setDir(workingDir);
	if ( ied->exec() == QDialog::Accepted )
	{
		workingDir = ied->directory().path();
		QString fname = ied->selectedFile(); 
		QString selectedFilter = ied->selectedFilter();

		QFileInfo fi(fname);
		QString baseName = fi.fileName();

		if (baseName.contains(".")==0)
			fname.append(selectedFilter.remove("*"));		

		if ( QFile::exists(fname) &&
				QMessageBox::question(0, tr("QtiPlot - Overwrite file?"),
					tr("A file called: <p><b>%1</b><p>already exists. "
						"Do you want to overwrite it?")
					.arg(fname),
					tr("&Yes"), tr("&No"),
					QString(), 0, 1 ) )
			return ;
		else
		{
			QFile f(fname);
			if ( !f.open( QIODevice::WriteOnly ) ) 
			{
				QMessageBox::critical(0, tr("QtiPlot - Export error"),
						tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that you have the right to write to this location!").arg(fname));
				return;
			}

			if (selectedFilter.contains(".eps"))
			{
				if (ied->showExportOptions())
				{
					EpsExportDialog *ed= new EpsExportDialog (fname, this, 0);
					ed->setAttribute(Qt::WA_DeleteOnClose);
					connect (ed, SIGNAL(exportToEPS(const QString&, int, QPrinter::Orientation, QPrinter::PageSize, QPrinter::ColorMode)), 
							g, SLOT(exportToEPS(const QString&, int, QPrinter::Orientation, QPrinter::PageSize, QPrinter::ColorMode)));

					ed->exec();
				}
				else
					g->exportToEPS(fname);

				if (((MultiLayer*)w)->hasOverlapingLayers())
					((MultiLayer*)w)->updateTransparency();
				return;
			}
			else if (selectedFilter.contains(".svg"))
			{
				g->exportToSVG(fname);
				return;
			}
			/*else if (selectedFilter.contains(".wmf"))
			  {
			  g->exportToWmf(fname);
			  return;
			  }*/
			QList<QByteArray> list = QImageWriter::supportedImageFormats();
			for (int i=0; i<(int)list.count(); i++)
			{
				if (selectedFilter.contains("."+(list[i]).lower()))
				{
					if (ied->showExportOptions())
					{
						ImageExportOptionsDialog* ed= new ImageExportOptionsDialog(false, this);
						ed->setAttribute(Qt::WA_DeleteOnClose);
						connect (ed, SIGNAL(options(const QString&, const QString&, int, bool)), 
								g, SLOT(exportImage(const QString&, const QString&, int, bool)));

						ed->setExportPath(fname, list[i]);
						ed->enableTransparency();
						ed->exec();
					}
					else
						g->exportImage(fname, list[i], 100, true);

					if (((MultiLayer*)w)->hasOverlapingLayers())
						((MultiLayer*)w)->updateTransparency();
					return;
				}
			}
		}		
	}
}

void ApplicationWindow::exportAllGraphs()
{
	QString dir = Q3FileDialog::getExistingDirectory(workingDir, this, "get existing directory", 
			"Choose a directory to export the graphs to", true, true);
	if (!dir.isEmpty())
	{
		ImageExportOptionsDialog* ed= new ImageExportOptionsDialog(true, this);
		ed->setAttribute(Qt::WA_DeleteOnClose);
		connect (ed, SIGNAL(exportAll(const QString&, const QString&, int, bool)),
				this, SLOT(exportAllGraphs(const QString&, const QString&, int, bool)));

		workingDir = dir;
		ed->setExportDirPath(dir);
		ed->enableTransparency();
		ed->exec();
	}
}

void ApplicationWindow::exportAllGraphs(const QString& dir, const QString& format, 
		int quality, bool transparency)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QWidgetList *windows = windowsList();
	QString fileType = format;
	fileType.lower();
	fileType.prepend(".");

	bool confirmOverwrite = true;	

	foreach (QWidget *w, *windows)
	{
		MultiLayer *plot = 0;
		Graph3D *splot =0;

		if (w->isA("MultiLayer"))
			plot = (MultiLayer*)w;
		else if (w->isA("Graph3D"))
			splot = (Graph3D*)w;

		if (plot || splot)
		{
			QString fileName = dir + "/" + w->name() + fileType;
			QFile f(fileName);
			if (f.exists(fileName) && confirmOverwrite)
			{
				QApplication::restoreOverrideCursor();
				switch(QMessageBox::question(0, tr("QtiPlot - Overwrite file?"),
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

	delete windows;
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
					QString(plot->name()) + "</b>.<br>Graph window not exported!"));
		return;
	}

	QFile f(fileName);
	if ( !f.open( QIODevice::WriteOnly ) ) 
	{
		QApplication::restoreOverrideCursor();
		QMessageBox::critical(0, tr("QtiPlot - Export error"), 
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
	if ( !f.open( QIODevice::WriteOnly ) ) 
	{
		QMessageBox::critical(0, tr("QtiPlot - Export error"), tr("Could not write to file: <br><h4>%1</h4><p>Please verify that you have the right to write to this location!").arg(fileName));

		QApplication::restoreOverrideCursor();
		return;
	}

	plot->saveImageToFile(fileName, format);	
}

QString ApplicationWindow::windowGeometryInfo(QWidget *w)
{
	QString s = "geometry\t";

	if (((MyWidget *)w)->status() == MyWidget::Minimized)
		s+="minimized\n";
	else if (((MyWidget *)w)->status() == MyWidget::Maximized)
	{
		if (w == ws->activeWindow())
			s+="maximized\tactive\n";
		else
			s+="maximized\n";
	}
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

	setWindowTitle("QtiPlot - "+projectname);
	savedProject();
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
	QString fn = Q3FileDialog::getSaveFileName(workingDir, filter, this, "project",
			tr("Save Project As"), &selectedFilter, false);
	if ( !fn.isEmpty() )
	{
		QFileInfo fi(fn);
		workingDir = fi.dirPath(true);
		QString baseName = fi.fileName();	
		if (!baseName.contains("."))
			fn.append(".qti");

		if ( QFile::exists(fn) && !selectedFilter.contains(".gz") &&
				QMessageBox::question(this, tr("QtiPlot - Overwrite file? "),
					tr("A file called: <p><b>%1</b><p>already exists.\n"
						"Do you want to overwrite it?")
					.arg(fn), tr("&Yes"), tr("&No"),QString(), 0, 1 ) )
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

void ApplicationWindow::saveNoteAs()
{
	Note *w = (Note*)ws->activeWindow();
	if (!w || !w->inherits("Note"))
		return;
	w->exportASCII();
}

void ApplicationWindow::saveAsTemplate()
{
	MyWidget* w = (MyWidget*)ws->activeWindow();
	if (!w)
		return;

	QString filter;
	if (w->isA("Matrix"))
		filter = tr("QtiPlot Matrix Template")+" (*.qmt)";
	else if (w->isA("MultiLayer"))
		filter = tr("QtiPlot 2D Graph Template")+" (*.qpt)";
	else if (w->isA("Table"))
		filter = tr("QtiPlot Table Template")+" (*.qtt)";
	else if (w->isA("Graph3D"))
		filter = tr("QtiPlot 3D Surface Template")+" (*.qst)";

	QString selectedFilter;
	QString fn = Q3FileDialog::getSaveFileName(workingDir, filter, this, "template",
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
				QMessageBox::question(this, tr("QtiPlot -- Overwrite file? "),
					tr("A file called: <p><b>%1</b><p>already exists.\n"
						"Do you want to overwrite it?")
					.arg(fn), tr("&Yes"), tr("&No"),QString(), 0, 1 ) )
			return ;
		else
		{
			QFile f(fn);
			if ( !f.open( QIODevice::WriteOnly ) )
			{
				QMessageBox::critical(0, tr("QtiPlot - Export error"),
						tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that you have the right to write to this location!").arg(fn));
				return;
			}
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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
	MyWidget* m = (MyWidget*)ws->activeWindow();
	if (!m)
		return;

	RenameWindowDialog *rwd = new RenameWindowDialog(this);
	rwd->setAttribute(Qt::WA_DeleteOnClose);
	rwd->setWidget(m);
	rwd->exec();
}

void ApplicationWindow::renameWindow()
{
	WindowListItem *it = (WindowListItem *)lv->currentItem();
	MyWidget *w= it->window();
	if (!w)
		return;

	RenameWindowDialog *rwd = new RenameWindowDialog(this,0);
	rwd->setAttribute(Qt::WA_DeleteOnClose);
	rwd->setWidget(w);
	rwd->exec();
}

void ApplicationWindow::renameWindow(Q3ListViewItem *item, int, const QString &text)
{
	if (!item)
		return;

	MyWidget *w = ((WindowListItem *)item)->window();
	if (!w || text == w->name())
		return;

	while(!renameWindow(w, text))
	{
		item->setRenameEnabled (0, true);
		item->startRename (0);
		return;
	}
}

bool ApplicationWindow::renameWindow(MyWidget *w, const QString &text)
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

	if (w->inherits("Table"))
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

	w->setName(text);
	w->setCaptionPolicy(w->captionPolicy());
	return true;
}

QStringList ApplicationWindow::columnsList(Table::PlotDesignation plotType)
{
	QList<QWidget*> *windows = windowsList();
	QStringList list;
	for (int i=0;i<(int)windows->count();i++)
	{
		if (!windows->at(i)->inherits("Table"))
			continue;

		Table *t = (Table *)windows->at(i);
		for (int j=0; j < t->tableCols(); j++)
		{
			if (t->colPlotDesignation(j) == plotType)
				list << QString(t->name()) + "_" + t->colLabel(j);
		}
	}
	delete windows;
	return list;
}

void ApplicationWindow::showCurvesDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	if (((MultiLayer*)ws->activeWindow())->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Error"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		return;
	}

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (!g)
		return;

	if (g->isPiePlot())
	{
		QMessageBox::warning(this,tr("QtiPlot - Error"),
				tr("This functionality is not available for pie plots!"));
	}
	else
	{
		activeGraph = g;

		CurvesDialog* crvDialog=new CurvesDialog(this,"curves",true,Qt::WindowStaysOnTopHint);
		crvDialog->setAttribute(Qt::WA_DeleteOnClose);
		connect (crvDialog,SIGNAL(showPlotAssociations(int)), this, SLOT(showPlotAssociations(int)));
		connect (crvDialog,SIGNAL(showFunctionDialog(Graph *, int)), this, SLOT(showFunctionDialog(Graph *, int)));

		crvDialog->insertCurvesToDialog(columnsList(Table::Y));
		crvDialog->setCurveDefaultSettings(defaultCurveStyle, defaultCurveLineWidth, defaultSymbolSize);
		crvDialog->setGraph(g);
		crvDialog->initTablesList(tableList());
		crvDialog->exec();
	}
}

QList<QWidget*>* ApplicationWindow::tableList()
{
	QList<QWidget*>* lst = new QList<QWidget*>();
	QList<QWidget*> *windows = windowsList();
	for (int i = 0; i < int(windows->count());i++ )
	{
		if (windows->at(i)->inherits("Table"))
			lst->append(windows->at(i));
	}
	delete windows;
	return lst;
}

void ApplicationWindow::showPlotAssociations(int curve)
{
	if (!activeGraph)
		return;

	AssociationsDialog* ad=new AssociationsDialog(this, "curves", true, Qt::WindowStaysOnTopHint);
	ad->setAttribute(Qt::WA_DeleteOnClose);
	ad->setGraph(activeGraph);
	ad->initTablesList(tableList(), curve);
	ad->exec();
}

void ApplicationWindow::showTitleDialog()
{
	QWidget *w = ws->activeWindow();
	if (!w)
		return;

	if (w->isA("MultiLayer"))
	{
		Graph* g = ((MultiLayer*)w)->activeGraph();
		if (g)
		{
			TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,0);
			td->setAttribute(Qt::WA_DeleteOnClose);
			connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setTitleFont(const QFont &)));
			connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setTitle(const QString &)));
			connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setTitleColor(const QColor &)));
			connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setTitleAlignment(int)));

			QwtText t = g->plotWidget()->title();
			td->setText(t.text());
			td->setFont(t.font());
			td->setTextColor(t.color());
			td->setAlignment(t.renderFlags());
			td->exec();
		}
	}
	else if (w->isA("Graph3D"))
	{
		Plot3DDialog* pd = (Plot3DDialog*)showPlot3dDialog();
		if (pd)
			pd->showTitleTab();
		delete pd;
	}
}

void ApplicationWindow::showXAxisTitleDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (g)
	{
		TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,0);
		td->setAttribute(Qt::WA_DeleteOnClose);
		connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setXAxisTitleFont(const QFont &)));
		connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setXAxisTitle(const QString &)));
		connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setXAxisTitleColor(const QColor &)));
		connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setXAxisTitleAlignment(int)));

		QStringList t=g->scalesTitles();
		td->setText(t[0]);
		td->setFont(g->axisTitleFont(2));
		td->setTextColor(g->axisTitleColor(2));
		td->setAlignment(g->axisTitleAlignment(2));
		td->setWindowTitle(tr("QtiPlot - X Axis Title"));
		td->exec();
	}
}

void ApplicationWindow::showYAxisTitleDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (g)
	{
		TextDialog* td= new TextDialog(TextDialog::AxisTitle, this,0);
		td->setAttribute(Qt::WA_DeleteOnClose);
		connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setYAxisTitleFont(const QFont &)));
		connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setYAxisTitle(const QString &)));
		connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setYAxisTitleColor(const QColor &)));
		connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setYAxisTitleAlignment(int)));

		QStringList t=g->scalesTitles();
		td->setText(t[1]);
		td->setFont(g->axisTitleFont(0));
		td->setTextColor(g->axisTitleColor(0));
		td->setAlignment(g->axisTitleAlignment(0));
		td->setWindowTitle(tr("QtiPlot - Y Axis Title"));
		td->exec();
	}
}

void ApplicationWindow::showRightAxisTitleDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (g)
	{
		TextDialog* td= new TextDialog(TextDialog::AxisTitle, this, 0);
		td->setAttribute(Qt::WA_DeleteOnClose);
		connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setRightAxisTitleFont(const QFont &)));
		connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setRightAxisTitle(const QString &)));
		connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setRightAxisTitleColor(const QColor &)));
		connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setRightAxisTitleAlignment(int)));

		QStringList t=g->scalesTitles();
		td->setText(t[3]);
		td->setFont(g->axisTitleFont(1));
		td->setTextColor(g->axisTitleColor(1));
		td->setAlignment(g->axisTitleAlignment(1));
		td->setWindowTitle(tr("QtiPlot - Right Axis Title"));
		td->exec();
	}
}

void ApplicationWindow::showTopAxisTitleDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (g)
	{
		TextDialog* td= new TextDialog(TextDialog::AxisTitle, this, 0);
		td->setAttribute(Qt::WA_DeleteOnClose);
		connect (td,SIGNAL(changeFont(const QFont &)),g,SLOT(setTopAxisTitleFont(const QFont &)));
		connect (td,SIGNAL(changeText(const QString &)),g,SLOT(setTopAxisTitle(const QString &)));
		connect (td,SIGNAL(changeColor(const QColor &)),g,SLOT(setTopAxisTitleColor(const QColor &)));
		connect (td,SIGNAL(changeAlignment(int)),g,SLOT(setTopAxisTitleAlignment(int)));

		QStringList t=g->scalesTitles();
		td->setText(t[2]);
		td->setFont(g->axisTitleFont(3));
		td->setTextColor(g->axisTitleColor(3));
		td->setAlignment(g->axisTitleAlignment(3));
		td->setWindowTitle(tr("QtiPLot - Top Axis Title"));
		td->exec();
	}
}

void ApplicationWindow::showExportASCIIDialog()
{
	if ( ws->activeWindow() && ws->activeWindow()->inherits("Table"))
	{
		ExportDialog* ed= new ExportDialog(this,Qt::WindowContextHelpButtonHint);
		ed->setAttribute(Qt::WA_DeleteOnClose);
		connect (ed, SIGNAL(exportTable(const QString&, const QString&, bool, bool)), 
				this, SLOT(exportASCII (const QString&, const QString&, bool, bool)));
		connect (ed, SIGNAL(exportAllTables(const QString&, bool, bool)), 
				this, SLOT(exportAllTables(const QString&, bool, bool)));

		ed->setTableNames(tableWindows);
		ed->setActiveTableName(ws->activeWindow()->name());
		ed->setColumnSeparator(columnSeparator);
		ed->exec();
	}
}

void ApplicationWindow::exportAllTables(const QString& sep, bool colNames, bool expSelection)
{
	QString dir = Q3FileDialog::getExistingDirectory(workingDir, this, "get existing directory",
			tr("Choose a directory to export the tables to"), true, true);
	if (!dir.isEmpty())
	{
		QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
		QWidgetList *windows = windowsList();
		workingDir = dir;

		bool confirmOverwrite = true;
		bool success = true;	
		QWidget *w;
		foreach(w, *windows)
		{
			if (w->inherits("Table"))
			{
				Table *t = (Table*)w;		
				QString fileName = dir + "/" + w->name() + ".txt";
				QFile f(fileName);
				if (f.exists(fileName) && confirmOverwrite)
				{
					QApplication::restoreOverrideCursor();
					switch(QMessageBox::question(0, tr("QtiPlot - Overwrite file?"),
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
		delete windows;	
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
	QString fname = Q3FileDialog::getSaveFileName(workingDir,"*.txt;;*.dat;;*.DAT", this,"file dialog",
			tr("Choose a filename to save under"),&selectedFilter,true);

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
					tr("QtiPlot - Overwrite file?"),
					tr("A file called: <p><b>%1</b><p>already exists. "
						"Do you want to overwrite it?")
					.arg(fname),
					tr("&Yes"), tr("&No"),
					QString(), 0, 1 ) )
			return ;
		else
		{	
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			t->exportToASCIIFile(fname, sep, colNames, expSelection);
			QApplication::restoreOverrideCursor();
		}
	}
}

void ApplicationWindow::showRowsDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	bool ok;
	int rows = QInputDialog::getInteger(tr("QtiPlot - Enter rows number"), tr("Rows"), 
			((Table*)ws->activeWindow())->tableRows(), 0, 1000000, 1, &ok, this );
	if ( ok ) 
		((Table*)ws->activeWindow())->resizeRows(rows);
}

void ApplicationWindow::showColsDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	bool ok;
	int cols = QInputDialog::getInteger(tr("QtiPlot - Enter columns number"), tr("Columns"), 
			((Table*)ws->activeWindow())->tableCols(), 0, 1000000, 1, &ok, this );
	if ( ok ) 
		((Table*)ws->activeWindow())->resizeCols(cols);
}

void ApplicationWindow::showColumnValuesDialog()
{
	Table* w = (Table*)ws->activeWindow();
	if ( w && w->isA("Table"))
	{
		if (int(w->selectedColumns().count())>0 || !(w->getSelection().isEmpty()) )
		{
			SetColValuesDialog* vd= new SetColValuesDialog(scriptEnv,this,"valuesDialog",true);
			vd->setAttribute(Qt::WA_DeleteOnClose);
			vd->setTable(w);
			vd->exec();
		}
		else
			QMessageBox::warning(this, tr("QtiPlot - Column selection error"),
					tr("Please select a column first!"));
	}
}

void ApplicationWindow::recalculateTable()
{
	Table* w = (Table*)ws->activeWindow();
	if ( w && w->isA("Table"))
		w->calculate();
}

void ApplicationWindow::sortActiveTable()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("Table"))
		return;

	if (int(((Table*)ws->activeWindow())->selectedColumns().count())>0)
		((Table*)ws->activeWindow())->sortTableDialog();
	else
		QMessageBox::warning(this, "QtiPlot - Column selection error","Please select a column first!");
}

void ApplicationWindow::sortSelection()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("Table"))
		return;

	((Table*)ws->activeWindow())->sortColumnsDialog();
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
	if (!ws->activeWindow() || !ws->activeWindow()->isA("Table"))
		return;

	if (int(((Table*)ws->activeWindow())->selectedColumns().count())>0)
		((Table*)ws->activeWindow())->normalizeSelection();
	else
		QMessageBox::warning(this, "QtiPlot - Column selection error","Please select a column first!");
}

void ApplicationWindow::correlate()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("Table"))
		return;

	((Table*)ws->activeWindow())->correlate();
}

void ApplicationWindow::convolute()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("Table"))
		return;

	((Table*)ws->activeWindow())->convolute(1);
}

void ApplicationWindow::deconvolute()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("Table"))
		return;

	((Table*)ws->activeWindow())->convolute(-1);
}

void ApplicationWindow::showColStatistics()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;
	Table *t = (Table*)ws->activeWindow();

	if (int(t->selectedColumns().count()) > 0)
	{
		QList<int> targets;
		for (int i=0; i < t->tableCols(); i++)
			if (t->isColumnSelected(i, true))
				targets << i;
		newTableStatistics(t, TableStatistics::column, targets)->showNormal();
	}
	else
		QMessageBox::warning(this, tr("QtiPlot - Column selection error"),
				tr("Please select a column first!"));
}

void ApplicationWindow::showRowStatistics()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;
	Table *t = (Table*)ws->activeWindow();

	if (t->selectedRows() > 0)
	{
		QList<int> targets;
		for (int i=0; i < t->tableRows(); i++)
			if (t->isRowSelected(i, true))
				targets << i;
		newTableStatistics(t, TableStatistics::row, targets)->showNormal();
	}
	else
		QMessageBox::warning(this, tr("QtiPlot - Row selection error"),
				tr("Please select a row first!"));
}

void ApplicationWindow::showColMenu(int c)
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	Table* w = (Table*)ws->activeWindow();

	QMenu contextMenu(this);
	QMenu plot(this);
	QMenu specialPlot(this);
	QMenu fill(this);
	QMenu sorting(this);
	QMenu colType(this);
	colType.setCheckable ( true );
	QMenu panels(this);
	QMenu stat(this);
	QMenu norm(this);

	if ((int)w->selectedColumns().count()==1)
	{
		w->setSelectedCol(c);
		plot.addAction(QIcon(QPixmap(lPlot_xpm)),tr("&Line"),w, SLOT(plotL()));
		plot.addAction(QIcon(QPixmap(pPlot_xpm)),tr("&Scatter"),w, SLOT(plotP()));
		plot.addAction(QIcon(QPixmap(lpPlot_xpm)),tr("Line + S&ymbol"),w,SLOT(plotLP()));

		specialPlot.addAction(QIcon(QPixmap(dropLines_xpm)),tr("Vertical &Drop Lines"),w,SLOT(plotVerticalDropLines()));
		specialPlot.addAction(QIcon(QPixmap(spline_xpm)),tr("&Spline"),w,SLOT(plotSpline()));
		specialPlot.addAction(QIcon(QPixmap(vert_steps_xpm)),tr("&Vertical Steps"),w,SLOT(plotVertSteps()));
		specialPlot.addAction(QIcon(QPixmap(hor_steps_xpm)),tr("&Horizontal Steps"),w,SLOT(plotHorSteps()));
		specialPlot.setTitle(tr("Special Line/Symb&ol"));
		plot.addMenu(&specialPlot);
		plot.insertSeparator();

		plot.addAction(QIcon(QPixmap(vertBars_xpm)),tr("&Columns"),w,SLOT(plotVB()));
		plot.addAction(QIcon(QPixmap(hBars_xpm)),tr("&Rows"),w,SLOT(plotHB()));
		plot.addAction(QIcon(QPixmap(area_xpm)),tr("&Area"),w,SLOT(plotArea()));

		plot.addAction(QIcon(QPixmap(pie_xpm)),tr("&Pie"),w,SLOT(plotPie()));
		plot.insertSeparator();

		plot.addAction(QIcon(QPixmap(ribbon_xpm)),tr("3D Ribbo&n"),w,SLOT(plot3DRibbon()));
		plot.addAction(QIcon(QPixmap(bars_xpm)),tr("3D &Bars"),w,SLOT(plot3DBars()));
		plot.addAction(QIcon(QPixmap(scatter_xpm)),tr("3&D Scatter"),w,SLOT(plot3DScatter()));
		plot.addAction(QIcon(QPixmap(trajectory_xpm)),tr("3D &Trajectory"),w,SLOT(plot3DTrajectory()));

		plot.insertSeparator();

		stat.addAction(actionBoxPlot);
		stat.addAction(QIcon(QPixmap(histogram_xpm)),tr("&Histogram"),w,SLOT(plotHistogram()));
		stat.addAction(QIcon(QPixmap(stacked_hist_xpm)),tr("&Stacked Histograms"),this,SLOT(plotStackedHistograms()));
		stat.setTitle(tr("Statistical &Graphs"));
		plot.addMenu(&stat);

		plot.setTitle(tr("&Plot"));
		contextMenu.addMenu(&plot);
		contextMenu.insertSeparator();

		contextMenu.addAction(QIcon(QPixmap(cut_xpm)),tr("Cu&t"), w, SLOT(cutSelection()));
		contextMenu.addAction(QIcon(QPixmap(copy_xpm)),tr("&Copy"), w, SLOT(copySelection()));
		contextMenu.addAction(QIcon(QPixmap(paste_xpm)),tr("Past&e"), w, SLOT(pasteSelection()));
		contextMenu.insertSeparator();

		QAction * xColID=colType.addAction(tr("X"), this, SLOT(setXCol()));
		QAction * yColID=colType.addAction(tr("Y"), this, SLOT(setYCol()));
		QAction * zColID=colType.addAction(tr("Z"), this, SLOT(setZCol()));
		colType.insertSeparator();
		QAction * xErrColID =colType.addAction(tr("X Error"), this, SLOT(setXErrCol()));
		QAction * yErrColID = colType.addAction(QIcon(QPixmap(errors_xpm)), tr("Y Error"), this, SLOT(setYErrCol()));
		colType.insertSeparator();
		QAction * noneID=colType.addAction(tr("None"), this, SLOT(disregardCol()));

		if (w->colPlotDesignation(c) == Table::X)
			xColID->setChecked(true);
		else if (w->colPlotDesignation(c) == Table::Y)
			yColID->setChecked(true);
		else if (w->colPlotDesignation(c) == Table::Z)
			zColID->setChecked(true);
		else if (w->colPlotDesignation(c) == Table::xErr)
			xErrColID->setChecked(true);
		else if (w->colPlotDesignation(c) == Table::yErr)
			yErrColID->setChecked(true);
		else
			noneID->setChecked(true);

		colType.setTitle(tr("Set As"));
		contextMenu.addMenu(&colType);
		if (ws->activeWindow()->isA("Table"))
		{
			contextMenu.insertSeparator();

			contextMenu.addAction(actionShowColumnValuesDialog);
			contextMenu.addAction(actionTableRecalculate);
			fill.addAction(actionSetAscValues);
			fill.addAction(actionSetRandomValues);
			fill.setTitle(tr("&Fill Column With"));
			contextMenu.addMenu(&fill);

			norm.addAction(tr("&Column"), w, SLOT(normalizeSelection()));
			norm.addAction(actionNormalizeTable);
			norm.setTitle(tr("&Normalize"));
			contextMenu.addMenu(& norm);

			contextMenu.insertSeparator();
			contextMenu.addAction(actionShowColStatistics);

			contextMenu.insertSeparator();

			contextMenu.addAction(QIcon(QPixmap(erase_xpm)), tr("Clea&r"), w, SLOT(clearCol()));
			contextMenu.addAction(QIcon(QPixmap(close_xpm)), tr("&Delete"), w, SLOT(removeCol()));
			contextMenu.addAction(tr("&Insert"), w, SLOT(insertCol()));
			contextMenu.addAction(tr("&Add Column"),w, SLOT(addCol()));
			contextMenu.insertSeparator();

			sorting.addAction(tr("&Ascending"),w, SLOT(sortColAsc()));
			sorting.addAction(tr("&Descending"),w, SLOT(sortColDesc()));
			sorting.setTitle(tr("Sort Colu&mn"));
			contextMenu.addMenu(&sorting);

			contextMenu.addAction(actionSortTable);
		}

		contextMenu.insertSeparator();
		contextMenu.addAction(actionShowColumnOptionsDialog);
	}
	else if ((int)w->selectedColumns().count()>1)
	{
		plot.addAction(QIcon(QPixmap(lPlot_xpm)),tr("&Line"),w, SLOT(plotL()));
		plot.addAction(QIcon(QPixmap(pPlot_xpm)),tr("&Scatter"),w, SLOT(plotP()));
		plot.addAction(QIcon(QPixmap(lpPlot_xpm)),tr("Line + S&ymbol"),w,SLOT(plotLP()));

		specialPlot.addAction(QIcon(QPixmap(dropLines_xpm)),tr("Vertical &Drop Lines"),w,SLOT(plotVerticalDropLines()));
		specialPlot.addAction(QIcon(QPixmap(spline_xpm)),tr("&Spline"),w,SLOT(plotSpline()));
		specialPlot.addAction(QIcon(QPixmap(vert_steps_xpm)),tr("&Vertical Steps"),w,SLOT(plotVertSteps()));
		specialPlot.addAction(QIcon(QPixmap(hor_steps_xpm)),tr("&Vertical Steps"),w,SLOT(plotHorSteps()));
		specialPlot.setTitle(tr("Special Line/Symb&ol"));
		plot.addMenu(&specialPlot);
		plot.insertSeparator();

		plot.addAction(QIcon(QPixmap(vertBars_xpm)),tr("&Columns"),w,SLOT(plotVB()));
		plot.addAction(QIcon(QPixmap(hBars_xpm)),tr("&Rows"),w,SLOT(plotHB()));
		plot.addAction(QIcon(QPixmap(area_xpm)),tr("&Area"),w,SLOT(plotArea()));
		plot.addAction(QIcon(QPixmap(vectXYXY_xpm)),tr("Vectors &XYXY"), w, SLOT(plotVectXYXY()));
		plot.insertSeparator();

		stat.addAction(actionBoxPlot);
		stat.addAction(QIcon(QPixmap(histogram_xpm)),tr("&Histogram"),w,SLOT(plotHistogram()));
		stat.addAction(QIcon(QPixmap(stacked_hist_xpm)),tr("&Stacked Histograms"),this,SLOT(plotStackedHistograms()));
		stat.setTitle(tr("Statistical &Graphs"));
		plot.addMenu(&stat);

		panels.addAction(QIcon(QPixmap(panel_v2_xpm)),tr("&Vertical 2 Layers"),this, SLOT(plot2VerticalLayers()));
		panels.addAction(QIcon(QPixmap(panel_h2_xpm)),tr("&Horizontal 2 Layers"),this, SLOT(plot2HorizontalLayers()));
		panels.addAction(QIcon(QPixmap(panel_4_xpm)),tr("&4 Layers"),this, SLOT(plot4Layers()));
		panels.addAction(QIcon(QPixmap(stacked_xpm)),tr("&Stacked Layers"),this, SLOT(plotStackedLayers()));
		panels.setTitle(tr("Pa&nel"));
		plot.addMenu(&panels);

		plot.setTitle(tr("&Plot"));
		contextMenu.addMenu(&plot);
		contextMenu.insertSeparator();
		contextMenu.addAction(QIcon(QPixmap(cut_xpm)),tr("Cu&t"), w, SLOT(cutSelection()));
		contextMenu.addAction(QIcon(QPixmap(copy_xpm)),tr("&Copy"), w, SLOT(copySelection()));
		contextMenu.addAction(QIcon(QPixmap(paste_xpm)),tr("Past&e"), w, SLOT(pasteSelection()));
		contextMenu.insertSeparator();

		if (ws->activeWindow()->isA("Table"))
		{
			contextMenu.addAction(QIcon(QPixmap(erase_xpm)),tr("Clea&r"), w, SLOT(clearSelection()));
			contextMenu.addAction(QIcon(QPixmap(close_xpm)),tr("&Delete"), w, SLOT(removeCol()));
			contextMenu.insertSeparator();
			contextMenu.addAction(tr("&Insert"), w, SLOT(insertCol()));
			contextMenu.addAction(tr("&Add Column"),w, SLOT(addCol()));
			contextMenu.insertSeparator();
		}

		colType.addAction(actionSetXCol);
		colType.addAction(actionSetYCol);
		colType.addAction(actionSetZCol);
		colType.insertSeparator();
		colType.addAction(actionSetXErrCol);
		colType.addAction(actionSetYErrCol);
		colType.insertSeparator();
		colType.addAction(actionDisregardCol);
		colType.setTitle(tr("Set As"));
		contextMenu.addMenu(&colType);

		if (ws->activeWindow()->isA("Table"))
		{
			contextMenu.insertSeparator();

			fill.addAction(actionSetAscValues);
			fill.addAction(actionSetRandomValues);
			fill.setTitle(tr("&Fill Columns With"));
			contextMenu.addMenu(&fill);

			norm.addAction(actionNormalizeSelection);
			norm.addAction(actionNormalizeTable);
			norm.setTitle(tr("&Normalize"));
			contextMenu.addMenu(&norm);

			contextMenu.insertSeparator();
			contextMenu.addAction(actionSortSelection);
			contextMenu.addAction(actionSortTable);
			contextMenu.insertSeparator();
			contextMenu.addAction(actionShowColStatistics);
		}
	}

	QPoint posMouse=QCursor::pos();
	contextMenu.exec(posMouse);
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
	if ( ws->activeWindow() && ws->activeWindow()->isA("Matrix"))
	{
		Matrix* w = (Matrix*)ws->activeWindow();

		MatrixDialog* md= new MatrixDialog(this);
		md->setAttribute(Qt::WA_DeleteOnClose);
		connect (md, SIGNAL(changeColumnsWidth(int)), w, SLOT(setColumnsWidth(int)));
		connect (md, SIGNAL(changeTextFormat(const QChar&, int)), 
				w, SLOT(setNumericFormat(const QChar&, int)));

		w->saveCellsToMemory();
		md->setTextFormat(w->textFormat(), w->precision());
		md->setColumnsWidth(w->columnsWidth());
		md->exec();
		w->forgetSavedCells();
	}
}

void ApplicationWindow::showMatrixSizeDialog()
{
	if ( ws->activeWindow() && ws->activeWindow()->isA("Matrix"))
	{
		Matrix* w = (Matrix*)ws->activeWindow();

		MatrixSizeDialog* md= new MatrixSizeDialog(this);
		md->setAttribute(Qt::WA_DeleteOnClose);
		connect (md, SIGNAL(changeDimensions(int, int)), w, SLOT(setMatrixDimensions(int, int)));
		connect (md, SIGNAL(changeCoordinates(double, double, double, double)), 
				w, SLOT(setCoordinates(double, double, double, double)));

		md->setCoordinates(w->xStart(), w->xEnd(), w->yStart(), w->yEnd());
		md->setColumns(w->numCols());
		md->setRows(w->numRows());
		md->exec();
	}
}

void ApplicationWindow::showMatrixValuesDialog()
{
	if ( ws->activeWindow() && ws->activeWindow()->isA("Matrix"))
	{
		MatrixValuesDialog* md= new MatrixValuesDialog(scriptEnv, this, 0, false);
		md->setAttribute(Qt::WA_DeleteOnClose);
		md->setMatrix((Matrix*)ws->activeWindow());
		md->exec();
	}
}

void ApplicationWindow::showColumnOptionsDialog()
{
	if ( !ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	Table* w = (Table*)ws->activeWindow();

	if(w->selectedColumns().count()>0)
	{
		TableDialog* td= new TableDialog(this,"optionsDialog", false);
		td->setAttribute(Qt::WA_DeleteOnClose);
		td->setWorksheet(w);
		td->exec();
	}
	else
		QMessageBox::warning(this, tr("QtiPlot"),tr("Please select a column first!"));
}

void ApplicationWindow::showAxis(int axis, int type, const QString& labelsColName, bool axisOn, 
		int majTicksType, int minTicksType, bool labelsOn, const QColor& c, int format, 
		int prec, int rotation, int baselineDist, const QString& formula)
{
	Table *w = table(labelsColName);
	if ((type == Graph::Txt || type == Graph::ColHeader) && !w)
		return;

	activeGraph->showAxis(axis, type, labelsColName, w, axisOn, majTicksType, minTicksType, labelsOn, 
			c, format, prec, rotation, baselineDist, formula);
}

void ApplicationWindow::showGeneralPlotDialog()
{
	QWidget* plot = ws->activeWindow();
	if (!plot)
		return;

	QDialog* gd = showScaleDialog();
	if (gd && plot->isA("MultiLayer") && ((MultiLayer*)plot)->graphsNumber())
	{
		Graph* g = ((MultiLayer*)plot)->activeGraph();
		if (!g->isPiePlot())
			((AxesDialog*)gd)->showGeneralPage();
		else
			((PieDialog*)gd)->showGeneralPage();
	}
	else if (gd && plot->isA("Graph3D"))
		((Plot3DDialog*)gd)->showGeneralTab();
	delete gd;
}

void ApplicationWindow::showAxisDialog()
{
	QWidget* plot = (QWidget*)ws->activeWindow();
	if (!plot)
		return;

	QDialog* gd = showScaleDialog();
	if (gd && plot->isA("MultiLayer") && ((MultiLayer*)plot)->graphsNumber())
		((AxesDialog*)gd)->showAxesPage();
	else if (gd && plot->isA("Graph3D"))
		((Plot3DDialog*)gd)->showAxisTab();
}

void ApplicationWindow::showGridDialog()
{
	AxesDialog* gd = (AxesDialog*)showScaleDialog();
	if (gd)
		gd->showGridPage();
	delete gd;
}

QDialog* ApplicationWindow::showScaleDialog()
{
	QWidget *w = ws->activeWindow();
	if (!w)
		return 0;

	if (w->isA("MultiLayer"))
	{
		if (((MultiLayer*)w)->isEmpty())
			return 0;

		Graph* g = ((MultiLayer*)w)->activeGraph();
		if (!g->isPiePlot())
		{
			activeGraph = g;

			AxesDialog* ad= new AxesDialog(this);
			connect (ad,SIGNAL(updateAxisTitle(int,const QString&)),g,SLOT(setAxisTitle(int,const QString&)));
			connect (ad,SIGNAL(changeAxisFont(int, const QFont &)),g,SLOT(setAxisFont(int,const QFont &)));
			connect (ad,SIGNAL(showAxis(int, int, const QString&, bool,int, int, bool,const QColor&, int, int, int, int, const QString&)),
					this, SLOT(showAxis(int,int, const QString&, bool, int, int, bool,const QColor&, int, int, int, int, const QString&)));

			ad->setMultiLayerPlot((MultiLayer*)w);
			ad->insertColList(columnsList(Table::All));
			ad->insertTablesList(tableWindows);
			ad->setAxesLabelsFormatInfo(g->axesLabelsFormatInfo());
			ad->setEnabledAxes(g->enabledAxes());
			ad->setAxesType(g->axesType());
			ad->setAxesBaseline(g->axesBaseline());
			
			ad->initAxisFonts(g->axisFont(2), g->axisFont(0),g->axisFont(3),g->axisFont(1));
			ad->setAxisTitles(g->scalesTitles());
			ad->updateTitleBox(0);
			ad->putGridOptions(g->getGridOptions());
			ad->setAxesColors(g->axesColors());
			ad->setTicksType(g->plotWidget()->getMajorTicksType(), g->plotWidget()->getMinorTicksType());
			ad->setEnabledTickLabels(g->enabledTickLabels());
			ad->initLabelsRotation(g->labelsRotation(QwtPlot::xBottom), g->labelsRotation(QwtPlot::xTop));
			ad->exec();
			return ad;
		}
		else if (g->isPiePlot())
			return showPieDialog();
	}
	else if (w->isA("Graph3D"))
		return showPlot3dDialog();

	return 0;
}

AxesDialog* ApplicationWindow::showScalePageFromAxisDialog(int axisPos)
{
	AxesDialog* gd = (AxesDialog*)showScaleDialog();
	if (gd)
		gd->setCurrentScale(axisPos);

	return gd;
}

AxesDialog* ApplicationWindow::showAxisPageFromAxisDialog(int axisPos)
{
	AxesDialog* gd = (AxesDialog*)showScaleDialog();
	if (gd)
	{
		gd->showAxesPage();
		gd->setCurrentScale(axisPos);
	}
	return gd;
}

QDialog* ApplicationWindow::showPlot3dDialog()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
	{
		Graph3D* g = (Graph3D*)ws->activeWindow();
		if (!g->hasData())
		{
			QApplication::restoreOverrideCursor();
			QMessageBox::warning(this, tr("QtiPlot - Warning"),
					tr("Not available for empty 3D surface plots!"));
			return 0;
		}

		Plot3DDialog* pd= new Plot3DDialog(this,"Plot3DDialog",true,0);
		pd->setPlot(g);
		connect (pd,SIGNAL(updateColors(const QColor&,const QColor&,const QColor&,const QColor&,const QColor&,const QColor&)),
				g,SLOT(updateColors(const QColor&,const QColor&,const QColor&,const QColor&,const QColor&,const QColor&)));
        connect(pd, SIGNAL(setDataColorMap(const QString&)), g, SLOT(setDataColorMap(const QString&)));
		connect (pd,SIGNAL(updateDataColors(const QColor&,const QColor&)),
				g,SLOT(setDataColors(const QColor&,const QColor&)));

		connect (pd,SIGNAL(updateTitle(const QString&,const QColor&,const QFont&)),
				g,SLOT(updateTitle(const QString&,const QColor&,const QFont&)));
		connect (pd,SIGNAL(updateResolution(int)),g,SLOT(setResolution(int)));
		connect (pd,SIGNAL(showColorLegend(bool)),g,SLOT(showColorLegend(bool)));
	    connect (pd,SIGNAL(setOrtho(bool)), g, SLOT(setOrtho(bool)));
		connect (pd,SIGNAL(updateLabel(int,const QString&, const QFont&)),
				g,SLOT(updateLabel(int,const QString&, const QFont&)));
		connect (pd,SIGNAL(updateScale(int,const QStringList&)),
				g,SLOT(updateScale(int,const QStringList&)));
		connect (pd,SIGNAL(adjustLabels(int)), g,SLOT(adjustLabels(int)));
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
	    pd->setOrthogonal(g->isOrthogonal());
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
			pd->customWorksheetBtn(QString());
		else if (g->getTable())
			pd->customWorksheetBtn(tr("&Worksheet"));
		else if (g->getMatrix())
			pd->customWorksheetBtn(tr("&Matrix"));

		pd->exec();
		return pd;
	}
	else return 0;
}

QDialog* ApplicationWindow::showPieDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return 0;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (g)
	{
		activeGraph = g;

		PieDialog* pd= new PieDialog(this,"PlotDialog",true,0);
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

		pd->setMultiLayerPlot((MultiLayer*)ws->activeWindow());
		pd->show();
		return pd;
	}
	return 0;
}

void ApplicationWindow::showPlotDialog()
{
	QWidget *w = ws->activeWindow();
	if (!w)
		return;

	if (w->isA("MultiLayer"))
	{
		Graph *g = ((MultiLayer*)w)->activeGraph();
		if (!g)
			return;

		if (g->curves()>0)
		{
			if (!g->isPiePlot())
			{
				PlotDialog* pd= new PlotDialog(this,"PlotDialog",false,0);
				pd->setAttribute(Qt::WA_DeleteOnClose);
				pd->insertColumnsList(columnsList(Table::All));
				pd->setGraph(g);
				pd->selectCurve(0);
				pd->exec();

				activeGraph = g;
			}
			else
				delete showPieDialog();
		}
		else if (g->curves() == 0)
			QMessageBox::warning(this, tr("QtiPlot - Empty plot"),
					tr("There are actually no curves on the active layer!"));
	}
	else if (w->isA("Graph3D"))
		delete showPlot3dDialog();
}

void ApplicationWindow::showPlotDialog(int curveKey)
{
	QWidget *w = ws->activeWindow();
	if (!w)
		return;

	if (w->isA("MultiLayer"))
	{
		Graph *g = ((MultiLayer*)w)->activeGraph();
		if (!g || g->curves() <= 0)
			return;

		if (!g->isPiePlot())
		{
			PlotDialog* pd= new PlotDialog(this,"PlotDialog",false,0);
			pd->setAttribute(Qt::WA_DeleteOnClose);
			pd->insertColumnsList(columnsList(Table::All));
			pd->setGraph(g);
			pd->selectCurve(g->curveIndex(curveKey));
			pd->exec();

			activeGraph = g;
		}
		else
			delete showPieDialog();
	}
}

void ApplicationWindow::showCurveContextMenu(int curveKey)
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	activeGraph = ((MultiLayer*)ws->activeWindow())->activeGraph();
	QwtPlotCurve *c = activeGraph->curve(activeGraph->curveIndex(curveKey));
	if (!c)
		return;

	QMenu curveMenu(this);

	QAction *act = curveMenu.addAction(c->title().text(), this, SLOT(showPlotDialog(int)));
	act->setData(curveKey);
	curveMenu.insertSeparator();
 
	if (c->rtti() == FunctionCurve::RTTI)
	{
		 act = curveMenu.addAction(tr("&Edit Function..."), this, SLOT(showFunctionDialog(int)));
		act->setData(curveKey);
	}

	act = curveMenu.addAction(tr("&Worksheet"), this, SLOT(showCurveWorksheet(int)));
	act->setData(curveKey);
	act = curveMenu.addAction(tr("&Plot details..."), this, SLOT(showPlotDialog(int)));
	act->setData(curveKey);
	curveMenu.insertSeparator();
	act = curveMenu.addAction(QPixmap(close_xpm), tr("&Delete"), this, SLOT(removeCurve(int)));
	act->setData(curveKey);
 
	curveMenu.exec(QCursor::pos());
}

void ApplicationWindow::removeCurve(int curveKey)
{
	activeGraph->removeCurve(activeGraph->curveIndex(curveKey));
}

void ApplicationWindow::showCurveWorksheet(int curveKey)
{
	const QwtPlotCurve *c = activeGraph->curve(activeGraph->curveIndex(curveKey));
	if (!c)
		return;

	QString curveTitle = c->title().text();
	if (c->rtti() == FunctionCurve::RTTI)
		activeGraph->createWorksheet(curveTitle);
	else
		showTable(curveTitle);
}

void ApplicationWindow::zoomIn()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();	
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setOn(true);
		return;
	}

	if ((Graph*)plot->activeGraph()->isPiePlot())
	{
		if (btnZoomIn->isOn())
			QMessageBox::warning(this,tr("QtiPlot - Warning"),
					tr("This functionality is not available for pie plots!"));
		btnPointer->setOn(true);
		return;
	}

	QWidgetList graphsList=plot->graphPtrs();
	foreach(QWidget *widget, graphsList)
	{
		Graph *g = (Graph *)widget;
		if (!g->isPiePlot())
			g->zoom(true);
	}
}

void ApplicationWindow::zoomOut()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();	
	if (plot->isEmpty() || (Graph*)plot->activeGraph()->isPiePlot())
		return;

	((Graph*)plot->activeGraph())->zoomOut();
	btnPointer->setOn(true);
}

void ApplicationWindow::setAutoScale()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
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

void ApplicationWindow::removePoints()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	Graph* g = (Graph*)plot->activeGraph();
	if (!g)
	{
		btnPointer->setChecked(true);
		return;
	}

	if (g->isPiePlot())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));
		btnPointer->setChecked(true);
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
				btnPointer->setChecked(true);
				break;
		}
	}
}

void ApplicationWindow::movePoints()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	Graph* g = (Graph*)plot->activeGraph();
	if (!g)
		return;

	if (g->isPiePlot())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));

		btnPointer->setChecked(true);
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
				btnPointer->setChecked(true);
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

	((MyWidget*)w)->print();
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
	MyWidget *w= it->window();
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

	if (printer.setup())
	{ 
		QPainter *paint = new QPainter (&printer);
		QWidgetList *windows = windowsList();

		int plots = 0;
		QWidget *w = 0;
		foreach(w, *windows)
		{
			if (w->isA("MultiLayer"))
				plots++;
		}

		printer.setMinMax (0, plots);
		printer.setFromTo (0, plots);

		foreach(w, *windows)
		{
			if (w->isA("MultiLayer") && printer.newPage())
				((MultiLayer*)w)->printAllLayers(paint);
		}
		paint->end();
		delete paint;
		delete windows;
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
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (!g || !g->validCurvesDataSize())
		return;

	activeGraph = g;
	aw = (MyWidget *)ws->activeWindow();

	ExpDecayDialog *edd = new ExpDecayDialog(type, this,"polyDialog", false, 0);
	edd->setAttribute(Qt::WA_DeleteOnClose);
	connect ((MyWidget*)ws->activeWindow(), SIGNAL(closedWindow(MyWidget*)), edd, SLOT(close()));

	edd->setGraph(g);
	edd->exec();
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
	if(w->isA("MultiLayer"))
		plot = (MultiLayer*)w;
	else if(w->isA("Table"))
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
	aw = (MyWidget *)ws->activeWindow();

	FitDialog *fd=new FitDialog(this,"FitDialog", false);
	fd->setAttribute(Qt::WA_DeleteOnClose);
	connect (fd, SIGNAL(clearFunctionsList()), this, SLOT(clearFitFunctionsList()));
	connect (fd, SIGNAL(saveFunctionsList(const QStringList&)), 
			this, SLOT(saveFitFunctionsList(const QStringList&)));
	connect (plot, SIGNAL(closedWindow(MyWidget*)), fd, SLOT(close()));

	fd->insertFunctionsList(fitFunctions);
	fd->setSrcTables(tableList());
	fd->setGraph(g);
	fd->exec();
}

void ApplicationWindow::lowPassFilterDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if ( g )
	{
		if (!g->curves())
		{
			QMessageBox::warning(this, tr("QtiPlot - Warning"),
					tr("There are no curves available on this plot!"));
			return;
		}

		FilterDialog *fd=new FilterDialog(FilterDialog::LowPass, this,"FilterDialog",
				true,0);	
		fd->setAttribute(Qt::WA_DeleteOnClose);
		fd->setGraph(g);
		fd->exec();
	}
}

void ApplicationWindow::highPassFilterDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if ( g )
	{
		if (!g->curves())
		{
			QMessageBox::warning(this, tr("QtiPlot - Warning"),
					tr("There are no curves available on this plot!"));
			return;
		}

		FilterDialog *fd=new FilterDialog(FilterDialog::HighPass, this,"FilterDialog",
				true,0);	
		fd->setAttribute(Qt::WA_DeleteOnClose);
		fd->setGraph(g);
		fd->exec();
	}
}

void ApplicationWindow::bandPassFilterDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if ( g )
	{
		if (!g->curves())
		{
			QMessageBox::warning(this, tr("QtiPlot - Warning"),
					tr("There are no curves available on this plot!"));
			return;
		}

		FilterDialog *fd=new FilterDialog(FilterDialog::BandPass, this,"FilterDialog",
				true,0);	
		fd->setAttribute(Qt::WA_DeleteOnClose);
		fd->setGraph(g);
		fd->exec();
	}
}

void ApplicationWindow::bandBlockFilterDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if ( g )
	{
		if (!g->curves())
		{
			QMessageBox::warning(this, tr("QtiPlot - Warning"),
					tr("There are no curves available on this plot!"));
			return;
		}

		FilterDialog *fd=new FilterDialog(FilterDialog::BandBlock, this,"FilterDialog",
				true,0);	
		fd->setAttribute(Qt::WA_DeleteOnClose);
		fd->setGraph(g);
		fd->exec();
	}
}

void ApplicationWindow::showFFTDialog()
{
	QWidget *w = ws->activeWindow();
	if (!w)
		return;

	FFTDialog *sd = 0;
	if (w->isA("MultiLayer"))
	{
		Graph* g = ((MultiLayer*)w)->activeGraph();
		if ( g )
		{
			if (!g->curves())
			{
				QMessageBox::warning(this, tr("QtiPlot - Warning"),
						tr("There are no curves available on this plot!"));
				return;
			}

			sd=new FFTDialog(FFTDialog::onGraph, this,"smoothDialog",true,0);	
			sd->setAttribute(Qt::WA_DeleteOnClose);
			sd->setGraph(g);
		}
	}
	else if (w->isA("Table"))
	{
		sd=new FFTDialog(FFTDialog::onTable, this,"smoothDialog",true,0);	
		sd->setAttribute(Qt::WA_DeleteOnClose);
		sd->setTable((Table*)w);
	}

	if (sd)
	{
		sd->exec();
	}
}

void ApplicationWindow::showSmoothSavGolDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (!g || !g->validCurvesDataSize())
		return;

	SmoothCurveDialog *sd=new SmoothCurveDialog(SmoothCurveDialog::SavitzkyGolay, 
			this,"smoothDialog",true,0);	
	sd->setAttribute(Qt::WA_DeleteOnClose);
	sd->setGraph(g);
	sd->exec();
}

void ApplicationWindow::showSmoothFFTDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (!g || !g->validCurvesDataSize())
		return;

	SmoothCurveDialog *sd=new SmoothCurveDialog(SmoothCurveDialog::FFT, this,"smoothDialog",true,0);	
	sd->setAttribute(Qt::WA_DeleteOnClose);
	sd->setGraph(g);
	sd->exec();
}

void ApplicationWindow::showSmoothAverageDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (!g || !g->validCurvesDataSize())
		return;

	SmoothCurveDialog *sd=new SmoothCurveDialog(SmoothCurveDialog::Average, this,"smoothDialog",true,0);	
	sd->setAttribute(Qt::WA_DeleteOnClose);
	sd->setGraph(g);
	sd->exec();
}

void ApplicationWindow::showInterpolationDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (!g || !g->validCurvesDataSize())
		return;

	InterpolationDialog *id=new InterpolationDialog(this,"InterpolationDialog",false,0);	
	id->setAttribute(Qt::WA_DeleteOnClose);
	connect ((MyWidget *)ws->activeWindow(), SIGNAL(closedWindow(MyWidget*)), id, SLOT(close()));
	id->setGraph(g);
	id->exec();
}

void ApplicationWindow::showFitPolynomDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (!g || !g->validCurvesDataSize())
		return;

	activeGraph=g;
	aw = (MyWidget *)ws->activeWindow();

	PolynomFitDialog *pfd=new PolynomFitDialog(this,"polyDialog",false,0);	
	pfd->setAttribute(Qt::WA_DeleteOnClose);
	connect ((MyWidget*)ws->activeWindow(), SIGNAL(closedWindow(MyWidget*)), pfd, SLOT(close()));
	pfd->setGraph(g);
	pfd->exec();
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
	}
}

void ApplicationWindow::showIntDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (!g || !g->validCurvesDataSize())
		return;

	IntDialog *id=new IntDialog(this,"IntDialog",false,0);
	id->setAttribute(Qt::WA_DeleteOnClose);
	connect ((MyWidget*)ws->activeWindow(), SIGNAL(closedWindow(MyWidget*)), id, SLOT(close()));
	id->setGraph(g);
	id->exec();
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
		QTextCursor cur = results->textCursor();
		cur.movePosition(QTextCursor::End);
		results->setTextCursor(cur);
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
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	QWidgetList graphsList=plot->graphPtrs();
	foreach(QWidget *w, graphsList)
		((Graph *)w)->showPlotPicker(true);

	info->setText(tr("Click on plot or move cursor to display coordinates!"));
	displayBar->show();
}

void ApplicationWindow::showRangeSelectors()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("There are no plot layers available in this window!"));
		btnPointer->setChecked(true);
		return;
	}

	Graph* g = (Graph*)plot->activeGraph();
	if (!g)
		return;

	if (!g->curves())
	{
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("There are no curves available on this plot!"));
		btnPointer->setChecked(true);
		return;
	}
	else if (g->isPiePlot())
	{
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));
		btnPointer->setChecked(true);
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
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	if ((Graph*)plot->activeGraph()->isPiePlot())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));

		btnPointer->setChecked(true);
		return;
	}

	QWidgetList graphsList=plot->graphPtrs();
	foreach(QWidget *w, graphsList)
	{
		if (!((Graph *)w)->isPiePlot())
			((Graph *)w)->enableCursor(true);
	}

	info->setText(tr("Click on data point to display information!"));
	displayBar->show();
}

void ApplicationWindow::newLegend()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		return;
	}

	Graph* g = (Graph*)plot->activeGraph();
	if ( g )
		g->newLegend();
}

void ApplicationWindow::addTimeStamp()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		return;
	}

	Graph* g = (Graph*)plot->activeGraph();
	if ( g )
		g->addTimeStamp();
}

void ApplicationWindow::disableAddText()
{
	actionAddText->setChecked(false);
	showTextDialog();
}

void ApplicationWindow::addText()
{
	if (!btnPointer->isOn())
		btnPointer->setOn(TRUE);

	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();

	switch(QMessageBox::information(this,
				tr("QtiPlot - Add new layer?"),
				tr("Do you want to add the text on a new layer or on the active layer?"),
				tr("On &New Layer"), tr("On &Active Layer"), tr("&Cancel"),
				0, 2 ) )
	{
		case 0:
			plot->addTextLayer(legendFrameStyle, plotLegendFont, legendTextColor, legendBackground);
			break;

		case 1:
			{
				if (plot->isEmpty())
				{
					QMessageBox::warning(this,tr("QtiPlot - Warning"),
							tr("<h4>There are no plot layers available in this window.</h4>"
								"<p><h4>Please add a layer and try again!</h4>"));

					actionAddText->setChecked(false);
					return;
				}

				Graph *g = (Graph*)plot->activeGraph();
				if (g)
					g->drawText(true);
			}
			break;

		case 2:
			actionAddText->setChecked(false);
			return;
			break;
	}
}

void ApplicationWindow::addImage()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
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
	   QList<QByteArray> list = QImageReader::supportedImageFormats();
	   QString filter = tr("Images") + " (", aux1, aux2;
	   for (int i=0; i<(int)list.count(); i++)
	       {
		   aux1 = " *."+list[i]+" ";
		   aux2 += " *."+list[i]+";;";
		   filter += aux1;
	       }
	    filter+=");;" + aux2;

		QString fn = Q3FileDialog::getOpenFileName(workingDir, filter, this, 0,
				"QtiPlot - Insert image from file", 0, true);
		if ( !fn.isEmpty() )
		{
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			QPixmap photo;
			for (int i=0;i<(int)list.count();i++)
			{
				if (fn.contains("."+list[i], false))
				{
					photo.load(fn,list[i],QPixmap::Color);
					break;
				}
			}

			g->insertImageMarker(photo,fn);

			QFileInfo fi(fn);
			workingDir = fi.dirPath(true);
			QApplication::restoreOverrideCursor();
		}
	}
}

void ApplicationWindow::drawLine()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));

		btnPointer->setChecked(true);
		return;
	}

	Graph* g = (Graph*)plot->activeGraph();
	if (g)
	{
		g->drawLine(true);
		emit modified();
	}
}

void ApplicationWindow::drawArrow()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
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
		g->drawLine(true, 1);
		emit modified();
	}
}

void ApplicationWindow::showImageDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (g)
	{
		ImageMarker *im= (ImageMarker *) g->selectedMarkerPtr();
		if (!im)
			return;

		ImageDialog *id=new ImageDialog(0,"ImageDialog",true,0);
		id->setAttribute(Qt::WA_DeleteOnClose);
		connect (id,SIGNAL(setGeometry(int,int,int,int)),g,SLOT(updateImageMarker(int,int,int,int)));
		id->setIcon(QPixmap(logo_xpm));
		id->setOrigin(im->getOrigin());
		id->setSize(im->size());
		id->exec();
	}
}

void ApplicationWindow::showLayerDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
	if(plot->isEmpty())
	{
		QMessageBox::warning(this, tr("QtiPlot - Warning"),
				tr("There are no plot layers available in this window."));
		return;
	}

	LayerDialog *id=new LayerDialog(this,"LayerDialog",true,0);
	id->setAttribute(Qt::WA_DeleteOnClose);
	id->setMultiLayer(plot);
	id->initFonts(plotTitleFont, plotAxesFont, plotNumbersFont, plotLegendFont);
	id->exec();
}

void ApplicationWindow::showPlotGeometryDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer*)ws->activeWindow();
	Graph* g = plot->activeGraph();
	if (g)
	{
		ImageDialog *id=new ImageDialog(0,"ImageDialog",true,0);
		id->setAttribute(Qt::WA_DeleteOnClose);
		connect (id,SIGNAL(setGeometry(int,int,int,int)),plot,SLOT(setGraphGeometry(int,int,int,int)));
		id->setIcon(QPixmap(logo_xpm));
		id->setWindowTitle(tr("QtiPlot - Layer Geometry"));
		id->setOrigin(g->pos());
		id->setSize(g->plotWidget()->size());
		id->exec();
	}
}

void ApplicationWindow::showTextDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if ( g )
	{
		LegendMarker *m= (LegendMarker *) g->selectedMarkerPtr();
		if (!m)
			return;

		TextDialog *td=new TextDialog(TextDialog::TextMarker, this, 0);
		td->setAttribute(Qt::WA_DeleteOnClose);
		connect (td,SIGNAL(values(const QString&,int,int,const QFont&, const QColor&, const QColor&)),
				g,SLOT(updateTextMarker(const QString&,int,int,const QFont&, const QColor&, const QColor&)));

		td->setIcon(QPixmap(logo_xpm));
		td->setText(m->getText());
		td->setFont(m->getFont());
		td->setTextColor(m->getTextColor());
		td->setBackgroundColor(m->backgroundColor());
		td->setBackgroundType(m->getBkgType());
		td->setAngle(m->getAngle());
		td->exec();
	}
}

void ApplicationWindow::showLineDialog()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (g)
	{
		LineMarker* lm=(LineMarker*)g->selectedMarkerPtr();
		if (!lm)
			return;

		LineDialog *ld = new LineDialog(lm, this, "lineDialog", TRUE, Qt::Tool);
		ld->setAttribute(Qt::WA_DeleteOnClose);
		ld->exec();
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

	if (m->inherits("Table"))
		((Table*)m)->clearSelection();
	else if (m->isA("Matrix"))
		((Matrix*)m)->clearSelection();
	else if (m->isA("MultiLayer"))
	{
		Graph* g = ((MultiLayer*)m)->activeGraph();
		if (!g)
			return;

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

	if (m->inherits("Table"))
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

	if (m->inherits("Table"))
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
			startArrowOn=m->hasStartArrow();
			endArrowOn=m->hasEndArrow();
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
	copiedLayer=false;
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
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

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
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
	{
		Graph3D* g = (Graph3D*)ws->activeWindow();
		if (!g->hasData())
		{
			QApplication::restoreOverrideCursor();
			QMessageBox::warning(this, tr("QtiPlot - Duplicate error"),
					tr("Empty 3D surface plots cannot be duplicated!"));
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
		else if (s.endsWith("(Z)",true))
			g2 = dataPlotXYZ(caption,s,g->xStart(),g->xStop(),
					g->yStart(),g->yStop(),g->zStart(),g->zStop());
		else if (s.endsWith("(Y)",true))
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
		if (!g->colorMap().isEmpty())
           g2->setDataColorMap(g->colorMap());
        else
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
		g2->setOrtho(g->isOrthogonal());
		g2->update();
		g2->animate(g->isAnimated());
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
	if (ws->activeWindow() &&  ws->activeWindow()->isA("MultiLayer"))
	{
		MultiLayer* plot = (MultiLayer*)ws->activeWindow();
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

		QWidgetList graphsList = plot->graphPtrs();

		for (int j=0;j<graphsList.count();j++)
		{
			Graph* g=(Graph*)graphsList.at(j);
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

MyWidget* ApplicationWindow::copyWindow()
{
	MyWidget* w=0;
	MyWidget* g = (MyWidget*)ws->activeWindow();
	if (!g)
	{
		QMessageBox::critical(this,tr("QtiPlot - Duplicate window error"),
				tr("There are no windows available in this project!"));
		return w;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	if (g->isA("MultiLayer"))
		w = copyGraph();
	else if (g->inherits("Table"))
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
			if (g->status() == MyWidget::Maximized)
				w->showMaximized();
		}
		else if (g->isA("Graph3D"))
		{
			((Graph3D*)w)->setIgnoreFonts(true);
			if (g->status() == MyWidget::Maximized)
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

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

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
		actionUndo->setEnabled(false);
		actionRedo->setEnabled(true);
	}
	else if (lastModified->isA("Note"))
	{
		((Note*)lastModified)->textWidget()->undo();
		actionUndo->setEnabled(false);
		actionRedo->setEnabled(true);
	}

	QApplication::restoreOverrideCursor();
}

void ApplicationWindow::redo()
{
	if (!lastModified)
		return;

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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
		actionUndo->setEnabled(true);
		actionRedo->setEnabled(false);
	}
	else if (lastModified->isA("Note"))
	{
		((Note*)lastModified)->textWidget()->redo();
		actionUndo->setEnabled(true);
		actionRedo->setEnabled(false);
	}
	QApplication::restoreOverrideCursor();
}

bool ApplicationWindow::hidden(QWidget* window)
{
	if (hiddenWindows->contains(window) || outWindows->contains(window))
		return true;

	return false;
}

void ApplicationWindow::updateWindowStatus(MyWidget* w)
{
	setListView(w->name(), w->aspect());

	if (w->status() == MyWidget::Maximized)
	{//set any other window having status = Maximized to status = Normal
		QList<MyWidget *> lst = current_folder->windowsList();
		if (!lst.contains(w))
			return;

		foreach(MyWidget *aw, lst)
		{
			if (aw != w && aw->status() == MyWidget::Maximized)
			{
				aw->setNormal();
				return;
			}
		}
	}
}

void ApplicationWindow::resizeActiveWindow()
{
	QWidget *w =(QWidget *)ws->activeWindow();
	if (!w)
		return;

	ImageDialog *id = new ImageDialog(this, "ImageDialog", true);
	id->setAttribute(Qt::WA_DeleteOnClose);
	connect (id, SIGNAL(setGeometry(int,int,int,int)), w->parentWidget(), SLOT(setGeometry(int,int,int,int)));

	id->setWindowTitle(tr("QtiPlot - Window Geometry"));
	id->setOrigin(w->parentWidget()->pos());
	id->setSize(w->parentWidget()->size());
	id->exec();
}

void ApplicationWindow::hideActiveWindow()
{
	MyWidget *w=(MyWidget *)ws->activeWindow();
	if (!w)
		return;

	hideWindow(w);
}

void ApplicationWindow::hideWindow(MyWidget* w)
{
	hiddenWindows->append(w);
	w->setHidden();
	emit modified();
}

void ApplicationWindow::hideWindow()
{
	WindowListItem *it = (WindowListItem *)lv->currentItem();
	MyWidget *w= it->window();
	if (!w)
		return;

	hideWindow(w);
}

void ApplicationWindow::resizeWindow()
{
	WindowListItem *it = (WindowListItem *)lv->currentItem();
	MyWidget *w= it->window();
	if (!w)
		return;

	ImageDialog *id=new ImageDialog(this,"ImageDialog",true,0);
	id->setAttribute(Qt::WA_DeleteOnClose);
	connect (id,SIGNAL(setGeometry(int,int,int,int)),w->parentWidget(),SLOT(setGeometry(int,int,int,int)));

	id->setWindowTitle(tr("QtiPlot - Window Geometry"));
	id->setOrigin(w->parentWidget()->pos());
	id->setSize(w->parentWidget()->size());
	id->exec();
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

void ApplicationWindow::maximizeWindow(Q3ListViewItem * lbi)
{
	if (!lbi || lbi->rtti() == FolderListItem::RTTI)
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
	MyWidget *w= it->window();
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

	if (hiddenWindows->contains(w))
		hiddenWindows->takeAt(hiddenWindows->indexOf(w));
	else if (outWindows->contains(w))
	{
		outWindows->takeAt(outWindows->indexOf(w));		
		w->setParent(ws);
		w->setAttribute(Qt::WA_DeleteOnClose);
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
			actionUndo->setEnabled(false);
			actionRedo->setEnabled(false);
		}
	}
	else if (w->isA("TableStatistics"))
		tableWindows.remove(caption);
	else if (w->isA("MultiLayer"))
	{
		MultiLayer *ml =  (MultiLayer*)w;
		Graph *g = ml->activeGraph();

		if (g && (g->selectorsEnabled() || g->zoomOn() || g->removePointActivated() ||
					g->movePointsActivated() || g->enabledCursor()|| g->pickerActivated()))
		{
			btnPointer->setChecked(true);
			activeGraph = 0;
		}	
	}	
	else if (w->isA("Matrix"))
		remove3DMatrixPlots((Matrix*)w);

	if (hiddenWindows->contains(w))
		hiddenWindows->takeAt(hiddenWindows->indexOf(w));
	else if (outWindows->contains(w))
		outWindows->takeAt(outWindows->indexOf(w));
}

void ApplicationWindow::closeWindow(MyWidget* window)
{
	if (!window)
		return;

	removeWindowFromLists(window);
	window->folder()->removeWindow((MyWidget*)window);

	//update list view in project explorer
	Q3ListViewItem *it=lv->findItem (window->name(), 0, Q3ListView::ExactMatch|Q3ListView::CaseSensitive);
	if (it)
		lv->takeItem(it);

	delete window;
	emit modified();
}

void ApplicationWindow::about()
{
	QString version = "QtiPlot " + QString::number(majVersion) + "." +
		QString::number(minVersion) + "." + QString::number(patchVersion) + versionSuffix;

	QMessageBox::about(this,tr("About QtiPlot"),
			tr("<h2>"+ version + "</h2>"
				"<p><h3>Copyright(C): Ion Vasilief, Tilman Hoener zu Siederdissen, Knut Franke</h3>"
				"<p><h3>Released: not yet</h3>"));
}

void ApplicationWindow::windowsMenuAboutToShow()
{
	QList<QWidget*> windows = ws->windowList();
	int n=int(windows.count());	
	if (!n )
		return;

	windowsMenu->clear();
	windowsMenu->insertItem(tr("&Cascade"), ws, SLOT(cascade() ) );
	windowsMenu->insertItem(tr("&Tile"), ws, SLOT(tile() ) );
	windowsMenu->insertSeparator();
	windowsMenu->addAction(actionNextWindow);
	windowsMenu->addAction(actionPrevWindow);
	windowsMenu->insertSeparator();
	windowsMenu->addAction(actionRename);
	windowsMenu->addAction(actionCopyWindow);
	windowsMenu->insertSeparator();
	windowsMenu->addAction(actionResizeActiveWindow);
	windowsMenu->insertItem(tr("&Hide Window"),
			this, SLOT(hideActiveWindow()));
	windowsMenu->insertItem(QPixmap(close_xpm), tr("Close &Window"),
			this, SLOT(closeActiveWindow()), Qt::CTRL+Qt::Key_W );

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
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	QMenu markerMenu(this);

	if (g->imageMarkerSelected())
	{
		markerMenu.insertItem(QPixmap(pixelProfile_xpm),tr("&View Pixel Line profile"),this, SLOT(pixelLineProfile()));
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
	QList<QWidget*> windows = ws->windowList();
	QWidget* w = windows.at( id );
	if ( w )
	{
		w->showNormal();
		w->setFocus();
		if(hidden(w))
		{
			hiddenWindows->takeAt(hiddenWindows->indexOf(w));
			setListView(w->name(),tr("Normal"));
		}
	}
}

void ApplicationWindow::newProject()
{
	saveSettings();//the recent projects must be saved 

	ApplicationWindow *ed = new ApplicationWindow();
	ed->applyUserSettings();
	ed->newTable();

	if (this->isMaximized())
		ed->showMaximized();
	else
		ed->show();

	ed->savedProject();

	this->close();
}

void ApplicationWindow::savedProject()
{
	actionSaveProject->setEnabled(false);
	saved = true;
}

void ApplicationWindow::modifiedProject()
{
	actionSaveProject->setEnabled(true);
	saved = false;
}

void ApplicationWindow::modifiedProject(QWidget *w)
{
	modifiedProject();

	actionUndo->setEnabled(true);
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
	if (Q3UriDrag::decodeLocalFiles(e, fileNames))
	{
		QList<QByteArray> lst = QImageReader::supportedImageFormats();
		QStringList asciiFiles;

		for(int i = 0; i<(int)fileNames.count(); i++)
		{
			QString fn = fileNames[i];
			QFileInfo fi (fn);
			QString ext = fi.extension().lower();
			QStringList tempList;
			QByteArray temp;
			// convert QList<QByteArray> to QStringList to be able to 'filter'
			foreach(temp,lst)
				tempList.append(QString(temp));
			QStringList l = tempList.filter(ext, Qt::CaseInsensitive);
			if (l.count()>0)
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

	e->accept(Q3UriDrag::canDecode(e));
}

void ApplicationWindow::closeEvent( QCloseEvent* ce )
{
	if (!saved)
	{
		QString s= tr("Save changes to project: <p><b> %1 </b> ?").arg(projectname);
		switch( QMessageBox::information(this, tr("QtiPlot"), s, tr("Yes"), tr("No"), 
					tr("Cancel"), 0, 2 ) )
		{
			case 0:
				if (!saveProject())
				{
					ce->ignore();
					break;
				}
				saveSettings();//the recent projects must be saved 
				ce->accept();
				break;

			case 1:
			default:
				saveSettings();//the recent projects must be saved 
				ce->accept();
				break;

			case 2:
				ce->ignore();
				break;
		}
	}
	else
	{
		saveSettings();//the recent projects must be saved 
		ce->accept();
	}
}

void ApplicationWindow::customEvent(QEvent *e)
{
	if (e->type() == SCRIPTING_CHANGE_EVENT)
	{
		scriptingChangeEvent((ScriptingChangeEvent*)e);
		connect(scriptEnv,SIGNAL(error(const QString&,const QString&,int)),this,SLOT(scriptError(const QString&,const QString&,int)));
	}
}

void ApplicationWindow::deleteSelectedItems()
{
	if (folders->hasFocus() && folders->currentItem() != folders->firstChild())
	{//we never allow the user to delete the project folder item
		deleteFolder();
		return;
	}

	Q3ListViewItem *item;
	QList<Q3ListViewItem *> lst;
	for (item = lv->firstChild(); item; item = item->nextSibling())
	{
		if (item->isSelected())
			lst.append(item);
	}

	folders->blockSignals(true);
	foreach(item, lst)
	{
		if (item->rtti() == FolderListItem::RTTI)
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
	QMenu cm(this);
	cm.insertItem(tr("&Delete Selection"), this, SLOT(deleteSelectedItems()), Qt::Key_F8);
	cm.exec(p);
}

void ApplicationWindow::showListViewPopupMenu(const QPoint &p)
{
	QMenu cm(this);
	QMenu window(this);

	window.addAction(actionNewTable);
	window.addAction(actionNewMatrix);
	window.addAction(actionNewNote);
	window.addAction(actionNewGraph);
	window.addAction(actionNewFunctionPlot);
	window.addAction(actionNewSurfacePlot);
	cm.insertItem(tr("New &Window"), &window);

	cm.insertItem(QPixmap(newfolder_xpm), tr("New F&older"), this, SLOT(addFolder()), Qt::Key_F7);
	cm.insertSeparator();
	cm.insertItem(tr("Auto &Column Width"), lv, SLOT(adjustColumns()));
	cm.exec(p);
}

void ApplicationWindow::showWindowPopupMenu(Q3ListViewItem *it, const QPoint &p, int)
{
	if (folders->isRenaming())
		return;

	if (!it) 
	{
		showListViewPopupMenu(p);
		return;
	}

	Q3ListViewItem *item;
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

	if (it->rtti() == FolderListItem::RTTI)
	{
		current_folder = ((FolderListItem *)it)->folder();
		showFolderPopupMenu(it, p, false);
		return;
	}

	MyWidget *w= ((WindowListItem *)it)->window();
	if (w)
	{
		QMenu cm(this);
		QMenu plots(this);

		cm.addAction(actionActivateWindow);
		cm.addAction(actionMinimizeWindow);
		cm.addAction(actionMaximizeWindow);
		cm.insertSeparator();
		if (!hidden(w))
			cm.addAction(actionHideWindow);
		cm.insertItem(QPixmap(close_xpm), tr("&Delete Window"), w, SLOT(close()), Qt::Key_F8);
		cm.insertSeparator();
		cm.insertItem(tr("&Rename Window"), this, SLOT(renameWindow()), Qt::Key_F2);
		cm.addAction(actionResizeWindow);
		cm.insertSeparator();
		cm.addAction(actionPrintWindow);
		cm.insertSeparator();
		cm.insertItem(tr("&Properties..."), this, SLOT(windowProperties()));

		if (w->isA("Table"))
		{
			QStringList graphs = dependingPlots(w->name());
			if (int(graphs.count())>0)
			{
				cm.insertSeparator();
				for (int i=0;i<int(graphs.count());i++)
					plots.insertItem(graphs[i], window(graphs[i]), SLOT(showMaximized()));

				cm.insertItem(tr("D&epending Graphs"),&plots);
			}
		}
		else if (w->isA("Matrix"))
		{
			QStringList graphs = depending3DPlots((Matrix*)w);
			if (int(graphs.count())>0)
			{
				cm.insertSeparator();
				for (int i=0;i<int(graphs.count());i++)
					plots.insertItem(graphs[i], window(graphs[i]), SLOT(showMaximized()));

				cm.insertItem(tr("D&epending 3D Graphs"),&plots);
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
	Q3ListViewItem *it=lv->findItem (t->name(), 0, Q3ListView::ExactMatch | Qt::CaseSensitive );
	if (it)
		it->setText(2,tr("Maximized"));
}

void ApplicationWindow::showTable(const QString& curve)
{
	Table* w=table(curve);
	if (!w)
		return;

	updateWindowLists(w);
	int colIndex = w->colIndex(curve);
	w->setSelectedCol(colIndex);
	w->table()->clearSelection();
	w->table()->selectColumn(colIndex);
	w->showMaximized();
	Q3ListViewItem *it=lv->findItem (w->name(), 0, Q3ListView::ExactMatch | Qt::CaseSensitive );
	if (it)
		it->setText(2,tr("Maximized"));
	emit modified();
}

QStringList ApplicationWindow::depending3DPlots(Matrix *m)
{
	QWidgetList *windows = windowsList();
	QStringList plots;
	for (int i=0; i<(int)windows->count(); i++)
	{
		QWidget *w = windows->at(i);
		if (w->isA("Graph3D") && ((Graph3D *)w)->getMatrix() == m)
			plots << w->name();
	}
	delete windows;
	return plots;
}

QStringList ApplicationWindow::dependingPlots(const QString& name)
{
	QWidgetList *windows = windowsList();
	QStringList onPlot, plots;

	for (int i=0; i<(int)windows->count(); i++)
	{
		QWidget *w = windows->at(i);
		if (w->isA("MultiLayer"))
		{
			QWidgetList lst= ((MultiLayer*)w)->graphPtrs();
			foreach(QWidget *widget, lst)
			{	
				Graph *g = (Graph *)widget;
				onPlot = g->curvesList();
				onPlot = onPlot.grep (name,TRUE);
				if (int(onPlot.count()) && plots.contains(w->name())<=0)
					plots << w->name();
			}
		}
		else if (w->isA("Graph3D"))
		{
			if ((((Graph3D*)w)->formula()).contains(name,TRUE) && plots.contains(w->name())<=0)
				plots << w->name();
		}
	}
	delete windows;
	return plots;
}

QStringList ApplicationWindow::multilayerDependencies(QWidget *w)
{
	QStringList tables;
	MultiLayer *g=(MultiLayer*)w;
	QWidgetList graphsList = g->graphPtrs();
	for (int i=0; i<graphsList.count(); i++)
	{
		Graph* ag=(Graph*)graphsList.at(i);
		QStringList onPlot = ag->curvesList();
		for (int j=0; j<onPlot.count(); j++)
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
		QMenu cm(this);
		QMenu exports(this);
		QMenu copy(this);
		QMenu prints(this);	
		QMenu calcul(this);
		QMenu smooth(this);
		QMenu filter(this);
		QMenu decay(this);
		QMenu translate(this);
		QMenu multiPeakMenu(this);

		Graph* ag = (Graph*)plot->activeGraph();

		if (ag->isPiePlot())
			cm.insertItem(tr("Re&move Pie Curve"),ag, SLOT(removePie()));
		else
		{
			cm.addAction(actionShowCurvesDialog);

			translate.addAction(actionTranslateVert);
			translate.addAction(actionTranslateHor);
			calcul.insertItem(tr("&Translate"),&translate);
			calcul.insertSeparator();

			calcul.addAction(actionDifferentiate);
			calcul.addAction(actionShowIntDialog);
			calcul.insertSeparator();
			smooth.addAction(actionSmoothSavGol);
			smooth.addAction(actionSmoothFFT);
			smooth.addAction(actionSmoothAverage);
			calcul.insertItem(tr("&Smooth"), &smooth);

			filter.addAction(actionLowPassFilter);
			filter.addAction(actionHighPassFilter);
			filter.addAction(actionBandPassFilter);
			filter.addAction(actionBandBlockFilter);
			calcul.insertItem(tr("&FFT Filter"),&filter);
			calcul.insertSeparator();
			calcul.addAction(actionInterpolate);
			calcul.addAction(actionFFT);
			calcul.insertSeparator();
			calcul.addAction(actionFitLinear);
			calcul.addAction(actionShowFitPolynomDialog);
			calcul.insertSeparator();
			decay.addAction(actionShowExpDecayDialog);
			decay.addAction(actionShowTwoExpDecayDialog);
			decay.addAction(actionShowExpDecay3Dialog);
			calcul.insertItem(tr("Fit E&xponential Decay"), &decay);
			calcul.addAction(actionFitExpGrowth);
			calcul.addAction(actionFitSigmoidal);
			calcul.addAction(actionFitGauss);
			calcul.addAction(actionFitLorentz);

			multiPeakMenu.addAction(actionMultiPeakGauss);
			multiPeakMenu.addAction(actionMultiPeakLorentz);
			calcul.insertItem(tr("Fit &Multi-Peak"), &multiPeakMenu);
			calcul.insertSeparator();
			calcul.addAction(actionShowFitDialog);
			cm.insertItem(tr("Anal&yze"), &calcul);
		}

		if (copiedLayer)
		{
			cm.insertSeparator();
			cm.insertItem(QPixmap(paste_xpm), tr("&Paste Layer"),this, SLOT(pasteSelection()));
		}
		else if (copiedMarkerType >=0 )
		{
			cm.insertSeparator();
			if (copiedMarkerType == Graph::Text )
				cm.insertItem(QPixmap(paste_xpm),tr("&Paste Text"),plot, SIGNAL(pasteMarker()));
			else if (copiedMarkerType == Graph::Arrow )
				cm.insertItem(QPixmap(paste_xpm),tr("&Paste Line/Arrow"),plot, SIGNAL(pasteMarker()));
			else if (copiedMarkerType == Graph::Image )
				cm.insertItem(QPixmap(paste_xpm),tr("&Paste Image"),plot, SIGNAL(pasteMarker()));
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

	QMenu cm(this);
	QMenu plot3D(this);
	if (w->isA("MultiLayer"))
	{
		MultiLayer *g=(MultiLayer*)w;
		if (copiedLayer)
		{
			cm.insertItem(QPixmap(paste_xpm),tr("&Paste Layer"),this, SLOT(pasteSelection()));
			cm.insertSeparator();
		}

		cm.addAction(actionAddLayer);	
		cm.insertSeparator();
		if (g->graphsNumber() != 0)
		{
			cm.addAction(actionDeleteLayer);
			cm.insertSeparator();
			cm.addAction(actionShowPlotGeometryDialog);
			cm.addAction(actionShowLayerDialog);
			cm.insertSeparator();
		}
		cm.addAction(actionRename);
		cm.addAction(actionCopyWindow);
		cm.insertSeparator();
		cm.insertItem(QPixmap(copy_xpm),tr("&Copy Page"), g, SLOT(copyAllLayers()));
		cm.insertItem(tr("E&xport Page"), this, SLOT(exportGraph()));
		cm.addAction(actionPrint);
		cm.insertSeparator();
		cm.addAction(actionCloseWindow);
	}
	else if (w->isA("Graph3D"))
	{
		Graph3D *g=(Graph3D*)w;
		if (!g->hasData())
		{
			cm.insertItem(tr("3D &Plot"), &plot3D);
			plot3D.addAction(actionAdd3DData);
			plot3D.insertItem(tr("&Matrix..."), this, SLOT(add3DMatrixPlot()));
			plot3D.addAction(actionEditSurfacePlot);
		}
		else
		{
			if (g->getTable())
				cm.insertItem(tr("Choose &Data Set..."), this, SLOT(change3DData()));
			else if (g->getMatrix())
				cm.insertItem(tr("Choose &Matrix..."), this, SLOT(change3DMatrix()));
			else if (g->userFunction())
				cm.addAction(actionEditSurfacePlot);
			cm.insertItem(QPixmap(erase_xpm), tr("C&lear"), g, SLOT(clearData()));
		}

		cm.insertSeparator();
		cm.addAction(actionRename);
		cm.addAction(actionCopyWindow);
		cm.insertSeparator();
		cm.insertItem(tr("&Copy Graph"), g, SLOT(copyImage()));
		cm.insertItem(tr("&Export"), g, SLOT(saveImage()));
		cm.addAction(actionPrint);
		cm.insertSeparator();
		cm.addAction(actionCloseWindow);
	}
	else if (w->isA("Matrix"))
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

void ApplicationWindow::showTableContextMenu(bool selection)
{
	Table *t = (Table*)ws->activeWindow();
	if (!t)
		return;

	QMenu cm(this);
	if (selection)
	{
		if ((int)t->selectedColumns().count() > 0) 
		{
			showColMenu(t->firstSelectedColumn());
			return;
		}
		else if (t->singleRowSelected())
		{
			cm.insertItem(QPixmap(cut_xpm),tr("Cu&t"), t, SLOT(cutSelection()));
			cm.insertItem(QPixmap(copy_xpm),tr("&Copy"), t, SLOT(copySelection()));
			cm.insertItem(QPixmap(paste_xpm),tr("&Paste"), t, SLOT(pasteSelection()));
			cm.insertSeparator();
			cm.addAction(actionTableRecalculate);
			cm.insertItem(tr("&Insert Row"), t, SLOT(insertRow()));
			cm.insertItem(QPixmap(close_xpm), tr("&Delete Row"), t, SLOT(deleteSelectedRows()));
			cm.insertItem(QPixmap(erase_xpm),tr("Clea&r Row"), t, SLOT(clearSelection()));
			cm.insertSeparator();
			cm.addAction(actionShowRowStatistics);
		}
		else if (t->multipleRowsSelected())
		{
			cm.insertItem(QPixmap(cut_xpm),tr("Cu&t"), t, SLOT(cutSelection()));
			cm.insertItem(QPixmap(copy_xpm),tr("&Copy"), t, SLOT(copySelection()));
			cm.insertItem(QPixmap(paste_xpm),tr("&Paste"), t, SLOT(pasteSelection()));
			cm.insertSeparator();
			cm.addAction(actionTableRecalculate);
			cm.insertItem(QPixmap(close_xpm), tr("&Delete Rows"), t, SLOT(deleteSelectedRows()));
			cm.insertItem(QPixmap(erase_xpm),tr("Clea&r Rows"), t, SLOT(clearSelection()));
			cm.insertSeparator();
			cm.addAction(actionShowRowStatistics);
		}
		else
		{
			cm.insertItem(QPixmap(cut_xpm),tr("Cu&t"), t, SLOT(cutSelection()));
			cm.insertItem(QPixmap(copy_xpm),tr("&Copy"), t, SLOT(copySelection()));
			cm.insertItem(QPixmap(paste_xpm),tr("&Paste"), t, SLOT(pasteSelection()));
			cm.insertSeparator();
			cm.addAction(actionTableRecalculate);
			cm.insertItem(QPixmap(erase_xpm),tr("Clea&r"), t, SLOT(clearSelection()));
		}	
	}
	else
	{
		cm.addAction(actionRename);
		cm.addAction(actionCopyWindow);
		cm.insertSeparator();
		cm.addAction(actionShowExportASCIIDialog);
		cm.addAction(actionPrint);
		cm.insertSeparator();
		cm.addAction(actionCloseWindow);
	}
	cm.exec(QCursor::pos());
}

void ApplicationWindow::chooseHelpFolder()
{
	QString dir = Q3FileDialog::getExistingDirectory(
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
	QMainWindow *helpWindow = new QMainWindow();
	helpWindow->setAttribute(Qt::WA_DeleteOnClose);
	helpWindow->setWindowTitle(tr("QtiPlot - Help Browser"));
	helpWindow->resize(QSize(800, 600));
	helpWindow->setIcon(QPixmap(logo_xpm));
	
    HelpBrowser *browser = new HelpBrowser (helpWindow);
	helpWindow->setFocus();
	helpWindow->setCentralWidget(browser);
	
	QToolBar* toolbar = new QToolBar( helpWindow );
	helpWindow->addToolBar( toolbar );
	QAction *button = toolbar->addAction(QIcon(QPixmap(folder_open_xpm)), tr("Open File"), browser, SLOT(open()));
	button->setAccel(tr("Ctrl+O"));
	button = toolbar->addAction(QIcon(QPixmap(fileprint_modern_xpm)), tr("Print"), browser, SLOT(print()));
	button->setAccel(tr("Ctrl+P"));
	button = toolbar->addAction(QIcon(QPixmap(export_pdf_xpm)), tr("Export PDF"), browser, SLOT(exportPdf()));
	button->setAccel(tr("Ctrl+E"));
	toolbar->addSeparator();
	button = toolbar->addAction(QIcon(QPixmap(prev_xpm)), tr("Backward"), browser, SLOT(backward()));
    connect( browser, SIGNAL( backwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( false );
	button = toolbar->addAction(QIcon(QPixmap(next_xpm)), tr("Forward"), browser, SLOT(forward()));
    connect( browser, SIGNAL( forwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( false );
   	toolbar->addAction(QIcon(QPixmap(home_xpm)), tr("Home"), browser, SLOT(home()));

    QFile helpFile(helpFilePath);
	if (!helpFile.exists())
		{
		QMessageBox::critical(this,tr("QtiPlot - Help Files Not Found!"),
			   tr("Please indicate the location of the help file!")+"<br>"+
			   tr("The manual can be downloaded from the following internet address:")+
			   "<p><a href = http://soft.proindependent.com/manuals.html>http://soft.proindependent.com/manuals.html</a></p>");
		QString fn = QFileDialog::getOpenFileName(QDir::currentDirPath(), "*.html", this );
		if (!fn.isEmpty())
			{
			QFileInfo fi(fn);
			helpFilePath = fi.absFilePath();
			saveSettings();
			}
		}		
	browser->setSource(QUrl::fromLocalFile(helpFilePath));
	helpWindow->show();
}

void ApplicationWindow::showPlotWizard()
{
	if (tableWindows.count()>0)
	{
		PlotWizard* pw = new PlotWizard(this, 0);
		pw->setAttribute(Qt::WA_DeleteOnClose);
		connect (pw,SIGNAL(plot(const QStringList&)),this,SLOT(multilayerPlot(const QStringList&)));
		connect (pw,SIGNAL(plot3D(const QString&)),this,SLOT(dataPlotXYZ(const QString&)));
		connect (pw,SIGNAL(plot3DRibbon(const QString&)),this,SLOT(dataPlot3D(const QString&)));

		pw->insertTablesList(tableWindows);
		pw->setColumnsList(columnsList(Table::All));
		pw->changeColumnsList(tableWindows[0]);
		pw->exec();
	}
	else
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no tables available in this project.</h4>"
					"<p><h4>Please create a table and try again!</h4>"));
}

void ApplicationWindow::showFunctionDialog(int curveKey)
{	
	if ( !activeGraph )
		return;
	showFunctionDialog(activeGraph, activeGraph->curveIndex(curveKey));
}

void ApplicationWindow::showFunctionDialog(Graph *g, int curve)
{	
	if ( !g )
		return;

	FunctionDialog* fd = functionDialog();
	fd->setWindowTitle(tr("QtiPlot - Edit function"));
	fd->setCurveToModify(g, curve);
}

FunctionDialog* ApplicationWindow::functionDialog()
{
	FunctionDialog* fd= new FunctionDialog(this,"FunctionDialog",true,0);
	fd->setAttribute(Qt::WA_DeleteOnClose);
	connect (fd,SIGNAL(clearParamFunctionsList()),this,SLOT(clearParamFunctionsList()));
	connect (fd,SIGNAL(clearPolarFunctionsList()),this,SLOT(clearPolarFunctionsList()));

	fd->insertParamFunctionsList(xFunctions, yFunctions);
	fd->insertPolarFunctionsList(rFunctions, tetaFunctions);
	fd->exec();

	return fd;
}

void ApplicationWindow::addFunctionCurve()
{
	QWidget* w = ws->activeWindow();
	if (!w || !w->isA("MultiLayer"))
		return;

	if (((MultiLayer*)w)->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		return;
	}

	Graph* g = ((MultiLayer*)w)->activeGraph();
	if ( g )
	{
		activeGraph=g;
		FunctionDialog* fd = functionDialog();
		if (fd)
			fd->setGraph(g);
	}
}

void ApplicationWindow::updateFunctionLists(int type, QStringList &formulas)
{
	int maxListSize = 10;
	if (type == 2)
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
	else if (type == 1)
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
}

void ApplicationWindow::newFunctionPlot()
{
	FunctionDialog* fd = functionDialog();
	if (fd)
		connect (fd,SIGNAL(newFunctionPlot(int, QStringList &, const QString&, QValueList<double> &, int )),
				this,SLOT(newFunctionPlot(int, QStringList &, const QString&, QValueList<double> &, int)));

}

void ApplicationWindow::newFunctionPlot(int type,QStringList &formulas, const QString& var, QList<double> &ranges, int points)
{
	QString label="graph"+QString::number(++graphs);
	while(alreadyUsedName(label)){
		label="graph"+QString::number(++graphs);}

	MultiLayer* plot = multilayerPlot(label);
	Graph* g=plot->addLayer();
	customGraph(g);
	g->addFunctionCurve(type,formulas, var,ranges,points);

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
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
	{
		((Graph3D*)ws->activeWindow())->setFramed();
		actionShowAxisDialog->setEnabled(TRUE);
	}
}

void ApplicationWindow::setBoxed3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
	{
		((Graph3D*)ws->activeWindow())->setBoxed();
		actionShowAxisDialog->setEnabled(TRUE);
	}
}

void ApplicationWindow::removeAxes3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
	{
		((Graph3D*)ws->activeWindow())->setNoAxes();
		actionShowAxisDialog->setEnabled(false);
	}
}

void ApplicationWindow::removeGrid3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setNoGrid();
}

void ApplicationWindow::setHiddenLineGrid3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setHiddenLineGrid();
}

void ApplicationWindow::setPoints3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setPointsMesh();
}

void ApplicationWindow::setCones3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setConesMesh();
}

void ApplicationWindow::setCrosses3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setCrossMesh();
}

void ApplicationWindow::setBars3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setBarsPlot();
}

void ApplicationWindow::setLineGrid3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setLineGrid();
}

void ApplicationWindow::setFilledMesh3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setFilledMesh();
}

void ApplicationWindow::setFloorData3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setFloorData();
}

void ApplicationWindow::setFloorIso3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setFloorIsolines();
}

void ApplicationWindow::setEmptyFloor3DPlot()
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setEmptyFloor();
}

void ApplicationWindow::setFrontGrid3DPlot(bool on)
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setFrontGrid(on);
}

void ApplicationWindow::setBackGrid3DPlot(bool on)
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setBackGrid(on);
}

void ApplicationWindow::setFloorGrid3DPlot(bool on)
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setFloorGrid(on);
}

void ApplicationWindow::setCeilGrid3DPlot(bool on)
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setCeilGrid(on);
}

void ApplicationWindow::setRightGrid3DPlot(bool on)
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setRightGrid(on);
}

void ApplicationWindow::setLeftGrid3DPlot(bool on)
{
	if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
		((Graph3D*)ws->activeWindow())->setLeftGrid(on);
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
	if (w && w->isA("Graph3D"))
	{
		Graph3D* plot= (Graph3D*)w;
		actionAnimate->setOn(plot->isAnimated());
		switch(plot->plotStyle())
		{
			case FILLEDMESH:
				wireframe->setChecked( false );
				hiddenline->setChecked( false );
				polygon->setChecked( false );
				filledmesh->setChecked( true );
				pointstyle->setChecked( false );
				barstyle->setChecked( false );
				conestyle->setChecked( false );
				crossHairStyle->setChecked( false );
				break;

			case FILLED:
				wireframe->setChecked( false );
				hiddenline->setChecked( false );
				polygon->setChecked( true );
				filledmesh->setChecked( false );
				pointstyle->setChecked( false );
				barstyle->setChecked( false );
				conestyle->setChecked( false );
				crossHairStyle->setChecked( false );
				break;

			case Qwt3D::USER:
				wireframe->setChecked( false );
				hiddenline->setChecked( false );
				polygon->setChecked( false );
				filledmesh->setChecked( false );

				if (plot->pointType() == Graph3D::VerticalBars)
				{
					pointstyle->setChecked( false );
					conestyle->setChecked( false );
					crossHairStyle->setChecked( false );
					barstyle->setChecked( true );
				}
				else if (plot->pointType() == Graph3D::Dots)
				{
					pointstyle->setChecked( true );
					barstyle->setChecked( false );
					conestyle->setChecked( false );
					crossHairStyle->setChecked( false );
				}
				else if (plot->pointType() == Graph3D::HairCross)
				{
					pointstyle->setChecked( false );
					barstyle->setChecked( false );
					conestyle->setChecked( false );
					crossHairStyle->setChecked( true );
				}
				else if (plot->pointType() == Graph3D::Cones)
				{
					pointstyle->setChecked( false );
					barstyle->setChecked( false );
					conestyle->setChecked( true );
					crossHairStyle->setChecked( false );
				}
				break;

			case WIREFRAME:
				wireframe->setChecked( true );
				hiddenline->setChecked( false );
				polygon->setChecked( false );
				filledmesh->setChecked( false );
				pointstyle->setChecked( false );
				barstyle->setChecked( false );
				conestyle->setChecked( false );
				crossHairStyle->setChecked( false );
				break;

			case HIDDENLINE:
				wireframe->setChecked( false );
				hiddenline->setChecked( true );
				polygon->setChecked( false );
				filledmesh->setChecked( false );
				pointstyle->setChecked( false );
				barstyle->setChecked( false );
				conestyle->setChecked( false );
				crossHairStyle->setChecked( false );
				break;

			default:
				break;
		}

		switch(plot->coordStyle())
		{
			case Qwt3D::NOCOORD:
				None->setChecked( true );
				Box->setChecked( false );
				Frame->setChecked( false );
				break;

			case Qwt3D::BOX:
				None->setChecked( false );
				Box->setChecked( true );
				Frame->setChecked( false );
				break;

			case Qwt3D::FRAME:
				None->setChecked(false );
				Box->setChecked( false );
				Frame->setChecked(true );
				break;
		}

		switch(plot->floorStyle())
		{
			case NOFLOOR:
				floornone->setChecked( true );
				flooriso->setChecked( false );
				floordata->setChecked( false );
				break;

			case FLOORISO:
				floornone->setChecked( false );
				flooriso->setChecked( true );
				floordata->setChecked( false );
				break;

			case FLOORDATA:
				floornone->setChecked(false );
				flooriso->setChecked( false );
				floordata->setChecked(true );
				break;
		}
		custom3DGrids(plot->grids());
	}
}

void ApplicationWindow::custom3DGrids(int grids)
{
	if (Qwt3D::BACK & grids)
		back->setChecked(true);
	else
		back->setChecked(false);

	if (Qwt3D::FRONT & grids)
		front->setChecked(true);
	else
		front->setChecked(false);

	if (Qwt3D::CEIL & grids)
		ceil->setChecked(true);
	else
		ceil->setChecked(false);

	if (Qwt3D::FLOOR & grids)
		floor->setChecked(true);
	else
		floor->setChecked(false);

	if (Qwt3D::RIGHT & grids)
		right->setChecked(true);
	else
		right->setChecked(false);

	if (Qwt3D::LEFT & grids)
		left->setChecked(true);
	else
		left->setChecked(false);
}

void ApplicationWindow::initPlot3DToolBar()
{
	plot3DTools = new QToolBar( tr( "3D Surface" ), this );
	plot3DTools->setObjectName("plot3DTools"); // this is needed for QMainWindow::restoreState()
	plot3DTools->setIconSize( QSize(20,20) );
	addToolBarBreak( Qt::TopToolBarArea );
	addToolBar( Qt::TopToolBarArea, plot3DTools );

	coord = new QActionGroup( this );
	Box = new QAction( coord );
	Box->setIcon(QIcon(QPixmap(box_xpm)));
	Box->setCheckable(true);

	Frame = new QAction( coord );
	Frame->setIcon(QIcon(QPixmap(free_axes_xpm)) );
	Frame->setCheckable(true);

	None = new QAction( coord );
	None->setIcon(QIcon(QPixmap(no_axes_xpm)) );
	None->setCheckable(true);

	plot3DTools->addAction(Frame);
	plot3DTools->addAction(Box);
	plot3DTools->addAction(None);
	Box->setChecked( true );

	plot3DTools->addSeparator();

	// grid actions
	grids = new QActionGroup( this );
	grids->setEnabled( true );
	grids->setExclusive( false );
	front = new QAction( grids );
	front->setCheckable( true );
	front->setIcon(QIcon(QPixmap(frontGrid_xpm)) );
	back = new QAction( grids );
	back->setCheckable( true );
	back->setIcon(QIcon(QPixmap(backGrid_xpm)));
	right = new QAction( grids );
	right->setCheckable( true );
	right->setIcon(QIcon(QPixmap(leftGrid_xpm)) );
	left = new QAction( grids );
	left->setCheckable( true );
	left->setIcon(QIcon(QPixmap(rightGrid_xpm)));
	ceil = new QAction( grids );
	ceil->setCheckable( true );
	ceil->setIcon(QIcon(QPixmap(ceilGrid_xpm)) );
	floor = new QAction( grids );
	floor->setCheckable( true );
	floor->setIcon(QIcon(QPixmap(floorGrid_xpm)) );

	plot3DTools->addAction(front);
	plot3DTools->addAction(back);
	plot3DTools->addAction(right);
	plot3DTools->addAction(left);
	plot3DTools->addAction(ceil);
	plot3DTools->addAction(floor);

	plot3DTools->addSeparator();

	//plot style actions
	plotstyle = new QActionGroup( this );
	wireframe = new QAction( plotstyle );
	wireframe->setCheckable( true );
	wireframe->setEnabled( true );
	wireframe->setIcon(QIcon(QPixmap(lineMesh_xpm)) );
	hiddenline = new QAction( plotstyle );
	hiddenline->setCheckable( true );
	hiddenline->setEnabled( true );
	hiddenline->setIcon(QIcon(QPixmap(grid_only_xpm)) );
	polygon = new QAction( plotstyle );
	polygon->setCheckable( true );
	polygon->setEnabled( true );
	polygon->setIcon(QIcon(QPixmap(no_grid_xpm)));
	filledmesh = new QAction( plotstyle );
	filledmesh->setCheckable( true );
	filledmesh->setIcon(QIcon(QPixmap(grid_poly_xpm)) );
	pointstyle = new QAction( plotstyle );
	pointstyle->setCheckable( true );
	pointstyle->setIcon(QIcon(QPixmap(pointsMesh_xpm)) );

	conestyle = new QAction( plotstyle );
	conestyle->setCheckable( true );
	conestyle->setIcon(QIcon(QPixmap(cones_xpm)) );

	crossHairStyle = new QAction( plotstyle );
	crossHairStyle->setCheckable( true );
	crossHairStyle->setIcon(QIcon(QPixmap(crosses_xpm)) );

	barstyle = new QAction( plotstyle );
	barstyle->setCheckable( true );
	barstyle->setIcon(QIcon(QPixmap(plot_bars_xpm)) );

	plot3DTools->addAction(barstyle);
	plot3DTools->addSeparator();
	plot3DTools->addAction(pointstyle);

	plot3DTools->addAction(conestyle);
	plot3DTools->addAction(crossHairStyle);
	plot3DTools->addSeparator();

	plot3DTools->addAction(wireframe);
	plot3DTools->addAction(hiddenline);
	plot3DTools->addAction(polygon);
	plot3DTools->addAction(filledmesh);
	filledmesh->setChecked( true );

	plot3DTools->addSeparator();

	//floor actions
	floorstyle = new QActionGroup( this );
	floordata = new QAction( floorstyle );
	floordata->setCheckable( true );
	floordata->setIcon(QIcon(QPixmap(floor_xpm)) );
	flooriso = new QAction( floorstyle );
	flooriso->setCheckable( true );
	flooriso->setIcon(QIcon(QPixmap(isolines_xpm)) );
	floornone = new QAction( floorstyle );
	floornone->setCheckable( true );
	floornone->setIcon(QIcon(QPixmap(no_floor_xpm)));

	plot3DTools->addAction(floordata);
	plot3DTools->addAction(flooriso);
	plot3DTools->addAction(floornone);
	floornone->setChecked( true );

    plot3DTools->addSeparator();
    
    actionAnimate = new QAction( this );
    actionAnimate->setToggleAction( true );
    actionAnimate->setIconSet(QPixmap(movie_xpm));
    plot3DTools->addAction(actionAnimate);
  	        
	plot3DTools->hide();

    connect(actionAnimate, SIGNAL(toggled(bool)), this, SLOT(toggle3DAnimation(bool)));
	connect( coord, SIGNAL( triggered( QAction* ) ), this, SLOT( pickCoordSystem( QAction* ) ) );
	connect( floorstyle, SIGNAL( triggered( QAction* ) ), this, SLOT( pickFloorStyle( QAction* ) ) );
	connect( plotstyle, SIGNAL( triggered( QAction* ) ), this, SLOT( pickPlotStyle( QAction* ) ) );

	connect( left, SIGNAL( triggered( bool ) ), this, SLOT( setLeftGrid3DPlot(bool) ));
	connect( right, SIGNAL( triggered( bool ) ), this, SLOT( setRightGrid3DPlot( bool ) ) );
	connect( ceil, SIGNAL( triggered( bool ) ), this, SLOT( setCeilGrid3DPlot( bool ) ) );
	connect( floor, SIGNAL( triggered( bool ) ), this, SLOT(setFloorGrid3DPlot( bool ) ) );
	connect( back, SIGNAL( triggered( bool ) ), this, SLOT(setBackGrid3DPlot( bool ) ) );
	connect( front, SIGNAL( triggered( bool ) ), this, SLOT( setFrontGrid3DPlot( bool ) ) );
}

void ApplicationWindow::pixelLineProfile()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer *)ws->activeWindow())->activeGraph();
	if (g)
	{
		bool ok;
		int res = QInputDialog::getInteger(
				tr("QtiPlot - Set the number of pixels to average"), tr("Number of averaged pixels"),1, 1, 2000, 2,
				&ok, this );
		if ( ok )
		{
			QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
			g->calculateProfile(res,true);
			QApplication::restoreOverrideCursor();
		}
		else
			return;
	}
}

void ApplicationWindow::intensityTable()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer *)ws->activeWindow())->activeGraph();
	if (g)
		g->showIntensityTable();
}

Matrix* ApplicationWindow::createIntensityMatrix(const QPixmap& pic)
{
	QImage image=pic.convertToImage();
	QSize size=pic.size();
	int cols=size.width();
	int rows=size.height();

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
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

void ApplicationWindow::autoArrangeLayers()
{
if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
   return;
  	 
MultiLayer* plot = (MultiLayer *)ws->activeWindow();
plot->setMargins(5, 5, 5, 5);
plot->setSpacing(5, 5);
plot->arrangeLayers(true, false);
}
  	
void ApplicationWindow::addLayer()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	MultiLayer* plot = (MultiLayer *)ws->activeWindow();
	switch(QMessageBox::information(this,
				tr("QtiPlot - Guess best origin for the new layer?"),
				tr("Do you want QtiPlot to guess the best position for the new layer?\n Warning: this will rearrange existing layers!"),
				tr("&Guess"), tr("&Top-left corner"), tr("&Cancel"), 0, 2 ) )
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

void ApplicationWindow::deleteLayer()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	((MultiLayer *)ws->activeWindow())->confirmRemoveLayer();
}

void ApplicationWindow::restoreWindowGeometry(ApplicationWindow *app, QWidget *w, const QString s)
{
	w->blockSignals (true);
	QString caption = w->name();
	if (s.contains ("minimized"))
	{
		w->parentWidget()->setGeometry(0, 0, 500, 400);
		w->showMinimized();
		((MyWidget *)w)->setStatus(MyWidget::Minimized);
		app->setListView(caption, tr("Minimized"));
	}
	else if (s.contains ("maximized"))
	{
		w->parentWidget()->setGeometry(0, 0, 500, 400);
		if (w->isA("MultiLayer"))
		   w->hide();//trick used in order to avoid a resize event
		w->showMaximized();
		((MyWidget *)w)->setStatus(MyWidget::Maximized);
		app->setListView(caption, tr("Maximized"));
	}
	else
	{
		QStringList lst=QStringList::split ("\t",s,true);
		w->showNormal();
		w->parentWidget()->setGeometry(lst[1].toInt(),lst[2].toInt(),lst[3].toInt(),lst[4].toInt());
		((MyWidget *)w)->setStatus(MyWidget::Normal);

		if (lst[5] == "hidden")
			hideWindow((MyWidget* )w);
	}

	if (s.contains ("active"))
		app->aw=(MyWidget*)w;

	w->blockSignals (false);
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
	w->setCaptionPolicy((MyWidget::CaptionPolicy)lst[2].toInt());
	app->setListViewLabel(w->name(), lst[1]);
	return w;
}

Matrix* ApplicationWindow::openMatrix(ApplicationWindow* app, const QStringList &flist)
{
	QStringList::const_iterator line = flist.begin();

	QStringList list=QStringList::split ("\t",*line,TRUE);
	QString caption=list[0];
	int rows = list[1].toInt();
	int cols = list[2].toInt();

	Matrix* w = app->newMatrix(caption, rows, cols);
	app->setListViewDate(caption,list[3]);
	w->setBirthDate(list[3]);

	if (caption.contains ("Matrix"))
	{
		bool ok;
		int tb=caption.remove("Matrix").toInt(&ok);
		if (tb > app->matrixes && ok) 
			app->matrixes = tb;
	}

	for (line++; line!=flist.end(); line++)
	{
		QStringList fields = QStringList::split("\t",*line,true);
		if (fields[0] == "geometry") {
			restoreWindowGeometry(app, (QWidget *)w, *line);
		} else if (fields[0] == "ColWidth") {
			w->setColumnsWidth(fields[1].toInt());
		} else if (fields[0] == "Formula") {
			w->setFormula(fields[1]);
		} else if (fields[0] == "<formula>") {
			QString formula;
			for (line++; line!=flist.end() && *line != "</formula>"; line++)
				formula += *line + "\n";
			formula.truncate(formula.length()-1);
			w->setFormula(formula);
		} else if (fields[0] == "TextFormat") {
			if (fields[1] == "f")
				w->setTextFormat('f', fields[2].toInt());
			else
				w->setTextFormat('e', fields[2].toInt());
		} else if (fields[0] == "WindowLabel") { // fileVersion > 71
			w->setWindowLabel(fields[1]);
			w->setCaptionPolicy((MyWidget::CaptionPolicy)fields[2].toInt());
			app->setListViewLabel(w->name(), fields[1]);
		} else if (fields[0] == "Coordinates") { // fileVersion > 81
			w->setCoordinates(fields[1].toDouble(), fields[2].toDouble(), fields[3].toDouble(), fields[4].toDouble());
		} else // <data> or values
			break;
	}
	if (*line == "<data>") line++;

	//read and set table values
	for (; line!=flist.end() && *line != "</data>"; line++)
	{
		QStringList fields = QStringList::split("\t",*line,true);
		int row = fields[0].toInt();
		for (int col=0; col<cols; col++)
			w->setText(row,col,fields[col+1]);
		qApp->processEvents(QEventLoop::ExcludeUserInput);
	}

	return w;
}

Table* ApplicationWindow::openTable(ApplicationWindow* app, const QStringList &flist)
{
	QStringList::const_iterator line = flist.begin();

	QStringList list=QStringList::split ("\t",*line,TRUE);
	QString caption=list[0];
	int rows = list[1].toInt();
	int cols = list[2].toInt();

	Table* w = app->newTable(caption, rows,cols);
	app->setListViewDate(caption,list[3]);
	w->setBirthDate(list[3]);

	if (caption.contains ("table"))
	{
		bool ok;
		int tb = caption.remove("table").toInt(&ok);
		if (tb > app->tables && ok) 
			app->tables = tb;	
	}

	for (line++; line!=flist.end(); line++)
	{
		QStringList fields = QStringList::split("\t",*line,true);
		if (fields[0] == "geometry") {
			restoreWindowGeometry(app, (QWidget *)w, *line);
		} else if (fields[0] == "header") {
			fields.pop_front();
			if (fileVersion >= 78)
				w->loadHeader(fields);
			else
			{
				w->setColPlotDesignation(list[4].toInt(), Table::X);
				w->setColPlotDesignation(list[6].toInt(), Table::Y);
				w->setHeader(fields);
			}
		} else if (fields[0] == "ColWidth") {
			fields.pop_front();
			w->setColWidths(fields);
		} else if (fields[0] == "com") { // legacy code
			w->setCommands(*line);
		} else if (fields[0] == "<com>") {
			for (line++; line!=flist.end() && *line != "</com>"; line++)
			{
				int col = (*line).mid(9,(*line).length()-11).toInt();
				QString formula;
				for (line++; line!=flist.end() && *line != "</col>"; line++)
					formula += *line + "\n";
				formula.truncate(formula.length()-1);
				w->setCommand(col,formula);
			}
		} else if (fields[0] == "ColType") { // fileVersion > 65
			fields.pop_front();
			w->setColumnTypes(fields);
		} else if (fields[0] == "Comments") { // fileVersion > 71
			fields.pop_front();
			w->setColComments(fields);
		} else if (fields[0] == "WindowLabel") { // fileVersion > 71
			w->setWindowLabel(fields[1]);
			w->setCaptionPolicy((MyWidget::CaptionPolicy)fields[2].toInt());
			app->setListViewLabel(w->name(), fields[1]);
		} else // <data> or values
			break;
	}
	if (*line == "<data>") line++;

	//read and set table values
	for (; line!=flist.end() && *line != "</data>"; line++)
	{
		QStringList fields = QStringList::split("\t",*line,true);
		int row = fields[0].toInt();
		for (int col=0; col<cols; col++)
			w->setText(row,col,fields[col+1]);
		qApp->processEvents(QEventLoop::ExcludeUserInput);
	}

	w->setSpecifications(w->saveToString("geometry\n"));
	return w;
}

TableStatistics* ApplicationWindow::openTableStatistics(const QStringList &flist)
{
	QStringList::const_iterator line = flist.begin();

	QStringList list=QStringList::split ("\t",*line++,TRUE);
	QString caption=list[0];

	QList<int> targets;
	for (int i=1; i <= (*line).count('\t'); i++)
		targets << (*line).section('\t',i,i).toInt();

	TableStatistics* w = newTableStatistics(table(list[1]),
			list[2]=="row" ? TableStatistics::row : TableStatistics::column, targets, caption);

	setListViewDate(caption,list[3]);
	w->setBirthDate(list[3]);

	for (line++; line!=flist.end(); line++)
	{
		QStringList fields = QStringList::split("\t",*line,true);
		if (fields[0] == "geometry"){
			restoreWindowGeometry(this, (QWidget *)w, *line);} 
		else if (fields[0] == "header") {
			fields.pop_front();
			if (fileVersion >= 78)
				w->loadHeader(fields);
			else
			{
				w->setColPlotDesignation(list[4].toInt(), Table::X);
				w->setColPlotDesignation(list[6].toInt(), Table::Y);
				w->setHeader(fields);
			}
		} else if (fields[0] == "ColWidth") {
			fields.pop_front();
			w->setColWidths(fields);
		} else if (fields[0] == "com") { // legacy code
			w->setCommands(*line);
		} else if (fields[0] == "<com>") {
			for (line++; line!=flist.end() && *line != "</com>"; line++)
			{
				int col = (*line).mid(9,(*line).length()-11).toInt();
				QString formula;
				for (line++; line!=flist.end() && *line != "</col>"; line++)
					formula += *line + "\n";
				formula.truncate(formula.length()-1);
				w->setCommand(col,formula);
			}
		} else if (fields[0] == "ColType") { // fileVersion > 65
			fields.pop_front();
			w->setColumnTypes(fields);
		} else if (fields[0] == "Comments") { // fileVersion > 71
			fields.pop_front();
			w->setColComments(fields);
		} else if (fields[0] == "WindowLabel") { // fileVersion > 71
			w->setWindowLabel(fields[1]);
			w->setCaptionPolicy((MyWidget::CaptionPolicy)fields[2].toInt());
			setListViewLabel(w->name(), fields[1]);
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
	CurveLayout cl;

	for (int j=0;j<(int)list.count()-1;j++)
	{
		QString s=list[j];
		if (s.contains ("ggeometry"))
		{
			fList=QStringList::split ("\t",s,TRUE);
			ag=(Graph*)plot->addLayer(fList[1].toInt(), fList[2].toInt(), 
					fList[3].toInt(), fList[4].toInt());
			ag->setIgnoreResizeEvents(true);
			ag->enableAutoscaling(autoscale2DPlots);
		}
		else if (s.left(10) == "Background")
		{
			fList=QStringList::split ("\t",s,true);
			if (QColor(fList[1]) != QColor(255, 255, 255))
				ag->setBackgroundColor(QColor(fList[1]));
		}
		else if (s.contains ("Margin"))
		{
			fList=QStringList::split ("\t",s,true);
			ag->plotWidget()->setMargin(fList[1].toInt());
		}
		else if (s.contains ("Border"))
		{
			fList=QStringList::split ("\t",s,true);
			ag->setBorder(fList[1].toInt(), QColor(fList[2]));
		}
		else if (s.contains ("EnabledAxes"))
		{
			fList=QStringList::split ("\t",s,true);
			ag->enableAxes(fList);
		}
		else if (s.contains ("AxesBaseline"))
		{
			fList=QStringList::split ("\t",s,false);
			ag->setAxesBaseline(fList);
		}
		else if (s.contains ("EnabledTicks"))
		{//version < 0.8.6
			fList=QStringList::split ("\t",s,true);
			fList.pop_front();
			fList.gres("-1", "3");
			ag->setMajorTicksType(fList);
			ag->setMinorTicksType(fList);
		}
		else if (s.contains ("MajorTicks"))
		{//version >= 0.8.6
			fList=QStringList::split ("\t",s,true);
			fList.pop_front();
			ag->setMajorTicksType(fList);
		}
		else if (s.contains ("MinorTicks"))
		{//version >= 0.8.6
			fList=QStringList::split ("\t",s,true);
			fList.pop_front();
			ag->setMinorTicksType(fList);
		}
		else if (s.contains ("TicksLength"))
		{
			fList=QStringList::split ("\t",s,true);
			ag->setTicksLength(fList[1].toInt(), fList[2].toInt());
		}
		else if (s.contains ("EnabledTickLabels"))
		{
			fList=QStringList::split ("\t",s,true);
			fList.pop_front();
			ag->setEnabledTickLabels(fList);
		}
		else if (s.contains ("AxesColors"))
		{
			fList=QStringList::split ("\t",s,TRUE);
			fList.pop_front();
			ag->setAxesColors(fList);
		}
		else if (s.left(5)=="grid\t")
		{
			QStringList grid=QStringList::split ("\t",s,true);
			GridOptions gr;
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
			if (grid.count() == 15)
               {
               gr.xAxis=grid[13].toInt();
               gr.yAxis=grid[14].toInt();
               }
			ag->setGridOptions(gr);
		}
		else if (s.contains ("PieCurve"))
		{
			curve=QStringList::split ("\t",s,true);
			QPen pen=QPen(QColor(curve[3]),curve[2].toInt(),Graph::getPenStyle(curve[4]));
			ag->plotPie(app->table(curve[1]),curve[1],pen,curve[5].toInt(),
					curve[6].toInt(),curve[7].toInt());
		}
		else if (s.left(6)=="curve\t")
		{
			curve = QStringList::split ("\t", s, false);
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

				if(plotType == Graph::VerticalBars || plotType == Graph::HorizontalBars || 
						plotType == Graph::Histogram)
				{
					if (fileVersion <= 76)
						ag->setBarsGap(curveID, curve[15].toInt(), 0);
					else
						ag->setBarsGap(curveID, curve[15].toInt(), curve[16].toInt());
				}
				ag->updateCurveLayout(curveID,&cl);
				if (fileVersion >= 88)
                   {
                   QwtPlotCurve *c = ag->curve(curveID);
                   if (c)
                      c->setAxis(curve[curve.count()-2].toInt(), curve[curve.count()-1].toInt());
                   }
			}
			curveID++;
		}
		else if (s.contains ("FunctionCurve"))
		{
			curve=QStringList::split ("\t", s, true);

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

			ag->insertFunctionCurve(curve[1], curve[3].toDouble(),
					curve[4].toDouble(),curve[2].toInt(), fileVersion);
			ag->setCurveType(curveID, curve[5].toInt());
			ag->updateCurveLayout(curveID, &cl);
			if (fileVersion >= 88)
               {
               QwtPlotCurve *c = ag->curve(curveID);
               if (c)
                  c->setAxis(curve[18].toInt(), curve[19].toInt());
               }
			curveID++;
		}
		else if (s.contains ("ErrorBars"))
		{
			curve=QStringList::split ("\t",s,false);
			w=app->table(curve[3]);
			Table *errTable=app->table(curve[4]);
			if (w && errTable)
			{
				double xOffset = 0, yOffset = 0;
				if (fileVersion > 85){
					xOffset = curve[11].toDouble(); yOffset = curve[12].toDouble();}

				ag->addErrorBars(w,curve[2],curve[3],errTable, curve[4], curve[1].toInt(),
						curve[5].toInt(),curve[6].toInt(), QColor(curve[7]), 
						curve[8].toInt(), curve[10].toInt(), curve[9].toInt(), xOffset, yOffset);
			}
		}
		else if (s.left(6)=="scale\t")
		{
			QStringList scl = QStringList::split ("\t", s, true);		
			scl.pop_front();
			if (fileVersion < 88)
				{
				double step = scl[2].toDouble();
				if (scl[5] == "0")
					step = 0.0;
				ag->setScale(QwtPlot::xBottom, scl[0].toDouble(), scl[1].toDouble(), step, 
							 scl[3].toInt(), scl[4].toInt(), scl[6].toInt(), bool(scl[7].toInt()));
				ag->setScale(QwtPlot::xTop, scl[0].toDouble(), scl[1].toDouble(), step, 
							 scl[3].toInt(), scl[4].toInt(), scl[6].toInt(), bool(scl[7].toInt()));

				step = scl[10].toDouble();
				if (scl[13] == "0")
					step = 0.0;
				ag->setScale(QwtPlot::yLeft, scl[8].toDouble(), scl[9].toDouble(), step, scl[11].toInt(), 
							 scl[12].toInt(), scl[14].toInt(), bool(scl[15].toInt()));
				ag->setScale(QwtPlot::yRight, scl[8].toDouble(), scl[9].toDouble(), step, scl[11].toInt(), 
							 scl[12].toInt(), scl[14].toInt(), bool(scl[15].toInt()));
				}
			else
				ag->setScale(scl[0].toInt(), scl[1].toDouble(), scl[2].toDouble(), scl[3].toDouble(),
							scl[4].toInt(), scl[5].toInt(),  scl[6].toInt(), bool(scl[7].toInt()));
        }
		else if (s.contains ("PlotTitle"))
		{
			fList=QStringList::split ("\t",s,true);
			ag->setTitle(fList[1]);
			ag->setTitleColor(QColor(fList[2]));
			ag->setTitleAlignment(fList[3].toInt());
		}
		else if (s.contains ("TitleFont"))
		{
			fList=QStringList::split ("\t",s,true);
			QFont fnt=QFont (fList[1],fList[2].toInt(),fList[3].toInt(),fList[4].toInt());
			fnt.setUnderline(fList[5].toInt());
			fnt.setStrikeOut(fList[6].toInt());
			ag->setTitleFont(fnt);
		}
		else if (s.contains ("AxesTitles"))
		{
			QStringList legend=QStringList::split ("\t",s,true);
			for (i=0;i<4;i++)
				ag->setAxisTitle(i,legend[i+1]);
		}
		else if (s.contains ("AxesTitleColors"))
		{
			QStringList colors=QStringList::split ("\t",s,false);
			ag->setAxesTitleColor(colors);
		}
		else if (s.contains ("AxesTitleAlignment"))
		{
			QStringList align=QStringList::split ("\t",s,false);
			ag->setAxesTitlesAlignment(align);
		}
		else if (s.contains ("ScaleFont"))
		{
			fList=QStringList::split ("\t",s,true);
			QFont fnt=QFont (fList[1],fList[2].toInt(),fList[3].toInt(),fList[4].toInt());
			fnt.setUnderline(fList[5].toInt());
			fnt.setStrikeOut(fList[6].toInt());

			int axis=(fList[0].right(1)).toInt();
			ag->setAxisTitleFont(axis,fnt);
		}
		else if (s.contains ("AxisFont"))
		{
			fList=QStringList::split ("\t",s,true);
			QFont fnt=QFont (fList[1],fList[2].toInt(),fList[3].toInt(),fList[4].toInt());
			fnt.setUnderline(fList[5].toInt());
			fnt.setStrikeOut(fList[6].toInt());

			int axis=(fList[0].right(1)).toInt();
			ag->setAxisFont(axis,fnt);
		}
		else if (s.contains ("AxesFormulas"))
		{
			fList=QStringList::split ("\t",s,true);
			fList.remove(fList.first());
			ag->setAxesFormulas(fList);
		}
		else if (s.startsWith("<AxisFormula "))
		{
			int pos = s.mid(18,s.length()-20).toInt();
			QString formula;
			for (j++; j<(int)list.count() && list[j] != "</AxisFormula>"; j++)
				formula += list[j] + "\n";
			formula.truncate(formula.length()-1);
			ag->setAxisFormula(pos,formula);
		}
		else if (s.contains ("LabelsFormat"))
		{
			fList=QStringList::split ("\t", s, true);
			fList.pop_front();
			ag->setLabelsNumericFormat(fList);
		}
		else if (s.contains ("LabelsRotation"))
		{
			fList=QStringList::split ("\t",s,true);
			ag->setAxisLabelRotation(QwtPlot::xBottom, fList[1].toInt());
			ag->setAxisLabelRotation(QwtPlot::xTop, fList[2].toInt());
		}
		else if (s.contains ("DrawAxesBackbone"))
		{
			fList=QStringList::split ("\t",s,true);
			ag->loadAxesOptions(fList[1]);
		}
		else if (s.contains ("AxesLineWidth"))
		{
			fList=QStringList::split ("\t",s,true);
			ag->loadAxesLinewidth(fList[1].toInt());
		}
		else if (s.contains ("CanvasFrame"))
		{
			QStringList list=QStringList::split ("\t",s,true);
			ag->drawCanvasFrame(list);
		}
		else if (s.contains ("CanvasBackground"))
		{
			QStringList list=QStringList::split ("\t",s,TRUE);
			ag->setCanvasBackground(QColor(list[1]));
		}
		else if (s.contains ("Legend"))
		{
			fList=QStringList::split ("\t",s,TRUE);
			ag->insertLegend(fList, fileVersion);
		}
		else if (s.contains ("textMarker"))
		{
			fList=QStringList::split ("\t",s,TRUE);
			ag->insertTextMarker(fList, fileVersion);
		}
		else if (s.contains ("lineMarker"))
		{
			fList=QStringList::split ("\t",s,TRUE);
			ag->insertLineMarker(fList, fileVersion);
		}
		else if (s.contains ("ImageMarker"))
		{
			fList=QStringList::split ("\t",s,TRUE);
			ag->insertImageMarker(fList, fileVersion);
		}
		else if (s.contains ("FitID"))
		{
			fList=QStringList::split ("\t",s,true);
			ag->setFitID(fList[1].toInt());
		}
		else if (s.contains("AxisType"))
		{
			fList=QStringList::split ("\t",s,true);
			for (i=0; i<4; i++)
			{
				QStringList lst = QStringList::split(";", fList[i+1], false);
				int format = lst[0].toInt();
				if (format == Graph::Day)
					ag->setLabelsDayFormat(i, lst[1].toInt());
				else if (format == Graph::Month)
					ag->setLabelsMonthFormat(i, lst[1].toInt());
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
			fList=QStringList::split ("\t",s,true);
			QList<int> axesTypes = ag->axesType();
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
	ag->setTextMarkerDefaults(legendFrameStyle, plotLegendFont, legendTextColor, legendBackground);
	ag->setArrowDefaults(defaultArrowLineWidth, defaultArrowColor, defaultArrowLineStyle,
			defaultArrowHeadLength, defaultArrowHeadAngle, defaultArrowHeadFill);
	plot->connectLayer(ag);
}

Graph3D* ApplicationWindow::openSurfacePlot(ApplicationWindow* app, const QStringList &lst)
{
	QStringList fList=QStringList::split ("\t",lst[0],true);
	QString caption=fList[0];
	QString date=fList[1];
	if (date.isEmpty())
		date = QDateTime::currentDateTime().toString(Qt::LocalDate);

	fList=QStringList::split ("\t",lst[2],false );
	Graph3D *plot=0;

	if (fList[1].endsWith("(Y)",true))//Ribbon plot
		plot=app->dataPlot3D(caption, fList[1],fList[2].toDouble(),fList[3].toDouble(),
				fList[4].toDouble(),fList[5].toDouble(),fList[6].toDouble(),fList[7].toDouble());
	else if (fList[1].contains("(Z)",true) > 0)
		plot=app->dataPlotXYZ(caption, fList[1], fList[2].toDouble(),fList[3].toDouble(),
				fList[4].toDouble(),fList[5].toDouble(),fList[6].toDouble(),fList[7].toDouble());
	else if (fList[1].startsWith("matrix<",true) && fList[1].endsWith(">",false))
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

	if (caption.contains ("graph",true))
	{
		bool ok;
		int gr=caption.remove("graph").toInt(&ok);
		if (gr > app->graphs && ok) 
			app->graphs = gr;
	}

	plot->setIgnoreFonts(true);
	restoreWindowGeometry(app, (QWidget *)plot, lst[1]);

	fList=QStringList::split ("\t",lst[3],false );
	plot->setStyle(fList);

	fList=QStringList::split ("\t", lst[4],false );
	plot->setGrid(fList[1].toInt());

	fList=QStringList::split ("\t",lst[5],true );
	plot->setTitle(fList);

	fList=QStringList::split ("\t",lst[6],false );
	plot->setColors(fList);

	fList=QStringList::split ("\t",lst[7],false );
	fList.pop_front();
	plot->setAxesLabels(fList);

	fList=QStringList::split ("\t",lst[8],false );
	plot->setTicks(fList);

	fList=QStringList::split ("\t",lst[9],false );
	plot->setTickLengths(fList);

	fList=QStringList::split ("\t",lst[10],false );
	plot->setOptions(fList);

	fList=QStringList::split ("\t",lst[11],false );
	plot->setNumbersFont(fList);

	fList=QStringList::split ("\t",lst[12],false );
	plot->setXAxisLabelFont(fList);

	fList=QStringList::split ("\t",lst[13],false );
	plot->setYAxisLabelFont(fList);

	fList=QStringList::split ("\t",lst[14],false );
	plot->setZAxisLabelFont(fList);

	fList=QStringList::split ("\t",lst[15],false );
	plot->setRotation(fList[1].toDouble(),fList[2].toDouble(),fList[3].toDouble());

	fList=QStringList::split ("\t",lst[16],false );
	plot->setZoom(fList[1].toDouble());

	fList=QStringList::split ("\t",lst[17],false );
	plot->setScale(fList[1].toDouble(),fList[2].toDouble(),fList[3].toDouble());

	fList=QStringList::split ("\t",lst[18],false );
	plot->setShift(fList[1].toDouble(),fList[2].toDouble(),fList[3].toDouble());

    fList=QStringList::split ("\t",lst[19],false );
    plot->setMeshLineWidth(fList[1].toInt());

	if (fileVersion > 71)
	{
		fList=QStringList::split ("\t",lst[20],false );
		plot->setWindowLabel(fList[1]);
		plot->setCaptionPolicy((MyWidget::CaptionPolicy)fList[2].toInt());
		app->setListViewLabel(plot->name(),fList[1]);
	}

    if (fileVersion >= 88)
       {
       fList=QStringList::split ("\t",lst[21],false);
       plot->setOrtho(fList[1].toInt());
       }
  	        
	plot->update();
	plot->setIgnoreFonts(true);
	return plot;
}

void ApplicationWindow::copyActiveLayer()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	copiedLayer=TRUE;

	Graph *g = ((MultiLayer *)ws->activeWindow())->activeGraph();
	delete lastCopiedLayer;
	lastCopiedLayer = new Graph (0, 0, 0);
	lastCopiedLayer->setAttribute(Qt::WA_DeleteOnClose);
	lastCopiedLayer->setGeometry(0, 0, g->width(), g->height());
	lastCopiedLayer->copy(g);
	g->copyImage();
}

void ApplicationWindow::showDataSetDialog(const QString& whichFit)
{
	DataSetDialog *ad=new DataSetDialog(tr("Curve:"));
	ad->setAttribute(Qt::WA_DeleteOnClose);
	ad->setCurveNames(activeGraph->curvesList());
	ad->setOperationType(whichFit);
	ad->exec();

	connect (ad,SIGNAL(analyze(const QString&, const QString&)),this,SLOT(analyzeCurve(const QString&, const QString& )));
}

void ApplicationWindow::analyzeCurve(const QString& whichFit, const QString& curveTitle)
{
	if(whichFit=="fitLinear" || whichFit=="fitSigmoidal" || whichFit=="fitGauss" || whichFit=="fitLorentz")
	{
		Fit *fitter = 0;
		if (whichFit == "fitLinear")
			fitter = new LinearFit (this, activeGraph);
		else if (whichFit == "fitSigmoidal")
			fitter = new SigmoidalFit (this, activeGraph);
		else if(whichFit == "fitGauss")
			fitter = new GaussFit(this, activeGraph);
		else if(whichFit == "fitLorentz")
			fitter = new LorentzFit(this, activeGraph);

		if (fitter->setDataFromCurve(curveTitle))
		{
			if (whichFit != "fitLinear")
				fitter->guessInitialValues();

			fitter->setFitCurveParameters(generateUniformFitPoints, fitPoints);
			fitter->fit();
			delete fitter;
		}
	}
	else if(whichFit == "differentiate" && activeGraph->diffCurve(curveTitle))
	{
		Table* w = table(tableWindows.last());
		QStringList list;
		list<<QString(w->name())+"_derivative";
		MultiLayer* d=multilayerPlot(w,list,0);
		d->setFocus();
	}
}

void ApplicationWindow::analysis(const QString& whichFit)
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("MultiLayer"))
		return;

	Graph* g = ((MultiLayer*)ws->activeWindow())->activeGraph();
	if (!g || !g->validCurvesDataSize())
		return;

	activeGraph=g;
	aw = (MyWidget *)ws->activeWindow();

	if (g->selectorsEnabled()) // a curve is selected
		analyzeCurve(whichFit, g->selectedCurveTitle());
	else if(g->curves() == 1)
	{
		const QwtPlotCurve *c = g->curve(0);
		if (c)
			analyzeCurve(whichFit,c->title().text());
	}
	else
		showDataSetDialog(whichFit);
}

void ApplicationWindow::pickPointerCursor()
{
	btnPointer->setChecked(true);
	activeGraph = 0;
}

void ApplicationWindow::disableTools()
{
	if (displayBar->isVisible())
		displayBar->hide();

	QWidgetList *windows = windowsList();
	foreach(QWidget *w, *windows)
	{
		if (w->isA("MultiLayer"))
		{
			QWidgetList lst= ((MultiLayer *)w)->graphPtrs();
			foreach(QWidget *widget, lst)
				((Graph *)widget)->disableTools();
		}
	}
	delete windows;
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
	else if (action == btnZoomIn)
		zoomIn();
	else if (action == btnZoomOut)
		zoomOut();
	else if (action == btnArrow)
		drawArrow();
	else if (action == btnLine)
		drawLine();
}

void ApplicationWindow::connectSurfacePlot(Graph3D *plot)
{
	connect (plot,SIGNAL(showContextMenu()),this,SLOT(showWindowContextMenu()));
	connect (plot,SIGNAL(showOptionsDialog()),this,SLOT(showPlot3dDialog()));
	connect (plot,SIGNAL(closedWindow(MyWidget*)),this, SLOT(closeWindow(MyWidget*)));
	connect (plot,SIGNAL(hiddenWindow(MyWidget*)),this, SLOT(hideWindow(MyWidget*)));
	connect (plot,SIGNAL(statusChanged(MyWidget*)),this, SLOT(updateWindowStatus(MyWidget*)));
	connect (plot,SIGNAL(modified()),this, SIGNAL(modified()));
	connect (plot,SIGNAL(custom3DActions(QWidget*)),this, SLOT(custom3DActions(QWidget*)));

	plot->askOnCloseEvent(confirmClosePlot3D);
}

void ApplicationWindow::connectMultilayerPlot(MultiLayer *g)
{
	// FIXME: the signal changeActiveLayer does not exist
	//connect (g,SIGNAL(changeActiveLayer(Graph *)),this,SLOT(changeActiveGraph(Graph *)));

	connect (g,SIGNAL(showTextDialog()),this,SLOT(showTextDialog()));
	connect (g,SIGNAL(showPlotDialog(int)),this,SLOT(showPlotDialog(int)));
	connect (g,SIGNAL(showScaleDialog(int)), this, SLOT(showScalePageFromAxisDialog(int)));
	connect (g,SIGNAL(showAxisDialog(int)), this, SLOT(showAxisPageFromAxisDialog(int)));
	connect (g,SIGNAL(showCurveContextMenu(int)),this,SLOT(showCurveContextMenu(int)));

	connect (g,SIGNAL(showWindowContextMenu()),this,SLOT(showWindowContextMenu()));
	connect (g,SIGNAL(showCurvesDialog()),this,SLOT(showCurvesDialog()));
	connect (g,SIGNAL(drawLineEnded(bool)), btnPointer, SLOT(setOn(bool)));
	connect (g,SIGNAL(drawTextOff()),this, SLOT(disableAddText()));
	connect (g,SIGNAL(showXAxisTitleDialog()),this,SLOT(showXAxisTitleDialog()));
	connect (g,SIGNAL(showYAxisTitleDialog()),this,SLOT(showYAxisTitleDialog()));
	connect (g,SIGNAL(showRightAxisTitleDialog()),this,SLOT(showRightAxisTitleDialog()));
	connect (g,SIGNAL(showTopAxisTitleDialog()),this,SLOT(showTopAxisTitleDialog()));
	connect (g,SIGNAL(showMarkerPopupMenu()),this,SLOT(showMarkerPopupMenu()));
	connect (g,SIGNAL(closedWindow(MyWidget*)),this, SLOT(closeWindow(MyWidget*)));
	connect (g,SIGNAL(hiddenWindow(MyWidget*)),this, SLOT(hideWindow(MyWidget*)));
	connect (g,SIGNAL(statusChanged(MyWidget*)),this, SLOT(updateWindowStatus(MyWidget*)));
	connect (g,SIGNAL(cursorInfo(const QString&)),info,SLOT(setText(const QString&)));
	connect (g,SIGNAL(showImageDialog()),this,SLOT(showImageDialog()));
	connect (g,SIGNAL(createTablePlot(const QString&,int,int,const QString&)),
			this,SLOT(newWrksheetPlot(const QString&,int,int,const QString&)));
	connect (g,SIGNAL(createHiddenTable(const QString&,int,int,const QString&)),
			this,SLOT(newHiddenTable(const QString&,int,int,const QString&)));
	connect (g,SIGNAL(createTable(const QString&,int,int,const QString&)),
			this,SLOT(newTable(const QString&,int,int,const QString&)));
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
	connect (w,SIGNAL(statusChanged(MyWidget*)),this, SLOT(updateWindowStatus(MyWidget*)));
	connect (w,SIGNAL(hiddenWindow(MyWidget*)),this, SLOT(hideWindow(MyWidget*)));
	connect (w,SIGNAL(closedWindow(MyWidget*)),this, SLOT(closeWindow(MyWidget*)));
	connect (w,SIGNAL(removedCol(const QString&)),this,SLOT(removeCurves(const QString&)));
	connect (w,SIGNAL(modifiedData(Table *, const QString&)),
			this,SLOT(updateCurves(Table *, const QString&)));
	connect (w,SIGNAL(plotCol(Table*,const QStringList&, int)),this, SLOT(multilayerPlot(Table*,const QStringList&, int)));
	connect (w,SIGNAL(modifiedWindow(QWidget*)),this,SLOT(modifiedProject(QWidget*)));
	connect (w,SIGNAL(optionsDialog()),this,SLOT(showColumnOptionsDialog()));
	connect (w,SIGNAL(colValuesDialog()),this,SLOT(showColumnValuesDialog()));
	connect (w,SIGNAL(showContextMenu(bool)),this,SLOT(showTableContextMenu(bool)));
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
		QPalette pal = results->palette();
		pal.setBrush(QPalette::Active, QPalette::Window, QBrush(pc, Qt::SolidPattern ) );
		results->setPalette(pal);

		pal = qApp->palette();
		pal.setColor(QPalette::Active, QPalette::Base, QColor(panelsColor) );
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
	QList<QWidget*> *windows = windowsList();
	for (int i = 0; i<int(windows->count());i++ )
	{
		if (windows->at(i)->isA("Graph3D"))
			{
            Graph3D *g = (Graph3D*)windows->at(i);
            g->setSmoothMesh(smooth3DMesh);
            g->setOrtho(orthogonal3DPlots);
            }
        } 	        
  	delete windows;
}

void ApplicationWindow::createActions()
{
	actionNewProject = new QAction(QIcon(QPixmap(new_xpm)), tr("New &Project"), this);
	actionNewProject->setShortcut( tr("Ctrl+N") );
	connect(actionNewProject, SIGNAL(activated()), this, SLOT(newProject()));

	actionNewGraph = new QAction(QIcon(QPixmap(new_graph_xpm)), tr("New &Graph"), this);
	actionNewGraph->setShortcut( tr("Ctrl+G") );
	connect(actionNewGraph, SIGNAL(activated()), this, SLOT(newGraph()));

	actionNewNote = new QAction(QIcon(QPixmap(new_note_xpm)), tr("New &Note"), this);
	connect(actionNewNote, SIGNAL(activated()), this, SLOT(newNote()));

	actionNewTable = new QAction(QIcon(QPixmap(table_xpm)), tr("New &Table"), this);
	actionNewTable->setShortcut( tr("Ctrl+T") );
	connect(actionNewTable, SIGNAL(activated()), this, SLOT(newTable()));

	actionNewMatrix = new QAction(QIcon(QPixmap(new_matrix_xpm)), tr("New &Matrix"), this);
	actionNewMatrix->setShortcut( tr("Ctrl+M") );
	connect(actionNewMatrix, SIGNAL(activated()), this, SLOT(newMatrix()));

	actionNewFunctionPlot = new QAction(QIcon(QPixmap(newF_xpm)), tr("New &Function Plot"), this);
	actionNewFunctionPlot->setShortcut( tr("Ctrl+F") );
	connect(actionNewFunctionPlot, SIGNAL(activated()), this, SLOT(newFunctionPlot()));

	actionNewSurfacePlot = new QAction(QIcon(QPixmap(newFxy_xpm)), tr("New 3D &Surface Plot"), this);
	actionNewSurfacePlot->setShortcut( tr("Ctrl+ALT+Z") );
	connect(actionNewSurfacePlot, SIGNAL(activated()), this, SLOT(newSurfacePlot()));

	actionOpen = new QAction(QIcon(QPixmap(fileopen_xpm)), tr("&Open"), this);
	actionOpen->setShortcut( tr("Ctrl+O") );
	connect(actionOpen, SIGNAL(activated()), this, SLOT(open()));

	actionLoadImage = new QAction(tr("Open Image &File"), this);
	actionLoadImage->setShortcut( tr("Ctrl+I") );
	connect(actionLoadImage, SIGNAL(activated()), this, SLOT(loadImage()));

	actionImportImage = new QAction(tr("Import I&mage..."), this);
	connect(actionImportImage, SIGNAL(activated()), this, SLOT(importImage()));

	actionSaveProject = new QAction(QIcon(QPixmap(filesave_xpm)), tr("&Save Project"), this);
	actionSaveProject->setShortcut( tr("Ctrl+S") );
	connect(actionSaveProject, SIGNAL(activated()), this, SLOT(saveProject()));
	savedProject();

	actionSaveProjectAs = new QAction(tr("Save Project &As..."), this);
	connect(actionSaveProjectAs, SIGNAL(activated()), this, SLOT(saveProjectAs()));

	actionOpenTemplate = new QAction(QIcon(QPixmap(open_template_xpm)),tr("Open Temp&late..."), this);
	connect(actionOpenTemplate, SIGNAL(activated()), this, SLOT(openTemplate()));

	actionSaveTemplate = new QAction(QIcon(QPixmap(save_template_xpm)), tr("Save As &Template..."), this);
	connect(actionSaveTemplate, SIGNAL(activated()), this, SLOT(saveAsTemplate()));

	actionSaveNote = new QAction(tr("Save Note As..."), this);
	connect(actionSaveNote, SIGNAL(activated()), this, SLOT(saveNoteAs()));

	actionLoad = new QAction(QIcon(QPixmap(import_xpm)), tr("&Single File..."), this);
	connect(actionLoad, SIGNAL(activated()), this, SLOT(loadASCII()));

	actionLoadMultiple = new QAction(QIcon(QPixmap(multiload_xpm)), tr("&Multiple Files..."), this);
	connect(actionLoadMultiple, SIGNAL(activated()), this, SLOT(loadMultiple()));

	actionUndo = new QAction(QIcon(QPixmap(undo_xpm)), tr("&Undo"), this);
	actionUndo->setShortcut( tr("Ctrl+Z") );
	connect(actionUndo, SIGNAL(activated()), this, SLOT(undo()));
	actionUndo->setEnabled(false);

	actionRedo = new QAction(QIcon(QPixmap(redo_xpm)), tr("&Redo"), this);
	actionRedo->setShortcut( tr("Ctrl+R") );
	connect(actionRedo, SIGNAL(activated()), this, SLOT(redo()));
	actionRedo->setEnabled(false);

	actionCopyWindow = new QAction(QIcon(QPixmap(duplicate_xpm)), tr("&Duplicate"), this);
	connect(actionCopyWindow, SIGNAL(activated()), this, SLOT(copyWindow()));

	actionCutSelection = new QAction(QIcon(QPixmap(cut_xpm)), tr("Cu&t Selection"), this);
	actionCutSelection->setShortcut( tr("Ctrl+X") );
	connect(actionCutSelection, SIGNAL(activated()), this, SLOT(cutSelection()));

	actionCopySelection = new QAction(QIcon(QPixmap(copy_xpm)), tr("&Copy Selection"), this);
	actionCopySelection->setShortcut( tr("Ctrl+C") );
	connect(actionCopySelection, SIGNAL(activated()), this, SLOT(copySelection()));

	actionPasteSelection = new QAction(QIcon(QPixmap(paste_xpm)), tr("&Paste Selection"), this);
	actionPasteSelection->setShortcut( tr("Ctrl+V") );
	connect(actionPasteSelection, SIGNAL(activated()), this, SLOT(pasteSelection()));

	actionClearSelection = new QAction(QIcon(QPixmap(erase_xpm)), tr("&Delete Selection"), this);
	actionClearSelection->setShortcut( tr("Del","delete key") );
	connect(actionClearSelection, SIGNAL(activated()), this, SLOT(clearSelection()));

	actionShowExplorer = new QAction(QIcon(QPixmap(folder_xpm)), tr("Project &Explorer"), this);
	actionShowExplorer->setShortcut( tr("Ctrl+E") );
	actionShowExplorer->setCheckable(true);
	actionShowExplorer->setChecked(false);
	connect(actionShowExplorer, SIGNAL(activated()), this, SLOT(showExplorer()));

	actionShowLog = new QAction(QIcon(QPixmap(log_xpm)), tr("Results &Log"), this);
	actionShowLog->setCheckable(true);
	actionShowLog->setChecked(false);

#ifdef SCRIPTING_CONSOLE
	actionShowConsole = new QAction(tr("Scripting &Console"), this);
	actionShowConsole->setCheckable(true);
	actionShowConsole->setChecked(false);
#endif

	actionAddLayer = new QAction(QIcon(QPixmap(newLayer_xpm)), tr("Add La&yer"), this);
	actionAddLayer->setShortcut( tr("ALT+L") );
	connect(actionAddLayer, SIGNAL(activated()), this, SLOT(addLayer()));

	actionShowLayerDialog = new QAction(QIcon(QPixmap(arrangeLayers_xpm)), tr("Arran&ge Layers"), this);
	actionShowLayerDialog->setShortcut( tr("ALT+A") );
	connect(actionShowLayerDialog, SIGNAL(activated()), this, SLOT(showLayerDialog()));

    actionAutomaticLayout = new QAction(QIcon(QPixmap(auto_layout_xpm)), tr("Automatic Layout"), this);
    connect(actionAutomaticLayout, SIGNAL(activated()), this, SLOT(autoArrangeLayers()));
  	  
	actionExportGraph = new QAction(tr("&Current"), this);
	actionExportGraph->setShortcut( tr("Alt+G") );
	connect(actionExportGraph, SIGNAL(activated()), this, SLOT(exportGraph()));

	actionExportAllGraphs = new QAction(tr("&All"), this);
	actionExportAllGraphs->setShortcut( tr("Alt+X") );
	connect(actionExportAllGraphs, SIGNAL(activated()), this, SLOT(exportAllGraphs()));

	actionPrint = new QAction(QIcon(QPixmap(fileprint_xpm)), tr("&Print"), this);
	actionPrint->setShortcut( tr("Ctrl+P") );
	connect(actionPrint, SIGNAL(activated()), this, SLOT(print()));

	actionPrintAllPlots = new QAction(tr("Print All Plo&ts"), this);
	connect(actionPrintAllPlots, SIGNAL(activated()), this, SLOT(printAllPlots()));

	actionShowExportASCIIDialog = new QAction(tr("E&xport ASCII"), this);
	connect(actionShowExportASCIIDialog, SIGNAL(activated()), this, SLOT(showExportASCIIDialog()));

	actionShowImportDialog = new QAction(tr("Set Import &Options"), this);
	connect(actionShowImportDialog, SIGNAL(activated()), this, SLOT(showImportDialog()));

	actionCloseAllWindows = new QAction(QIcon(QPixmap(quit_xpm)), tr("&Quit"), this);
	actionCloseAllWindows->setShortcut( tr("Alt+F4") );
	connect(actionCloseAllWindows, SIGNAL(activated()), qApp, SLOT(closeAllWindows()));

	actionClearLogInfo = new QAction(tr("Clear &Log Information"), this);
	connect(actionClearLogInfo, SIGNAL(activated()), this, SLOT(clearLogInfo()));

	actionDeleteFitTables = new QAction(QIcon(QPixmap(close_xpm)), tr("Delete &Fit Tables"), this);
	connect(actionDeleteFitTables, SIGNAL(activated()), this, SLOT(deleteFitTables()));

	actionShowPlotWizard = new QAction(QIcon(QPixmap(wizard_xpm)), tr("Plot &Wizard"), this);
	actionShowPlotWizard->setShortcut( tr("Ctrl+Alt+W") );
	connect(actionShowPlotWizard, SIGNAL(activated()), this, SLOT(showPlotWizard()));

	actionShowConfigureDialog = new QAction(tr("&Preferences..."), this);
	connect(actionShowConfigureDialog, SIGNAL(activated()), this, SLOT(showPreferencesDialog()));

	actionShowCurvesDialog = new QAction(QIcon(QPixmap(curves_xpm)), tr("Add/Remove &Curve..."), this);
	actionShowCurvesDialog->setShortcut( tr("ALT+C") );
	connect(actionShowCurvesDialog, SIGNAL(activated()), this, SLOT(showCurvesDialog()));

	actionAddErrorBars = new QAction(QIcon(QPixmap(errors_xpm)), tr("Add &Error Bars..."), this);
	actionAddErrorBars->setShortcut( tr("Ctrl+B") );
	connect(actionAddErrorBars, SIGNAL(activated()), this, SLOT(addErrorBars()));

	actionAddFunctionCurve = new QAction(QIcon(QPixmap(fx_xpm)), tr("Add &Function..."), this);
	actionAddFunctionCurve->setShortcut( tr("Ctrl+Alt+F") );
	connect(actionAddFunctionCurve, SIGNAL(activated()), this, SLOT(addFunctionCurve()));

	actionUnzoom = new QAction(QIcon(QPixmap(unzoom_xpm)), tr("&Rescale to Show All"), this);
	actionUnzoom->setShortcut( tr("Ctrl+Shift+R") );
	connect(actionUnzoom, SIGNAL(activated()), this, SLOT(setAutoScale()));

	actionNewLegend = new QAction(QIcon(QPixmap(legend_xpm)), tr("New &Legend"), this);
	actionNewLegend->setShortcut( tr("Ctrl+L") );
	connect(actionNewLegend, SIGNAL(activated()), this, SLOT(newLegend()));

	actionTimeStamp = new QAction(QIcon(QPixmap(clock_xpm)), tr("Add Time Stamp"), this);
	actionTimeStamp->setShortcut( tr("Ctrl+ALT+T") );
	connect(actionTimeStamp, SIGNAL(activated()), this, SLOT(addTimeStamp()));

	actionAddImage = new QAction(QIcon(QPixmap(monalisa_xpm)), tr("Add &Image"), this);
	actionAddImage->setShortcut( tr("ALT+I") );
	connect(actionAddImage, SIGNAL(activated()), this, SLOT(addImage()));

	actionPlotL = new QAction(QIcon(QPixmap(lPlot_xpm)), tr("&Line"), this);
	connect(actionPlotL, SIGNAL(activated()), this, SLOT(plotL()));

	actionPlotP = new QAction(QIcon(QPixmap(pPlot_xpm)), tr("&Scatter"), this);
	connect(actionPlotP, SIGNAL(activated()), this, SLOT(plotP()));

	actionPlotLP = new QAction(QIcon(QPixmap(lpPlot_xpm)), tr("Line + S&ymbol"), this);
	connect(actionPlotLP, SIGNAL(activated()), this, SLOT(plotLP()));

	actionPlotVerticalDropLines = new QAction(QIcon(QPixmap(dropLines_xpm)), tr("Vertical &Drop Lines"), this);
	connect(actionPlotVerticalDropLines, SIGNAL(activated()), this, SLOT(plotVerticalDropLines()));

	actionPlotSpline = new QAction(QIcon(QPixmap(spline_xpm)), tr("&Spline"), this);
	connect(actionPlotSpline, SIGNAL(activated()), this, SLOT(plotSpline()));

    actionPlotHorSteps = new QAction(QPixmap(hor_steps_xpm), tr("&Horizontal Steps"), this);
    connect(actionPlotHorSteps, SIGNAL(activated()), this, SLOT(plotHorSteps()));
  
	actionPlotVertSteps = new QAction(QIcon(QPixmap(vert_steps_xpm)), tr("&Vertical Steps"), this);
	connect(actionPlotVertSteps, SIGNAL(activated()), this, SLOT(plotVertSteps()));

	actionPlotVerticalBars = new QAction(QIcon(QPixmap(vertBars_xpm)), tr("&Columns"), this);
	connect(actionPlotVerticalBars, SIGNAL(activated()), this, SLOT(plotVerticalBars()));

	actionPlotHorizontalBars = new QAction(QIcon(QPixmap(hBars_xpm)), tr("&Rows"), this);
	connect(actionPlotHorizontalBars, SIGNAL(activated()), this, SLOT(plotHorizontalBars()));

	actionPlotArea = new QAction(QIcon(QPixmap(area_xpm)), tr("&Area"), this);
	connect(actionPlotArea, SIGNAL(activated()), this, SLOT(plotArea()));

	actionPlotPie = new QAction(QIcon(QPixmap(pie_xpm)), tr("&Pie"), this);
	connect(actionPlotPie, SIGNAL(activated()), this, SLOT(plotPie()));

	actionPlotVectXYAM = new QAction(QIcon(QPixmap(vectXYAM_xpm)), tr("Vectors XY&AM"), this);
	connect(actionPlotVectXYAM, SIGNAL(activated()), this, SLOT(plotVectXYAM()));

	actionPlotVectXYXY = new QAction(QIcon(QPixmap(vectXYXY_xpm)), tr("&Vectors &XYXY"), this);
	connect(actionPlotVectXYXY, SIGNAL(activated()), this, SLOT(plotVectXYXY()));

	actionPlotHistogram = new QAction(QIcon(QPixmap(histogram_xpm)), tr("&Histogram"), this);
	connect(actionPlotHistogram, SIGNAL(activated()), this, SLOT(plotHistogram()));

	actionPlotStackedHistograms = new QAction(QIcon(QPixmap(stacked_hist_xpm)), tr("&Stacked Histogram"), this);
	connect(actionPlotStackedHistograms, SIGNAL(activated()), this, SLOT(plotStackedHistograms()));

	actionPlot2VerticalLayers = new QAction(QIcon(QPixmap(panel_v2_xpm)), tr("&Vertical 2 Layers"), this);
	connect(actionPlot2VerticalLayers, SIGNAL(activated()), this, SLOT(plot2VerticalLayers()));

	actionPlot2HorizontalLayers = new QAction(QIcon(QPixmap(panel_h2_xpm)), tr("&Horizontal 2 Layers"), this);
	connect(actionPlot2HorizontalLayers, SIGNAL(activated()), this, SLOT(plot2HorizontalLayers()));

	actionPlot4Layers = new QAction(QIcon(QPixmap(panel_4_xpm)), tr("&4 Layers"), this);
	connect(actionPlot4Layers, SIGNAL(activated()), this, SLOT(plot4Layers()));

	actionPlotStackedLayers = new QAction(QIcon(QPixmap(stacked_xpm)), tr("&Stacked Layers"), this);
	connect(actionPlotStackedLayers, SIGNAL(activated()), this, SLOT(plotStackedLayers()));

	actionPlot3DRibbon = new QAction(QIcon(QPixmap(ribbon_xpm)), tr("&Ribbon"), this);
	connect(actionPlot3DRibbon, SIGNAL(activated()), this, SLOT(plot3DRibbon()));

	actionPlot3DBars = new QAction(QIcon(QPixmap(bars_xpm)), tr("&Bars"), this);
	connect(actionPlot3DBars, SIGNAL(activated()), this, SLOT(plot3DBars()));

	actionPlot3DScatter = new QAction(QIcon(QPixmap(scatter_xpm)), tr("&Scatter"), this);
	connect(actionPlot3DScatter, SIGNAL(activated()), this, SLOT(plot3DScatter()));

	actionPlot3DTrajectory = new QAction(QIcon(QPixmap(trajectory_xpm)), tr("&Trajectory"), this);
	connect(actionPlot3DTrajectory, SIGNAL(activated()), this, SLOT(plot3DTrajectory()));

	actionShowColStatistics = new QAction(QIcon(QPixmap(col_stat_xpm)), tr("Statistics on &Columns"), this);
	connect(actionShowColStatistics, SIGNAL(activated()), this, SLOT(showColStatistics()));

	actionShowRowStatistics = new QAction(QIcon(QPixmap(stat_rows_xpm)), tr("Statistics on &Rows"), this);
	connect(actionShowRowStatistics, SIGNAL(activated()), this, SLOT(showRowStatistics()));

	actionShowIntDialog = new QAction(tr("&Integrate ..."), this);
	connect(actionShowIntDialog, SIGNAL(activated()), this, SLOT(showIntDialog()));

	actionInterpolate = new QAction(tr("Inte&rpolate ..."), this);
	connect(actionInterpolate, SIGNAL(activated()), this, SLOT(showInterpolationDialog()));

	actionLowPassFilter = new QAction(tr("&Low Pass..."), this);
	connect(actionLowPassFilter, SIGNAL(activated()), this, SLOT(lowPassFilterDialog()));

	actionHighPassFilter = new QAction(tr("&High Pass..."), this);
	connect(actionHighPassFilter, SIGNAL(activated()), this, SLOT(highPassFilterDialog()));

	actionBandPassFilter = new QAction(tr("&Band Pass..."), this);
	connect(actionBandPassFilter, SIGNAL(activated()), this, SLOT(bandPassFilterDialog()));

	actionBandBlockFilter = new QAction(tr("&Band Block..."), this);
	connect(actionBandBlockFilter, SIGNAL(activated()), this, SLOT(bandBlockFilterDialog()));

	actionFFT = new QAction(tr("&FFT..."), this);
	connect(actionFFT, SIGNAL(activated()), this, SLOT(showFFTDialog()));

	actionSmoothSavGol = new QAction(tr("&Savitzky-Golay..."), this);
	connect(actionSmoothSavGol, SIGNAL(activated()), this, SLOT(showSmoothSavGolDialog()));

	actionSmoothFFT = new QAction(tr("&FFT Filter..."), this);
	connect(actionSmoothFFT, SIGNAL(activated()), this, SLOT(showSmoothFFTDialog()));

	actionSmoothAverage = new QAction(tr("Moving Window &Average..."), this);
	connect(actionSmoothAverage, SIGNAL(activated()), this, SLOT(showSmoothAverageDialog()));

	actionDifferentiate = new QAction(tr("&Differentiate"), this);
	connect(actionDifferentiate, SIGNAL(activated()), this, SLOT(differentiate()));

	actionFitLinear = new QAction(tr("Fit &Linear"), this);
	connect(actionFitLinear, SIGNAL(activated()), this, SLOT(fitLinear()));

	actionShowFitPolynomDialog = new QAction(tr("Fit &Polynomial ..."), this);
	connect(actionShowFitPolynomDialog, SIGNAL(activated()), this, SLOT(showFitPolynomDialog()));

	actionShowExpDecayDialog = new QAction(tr("&First Order ..."), this);
	connect(actionShowExpDecayDialog, SIGNAL(activated()), this, SLOT(showExpDecayDialog()));

	actionShowTwoExpDecayDialog = new QAction(tr("&Second Order ..."), this);
	connect(actionShowTwoExpDecayDialog, SIGNAL(activated()), this, SLOT(showTwoExpDecayDialog()));

	actionShowExpDecay3Dialog = new QAction(tr("&Third Order ..."), this);
	connect(actionShowExpDecay3Dialog, SIGNAL(activated()), this, SLOT(showExpDecay3Dialog()));

	actionFitExpGrowth = new QAction(tr("Fit Exponential Gro&wth ..."), this);
	connect(actionFitExpGrowth, SIGNAL(activated()), this, SLOT(showExpGrowthDialog()));

	actionFitSigmoidal = new QAction(tr("Fit &Boltzmann (Sigmoidal)"), this);
	connect(actionFitSigmoidal, SIGNAL(activated()), this, SLOT(fitSigmoidal()));

	actionFitGauss = new QAction(tr("Fit &Gaussian"), this);
	connect(actionFitGauss, SIGNAL(activated()), this, SLOT(fitGauss()));

	actionFitLorentz = new QAction(tr("Fit Lorent&zian"), this);
	connect(actionFitLorentz, SIGNAL(activated()), this, SLOT(fitLorentz()));

	actionShowFitDialog = new QAction(tr("&Nonlinear Curve Fit ..."), this);
	actionShowFitDialog->setShortcut( tr("Ctrl+Y") );
	connect(actionShowFitDialog, SIGNAL(activated()), this, SLOT(showFitDialog()));

	actionShowPlotDialog = new QAction(tr("&Plot ..."), this);
	connect(actionShowPlotDialog, SIGNAL(activated()), this, SLOT(showGeneralPlotDialog()));

	actionShowCurveFormatDialog = new QAction(tr("&Curves ..."), this);
	connect(actionShowCurveFormatDialog, SIGNAL(activated()), this, SLOT(showPlotDialog()));

	actionShowScaleDialog = new QAction(tr("&Scales..."), this);
	connect(actionShowScaleDialog, SIGNAL(activated()), this, SLOT(showScaleDialog()));

	actionShowAxisDialog = new QAction(tr("&Axes..."), this);
	connect(actionShowAxisDialog, SIGNAL(activated()), this, SLOT(showAxisDialog()));

	actionShowGridDialog = new QAction(tr("&Grid ..."), this);
	connect(actionShowGridDialog, SIGNAL(activated()), this, SLOT(showGridDialog()));

	actionShowTitleDialog = new QAction(tr("&Title ..."), this);
	connect(actionShowTitleDialog, SIGNAL(activated()), this, SLOT(showTitleDialog()));

	actionShowColumnOptionsDialog = new QAction(tr("Column &Options ..."), this);
	actionShowColumnOptionsDialog->setShortcut(tr("Ctrl+Alt+O"));
	connect(actionShowColumnOptionsDialog, SIGNAL(activated()), this, SLOT(showColumnOptionsDialog()));

	actionShowColumnValuesDialog = new QAction(tr("Set Column &Values ..."), this);
	connect(actionShowColumnValuesDialog, SIGNAL(activated()), this, SLOT(showColumnValuesDialog()));

	actionShowColumnValuesDialog->setShortcut(tr("Ctrl+Q"));
	actionTableRecalculate = new QAction(tr("Recalculate"), this);
	actionTableRecalculate->setShortcut(tr("Ctrl+Return"));
	connect(actionTableRecalculate, SIGNAL(activated()), this, SLOT(recalculateTable()));

	actionShowColsDialog = new QAction(tr("&Columns..."), this);
	connect(actionShowColsDialog, SIGNAL(activated()), this, SLOT(showColsDialog()));

	actionShowRowsDialog = new QAction(tr("&Rows..."), this);
	connect(actionShowRowsDialog, SIGNAL(activated()), this, SLOT(showRowsDialog()));

	actionAbout = new QAction(tr("&About QtiPlot"), this);
	actionAbout->setShortcut( tr("F1") );
	connect(actionAbout, SIGNAL(activated()), this, SLOT(about()));

	actionShowHelp = new QAction(tr("&Help"), this);
	actionShowHelp->setShortcut( tr("Ctrl+H") );
	connect(actionShowHelp, SIGNAL(activated()), this, SLOT(showHelp()));

	actionChooseHelpFolder = new QAction(tr("&Choose Help Folder..."), this);
	connect(actionChooseHelpFolder, SIGNAL(activated()), this, SLOT(chooseHelpFolder()));

	actionRename = new QAction(tr("&Rename Window"), this);
	connect(actionRename, SIGNAL(activated()), this, SLOT(rename()));

	actionCloseWindow = new QAction(QIcon(QPixmap(close_xpm)), tr("Close &Window"), this);
	actionCloseWindow->setShortcut( tr("Ctrl+W") );
	connect(actionCloseWindow, SIGNAL(activated()), this, SLOT(closeActiveWindow()));

	actionAddColToTable = new QAction(QIcon(QPixmap(addCol_xpm)), tr("Add Column"), this);
	connect(actionAddColToTable, SIGNAL(activated()), this, SLOT(addColToTable()));

	actionDeleteLayer = new QAction(QIcon(QPixmap(erase_xpm)), tr("&Remove Layer"), this);
	actionDeleteLayer->setShortcut( tr("Alt+R") );
	connect(actionDeleteLayer, SIGNAL(activated()), this, SLOT(deleteLayer()));

	actionResizeActiveWindow = new QAction(QIcon(QPixmap(resize_xpm)), tr("Window &Geometry..."), this);
	connect(actionResizeActiveWindow, SIGNAL(activated()), this, SLOT(resizeActiveWindow()));

	actionHideActiveWindow = new QAction(tr("&Hide Window"), this);
	connect(actionHideActiveWindow, SIGNAL(activated()), this, SLOT(hideActiveWindow()));

	actionShowMoreWindows = new QAction(tr("More windows..."), this);
	connect(actionShowMoreWindows, SIGNAL(activated()), this, SLOT(showMoreWindows()));

	actionPixelLineProfile = new QAction(QIcon(QPixmap(pixelProfile_xpm)), tr("&View Pixel Line Profile"), this);
	connect(actionPixelLineProfile, SIGNAL(activated()), this, SLOT(pixelLineProfile()));

	actionIntensityTable = new QAction(tr("&Intensity Table"), this);
	connect(actionIntensityTable, SIGNAL(activated()), this, SLOT(intensityTable()));

	actionShowLineDialog = new QAction(tr("&Properties"), this);
	connect(actionShowLineDialog, SIGNAL(activated()), this, SLOT(showLineDialog()));

	actionShowImageDialog = new QAction(tr("&Properties"), this);
	connect(actionShowImageDialog, SIGNAL(activated()), this, SLOT(showImageDialog()));

	actionShowTextDialog = new QAction(tr("&Properties"), this);
	connect(actionShowTextDialog, SIGNAL(activated()), this, SLOT(showTextDialog()));

	actionActivateWindow = new QAction(tr("&Activate Window"), this);
	connect(actionActivateWindow, SIGNAL(activated()), this, SLOT(activateWindow()));

	actionMinimizeWindow = new QAction(tr("Mi&nimize Window"), this);
	connect(actionMinimizeWindow, SIGNAL(activated()), this, SLOT(minimizeWindow()));

	actionMaximizeWindow = new QAction(tr("Ma&ximize Window"), this);
	connect(actionMaximizeWindow, SIGNAL(activated()), this, SLOT(maximizeWindow()));

	actionHideWindow = new QAction(tr("&Hide Window"), this);
	connect(actionHideWindow, SIGNAL(activated()), this, SLOT(hideWindow()));

	actionResizeWindow = new QAction(QIcon(QPixmap(resize_xpm)), tr("Re&size Window..."), this);
	connect(actionResizeWindow, SIGNAL(activated()), this, SLOT(resizeWindow()));

	actionPrintWindow = new QAction(QIcon(QPixmap(fileprint_xpm)),tr("&Print Window"), this);
	connect(actionPrintWindow, SIGNAL(activated()), this, SLOT(printWindow()));

	actionShowPlotGeometryDialog = new QAction(QIcon(QPixmap(resize_xpm)), tr("&Layer Geometry"), this);
	connect(actionShowPlotGeometryDialog, SIGNAL(activated()), this, SLOT(showPlotGeometryDialog()));

	actionEditSurfacePlot = new QAction(tr("&Surface..."), this);
	connect(actionEditSurfacePlot, SIGNAL(activated()), this, SLOT(editSurfacePlot()));

	actionAdd3DData = new QAction(tr("&Data Set..."), this);
	connect(actionAdd3DData, SIGNAL(activated()), this, SLOT(add3DData()));

	actionSetMatrixProperties = new QAction(tr("Set &Properties..."), this);
	connect(actionSetMatrixProperties, SIGNAL(activated()), this, SLOT(showMatrixDialog()));

	actionSetMatrixDimensions = new QAction(tr("Set &Dimensions..."), this);
	connect(actionSetMatrixDimensions, SIGNAL(activated()), this, SLOT(showMatrixSizeDialog()));

	actionSetMatrixValues = new QAction(tr("Set &Values..."), this);
	connect(actionSetMatrixValues, SIGNAL(activated()), this, SLOT(showMatrixValuesDialog()));

	actionTransposeMatrix = new QAction(tr("&Transpose"), this);
	connect(actionTransposeMatrix, SIGNAL(activated()), this, SLOT(transposeMatrix()));

	actionInvertMatrix = new QAction(tr("&Invert"), this);
	connect(actionInvertMatrix, SIGNAL(activated()), this, SLOT(invertMatrix()));

	actionMatrixDeterminant = new QAction(tr("&Determinant"), this);
	connect(actionMatrixDeterminant, SIGNAL(activated()), this, SLOT(matrixDeterminant()));

	actionConvertMatrix = new QAction(tr("&Convert to Spreadsheet"), this);
	connect(actionConvertMatrix, SIGNAL(activated()), this, SLOT(convertMatrixToTable()));

	actionConvertTable= new QAction(tr("Convert to &Matrix"), this);
	connect(actionConvertTable, SIGNAL(activated()), this, SLOT(convertTableToMatrix()));

	actionPlot3DWireFrame = new QAction(QIcon(QPixmap(lineMesh_xpm)), tr("3D &Wire Frame"), this);
	connect(actionPlot3DWireFrame, SIGNAL(activated()), this, SLOT(plot3DWireframe()));

	actionPlot3DHiddenLine = new QAction(QIcon(QPixmap(grid_only_xpm)), tr("3D &Hidden Line"), this);
	connect(actionPlot3DHiddenLine, SIGNAL(activated()), this, SLOT(plot3DHiddenLine()));

	actionPlot3DPolygons = new QAction(QIcon(QPixmap(no_grid_xpm)), tr("3D &Polygons"), this);
	connect(actionPlot3DPolygons, SIGNAL(activated()), this, SLOT(plot3DPolygons()));

	actionPlot3DWireSurface = new QAction(QIcon(QPixmap(grid_poly_xpm)), tr("3D Wire &Surface"), this);
	connect(actionPlot3DWireSurface, SIGNAL(activated()), this, SLOT(plot3DWireSurface()));

	actionSortTable = new QAction(tr("Sort Ta&ble"), this);
	connect(actionSortTable, SIGNAL(activated()), this, SLOT(sortActiveTable()));

	actionSortSelection = new QAction(tr("Sort Columns"), this);
	connect(actionSortSelection, SIGNAL(activated()), this, SLOT(sortSelection()));

	actionNormalizeTable = new QAction(tr("&Table"), this);
	connect(actionNormalizeTable, SIGNAL(activated()), this, SLOT(normalizeActiveTable()));

	actionNormalizeSelection = new QAction(tr("&Columns"), this);
	connect(actionNormalizeSelection, SIGNAL(activated()), this, SLOT(normalizeSelection()));

	actionCorrelate = new QAction(tr("Co&rrelate"), this);
	connect(actionCorrelate, SIGNAL(activated()), this, SLOT(correlate()));

	actionConvolute = new QAction(tr("&Convolute"), this);
	connect(actionConvolute, SIGNAL(activated()), this, SLOT(convolute()));

	actionDeconvolute = new QAction(tr("&Deconvolute"), this);
	connect(actionDeconvolute, SIGNAL(activated()), this, SLOT(deconvolute()));

	actionTranslateHor = new QAction(tr("&Horizontal"), this);
	connect(actionTranslateHor, SIGNAL(activated()), this, SLOT(translateCurveHor()));

	actionTranslateVert = new QAction(tr("&Vertical"), this);
	connect(actionTranslateVert, SIGNAL(activated()), this, SLOT(translateCurveVert()));

	actionSetAscValues = new QAction(QIcon(QPixmap(rowNumbers_xpm)),tr("Ro&w Numbers"), this);
	connect(actionSetAscValues, SIGNAL(activated()), this, SLOT(setAscValues()));

	actionSetRandomValues = new QAction(QIcon(QPixmap(randomNumbers_xpm)),tr("&Random Values"), this);
	connect(actionSetRandomValues, SIGNAL(activated()), this, SLOT(setRandomValues()));

	actionSetXCol = new QAction(tr("&X"), this);
	connect(actionSetXCol, SIGNAL(activated()), this, SLOT(setXCol()));

	actionSetYCol = new QAction(tr("&Y"), this);
	connect(actionSetYCol, SIGNAL(activated()), this, SLOT(setYCol()));

	actionSetZCol = new QAction(tr("&Z"), this);
	connect(actionSetZCol, SIGNAL(activated()), this, SLOT(setZCol()));

	actionSetXErrCol = new QAction(tr("X E&rror"), this);
	connect(actionSetXErrCol, SIGNAL(activated()), this, SLOT(setXErrCol()));

	actionSetYErrCol = new QAction(QIcon(QPixmap(errors_xpm)), tr("Y &Error"), this);
	connect(actionSetYErrCol, SIGNAL(activated()), this, SLOT(setYErrCol()));

	actionDisregardCol = new QAction(tr("&None"), this);
	connect(actionDisregardCol, SIGNAL(activated()), this, SLOT(disregardCol()));

	actionBoxPlot = new QAction(QIcon(QPixmap(boxPlot_xpm)),tr("&Box Plot"), this);
	connect(actionBoxPlot, SIGNAL(activated()), this, SLOT(plotBoxDiagram()));

	actionMultiPeakGauss = new QAction(tr("&Gaussian..."), this);
	connect(actionMultiPeakGauss, SIGNAL(activated()), this, SLOT(fitMultiPeakGauss()));

	actionMultiPeakLorentz = new QAction(tr("&Lorentzian..."), this);
	connect(actionMultiPeakLorentz, SIGNAL(activated()), this, SLOT(fitMultiPeakLorentz()));

	actionCheckUpdates = new QAction(tr("Search for &Updates"), this);
	connect(actionCheckUpdates, SIGNAL(activated()), this, SLOT(searchForUpdates()));

	actionHomePage = new QAction(tr("&QtiPlot Homepage"), this);
	connect(actionHomePage, SIGNAL(activated()), this, SLOT(showHomePage()));

	actionHelpForums = new QAction(tr("QtiPlot &Forums"), this);
	connect(actionHelpForums, SIGNAL(triggered()), this, SLOT(showForums()));

	actionHelpBugReports = new QAction(tr("Report a &Bug"), this);
	connect(actionHelpBugReports, SIGNAL(triggered()), this, SLOT(showBugTracker()));

	actionDownloadManual = new QAction(tr("Download &Manual"), this);
	connect(actionDownloadManual, SIGNAL(activated()), this, SLOT(downloadManual()));

	actionTranslations = new QAction(tr("&Translations"), this);
	connect(actionTranslations, SIGNAL(activated()), this, SLOT(downloadTranslation()));

	actionDonate = new QAction(tr("Make a &Donation"), this);
	connect(actionDonate, SIGNAL(activated()), this, SLOT(showDonationsPage()));

	actionTechnicalSupport = new QAction(tr("Technical &Support"), this);
	connect(actionTechnicalSupport, SIGNAL(activated()), this, SLOT(showSupportPage()));

#ifdef SCRIPTING_DIALOG
	actionScriptingLang = new QAction(tr("Scripting &language"), this);
	connect(actionScriptingLang, SIGNAL(activated()), this, SLOT(showScriptingLangDialog()));
#endif

	actionRestartScripting = new QAction(tr("&Restart scripting"), this);
	connect(actionRestartScripting, SIGNAL(activated()), this, SLOT(restartScriptingEnv()));

	actionNoteExecute = new QAction(tr("E&xecute"), this);
	actionNoteExecute->setShortcut(tr("Ctrl+J"));

	actionNoteExecuteAll = new QAction(tr("Execute &All"), this);
	actionNoteExecuteAll->setShortcut(tr("Ctrl+Shift+J"));

	actionNoteEvaluate = new QAction(tr("&Evaluate Expression"), this);
	actionNoteEvaluate->setShortcut(tr("Ctrl+Return"));
}

void ApplicationWindow::translateActionsStrings()
{
	actionNewProject->setMenuText(tr("New &Project"));
	actionNewProject->setToolTip(tr("Open a new project"));
	actionNewProject->setShortcut(tr("Ctrl+N"));

	actionNewGraph->setMenuText(tr("New &Graph"));
	actionNewGraph->setToolTip(tr("Create an empty 2D plot"));
	actionNewGraph->setShortcut(tr("Ctrl+G"));	

	actionNewNote->setMenuText(tr("New &Note"));
	actionNewNote->setToolTip(tr("Create an empty note window"));

	actionNewTable->setMenuText(tr("New &Table"));
	actionNewTable->setShortcut(tr("Ctrl+T"));
	actionNewTable->setToolTip(tr("New table"));

	actionNewMatrix->setMenuText(tr("New &Matrix"));
	actionNewMatrix->setShortcut(tr("Ctrl+M"));
	actionNewMatrix->setToolTip(tr("New matrix"));

	actionNewFunctionPlot->setMenuText(tr("New &Function Plot"));
	actionNewFunctionPlot->setToolTip(tr("Create a new 2D function plot"));
	actionNewFunctionPlot->setShortcut(tr("Ctrl+F"));

	actionNewSurfacePlot->setMenuText(tr("New 3D &Surface Plot"));
	actionNewSurfacePlot->setToolTip(tr("Create a new 3D surface plot"));
	actionNewSurfacePlot->setShortcut(tr("Ctrl+ALT+Z"));

	actionOpen->setMenuText(tr("&Open"));
	actionOpen->setShortcut(tr("Ctrl+O"));
	actionOpen->setToolTip(tr("Open project"));

	actionLoadImage->setMenuText(tr("Open Image &File"));
	actionLoadImage->setShortcut(tr("Ctrl+I"));

	actionImportImage->setMenuText(tr("Import I&mage..."));

	actionSaveProject->setMenuText(tr("&Save Project"));
	actionSaveProject->setToolTip(tr("Save project"));
	actionSaveProject->setShortcut(tr("Ctrl+S"));

	actionSaveProjectAs->setMenuText(tr("Save Project &As..."));

	actionOpenTemplate->setMenuText(tr("Open Te&mplate..."));
	actionOpenTemplate->setToolTip(tr("Open template"));

	actionSaveTemplate->setMenuText(tr("Save As &Template..."));
	actionSaveTemplate->setToolTip(tr("Save window as template"));

	actionLoad->setMenuText(tr("&Single File..."));
	actionLoad->setToolTip(tr("Import data file"));

	actionLoadMultiple->setMenuText(tr("&Multiple Files..."));
	actionLoadMultiple->setToolTip(tr("Import multiple data files"));

	actionUndo->setMenuText(tr("&Undo"));
	actionUndo->setToolTip(tr("Undo changes"));
	actionUndo->setShortcut(tr("Ctrl+Z"));

	actionRedo->setMenuText(tr("&Redo"));
	actionRedo->setToolTip(tr("Redo changes"));
	actionRedo->setShortcut(tr("Ctrl+R"));

	actionCopyWindow->setMenuText(tr("&Duplicate"));
	actionCopyWindow->setToolTip(tr("Duplicate window"));

	actionCutSelection->setMenuText(tr("Cu&t Selection"));
	actionCutSelection->setToolTip(tr("Cut selection"));
	actionCutSelection->setShortcut(tr("Ctrl+X"));

	actionCopySelection->setMenuText(tr("&Copy Selection"));
	actionCopySelection->setToolTip(tr("Copy selection"));
	actionCopySelection->setShortcut(tr("Ctrl+C"));

	actionPasteSelection->setMenuText(tr("&Paste Selection"));
	actionPasteSelection->setToolTip(tr("Paste selection"));
	actionPasteSelection->setShortcut(tr("Ctrl+V"));

	actionClearSelection->setMenuText(tr("&Delete Selection"));
	actionClearSelection->setToolTip(tr("Delete selection"));
	actionClearSelection->setShortcut(tr("Del","delete key"));

	actionShowExplorer->setMenuText(tr("Project &Explorer"));
	actionShowExplorer->setShortcut(tr("Ctrl+E"));
	actionShowExplorer->setToolTip(tr("Show project explorer"));

	actionShowLog->setMenuText(tr("Results &Log"));
	actionShowLog->setToolTip(tr("Show analysis results"));

#ifdef SCRIPTING_CONSOLE
	actionShowConsole->setMenuText(tr("&Console"));
	actionShowConsole->setToolTip(tr("Show Scripting console"));
#endif

	actionAddLayer->setMenuText(tr("Add La&yer"));
	actionAddLayer->setShortcut(tr("ALT+L"));

	actionShowLayerDialog->setMenuText(tr("Arran&ge Layers"));
	actionShowLayerDialog->setShortcut(tr("ALT+A"));
	
	actionAutomaticLayout->setMenuText(tr("Automatic Layout"));

	actionExportGraph->setMenuText(tr("&Current"));
	actionExportGraph->setShortcut(tr("Alt+G"));
	actionExportGraph->setToolTip(tr("Export current graph"));

	actionExportAllGraphs->setMenuText(tr("&All")); 
	actionExportAllGraphs->setShortcut(tr("Alt+X"));
	actionExportAllGraphs->setToolTip(tr("Export all graphs"));

	actionPrint->setMenuText(tr("&Print")); 
	actionPrint->setShortcut(tr("Ctrl+P"));
	actionPrint->setToolTip(tr("Print window"));

	actionPrintAllPlots->setMenuText(tr("Print All Plo&ts"));
	actionShowExportASCIIDialog->setMenuText(tr("E&xport ASCII"));
	actionShowImportDialog->setMenuText(tr("Set import &options"));

	actionCloseAllWindows->setMenuText(tr("&Quit")); 
	actionCloseAllWindows->setShortcut(tr("Alt+F4"));

	actionClearLogInfo->setMenuText(tr("Clear &Log Information"));
	actionDeleteFitTables->setMenuText(tr("Delete &Fit Tables"));

	actionShowPlotWizard->setMenuText(tr("Plot &Wizard")); 
	actionShowPlotWizard->setShortcut(tr("Ctrl+Alt+W"));

	actionShowConfigureDialog->setMenuText(tr("&Preferences..."));

	actionShowCurvesDialog->setMenuText(tr("Add/Remove &Curve...")); 
	actionShowCurvesDialog->setShortcut(tr("ALT+C"));
	actionShowCurvesDialog->setToolTip(tr("Add curve to graph"));

	actionAddErrorBars->setMenuText(tr("Add &Error Bars...")); 
	actionAddErrorBars->setShortcut(tr("Ctrl+B"));

	actionAddFunctionCurve->setMenuText(tr("Add &Function...")); 
	actionAddFunctionCurve->setShortcut(tr("Ctrl+Alt+F"));

	actionUnzoom->setMenuText(tr("&Rescale to Show All")); 
	actionUnzoom->setShortcut(tr("Ctrl+Shift+R"));
	actionUnzoom->setToolTip(tr("Best fit"));

	actionNewLegend->setMenuText( tr("New &Legend")); 
	actionNewLegend->setShortcut(tr("Ctrl+L"));
	actionNewLegend->setToolTip(tr("Add new legend"));

	actionTimeStamp->setMenuText(tr("Add Time Stamp")); 
	actionTimeStamp->setShortcut(tr("Ctrl+ALT+T"));
	actionTimeStamp->setToolTip(tr("Date && time "));

	actionAddImage->setMenuText(tr("Add &Image")); 
	actionAddImage->setShortcut(tr("ALT+I"));

	actionPlotL->setMenuText(tr("&Line"));
	actionPlotL->setToolTip(tr("Plot as line"));

	actionPlotP->setMenuText(tr("&Scatter"));
	actionPlotP->setToolTip(tr("Plot as symbols"));

	actionPlotLP->setMenuText(tr("Line + S&ymbol"));
	actionPlotLP->setToolTip(tr("Plot as line + symbols"));

	actionPlotVerticalDropLines->setMenuText(tr("Vertical &Drop Lines"));

	actionPlotSpline->setMenuText(tr("&Spline"));
	actionPlotVertSteps->setMenuText(tr("&Vertical Steps"));
	actionPlotHorSteps->setMenuText(tr("&Horizontal Steps"));

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
	actionPlot3DRibbon->setToolTip(tr("Plot 3D ribbon"));

	actionPlot3DBars->setMenuText(tr("&Bars"));
	actionPlot3DBars->setToolTip(tr("Plot 3D bars"));

	actionPlot3DScatter->setMenuText(tr("&Scatter"));
	actionPlot3DScatter->setToolTip(tr("Plot 3D scatter"));

	actionPlot3DTrajectory->setMenuText(tr("&Trajectory"));
	actionPlot3DTrajectory->setToolTip(tr("Plot 3D trajectory"));

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

	actionShowFitDialog->setMenuText(tr("&Nonlinear Curve Fit ...")); 
	actionShowFitDialog->setShortcut(tr("Ctrl+Y"));

	actionShowPlotDialog->setMenuText(tr("&Plot ..."));
	actionShowScaleDialog->setMenuText(tr("&Scales..."));
	actionShowCurveFormatDialog->setMenuText(tr("&Curves..."));
	actionShowAxisDialog->setMenuText(tr("&Axes..."));
	actionShowGridDialog->setMenuText(tr("&Grid ..."));
	actionShowTitleDialog->setMenuText(tr("&Title ..."));
	actionShowColumnOptionsDialog->setMenuText(tr("Column &Options ..."));
	actionShowColumnOptionsDialog->setShortcut(tr("Ctrl+Alt+O"));
	actionShowColumnValuesDialog->setMenuText(tr("Set Column &Values ..."));
	actionShowColumnValuesDialog->setShortcut(tr("Ctrl+Q"));
	actionTableRecalculate->setMenuText(tr("Recalculate"));
	actionShowColsDialog->setMenuText(tr("&Columns..."));
	actionShowRowsDialog->setMenuText(tr("&Rows..."));

	actionAbout->setMenuText(tr("&About QtiPlot")); 
	actionAbout->setShortcut(tr("F1"));

	actionShowHelp->setMenuText(tr("&Help")); 
	actionShowHelp->setShortcut(tr("Ctrl+H"));

	actionChooseHelpFolder->setMenuText(tr("&Choose Help Folder..."));
	actionRename->setMenuText(tr("&Rename Window"));

	actionCloseWindow->setMenuText(tr("Close &Window"));
	actionCloseWindow->setShortcut(tr("Ctrl+W"));

	actionAddColToTable->setMenuText(tr("Add Column"));

	actionDeleteLayer->setMenuText(tr("&Remove Layer"));
	actionDeleteLayer->setShortcut(tr("Alt+R"));

	actionResizeActiveWindow->setMenuText(tr("Window &Geometry..."));
	actionHideActiveWindow->setMenuText(tr("&Hide Window"));
	actionShowMoreWindows->setMenuText(tr("More Windows..."));
	actionPixelLineProfile->setMenuText(tr("&View Pixel Line Profile"));
	actionIntensityTable->setMenuText(tr("&Intensity Table"));
	actionShowLineDialog->setMenuText(tr("&Properties"));
	actionShowImageDialog->setMenuText(tr("&Properties"));
	actionShowTextDialog->setMenuText(tr("&Properties"));
	actionActivateWindow->setMenuText(tr("&Activate Window"));
	actionMinimizeWindow->setMenuText(tr("Mi&nimize Window"));
	actionMaximizeWindow->setMenuText(tr("Ma&ximize Window"));
	actionHideWindow->setMenuText(tr("&Hide Window"));
	actionResizeWindow->setMenuText(tr("Re&size Window..."));
	actionPrintWindow->setMenuText(tr("&Print Window"));
	actionShowPlotGeometryDialog->setMenuText(tr("&Layer Geometry"));
	actionEditSurfacePlot->setMenuText(tr("&Surface..."));
	actionAdd3DData->setMenuText(tr("&Data Set..."));
	actionSetMatrixProperties->setMenuText(tr("Set &Properties..."));
	actionSetMatrixDimensions->setMenuText(tr("Set &Dimensions..."));
	actionSetMatrixValues->setMenuText(tr("Set &Values..."));
	actionTransposeMatrix->setMenuText(tr("&Transpose"));
	actionInvertMatrix->setMenuText(tr("&Invert"));
	actionMatrixDeterminant->setMenuText(tr("&Determinant"));
	actionConvertMatrix->setMenuText(tr("&Convert to Spreadsheet"));
	actionConvertTable->setMenuText(tr("Convert to &Matrix"));
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
	actionSetXCol->setMenuText(tr("&X"));
	actionSetYCol->setMenuText(tr("&Y"));
	actionSetZCol->setMenuText(tr("&Z"));
	actionSetXErrCol->setMenuText(tr("X E&rror"));
	actionSetYErrCol->setMenuText(tr("Y &Error"));
	actionDisregardCol->setMenuText(tr("&None"));

	actionBoxPlot->setMenuText(tr("&Box Plot"));
	actionBoxPlot->setToolTip(tr("Box and whiskers plot"));

	actionMultiPeakGauss->setMenuText(tr("&Gaussian..."));
	actionMultiPeakLorentz->setMenuText(tr("&Lorentzian..."));
	actionHomePage->setMenuText(tr("&QtiPlot Homepage"));
	actionCheckUpdates->setMenuText(tr("Search for &Updates"));
	actionHelpForums->setText(tr("Visit QtiPlot &Forums"));
	actionHelpBugReports->setText(tr("Report a &Bug"));
	actionDownloadManual->setMenuText(tr("Download &Manual"));
	actionTranslations->setMenuText(tr("&Translations"));
	actionDonate->setMenuText(tr("Make a &Donation"));
	actionTechnicalSupport->setMenuText(tr("Technical &Support"));

#ifdef SCRIPTING_DIALOG
	actionScriptingLang->setMenuText(tr("Scripting &language"));
#endif
	actionRestartScripting->setMenuText(tr("&Restart scripting"));

	actionNoteExecute->setMenuText(tr("E&xecute"));
	actionNoteExecute->setShortcut(tr("Ctrl+J"));

	actionNoteExecuteAll->setMenuText(tr("Execute &All"));
	actionNoteExecuteAll->setShortcut(tr("Ctrl+Shift+J"));

	actionNoteEvaluate->setMenuText(tr("&Evaluate Expression"));
	actionNoteEvaluate->setShortcut(tr("Ctrl+Return"));

	btnPointer->setMenuText(tr("Disable &tools"));
	btnPointer->setToolTip( tr( "Pointer" ) );

	btnZoomIn->setMenuText(tr("&Zoom In"));
	btnZoomIn->setShortcut(tr("Ctrl++"));
	btnZoomIn->setToolTip(tr("Zoom In"));

	btnZoomOut->setMenuText(tr("Zoom &Out"));
	btnZoomOut->setShortcut(tr("Ctrl+-"));
	btnZoomOut->setToolTip(tr("Zoom Out"));

	btnCursor->setMenuText(tr("&Data Reader"));
	btnCursor->setShortcut(tr("CTRL+D"));
	btnCursor->setToolTip(tr("Data reader"));

	btnSelect->setMenuText(tr("&Select Data Range"));
	btnSelect->setShortcut(tr("ALT+S"));
	btnSelect->setToolTip(tr("Select data range"));

	btnPicker->setMenuText(tr("S&creen Reader"));
	btnPicker->setToolTip(tr("Screen reader"));

	btnMovePoints->setMenuText(tr("&Move Data Points..."));
	btnMovePoints->setShortcut(tr("Ctrl+ALT+M"));
	btnMovePoints->setToolTip(tr("Move data points"));

	btnRemovePoints->setMenuText(tr("Remove &Bad Data Points..."));
	btnRemovePoints->setShortcut(tr("Alt+B"));
	btnRemovePoints->setToolTip(tr("Remove data points"));

	actionAddText->setMenuText(tr("Add &Text"));
	actionAddText->setShortcut(tr("ALT+T"));

	btnArrow->setMenuText(tr("Draw &Arrow"));
	btnArrow->setShortcut(tr("CTRL+ALT+A"));
	btnArrow->setToolTip(tr("Draw arrow"));

	btnLine->setMenuText(tr("Draw &Line"));
	btnLine->setShortcut(tr("CTRL+ALT+L"));
	btnLine->setToolTip(tr("Draw line"));

	// FIXME: is setText necessary for action groups?
	//	coord->setText( tr( "Coordinates" ) );
	//	coord->setMenuText( tr( "&Coord" ) );
	//  coord->setStatusTip( tr( "Coordinates" ) );
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
	None->setToolTip( tr( "No axes" ) );
	None->setStatusTip( tr( "No axes" ) );

	//grids->setText( tr( "grid" ) );
	//grids->setMenuText( tr( "grid" ) );
	//grids->setStatusTip( tr( "grid" ) );
	front->setToolTip( tr( "Front grid" ) );
	back->setToolTip( tr( "Back grid" ) );
	right->setToolTip( tr( "Right grid" ) );
	left->setToolTip( tr( "Left grid" ) );
	ceil->setToolTip( tr( "Ceiling grid" ) );
	floor->setToolTip( tr( "Floor grid" ) );

	//plotstyle->setText( tr( "Plot Style" ) );
	//plotstyle->setMenuText( tr( "Plot Style" ) );
	//plotstyle->setStatusTip( tr( "Plot Style" ) );
	wireframe->setText( tr( "Wireframe" ) );
	wireframe->setMenuText( tr( "Wireframe" ) );
	wireframe->setToolTip( tr( "Wireframe" ) );
	wireframe->setStatusTip( tr( "Wireframe" ) );
	hiddenline->setText( tr( "Hidden Line" ) );
	hiddenline->setMenuText( tr( "Hidden Line" ) );
	hiddenline->setToolTip( tr( "Hidden line" ) );
	hiddenline->setStatusTip( tr( "Hidden line" ) );
	polygon->setText( tr( "Polygon Only" ) );
	polygon->setMenuText( tr( "Polygon Only" ) );
	polygon->setToolTip( tr( "Polygon only" ) );
	polygon->setStatusTip( tr( "Polygon only" ) );
	filledmesh->setText( tr( "Mesh & Filled Polygons" ) );
	filledmesh->setMenuText( tr( "Mesh & Filled Polygons" ) );
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
	crossHairStyle->setText( tr( "Crosshairs" ) );
	crossHairStyle->setMenuText( tr( "Crosshairs" ) );
	crossHairStyle->setToolTip( tr( "Crosshairs" ) );
	crossHairStyle->setStatusTip( tr( "Crosshairs" ) );

	//floorstyle->setText( tr( "Floor Style" ) );
	//floorstyle->setMenuText( tr( "Floor Style" ) );
	//floorstyle->setStatusTip( tr( "Floor Style" ) );
	floordata->setText( tr( "Floor Data Projection" ) );
	floordata->setMenuText( tr( "Floor Data Projection" ) );
	floordata->setToolTip( tr( "Floor data projection" ) );
	floordata->setStatusTip( tr( "Floor data projection" ) );
	flooriso->setText( tr( "Floor Isolines" ) );
	flooriso->setMenuText( tr( "Floor Isolines" ) );
	flooriso->setToolTip( tr( "Floor isolines" ) );
	flooriso->setStatusTip( tr( "Floor isolines" ) );
	floornone->setText( tr( "Empty Floor" ) );
	floornone->setMenuText( tr( "Empty Floor" ) );
	floornone->setToolTip( tr( "Empty floor" ) );
	floornone->setStatusTip( tr( "Empty floor" ) );
	
	actionAnimate->setText( tr( "Animation" ) );
    actionAnimate->setMenuText( tr( "Animation" ) );
    actionAnimate->setToolTip( tr( "Animation" ) );
    actionAnimate->setStatusTip( tr( "Animation" ) );
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

	Graph3D *plot=new Graph3D("", ws, 0, 0);
	plot->setAttribute(Qt::WA_DeleteOnClose);
	plot->setWindowTitle(caption);
	plot->setName(caption);
	plot->addMatrixData(m, xl, xr, yl, yr, zl, zr);
	plot->update();

	initPlot3D(plot);
	return plot;
}

void ApplicationWindow::plot3DMatrix(int style)
{
	if (!ws->activeWindow()|| !ws->activeWindow()->isA("Matrix"))
		return;

	QApplication::setOverrideCursor(Qt::WaitCursor);
	QString label="graph"+QString::number(++graphs);
	while (alreadyUsedName(label)){
		label="graph"+QString::number(++graphs);}

	Graph3D *plot=new Graph3D("", ws, 0);
	plot->setAttribute(Qt::WA_DeleteOnClose);
	plot->addMatrixData((Matrix*)ws->activeWindow());
	plot->customPlotStyle(style);
	customPlot3D(plot);
	plot->update();

	plot->resize(500,400);
	plot->setWindowTitle(label);
	plot->setName(label);
	initPlot3D(plot);

	emit modified();
	QApplication::restoreOverrideCursor();
}

ApplicationWindow* ApplicationWindow::importOPJ(const QString& filename)
{
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	ApplicationWindow *app = new ApplicationWindow();
	app->applyUserSettings();
	app->setWindowTitle("QtiPlot - " + filename);
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
	foreach(QWidget *w, *windows)
	{
		QString caption = w->name();
		if (w->isA("Table") && (caption.startsWith(tr("Fit")) || caption.startsWith(tr("LinearFit"))))
		{
			((Table*)w)->askOnCloseEvent(false);
			((Table*)w)->close();
		}
	}
	delete windows;
}

QWidgetList* ApplicationWindow::windowsList()
{
	QWidgetList *lst = new QWidgetList;

	QWidgetList windows = ws->windowList(QWorkspace::StackingOrder);
	for (int i = 0; i<(int)windows.count(); i++ )
		lst->append(windows.at(i));

	for (int j = 0; j<(int)outWindows->count(); j++ )
		lst->append(outWindows->at(j));

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
	if (!w || !w->isA("MultiLayer"))
		return;

	MultiLayer *plot = (MultiLayer*)w;
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	Graph* g = (Graph*)plot->activeGraph();
	if (!g)
		return;

	if (g->isPiePlot())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));

		btnPointer->setChecked(true);
		return;
	}
	else
	{	
		activeGraph=g;
		btnPointer->setChecked(true);
		g->translateCurve(0);
		info->setText(tr("Double-click on plot to select a data point!"));
		displayBar->show();
	}
}

void ApplicationWindow::translateCurveVert()
{
	QWidget *w=ws->activeWindow();
	if (!w || !w->isA("MultiLayer"))
		return;

	MultiLayer *plot = (MultiLayer*)w;
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
		return;
	}

	Graph* g = (Graph*)plot->activeGraph();
	if (!g)
		return;

	if (g->isPiePlot())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("This functionality is not available for pie plots!"));

		btnPointer->setChecked(true);
		return;
	}
	else
	{	
		activeGraph=g;
		btnPointer->setChecked(true);
		g->translateCurve(1);
		info->setText(tr("Double-click on plot to select a data point!"));
		displayBar->show();
	}
}

void ApplicationWindow::setAscValues()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("Table"))
		return;

	((Table *)ws->activeWindow())->setAscValues();
}

void ApplicationWindow::setRandomValues()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("Table"))
		return;

	((Table *)ws->activeWindow())->setRandomValues();
}

void ApplicationWindow::setXErrCol()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table *)ws->activeWindow())->setPlotDesignation(Table::xErr);
}

void ApplicationWindow::setYErrCol()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table *)ws->activeWindow())->setPlotDesignation(Table::yErr);
}

void ApplicationWindow::setXCol()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table *)ws->activeWindow())->setPlotDesignation(Table::X);
}

void ApplicationWindow::setYCol()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table *)ws->activeWindow())->setPlotDesignation(Table::Y);
}

void ApplicationWindow::setZCol()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table *)ws->activeWindow())->setPlotDesignation(Table::Z);
}

void ApplicationWindow::disregardCol()
{
	if (!ws->activeWindow() || !ws->activeWindow()->inherits("Table"))
		return;

	((Table *)ws->activeWindow())->setPlotDesignation(Table::None);
}

void ApplicationWindow::plotBoxDiagram()
{
	if (!ws->activeWindow() || !ws->activeWindow()->isA("Table"))
		return;

	((Table *)ws->activeWindow())->plotBoxDiagram();
}

void ApplicationWindow::fitMultiPeakGauss()
{
	fitMultiPeak((int)MultiPeakFit::Gauss);
}

void ApplicationWindow::fitMultiPeakLorentz()
{
	fitMultiPeak((int)MultiPeakFit::Lorentz);
}

void ApplicationWindow::fitMultiPeak(int profile)
{
	QWidget *w=ws->activeWindow();
	if (!w || !w->isA("MultiLayer"))
		return;

	MultiLayer *plot = (MultiLayer*)w;
	if (plot->isEmpty())
	{
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("<h4>There are no plot layers available in this window.</h4>"
					"<p><h4>Please add a layer and try again!</h4>"));
		btnPointer->setChecked(true);
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
				tr("Peaks"), 2, 2, 1000000, 1, &ok, this);
		if (ok && peaks) 
		{
			g->multiPeakFit(this, (int)profile, peaks);
			info->setText(tr("Move cursor and click to select a point and double-click/press 'Enter' to set the position of a peak!"));
			displayBar->show();
			connect (g,SIGNAL(showFitResults(const QString&)), this, SLOT(showResults(const QString&)));
		}
	}
}

void ApplicationWindow::showSupportPage()
{
	QDesktopServices::openUrl(QUrl("http://soft.proindependent.com/contracts.html"));
}


void ApplicationWindow::showDonationsPage()
{
	QDesktopServices::openUrl(QUrl("http://soft.proindependent.com/why_donate.html"));
}

void ApplicationWindow::downloadManual()
{
	QDesktopServices::openUrl(QUrl("http://soft.proindependent.com/manuals.html"));
}

void ApplicationWindow::downloadTranslation()
{
	QDesktopServices::openUrl(QUrl("http://soft.proindependent.com/translations.html"));
}

void ApplicationWindow::showHomePage()
{
	QDesktopServices::openUrl(QUrl("http://www.qtiplot.ro"));
}

void ApplicationWindow::showForums()
{
	QDesktopServices::openUrl(QUrl("https://developer.berlios.de/forum/?group_id=6626"));
}

void ApplicationWindow::showBugTracker()
{
	QDesktopServices::openUrl(QUrl("https://developer.berlios.de/bugs/?group_id=6626"));
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
		ApplicationWindow *aux = new ApplicationWindow();
		aux->hideActiveWindow();
		aux->showHelp();
		aux->saveSettings();//save any changes to the help folder path
		delete aux;
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
			setWindowTitle(tr("QtiPlot - untitled"));
			showMaximized();
			savedProject();
		}

		if (!locales.contains(locale))
			QMessageBox::critical(0, tr("QtiPlot - Error"),
					tr("<b> %1 </b>: Wrong locale option or no translation available!").arg(locale));
	}
	else
	{
		ApplicationWindow *aux = new ApplicationWindow();
		aux->hideActiveWindow();
		QMessageBox::critical(0, tr("QtiPlot - Error"),
				tr("<b> %1 </b>: Unknown command line option or the file doesn't exist!").arg(s));
		delete aux;
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
		qApp->removeTranslator(appTranslator);
		qApp->removeTranslator(qtTranslator);
		delete appTranslator;
		delete qtTranslator;
		appTranslator = new QTranslator(this);
		qtTranslator = new QTranslator(this);
		qApp->installTranslator(appTranslator);
		qApp->installTranslator(qtTranslator);
	}
	else
	{
		QString qmPath = qApp->applicationDirPath() + "/translations";
		appTranslator->load("qtiplot_" + locale, qmPath);
		qtTranslator->load("qt_" + locale, qmPath+"/qt");
	}
	insertTranslatedStrings();
}

QStringList ApplicationWindow::matrixNames()
{
	QStringList names;
	QWidgetList *windows = windowsList();
	for (int i=0; i<(int)windows->count(); i++)
	{
		if (windows->at(i)->isA("Matrix"))
			names << windows->at(i)->name();
	}
	delete windows;
	return names;
}

bool ApplicationWindow::alreadyUsedName(const QString& label)
{
	QWidgetList *windows = windowsList();
	for (int i=0; i<(int)windows->count(); i++)
	{
		if (windows->at(i)->name() == label)
		{
			delete windows;
			return true;
		}
	}
	delete windows;
	return false;
}

bool ApplicationWindow::projectHas2DPlots()
{
	QWidgetList *windows = windowsList();
	for (int i=0; i<(int)windows->count(); i++)
	{
		if (windows->at(i)->isA("Graph"))
		{
			delete windows;
			return true;
		}
	}
	delete windows;
	return false;
}

bool ApplicationWindow::projectHas3DPlots()
{
	QWidgetList *windows = windowsList();
	for (int i=0; i<(int)windows->count(); i++)
	{
		if (windows->at(i)->isA("Graph3D"))
		{
			delete windows;
			return true;
		}
	}
	delete windows;
	return false;
}

void ApplicationWindow::appendProject()
{
	QString filter = tr("QtiPlot project") + " (*.qti);;";
	filter += tr("Compressed QtiPlot project") + " (*.qti.gz);;";
	filter += tr("Origin project") + " (*.opj *.OPJ);;";

	QString fn = Q3FileDialog::getOpenFileName(workingDir, filter, this, 0,
			tr("QtiPlot - Open project"), 0, true);

	if (fn.isEmpty())
		return;

	QFileInfo fi(fn);
	workingDir = fi.dirPath(true);

	if (fn.contains(".qti",true) || fn.contains(".opj",false))
	{
		QFileInfo f(fn);
		if (!f.exists ())
		{
			QMessageBox::critical(this, tr("QtiPlot - File opening error"),
					tr("The file: <b>%1</b> doesn't exist!").arg(fn));
			return;
		}
	}
	else
	{
		QMessageBox::critical(this,tr("QtiPlot - File opening error"),
				tr("The file: <b>%1</b> is not a QtiPlot or Origin project file!").arg(fn));
		return;
	}

	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QString fname = fn;
	if (fn.contains(".qti.gz"))
	{//decompress using zlib
		file_uncompress((char *)fname.ascii());
		fname.remove(".gz");
	}

	Folder *cf = current_folder;
	FolderListItem *item = (FolderListItem *)current_folder->folderListItem();
	folders->blockSignals (true);
	blockSignals (true);

	QString baseName = fi.baseName();
	QStringList lst = current_folder->subfolders();
	int n = lst.count(baseName);
	if (n)
	{//avoid identical subfolder names
		while (lst.count(baseName + QString::number(n)))
			n++;
		baseName += QString::number(n);
	}

	current_folder = new Folder(current_folder, baseName);
	FolderListItem *fli = new FolderListItem(item, current_folder);
	current_folder->setFolderListItem(fli);

	if (fn.contains(".opj", false))
		ImportOPJ(this, fn);
	else
	{
		QFile f(fname);
		QTextStream t( &f );
		t.setEncoding(QTextStream::UnicodeUTF8);
		f.open(QIODevice::ReadOnly);

		QString s = t.readLine();
		lst = QStringList::split (QRegExp("\\s"),s,false);
		QString version = lst[1];
		lst = QStringList::split (".", version, false);
		fileVersion =100*(lst[0]).toInt()+10*(lst[1]).toInt()+(lst[2]).toInt();

		t.readLine(); 
		if (fileVersion < 73)
			t.readLine();

		//process tables and matrix information
		while ( !t.atEnd())
		{
			s = t.readLine();
			lst.clear();
			if  (s.left(8) == "<folder>")
			{
				lst = QStringList::split ("\t",s,true);
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
				while ( s!="</table>" )
				{
					s=t.readLine();
					lst<<s;
				}
				lst.pop_back();
				openTable(this,lst);
			}
			else if  (s == "<matrix>")
			{
				while ( s != "</matrix>" )
				{
					s=t.readLine();
					lst<<s;
				}
				lst.pop_back();
				openMatrix(this, lst);
			}
			else if  (s == "<note>")
			{
				for (int i=0; i<3; i++)
				{
					s = t.readLine();
					lst << s;
				}
				Note* m = openNote(this, lst);
				QStringList cont;
				while ( s != "</note>" )
				{
					s=t.readLine();
					cont << s;
				}
				cont.pop_back();
				m->restore(cont);
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
		f.open(QIODevice::ReadOnly);

		MultiLayer *plot=0;
		while ( !t.atEnd())
		{
			s=t.readLine();
			if  (s.left(8) == "<folder>")
			{
				lst = QStringList::split ("\t",s,true);
				current_folder = current_folder->findSubfolder(lst[1]);
			}
			else if  (s == "<multiLayer>")
			{//process multilayers information
				s=t.readLine();
				QStringList graph=QStringList::split ("\t",s,true);
				QString caption=graph[0];
				plot=multilayerPlot(caption);
				plot->setCols(graph[1].toInt());
				plot->setRows(graph[2].toInt());
				setListViewDate(caption, graph[3]);
				plot->setBirthDate(graph[3]);
				plot->blockSignals(true);	

				restoreWindowGeometry(this, plot, t.readLine());

				if (fileVersion > 71)
				{
					QStringList lst=QStringList::split ("\t", t.readLine(), true);
					plot->setWindowLabel(lst[1]);
					setListViewLabel(plot->name(),lst[1]);
					plot->setCaptionPolicy((MyWidget::CaptionPolicy)lst[2].toInt());
				}

				if (caption.contains ("graph",true))
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
	}

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
		while (!f.open(QIODevice::ReadOnly) || !backup.open(QIODevice::WriteOnly))
		{
			if (f.isOpen()) 
				f.close();
			if (backup.isOpen()) 
				backup.close();
			int choice = QMessageBox::warning(this, tr("QtiPlot - File backup error"),
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

	if ( !f.open( QIODevice::WriteOnly ) )
	{
		QMessageBox::about(this, tr("QtiPlot - File save error"), tr("The file: <br><b>%1</b> is opened in read-only mode").arg(fn));
		return;
	}
	QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

	QList<MyWidget *> lst = folder->windowsList();
	int windows = 0;
	QString text;
	foreach(MyWidget *w, lst)
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
		foreach(MyWidget *w, lst)
		{
			text += w->saveToString(windowGeometryInfo(w));
			windows++;
		}

		if ( (dir->children()).isEmpty() )
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
	text.prepend("<scripting-lang>\t"+QString(scriptEnv->name())+"\n");
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
	QString fn = Q3FileDialog::getSaveFileName(workingDir, filter, this, "project",
			tr("Save project as"), &selectedFilter, false);
	if ( !fn.isEmpty() )
	{
		QFileInfo fi(fn);
		workingDir = fi.dirPath(true);
		QString baseName = fi.fileName();	
		if (!baseName.contains("."))
			fn.append(".qti");

		if ( QFile::exists(fn) && !selectedFilter.contains(".gz") &&
				QMessageBox::question(this, tr("QtiPlot - Overwrite file? "),
					tr("A file called: <p><b>%1</b><p>already exists.\n"
						"Do you want to overwrite it?")
					.arg(fn), tr("&Yes"), tr("&No"),QString(), 0, 1 ) )
			return ;
		else
		{
			saveFolder(f, fn);
			if (selectedFilter.contains(".gz"))
				file_compress((char *)fn.ascii(), "wb9");
		}
	}
}

void ApplicationWindow::showFolderPopupMenu(Q3ListViewItem *it, const QPoint &p, int)
{
	showFolderPopupMenu(it, p, true);
}

//! fromFolders = true means the user clicked right mouse buttom on a list iten from QListView "folders"
void ApplicationWindow::showFolderPopupMenu(Q3ListViewItem *it, const QPoint &p, bool fromFolders)
{
	if (!it || folders->isRenaming())
		return;

	QMenu cm(this);
	QMenu window(this);
	QMenu viewWindowsMenu(this);
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
		cm.insertItem(QPixmap(close_xpm), tr("&Delete Folder"), this, SLOT(deleteFolder()), Qt::Key_F8);
		cm.insertItem(tr("&Rename"), this, SLOT(startRenameFolder()), Qt::Key_F2);
		cm.insertSeparator();
	}

	if (fromFolders)
	{
		window.addAction(actionNewTable);
		window.addAction(actionNewMatrix);
		window.addAction(actionNewNote);
		window.addAction(actionNewGraph);
		window.addAction(actionNewFunctionPlot);
		window.addAction(actionNewSurfacePlot);
		cm.insertItem(tr("New &Window"), &window);
	}

	cm.insertItem(QPixmap(newfolder_xpm), tr("New F&older"), this, SLOT(addFolder()), Qt::Key_F7);
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
		QList<QWidget*> *lst = windowsList(); 
		foreach(QWidget *w, *lst)
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
	FindDialog *fd = new FindDialog(this);
	fd->setAttribute(Qt::WA_DeleteOnClose);
	fd->exec();
}

void ApplicationWindow::startRenameFolder()
{
	FolderListItem *fi = current_folder->folderListItem();
	if (!fi)
		return;

	disconnect(folders, SIGNAL(currentChanged(Q3ListViewItem *)), this, SLOT(folderItemChanged(Q3ListViewItem *)));
	fi->setRenameEnabled (0, true);
	fi->startRename (0);
}

void ApplicationWindow::startRenameFolder(Q3ListViewItem *item)
{
	if (!item || item == folders->firstChild())
		return;

	disconnect(folders, SIGNAL(currentChanged(Q3ListViewItem *)), this, SLOT(folderItemChanged(Q3ListViewItem *)));

	if (item->listView() == lv && item->rtti() == FolderListItem::RTTI)
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

void ApplicationWindow::renameFolder(Q3ListViewItem *it, int col, const QString &text)
{
	Q_UNUSED(col)

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
	connect(folders, SIGNAL(currentChanged(Q3ListViewItem *)), 
			this, SLOT(folderItemChanged(Q3ListViewItem *)));
	folders->setCurrentItem(parent->folderListItem());//update the list views
}

void ApplicationWindow::showAllFolderWindows()
{
	QList<MyWidget *> lst = current_folder->windowsList();
	foreach(MyWidget *w, lst)
	{//force show all windows in current folder
		if (w)
		{
			updateWindowLists(w);
			switch (w->status())
			{
				case MyWidget::Hidden:
					w->showNormal();
					break;

				case MyWidget::Normal:
					w->showNormal();
					break;

				case MyWidget::Minimized:
					w->showMinimized();
					break;

				case MyWidget::Maximized:
					w->showMaximized();
					break;
			}
		}
	}

	if ( (current_folder->children()).isEmpty() )
		return;

	FolderListItem *fi = current_folder->folderListItem();
	FolderListItem *item = (FolderListItem *)fi->firstChild();
	int initial_depth = item->depth();
	while (item && item->depth() >= initial_depth)
	{// show/hide windows in all subfolders
		lst = ((Folder *)item->folder())->windowsList();
		foreach(MyWidget *w, lst)
		{
			if (w && show_windows_policy == SubFolders)
			{
				updateWindowLists(w);
				switch (w->status())
				{
					case MyWidget::Hidden:
						w->showNormal();
						break;

					case MyWidget::Normal:
						w->showNormal();
						break;

					case MyWidget::Minimized:
						w->showMinimized();
						break;

					case MyWidget::Maximized:
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
	QList<MyWidget *> lst = current_folder->windowsList();
	foreach(MyWidget *w, lst)
		hideWindow(w);

	if ( (current_folder->children()).isEmpty() )
		return;

	if (show_windows_policy == SubFolders)
	{
		FolderListItem *fi = current_folder->folderListItem();
		FolderListItem *item = (FolderListItem *)fi->firstChild();
		int initial_depth = item->depth();
		while (item && item->depth() >= initial_depth)
		{
			lst = item->folder()->windowsList();
			foreach(MyWidget *w, lst)
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

	QList<QWidget*> *lst = windowsList();
	s += tr("Contents") + ": " + QString::number(lst->count()) + " " + tr("windows");
	delete lst;

	s += ", " + QString::number(current_folder->subfolders().count()) + " " + tr("folders") + "\n\n";
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
	s += tr("Contents") + ": " + QString::number(current_folder->windowsList().count()) + " " + tr("windows");
	s += ", " + QString::number(current_folder->subfolders().count()) + " " + tr("folders") + "\n\n";
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
		QList<MyWidget *> lst = f->windowsList();
		foreach(MyWidget *w, lst)
			removeWindowFromLists(w);

		if ( !(f->children()).isEmpty() )
		{
			FolderListItem *item = (FolderListItem *)fi->firstChild();
			int initial_depth = item->depth();
			while (item && item->depth() >= initial_depth)
			{
				lst = ((Folder *)item->folder())->windowsList();
				foreach(MyWidget *w, lst)
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

void ApplicationWindow::folderItemDoubleClicked(Q3ListViewItem *it)
{
	if (!it || it->rtti() != FolderListItem::RTTI)
		return;

	FolderListItem *item = ((FolderListItem *)it)->folder()->folderListItem();
	folders->setCurrentItem(item);
}

void ApplicationWindow::folderItemChanged(Q3ListViewItem *it)
{
	if (!it)
		return;

	it->setOpen(true);
	changeFolder (((FolderListItem *)it)->folder());
	folders->setFocus();
}

void ApplicationWindow::hideFolderWindows(Folder *f)
{
	QList<MyWidget *> lst = f->windowsList();
	foreach(MyWidget *w, lst)
	{
		if (w && !w->isHidden())
			w->hide();
	}

	if ( (f->children()).isEmpty() )
		return;

	FolderListItem *fi = f->folderListItem();
	FolderListItem *item = (FolderListItem *)fi->firstChild();
	int initial_depth = item->depth();
	while (item && item->depth() >= initial_depth)
	{
		lst = item->folder()->windowsList();
		foreach(MyWidget *w, lst)
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

	QObjectList folderLst = newFolder->children();
	if(!folderLst.isEmpty())
	{
		foreach(QObject *f, folderLst)
			addFolderListViewItem(static_cast<Folder *>(f));
	}

	QList<MyWidget *> lst = newFolder->windowsList();
	foreach(MyWidget *w, lst)
	{//show only windows in the current folder which are not hidden by the user
		if (w)
		{
			if (!hiddenWindows->contains(w) && !outWindows->contains(w) &&
					show_windows_policy != HideAll)
			{
				switch (w->status())
				{
					case MyWidget::Normal:
						w->showNormal();
						break;
					case MyWidget::Minimized:
						w->showMinimized();
						break;
					case MyWidget::Maximized:
						{
							if (w->isA("Graph3D"))
								((Graph3D *)w)->setIgnoreFonts(true);

							w->showMaximized();

							if (w->isA("Graph3D"))
								((Graph3D *)w)->setIgnoreFonts(false);
						}
						break;
					case MyWidget::Hidden: // this case is only here to suppress compiler warnings
						break;
				}
			}
			else
				w->setStatus(MyWidget::Hidden);

			addListViewItem(w);
		}
	}

	if ( (newFolder->children()).isEmpty() )
		return;

	FolderListItem *fi = newFolder->folderListItem();
	FolderListItem *item = (FolderListItem *)fi->firstChild();
	int initial_depth = item->depth();
	while (item && item->depth() >= initial_depth)
	{//show/hide windows in subfolders
		lst = ((Folder *)item->folder())->windowsList();
		foreach(MyWidget *w, lst)
		{
			if (w &&!hiddenWindows->contains(w) && !outWindows->contains(w))
			{
				if (show_windows_policy == SubFolders)
				{
					switch (w->status())
					{
						case MyWidget::Normal:
							w->showNormal();
							break;
						case MyWidget::Minimized:
							w->showMinimized();
							break;
						case MyWidget::Maximized:
							if (w->isA("Graph3D"))
								((Graph3D*)w)->setIgnoreFonts(true);

							w->showMaximized();

							if (w->isA("Graph3D"))
								((Graph3D*)w)->setIgnoreFonts(false);
							break;
						case MyWidget::Hidden: // this case is only here to suppress compiler warnings
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

void ApplicationWindow::addListViewItem(MyWidget *w)
{
	if (!w)
		return;

	WindowListItem* it = new WindowListItem(lv, w);

	if (w->isA("Matrix"))
	{
		it->setPixmap(0, QPixmap(matrix_xpm));
		it->setText(1, tr("Matrix"));
	}
	else if (w->inherits("Table"))
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
		it->setText(1, tr("Graph"));
	}
	else if (w->isA("Graph3D"))
	{
		it->setPixmap(0, QPixmap(trajectory_xpm));
		it->setText(1, tr("3D Graph"));
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
	MyWidget *w = it->window();
	if (!w)
		return;

	QMessageBox *mbox = new QMessageBox ( tr("Properties"), QString(), QMessageBox::NoIcon, 
			QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, this);

	QString s = QString(w->name()) + "\n\n";
	s += "\n\n\n";

	s += tr("Label") + ": " + ((MyWidget *)w)->windowLabel() + "\n\n";

	if (w->isA("Matrix"))
	{
		mbox->setIconPixmap(QPixmap(matrix_xpm));
		s +=  tr("Type") + ": " + tr("Matrix") + "\n\n";
	}
	else if (w->inherits("Table"))
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
		s +=  tr("Type") + ": " + tr("Graph") + "\n\n";
	}
	else if (w->isA("Graph3D"))
	{
		mbox->setIconPixmap(QPixmap(trajectory_xpm));
		s +=  tr("Type") + ": " + tr("3D Graph") + "\n\n";
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
		MyWidget *w = current_folder->findWindow(s,windowNames,labels,caseSensitive,partialMatch);
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
				MyWidget *w = f->findWindow(s,windowNames,labels,caseSensitive,partialMatch);
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

void ApplicationWindow::dropFolderItems(Q3ListViewItem *dest)
{
	if (!dest || draggedItems.isEmpty ())
		return;

	Folder *dest_f = ((FolderListItem *)dest)->folder();

	Q3ListViewItem *it;
	QStringList subfolders = dest_f->subfolders();

	foreach(it, draggedItems)
	{
		if (it->rtti() == FolderListItem::RTTI)
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
				QMessageBox::critical(this, tr("QtiPlot") +" - " + tr("Skipped moving folder"), 
						tr("The destination folder already contains a folder called '%1'! Folder skipped!").arg(f->folderName()));
			}
			else
				moveFolder(src, (FolderListItem *)dest);
		}
		else
		{
			if (dest_f == current_folder)
				return;

			MyWidget *w = ((WindowListItem *)it)->window();
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

	QList<MyWidget *> lst = QList<MyWidget *>(src_f->windowsList());
	foreach(MyWidget *w, lst)
	{
		src_f->removeWindow(w);
		w->hide();
		dest_f->addWindow(w);
	}	

	if ( !(src_f->children()).isEmpty() )
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

			lst = QList<MyWidget *>(src_f->windowsList());
			foreach(MyWidget *w, lst)
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

void ApplicationWindow::searchForUpdates()
{
	versionFile.setName("qtiplot_last_version.txt");
	if (!versionFile.open(IO_WriteOnly))
	{
		QMessageBox::warning(this, tr("QtiPlot - HTTP Get Version File"),
				tr("Cannot write file %1\n%2.").arg(versionFile.name()).arg(versionFile.errorString()));
		return;
	}
	http.setHost("soft.proindependent.com");
	http.get("/version.txt", &versionFile);
	http.closeConnection();
}

void ApplicationWindow::receivedVersionFile(bool error)
{
	if (error)
	{
		QMessageBox::warning(this, tr("QtiPlot - HTTP get version file"),
				tr("Error while fetching version file with HTTP: %1.").arg(http.errorString()));
		return;
	}

	versionFile.close();

	if (versionFile.open(IO_ReadOnly))
	{
		QTextStream t( &versionFile );
		t.setEncoding(QTextStream::UnicodeUTF8);
		QString version = t.readLine();

		versionFile.close();
		versionFile.remove();

		QString currentVersion = QString::number(majVersion) + "." + QString::number(minVersion) +
			"." + QString::number(patchVersion);

		if (currentVersion != version)
		{
			if(QMessageBox::question(this, tr("QtiPlot - Updates Available"),
						tr("There is a newer version of QtiPlot (%1) available for download. Would you like to download it?").arg(version),
						QMessageBox::Yes|QMessageBox::Default, QMessageBox::No|QMessageBox::Escape) == QMessageBox::Yes)
				QDesktopServices::openUrl(QUrl("http://soft.proindependent.com/download.html"));
		}
		else if (!autoSearchUpdatesRequest)
		{
			QMessageBox::information(this, tr("QtiPlot - No Updates Available"),
					tr("No updates available. Your current version %1 is the last version available!").arg(version));
		}
		autoSearchUpdatesRequest = false;
	}
}

/*!
Turns 3D animation on or off
*/
void ApplicationWindow::toggle3DAnimation(bool on)
{
if (ws->activeWindow() && ws->activeWindow()->isA("Graph3D"))
   ((Graph3D*)ws->activeWindow())->animate(on);
}
 	
QString ApplicationWindow::generateUnusedName(const QString& name, bool increment)
{
	int index = 0;
	QWidgetList *windows = windowsList();
	for (int i = 0; i < windows->count();i++ )
	{
		if (QString(windows->at(i)->name()).startsWith(name))
			index++;
	}
	delete windows;

	QString newName = name;
	if (increment)//force return of a different name
		newName += QString::number(++index);
	else
	{
		if (index>0)
			newName += QString::number(index);
	}
	return newName;
}

ApplicationWindow::~ApplicationWindow()
{
	if (lastCopiedLayer)
		delete lastCopiedLayer;

	delete hiddenWindows;
	delete outWindows;

	QApplication::clipboard()->clear(QClipboard::Clipboard);
}

/*****************************************************************************
 *
 * Class HelpBrowser
 *
 *****************************************************************************/

HelpBrowser::HelpBrowser(QWidget * parent, const char * name)
		:QTextBrowser (parent, name)
{
setFrameStyle(QFrame::Panel | QFrame::Sunken);
setOpenExternalLinks(true);
// TODO: be able to set a .css file
//document()->setDefaultStyleSheet("qtiplot.css"); //doesn't work
}

void HelpBrowser::open()
{
QString fn = QFileDialog::getOpenFileName(QDir::currentDirPath(), "*.html", this);
if (!fn.isEmpty() && QFile(fn).exists())
   setSource(QUrl::fromLocalFile(fn));
}

void HelpBrowser::print()
{
	QTextDocument *doc = document();
	QPrinter printer(QPrinter::HighResolution);
	QPrintDialog printDialog(&printer);
	// TODO: Write a dialog to use more features of Qt4's QPrinter class
	if (printDialog.exec() == QDialog::Accepted) 
	{
		doc->print(&printer);
	}
}

void HelpBrowser::exportPdf()
{
#ifndef QT_NO_PRINTER
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF", QString(), "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        document()->print(&printer);
    }
#endif
}
