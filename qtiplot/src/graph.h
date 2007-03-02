/***************************************************************************
    File                 : graph.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Graph widget
                           
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
#ifndef GRAPH_H
#define GRAPH_H

/*#include <QResizeEvent>
#include <QContextMenuEvent>
#include <QCloseEvent>*/

#include <Q3ValueList>
#include <Q3MemArray>
#include <QPointer>
#include <QPrinter>
#include <QVector>
#include <QEvent>

#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>

#include "worksheet.h"
#include "axesDialog.h"

class QwtPlotCurve;
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
class MultiPeakFit;
class ApplicationWindow;
class Matrix;
class SelectionMoveResizer;
	
//! Structure containing curve layout parameters
typedef struct{
  int lCol;
  int lWidth;
  int lStyle;
  int filledArea;
  int aCol;
  int aStyle;
  int symCol;
  int fillCol;
  int penWidth;
  int sSize;
  int sType;
  int connectType;
}  CurveLayout;

/**
 * \brief A 2D-plotting widget.
 *
 * Graphs are managed by a MultiLayer, where they are sometimes referred to as "graphs" and sometimes as "layers".
 * Other parts of the code also call them "plots", regardless of the fact that there's also a class Plot.
 * Within the user interface, they are quite consistently called "layers".
 *
 * Each graph owns a Plot called #d_plot, which handles parts of the curve, axis and marker management (similarly to QwtPlot),
 * as well as the pickers #d_zoomer (a QwtPlotZoomer), #titlePicker (a TitlePicker), #scalePicker (a ScalePicker) and #cp (a CanvasPicker),
 * which handle various parts of the user interaction.
 *
 * Graph contains support for various curve types (see #CurveType),
 * some of them relying on Qtiplot-specific QwtPlotCurve subclasses for parts of the functionality.
 *
 * %Note that some of Graph's methods are implemented in analysis.cpp.
 *
 * \section future Future Plans
 * Merge with Plot and CanvasPicker.
 * Think about merging in TitlePicker and ScalePicker.
 * On the other hand, things like range selection, peak selection or (re)moving data points could be split out into tool classes
 * like QwtPlotZoomer or SelectionMoveResizer.
 *
 * What definitely should be split out are plot types like histograms and pie charts (TODO: which others?).
 * We need a generic framework for this in any case so that new plot types can be implemented in plugins,
 * and Graph could do with a little diet. Especially after merging in Plot and CanvasPicker.
 * [ Framework needs to support plug-ins; assigned to knut ]
 *
 * Add support for floating-point line widths of curves and axes (request 2300).
 * [ assigned to thzs ]
 */

class Graph: public QWidget
{
	Q_OBJECT

	public:
		Graph (QWidget* parent=0, const char* name=0, Qt::WFlags f=0);
		~Graph();

		enum AxisType{Numeric = 0, Txt = 1, Day = 2, Month = 3, Time = 4, Date = 5, ColHeader = 6};
		enum MarkerType{None=-1, Text = 0, Arrow=1, Image=2};
		enum CurveType{Line, Scatter, LineSymbols, VerticalBars , Area, Pie, VerticalDropLines, 
			Spline, HorizontalSteps, Histogram, HorizontalBars, VectXYXY, ErrorBars, 
			Box, VectXYAM, VerticalSteps, ColorMap, GrayMap, ContourMap};

		Plot *d_plot;
		QwtPlotZoomer *d_zoomer[2];
		TitlePicker *titlePicker;
		ScalePicker *scalePicker;
		CanvasPicker* cp;

		//! Returns the name of the parent MultiLayer object.
		QString parentPlotName();

	public slots:
		//! Accessor method for #d_plot.
		Plot* plotWidget(){return d_plot;};
		void copy(Graph* g);

		//! \name Pie Curves
		//@{
		//! Returns true if this Graph is a pie plot, false else.
		bool isPiePlot(){return piePlot;};
		void plotPie(QwtPieCurve* curve);
		void plotPie(Table* w,const QString& name);
		void plotPie(Table* w,const QString& name,const QPen& pen, int brush, int size, int firstColor);
		void updatePie(const QPen& pen, const Qt::BrushStyle &brushStyle, int size, int firstColor);
		void updatePieCurveData(Table* w, const QString& yColName, int curve);
		void removePie();
		QString pieLegendText();
		QString savePieCurveLayout();
		//@}

		bool insertCurvesList(Table* w, const QStringList& names, int style, int lWidth, int sSize);
		bool insertCurve(Table* w, const QString& name, int style);
		bool insertCurve(Table* w, int xcol, const QString& name, int style);
		bool insertCurve(Table* w, const QString& xColName, const QString& yColName, int style);
		void insertPlotItem(QwtPlotItem *i, const QString& plotAssociation, int type);

		//! Removes a curve defined by its index.
		void removeCurve(int index);

		/**
		 * \brief Removes a curve defined by the title/plot association string s.
		 *
		 * See #associations for a documentation of the format of s.
		 * \sa #associations, curvesList()
		 */
		void removeCurve(const QString& s);

		void updateData(Table* w, int curve);
		void updateCurveData(Table* w, const QString& yColName, int curve);
		void updateBoxData(Table* w, const QString& yColName, int curve);

		int curves(){return n_curves;};
		bool validCurvesDataSize();
		double selectedXStartValue();
		double selectedXEndValue();

		long curveKey(int curve);
		int curveIndex(long key);
		//! map curve title to index
  	    int curveIndex(const QString &title) { return curvesList().findIndex(title); }
  	    //! get curve by index
  	    QwtPlotCurve* curve(int index);
  	    //! get curve by name
  	    QwtPlotCurve* curve(const QString &title) { return curve(curveIndex(title)); }

		QString curveXColName(const QString& curveTitle);

		void insertPlottedList(const QStringList& names);
		//! Returns the names of all the QwtPlotCurve items on the plot, as a string list
  		QStringList curvesList();
  	    //! Returns the names of all plot items, including spectrograms, as a string list
  		QStringList plotItemsList();
		QStringList plotAssociations();
		void setPlotAssociations(const QStringList& newList);
		void changePlotAssociation(Table* t, int curve, const QString& text);

		int curveType(int curveIndex);
		void setCurveType(int curve, int style);

		//! \name Output: Copy/Export/Print
		//@{
		void print();
		void copyImage();
		QPixmap graphPixmap();
		//! Provided for convenience in scripts
		void exportLayer(const QString& fileName);
		void exportSVG(const QString& fname);
		void exportPDF(const QString& fileName, int res = 0, QPrinter::Orientation o = QPrinter::Landscape, 
						QPrinter::PageSize size = QPrinter::A5, QPrinter::ColorMode col = QPrinter::Color);		
		void exportEPS(const QString& fileName, int res = 0, QPrinter::Orientation o = QPrinter::Landscape, 
						QPrinter::PageSize size = QPrinter::A5, QPrinter::ColorMode col = QPrinter::Color);		
		void exportVector(const QString& fileName, const QString& fileType = "pdf", int res = 0, 
						  QPrinter::Orientation o = QPrinter::Landscape, QPrinter::PageSize size = QPrinter::A5, 
						  QPrinter::ColorMode col = QPrinter::Color);
		void exportImage(const QString& fileName, const QString& fileType, int quality = 100, bool transparent = false);
		//@}
		
		void replot();
		void updatePlot();

		//! \name Error Bars
		//@{
		bool addErrorBars(Table *w, const QString& xColName, const QString& yColName, Table *errTable, 
				const QString& errColName, int type = 1, int width = 1, int cap = 8, const QColor& color = QColor(Qt::black),
				bool through = true, bool minus = true, bool plus = true, double xOffset = 0, double yOffset = 0);

		bool addErrorBars(Table *w, const QString& yColName, Table *errTable, const QString& errColName,
				int type = 1, int width = 1, int cap = 8, const QColor& color = QColor(Qt::black),
				bool through = true, bool minus = true, bool plus = true);

		void updateErrorBarsData(Table* w, int curve);
		void updateErrorBars(int curve, bool xErr,int width, int cap, const QColor& c, bool plus, bool minus, bool through);

		//! Called when a bar curve associated to an error bars curve curve is deleted
		void resetErrorBarsOffset(int index);
		//@}

		//! \name Event Handlers 
		//@{
		void contextMenuEvent(QContextMenuEvent *);
		void closeEvent(QCloseEvent *e);
		//@}

		//! Set axis scale
		void setScale(int axis, double start, double end, double step = 0.0, 
				int majorTicks = 5, int minorTicks = 5, int type = 0, bool inverted = false);
		double axisStep(int axis){return d_user_step[axis];};

		//! \name Curves Layout
		//@{
		CurveLayout initCurveLayout(int i, int curves, int style);
		static CurveLayout initCurveLayout();
		void updateCurveLayout(int index,const CurveLayout *cL);
		//! Tries to guess not already used curve color and symbol style
		void guessUniqueCurveLayout(int& colorIndex, int& symbolIndex);
		//@}

		GridOptions getGridOptions();
		void setGridOptions(const GridOptions& options);

		//! \name Zoom
		//@{
		void zoomed (const QwtDoubleRect &);
		void zoom(bool on);
		void zoomOut();
		bool zoomOn();
		//@}

		void movedPicker(const QPoint &pos, bool mark);
		void setAutoScale();
		void updateScale();

		//! \name Saving to File 
		//@{
		QString saveToString(bool saveAsTemplate = false);
		QString saveGridOptions();
		QString saveScale();
		QString saveScaleTitles();
		QString saveFonts();
		QString saveMarkers();
		QString saveCurveLayout(int index);
		QString saveAxesTitleColors();
		QString saveAxesColors();
		QString saveEnabledAxes();
		QString saveCanvas();
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
		//@}

		//! \name Text Markers 
		//@{
		void drawText(bool on);
		bool drawTextActive(){return drawTextOn;};
		long insertTextMarker(LegendMarker* mrk);

		//! Used when opening a project file
		long insertTextMarker(const QStringList& list, int fileVersion);
		void updateTextMarker(const QString& text,int angle, int bkg,const QFont& fnt,
				const QColor& textColor, const QColor& backgroundColor);

		QFont defaultTextMarkerFont(){return defaultMarkerFont;};
		QColor textMarkerDefaultColor(){return defaultTextMarkerColor;};
		QColor textMarkerDefaultBackground(){return defaultTextMarkerBackground;};
		int textMarkerDefaultFrame(){return defaultMarkerFrame;};
		void setTextMarkerDefaults(int f, const QFont &font, const QColor& textCol, const QColor& backgroundCol);

		void setCopiedMarkerType(Graph::MarkerType type){selectedMarkerType=type;};
		void setCopiedMarkerEnds(const QPoint& start, const QPoint& end);
		void setCopiedTextOptions(int bkg, const QString& text, const QFont& font, 
				const QColor& color, const QColor& bkgColor);
		void setCopiedArrowOptions(int width, Qt::PenStyle style, const QColor& color,
				bool start, bool end, int headLength, int headAngle, bool filledHead);	
		void setCopiedImageName(const QString& fn){auxMrkFileName=fn;};	
		QRect copiedMarkerRect(){return QRect(auxMrkStart, auxMrkEnd);};
		QwtArray<long> textMarkerKeys(){return d_texts;};
		LegendMarker* textMarker(long id);

		void addTimeStamp();

		void removeLegend();
		void removeLegendItem(int index);
		void addLegendItem(const QString& colName);
		void insertLegend(const QStringList& lst, int fileVersion);
		LegendMarker *legend();
		void newLegend();
		LegendMarker *newLegend(const QString& text);
		bool hasLegend(){return legendMarkerID >= 0;};

		//! Creates a new legend text using the curves titles
		QString legendText();
		//@}

		//! \name Line Markers 
		//@{
		LineMarker* lineMarker(long id);
		void insertLineMarker(LineMarker* mrk);

		//! Used when opening a project file
		void insertLineMarker(QStringList list, int fileVersion);
		QwtArray<long> lineMarkerKeys(){return d_lines;};

		//!Draws a line/arrow depending on the value of "arrow"
		void drawLine(bool on, bool arrow = FALSE);
		bool drawArrow(){return drawArrowOn;};
		bool drawLineActive(){return drawLineOn;};

		Qt::PenStyle arrowLineDefaultStyle(){return defaultArrowLineStyle;};
		bool arrowHeadDefaultFill(){return defaultArrowHeadFill;};
		int arrowDefaultWidth(){return defaultArrowLineWidth;};
		int arrowHeadDefaultLength(){return defaultArrowHeadLength;};
		int arrowHeadDefaultAngle(){return defaultArrowHeadAngle;};
		QColor arrowDefaultColor(){return defaultArrowColor;};

		void setArrowDefaults(int lineWidth,  const QColor& c, Qt::PenStyle style,
				int headLength, int headAngle, bool fillHead);
		//@}

		//! \name Image Markers
		//@{
		ImageMarker* imageMarker(long id);
		QwtArray<long> imageMarkerKeys(){return d_images;};
		void insertImageMarker(ImageMarker* mrk);
		void insertImageMarker(const QPixmap& photo, const QString& fileName);

		void insertImageMarker(const QStringList& lst, int fileVersion);
		bool imageMarkerSelected();
		void updateImageMarker(int x, int y, int width, int height);

		bool arrowMarkerSelected();
		//@}

		//! \name Common to all Markers
		//@{
		void removeMarker();
		void cutMarker();
		void copyMarker();
		void pasteMarker();
		//! Keep the markers on screen each time the scales are modified by adding/removing curves
		void updateMarkersBoundingRect();

		long selectedMarkerKey();
		/*!\brief Set the selected marker.
		 * \param mrk key of the marker to be selected.
		 * \param add whether the marker is to be added to an existing selection.
		 * If \var add is false (the default) or there is no existing selection, a new SelectionMoveResizer is
		 * created and stored in #d_markers_selector.
		 */
		void setSelectedMarker(long mrk, bool add=false);
		QwtPlotMarker* selectedMarkerPtr();
		bool markerSelected();
		//! Reset any selection states on markers.
		void deselectMarker();
		MarkerType copiedMarkerType(){return selectedMarkerType;};
		//@}

		//! \name Axes 
		//@{
		Q3ValueList<int> axesType();
		void setAxesType(const Q3ValueList<int> tl); 

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
		void initFonts(const QFont &scaleTitleFnt,const QFont &numbersFnt);

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

        QColor axisColor(int axis);
		QStringList axesColors();
		void setAxesColors(const QStringList& colors);

        QColor axisNumbersColor(int axis);
  	    QStringList axesNumColors();
  	    void setAxesNumColors(const QStringList& colors);

		void showAxis(int axis, int type, const QString& formatInfo, Table *table, bool axisOn, 
				int majTicksType, int minTicksType, bool labelsOn, const QColor& c, int format,
                int prec, int rotation, int baselineDist, const QString& formula, const QColor& labelsColor);

		QVector<bool> enabledAxes();
		void enableAxes(QVector<bool> axesOn);
		void enableAxes(const QStringList& list);	

		int labelsRotation(int axis);
		void setAxisLabelRotation(int axis, int rotation);

		QStringList enabledTickLabels();
		void setEnabledTickLabels(const QStringList& list);

		void setAxesLinewidth(int width);
		//! used when opening a project file
		void loadAxesLinewidth(int width);

		void drawAxesBackbones(bool yes);
		bool axesBackbones(){return drawAxesBackbone;};
		//! used when opening a project file
		void loadAxesOptions(const QString& s);

		Q3ValueList<int> axesBaseline();
		void setAxesBaseline(const Q3ValueList<int> &lst);
		void setAxesBaseline(QStringList &lst);

		void setMajorTicksType(const Q3ValueList<int>& lst);
		void setMajorTicksType(const QStringList& lst);

		void setMinorTicksType(const Q3ValueList<int>& lst);
		void setMinorTicksType(const QStringList& lst);

		int minorTickLength();
		int majorTickLength();
		void setAxisTicksLength(int axis, int majTicksType, int minTicksType,
				int minLength, int majLength);
		void setTicksLength(int minLength, int majLength);
		void changeTicksLength(int minLength, int majLength);

		void setLabelsNumericFormat(const QStringList& l);
		void setLabelsNumericFormat(int axis, const QStringList& l);	
		void setLabelsNumericFormat(int axis, int format, int prec, const QString& formula);
		void setLabelsDateTimeFormat(int axis, int type, const QString& formatInfo);
		void setLabelsDayFormat(int axis, int format);
		void setLabelsMonthFormat(int axis, int format);

		QStringList axesLabelsFormatInfo(){return axesFormatInfo;};
		void setLabelsTextFormat(int axis, int type, const QString& labelsColName, Table *table);

		QStringList getAxesFormulas(){return axesFormulas;};
		void setAxesFormulas(const QStringList& l){axesFormulas = l;};
		void setAxisFormula(int pos, const QString &f){axesFormulas[pos] = f;};
		//@}

		//! \name Canvas Frame 
		//@{
		void drawCanvasFrame(bool frameOn, int width);
		void drawCanvasFrame(const QStringList& frame);
		void drawCanvasFrame(bool frameOn, int width, const QColor& color);
		QColor canvasFrameColor();
		int canvasFrameWidth();
		bool framed();
		//@}

		//! \name Plot Title
		//@{
		void setTitle(const QString& t);
		void setTitleFont(const QFont &fnt);
		void setTitleColor(const QColor &c);
		void setTitleAlignment(int align);

		bool titleSelected();
		void selectTitle();

		void removeTitle();
		void initTitle( bool on, const QFont& fnt);
		//@}

		//! \name Modifing insertCurve Data 
		//@{
		bool selectPoint(const QPoint &pos);
		void highlightPoint(bool showIt);
		void selectCurve(const QPoint &pos);
		int selectedCurveID(){return selectedCurve;};
		QString selectedCurveTitle();
		//@}

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

		void disableTools();

		//! \name Translating Curves
		//@{
		void translateCurve(int direction);
		void translateCurveTo(const QPoint& p);
		bool translationInProgress(){return translateOn;};
		void startCurveTranslation();
		//@}

		//! Returns TRUE if the data range selectors are enables, FALSE otherwise.
		bool selectorsEnabled(){return rangeSelectorsEnabled;};

		//! Enables the data range selectors depending on the value of ON (ON = false will call disableRangeSelectors()).
		bool enableRangeSelectors(bool on);

		//! Disables the data range selectors tool.
		void disableRangeSelectors();

		void moveRangeSelector(bool up);
		void moveRangeSelector();
		void shiftRangeSelector(bool shift);

		//! Select the next/previous curve 
		void shiftCurveSelector(bool up);
		int selectedPoints(long curveKey);

		//! \name Border and Margin
		//@{
		void changeMargin (int d);
		void drawBorder (int width, const QColor& color);
		void setBorder (int width, const QColor& color);
		void setBackgroundColor(const QColor& color);
		void setCanvasBackground(const QColor& color);
		//@}

		//! \name Functions in analysis.cpp
		//@{
		void deleteFitCurves();
   		//! Set start and end to selected X range of curve index or, if there's no selection, to the curve's total range.
        void range(int index, double *start, double *end);
		//@}

		//! \name Histograms
		//@{
		void initHistogram(long curveID, const Q3MemArray<double>& Y, int it);
		void updateHistogram(Table* w, const QString& curveName, int curve);
		void updateHistogram(Table* w, const QString& curveName, int curve, bool automatic, 
				double binSize, double begin, double end);
		void setBarsGap(int curve, int gapPercent, int offset);
		QString showHistogramStats(Table* w, const QString& curveName, int curve);
		//@}

		//! \name Image Analysis Tools
		//@{
		//! Return #lineProfileOn.
		bool lineProfile();
		void calculateProfile(int average, bool ok);
		void calculateLineProfile(const QPoint& start, const QPoint& end);
		int averageImagePixel(const QImage& image, int px, int py, int average, bool moreHorizontal);
		void showIntensityTable();
		//@}

		//! \name User-defined Functions
		//@{
		void modifyFunctionCurve(int curve, int type, const QStringList &formulas, const QString &var,QList<double> &ranges, int points);
		void addFunctionCurve(int type, const QStringList &formulas, const QString& var,
				QList<double> &ranges, int points, const QString& title = QString::null);	 
		//! Used when reading from a project file.
		void insertFunctionCurve(const QString& formula, int points, int fileVersion);
		//! Returns an unique function name
        QString generateFunctionName(const QString& name = tr("F"));
		//@}

		void createWorksheet(const QString& name);
		void activateGraph();

		//! \name Vector Curves
		//@{
		void plotVectorCurve(Table* w, const QStringList& colList, int style);
		void setVectorsLook(int curve, const QColor& c, int width, int arrowLength,
				int arrowAngle, bool filled, int position);
		void updateVectorsLayout(Table *w, int curve, int colorIndex, int width, 
				int arrowLength, int arrowAngle, bool filled, int position,
				const QString& xEndColName, const QString& yEndColName);
		void updateVectorsData(Table* w,  int curve);
		//@}

		//! \name Box Plots
		//@{
		void openBoxDiagram(Table *w, const QStringList& l);
		void plotBoxDiagram(Table *w, const QStringList& names);
		//@}

		void setCurveSymbol(int index, const QwtSymbol& s);
		void setCurvePen(int index, const QPen& p);
		void setCurveBrush(int index, const QBrush& b);
		void setCurveStyle(int index, int s);

		//! \name Resizing	
		//@{
		bool ignoresResizeEvents(){return ignoreResize;};
		void setIgnoreResizeEvents(bool ok){ignoreResize=ok;};
		void resizeEvent(QResizeEvent *e);
		void scaleFonts(double factor);
		//@}

		void modified();
		void emitModified();

		void updateSecondaryAxis(int axis);
		void enableAutoscaling(bool yes){autoscale = yes;};

		bool autoscaleFonts(){return autoScaleFonts;};
		void setAutoscaleFonts(bool yes){autoScaleFonts = yes;};

		static int obsoleteSymbolStyle(int type);
		static QString penStyleName(Qt::PenStyle style);
		static Qt::PenStyle getPenStyle(const QString& s);
		static Qt::PenStyle getPenStyle(int style);
		static Qt::BrushStyle getBrushStyle(int style);
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

		//! Initialize a multi peak fitting operations and creates a new MultiPeakFit object
		void multiPeakFit(ApplicationWindow *app, int profile, int peaks);
		void selectPeak(const QPoint &pos);
		bool selectPeaksOn();
		
		//! Add a spectrogram to the graph
  		void plotSpectrogram(Matrix *m, CurveType type);
		//! Restores a spectrogram. Used when opening a project file.
  		void restoreSpectrogram(ApplicationWindow *app, const QStringList& lst);

signals:
		void selectedGraph (Graph*);
		void closedGraph();
		void drawTextOff();
		void drawLineEnded(bool);
		void cursorInfo(const QString&);
		void showPlotDialog(int);
		void showPieDialog();
		void createTable(const QString&,int,int,const QString&);
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
		void showCurveContextMenu(int);
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
		//! Stores columns used for axes with text labels or  time/date format info
		QStringList axesFormatInfo;
		Q3ValueList <int> axisType;
		//! Structure used to define the grid
		GridOptions grid;
		MarkerType selectedMarkerType;
		QwtPlotMarker::LineStyle mrklStyle;
		/**
		 * \brief A list of data sources for my curves.
		 *
		 * Elements must be in either of the following forms:
		 *  - &lt;id of X column> "(X)," &lt;id of Y column> "(Y)" [ "," &lt;id of error column> ("(xErr)" | "(yErr)") ]
		 *  - &lt;id of Xstart column> "(X)," &lt;id of Ystart column>"(Y)," &lt;id of Xend column> "(X)," &lt;id of Yend column> "(Y)"\n
		 *    (denoting start and end coordinates for the #VectXYXY style)
		 *  - &lt;id of Xstart column> "(X)," &lt;id of Ystart column> "(Y)," &lt;id of angle column> "(A)," &lt;id of magnitude column> "(M)"\n
		 *    (denoting start coordinates, angle in radians and length for the #VectXYAM style)
		 *  - A function.
		 *    Some parts of the code test for a '=' in this context, others seem to use the format 'F&lt;number>'.
		 *    Formulas are stored in the associated FunctionCurve object.
		 * .
		 * Column ids are of the form '&lt;name of table> "_" &lt;name of column>'.
		 *
		 * [ TODO: what about pie charts, histograms, box plots and others? ]
		 *
		 * Access via plotAssociations() and setPlotAssociations().
		 * \sa curvesList()
		 */
		QStringList associations;

		//! Stores the step the user specified for the four scale. If step = 0.0, the step will be calculated automatically by the Qwt scale engine.
		QVector<double> d_user_step;
		//! Curve types
		Q3MemArray<int> c_type; 
		//! Curves on plot keys
		Q3MemArray<long> c_keys;
		//! Arrows/lines on plot keys
		Q3MemArray<long> d_lines; 
		//! Images on plot keys
		Q3MemArray<long> d_images; 
		//! Stores the identifiers (keys) of the text objects on the plot
		Q3MemArray<long> d_texts;

		QPen mrkLinePen;
		QFont auxMrkFont, defaultMarkerFont;
		QColor auxMrkColor, auxMrkBkgColor;
		QPoint auxMrkStart, auxMrkEnd;
		Qt::PenStyle auxMrkStyle;
		QString auxMrkFileName, auxMrkText;

		int n_curves, selectedCurve, selectedPoint,startPoint,endPoint, selectedCursor, pieRay;
		int selectedCol,xCol,widthLine, defaultMarkerFrame;
		QColor defaultTextMarkerColor, defaultTextMarkerBackground;
		int auxMrkAngle,auxMrkBkg,auxMrkWidth, averagePixels;
		int auxArrowHeadLength, auxArrowHeadAngle;
		int translationDirection;
		long selectedMarker,legendMarkerID, startID, endID;
		long mrkX,mrkY;//x=0 et y=0 line markers keys
		bool startArrowOn, endArrowOn, drawTextOn, drawLineOn, drawArrowOn;

		//! \name Which data tool is activated by the user
		//@{
		bool removePointsEnabled,movePointsEnabled, translateOn;
		bool pickerEnabled, cursorEnabled, rangeSelectorsEnabled;	
		//@}

		//! Whether the plot is a pie plot.
		bool piePlot;
		//! Whether pixel line profile is asked.
		bool lineProfileOn;
		bool auxFilledArrowHead, ignoreResize;
		bool drawAxesBackbone, autoscale;

		int selected_peaks;
		MultiPeakFit *fitter;

		QColor defaultArrowColor;
		int defaultArrowLineWidth, defaultArrowHeadLength, defaultArrowHeadAngle;
		bool defaultArrowHeadFill;
		Qt::PenStyle defaultArrowLineStyle;

	//! The markers selected for move/resize operations or 0 none are selected.
	QPointer<SelectionMoveResizer> d_markers_selector;
};
#endif // GRAPH_H
