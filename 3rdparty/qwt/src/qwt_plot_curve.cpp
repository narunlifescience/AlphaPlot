/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#include "qwt_plot.h"
#include "qwt_plot_dict.h"
#include "qwt_math.h"
#include "qwt_legend.h"

//! Return an iterator for the plot curves
QwtPlotCurveIterator QwtPlot::curveIterator() const
{
    return QwtPlotCurveIterator(*d_curves);
}

/*!
  Find the curve which is closest to a
  specified point in the plotting area.
  \param xpos
  \param ypos position in the plotting region
  \retval dist distance in points between (xpos, ypos) and the
               closest data point
  \return Key of the closest curve or 0 if no curve was found.
*/
long QwtPlot::closestCurve(int xpos, int ypos, int &dist) const
{
    double x,y;
    int index;
    return closestCurve(xpos, ypos, dist, x,y, index);
}

/*!
  Find the curve which is closest to a point in the plotting area.
  
  Determines the position and index of the closest data point.
  \param xpos
  \param ypos coordinates of a point in the plotting region
  \retval xval 
  \retval yval values of the closest point in the curve's data array
  \retval dist -- distance in points between (xpos, ypos) and the
                  closest data point
  \retval index -- index of the closest point in the curve's data array
  \return Key of the closest curve or 0 if no curve was found.
*/
long QwtPlot::closestCurve(int xpos, int ypos, int &dist, double &xval,
                           double &yval, int &index) const
{
    QwtDiMap map[axisCnt];
    for ( int axis = 0; axis < axisCnt; axis++ )
        map[axis] = canvasMap(axis);

    long rv = 0;
    double dmin = 1.0e10;

    QwtPlotCurveIterator itc = curveIterator();
    for (QwtPlotCurve *c = itc.toFirst(); c != 0; c = ++itc )
    {
        for (int i=0; i<c->dataSize(); i++)
        {
            double cx = map[c->xAxis()].xTransform(c->x(i)) - double(xpos);
            double cy = map[c->yAxis()].xTransform(c->y(i)) - double(ypos);

            double f = qwtSqr(cx) + qwtSqr(cy);
            if (f < dmin)
            {
                dmin = f;
                rv = itc.currentKey();
                xval = c->x(i);
                yval = c->y(i);
                index = i;
            }
        }
    }

    dist = int(sqrt(dmin));
    return rv;
}



/*!
  \return the style of the curve indexed by key
  \param key Key of the curve
  \sa setCurveStyle()
*/
int QwtPlot::curveStyle(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
    return c ? c->style() : 0;
}

/*!
  \brief the symbol of the curve indexed by key
  \param key Key of the curve
  \return The symbol of the specified curve. If the key is invalid,
          a symbol of type 'NoSymbol'.
*/
QwtSymbol QwtPlot::curveSymbol(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
    return c ? c->symbol() : QwtSymbol();
}

/*!
  \return the brush of the curve indexed by key
  \param key Key of the curve
*/
QPen QwtPlot::curvePen(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
    return c ? c->pen() : QPen();
}

/*!
  \return the pen of the curve indexed by key
  \param key Key of the curve
  \sa QwtPlot::setCurveBrush(), QwtCurve::setBrush()
*/
QBrush QwtPlot::curveBrush(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
    return c ? c->brush() : QBrush();
}
/*!
  \return the drawing options of the curve indexed by key
  \param key Key of the curve
*/
int QwtPlot::curveOptions(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
    return c ? c->options() : 0;
}

/*!
  \return the spline size of the curve indexed by key
  \param key Key of the curve
*/
int QwtPlot::curveSplineSize(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
    return c ? c->splineSize() : 0;
}

/*!
  \return the title of the curve indexed by key
  \param key Key of the curve
*/
QString QwtPlot::curveTitle(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
    return c ? c->title() : QString::null;
}

/*!
  \return an array containing the keys of all curves
*/
QwtArray<long> QwtPlot::curveKeys() const
{
    QwtArray<long> keys(d_curves->count());

    int i = 0;

    QwtPlotCurveIterator itc = curveIterator();
    for (const QwtPlotCurve *c = itc.toFirst(); c != 0; c = ++itc, i++ )
        keys[i] = itc.currentKey();

    return keys;
}

/*!
  \brief Return the index of the x axis to which a curve is mapped
  \param key Key of the curve
  \return x axis of the curve or -1 if the key is invalid.
*/
int QwtPlot::curveXAxis(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
    return c ? c->xAxis() : -1;
}


/*!
  \brief the index of the y axis to which a curve is mapped
  \param key Key of the curve
  \return y axis of the curve or -1 if the key is invalid.
*/
int QwtPlot::curveYAxis(long key) const
{
    QwtPlotCurve *c = d_curves->find(key);
    return c ? c->yAxis() : -1;
}


/*!
  \brief Generate a unique key for a new curve
  \return new unique key or 0 if no key could be found.
*/
long QwtPlot::newCurveKey()
{
    long newkey = d_curves->count() + 1;

    if (newkey > 1)                     // size > 0: check if key exists
    {
        if (d_curves->find(newkey))     // key size+1 exists => there must be a
                                        // free key <= size
        {
            // find the first available key <= size
            newkey = 1;
            while (newkey <= long(d_curves->count()))
            {
                if (d_curves->find(newkey))
                   newkey++;
                else
                   break;
            }

            // This can't happen. Just paranoia.
            if (newkey > long(d_curves->count()))
            {
                while (!d_curves->find(newkey))
                {
                    newkey++;
                    if (newkey > 10000) // prevent infinite loop
                    {
                        newkey = 0;
                        break;
                    }
                }
            }
        }
    }
    return newkey;
}

/*!
  \brief Insert a curve
  \param curve Curve
  \return The key of the new curve or 0 if no new key could be found
          or if no new curve could be allocated.
*/

long QwtPlot::insertCurve(QwtPlotCurve *curve)
{
    if (curve == 0)
        return 0;

    long key = newCurveKey();
    if (key == 0)
        return 0;

    curve->reparent(this);

    d_curves->insert(key, curve);
    if (d_autoLegend)
    {
        insertLegendItem(key);
        updateLayout();
    }

    return key;
}

/*!
  \brief Insert a new curve and return a unique key
  \param title title of the new curve
  \param xAxis x axis to be attached. Defaults to xBottom.
  \param yAxis y axis to be attached. Defaults to yLeft.
  \return The key of the new curve or 0 if no new key could be found
          or if no new curve could be allocated.
*/
long QwtPlot::insertCurve(const QString &title, int xAxis, int yAxis)
{
    QwtPlotCurve *curve = new QwtPlotCurve(this);
    if (!curve)
        return 0;

    curve->setAxis(xAxis, yAxis);
    curve->setTitle(title);

    long key = insertCurve(curve);
    if ( key == 0 )
        delete curve;

    return key;
}

/*!
  \brief Find and return an existing curve.
  \param key Key of the curve
  \return The curve for the given key or 0 if key is not valid.
*/
QwtPlotCurve *QwtPlot::curve(long key)
{
    return d_curves->find(key);
}

/*!
  \brief Find and return an existing curve.
  \param key Key of the curve
  \return The curve for the given key or 0 if key is not valid.
*/
const QwtPlotCurve *QwtPlot::curve(long key) const
{
    return d_curves->find(key);
}


/*!
  \brief remove the curve indexed by key
  \param key Key of the curve
*/
bool QwtPlot::removeCurve(long key)
{
    bool ok = d_curves->remove(key);
    if ( !ok )
        return FALSE;

    QWidget *item = d_legend->findItem(key);
    if ( item )
    {
        delete item;
        updateLayout();
    }
    
    autoRefresh();
    return TRUE;
}

/*!
  \brief Assign a pen to a curve indexed by key
  \param key Key of the curve
  \param pen new pen
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurvePen(long key, const QPen &pen)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;
    
    c->setPen(pen);
    updateLegendItem(key);

    return TRUE;
}

/*!
  \brief Assign a brush to a curve indexed by key
         The brush will be used to fill the area between the 
         curve and the baseline. 
  \param key Key of the curve
  \param brush new brush
  \return \c TRUE if the curve exists
  \sa QwtCurve::setBrush for further details.
  \sa QwtPlot::brush(), QwtPlot::setCurveBaseline
*/
bool QwtPlot::setCurveBrush(long key, const QBrush &brush)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;
    
    c->setBrush(brush);
    updateLegendItem(key);

    return TRUE;
}

/*!
  \brief Assign a symbol to the curve indexed by key
  \param key key of the curve
  \param symbol new symbol
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurveSymbol(long key, const QwtSymbol &symbol)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setSymbol(symbol);
    updateLegendItem(key);

    return TRUE;
}

/*!
  \brief  Initialize the curve data by pointing to memory blocks which are not
  managed by QwtPlot.

  \param key Key of the curve
  \param xData pointer to x data
  \param yData pointer to y data
  \param size size of x and y
  \return \c TRUE if the curve exists

  \warning setRawData is provided for efficiency.  The programmer should not
  delete the data during the lifetime of the underlying QwtCPointerData class.

  \sa QwtPlot::setCurveData(), QwtCurve::setRawData()
*/
bool QwtPlot::setCurveRawData(long key, 
    const double *xData, const double *yData, int size)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setRawData(xData, yData, size);
    return TRUE;
}

/*!
  \brief Assign a title to the curve indexed by key.
  \param key key of the curve
  \param title new title
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurveTitle(long key, const QString &title)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setTitle(title);
    updateLegendItem(key);

    return TRUE;
}

/*!
  \brief Set curve data by copying x- and y-values from specified blocks.
  Contrary to \b QwtPlot::setCurveRawData, this function makes a 'deep copy' of
  the data.

  \param key curve key
  \param xData pointer to x values
  \param yData pointer to y values
  \param size size of xData and yData
  \return \c TRUE if the curve exists

  \sa setCurveRawData(), QwtCurve::setData
*/
bool QwtPlot::setCurveData(long key, 
    const double *xData, const double *yData, int size)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setData(xData, yData, size);
    return TRUE;
}
    
/*!
  \brief Initialize curve data with x- and y-arrays (data is explicitly shared)

  \param key curve key
  \param xData array with x-values
  \param yData array with y-values
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurveData(long key, 
    const QwtArray<double> &xData, const QwtArray<double> &yData)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setData(xData, yData);
    return TRUE;
}
    
/*!
  \brief Initialize curve data with a array of points (explicitly shared)

  \param key curve key
  \param data Data
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurveData(long key, const QwtArray<QwtDoublePoint> &data)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setData(data);
    return TRUE;
}
    
/*!
  \brief Initialize curve data with any QwtData object

  \param key curve key
  \param data Data
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurveData(long key, const QwtData &data)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setData(data);
    return TRUE;
}
    
/*!
  \brief Change a curve's style
  \param key Key of the curve
  \param s display style of the curve
  \param options style options
  \return \c TRUE if the curve exists
  \sa QwtCurve::setStyle() for a detailed description of valid styles.
*/
bool QwtPlot::setCurveStyle(long key, int s, int options)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setStyle(s, options);
    updateLegendItem(key);

    return TRUE;
}

/*!
  \brief Set the style options of a curve indexed by key
  \param key The curve's key
  \param opt curve options
  \return \c TRUE if the specified curve exists.
  \sa QwtCurve::setOptions for a detailed description of valid options.
*/
bool QwtPlot::setCurveOptions(long key, int opt)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setOptions(opt);
    return TRUE;
}

/*!
  \brief Set the number of interpolated points of a curve indexed by key
  \param key key of the curve
  \param s size of the spline
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurveSplineSize(long key, int s)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setSplineSize(s);
    return TRUE;
}


/*!
  \brief Attach a curve to an x axis
  \param key key of the curve
  \param axis x axis to be attached
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurveXAxis(long key, int axis)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setXAxis(axis);
    return TRUE;
}

/*!
  \brief Attach a curve to an y axis
  \param key key of the curve
  \param axis y axis to be attached
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurveYAxis(long key, int axis)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setYAxis(axis);
    return TRUE;
}


/*!
  \brief Set the baseline for a specified curve

  The baseline is needed for the curve style QwtCurve::Sticks,
  \param key curve key
  \param ref baseline offset from zero
  \sa QwtCurve::setBaseline
  \return \c TRUE if the curve exists
*/
bool QwtPlot::setCurveBaseline(long key, double ref)
{
    QwtPlotCurve *c = d_curves->find(key);
    if ( !c )
        return FALSE;

    c->setBaseline(ref);
    return TRUE;
}

/*!
  \brief Return the baseline offset for a specified curve
  \param key curve key
  \return Baseline offset of the specified curve,
          or 0 if the curve doesn't exist
  \sa setCurveBaseline()
*/
double QwtPlot::curveBaseline(long key) const
{
    double rv = 0.0;
    QwtPlotCurve *c;
    if ((c = d_curves->find(key)))
        rv = c->baseline();
    return rv;
}

/*!
  \brief Insert a QwtLegendItem for a specified curve

  In case of legend()->isReadOnly the item will be a QwtLegendLabel,
  otherwise a QwtLegendButton.

  \param curveKey curve key
  \sa QwtLegendButton, QwtLegendItem, QwtLegend, updateLegendItem(), printLegendItem()
*/
void QwtPlot::insertLegendItem(long curveKey)
{
    if ( d_legend->isReadOnly() )
    {
        QwtLegendLabel *label = 
            new QwtLegendLabel(d_legend->contentsWidget());
        d_legend->insertItem(label, curveKey);
    }
    else
    {
        QwtLegendButton *button = 
            new QwtLegendButton(d_legend->contentsWidget());
        connect(button, SIGNAL(clicked()), SLOT(lgdClicked()));

        d_legend->insertItem(button, curveKey);
    }

    updateLegendItem(curveKey);
}

/*!
  Update the legend item of a specified curve
  \param curveKey curve key
  \sa QwtLegendButton, QwtLegend, insertLegendItem(), printLegendItem()
*/
void QwtPlot::updateLegendItem(long curveKey)
{
    const QwtPlotCurve *curve = d_curves->find(curveKey);
    if ( !curve )
        return;

    QWidget *item = d_legend->findItem(curveKey);
    if (item && item->inherits("QwtLegendButton")) 
    {
        QwtLegendButton *button = (QwtLegendButton *)item;

        const bool doUpdate = button->isUpdatesEnabled();
        button->setUpdatesEnabled(FALSE);

        updateLegendItem(curve, button);

        button->setUpdatesEnabled(doUpdate);
        button->update();
    }
    if (item && item->inherits("QwtLegendLabel")) 
    {
        QwtLegendLabel *label = (QwtLegendLabel *)item;

        const bool doUpdate = label->isUpdatesEnabled();
        label->setUpdatesEnabled(FALSE);

        updateLegendItem(curve, label);

        label->setUpdatesEnabled(doUpdate);
        label->update();
    }
}

/*!
  Update a liegen item for a specified curve
  \param curve Curve
  \param item Legend item
  \sa QwtLegendButton, QwtLegend, insertLegendItem(), printLegendItem()
*/
void QwtPlot::updateLegendItem(
    const QwtPlotCurve *curve, QwtLegendItem *item)
{
    if ( !curve || !item )
        return;

    int policy = d_legend->displayPolicy();

    if (policy == QwtLegend::Fixed) 
    {
        int mode = d_legend->identifierMode();

        if (mode & QwtLegendButton::ShowLine) 
            item->setCurvePen(curve->pen());

        if (mode & QwtLegendButton::ShowSymbol) 
            item->setSymbol(curve->symbol());

        if (mode & QwtLegendButton::ShowText) 
            item->setTitle(curve->title());
        else 
            item->setTitle(QString::null);

        item->setIdentifierMode(mode);
    } 
    else if (policy == QwtLegend::Auto) 
    {
        int mode = 0;

        if (QwtCurve::NoCurve != curve->style()) 
        {
            item->setCurvePen(curve->pen());
            mode |= QwtLegendButton::ShowLine;
        }
        if (QwtSymbol::None != curve->symbol().style()) 
        {
            item->setSymbol(curve->symbol());
            mode |= QwtLegendButton::ShowSymbol;
        }
        if ( !curve->title().isEmpty() )
        { 
            item->setTitle(curve->title());
            mode |= QwtLegendButton::ShowText;
        } 
        else 
        {
            item->setTitle(QString::null);
        }
        item->setIdentifierMode(mode);
    }
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
