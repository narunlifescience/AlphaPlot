/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#include <qlabel.h>
#include "qwt_rect.h"
#include "qwt_text.h"
#include "qwt_plot_canvas.h"
#include "qwt_scale.h"
#include "qwt_legend.h"
#include "qwt_plot_layout.h"

class QwtPlotLayoutData
{
    friend class QwtPlotLayout;

protected:
    QwtPlotLayoutData();
    ~QwtPlotLayoutData();

    void init(const QwtPlot *, const QRect &rect);

    struct t_legendData
    {
        int frameWidth;
        int vScrollBarWidth;
        int hScrollBarHeight;
        QSize hint;
    } legend;
    
    struct t_titleData
    {
        const QwtText *text;
        int frameWidth;
    } title;

    struct t_scaleData
    {
        bool isEnabled;
        const QwtScale *scale;
        QFont scaleFont;
        int start;
        int end;
        int baseLineOffset;
        int tickOffset; 
        int dimWithoutTitle;
    } scale[QwtPlot::axisCnt];

    struct t_canvasData
    {
        int frameWidth;
    } canvas;
};

/*!
  \brief Constructor
 */

QwtPlotLayout::QwtPlotLayout():
    d_margin(0),
    d_spacing(5),
    d_alignCanvasToScales(FALSE)
{
    setLegendPosition(QwtPlot::Bottom);
    setCanvasMargin(4);
    d_layoutData = new QwtPlotLayoutData;

    invalidate();
}

//! Destructor
QwtPlotLayout::~QwtPlotLayout()
{
    delete d_layoutData;
}

/*!
  Change the margin of the plot. The margin is the space
  around all components.
 
  \param margin new margin
  \sa QwtPlotLayout::margin(), QwtPlotLayout::setSpacing(),
      QwtPlot::setMargin()
*/

void QwtPlotLayout::setMargin(int margin)
{
    if ( margin < 0 )
        margin = 0;
    d_margin = margin;
}

/*!
    \return margin
    \sa QwtPlotLayout::setMargin(), QwtPlotLayout::spacing(), 
        QwtPlot::margin()
*/

int QwtPlotLayout::margin() const
{
    return d_margin;
}

/*!
  Change a margin of the canvas. The margin is the space
  above/below the scale ticks. A negative margin will
  be set to -1, excluding the borders of the scales.
 
  \param margin New margin
  \param axis One of QwtPlot::Axis. Specifies where the position of the margin. 
              -1 means margin at all borders.
  \sa QwtPlotLayout::canvasMargin() 

  \warning The canvas will have no effect when alignCanvasToScales is TRUE
*/

void QwtPlotLayout::setCanvasMargin(int margin, int axis)
{
    if ( margin < -1 )
        margin = -1;

    if ( axis == -1 )
    {
        for (axis = 0; axis < QwtPlot::axisCnt; axis++)
            d_canvasMargin[axis] = margin;
    }
    else if ( axis >= 0 || axis < QwtPlot::axisCnt )
        d_canvasMargin[axis] = margin;
}

/*!
    \return Margin around the scale tick borders
    \sa QwtPlotLayout::setCanvasMargin()
*/

int QwtPlotLayout::canvasMargin(int axis) const
{
    if ( axis < 0 || axis >= QwtPlot::axisCnt )
        return 0;

    return d_canvasMargin[axis];
}


/*!
  Change the align-canvas-to-axis-scales setting. The canvas may:
  - extend beyond the axis scale ends to maximize its size,
  - align with the axis scale ends to control its size.

  \param alignCanvasToScales New align-canvas-to-axis-scales setting

  \sa QwtPlotLayout::alignCanvasToTicks, QwtPlotLayout::setCanvasMargin() 
  \note In this context the term 'scale' means the backbone of a scale.
  \warning In case of alignCanvasToScales == TRUE canvasMargin will have 
           no effect
*/

void QwtPlotLayout::setAlignCanvasToScales(bool alignCanvasToScales)
{
    d_alignCanvasToScales = alignCanvasToScales;
}

/*!
  Return the align-canvas-to-axis-scales setting. The canvas may:
  - extend beyond the axis scale ends to maximize its size
  - align with the axis scale ends to control its size.

  \return align-canvas-to-axis-scales setting
  \sa QwtPlotLayout::setAlignCanvasToScales, QwtPlotLayout::setCanvasMargin() 
  \note In this context the term 'scale' means the backbone of a scale.
*/

bool QwtPlotLayout::alignCanvasToScales() const
{
    return d_alignCanvasToScales;
}

/*!
  Change the spacing of the plot. The spacing is the distance
  between the plot components.
 
  \param spacing new spacing
  \sa QwtPlotLayout::setMargin(), QwtPlotLayout::spacing() 
*/

void QwtPlotLayout::setSpacing(int spacing)
{
    d_spacing = QMAX(0, spacing);
}

/*!
  \return spacing
  \sa QwtPlotLayout::margin(), QwtPlotLayout::setSpacing() 
*/
int QwtPlotLayout::spacing() const
{
    return d_spacing;
}

/*!
  \brief Specify the position of the legend
  \param pos The legend's position. Valid values are \c QwtPlot::Left,
           \c QwtPlot::Right, \c QwtPlot::Top, \c QwtPlot::Bottom.
  \param ratio Ratio between legend and the bounding rect 
               of title, canvas and axes. The legend will be shrinked
               if it would need more space than the given ratio.
               The ratio is limited to ]0.0 .. 1.0]. In case of <= 0.0
               it will be reset to the default ratio.
               The default vertical/horizontal ratio is 0.33/0.5. 
               
  \sa QwtPlot::setLegendPosition()
*/

void QwtPlotLayout::setLegendPosition(QwtPlot::Position pos, double ratio)
{
    if ( ratio > 1.0 )
        ratio = 1.0;

    switch(pos)
    {
        case QwtPlot::Top:
        case QwtPlot::Bottom:
            if ( ratio <= 0.0 )
                ratio = 0.33;
            d_legendRatio = ratio;
            d_legendPos = pos;
            break;
        case QwtPlot::Left:
        case QwtPlot::Right:
            if ( ratio <= 0.0 )
                ratio = 0.5;
            d_legendRatio = ratio;
            d_legendPos = pos;
            break;
        default:
            break;
    }
}

/*!
  \brief Specify the position of the legend
  \param pos The legend's position. Valid values are \c QwtPlot::Left,
           \c QwtPlot::Right, \c QwtPlot::Top, \c QwtPlot::Bottom.
               
  \sa QwtPlot::setLegendPosition()
*/

void QwtPlotLayout::setLegendPosition(QwtPlot::Position pos)
{
    setLegendPosition(pos, 0.0);
}

/*!
  \return Position of the legend
  \sa QwtPlotLayout::setLegendPosition(), QwtPlot::setLegendPosition(),
      QwtPlot::legendPosition()
*/

QwtPlot::Position QwtPlotLayout::legendPosition() const
{
    return d_legendPos;
}

#ifndef QWT_NO_COMPAT

/*!
  \brief Specify the position of the legend
  \param pos The legend's position. Valid values are \c Qwt::Left,
           \c Qwt::Right, \c Qwt::Top, \c QwtBottom.
  \param ratio Ratio between legend and the bounding rect 
               of title, canvas and axes. The legend will be shrinked
               if it would need more space than the given ratio.
               The ratio is limited to ]0.0 .. 1.0]. In case of <= 0.0
               it will be reset to the default ratio.
               The default vertical/horizontal ratio is 0.33/0.5. 
               
  \sa QwtPlot::setLegendPosition()
  \warning This function is deprecated. Use QwtPlotLayout::setLegendPosition().
*/

void QwtPlotLayout::setLegendPos(int pos, double ratio)
{
    setLegendPosition(QwtPlot::Position(pos), ratio);
}

/*!
  \return Position of the legend
  \sa QwtPlotLayout::setLegendPosition(), QwtPlot::setLegendPosition(),
      QwtPlot::legendPosition()
  \warning This function is deprecated. Use QwtPlotLayout::setLegendPosition().
*/

int QwtPlotLayout::legendPos() const
{
    return d_legendPos;
}

#endif // !QWT_NO_COMPAT

/*!
  Specify the relative size of the legend in the plot
  \param ratio Ratio between legend and the bounding rect 
               of title, canvas and axes. The legend will be shrinked
               if it would need more space than the given ratio.
               The ratio is limited to ]0.0 .. 1.0]. In case of <= 0.0
               it will be reset to the default ratio.
               The default vertical/horizontal ratio is 0.33/0.5. 
               
  \sa QwtPlot::setLegendPosition()
*/
void QwtPlotLayout::setLegendRatio(double ratio)
{
    setLegendPosition(legendPosition(), ratio);
}

/*!
  \return The relative size of the legend in the plot.
  \sa QwtPlotLayout::setLegendPos(), QwtPlot::setLegendPos()
*/

double QwtPlotLayout::legendRatio() const
{
    return d_legendRatio;
}

/*!
  \return Geometry for the title
  \sa QwtPlotLayout::activate(), QwtPlotLayout::invalidate()
*/

const QRect &QwtPlotLayout::titleRect() const
{
    return d_titleRect;
}

/*!
  \return Geometry for the legend
  \sa QwtPlotLayout::activate(), QwtPlotLayout::invalidate()
*/

const QRect &QwtPlotLayout::legendRect() const
{
    return d_legendRect;
}

/*!
  \param axis Axis index
  \return Geometry for the scale
  \sa QwtPlotLayout::activate(), QwtPlotLayout::invalidate()
*/

const QRect &QwtPlotLayout::scaleRect(int axis) const
{
    if ( axis < 0 || axis >= QwtPlot::axisCnt )
    {
        static QRect dummyRect;
        return dummyRect;
    }
    return d_scaleRect[axis];
}

/*!
  \return Geometry for the canvas
  \sa QwtPlotLayout::activate(), QwtPlotLayout::invalidate()
*/

const QRect &QwtPlotLayout::canvasRect() const
{
    return d_canvasRect;
}

/*!
  Invalidate the geometry of all components. 
  \sa QwtPlotLayout::activate()
*/
void QwtPlotLayout::invalidate()
{
    d_titleRect = d_legendRect = d_canvasRect = QRect();
    for (int axis = 0; axis < QwtPlot::axisCnt; axis++ )
        d_scaleRect[axis] = QRect();
}

/*!  
  \brief Return a minimum size hint
  \sa QwtPlot::minimumSizeHint()
*/

QSize QwtPlotLayout::minimumSizeHint(const QwtPlot *plot) const
{
    class ScaleData
    {
    public:
        ScaleData()
        {
            w = h = minLeft = minRight = tickOffset = 0;
        }

        int w;
        int h;
        int minLeft;
        int minRight;
        int tickOffset;
    } scaleData[QwtPlot::axisCnt];

    int canvasBorder[QwtPlot::axisCnt];

    int axis;
    for ( axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        const QwtScale *scl = plot->axis(axis);
        if ( scl )
        {
            ScaleData &sd = scaleData[axis];

            const QSize hint = scl->minimumSizeHint();
            sd.w = hint.width(); 
            sd.h = hint.height(); 
            scl->minBorderDist(sd.minLeft, sd.minRight);
            sd.tickOffset = scl->baseLineDist() +
                scl->scaleDraw()->majTickLength();
        }

        canvasBorder[axis] = plot->canvas()->frameWidth() +
            d_canvasMargin[axis] + 1;
            
    }


    for ( axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        ScaleData &sd = scaleData[axis];
        if ( sd.w && (axis == QwtPlot::xBottom || axis == QwtPlot::xTop) )
        {
            if ( (sd.minLeft > canvasBorder[QwtPlot::yLeft]) 
                && scaleData[QwtPlot::yLeft].w )
            {
                int shiftLeft = sd.minLeft - canvasBorder[QwtPlot::yLeft];
                if ( shiftLeft > scaleData[QwtPlot::yLeft].w )
                    shiftLeft = scaleData[QwtPlot::yLeft].w;

                sd.w -= shiftLeft;
            }
            if ( (sd.minRight > canvasBorder[QwtPlot::yRight]) 
                && scaleData[QwtPlot::yRight].w )
            {
                int shiftRight = sd.minRight - canvasBorder[QwtPlot::yRight];
                if ( shiftRight > scaleData[QwtPlot::yRight].w )
                    shiftRight = scaleData[QwtPlot::yRight].w;

                sd.w -= shiftRight;
            }
        }

        if ( sd.h && (axis == QwtPlot::yLeft || axis == QwtPlot::yRight) )
        {
            if ( (sd.minLeft > canvasBorder[QwtPlot::xBottom]) &&
                scaleData[QwtPlot::xBottom].h )
            {
                int shiftBottom = sd.minLeft - canvasBorder[QwtPlot::xBottom];
                if ( shiftBottom > scaleData[QwtPlot::xBottom].tickOffset )
                    shiftBottom = scaleData[QwtPlot::xBottom].tickOffset;

                sd.h -= shiftBottom;
            }
            if ( (sd.minLeft > canvasBorder[QwtPlot::xTop]) &&
                scaleData[QwtPlot::xTop].h )
            {
                int shiftTop = sd.minRight - canvasBorder[QwtPlot::xTop];
                if ( shiftTop > scaleData[QwtPlot::xTop].tickOffset )
                    shiftTop = scaleData[QwtPlot::xTop].tickOffset;

                sd.h -= shiftTop;
            }
        }
    }

    const QwtPlotCanvas *canvas = plot->canvas();

    int w = scaleData[QwtPlot::yLeft].w + scaleData[QwtPlot::yRight].w
        + QMAX(scaleData[QwtPlot::xBottom].w, scaleData[QwtPlot::xTop].w)
        + 2 * (canvas->frameWidth() + 1);
    int h = scaleData[QwtPlot::xBottom].h + scaleData[QwtPlot::xTop].h 
        + QMAX(scaleData[QwtPlot::yLeft].h, scaleData[QwtPlot::yRight].h)
        + 2 * (canvas->frameWidth() + 1);

    const QLabel *title = plot->titleLabel();
    if (title && !title->text().isEmpty())
    {
        // If only QwtPlot::yLeft or QwtPlot::yRight is showing, 
        // we center on the plot canvas.
        const bool centerOnCanvas = plot->axis(QwtPlot::yLeft) == 0 
            || plot->axis(QwtPlot::yRight) == 0;

        int titleW = w;
        if ( centerOnCanvas )
        {
            titleW -= scaleData[QwtPlot::yLeft].w 
                + scaleData[QwtPlot::yRight].w;
        }

        int titleH = title->heightForWidth(titleW);
        if ( titleH > titleW ) // Compensate for a long title
        {
            w = titleW = titleH;
            if ( centerOnCanvas )
            {
                w += scaleData[QwtPlot::yLeft].w
                    + scaleData[QwtPlot::yRight].w;
            }

            titleH = title->heightForWidth(titleW);
        }
        h += titleH + d_spacing;
    }

    // Compute the legend contribution

    const QwtLegend *legend = plot->legend();
    if ( legend && !legend->isEmpty() )
    {
        if ( d_legendPos == QwtPlot::Left || d_legendPos == QwtPlot::Right )
        {
            int legendW = legend->sizeHint().width();
            int legendH = legend->heightForWidth(legendW); 

            if ( legend->frameWidth() > 0 )
                w += d_spacing;

            if ( legendH > h )
                legendW += legend->verticalScrollBar()->sizeHint().height();

            if ( d_legendRatio < 1.0 )
                legendW = QMIN(legendW, int(w / (1.0 - d_legendRatio)));

            w += legendW;
        }
        else // QwtPlot::Top, QwtPlot::Bottom
        {
            int legendW = QMIN(legend->sizeHint().width(), w);
            int legendH = legend->heightForWidth(legendW); 

            if ( legend->frameWidth() > 0 )
                h += d_spacing;

            if ( d_legendRatio < 1.0 )
                legendH = QMIN(legendH, int(h / (1.0 - d_legendRatio)));
            
            h += legendH;
        }
    }

    w += 2 * d_margin;
    h += 2 * d_margin;

    return QSize( w, h );
}

/*!
  Find the geometry for the legend
  \param options Options how to layout the legend
  \param rect Rectangle where to place the legend
  \return Geometry for the legend
*/

QRect QwtPlotLayout::layoutLegend(int options, 
    const QRect &rect) const
{
    const QSize hint(d_layoutData->legend.hint);

    int dim;
    if ( d_legendPos == QwtPlot::Left || d_legendPos == QwtPlot::Right )
    {
        // We don't allow vertical legends to take more than
        // half of the available space.

        dim = QMIN(hint.width(), int(rect.width() * d_legendRatio));

        if ( !(options & IgnoreScrollbars) )
        {
            if ( hint.height() > rect.height() )
            {
                // The legend will need additional
                // space for the vertical scrollbar. 

                dim += d_layoutData->legend.vScrollBarWidth;
            }
        }
    }
    else
    {
        dim = QMIN(hint.height(), int(rect.height() * d_legendRatio));
        dim = QMAX(dim, d_layoutData->legend.hScrollBarHeight);
    }

    QRect legendRect = rect;
    switch(d_legendPos)
    {
        case QwtPlot::Left:
            legendRect.setWidth(dim);
            break;
        case QwtPlot::Right:
            legendRect.setX(rect.right() - dim + 1);
            legendRect.setWidth(dim);
            break;
        case QwtPlot::Top:
            legendRect.setHeight(dim);
            break;
        case QwtPlot::Bottom:
            legendRect.setY(rect.bottom() - dim + 1);
            legendRect.setHeight(dim);
            break;
    }

    return legendRect;
}

/*!
  Align the legend to the canvas
  \param canvasRect Geometry of the canvas
  \param legendRect Maximum geometry for the legend
  \return Geometry for the aligned legend
*/
QRect QwtPlotLayout::alignLegend(const QRect &canvasRect, 
    const QRect &legendRect) const
{
    QRect alignedRect = legendRect;

    if ( d_legendPos == QwtPlot::Bottom || d_legendPos == QwtPlot::Top )
    {
        if ( d_layoutData->legend.hint.width() < canvasRect.width() )
        {
            alignedRect.setX(canvasRect.x());
            alignedRect.setWidth(canvasRect.width());
        }
    }
    else
    {
        if ( d_layoutData->legend.hint.height() < canvasRect.height() )
        {
            alignedRect.setY(canvasRect.y());
            alignedRect.setHeight(canvasRect.height());
        }
    }

    return alignedRect;
}

/*!
  Expand all line breaks in text labels, and calculate the height
  of their widgets in orientation of the text.

  \param options Options how to layout the legend
  \param rect Bounding rect for title, axes and canvas.
  \param dimTitle Expanded height of the title widget
  \param dimAxis Expanded heights of the axis in axis orientation.
*/
void QwtPlotLayout::expandLineBreaks(int options, const QRect &rect, 
    int &dimTitle, int dimAxis[QwtPlot::axisCnt]) const
{
    dimTitle = 0;
    for ( int i = 0; i < QwtPlot::axisCnt; i++ )
        dimAxis[i] = 0;

    bool done = FALSE;
    while (!done)
    {
        done = TRUE;

        // the size for the 4 axis depend on each other. Expanding
        // the height of a horizontal axis will shrink the height
        // for the vertical axis, shrinking the height of a vertical
        // axis will result in a line break what will expand the
        // width and results in shrinking the width of a horizontal
        // axis what might result in a line break of a horizontal
        // axis ... . So we loop as long until no size changes.

        if ( d_layoutData->title.text)
        {
            int w = rect.width();

            if ( d_layoutData->scale[QwtPlot::yLeft].isEnabled
                != d_layoutData->scale[QwtPlot::yRight].isEnabled )
            {
                // center to the canvas
                w -= dimAxis[QwtPlot::yLeft] + dimAxis[QwtPlot::yRight]; 
            }

            int d = d_layoutData->title.text->heightForWidth(w);
            if ( !(options & IgnoreFrames) )
                d += 2 * d_layoutData->title.frameWidth;

            if ( d > dimTitle )
            {
                dimTitle = d;
                done = FALSE;
            }
        }

        for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
        {
            int backboneOffset = d_canvasMargin[axis];
            if ( !(options & IgnoreFrames) )
                backboneOffset += d_layoutData->canvas.frameWidth;

            const struct QwtPlotLayoutData::t_scaleData &scaleData = 
                d_layoutData->scale[axis];

            if (scaleData.isEnabled)
            {
                int length;
                if ( axis == QwtPlot::xTop || axis == QwtPlot::xBottom )
                {
                    length = rect.width() - dimAxis[QwtPlot::yLeft] 
                        - dimAxis[QwtPlot::yRight];
                    length += QMIN(dimAxis[QwtPlot::yLeft], 
                        scaleData.start - backboneOffset);
                    length += QMIN(dimAxis[QwtPlot::yRight], 
                        scaleData.end - backboneOffset);
                }
                else // QwtPlot::yLeft, QwtPlot::yRight
                {
                    length = rect.height() - dimAxis[QwtPlot::xTop] 
                        - dimAxis[QwtPlot::xBottom];

                    if ( dimAxis[QwtPlot::xBottom] > 0 )
                    {
                        length += QMIN(
                            d_layoutData->scale[QwtPlot::xBottom].tickOffset, 
                            scaleData.start - backboneOffset);
                    }
                    if ( dimAxis[QwtPlot::xTop] > 0 )
                    {
                        length += QMIN(
                            d_layoutData->scale[QwtPlot::xTop].tickOffset, 
                            scaleData.end - backboneOffset);
                    }

                    if ( dimTitle > 0 )
                        length -= dimTitle + d_spacing;
                }

                int d = scaleData.dimWithoutTitle;
                if ( !scaleData.scale->title().isEmpty() )
                {
                    d += scaleData.scale->titleHeightForWidth(length);
                }

                if ( options & AlignScales )
                    d -= scaleData.baseLineOffset;

                if ( d > dimAxis[axis] )
                {
                    dimAxis[axis] = d;
                    done = FALSE;
                }
            }
        }
    }
}

/*!
  Align the ticks of the axis to the canvas borders using
  the empty corners.
*/

void QwtPlotLayout::alignScales(int options,
    QRect &canvasRect, QRect scaleRect[QwtPlot::axisCnt]) const
{
    int axis;

    int backboneOffset[QwtPlot::axisCnt];
    for (axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        backboneOffset[axis] = 0;
        if ( !d_alignCanvasToScales )
            backboneOffset[axis] += d_canvasMargin[axis];
        if ( !(options & IgnoreFrames) )
            backboneOffset[axis] += d_layoutData->canvas.frameWidth;
    }

    for (axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        if ( !scaleRect[axis].isValid() )
            continue;

        const int startDist = d_layoutData->scale[axis].start;
        const int endDist = d_layoutData->scale[axis].end;

        QRect &axisRect = scaleRect[axis];

        if ( axis == QwtPlot::xTop || axis == QwtPlot::xBottom )
        {
            const int leftOffset = backboneOffset[QwtPlot::yLeft] - startDist;

            if ( scaleRect[QwtPlot::yLeft].isValid() )
            {
                int minLeft = scaleRect[QwtPlot::yLeft].left();
                int left = axisRect.left() + leftOffset;
                axisRect.setLeft(QMAX(left, minLeft));
            }
            else
            {
                if ( d_alignCanvasToScales )
                {
                    canvasRect.setLeft(QMAX(canvasRect.left(), 
                        axisRect.left() - leftOffset));
                }
                else
                {
                    if ( leftOffset > 0 )
                        axisRect.setLeft(axisRect.left() + leftOffset);
                }
            }

            const int rightOffset = backboneOffset[QwtPlot::yRight] - endDist;

            if ( scaleRect[QwtPlot::yRight].isValid() )
            {
                int maxRight = scaleRect[QwtPlot::yRight].right();
                int right = axisRect.right() - rightOffset;
                axisRect.setRight(QMIN(right, maxRight));
            }
            else
            {
                if ( d_alignCanvasToScales )
                {
                    canvasRect.setRight( QMIN(canvasRect.right(), 
                        axisRect.right() + rightOffset) );
                }
                else
                {
                    if ( rightOffset > 0 )
                        axisRect.setRight(axisRect.right() - rightOffset);
                }
            }
        }
        else // QwtPlot::yLeft, QwtPlot::yRight
        {
            const int bottomOffset = 
                backboneOffset[QwtPlot::xBottom] - startDist;

            if ( scaleRect[QwtPlot::xBottom].isValid() )
            {
                int maxBottom = scaleRect[QwtPlot::xBottom].top() + 
                    d_layoutData->scale[QwtPlot::xBottom].tickOffset;

                int bottom = axisRect.bottom() - bottomOffset;
                axisRect.setBottom(QMIN(bottom, maxBottom));
            }
            else
            {
                if ( d_alignCanvasToScales )
                {
                    canvasRect.setBottom(QMIN(canvasRect.bottom(), 
                        axisRect.bottom() + bottomOffset));
                }
                else
                {
                    if ( bottomOffset > 0 )
                        axisRect.setBottom(axisRect.bottom() - bottomOffset);
                }
            }
        
            const int topOffset = backboneOffset[QwtPlot::xTop] - endDist;

            if ( scaleRect[QwtPlot::xTop].isValid() )
            {
                int minTop = scaleRect[QwtPlot::xTop].bottom() -
                    d_layoutData->scale[QwtPlot::xTop].tickOffset;

                int top = axisRect.top() + topOffset;
                axisRect.setTop(QMAX(top, minTop));
            }
            else
            {
                if ( d_alignCanvasToScales )
                {
                    canvasRect.setTop(QMAX(canvasRect.top(), 
                        axisRect.top() + - topOffset));
                }
                else
                {
                    if ( topOffset > 0 )
                        axisRect.setTop(axisRect.top() + topOffset);
                }
            }
        }
    }
}

/*!
  \brief Recalculate the geometry of all components. 

  \param plot Plot to be layout
  \param plotRect Rect where to place the components
  \param options Options

  \sa QwtPlotLayout::invalidate(), QwtPlotLayout::titleRect(),
      QwtPlotLayout::legendRect(), QwtPlotLayout::scaleRect(), 
      QwtPlotLayout::canvasRect()
*/

void QwtPlotLayout::activate(const QwtPlot *plot,
    const QRect &plotRect, int options) 
{
    invalidate();

    QRect rect(plotRect);  // undistributed rest of the plot rect

    if ( !(options & IgnoreMargin) )
    {
        // subtract the margin

        rect.setRect(
            rect.x() + d_margin, 
            rect.y() + d_margin,
            rect.width() - 2 * d_margin, 
            rect.height() - 2 * d_margin
        );
    }

    // We extract all layout relevant data from the widgets,
    // filter them through pfilter and save them to d_layoutData.

    d_layoutData->init(plot, rect);

    if (!(options & IgnoreLegend)
        && plot->legend() && !plot->legend()->isEmpty() )
    {
        d_legendRect = layoutLegend(options, rect);

        // subtract d_legendRect from rect

        const QRegion region(rect);
        rect = region.subtract(d_legendRect).boundingRect(); 

        if ( d_layoutData->legend.frameWidth && 
            !(options & IgnoreFrames ) )
        {
            // In case of a frame we have to insert a spacing.
            // Otherwise the leading of the font separates
            // legend and scale/canvas

            switch(d_legendPos)
            {
                case QwtPlot::Left:
                    rect.setLeft(rect.left() + d_spacing);
                    break;
                case QwtPlot::Right:
                    rect.setRight(rect.right() - d_spacing);
                    break;
                case QwtPlot::Top:
                    rect.setTop(rect.top() + d_spacing);
                    break;
                case QwtPlot::Bottom:
                    rect.setBottom(rect.bottom() - d_spacing);
                    break;
            }
        }
    }

    /*
     +---+-----------+---+
     |       Title       |
     +---+-----------+---+
     |   |   Axis    |   |
     +---+-----------+---+
     | A |           | A |
     | x |  Canvas   | x |
     | i |           | i |
     | s |           | s |
     +---+-----------+---+
     |   |   Axis    |   |
     +---+-----------+---+
    */


    // axes and title include text labels. The height of each
    // label depends on its line breaks, that depend on the width
    // for the label. A line break in a horizontal text will reduce
    // the available width for vertical texts and vice versa. 
    // expandLineBreaks finds the height/width for title and axes
    // including all line breaks.

    int dimTitle, dimAxes[QwtPlot::axisCnt];
    expandLineBreaks(options, rect, dimTitle, dimAxes);

    if (dimTitle > 0 )
    {
        d_titleRect = QRect(rect.x(), rect.y(),
            rect.width(), dimTitle);

        if ( d_layoutData->scale[QwtPlot::yLeft].isEnabled !=
            d_layoutData->scale[QwtPlot::yRight].isEnabled )
        {
            // if only one of the y axes is missing we align
            // the title centered to the canvas

            d_titleRect.setX(rect.x() + dimAxes[QwtPlot::yLeft]);
            d_titleRect.setWidth(rect.width() 
                - dimAxes[QwtPlot::yLeft] - dimAxes[QwtPlot::yRight]);
        }

        // subtract title 
        rect.setTop(rect.top() + dimTitle + d_spacing);
    }

    d_canvasRect.setRect(
        rect.x() + dimAxes[QwtPlot::yLeft],
        rect.y() + dimAxes[QwtPlot::xTop],
        rect.width() - dimAxes[QwtPlot::yRight] - dimAxes[QwtPlot::yLeft],
        rect.height() - dimAxes[QwtPlot::xBottom] - dimAxes[QwtPlot::xTop]);

    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        // set the rects for the axes

        if ( dimAxes[axis] )
        {
            int dim = dimAxes[axis];
            QRect &scaleRect = d_scaleRect[axis];

            scaleRect = d_canvasRect;
            switch(axis)
            {
                case QwtPlot::yLeft:
                    scaleRect.setX(d_canvasRect.left() - dim);
                    scaleRect.setWidth(dim);
                    break;
                case QwtPlot::yRight:
                    scaleRect.setX(d_canvasRect.right() + 1);
                    scaleRect.setWidth(dim);
                    break;
                case QwtPlot::xBottom:
                    scaleRect.setY(d_canvasRect.bottom() + 1);
                    scaleRect.setHeight(dim);
                    break;
                case QwtPlot::xTop:
                    scaleRect.setY(d_canvasRect.top() - dim);
                    scaleRect.setHeight(dim);
                    break;
            }
            scaleRect = scaleRect.normalize();
        }
    }

    // +---+-----------+---+
    // |  <-   Axis   ->   |
    // +-^-+-----------+-^-+
    // | | |           | | |
    // |   |           |   |
    // | A |           | A |
    // | x |  Canvas   | x |
    // | i |           | i |
    // | s |           | s |
    // |   |           |   |
    // | | |           | | |
    // +-V-+-----------+-V-+
    // |   <-  Axis   ->   |
    // +---+-----------+---+

    // The ticks of the axes - not the labels above - should
    // be aligned to the canvas. So we try to use the empty
    // corners to extend the axes, so that the label texts
    // left/right of the min/max ticks are moved into them.
 
    alignScales(options, d_canvasRect, d_scaleRect);

    if (!d_legendRect.isEmpty() )
    {
        // We prefer to align the legend to the canvas - not to
        // the complete plot - if possible.

        d_legendRect = alignLegend(d_canvasRect, d_legendRect);
    }
}

QwtPlotLayoutData::QwtPlotLayoutData()
{
    title.text = NULL;
}

QwtPlotLayoutData::~QwtPlotLayoutData()
{
    delete title.text;
}

/*
  Extract all layout relevant data from the plot components
*/

void QwtPlotLayoutData::init(const QwtPlot *plot, const QRect &rect)
{
    // legend

    legend.frameWidth = plot->legend()->frameWidth();
    legend.vScrollBarWidth = 
        plot->legend()->verticalScrollBar()->sizeHint().width();
    legend.hScrollBarHeight = 
        plot->legend()->horizontalScrollBar()->sizeHint().height();

    const QSize hint = plot->legend()->sizeHint();

    int w = QMIN(hint.width(), rect.width());
    int h = plot->legend()->heightForWidth(w);
    if ( h == 0 )
        h = hint.height();

    if ( h > rect.height() )
        w += legend.vScrollBarWidth;

    legend.hint = QSize(w, h);

    // title 

    title.frameWidth = 0;
    delete title.text;
    title.text = NULL;

    if (plot->titleLabel() && !plot->titleLabel()->text().isEmpty())
    {
        const QLabel *label = plot->titleLabel();
        title.text = QwtText::makeText(label->text(), label->textFormat(), 
            label->alignment(), label->font());
        title.frameWidth = plot->titleLabel()->frameWidth();
    }

    // scales 

    for (int axis = 0; axis < QwtPlot::axisCnt; axis++ )
    {
        const QwtScale *sd = plot->axis(axis);
        if ( sd )
        {
            scale[axis].isEnabled = TRUE;

            scale[axis].scale = sd;

            scale[axis].scaleFont = sd->font();

            scale[axis].start = sd->startBorderDist();
            scale[axis].end = sd->endBorderDist();

            scale[axis].baseLineOffset = sd->baseLineDist();
            scale[axis].tickOffset = sd->baseLineDist() + 
                (int)sd->scaleDraw()->majTickLength();

            scale[axis].dimWithoutTitle = sd->dimForLength(
                QCOORD_MAX, scale[axis].scaleFont);

            if ( !sd->title().isEmpty() )
            {
                scale[axis].dimWithoutTitle -= 
                    sd->titleHeightForWidth(QCOORD_MAX);
            }
        }
        else
        {
            scale[axis].isEnabled = FALSE;
            scale[axis].start = 0;
            scale[axis].end = 0;
            scale[axis].baseLineOffset = 0;
            scale[axis].tickOffset = 0;
            scale[axis].dimWithoutTitle = 0;
        }
    }

    // canvas 

    canvas.frameWidth = plot->canvas()->frameWidth();
}
