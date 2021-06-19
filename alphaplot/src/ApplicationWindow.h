/* This file is part of AlphaPlot.
   Copyright 2016 - 2020, Arun Narayanankutty <n.arun.lifescience@gmail.com>
   Copyright 2006 - 2007, Ion Vasilief <ion_vasilief@yahoo.fr>
   Copyright 2006 - 2009, Knut Franke <knut.franke@gmx.de>
   Copyright 2006 - 2009, Tilman Benkert <thzs@gmx.net>

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : Main part of UI & project management related stuff */

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#ifdef SEARCH_FOR_UPDATES
#include <QNetworkAccessManager>
#include <QNetworkReply>
#endif
#include <QBuffer>
#include <QDesktopServices>
#include <QFile>
#include <QLocale>

#include "Table.h"

// Scripting
#include "../3rdparty/qcustomplot/qcustomplot.h"
#include "2Dplot/Graph2DCommon.h"
#include "3Dplot/Graph3DCommon.h"
#include "scripting/Script.h"
#include "scripting/ScriptingEnv.h"

class QPixmap;
class QCloseEvent;
class QDropEvent;
class QTimerEvent;
class QDragEnterEvent;
class QTranslator;
class QDockWidget;
class QAction;
class QActionGroup;
class QLineEdit;
class QTranslator;
class QToolButton;
class QShortcut;
class QMenu;
class QToolBar;
class QStatusBar;
class QSignalMapper;
class QTreeWidgetItem;
class QMdiArea;

class Ui_ApplicationWindow;
class Matrix;
class Table;
class ScalePicker;
class Note;
class Layout2D;
class Layout3D;
class Folder;
class FolderTreeWidget;
class FolderTreeWidgetItem;
class Plot3DDialog;
class MyWidget;
class TableStatistics;
class CurveRangeDialog;
class Project;
class AbstractAspect;

class ConsoleWidget;
class IconLoader;
class AprojHandler;
class SettingsDialog;
class PropertiesDialog;
class PropertyEditor;
class AxisRect2D;
class Curve2D;
class Function2DDialog;

#ifndef TS_PATH
#define TS_PATH (qApp->applicationDirPath() + "/translations")
#endif

/* brief AlphaPlot's main window.
 *
 * This class contains the main part of the user interface as well as the
 * central project management facilities.It manages all MyWidget MDI Windows
 * in a project, knows about their organization in Folder objects and contains
 * the parts of the project explorer not implemented in Folder,
 * FolderListItem or FolderListView.
 *
 * Furthermore, it is responsible for displaying most MDI Windows' context
 * menus and opening all sorts of dialogs.
 *
 * section future_plans Future Plans
 * Split out the project management part into a new Project class. If MyWidget
 * maintains a reference to its parent Project, it should be possible to have
 * its subclasses display their own context menus and dialogs.This is
 * necessary for implementing new plot types or even completely new
 * MyWidget subclasses in plug-ins. It will also make ApplicationWindow more
 * manageable by removing those parts not directly related to the main window.
 *
 * Project would also take care of basic project file reading/writing (using
 * Qt's XML framework), but delegate most of the work to MyWidget and its
 * subclasses. This is necessary for providing save/restore of classes
 * implemented in plug-ins. Support for foreign formats on the other hand
 * could go into import/export classes (which could also be implemented in
 * plug-ins). Those would interface directly with Project and the MyWidgets it
 * manages. Thus, in addition to supporting QtXML-based save/restore, Project,
 * MyWidget and subclasses will also have to provide generalized save/restore
 * methods/constructors.
 *
 * Maybe split out the project explorer into a new ProjectExplorer class,
 * depending on how much code is left in ApplicationWindow after the above
 * reorganizations. Think about whether a Model/View approach can be used for
 * Project/ProjectExplorer.*/

class ApplicationWindow : public QMainWindow,
                          public scripted,
                          public QScriptable {
  Q_OBJECT
 public:
  ApplicationWindow();
  ApplicationWindow(const QStringList& l);
  ~ApplicationWindow();

 private:
  Ui_ApplicationWindow* ui_;
  enum FindItem { FindWindowItem, FindFolderItem };

 public:
  // Folder windows handling policy
  enum ShowWindowsPolicy { HideAll, ActiveFolder, SubFolders };
  enum SubWindowType {
    TableSubWindow,
    MatrixSubWindow,
    NoteSubWindow,
    Plot2DSubWindow,
    Plot3DSubWindow,
  };

  enum class Graph {
    Line = 0,
    Scatter = 1,
    LineSymbols = 2,
    VerticalBars = 3,
    Area = 4,
    Pie = 5,
    VerticalDropLines = 6,
    Spline = 7,
    HorizontalSteps = 8,
    Histogram = 9,
    HorizontalBars = 10,
    VectXYXY = 11,
    ErrorBars = 12,
    Box = 13,
    VectXYAM = 14,
    VerticalSteps = 15,
    ColorMap = 16,
    GrayMap = 17,
    ContourMap = 18,
    Function = 19,
    Channel = 20,
    VerticalStackedBars = 21,
    HorizontalStackedBars = 22,
    VerticalGroupedBars = 23,
    HorizontalGroupedBars = 24,
    ScatterYError = 25,
    ScatterXError = 26,
    ScatterXYError = 27
  };

  QTranslator* appTranslator;
  QTranslator* qtTranslator;
#ifdef SCRIPTING_CONSOLE
  ConsoleWidget* consoleWindow;
#endif
  PropertyEditor* propertyeditor;
  QMdiArea* d_workspace;
  QToolButton* btnResults;
  QWidgetList* hiddenWindows;
  QWidgetList* outWindows;
  MyWidget* lastModified;

  // Toolbars
  QToolBar* fileToolbar;
  QToolBar* editToolbar;
  QToolBar* graphToolsToolbar;
  QToolBar* plot2DToolbar;
  QToolBar* tableToolbar;
  QToolBar* matrix3DPlotToolbar;
  QToolBar* graph3DToolbar;

  MyWidget* getactiveMyWidget();
  /* Generates a new unique name starting with string /param name.
  You can force the output to be a name different from /param name,
  even if 'name' is not used in the project, by setting /param increment = true
  (the default)*/
  QString generateUniqueName(const QString& name, bool increment = true);
  void blockFolderviewsignals(bool value);
  FolderTreeWidgetItem* getProjectRootItem();
  QString getLogInfoText() const;
  void setCurrentFolderViewItem(FolderTreeWidgetItem* item);

 public slots:
  //! Copy the status bar text to the clipboard
  void copyStatusBarText();
  //! Show the context menu for the status bar
  void showStatusBarContextMenu(const QPoint& pos);
  //! \name Projects and Project Files
  //@{
  void openAproj();
  ApplicationWindow* openAproj(const QString& fileName);
  //! Returns temporary file ready for reading uncompressed content
  /**
   * Close and delete after you're done with it.
   */
  void showUndoRedoHistory();

  /* brief Create a new project from a data file.
   *
   * \param fn is read as a data file with the default column separator (as set
   * by the user) and inserted as a table into a new, empty project.
   * This table is then plotted with the Graph::LineSymbols style.*/
  ApplicationWindow* plotFile(const QString& fn);

  /* brief Create a new project from a script file.
   *
   * param fn is read as Alpha script file and loaded in the scripting console
   * param execute specifies if the script should be executed after opening.*/
  ApplicationWindow* loadScript(const QString& fn, bool execute = false);

  QList<QMdiSubWindow*> subWindowsList();
  QList<QMdiSubWindow*> subWindowsListFromTreeRecursive(
      QList<QMdiSubWindow*> list, FolderTreeWidgetItem* item);
  void updateWindowLists(MyWidget* w);

  void saveProjectAs();
  bool saveProject();

  // Set the project status to modifed
  void modifiedProject();
  // Set the project status to saved (not modified)
  void savedProject();
  // Set project status to modified and save 'w' as the last modified widget
  void modifiedProject(MyWidget* widget);
  //@}

  //! \name Settings
  //@{
  void loadSettings();
  void saveSettings();
  void applyUserSettings();
  void setSaveSettings(bool autoSaving, int min);
  void changeAppStyle(const QString& s);
  void changeAppColorScheme(int colorScheme);
  void changeAppFont(const QFont& font);
  void setAppColors();
  //@}

  void addLayout(const Graph2DCommon::AddLayoutElement& position);
  void deleteLayout();

  //! Creates a new spectrogram graph
  Layout2D* plotGrayScale();
  Layout2D* plotContour();
  Layout2D* plotColorMap();

  //! Rearrange the layersin order to fit to the size of the plot window
  void addNestedLayout();
  void plotStackedHistograms();
  //@}

  //! \name 3D Data Plots
  //@{
  Layout3D* dataPlot3D(Table* table, const Graph3DCommon::Plot3DType& type);
  Layout3D* dataPlotXYZ(Table* table, const Graph3DCommon::Plot3DType& type);
  //@}

  //! \name Surface Plots
  //@{
  Layout3D* newPlot3D(const QString& formula, const double xl, const double xr,
                      const double yl, const double yr, const double zl,
                      const double zr);
  Layout3D* newPlot3D(const QString& caption, const QString& formula,
                      const double xl, const double xr, const double yl,
                      const double yr, const double zl, const double zr);
  void newSurfacePlot();
  void editSurfacePlot();
  void add3DData();
  void change3DData();
  void change3DData(const QString& colName);
  void change3DMatrix();
  void change3DMatrix(const QString& matrix_name);
  void add3DMatrixPlot();

  Layout3D* plot3DMatrix(const Graph3DCommon::Plot3DType& plottype);

  void plot3DRibbon();
  void plot3DScatter();
  void plot3DTrajectory();
  void plot3DBars();
  //@}

  //! \name User-defined Functions
  //@{
  bool newFunctionPlot(const int type, const QStringList& formulas,
                       const QString& var, const QList<double>& ranges,
                       const int points);
  Curve2D* addFunctionPlot(const int type, const QStringList& formulas,
                           const QString& var, const QList<double>& ranges,
                           const int points, AxisRect2D* axisrect);
  QPair<QVector<double>*, QVector<double>*> generateFunctiondata(
      const int type, const QStringList& formulas, const QString& var,
      const QList<double>& ranges, const int points);
  QList<QPair<QPair<double, double>, double>>* generateFunction3ddata(
      const Graph3DCommon::Function3DData& funcdata);

  Function2DDialog* functionDialog();
  void addFunctionCurve();
  void addGraph2DAxis();
  void clearFitFunctionsList();
  void saveFitFunctionsList(const QStringList& l);
  void clearSurfaceFunctionsList();
  void clearLogInfo();
  void clearParamFunctionsList();
  void clearPolarFunctionsList();
  void updateFunctionLists(int type, QStringList& formulas);
  void updateSurfaceFuncList(const QString& s);
  //@}

  //! \name Matrices
  //@{
  //! Creates a new empty matrix
  Matrix* newMatrix(int rows = 32, int columns = 32);
  //! To be used when opening a project file only!
  Matrix* newMatrix(const QString& caption, int r, int c);
  Matrix* matrix(const QString& name);
  Matrix* convertTableToMatrix();
  void initMatrix(Matrix* matrix);
  void invertMatrix();
  void matrixDeterminant();
  //@}

  //! \name Tables
  //@{
  //! Creates an empty table
  Table* newTable();
  //! Used when importing an ASCII file
  Table* newTable(const QString& fname, const QString& sep, int lines,
                  bool renameCols, bool stripSpaces, bool simplifySpaces,
                  bool convertToNumeric, QLocale numericLocale);
  //! Used when loading a table from a project file
  Table* newTable(const QString& caption, int r, int c);
  Table* newTable(int r, int c, const QString& name = QString(),
                  const QString& legend = QString());
  Table* newTable(const QString& name, const QString& legend,
                  QList<Column*> columns);
  /**
   * \brief Create a Table which is initially hidden; used to return the result
   * of an analysis operation.
   *
   * \param name window name (compare MyWidget::MyWidget)
   * \param label window label (compare MyWidget::MyWidget)
   * \param r number of rows
   * \param c number of columns
   *\param text tab/newline - seperated initial content; may be empty
   */
  Table* newHiddenTable(const QString& name, const QString& label,
                        QList<Column*> columns);
  Table* table(const QString& name);
  Table* convertMatrixToTable();
  QList<QMdiSubWindow*>* tableList();

  void connectTable(Table* table);
  void initTable(Table* table);
  void customTable(Table* table, const Table::Custom &custom);
  void customizeTables(const Table::Custom &custom);
  void customizeCommentsTables();

  void importASCII();
  void importASCII(const QStringList& files, int import_mode,
                   const QString& local_column_separator,
                   int local_ignored_lines, bool local_rename_columns,
                   bool local_strip_spaces, bool local_simplify_spaces,
                   bool local_convert_to_numeric, QLocale local_numeric_locale);
  void exportAllTables(const QString& sep, bool colNames, bool expSelection);
  void exportASCII(const QString& tableName, const QString& sep, bool colNames,
                   bool expSelection);

  TableStatistics* newTableStatistics(Table* base, int type, QList<int>,
                                      const QString& caption = QString());
  //@}

  //! \name Graphs
  //@{
  void plotPie(const Graph2DCommon::PieStyle& style);
  void plotVectXYXY();
  void plotVectXYAM();
  //@}

  //! \name Image Analysis
  //@{
  void intensityTable();
  void pixelLineProfile();
  void loadImage();
  void loadImage(const QString& fn);
  Matrix* importImage();
  Matrix* importImage(const QString& fn);
  //@}

  //! \name Export and Print
  //@{
  void exportLayer();
  void exportGraph();
  void exportAllGraphs();
  void exportPDF();
  void print();
  void print(QMdiSubWindow* subwindow);
  void printAllPlots();
  //@}

  QStringList columnsList(AlphaPlot::PlotDesignation plotType);
  QList<QPair<Table*, Column*>> columnList(AlphaPlot::PlotDesignation plotType);
  QList<QPair<Table*, Column*>> columnList(Folder* folder,
                                           AlphaPlot::PlotDesignation plotType);
  QList<QPair<Table*, QPair<Column*, Column*>>> columnList();
  QList<QPair<Table*, QPair<Column*, Column*>>> columnList(Folder* folder);
  QStringList columnsList();

  void undo();
  void redo();

  //! \name MDI Windows
  //@{
  MyWidget* clone();
  MyWidget* clone(MyWidget*widget);

  //!  Called when the user presses F2 and an item is selected in lv.
  void renameWindow(QTreeWidgetItem* item, int, const QString& text);

  //!  Checks weather the new window name is valid and modifies the name.
  bool renameWindow(MyWidget* w, const QString& text);

  void maximizeWindow(QTreeWidgetItem* lbi);
  void maximizeWindow();
  void minimizeWindow();
  //! Changes the geometry of the active MDI window
  void setWindowGeometry(int x, int y, int w, int h);

  void updateWindowStatus(MyWidget*);

  bool hidden(QWidget* window);
  void closeActiveWindow();
  void closeWindow(MyWidget* window);

  //!  Does all the cleaning work before actually deleting a window!
  void removeWindowFromLists(MyWidget* widgrt);

  void hideWindow(MyWidget* window);
  void hideActiveWindow();
  void activateWindow();
  void activateWindow(MyWidget*);
  void printWindow();
  //@}

  //! Return a version string ("AlphaPlot x.y.z")
  static QString versionString();
  void windowsMenuActivated(int id);
  void removeCurves(Table* table, const QString& name);
  QStringList dependingPlots(const QString& caption);
  QStringList depending3DPlots(Matrix* m);

  void saveAsTemplate();
  void openTemplate();

  QString windowGeometryInfo(MyWidget* w);
  void restoreWindowGeometry(ApplicationWindow* app, MyWidget* w,
                             const QString s);

  //! \name List View in Project Explorer
  //@{
  void setListViewView(const QString& caption, const QString& view);
  void renameListViewItem(const QString& oldName, const QString& newName);
  void setListViewDate(const QString& caption, const QString& date);
  QString listViewDate(const QString& caption);
  void setListViewLabel(const QString& caption, const QString& label);
  //@}
  void updateCurves(Table* t, const QString& name);

  void showTable(const QString& curve);

  void addColToTable();
  void cutSelection();
  void copySelection();
  void pasteSelection();
  void clearSelection();
  void copyActiveLayer();

  void newAproj();

  //! Creates a new empty 2d plot
  Layout2D* newGraph2D(const QString& caption = tr("Graph"));
  //! Creates a new empty 3d plot
  Layout3D* newGraph3D(const Graph3DCommon::Plot3DType& type,
                       const QString& caption = tr("Graph"));
  QList<QPair<QPair<double, double>, double>>* generateFunctiondata(
      const QString& formula, double xl, double xr, double yl, double yr,
      double zl, double zr);

  //! \name Reading from a Project File
  //@{
  void openRecentAproj();
  //@}

  //! \name Initialization
  //@{
  void makeToolBars();
  void disableActions();
  void customToolBars(QMdiSubWindow* subwindow);
  void customMenu(QMdiSubWindow* subwindow);
  void windowActivated(QMdiSubWindow* subwindow);
  //@}

  //! \name Table Tools
  //@{
  void correlate();
  void autoCorrelate();
  void convolute();
  void deconvolute();
  void clearTable();
  //@}

  //! \name Plot Tools
  //@{
  void drawEllipse();
  void addTimeStamp();
  void drawLine();
  void drawArrow();
  void addText();
  void disableAddText();
  void addImage();
  void setAutoScale();
  void showRangeSelectors();
  void showDataReader();
  void dragRange();
  void zoomRange();
  void showScreenReader();
  void pickPointerCursor();
  void pickGraphTool(QAction* action);

  void updateLog(const QString& result);
  //@}

  //! \name Fitting
  //@{
  void deleteFitTables();
  void fitLinear();
  void fitBoltzmannSigmoid();
  void fitGaussian();
  void fitLorentzian();
  void fitMultiPeak(int profile);
  void fitMultiPeakGaussian();
  void fitMultiPeakLorentzian();
  //@}

  //! \name Calculus
  //@{
  void differentiate();
  void analysis(const QString& whichFit);
  void analyzeCurve(AxisRect2D* axisrect, const QString& whichFit,
                    const QString& curveTitle);
  void showDataSetDialog(const QString& whichFit);
  //@}

  void addErrorBars();
  void movePoints();
  void removePoints();

  //! \name Event Handlers
  //@{
  void closeEvent(QCloseEvent* event);
  void timerEvent(QTimerEvent* event);
  void dragEnterEvent(QDragEnterEvent* event);
  void dropEvent(QDropEvent* event);
  void customEvent(QEvent* event);
  //@}

  //! \name Dialogs
  //@{
  void findWindowOrFolderFromProjectExplorer();
  void showResults(bool ok);
  void showResults(const QString& text, bool ok = false);
  void showExportASCIIDialog();
  void showCurvesDialog();
  void showCurveRangeDialog();
  CurveRangeDialog* showCurveRangeDialog(AxisRect2D* axisrect, int curve);
  void showPlotAssociations(int curve);

  void showWindowContextMenu();
  void itemContextMenuRequested(Layout2D *layout, AxisRect2D *axisrect);
  void showWindowTitleBarMenu();
  void showWindowPopupMenu(const QPoint& p);

  //! Connected to the context menu signal from lv; it's called when there are
  //! several items selected in the list
  void showListViewSelectionMenu(const QPoint& p);

  //! Connected to the context menu signal from lv; it's called when there are
  //! no items selected in the list
  void showListViewPopupMenu(const QPoint& p);

  void showMoreWindows();
  void showMarkerPopupMenu();
  void showPlotWizard();
  void fitPolynomial();
  void integrate();
  void interpolate();
  void fitExponentialGrowth();
  void fitFirstOrderExponentialDecay();
  void fitExponential(int type);
  void fitSecondOrderExponentialDecay();
  void fitThirdOrderExponentialDecay();
  void showRowStatistics();
  void showColumnStatistics();
  void showFitDialog();
  void showSwapLayoutDialog();
  void showAddGlyphs();
  void showPreferencesDialog();
  void savitzkySmooth();
  void fFTFilterSmooth();
  void movingWindowAverageSmooth();
  void showSmoothDialog(int m);
  void showFilterDialog(int filter);
  void lowPassFilter();
  void highPassFilter();
  void bandPassFilter();
  void bandBlockFilter();
  void showFFTDialog();
  //@}

  void horizontalTranslate();
  void verticalTranslate();

  void updateGeneralApplicationOptions();
  void updateGeneralConfirmOptions();
  void updateGeneralAppearanceOptions();
  void updateGeneralNumericFormatOptions();
  void updateTableBasicOptions();
  void updateTableColorOptions();
  void updateTableFontOptions();

  //! \name Plot3D Tools
  //@{
  void toggle3DAnimation(bool on = true);
  // Resets rotation of 3D plots to default values
  void setCameraPresetFront();
  void resetZoomfactor();
  void removeAxes3DPlot();

  void pickSelectionType(QAction* action);
  void custom3DActions(QMdiSubWindow* subwindow);

  void updateRecentProjectsList();

#ifdef SEARCH_FOR_UPDATES
  //!  connected to the done(bool) signal of the http object
  void receivedVersionFile(QNetworkReply* reply);
  //!  called when the user presses the actionCheckUpdates
  void searchForUpdates();
#endif

  //! Open AlphaPlot homepage in external browser
  void showHomePage();
  //! Open forums page at SF.net in external browser
  void showForums();
  //! Open bug tracking system at SF.net in external browser
  void showBugTracker();
#ifdef DOWNLOAD_LINKS
  //! Show download page in external browser
  void downloadManual();
#endif

  void parseCommandLineArguments(const QStringList& args);
  void createLanguagesList();
  void switchToLanguage(int param);
  void switchToLanguage(const QString& locale);

  bool alreadyUsedName(const QString& label);
  bool projectHas2DPlots();
  bool projectHas3DPlots();
  bool projectHasMatrices();

  //! Returns a pointer to the window named "name"
  QMdiSubWindow* window(const QString& name);

  //! Returns a list with the names of all the matrices in the project
  QStringList matrixNames();

  //! \name Notes
  //@{
  //! Creates a new empty note window
  Note* newNote(const QString& caption = QString());
  void initNote(Note* note, const QString& caption);
  void saveNoteAs();
  //@}

  //! \name Folders
  //@{
  //! Returns a to the current folder in the project
  Folder* currentFolder() { return current_folder; }
  //! Adds a new folder to the project
  void addFolder();
  //! Deletes the current folder
  void deleteFolder();

  //! Ask confirmation from user, deletes the folder f if user confirms and
  //! returns true, otherwise returns false;
  bool deleteFolder(Folder* f);

  //! Deletes the currently selected items from the list view #lv.
  void deleteSelectedItems();

  //! Sets all items in the folders list view to be deactivated
  void deactivateFolders();
  void deactivateFolderTreeWidgetItemsRecursive(FolderTreeWidgetItem* item);

  //! Changes the current folder
  bool changeFolder(Folder* newFolder, bool force = false);
  void refreshFolderTreeWidgetItemsRecursive(FolderTreeWidgetItem* item);

  //! Changes the current folder when the user changes the current item in the
  //! QListView "folders"
  void folderItemChanged(QTreeWidgetItem* item);
  //! Changes the current folder when the user double-clicks on a folder item in
  //! the QListView "lv"
  void folderItemDoubleClicked(QTreeWidgetItem* it);

  //!  creates and opens the context menu of a folder list view item
  /**
   * \param it list view item
   * \param p mouse global position
   * \param fromFolders: true means that the user clicked right mouse buttom on
   *an item from QListView "folders"
   *false means that the user clicked
   *right
   *mouse
   *buttom
   *on an item from QListView "lv"
   */
  void showFolderPopupMenu(QTreeWidgetItem* it, const QPoint& p,
                           bool fromFolders);

  //!  connected to the SIGNAL contextMenuRequested from the list views
  void showFolderPopupMenu(const QPoint& p);

  // prepare to rename by passing item
  void renameFolderFromMenu();
  void startRenameFolder(FolderTreeWidgetItem* fi);
  void renameFolder(QTreeWidgetItem* item);

  //!  forces showing all windows in the current folder and subfolders,
  //!  depending on the user's viewing policy
  void showAllFolderWindows();
  void showAllFolderWindowsRecursive(FolderTreeWidgetItem* item);

  //!  forces hidding all windows in the current folder and subfolders,
  //!  depending on the user's viewing policy
  void hideAllFolderWindows();

  //!  hides all windows in folder f
  void hideFolderWindows(Folder* f);

  //  pops up project, folder or window information
  void folderProperties();
  void windowProperties();

  //! Pops up a file dialog and invokes appendProject(const QString&) on the
  //! result.
  void appendProject();
  //! Open the specified project file and add it as a subfolder to the current
  //! folder.
  void saveAsProject();

  //!  adds a folder list item to the list view "lv"
  void addFolderListViewItem(Folder* folder);

  //!  adds a widget list item to the list view "lv"
  void addListViewItem(MyWidget* widget);

  //!  hides or shows windows in the current folder & windows policy
  void setShowWindowsPolicy(int policy);

  //!  returns a pointer to the root project folder
  Folder* projectFolder();

  //!  used by the findDialog
  void find(const QString& s, bool windowNames, bool labels, bool folderNames,
            bool caseSensitive, bool partialMatch, bool subfolders);
  bool findRecursive(FolderTreeWidgetItem* item, FindItem findItem, QString s,
                     bool labels, bool caseSensitive, bool partialMatch);

  //!  initializes the list of items dragged by the user
  void dragFolderItems(QList<QTreeWidgetItem*> items) { draggedItems = items; }

  //!  Drop the objects in the list draggedItems to the folder of the
  //!  destination item
  void dropFolderItems(QTreeWidgetItem* dest);

  //!  moves a folder item to another
  /**
   * \param src source folder item
   * \param dest destination folder item
   */
  void moveFolder(FolderTreeWidgetItem* src, FolderTreeWidgetItem* dest);
  //@}

  //! \name Scripting
  //@{
  //! notify the user that an error occured in the scripting system
  void scriptError(const QString& message, const QString& scriptName,
                   int lineNumber);
  //! execute all notes marked auto-exec
  void executeNotes();
  //! show scripting language selection dialog
  void showScriptingLangDialog();
  //! create a new environment for the current scripting language
  void restartScriptingEnv();
  //! print to scripting console (if available) or to stdout
  void scriptPrint(const QString& text);
  //! switches to the given scripting language; if this is the same as the
  //! current one and force is true, restart it
  bool setScriptingLang(const QString& lang, bool force = false);
  //@}
  void newCurve2D(Table* table, Column* xcol, Column* ycol);

 signals:
  void modified();

 private slots:

  void showHelp();          // Open help file index.html
  void chooseHelpFolder();  // Choose help folder
  void about();             // Show about dialog

  // TODO: a lot of this stuff should be private
 public:
  QStringList tableWindows();
  //! Last selected filter in export image dialog
  QString d_image_export_filter;
  bool d_keep_plot_aspect;
  int d_export_vector_size;
  bool d_export_transparency;
  int d_export_quality;
  int d_export_resolution;
  bool d_export_color;
  //! Default paper orientation for image exports.
  int d_export_orientation;
  //! Locale used to specify the decimal separators in imported ASCII files
  QLocale d_ASCII_import_locale;
  //! Last selected filter in import ASCII dialog
  QString d_ASCII_file_filter;
  bool d_convert_to_numeric;
  //! Specifies if only the Tables/Matrices in the current folder should be
  //! displayed in the Add/remove curve dialog.
  bool d_show_current_folder;
  bool d_scale_plots_on_print;
  bool d_print_cropmarks;
  bool d_show_table_comments;
  bool d_extended_plot_dialog;
  bool d_extended_import_ASCII_dialog;
  bool d_extended_export_dialog;
  bool d_extended_open_dialog;
  bool generateUniformFitPoints;
  bool generatePeakCurves;
  int peakCurvesColor;
  // User defined size for the Add/Remove curves dialog
  QSize d_add_curves_dialog_size;

  // Scale the errors output in fit operations with reduced chi^2
  bool fit_scale_errors;

  // Number of points in a generated fit curve
  int fitPoints;

  // Calculate only 2 points in a generated linear fit function curve
  bool d_2_linear_fit_points;

  bool pasteFitResultsToPlot;

  // Write fit output information to Result Log
  bool writeFitResultsToLog;

  // precision used for the output of the fit operations
  int fit_output_precision;

  // default precision to be used for all other operations than fitting
  int d_decimal_digits;

  char d_default_numeric_format;

  // pointer to the current folder in the project
  Folder* current_folder;
  // Describes which windows are shown when the folder becomes the current
  ShowWindowsPolicy show_windows_policy;
  enum { MaxRecentProjects = 10 };

  bool appCustomColor;
  QColor workspaceColor;
  QColor panelsColor;
  QColor panelsTextColor;
  QString appStyle;
  QString workingDir;
  int appColorScheme;

  // Path to the folder where the last template file was opened/saved
  QString templatesDir;
  bool smooth3DMesh;
  bool autoScaleFonts;
  bool autoResizeLayers;
  bool confirmCloseTable;
  bool confirmCloseMatrix;
  bool confirmClosePlot2D;
  bool confirmClosePlot3D;
  bool confirmCloseFolder;
  bool confirmCloseNotes;
  bool canvasFrameOn;
  bool titleOn;
  bool autoSave;
  bool drawBackbones;
  bool allAxesOn;
  bool autoscale2DPlots;
  bool antialiasing2DPlots;
  int majTicksStyle;
  int minTicksStyle;
  int legendFrameStyle;
  int autoSaveTime;
  int axesLineWidth;
  int canvasFrameWidth;
  QColor legendBackground;
  QColor legendTextColor;
  QColor defaultArrowColor;
  int defaultArrowLineWidth;
  int defaultArrowHeadLength;
  int defaultArrowHeadAngle;
  bool defaultArrowHeadFill;
  Qt::PenStyle defaultArrowLineStyle;
  int majTicksLength;
  int minTicksLength;
  int defaultPlotMargin;
  int defaultCurveStyle;
  int defaultCurveLineWidth;
  int defaultSymbolSize;
  int undoLimit;
  QFont appFont;
  QFont plot3DTitleFont;
  QFont plot3DNumbersFont;
  QFont plot3DAxesFont;
  QFont tableTextFont;
  QFont tableHeaderFont;
  QFont plotAxesFont;
  QFont plotLegendFont;
  QFont plotNumbersFont;
  QFont plotTitleFont;
  QColor tableBkgdColor;
  QColor tableTextColor;
  QColor tableHeaderColor;
  QString projectname;
  QString columnSeparator;
  QString appLanguage;
  QString configFilePath;
  QString logInfo;
  QString fitPluginsPath;
  QString asciiDirPath;
  QString imagesDirPath;
  int logID;
  int asciiID;
  int printAllID;
  int ignoredLines;
  int savingTimerId;
  int plot3DResolution;
  bool renameColumns;
  bool copiedLayer;
  bool strip_spaces;
  bool simplify_spaces;
  bool tableCustomColor;
  QStringList recentProjects;
  bool saved;
  bool showPlot3DProjection;
  bool showPlot3DLegend;
  bool orthogonal3DPlots;
  bool autoscale3DPlots;
  QStringList plot3DColors;
  QStringList locales;
  // user-defined functions;
  QStringList functions;
  // user functions for parametric and polar plots
  QStringList xFunctions;
  QStringList yFunctions;
  QStringList rFunctions;
  QStringList thetaFunctions;
  // user-defined fit functions;
  QStringList fitFunctions;
  // user-defined surface functions;
  QStringList surfaceFunc;

  // List of tables & matrices renamed in order to avoid conflicts when
  // appending a project to a folder
  QStringList renamedTables;

  // name variables used when user copy/paste markers
  QString auxMrkText;
  QFont auxMrkFont;
  QColor auxMrkColor;
  QColor auxMrkBkgColor;
  QPoint auxMrkStart;
  QPoint auxMrkEnd;
  Qt::PenStyle auxMrkStyle;
  QString auxMrkFileName;
  int auxMrkBkg;
  int auxMrkWidth;

  bool startArrowOn;
  bool endArrowOn;
  bool fillArrowHead;
  int arrowHeadLength;
  int arrowHeadAngle;
  int plotMenuID;
  int setAsMenuID;
  int fillMenuID;

  // The scripting language to use for new projects.
  QString defaultScriptingLang;
  /// location of translation resources
  QString qmPath;
#ifdef SEARCH_FOR_UPDATES
  bool autoSearchUpdates;
  // Return true if an auto search for updates was performed on start-up
  bool autoSearchUpdatesRequest;
#endif

 private:
  // Load icons
  void loadIcons();
  // Show a context menu for the widget
  void showWindowMenu(MyWidget* widget);

  //! Check if a table is valid for 3D plot & display an error if not
  bool validFor3DPlot(Table* table);
  //! Check if a table is valid for 2D plot & display an error if not
  bool validFor2DPlot(Table* table, Graph type);

  // Attach to the scripting environment
  void attachQtScript();

  bool isActiveSubwindow(const SubWindowType& subwindowtype);
  bool isActiveSubWindow(QMdiSubWindow* subwindow,
                         const SubWindowType& subwindowtype);

  // Stores the pointers to the dragged items from the FolderListViews objects
  QList<QTreeWidgetItem*> draggedItems;

  AprojHandler* aprojhandler_;
  QString helpFilePath;

#ifdef SEARCH_FOR_UPDATES
  //! Used when checking for new versions
  QNetworkAccessManager http;
  //! Used when checking for new versions
  QByteArray version_buffer;
#endif

  QAction* actionCopyStatusBarText;
  QAction* actionEditCurveRange;

  QAction* actionShowExportASCIIDialog;
  QAction* actionExportPDF;
  QAction* actionCloseAllWindows;
  QAction* actionConvertTable;
  QAction* actionAddColToTable;
  QAction* actionHideActiveWindow;
  QAction* actionShowMoreWindows;
  QAction* actionPixelLineProfile;
  QAction* actionIntensityTable;
  QAction* actionActivateWindow;
  QAction* actionMinimizeWindow;
  QAction* actionMaximizeWindow;
  QAction* actionPrintWindow;
  QAction* actionEditSurfacePlot;
  QAction* actionAdd3DData;
  QAction* actionMatrixDeterminant;
  QAction* actionConvertMatrix;
  QAction* actionInvertMatrix;

  QAction* actionClearTable;
  QAction* actionGoToCell;
  QAction* actionSaveNote;
  QAction* actionplot3dAnimate_;
  QAction* actionResetCameraFront_;
  QAction* actionResetZoomfactor_;

  QActionGroup* graphToolsGroup;

  QActionGroup* groupplot3dselectionmode_;
  QAction* actionplot3dmodecolumnselect_;
  QAction* actionplot3dmoderowselect_;
  QAction* actionplot3dmodeitemselect_;
  QAction* actionplot3dmodenoneselect_;

  // Manages connection between 2dplot actions (not used by all 2dplot actions).
  QSignalMapper* d_plot_mapper;

  QLabel* statusBarInfo;

  Project* d_project;
  // SettingsDialog* settings_;

  bool was_maximized_;

  QAction* actionShowPropertyEditor;
  QAction* actionShowProjectExplorer;
  QAction* actionShowResultsLog;
  QAction* actionShowConsole;

  QToolButton* btn_new_aspect_;
  QToolButton* btn_layout_;
  QToolButton* btn_curves_;
  QToolButton* btn_plot_enrichments_;
  QToolButton* btn_plot_scatters_;
  QToolButton* btn_plot_linespoints_;
  QToolButton* btn_plot_bars_;
  QToolButton* btn_plot_vect_;
  QToolButton* btn_plot_pie_;
  bool multiPeakfitactive_;
  int multiPeakfitpoints_;
  int multiPeakfittype_;
  QVector<QPair<Curve2D*, QPair<double, double>>> multipeakfitvalues_;
  bool glowstatus_;
  QColor glowcolor_;
  double glowxoffset_;
  double glowyoffset_;
  double glowradius_;

 private slots:
  void multipeakfitappendpoints(Curve2D* curve, double x, double y);
  void removeDependentTableStatistics(const AbstractAspect* aspect);
  // Set the active window selected from the context menu's dependency list
  void setActiveWindowFromAction();
  void selectPlotType(int value);

  void handleAspectAdded(const AbstractAspect* aspect, int index);
  void handleAspectAboutToBeRemoved(const AbstractAspect* aspect, int index);
  void lockToolbars(const bool status);

 public slots:
  Table* getTableHandle();
  Matrix* getMatrixHandle();
  Note* getNoteHandle();
};

#endif  // APPLICATION_H
