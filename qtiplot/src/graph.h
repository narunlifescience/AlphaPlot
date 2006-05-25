#ifndef GRAPH_H
#define GRAPH_H

#include <qprinter.h>
#include <qpainter.h>
#include <qpointarray.h>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <gsl/gsl_multifit_nlin.h>
#include <gsl/gsl_multimin.h>

#include "plotDialog.h"
#include "worksheet.h"
#include "axesDialog.h"

class QwtPlotZoomer;
class QwtPieCurve;	
class Table;
class LegendMarker;
class LineMarker;
class ImageMarker;
class TitlePicker;
class ScalePicker;
class CanvasPicker;
class Plot;

class Graph: public QWidget
{
	Q_OBJECT

public:
    Graph (QWidget* parent=0, const char* name=0, WFlags f=0);
	~Graph();

	enum AxisType{Numeric = 0, Txt = 1, Day = 2, Month = 3, Time = 4, Date = 5, ColHeader = 6};
	enum MarkerType{None=-1, Text = 0, Arrow=1, Image=2};
	enum LabelFormat{Automatic, Decimal, Scientific, Superscripts};
	enum CurveType{Line, Scatter, LineSymbols, VerticalBars , Area, Pie, VerticalDropLines, 
				  Spline, Steps, Histogram, HorizontalBars, VectXYXY, ErrorBars, Box, VectXYAM};

	Plot *d_plot;
	QwtPlotZoomer *d_zoomer;
	TitlePicker *titlePicker;
	ScalePicker *scalePicker;
	CanvasPicker* cp;
	 
public slots:
	Plot* plotWidget(){return d_plot;};
	void copy(Graph* g);

	// pie curves	
	bool isPiePlot(){return piePlot;};
	void plotPie(QwtPieCurve* curve);
	void plotPie(Table* w,const QString& name);
	void plotPie(Table* w,const QString& name,const QPen& pen, int brush, int size, int firstColor);
	void updatePie(const QPen& pen, const Qt::BrushStyle &brushStyle, int size, int firstColor);
	void updatePieCurveData(Table* w, const QString& yColName, int curve);
	void removePie();
	QString pieLegendText();
	QPen pieCurvePen();
	Qt::BrushStyle pieBrushStyle();
	int pieFirstColor();
	int pieSize();
	QString savePieCurveLayout();

	bool insertCurvesList(Table* w, const QStringList& names, int style, int lWidth, int sSize);
	bool insertCurve(Table* w, const QString& name, int style);
	bool insertCurve(Table* w, int xcol, const QString& name, int style);
	bool insertCurve(Table* w, const QString& xColName, const QString& yColName, int style);

	void removeCurve(int index);
	void removeCurve(const QString& s);
	
	void updateData(Table* w, int curve);
	void updateCurveData(Table* w, const QString& yColName, int curve);
	void updateBoxData(Table* w, const QString& yColName, int curve);
	 
	 int curves(){return n_curves;};
	 int curveDataSize(int curve);
	 bool validCurvesDataSize();
	 double selectedXStartValue();
	 double selectedXEndValue();

	 long curveKey(int curve);
	 int curveIndex(long key);
	 QwtPlotCurve *curve(int index);

	 QString curveXColName(const QString& curveTitle);

	 void insertPlottedList(const QStringList& names);
	 QStringList curvesList();
	 QStringList plotAssociations();
	 void setPlotAssociations(const QStringList& newList);
	 void changePlotAssociation(Table* t, int curve, const QString& text);

	 int curveType(int curveIndex);
	 void setCurveType(int curve, int style);
	
	 void print();
	 void copyImage();
	 void exportToEPS(const QString& fname);
	 void exportToEPS(const QString& fname, int res, QPrinter::Orientation o, 
					 QPrinter::PageSize size, QPrinter::ColorMode col);
	 static void addBoundingBox(const QString& fname, const QRect& rect);

	 void exportToWmf(const QString& fname);
	 
	 void clearPlot();
	 void replot();
	 void updatePlot();

	 QPixmap graphPixmap();
	 void exportImage(const QString& fileName,const QString& fileType, int quality, bool transparent);

	 // error bars
	 void addErrorBars(Table *w, const QString& xColName, const QString& yColName, 
					   Table *errTable, const QString& errColName,
					   int type, int width, int cap, const QColor& color,
					   bool through, bool minus,bool plus);
	
	 void addErrorBars(Table *w, const QString& yColName, 
						 Table *errTable, const QString& errColName,
						 int type, int width, int cap, const QColor& color,
						 bool through, bool minus,bool plus);

	 void updateErrorBarsData(Table* w, int curve);
	 void updateErrorBars(int curve,bool xErr,int width,int cap,
		          const QColor& c,bool plus,bool minus,bool through);
				  
	 // event handlers 
	 void contextMenuEvent(QContextMenuEvent *);
	 void closeEvent(QCloseEvent *e);

	 // plot scales
	 void setAxisScale(int axis,const QStringList& s);
	 void setScaleDiv(int axis,const QStringList& s);
	 QStringList plotLimits();
	 void setScales(const QStringList& s);

	 //curves layout
	 curveLayout initCurveLayout(int i, int curves, int style);
	 static curveLayout initCurveLayout();
	 void updateCurveLayout(int index,const curveLayout *cL);

	 gridOptions getGridOptions();
	 void setGridOptions(const gridOptions& options);

	// zoom
	 void zoomed (const QwtDoubleRect &rect);
	 void zoom(bool on);
	 bool zoomOn();
	 
	 void movedPicker(const QPoint &pos, bool mark);
	 void setAutoScale();
	 void updateScale();
	 void initScales();
	
	 QString saveAsTemplate();

	 // saving to file 
	 QString saveToString();
	 QString saveGridOptions();
	 QString saveScale();
	 QString saveScaleTitles();
	 QString saveFonts();
	 QString saveLegend();
	 QString saveMarkers();
	 QString saveCurveLayout(int index);
	 QString saveAxesTitleColors();
	 QString saveAxesColors();
	 QString saveEnabledAxes();
	 QString saveErrorBars();
	 QString saveCanvasFrame();
	 QString saveTitle();
	 QString saveAxesTitleAlignement();
	 QString saveEnabledTickLabels();
	 QString saveTicksType();
	 QString saveCurves();
	 QString saveLabelsFormat();
	 QString saveLabelsRotation();
	 QString saveAxesLabelsType();
	 QString saveAxesBaseline();
	 QString saveAxesFormulas();

	 // text markers 
	 void drawText(bool on);
	 bool drawTextActive();
	 
	 void insertTextMarker(LegendMarker* mrk);
	 long insertTextMarker(const QStringList& list);
	 long insertTextMarker_obsolete(const QStringList& list);
	 void updateTextMarker(const QString& text,int angle, int bkg,const QFont& fnt,
						   const QColor& textColor, const QColor& backgroundColor);
	
	 void removeMarker();
	 void cutMarker();
	 void copyMarker();
	 void pasteMarker();
	 void selectNextMarker();
	 void highlightLineMarker(long markerID);
	 void highlightTextMarker(long markerID);
	 void highlightImageMarker(long markerID);
	 void moveMarkerBy(int dx, int dy);
	 QFont defaultTextMarkerFont(){return defaultMarkerFont;};

	 int textMarkerDefaultFrame(){return defaultMarkerFrame;};
	 void setTextMarkerDefaultFrame(int f){defaultMarkerFrame = f;};
	 
	 MarkerType copiedMarkerType(){return selectedMarkerType;};
	 void setCopiedMarkerType(Graph::MarkerType type){selectedMarkerType=type;};
	 void setCopiedMarkerEnds(const QPoint& start, const QPoint& end);
 	 void setCopiedTextOptions(int bkg, const QString& text, const QFont& font, 
								const QColor& color, const QColor& bkgColor);
	 void setCopiedArrowOptions(int width, Qt::PenStyle style, const QColor& color,
													bool start, bool end, int headLength, int headAngle, bool filledHead);	
	 void setCopiedImageName(const QString& fn){auxMrkFileName=fn;};	
	 QRect copiedMarkerRect(){return QRect(auxMrkStart, auxMrkEnd);};
	 
	 //legendMarker
	 QwtArray<long> textMarkerKeys();
	 LegendMarker* textMarker(long id);

	 void addTimeStamp(const QFont& fnt, int frameStyle);
	 
	  // legend  
	 LegendMarker* legend();
	 void removeLegend();
	 void removeLegendItem(int index);
	 void addLegendItem(const QString& colName);
	 void insertLegend(const QStringList& lst);
	 void insertLegend_obsolete(const QStringList& lst);
	 void newLegend(const QFont& fnt, int frameStyle);
	 QSize newLegend(const QString& text);
	 bool legendOn();
	 
	 QString getLegendText();
	 void setLegendText(const QString& text);	 
	 QString legendText();//recreates the text from curves titles
	 
	 // line markers 
	 LineMarker* lineMarker(long id);
	 void insertLineMarker(LineMarker* mrk);
	 void insertLineMarker(QStringList list);
	 void updateLineMarker(const QColor& c,int w,Qt::PenStyle style,bool endArrow, bool startArrow);
	 QwtArray<long> lineMarkerKeys();
	 void drawLine(bool on);
	 bool drawLineActive();
	 void setArrowHeadGeometry(int length, int angle, bool filled);
	 void updateLineMarkerGeometry(const QPoint& sp,const QPoint& ep);

	 //image markers
	 ImageMarker* imageMarker(long id);
	 QwtArray<long> imageMarkerKeys();
	 void insertImageMarker(ImageMarker* mrk);
	 void insertImageMarker(const QPixmap& photo, const QString& fileName);
	 void insertImageMarker(const QStringList& options);
	 bool imageMarkerSelected();
	 void updateImageMarker(int x, int y, int width, int height);
	 
	 void resizeMarkers (double w_ratio, double h_ratio);
	 long selectedMarkerKey();
	 void setSelectedMarker(long mrk);
	  QwtPlotMarker* selectedMarkerPtr();
	 bool markerSelected();
	 bool arrowMarkerSelected();
	 void deselectMarker();

	 // axes 
	 QValueList<int> axesType();
	 void setAxesType(const QValueList<int> tl); 
	 	
	 QStringList scalesTitles();
	 void setXAxisTitle(const QString& text);
	 void setYAxisTitle(const QString& text);
	 void setRightAxisTitle(const QString& text);
	 void setTopAxisTitle(const QString& text);
	 void setAxisTitle(int axis, const QString& text);

	 QFont axisTitleFont(int axis);
	 void setXAxisTitleFont(const QFont &fnt);
	 void setYAxisTitleFont(const QFont &fnt);
	 void setRightAxisTitleFont(const QFont &fnt);
	 void setTopAxisTitleFont(const QFont &fnt);
	 void setAxisTitleFont(int axis,const QFont &fnt);

	 void setAxisFont(int axis,const QFont &fnt);
	 QFont axisFont(int axis);
	 void initFonts(const QFont &scaleTitleFnt,const QFont &numbersFnt,const QFont &textMarkerFnt);
	
	 QColor axisTitleColor(int axis);
	 void setXAxisTitleColor(const QColor& c);
	 void setYAxisTitleColor(const QColor& c);
	 void setRightAxisTitleColor(const QColor& c);
	 void setTopAxisTitleColor(const QColor& c);
	 void setAxesTitleColor(QStringList l);
	 
	 int axisTitleAlignment (int axis);
	 void setXAxisTitleAlignment(int align);
	 void setYAxisTitleAlignment(int align);
	 void setTopAxisTitleAlignment(int align);
	 void setRightAxisTitleAlignment(int align);
	 void setAxesTitlesAlignment(const QStringList& align);

	 QStringList axesColors();
	 void setAxesColors(const QStringList& colors);
	 void showAxis(int axis, int type, const QString& formatInfo, Table *table, bool axisOn, 
				   int ticksType, bool labelsOn, const QColor& c, int format, int prec, 
				   int rotation, int baselineDist, const QString& formula);

	 QMemArray<bool> enabledAxes();
	 void enableAxes(QMemArray<bool> axesOn);
	 void enableAxes(const QStringList& list);	

	int labelsRotation(int axis);
	void setAxisLabelRotation(int axis, int rotation);
	
	QStringList enabledTickLabels();
	void setEnabledTickLabels(const QStringList& list);
	
	int axesLinewidth();
	void setAxesLinewidth(int width);
	void loadAxesLinewidth(int width);//used when opening a project file

	void drawAxesBackbones(bool yes);
	bool axesBackbones(){return drawAxesBackbone;};
	void loadAxesOptions(const QString& s);//used when opening a project file

	QValueList<int> axesBaseline();
	void setAxesBaseline(const QValueList<int> &lst);
	void setAxesBaseline(QStringList &lst);

	QValueList<int> ticksType();
	void setTicksType(const QValueList<int>& list);
	void setTicksType(const QStringList& list); 
	
	int minorTickLength();
	int majorTickLength();
	void setAxisTicksLength(int axis, int ticksType, int minLength, int majLength);
	void setTicksLength(int minLength, int majLength);
	void changeTicksLength(int minLength, int majLength);

	QStringList labelsNumericFormat();
	void setLabelsNumericFormat(const QStringList& l);
	void setLabelsNumericFormat(int axis, const QStringList& l);	
	void setLabelsNumericFormat(int axis, int format, int prec, const QString& formula);
	void setLabelsDateTimeFormat(int axis, int type, const QString& formatInfo);
	
	QStringList axesLabelsFormatInfo(){return axesFormatInfo;};
	void setLabelsTextFormat(int axis, int type, const QString& labelsColName, Table *table);

	QStringList getAxesFormulas(){return axesFormulas;};
	void setAxesFormulas(const QStringList& l){axesFormulas = l;};

	 // canvas frame 
	 void drawCanvasFrame(bool frameOn, int width);
	 void drawCanvasFrame(const QStringList& frame);
	 void drawCanvasFrame(bool frameOn, int width, const QColor& color);
	 QColor canvasFrameColor();
	 int canvasFrameWidth();
	 bool framed();

	  // insertCurve title 
	 QString title();
	 void setTitle(const QString& t);
	 void setTitleFont(const QFont &fnt);
	 QFont titleFont();
	 QColor titleColor();
	 void setTitleColor(const QColor &c);
	 void removeTitle();
	 bool titleSelected();

	 void deselectTitle();
	 void selectTitle();
	 void setTitleSelected(bool on);
	 int titleAlignment();
	 void setTitleAlignment(int align);
	 void initTitle( bool on);
	 void initTitleFont( const QFont& fnt);
	
	 // tools for modifing insertCurve data 
	 bool selectPoint(const QPoint &pos);
	 void highlightPoint(bool showIt);
	 void selectCurve(const QPoint &pos);
	 int selectedCurveID(){return selectedCurve;};
	 QString selectedCurveTitle();
	 
	 void showCursor(bool showIt);
	 void shiftPointCursor(bool up);
	 void shiftCurveCursor(bool up);
	 void moveBy(int dx, int dy);
	 void move(const QPoint &pos);
	 void removePoint();
	 void movePoints(bool enabled);
	 bool movePointsActivated();
	 void removePoints(bool enabled);
	 bool removePointActivated();

	 void copyCanvas(bool on);
	 void enableCursor(bool on){cursorEnabled=on;};
	 bool enabledCursor(){return cursorEnabled;};
	 void showPlotPicker(bool on);
	 bool pickerActivated();

	 //translating curves
	 void translateCurve(int direction);
	 void translateCurveTo(const QPoint& p);
	 bool translationInProgress(){return translateOn;};
	 void startCurveTranslation();

	// data range selectors
   	 bool selectorsEnabled(){return rangeSelectorsEnabled;};
	 bool enableRangeSelectors(bool on);
	 void disableRangeSelectors();
	 void moveRangeSelector(bool up);
	 void moveRangeSelector();
	 void shiftRangeSelector(bool shift);
	 void shiftCurveSelector(bool up);
	 int selectedPoints(long curveKey);

	 //border and margin
	 void changeMargin (int d);
	 void drawBorder (int width, const QColor& color);
	 void setBorder (int width, const QColor& color);
	 void setBackgroundColor(const QColor& color);

	 //functions in analysis.cpp file
	 void smoothSavGol(long curveKey, int order, int nl, int nr, int colIndex);
	 void smoothFFT(long curveKey, int points, int colIndex);
	 void smoothAverage(long curveKey, int points, int colIndex);

	 void interpolate(QwtPlotCurve *curve, int spline, int start, int end, int points, int colorIndex);
	 QString integrateCurve(QwtPlotCurve *c,int order,int iter,double tol,double low,double up);
	 bool diffCurve(const QString& curveTitle);
	 void fft(long curveKey, bool forward, double sampling, bool normalizeAmp, bool order);
	 void filterFFT(long curveKey, int filter_type, double lf, double hf, 
					bool DCOffset, int colIndex);
	
	 QwtPlotCurve* getValidCurve(const QString& name, int params, int &points, int &start, int &end);
	 QwtPlotCurve* getFitLimits(const QString& name, double from, double to,
								int params, int &start, int &end);

	 QString fitLinear(const QString& curveTitle);

	 QString fitExpDecay(const QString& name, double damping, double amplitude, double yOffset, int colorIndex);
	 QString fitExpDecay(const QString& name, double damping, double amplitude, double yOffset,
						   double from, double to, int iterations, int solver, double tolerance, int colorIndex);
	 QString fitExpDecay(QwtPlotCurve *curve, double damping, double amplitude, double yOffset,
						   int start, int end, int iterations, int solver, double tolerance, int colorIndex);
	 
	 QString fitExpDecay2(const QString& name, double firstTime, double secondTime,
						 double from, double yOffset, int colorIndex);
	 QString fitExpDecay2(const QString& name, double amp1, double t1, double amp2, double t2, double yOffset,
						  double from, double to, int iterations, int solver, double tolerance, int colorIndex);
	 QString fitExpDecay2(QwtPlotCurve *curve, double amp1, double t1, double amp2, double t2, double yOffset,
						   int start, int end, int iterations, int solver, double tolerance, int colorIndex);

	 QString fitExpDecay3(const QString& name, double firstTime, double secondTime,
						double thirdTime, double from, double yOffset, int colorIndex);
	 QString fitExpDecay3(const QString& name, double amp1, double t1, double amp2, double t2, 
						  double amp3, double t3, double yOffset, double from, double to, 
						  int iterations, int solver, double tolerance, int colorIndex);
	 QString fitExpDecay3(QwtPlotCurve *curve, double amp1, double t1, double amp2, double t2, 
						  double amp3, double t3, double yOffset, int start, int end, 
						  int iterations, int solver, double tolerance, int colorIndex);

	 QString fitExpGrowth(const QString& name, double damping, double amplitude, double yOffset, int colorIndex);
	 QString fitExpGrowth(const QString& name, double damping, double amplitude, double yOffset,
						   double from, double to, int iterations, int solver, double tolerance, int colorIndex);
	 QString fitExpGrowth(QwtPlotCurve *curve, double damping, double amplitude, double yOffset,
						   int start, int end, int iterations, int solver, double tolerance, int colorIndex);

	 QString fitBoltzmann(const QString& curveTitle);
	 QString fitBoltzmann(const QString& name, double A1, double A2, double x0, double dx,
						  double from, double to, int iterations, int solver, double tolerance, int colorIndex);
	 QString fitBoltzmann(QwtPlotCurve *curve, double A1, double A2, double x0, double dx,
						  int start, int end, int iterations, int solver, double tolerance, int colorIndex);

	 QString fitGauss(const QString& curveTitle);
	 QString fitGauss(const QString& name, double amplitude, double center, double width, double offset,
						   double from, double to, int iterations, int solver, double tolerance, int colorIndex);
	 QString fitGauss(QwtPlotCurve *curve, double amplitude, double center, double width, double offset,
						   int start, int end, int iterations, int solver, double tolerance, int colorIndex);

	 QString fitLorentz(const QString& curveTitle);
	 QString fitLorentz(const QString& name, double amplitude, double center, double width, double offset,
						   double from, double to, int iterations, int solver, double tolerance, int colorIndex);
	 QString fitLorentz(QwtPlotCurve *curve, double amplitude, double center, double width, double offset,
						   int start, int end, int iterations, int solver, double tolerance, int colorIndex);

	 QString fitPolynomial(const QString&,int order, int points,
				double start, double end, bool showFormula, int colorIndex);
	 void setFitID(int id);

	 QString fitNonlinearCurve(const QString& curve,const QString& formula,
							const QStringList& params,const QStringList& paramsInit,
							double from,double to,int points, int solver, double tolerance, int colorIndex);
	
	QString fitPluginFunction(const QString& curve,const QString& pluginName,
							  const QStringList& paramsInit, double from,
							  double to,int points, int solver, double tolerance, int colorIndex);

	static gsl_multifit_fdfsolver* fitGSL(gsl_multifit_function_fdf f, int p, int n, 
										  double *x_init, int solver, double tolerance,
										  int &iterations, int &status);

	static gsl_multimin_fminimizer* fitSimplex(gsl_multimin_function f, double *x_init,
										  double tolerance,int &iterations, int &status);


	void addResultCurve(int n, double *x, double *y, int colorIndex,const QString& tableName, const QString& legend);
	
	QString outputFitString(int n, double tolerance, double from, double to, int iter,
							int solver, int status, double *params, gsl_multifit_fdfsolver *s, 
							const QStringList& parNames,const QString& curve,
							const QString& f, const QString& fitType);

   	QString outputFitString(int n, double tolerance, double from, double to, int iter,
							gsl_matrix *J, int status, double *params, gsl_multimin_fminimizer *s, 
							const QStringList& parNames,const QString& curve,
							const QString& f, const QString& fitType);


	//histograms
	void initHistogram(long curveID, const QMemArray<double>& Y, int it);
	void updateHistogram(Table* w, const QString& curveName, int curve);
	void updateHistogram(Table* w, const QString& curveName, int curve, bool automatic, 
							double binSize, double begin, double end);
	void setBarsGap(int curve, int gapPercent, int offset);
	QString showHistogramStats(Table* w, const QString& curveName, int curve);

	//image analyse tools
	 bool lineProfile();
	 void calculateProfile(int average, bool ok);
	 void calculateLineProfile(const QPoint& start, const QPoint& end);
	 int averageImagePixel(const QImage& image, int px, int py, int average, bool moreHorizontal);
	 void showIntensityTable();
	 
	 //user defined functions
	 void modifyFunctionCurve(int curve, QString& type,QStringList &formulas,QStringList &vars,QValueList<double> &ranges,QValueList<int> &points);
	 void addFunctionCurve(QString& type,QStringList &formulas,QStringList &vars,QValueList<double> &ranges,QValueList<int> &points);	 
	 //when reading from file
	 void insertFunctionCurve(const QString& formula, double from, double to, int points);
	 //for versions <0.4.3
	 void insertOldFunctionCurve(const QString& formula, double from, double step, int points);

	 void createWorksheet(const QString& name);
	 void activateGraph();
	 void moveGraph(const QPoint& pos);
	 void releaseGraph();
	 void highlightGraph(){emit highlightGraph(this);};

	//vector curves
	void plotVectorCurve(Table* w, const QStringList& colList, int style);
	void setVectorsLook(int curve, const QColor& c, int width, int arrowLength,
						int arrowAngle, bool filled, int position);
	void updateVectorsLayout(Table *w, int curve, int colorIndex, int width, 
								int arrowLength, int arrowAngle, bool filled, int position,
								const QString& xEndColName, const QString& yEndColName);
	void updateVectorsData(Table* w,  int curve);
	
	//box plots
	void openBoxDiagram(Table *w, const QStringList& l);
	void plotBoxDiagram(Table *w, const QStringList& names);

	void setCurveSymbol(int index, const QwtSymbol& s);
	void setCurvePen(int index, const QPen& p);
	void setCurveBrush(int index, const QBrush& b);
	void setCurveStyle(int index, int s);

	//resizing	
	bool ignoresResizeEvents(){return ignoreResize;};
	void setIgnoreResizeEvents(bool ok){ignoreResize=ok;};
	void resizeEvent(QResizeEvent *e);
	void scaleFonts(double factor);

	void modified();
	void emitModified();

	void updateSecondaryAxis(int axis);
	void enableAutoscaling(bool yes){autoscale = yes;};

	bool autoscaleFonts(){return autoScaleFonts;};
	void setAutoscaleFonts(bool yes){autoScaleFonts = yes;};

	static int obsoleteSymbolStyle(int type);
	static Qt::PenStyle getPenStyle(const QString& s);
	static Qt::PenStyle getPenStyle(int style);
	static Qt::BrushStyle getBrushStyle(int style);
	static QColor color(int item);
	static void showPlotErrorMessage(QWidget *parent, const QStringList& emptyColumns);

	void showTitleContextMenu();
	void copyTitle();
	void cutTitle();

	void removeAxisTitle();
	void cutAxisTitle();
	void copyAxisTitle();
	void showAxisTitleMenu(int axis);
	void showAxisContextMenu(int axis);
	void hideSelectedAxis();
	void showGrids();
	void showAxisDialog();
	void showScaleDialog();

	QStringList fitResults(){return fit_results;};
	void multiPeakFit(int fitType, int peaks);
	void selectPeak(const QPoint &pos);
	void fitMultiPeak(int fitType, const QString& curveTitle);
	bool selectPeaksOn();

signals:
	void highlightGraph(Graph*);
    void releaseGraph(Graph*);
	void moveGraph(Graph*, const QPoint& pos);
    void selectedGraph (Graph*);
	void closedGraph();
	void drawTextOff();
	void drawLineOff();
	void cursorInfo(const QString&);
	void showPlotDialog(long);
	void showPieDialog();
	void createTable(const QString&,int,int,const QString&);
	void createHiddenTable(const QString&,int,int,const QString&);
	void updateTable(const QString&,int,const QString&);
	void updateTableColumn(const QString&, double *, int);
	void clearCell(const QString&,double);

	void viewImageDialog();
	void viewTextDialog();
	void viewLineDialog();
	void viewTitleDialog();
	void modifiedGraph();
	void modifiedGraph(Graph *);
	void hiddenPlot(QWidget*);

	void modifiedFunction();
	void modifiedPlotAssociation();

	void showContextMenu();
	void showMarkerPopupMenu();
	
	void showAxisDialog(int);
	void axisDblClicked(int);	
	void xAxisTitleDblClicked();		
	void yAxisTitleDblClicked();
	void rightAxisTitleDblClicked();
	void topAxisTitleDblClicked();
	
	void createTablePlot(const QString&,int,int,const QString&);
	void createIntensityTable(const QPixmap&);
	void createHistogramTable(const QString&,int,int,const QString&);
	void dataRangeChanged();
	void showFitResults(const QString&);
	
private:
	bool autoScaleFonts;
	int selectedAxis;
	QStringList axesFormulas;
	QStringList axesFormatInfo;//stores columns used for axes with text labels or  time/date format info
	QValueList <int> axisType;
	QValueList <int> lblFormat; //stores label format used for the axes
	QwtScaleMap xCanvasMap, yCanvasMap;
	gridOptions grid;
	MarkerType selectedMarkerType;
	QwtPlotMarker::LineStyle mrklStyle;
	QStringList scales,associations;
	QMemArray<int> c_type; //curve types
	QMemArray<long> c_keys; // arrows on plot keys
	QMemArray<long> lines; // arrows on plot keys
	QMemArray<long> images; // images on plot keys
	QStringList tickLabelsOn;// tells wich axes have tick labels enabled
	QPen mrkLinePen;
	QFont auxMrkFont, defaultMarkerFont;
	QColor auxMrkColor, auxMrkBkgColor;
	QPoint auxMrkStart,auxMrkEnd;
	Qt::PenStyle auxMrkStyle;
	QString auxMrkFileName, auxMrkText;

	int n_curves, selectedCurve, selectedPoint,startPoint,endPoint, selectedCursor, pieRay;
	int selectedCol,xCol,widthLine,fitID,linesOnPlot, defaultMarkerFrame;
	int auxMrkAngle,auxMrkBkg,auxMrkWidth, averagePixels;
	int auxArrowHeadLength, auxArrowHeadAngle;
	int axesLineWidth, translationDirection;
	long selectedMarker,legendMarkerID, startID, endID, functions;
	long mrkX,mrkY;//x=0 et y=0 line markers keys
	bool startArrowOn, endArrowOn, drawTextOn, drawLineOn;
	
	//the following bools tell iwhich data tool is activated by the user
	bool removePointsEnabled,movePointsEnabled, translateOn;
	bool pickerEnabled, cursorEnabled, rangeSelectorsEnabled;	
	bool piePlot;//tells if the plot is a pie plot
	bool lineProfileOn; // tells if pixel line profile is asked
	bool isTitleSelected, auxFilledArrowHead, ignoreResize;
	bool drawAxesBackbone, autoscale;

	QStringList fit_results;
	double *peaks_array;
	int n_peaks, selected_peaks, fit_type;
};

class PrintFilter: public QwtPlotPrintFilter
{
public:
    PrintFilter(QwtPlot *insertCurve) 
	{
	// FIXME: This is a workaround, proper grid handling must be implemented
	gridMajorColor = QColor(Qt::black);
	gridMinorColor = QColor(Qt::black);
	// orig. code:
	//X gridMajorColor=insertCurve->gridMajPen().color();
	//X gridMinorColor=insertCurve->gridMinPen().color();		
	};

	virtual QColor color(const QColor &c, Item item, int) const
    {
        if ( !(options() & PrintCanvasBackground))
        {
            switch(item)
            {
            case MajorGrid:
                return gridMajorColor;
            case MinorGrid:
                return gridMinorColor;
            default:	
                ;
            }
        }
        return c;
    }

private:
	QColor gridMajorColor,gridMinorColor;
};

#endif // GRAPH_H

