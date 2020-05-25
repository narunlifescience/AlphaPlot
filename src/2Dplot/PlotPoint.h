#ifndef PLOTPOINT_H
#define PLOTPOINT_H

#include "../3rdparty/qcustomplot/qcustomplot.h"

class PlotPoint : public QCPItemEllipse
{
    Q_OBJECT
public:
    explicit PlotPoint(QCustomPlot *parentPlot, int halfSize = 5);

    QPointF pos() const;
    const QColor &color() const;
    void setColor(const QColor &color);
    void startMoving(const QPointF &mousePos, bool shiftIsPressed);

public slots:
    void setVisible(bool on);

signals:
    void activated(); ///< emitted on mouse over
    void disactivated(); ///< emitted when cursor leave us

    void moved(const QPointF &pos);
    void stoppedMoving();
public slots:
    void move(double x, double y, bool signalNeeded = true);
    void movePx(double x, double y);
    void setActive(bool isActive);

private slots:
    void onMouseMove(QMouseEvent *event);
    void stopMoving();
    void moveToWantedPos();
    void onShiftStateChanged(bool shiftPressed);

private:
    QCPItemTracer *mCenterTracer;
    QPointF mGripDelta;
    QPointF mInitialPos;
    bool mIsChangingOnlyOneCoordinate;
    QList<QCPAbstractItem *> mHelperItems;
    QPointF mLastWantedPos;
    QTimer *mMoveTimer;
    QPointF mCurWantedPosPx;
};

#endif // PLOTPOINT_H
