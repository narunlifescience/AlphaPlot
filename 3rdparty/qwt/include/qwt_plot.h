/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_PLOT_H
#define QWT_PLOT_H

#include <qframe.h>
#include "qwt_global.h"
#include "qwt_array.h"
#include "qwt_grid.h"
#include "qwt_autoscl.h"
#include "qwt_scldraw.h"
#include "qwt_symbol.h"
#include "qwt_curve.h"
#include "qwt_marker.h"
#include "qwt_legend.h"
#include "qwt_plot_printfilter.h"
#include "qwt_plot_classes.h"
#include "qwt_plot_dict.h"

class QLabel;
class QwtPlotLayout;
class QwtScale;
class QwtPlotItem;
class QwtPlotCanvas;
class QwtPlotPrintFilter;

/*!
  \brief A 2-D plotting widget

  QwtPlot is a widget for plotting two-dimensional graphs.
  An unlimited number of data pairs can be displayed as
  curves in different styles and colors. A plot can have
  up to four axes, with each curve attached to an x- and
  a y axis. The scales at the axes are dimensioned automatically
  using an algorithm which can be configured separately for each
  axis. Linear and logarithmic scaling is supported. Markers of
  different styles can be added to the plot.

  Curves and markers are identified by unique keys which are generated
  automatically when a curve or a marker is inserted. These keys are used to
  access the properties of the corresponding curves and markers.

  A QwtPlot widget can have up to four axes which are indexed by
  the constants QwtPlot::yLeft, QwtPlot::yRight, QwtPlot::xTop,
  and QwtPlot::xBottom. Curves, markers, and the grid must be
  attached to an x axis and a y axis (Default: yLeft and xBottom).

  \par Example
  The following example shows (schematically) the most simple
  way to use QwtPlot. By default, only the left and bottom axes are
  visible and their scales are computed automatically.
  \verbatim
#include "../include/qwt_plot.h>

QwtPlot *myPlot;
long curve1, curve2;                    // keys
double x[100], y1[100], y2[100];        // x and y values

myPlot = new QwtPlot("Two Graphs", parent, name);

// add curves
curve1 = myPlot->insertCurve("Graph 1");
curve2 = myPlot->insertCurve("Graph 2");

getSomeValues(x, y1, y2);

// copy the data into the curves
myPlot->setCurveData(curve1, x, y1, 100);
myPlot->setCurveData(curve2, x, y2, 100);

// finally, refresh the plot
myPlot->replot();
\endverbatim
*/

class QWT_EXPORT QwtPlot: public QFrame
{
    friend class QwtPlotItem;
    friend class QwtPlotCanvas;
    friend class QwtPlotPrintFilter;

    Q_OBJECT

    Q_ENUMS( Axis Position )
        
    // plot
    Q_PROPERTY( bool autoReplot READ autoReplot WRITE setAutoReplot )

    // canvas
    Q_PROPERTY( QColor canvasBackground READ canvasBackground 
        WRITE setCanvasBackground )
    Q_PROPERTY( int canvasLineWidth READ canvasLineWidth 
        WRITE setCanvasLineWidth )

    // axes  
    Q_PROPERTY( bool xBottomAxis READ xBottomAxisEnabled 
        WRITE enableXBottomAxis )
    Q_PROPERTY( bool xTopAxis READ xTopAxisEnabled WRITE enableXTopAxis )
    Q_PROPERTY( bool yLeftAxis READ yLeftAxisEnabled WRITE enableYLeftAxis )
    Q_PROPERTY( bool yRightAxis READ yRightAxisEnabled WRITE enableYRightAxis )

    // title
    Q_PROPERTY( QString title READ title WRITE setTitle )
    Q_PROPERTY( QFont titleFont READ titleFont WRITE setTitleFont )

    // grid
    Q_PROPERTY( bool xGrid READ gridXEnabled WRITE enableGridX )
    Q_PROPERTY( bool xMinGrid READ gridXMinEnabled WRITE enableGridX )
    Q_PROPERTY( bool yGrid READ gridYEnabled WRITE enableGridY )
    Q_PROPERTY( bool yMinGrid READ gridYMinEnabled WRITE enableGridY )

    // legend
    Q_PROPERTY( bool autoLegend READ autoLegend WRITE setAutoLegend )
    Q_PROPERTY( QFont legendFont READ legendFont WRITE setLegendFont )
    Q_PROPERTY( Position legendPosition READ legendPosition
        WRITE setLegendPosition)

public:
    //! Axis index
    enum Axis { yLeft, yRight, xBottom, xTop, axisCnt };
    //! Position
    // The enums QwtPlot::Position and Qwt::Position must match,
    // because deprecated functions like
    // QwtPlot::setLegendPos(Qwt::Left)
    // are now implemented in terms of functions like
    // QwtPlot::setLegendPosition(QwtPlot::Left).
    enum Position {
        Left = Qwt::Left,
        Right = Qwt::Right,
        Bottom = Qwt::Bottom,
        Top = Qwt::Top
    };

    QwtPlot(QWidget *p = 0, const char *name = 0);
    QwtPlot(const QString &title, QWidget *p = 0, const char *name = 0);
    virtual ~QwtPlot();

    //! Designer API for enableAxis
    void enableXBottomAxis(bool b) {enableAxis(xBottom,b);}     
    //! Designer API for axisEnabled
    bool xBottomAxisEnabled() const {return axisEnabled(xBottom);};
    //! Designer API for enableAxis
    void enableXTopAxis(bool b) {enableAxis(xTop,b);}       
    //! Designer API for axisEnabled
    bool xTopAxisEnabled() const {return axisEnabled(xTop);};
    //! Designer API for enableAxis
    void enableYRightAxis(bool b) {enableAxis(yRight,b);}       
    //! Designer API for axisEnabled
    bool yRightAxisEnabled() const {return axisEnabled(yRight);};
    //! Designer API for enableAxis
    void enableYLeftAxis(bool b) {enableAxis(yLeft,b);}     
    //! Designer API for axisEnabled
    bool yLeftAxisEnabled() const {return axisEnabled(yLeft);};

    void setAutoReplot(bool tf = TRUE);
    bool autoReplot() const;

    void print(QPaintDevice &p,
        const QwtPlotPrintFilter & = QwtPlotPrintFilter()) const;
    virtual void print(QPainter *, const QRect &rect,
        const QwtPlotPrintFilter & = QwtPlotPrintFilter()) const;


#ifndef QWT_NO_COMPAT
    // Outline

    void enableOutline(bool tf);
    bool outlineEnabled() const;
    void setOutlineStyle(Qwt::Shape os);
    Qwt::Shape outlineStyle() const;
    void setOutlinePen(const QPen &pn);
    const QPen& outlinePen() const;
#endif

    // Layout

    QwtPlotLayout *plotLayout();
    const QwtPlotLayout *plotLayout() const;

    void setMargin(int margin);
    int margin() const;

    // Title

    void setTitle(const QString &t);
    QString title() const;
    void setTitleFont (const QFont &f);
    QFont titleFont() const;

    QLabel *titleLabel();
    const QLabel *titleLabel() const;

    // Canvas

    QwtPlotCanvas *canvas();
    const QwtPlotCanvas *canvas() const;

    void setCanvasBackground (const QColor &c);
    const QColor& canvasBackground() const;

    void setCanvasLineWidth(int w);
    int canvasLineWidth() const;

    QwtPlotCurveIterator curveIterator() const;
    QwtPlotMarkerIterator markerIterator() const;

    QwtDiMap canvasMap(int axis) const;

    double invTransform(int axis, int pos) const;
    int transform(int axis, double value) const;

    // Curves

    long insertCurve(QwtPlotCurve *);
    long insertCurve(const QString &title,
        int xAxis = xBottom, int yAxis = yLeft);

    QwtPlotCurve *curve(long key);
    const QwtPlotCurve *curve(long key) const;

    QwtArray<long> curveKeys() const;

    long closestCurve(int xpos, int ypos, int &dist) const;
    long closestCurve(int xpos, int ypos, int &dist,
        double &xval, double &yval, int &index) const;

    bool setCurveBaseline(long key, double ref);
    double curveBaseline(long key) const;

    bool setCurveRawData(long key, const double *x, const double *y, int size);
    bool setCurveData(long key, const double *x, const double *y, int size);
    bool setCurveData(long key, 
        const QwtArray<double> &x, const QwtArray<double> &y);
    bool setCurveData(long key, const QwtArray<QwtDoublePoint> &data);
    bool setCurveData(long key, const QwtData &data);

    bool setCurveOptions(long key, int t);
    int curveOptions(long key) const;

    bool setCurvePen(long key, const QPen &pen);
    QPen curvePen(long key) const;

    bool setCurveBrush(long key, const QBrush &brush);
    QBrush curveBrush(long key) const;

    bool setCurveSplineSize(long key, int s);
    int curveSplineSize(long key) const;

    bool setCurveStyle(long key, int s, int options = 0);
    int curveStyle(long key) const;

    bool setCurveSymbol(long key, const QwtSymbol &s);
    QwtSymbol curveSymbol(long key) const;

    bool setCurveTitle(long key, const QString &s);
    QString curveTitle(long key) const;

    bool setCurveXAxis(long key, int axis);
    int curveXAxis(long key) const;
    bool setCurveYAxis(long key, int axis);
    int curveYAxis(long key) const;

    void drawCurve(long key, int from = 0, int to = -1);

    //  Grid

    void enableGridX(bool tf = TRUE);
    void enableGridXMin(bool tf = TRUE);
    void enableGridY(bool tf = TRUE);
    void enableGridYMin(bool tf = TRUE);

    bool gridXEnabled() const;
    bool gridXMinEnabled() const;
    bool gridYEnabled() const;
    bool gridYMinEnabled() const;

    void setGridXAxis(int axis);
    int gridXAxis() const;
    void setGridYAxis(int axis);
    int gridYAxis() const;

    void setGridPen(const QPen &p);
    void setGridMajPen(const QPen &p);
    const QPen& gridMajPen() const;
    void setGridMinPen(const QPen &p);
    const QPen& gridMinPen() const;

    QwtPlotGrid &grid();
    const QwtPlotGrid &grid() const;

    // Axes

    void setAxisAutoScale(int axis);
    bool axisAutoScale(int axis) const;

    void enableAxis(int axis, bool tf = TRUE);
    bool axisEnabled(int axis) const;

    void changeAxisOptions(int axis, int opt, bool value);
    void setAxisOptions(int axis, int opt);
    int axisOptions(int axis) const;

    void setAxisFont(int axis, const QFont &f);
    QFont axisFont(int axis) const;

    void setAxisMargins(int axis, double mlo, double mhi);
    bool axisMargins(int axis, double &mlo, double &mhi) const;

    void setAxisScale(int axis, double min, double max, double step = 0);
    void setAxisScaleDraw(int axis, QwtScaleDraw *);
    const QwtScaleDiv *axisScale(int axis) const;
    const QwtScaleDraw *axisScaleDraw(int axis) const;
    const QwtScale *axis(int axis) const;

    void setAxisLabelFormat(int axis, char f, int prec, int fieldwidth = 0);
    void axisLabelFormat(int axis, char &f, int &prec, int &fieldwidth) const;

    void setAxisLabelAlignment(int axis, int alignment);
    void setAxisLabelRotation(int axis, double rotation);

    void setAxisTitle(int axis, const QString &t);
    QString axisTitle(int axis) const;
    void setAxisTitleFont(int axis, const QFont &f);
    QFont axisTitleFont(int axis) const;
    void setAxisTitleAlignment(int axis, int align);
    int axisTitleAlignment(int axis) const;

    void setAxisMaxMinor(int axis, int maxMinor);
    int axisMaxMajor(int axis) const;
    void setAxisMaxMajor(int axis, int maxMajor);
    int axisMaxMinor(int axis) const;

    void setAxisReference(int axis, double value);
    double axisReference(int axis) const;

    //  Markers

    long insertMarker(QwtPlotMarker *);
    long insertMarker(const QString &label = QString::null,
                int xAxis = xBottom, int yAxis = yLeft);
    long insertLineMarker(const QString &label, int axis);

    QwtPlotMarker *marker(long key);
    const QwtPlotMarker *marker(long key) const;

    long closestMarker(int xpos, int ypos, int &dist) const;
    QwtArray<long> markerKeys() const;

    bool setMarkerXAxis(long key, int axis);
    int markerXAxis(long key) const;
    bool setMarkerYAxis(long key, int axis);
    int markerYAxis(long key) const;

    bool setMarkerPos(long key, double xval, double yVal);
    bool setMarkerXPos(long key, double val);
    bool setMarkerYPos(long key, double val);
    void markerPos(long key, double &mx, double &my) const;

    bool setMarkerFont(long key, const QFont &f);
    QFont markerFont(long key) const;
    bool setMarkerPen(long key, const QPen &p);

    bool setMarkerLabel(long key, const QString &text,
        const QFont &font = QFont(), const QColor &color = QColor(),
        const QPen &pen = QPen(Qt::NoPen),
        const QBrush &brush = QBrush(Qt::NoBrush));

    bool setMarkerLabelText(long key, const QString &text);
    const QString markerLabel(long key) const;
    bool setMarkerLabelAlign(long key, int align);
    int markerLabelAlign(long key) const;
    bool setMarkerLabelPen(long key, const QPen &p);
    QPen markerLabelPen(long key) const;

    bool setMarkerLinePen(long key, const QPen &p);
    QPen markerLinePen(long key) const;
    bool setMarkerLineStyle(long key, QwtMarker::LineStyle st);
    QwtMarker::LineStyle markerLineStyle(long key) const;

    bool setMarkerSymbol(long key, const QwtSymbol &s);
    QwtSymbol markerSymbol(long key) const;

    // Legend 

    void setAutoLegend(bool enabled);
    bool autoLegend() const;

    void enableLegend(bool tf, long curveKey = -1);
    bool legendEnabled(long curveKey) const;

    void setLegendPosition(Position pos, double ratio);

    void setLegendPosition(Position pos);
    Position legendPosition() const;

#ifndef QWT_NO_COMPAT
    void setLegendPos(int pos, double ratio = 0.0);
    int legendPos() const;
#endif

    void setLegendFont(const QFont &f);
    const QFont legendFont() const;

    void setLegendFrameStyle(int st);
    int legendFrameStyle() const;

    QwtLegend *legend();
    const QwtLegend *legend() const;

    void setLegendDisplayPolicy(
        QwtLegend::LegendDisplayPolicy, int mode = -1);

    // Misc
    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;

    virtual bool event(QEvent *);

public slots:
    void clear();

    bool removeCurve(long key);
    void removeCurves();

    bool removeMarker(long key);
    void removeMarkers();

    virtual void replot();

protected slots:
    virtual void lgdClicked();

#ifndef QWT_NO_COMPAT
signals:
    /*!
      A signal which is emitted when the mouse is pressed in the
      plot canvas. 

      \warning Canvas signals are obsolete: use QwtPlotPicker or
      QwtPlotZoomer.

      The coordinates are pixel values referring
      to the plot canvas. They can be translated using the canvasMaps.
      \param e Mouse event object
      \sa QwtPlot::canvasMap()
     */
    void plotMousePressed(const QMouseEvent &e);

    /*!
      A signal which is emitted when a mouse button has been
      released in the plot canvas.

      \warning Canvas signals are obsolete: use QwtPlotPicker or
      QwtPlotZoomer.

      The coordinates are pixel values referring
      to the plot canvas. They can be translated using the canvasMaps.
      \param e Mouse event object, event coordinates referring
               to the plot canvas
      \sa QwtPlot::canvasMap()
     */
    void plotMouseReleased(const QMouseEvent &e);

    /*!
      A signal which is emitted when the mouse is moved in the
      plot canvas.

      \warning Canvas signals are obsolete: use QwtPlotPicker or
      QwtPlotZoomer.

      The coordinates are pixel values referring
      to the plot canvas. They can be translated using the canvasMaps.
      \param e Mouse event object
      \sa QwtPlot::canvasMap()
     */
    void plotMouseMoved(const QMouseEvent &e);
#else
private:
    // needed for mocs #ifdef ignorance
    void plotMousePressed(const QMouseEvent &);
    void plotMouseReleased(const QMouseEvent &);
    void plotMouseMoved(const QMouseEvent &);
#endif // !QWT_NO_COMPAT

signals:
    /*!
      A signal which is emitted when legend()->isReadOnly() == FALSE and
      the user has clicked on a legend item,  

      \param key Key of the curve corresponding to the
                 selected legend item

      \note To enable clicks legend()->setReadOnly(FALSE) has 
            to be set before the items have been inserted.

      \sa QwtLegend::setReadOnly, QwtLegend::isReadOnly
     */
    void legendClicked(long key);

protected:
    void autoRefresh();
    static bool axisValid(int axis);

    virtual void drawCanvas(QPainter *);
    virtual void drawCanvasItems(QPainter *, const QRect &,
        const QwtArray<QwtDiMap> &, const QwtPlotPrintFilter &) const;
    virtual void drawContents( QPainter* p );

    virtual void updateTabOrder();

    void updateAxes();
    void updateLayout();

    virtual void resizeEvent(QResizeEvent *e);

    virtual void insertLegendItem(long curveKey);
    virtual void updateLegendItem(long curveKey);
    virtual void printLegendItem(QPainter *, 
        const QWidget *, const QRect &) const;

    void updateLegendItem(const QwtPlotCurve *, QwtLegendItem *);

    virtual void printTitle(QPainter *, const QRect &) const;
    virtual void printScale(QPainter *, int axis, int startDist, int endDist,
        int baseDist, const QRect &) const;
    virtual void printCanvas(QPainter *, const QRect &,
        const QwtArray<QwtDiMap> &, const QwtPlotPrintFilter &) const;
    virtual void printLegend(QPainter *, const QRect &) const;

private:
    void initAxes();
    long newCurveKey();
    long newMarkerKey();
    void initPlot(const QString &title = QString::null);

    QLabel *d_lblTitle;
    QwtPlotCanvas *d_canvas;
    QwtLegend *d_legend;
    QwtPlotLayout *d_layout;

    bool d_axisEnabled[axisCnt];
    QwtScale *d_scale[axisCnt];
    QwtAutoScale d_as[axisCnt];

    QwtCurveDict *d_curves;
    QwtMarkerDict *d_markers;
    QwtPlotGrid *d_grid;

    bool d_autoReplot;
    bool d_autoLegend;

};

#endif

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
