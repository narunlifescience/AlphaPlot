#include <qobject.h>
#include "plot.h"

class Graph;
	
class CanvasPicker: public QObject
{
    Q_OBJECT
public:
    CanvasPicker(Graph *plot);
    virtual bool eventFilter(QObject *, QEvent *);
	void selectPoints(int n);
	int selectedPoints(){return selected_points;};
	void selectPeak(const QPoint& p);

private:
	void drawTextMarker(const QPoint&);
	void drawLineMarker(const QPoint&, bool endArrow);
	bool selectMarker(const QPoint& );
	void moveMarker(QPoint& );
	void releaseMarker();

	Graph *plot() { return (Graph *)parent(); }
	
	Plot* plotWidget;	
	QPoint startLinePoint, endLinePoint;
	
	int xMouse, yMouse, xMrk, yMrk, n_peaks, selected_points;
	bool moved,	movedGraph, resizedGraph, ShiftButton, pointSelected, select_peaks;	
	
signals:
	void showPieDialog();
	void showPlotDialog(long);
	void viewTextDialog();
	void viewLineDialog();
	void viewImageDialog();
	void drawTextOff();
	void showMarkerPopupMenu();
	void modified();
	void calculateProfile(const QPoint&, const QPoint&);
	void selectPlot();
	void moveGraph(const QPoint&);
	void resizeGraph(const QPoint&);
	void releasedGraph();
	void newSizeGraph();
};
