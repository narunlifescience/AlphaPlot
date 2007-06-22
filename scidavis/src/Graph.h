/***************************************************************************
    File                 : Graph.h
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

#include <QList>
#include <QPointer>
#include <QPrinter>
#include <QVector>
#include <QEvent>

#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>

#include "Plot.h"
#include "Table.h"
#include "AxesDialog.h"
#include "PlotToolInterface.h"

class QwtPlotCurve;
class QwtPlotZoomer;
class QwtPieCurve;
class Table;
class Legend;
class ArrowMarker;
class ImageMarker;
class TitlePicker;
class ScalePicker;
class CanvasPicker;
class ApplicationWindow;
class Matrix;
class SelectionMoveResizer;
class RangeSelectorTool;
class DataCurve;
class PlotCurve;
class QwtErrorPlotCurve;

//! Structure containing curve layout parameters
typedef struct{
  int lCol;        //!< line color
  int lWidth;      //!< line width
  int lStyle;      //!< line style
  int filledArea;  //!< flag: toggles area filling under curve
  int aCol;        //!< curve area color
  int aStyle;      //!< area filling style
  int symCol;      //!< symbol outline color
  int fillCol;     //!< symbol fill color
  int penWidth;    //!< symbol outline width
  int sSize;       //!< symbol size
  int sType;       //!< symbol type (shape)
  int connectType; //!< symbol connection type
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
		enum MarkerType{None = -1, Text = 0, Arrow = 1, Image = 2};
		enum CurveType{Line, Scatter, LineSymbols, VerticalBars, Area, Pie, VerticalDropLines,
			Spline, HorizontalSteps, Histogram, HorizontalBars, VectXYXY, ErrorBars,
			Box, VectXYAM, VerticalSteps, ColorMap, GrayMap, ContourMap, Function};

		Plot *d_plot;
		QwtPlotZoomer *d_zoomer[2];
		TitlePicker *titlePicker;
		ScalePicker *scalePicker;
		CanvasPicker* cp;

		//! Returns the name of the parent MultiLayer object.
		QString parentPlotName();

		//! Change the active tool, deleting the old one if it exists.
		void setActiveTool(PlotToolInterface *tool) { if(d_active_tool) delete d_active_tool; d_active_tool=tool; }
		//! Return the active tool, or NULL if none is active.
		PlotToolInterface* activeTool() const { return d_active_tool; }

	public slots:
		//! Accessor method for #d_plot.
		Plot* plotWidget(){return d_plot;};
		void copy(Graph* g);

		//! \name Pie Curves
		//@{
		//! Returns true if this Graph is a pie plot, false otherwise.
		bool isPiePlot(){return (c_type.count() == 1 && c_type[0] == Pie);};
		void plotPie(Table* w,const QString& name, int startRow = 0, int endRow = -1);
		//! Used when restoring a pie plot from a project file
		void plotPie(Table* w,const QString& name, const QPen& pen, int brush, int size, int firstColor, int startRow = 0, int endRow = -1, bool visible = true);
		void removePie();
		QString pieLegendText();
		QString savePieCurveLayout();
		//@}

		bool insertCurvesList(Table* w, const QStringList& names, int style, int lWidth, int sSize, int startRow = 0, int endRow = -1);
		bool insertCurve(Table* w, const QString& name, int style, int startRow = 0, int endRow = -1);
		bool insertCurve(Table* w, int xcol, const QString& name, int style);
		bool insertCurve(Table* w, const QString& xColName, const QString& yColName, int style, int startRow = 0, int endRow = -1);
		void insertPlotItem(QwtPlotItem *i, int type);

		//! Shows/Hides a curve defined by its index.
		void showCurve(int index, bool visible = true);
		int visibleCurves();

		//! Removes a curve defined by its index.
		void removeCurve(int index);
		/**
		 * \brief Removes a curve defined by its title string s.
		 */
		void removeCurve(const QString& s);
		/**
		 * \brief Removes all curves defined by the title/plot association string s.
		 */
		void removeCurves(const QString& s);

		void updateCurvesData(Table* w, const QString& yColName);

		int curves(){return n_curves;};
		bool validCurvesDataSize();
		double selectedXStartValue();
		double selectedXEndValue();

        long curveKey(int curve){return c_keys[curve];}
		int curveIndex(long key){return c_keys.indexOf(key);};
		//! Map curve pointer to index.
		int curveIndex(QwtPlotCurve *c) const;
		//! map curve title to index
  	    int curveIndex(const QString &title){return plotItemsList().findIndex(title);}
  	    //! get curve by index
  	    QwtPlotCurve* curve(int index);
  	    //! get curve by name
  	    QwtPlotCurve* curve(const QString &title){return curve(curveIndex(title));}

		//! Returns the names of all the curves suitable for data analysis, as a string list. The list excludes error bars and spectrograms.
		QStringList analysableCurvesList();
		//! Returns the names of all the QwtPlotCurve items on the plot, as a string list
  		QStringList curvesList();
  	    //! Returns the names of all plot items, including spectrograms, as a string list
  		QStringList plotItemsList();
  		 //! get plotted item by index
  	    QwtPlotItem* plotItem(int index);
  	    //! get plot item by index
  	    int plotItemIndex(QwtPlotItem *it) const;

        void updateCurveNames(const QString& oldName, const QString& newName, bool updateTableName = true);

		int curveType(int curveIndex);
		void setCurveType(int curve, int style);
		void setCurveFullRange(int curveIndex);

		//! \name Output: Copy/Export/Print
		//@{
		void print();
		void setScaleOnPrint(bool on){d_scale_on_print = on;};
		void printCropmarks(bool on){d_print_cropmarks = on;};

		void copyImage();
		QPixmap graphPixmap();
		//! Provided for convenience in scripts
		void exportToFile(const QString& fileName);
		void exportSVG(const QString& fname);
		void exportVector(const QString& fileName, int res = 0, bool color = true,
                        bool keepAspect = true, QPrinter::PageSize pageSize = QPrinter::Custom);
		void exportImage(const QString& fileName, int quality = 100, bool transparent = false);
		//@}

		void replot(){d_plot->replot();};
		void updatePlot();

		//! \name Error Bars
		//@{
		bool addErrorBars(const QString& xColName, const QString& yColName, Table *errTable,
				const QString& errColName, int type = 1, int width = 1, int cap = 8, const QColor& color = QColor(Qt::black),
				bool through = true, bool minus = true, bool plus = true);

		bool addErrorBars(const QString& yColName, Table *errTable, const QString& errColName,
				int type = 1, int width = 1, int cap = 8, const QColor& color = QColor(Qt::black),
				bool through = true, bool minus = true, bool plus = true);

		void updateErrorBars(QwtErrorPlotCurve *er, bool xErr,int width, int cap, const QColor& c, bool plus, bool minus, bool through);

		//! Returns a valid master curve for the error bars curve.
		DataCurve* masterCurve(QwtErrorPlotCurve *er);
		//! Returns a valid master curve for a plot association.
		DataCurve* masterCurve(const QString& xColName, const QString& yColName);
		//@}

		//! \name Event Handlers
		//@{
		void contextMenuEvent(QContextMenuEvent *);
		void closeEvent(QCloseEvent *e);
		bool focusNextPrevChild ( bool next );
		//@}

		//! Set axis scale
		void setScale(int axis, double start, double end, double step = 0.0,
				int majorTicks = 5, int minorTicks = 5, int type = 0, bool inverted = false);
		double axisStep(int axis){return d_user_step[axis];};

		//! \name Curves Layout
		//@{
		CurveLayout initCurveLayout(int style, int curves = 0);
		static CurveLayout initCurveLayout();
		void updateCurveLayout(int index,const CurveLayout *cL);
		//! Tries to guess not already used curve color and symbol style
		void guessUniqueCurveLayout(int& colorIndex, int& symbolIndex);
		//@}

		GridOptions gridOptions(){return grid;};
		void setGridOptions(const GridOptions& options);

		//! \name Zoom
		//@{
		void zoomed (const QwtDoubleRect &);
		void zoom(bool on);
		void zoomOut();
		bool zoomOn();
		//@}

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
		long insertTextMarker(Legend* mrk);

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
		QVector<int> textMarkerKeys(){return d_texts;};
		Legend* textMarker(long id);

		void addTimeStamp();

		void removeLegend();
		void removeLegendItem(int index);
		void addLegendItem(const QString& colName);
		void insertLegend(const QStringList& lst, int fileVersion);
		Legend *legend();
		Legend *newLegend();
		Legend *newLegend(const QString& text);
		bool hasLegend(){return legendMarkerID >= 0;};

		//! Creates a new legend text using the curves titles
		QString legendText();
		//@}

		//! \name Line Markers
		//@{
		ArrowMarker* arrow(long id);
		void addArrow(ArrowMarker* mrk);

		//! Used when opening a project file
		void addArrow(QStringList list, int fileVersion);
		QVector<int> lineMarkerKeys(){return d_lines;};

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
        bool arrowMarkerSelected();
		//@}

		//! \name Image Markers
		//@{
		ImageMarker* imageMarker(long id);
		QVector<int> imageMarkerKeys(){return d_images;};
		ImageMarker* addImage(ImageMarker* mrk);
		ImageMarker* addImage(const QString& fileName);

		void insertImageMarker(const QStringList& lst, int fileVersion);
		bool imageMarkerSelected();
		void updateImageMarker(int x, int y, int width, int height);
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
		 * If <i>add</i> is false (the default) or there is no existing selection, a new SelectionMoveResizer is
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
		QList<int> axesType();
		void setAxesType(const QList<int> tl);

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

		void enableAxis(int axis, bool on = true);
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

		QList<int> axesBaseline();
		void setAxesBaseline(const QList<int> &lst);
		void setAxesBaseline(QStringList &lst);

		void setMajorTicksType(const QList<int>& lst);
		void setMajorTicksType(const QStringList& lst);

		void setMinorTicksType(const QList<int>& lst);
		void setMinorTicksType(const QStringList& lst);

		int minorTickLength();
		int majorTickLength();
		void setAxisTicksLength(int axis, int majTicksType, int minTicksType,
				int minLength, int majLength);
		void setTicksLength(int minLength, int majLength);
		void changeTicksLength(int minLength, int majLength);

		void setLabelsNumericFormat(const QStringList& l);
		void setLabelsNumericFormat(int axis, const QStringList& l);
		void setLabelsNumericFormat(int axis, int format, int prec = 6, const QString& formula = QString());
		void setLabelsDateTimeFormat(int axis, int type, const QString& formatInfo);
		void setLabelsDayFormat(int axis, int format);
		void setLabelsMonthFormat(int axis, int format);

		QString axisFormatInfo(int axis);
		QStringList axesLabelsFormatInfo(){return axesFormatInfo;};

		void setLabelsTextFormat(int axis, int type, const QString& name, const QStringList& lst);
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
		int selectedCurveID();
		int selectedCurveIndex() { return curveIndex(selectedCurveID()); }
		QString selectedCurveTitle();
		//@}

		void disableTools();

		/*! Enables the data range selector tool.
		 *
		 * This one is a bit special, because other tools can depend upon an existing selection.
		 * Therefore, range selection (like zooming) has to be provided in addition to the generic
		 * tool interface.
		 */
		bool enableRangeSelectors(const QObject *status_target=NULL, const char *status_slot="");

		//! \name Border and Margin
		//@{
		void setMargin (int d);
		void setFrame(int width = 1, const QColor& color = QColor(Qt::black));
		void setBackgroundColor(const QColor& color);
		void setCanvasBackground(const QColor& color);
		//@}

		void addFitCurve(QwtPlotCurve *c);
		void deleteFitCurves();
		QList<QwtPlotCurve *> fitCurvesList(){return d_fit_curves;};
		/*! Set start and end to selected X range of curve index or, if there's no selection, to the curve's total range.
		 *
		 * \return the number of selected or total points
		 */
		int range(int index, double *start, double *end);

		//!  Used for VerticalBars, HorizontalBars and Histograms
		void setBarsGap(int curve, int gapPercent, int offset);

		//! \name Image Analysis Tools
		//@{
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

        //! Provided for convenience in scripts.
		void createTable(const QString& curveName);
        void createTable(const QwtPlotCurve* curve);
		void activateGraph();

		//! \name Vector Curves
		//@{
		void plotVectorCurve(Table* w, const QStringList& colList, int style, int startRow = 0, int endRow = -1);
		void updateVectorsLayout(int curve, const QColor& color, int width, int arrowLength, int arrowAngle, bool filled, int position,
				const QString& xEndColName = QString(), const QString& yEndColName = QString());
		//@}

		//! \name Box Plots
		//@{
		void openBoxDiagram(Table *w, const QStringList& l, int fileVersion);
		void plotBoxDiagram(Table *w, const QStringList& names, int startRow = 0, int endRow = -1);
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

		void notifyChanges();

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
		static QPrinter::PageSize minPageSize(const QPrinter& printer, const QRect& r);

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

		//! Convenience function enabling the grid for QwtScaleDraw::Left and Bottom Scales
		void showGrid();
		//! Convenience function enabling the grid for a user defined axis
		void showGrid(int axis);

		void showAxisDialog();
		void showScaleDialog();

		//! Add a spectrogram to the graph
  		void plotSpectrogram(Matrix *m, CurveType type);
		//! Restores a spectrogram. Used when opening a project file.
  		void restoreSpectrogram(ApplicationWindow *app, const QStringList& lst);

		bool antialiasing(){return d_antialiasing;};
		//! Enables/Disables antialiasing of plot items.
		void setAntialiasing(bool on = true, bool update = true);

signals:
		void selectedGraph (Graph*);
		void closedGraph();
		void drawTextOff();
		void drawLineEnded(bool);
		void cursorInfo(const QString&);
		void showPlotDialog(int);
		void createTable(const QString&,int,int,const QString&);

		void viewImageDialog();
		void viewTextDialog();
		void viewLineDialog();
		void viewTitleDialog();
		void modifiedGraph();
		void hiddenPlot(QWidget*);

		void showContextMenu();
		void showCurveContextMenu(int);
		void showMarkerPopupMenu();

		void showAxisDialog(int);
		void axisDblClicked(int);
		void xAxisTitleDblClicked();
		void yAxisTitleDblClicked();
		void rightAxisTitleDblClicked();
		void topAxisTitleDblClicked();

		void createIntensityTable(const QString&);
		void dataRangeChanged();
		void showFitResults(const QString&);

	private:
		//! List storing pointers to the curves resulting after a fit session, in case the user wants to delete them later on.
		QList<QwtPlotCurve *>d_fit_curves;
		//! Render hint for plot items.
		bool d_antialiasing;
		bool autoScaleFonts;
		bool d_scale_on_print, d_print_cropmarks;
		int selectedAxis;
		QStringList axesFormulas;
		//! Stores columns used for axes with text labels or time/date format info
		QStringList axesFormatInfo;
		QList <int> axisType;
		//! Structure used to define the grid
		GridOptions grid;
		MarkerType selectedMarkerType;
		QwtPlotMarker::LineStyle mrklStyle;

		//! Stores the step the user specified for the four scale. If step = 0.0, the step will be calculated automatically by the Qwt scale engine.
		QVector<double> d_user_step;
		//! Curve types
		QVector<int> c_type;
		//! Curves on plot keys
		QVector<int> c_keys;
		//! Arrows/lines on plot keys
		QVector<int> d_lines;
		//! Images on plot keys
		QVector<int> d_images;
		//! Stores the identifiers (keys) of the text objects on the plot
		QVector<int> d_texts;

		QPen mrkLinePen;
		QFont auxMrkFont, defaultMarkerFont;
		QColor auxMrkColor, auxMrkBkgColor;
		QPoint auxMrkStart, auxMrkEnd;
		Qt::PenStyle auxMrkStyle;
		QString auxMrkFileName, auxMrkText;

		int n_curves;
		int widthLine, defaultMarkerFrame;
		QColor defaultTextMarkerColor, defaultTextMarkerBackground;
		int auxMrkAngle,auxMrkBkg,auxMrkWidth;
		int auxArrowHeadLength, auxArrowHeadAngle;
		long selectedMarker,legendMarkerID;
		long mrkX, mrkY;//x=0 et y=0 line markers keys
		bool startArrowOn, endArrowOn, drawTextOn, drawLineOn, drawArrowOn;

		bool auxFilledArrowHead, ignoreResize;
		bool drawAxesBackbone, autoscale;

		QColor defaultArrowColor;
		int defaultArrowLineWidth, defaultArrowHeadLength, defaultArrowHeadAngle;
		bool defaultArrowHeadFill;
		Qt::PenStyle defaultArrowLineStyle;

		//! The markers selected for move/resize operations or NULL if none are selected.
		QPointer<SelectionMoveResizer> d_markers_selector;
		//! The current curve selection, or NULL if none is active.
		QPointer<RangeSelectorTool> d_range_selector;
		//! The currently active tool, or NULL for default (pointer).
		PlotToolInterface *d_active_tool;
};
#endif // GRAPH_H
