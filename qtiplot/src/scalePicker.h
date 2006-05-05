#include <qobject.h>

class QwtPlot;
class QwtScale;
class QLabel;
	
class ScalePicker: public QObject
{
    Q_OBJECT
public:
    ScalePicker(QwtPlot *plot);
    virtual bool eventFilter(QObject *, QEvent *);
	QRect scaleRect(const QwtScale *) const;
	int maxLabelWidth(const QwtScale *scale) const;
	int maxLabelHeight(const QwtScale *scale) const;

	void mouseDblClicked(const QwtScale *, const QPoint &);
	void mouseClicked(const QwtScale *scale, const QPoint &pos) ;
	void mouseRightClicked(const QwtScale *scale, const QPoint &pos);

	void refresh();
	
	QwtPlot *plot() { return (QwtPlot *)parent(); }

signals:
	void clicked();

	void axisRightClicked(int);
	void axisTitleRightClicked(int);

	void axisDblClicked(int);
	void axisTitleDblClicked(int);

	void xAxisTitleDblClicked();
	void yAxisTitleDblClicked();
	void rightAxisTitleDblClicked();
	void topAxisTitleDblClicked();
	
	void moveGraph(const QPoint&);
	void releasedGraph();
	void highlightGraph();

private:
	bool movedGraph;
};

class TitlePicker: public QObject
{
    Q_OBJECT
public:
    TitlePicker(QwtPlot *plot);
    virtual bool eventFilter(QObject *, QEvent *);

signals:
	void clicked();
    void doubleClicked();
	void removeTitle();
	void showTitleMenu();

	//moving and highlighting the plot parent
	void moveGraph(const QPoint&);
	void releasedGraph();
	void highlightGraph();

protected:
	QLabel *title;
	bool movedGraph;
};
