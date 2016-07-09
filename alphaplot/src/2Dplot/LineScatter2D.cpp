#include "LineScatter2D.h"

LineScatter2D::LineScatter2D(Axis2D *xAxis, Axis2D *yAxis)
    : QCPGraph(xAxis, yAxis),
      scatterShape_(QCPScatterStyle::ssPlus),
      scatterPen_(QPen(QColor(0, 0, 175, 100))),
      scatterBrush_(QBrush(Qt::yellow)),
      scatterSize_(5),
      lineStyle_(NonePlot),
      linePen_(QPen(Qt::red, 0)),
      lineBrush_(QBrush(Qt::NoBrush)),
      autoAntialiasing_(true) {
  scatterStyle_.setShape(QCPScatterStyle::ssTriangle);
  scatterStyle_.setPen(scatterPen_);
  scatterStyle_.setBrush(scatterBrush_);
  scatterStyle_.setSize(scatterSize_);
  setScatterStyle(scatterStyle_);
  setPen(linePen_);
  setBrush(lineBrush_);
}

LineScatter2D::~LineScatter2D() {}

void LineScatter2D::setLineScatter2DPlot(const Line &line,
                                         const Scatter &scatter) {
  if (line == NonePlot && scatter == ScatterHidden) {
    qDebug() << "incompatible Line Scatter combo : QCPGraph::lsNone, "
                "QCPScatterStyle::ssNone";
    return;
  }
  bool antialiasedLine = false;

    switch (line) {
      case LinePlot:
        setLineStyle(QCPGraph::lsLine);
        antialiasedLine = true;
        break;
      case NonePlot:
        setLineStyle(QCPGraph::lsNone);
        break;
      case VerticalDropLinePlot:
        setLineStyle(QCPGraph::lsImpulse);
        antialiasedLine = false;
        break;
      case SplinePlot:
        // a quadratic or higher order interpolation
        break;
      case CentralStepAndScatterPlot:
        setLineStyle(QCPGraph::lsStepCenter);
        antialiasedLine = false;
        break;
      case HorizontalStepPlot:
        setLineStyle(QCPGraph::lsStepLeft);
        antialiasedLine = false;
        break;
      case VerticalStepPlot:
        setLineStyle(QCPGraph::lsStepRight);
        antialiasedLine = false;
        break;
      case AreaPlot:
        setLineStyle(QCPGraph::lsLine);
        setBrush(QBrush(Qt::green));
        antialiasedLine = true;
        break;
    }

    if(autoAntialiasing_)
    setAntialiased(antialiasedLine);

  switch (scatter) {
    case ScatterVisible:
       if (scatterShape_ == QCPScatterStyle::ssNone) {
        scatterShape_ = QCPScatterStyle::ssDisc;
      }
      setScatterShape2D(QCPScatterStyle::ssDisc);
      break;
    case ScatterHidden:
      scatterShape_ = QCPScatterStyle::ssNone;
      setScatterShape2D(scatterShape_);
      break;
  }
}

// scatter style
void LineScatter2D::setScatterShape2D(
    const QCPScatterStyle::ScatterShape &shape) {

  bool antialiasedScatter = false;

  switch (shape) {
    case QCPScatterStyle::ssPixmap:
    case QCPScatterStyle::ssCustom:
      qDebug() << "unsupported QCPScatterStyle";
      return;
    case QCPScatterStyle::ssNone:
      scatterStyle_.setShape(QCPScatterStyle::ssNone);
      break;
    case QCPScatterStyle::ssDot:
      scatterStyle_.setShape(QCPScatterStyle::ssDot);
      antialiasedScatter = false;
      break;
    case QCPScatterStyle::ssCross:
      scatterStyle_.setShape(QCPScatterStyle::ssCross);
      antialiasedScatter = true;
      break;
    case QCPScatterStyle::ssPlus:
      scatterStyle_.setShape(QCPScatterStyle::ssPlus);
      antialiasedScatter = false;
      break;
    case QCPScatterStyle::ssCircle:
      scatterStyle_.setShape(QCPScatterStyle::ssCircle);
      antialiasedScatter = true;
      break;
    case QCPScatterStyle::ssDisc:
      scatterStyle_.setShape(QCPScatterStyle::ssDisc);
      antialiasedScatter = true;
      break;
    case QCPScatterStyle::ssSquare:
      scatterStyle_.setShape(QCPScatterStyle::ssSquare);
      antialiasedScatter = false;
      break;
    case QCPScatterStyle::ssDiamond:
      scatterStyle_.setShape(QCPScatterStyle::ssDiamond);
      antialiasedScatter = true;
      break;
    case QCPScatterStyle::ssStar:
      scatterStyle_.setShape(QCPScatterStyle::ssStar);
      antialiasedScatter = true;
      break;
    case QCPScatterStyle::ssTriangle:
      scatterStyle_.setShape(QCPScatterStyle::ssTriangle);
      antialiasedScatter = true;
      break;
    case QCPScatterStyle::ssTriangleInverted:
      scatterStyle_.setShape(QCPScatterStyle::ssTriangleInverted);
      antialiasedScatter = true;
      break;
    case QCPScatterStyle::ssCrossSquare:
      scatterStyle_.setShape(QCPScatterStyle::ssCrossSquare);
      antialiasedScatter = true;
      break;
    case QCPScatterStyle::ssPlusSquare:
      scatterStyle_.setShape(QCPScatterStyle::ssPlusSquare);
      antialiasedScatter = false;
      break;
    case QCPScatterStyle::ssCrossCircle:
      scatterStyle_.setShape(QCPScatterStyle::ssCrossCircle);
      antialiasedScatter = true;
      break;
    case QCPScatterStyle::ssPlusCircle:
      scatterStyle_.setShape(QCPScatterStyle::ssPlusCircle);
      antialiasedScatter = true;
      break;
    case QCPScatterStyle::ssPeace:
      scatterStyle_.setShape(QCPScatterStyle::ssPeace);
      antialiasedScatter = true;
      break;
  }
  if(autoAntialiasing_) setAntialiasedScatters(antialiasedScatter);

  scatterShape_ = shape;
  setScatterStyle(scatterStyle_);
}

// scatter color
void LineScatter2D::setScatterPen2D(const QPen &pen) {
  if (scatterPen_ == pen) {
    return;
  }

  scatterStyle_.setPen(pen);
  scatterPen_ = pen;
  setScatterStyle(scatterStyle_);
}

// scatter fill
void LineScatter2D::setScatterBrush2D(const QBrush &brush) {
  if (scatterBrush_ == brush) {
    return;
  }

  scatterStyle_.setBrush(brush);
  scatterBrush_ = brush;
  setScatterStyle(scatterStyle_);
}

// scatter size
void LineScatter2D::setScatterSize2D(const double size) {
  if (scatterSize_ == size) {
    return;
  }

  scatterStyle_.setSize(size);
  scatterSize_ = size;
  setScatterStyle(scatterStyle_);
}

// line color
void LineScatter2D::setLinePen2D(const QPen &pen) {
  if (linePen_ == pen) {
    return;
  }

  setPen(pen);
  linePen_ = pen;
}

// fill under line area
void LineScatter2D::setLineBrush2D(const QBrush &brush) {
  if (lineBrush_ == brush) {
    return;
  }

  setBrush(brush);
  lineBrush_ = brush;
}
