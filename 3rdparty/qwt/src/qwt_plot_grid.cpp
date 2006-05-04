/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include "qwt_plot.h"
#include "qwt_math.h"

//! Return the canvas grid
QwtPlotGrid &QwtPlot::grid()
{
    return *d_grid;
}

//! Return the canvas grid
const QwtPlotGrid &QwtPlot::grid() const
{
    return *d_grid;
}

/*!
  Enable or disable vertical gridlines.
  \param tf Enable (TRUE) or disable (FALSE).
            Defaults to TRUE. 
*/
void QwtPlot::enableGridX(bool tf)
{
    d_grid->enableX(tf);
}

/*!
  \brief Enable or disable horizontal gridlines
  \param tf Enable (TRUE) or disable (FALSE).
            Defaults to TRUE.
*/
void QwtPlot::enableGridY(bool tf)
{
    d_grid->enableY(tf);
}

/*!
  \brief Enable or disable vertical gridlines for the minor scale marks
  \param tf enable (TRUE) or disable (FALSE). Defaults to TRUE.
*/
void QwtPlot::enableGridXMin(bool tf)
{
    d_grid->enableXMin(tf);
}

/*!
  \brief Enable or disable horizontal gridlines for the minor scale marks.
  \param tf enable (TRUE) or disable (FALSE). Defaults to TRUE.
*/
void QwtPlot::enableGridYMin(bool tf)
{
    d_grid->enableYMin(tf);
}

/*!
  \return TRUE if the X major grid is enabled (visible), FALSE otherwise.
*/
bool QwtPlot::gridXEnabled() const
{
    return d_grid->xEnabled();
}

/*!
  \return TRUE if the X minor grid is enabled (visible), FALSE otherwise.
*/
bool QwtPlot::gridXMinEnabled() const
{
    return d_grid->xMinEnabled();
}

/*!
  \return TRUE if the Y major grid is enabled (visible), FALSE otherwise.
*/
bool QwtPlot::gridYEnabled() const
{
    return d_grid->yEnabled();
}

/*!
  \return TRUE if the X minor grid is enabled (visible), FALSE otherwise.
*/
bool QwtPlot::gridYMinEnabled() const
{
    return d_grid->yMinEnabled();
}

/*!
  \brief Attach the grid to an x axis
  \param axis x axis to be attached
*/
void QwtPlot::setGridXAxis(int axis)
{
    if ((axis==xBottom)||(axis==xTop))
    {
        d_grid->setXAxis(axis);
        d_grid->setXDiv(d_as[axis].scaleDiv());
    }
}

/*!
    \return x-axis where the grid is attached to.
    \sa QwtPlot::setGridXAxis
*/
int QwtPlot::gridXAxis() const
{
    return d_grid->xAxis();
}

/*!
  \brief Attach the grid to an y axis
  \param axis y axis to be attached
*/
void QwtPlot::setGridYAxis(int axis)
{
    if ((axis==yLeft) || (axis == yRight))
    {
        d_grid->setYAxis(axis);
        d_grid->setYDiv(d_as[axis].scaleDiv());
    }
}

/*!
    \return y-axis where the grid is attached to.
    \sa QwtPlot::setGridYAxis
*/
int QwtPlot::gridYAxis() const
{
    return d_grid->yAxis();
}

/*!
  \brief Change the grid's pens for major and minor gridlines
  \param p new pen
*/
void QwtPlot::setGridPen(const QPen &p)
{
    d_grid->setPen(p);
}

/*!
  \brief Change the pen for the minor gridlines
  \param p new pen
*/
void QwtPlot::setGridMinPen(const QPen &p)
{
    d_grid->setMinPen(p);
}

/*!
    \return pen of the minor grid
    \sa QwtPlot::setGridMinPen, QwtPlot::gridMajPen
*/
const QPen& QwtPlot::gridMinPen() const
{
    return d_grid->minPen();
}

/*!
  \brief Change the pen for the major gridlines
  \param p new pen
*/
void QwtPlot::setGridMajPen(const QPen &p)
{
    d_grid->setMajPen(p);
}

/*!
    \return pen of the major grid
    \sa QwtPlot::setGridMajPen, QwtPlot::gridMinPen
*/
const QPen& QwtPlot::gridMajPen() const
{
    return d_grid->majPen();
}

// Local Variables:
// mode: C++
// c-file-style: "stroustrup"
// indent-tabs-mode: nil
// End:
