#include "PlotPoint.h"
#include <QApplication>
#include <QMouseEvent>

PlotPoint::PlotPoint(QCustomPlot *parentPlot, int halfSize)
    : QCPItemEllipse(parentPlot),
      mCenterTracer(new QCPItemTracer(parentPlot)),
      mGripDelta(),
      mInitialPos(),
      mLastWantedPos(),
      mMoveTimer(new QTimer(this)),
      mCurWantedPosPx()
{
    layer()->setMode(QCPLayer::LayerMode::lmBuffered);
    mCenterTracer->setStyle(QCPItemTracer::tsNone);

    topLeft->setParentAnchor(mCenterTracer->position);
    bottomRight->setParentAnchor(mCenterTracer->position);
    topLeft->setType(QCPItemPosition::ptAbsolute);
    bottomRight->setType(QCPItemPosition::ptAbsolute);

    topLeft->setCoords(-halfSize, -halfSize);
    bottomRight->setCoords(halfSize, halfSize);

    setSelectable(true); // plot moves only selectable points, see
                         // Plot::mouseMoveEvent
    setColor(QColor(255, 0, 0, 100));
    setPen(QPen(Qt::red));
    setSelectedPen(QPen(Qt::black, 3));

    mMoveTimer->setInterval(25); // 40 FPS
    connect(mMoveTimer, SIGNAL(timeout()), this, SLOT(moveToWantedPos()));
}

QPointF PlotPoint::pos() const
{
    return mCenterTracer->position->coords();
}

const QColor &PlotPoint::color() const
{
    return brush().color();
}

void PlotPoint::setColor(const QColor &color)
{
    setBrush(color);
    setSelectedBrush(color);
}

void PlotPoint::startMoving(const QPointF &mousePos, bool shiftIsPressed)
{
    mGripDelta.setX(
            parentPlot()->xAxis->coordToPixel(mCenterTracer->position->key())
            - mousePos.x());
    mGripDelta.setY(
            parentPlot()->yAxis->coordToPixel(mCenterTracer->position->value())
            - mousePos.y());

    mInitialPos = pos();
    mLastWantedPos = mInitialPos;
    mCurWantedPosPx = QPointF();
    mIsChangingOnlyOneCoordinate = shiftIsPressed;

    mMoveTimer->start();

    QCPItemStraightLine *horizontal = new QCPItemStraightLine(parentPlot());
    horizontal->setAntialiased(false);
    horizontal->point1->setCoords(mInitialPos.x(), mInitialPos.y());
    horizontal->point2->setCoords(mInitialPos.x() + 1, mInitialPos.y());
    // parentPlot()->addItem(horizontal);

    QCPItemStraightLine *vertical = new QCPItemStraightLine(parentPlot());
    vertical->setAntialiased(false);
    vertical->point1->setCoords(mInitialPos.x(), mInitialPos.y());
    vertical->point2->setCoords(mInitialPos.x(), mInitialPos.y() + 1);
    // parentPlot()->addItem(vertical);

    static const QPen linesPen(Qt::darkGray, 0, Qt::DashLine);
    horizontal->setPen(linesPen);
    vertical->setPen(linesPen);

    mHelperItems << vertical << horizontal;

    if (!mIsChangingOnlyOneCoordinate) {
        vertical->setVisible(false);
        horizontal->setVisible(false);
    }

    connect(parentPlot(), SIGNAL(mouseMove(QMouseEvent *)), this,
            SLOT(onMouseMove(QMouseEvent *)));

    connect(parentPlot(), SIGNAL(mouseRelease(QMouseEvent *)), this,
            SLOT(stopMoving()));

    connect(parentPlot(), SIGNAL(shiftStateChanged(bool)), this,
            SLOT(onShiftStateChanged(bool)));

    parentPlot()->grabKeyboard();
    QApplication::setOverrideCursor(Qt::ClosedHandCursor);
}

void PlotPoint::setVisible(bool on)
{
    setSelectable(on); // we are movable only when visible
    QCPItemEllipse::setVisible(on);
}

void PlotPoint::stopMoving()
{
    disconnect(parentPlot(), SIGNAL(mouseMove(QMouseEvent *)), this,
               SLOT(onMouseMove(QMouseEvent *)));

    disconnect(parentPlot(), SIGNAL(mouseRelease(QMouseEvent *)), this,
               SLOT(stopMoving()));

    disconnect(parentPlot(), SIGNAL(shiftStateChanged(bool)), this,
               SLOT(onShiftStateChanged(bool)));

    mMoveTimer->stop();
    moveToWantedPos();

    if (!mHelperItems.isEmpty()) {
        while (!mHelperItems.isEmpty()) {
            QCPAbstractItem *item = mHelperItems.takeFirst();
            mParentPlot->removeItem(item);
        }

        layer()->replot();
    }

    parentPlot()->releaseKeyboard();
    QApplication::restoreOverrideCursor();

    emit stoppedMoving();
}

void PlotPoint::move(double x, double y, bool signalNeeded)
{
    mLastWantedPos.setX(x);
    mLastWantedPos.setY(y);
    if (mIsChangingOnlyOneCoordinate) {
        double x1 = parentPlot()->xAxis->coordToPixel(x);
        double x2 = parentPlot()->xAxis->coordToPixel(mInitialPos.x());
        double y1 = parentPlot()->yAxis->coordToPixel(y);
        double y2 = parentPlot()->yAxis->coordToPixel(mInitialPos.y());
        if (qAbs(x1 - x2) < qAbs(y1 - y2)) {
            x = mInitialPos.x();
        } else {
            y = mInitialPos.y();
        }
    }

    mCenterTracer->position->setCoords(x, y);

    layer()->replot();

    if (signalNeeded) {
        emit moved(QPointF(x, y));
    }
}

void PlotPoint::movePx(double x, double y)
{
    move(parentPlot()->xAxis->pixelToCoord(x),
         parentPlot()->yAxis->pixelToCoord(y));
}

void PlotPoint::setActive(bool isActive)
{
    setSelected(isActive);
    emit(isActive ? activated() : disactivated());
}

void PlotPoint::onMouseMove(QMouseEvent *event)
{
    // use localPos()
    mCurWantedPosPx = QPointF(event->pos().x() + mGripDelta.x(),
                              event->pos().y() + mGripDelta.y());
}

void PlotPoint::moveToWantedPos()
{
    if (!mCurWantedPosPx.isNull()) {
        movePx(mCurWantedPosPx.x(), mCurWantedPosPx.y());
        mCurWantedPosPx = QPointF();
    }
}

void PlotPoint::onShiftStateChanged(bool shiftPressed)
{
    if (shiftPressed != mIsChangingOnlyOneCoordinate) {
        mIsChangingOnlyOneCoordinate = shiftPressed;
        foreach (QCPAbstractItem *item, mHelperItems) {
            item->setVisible(shiftPressed);
        }
        move(mLastWantedPos.x(), mLastWantedPos.y());
    }
}
