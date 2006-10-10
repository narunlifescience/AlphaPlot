#ifndef IMAGEMARKER_H
#define IMAGEMARKER_H

#include <qpixmap.h>

#include <qwt_plot.h>
#include <qwt_plot_marker.h>
	
class ImageMarker: public QwtPlotMarker
{
public:
	ImageMarker(const QPixmap& p);
    virtual void draw(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &r) const;
	
	QRect rect(){return transform(plot()->canvasMap(xAxis()), plot()->canvasMap(yAxis()), d_rect);};
	void setRect(int x, int y, int w, int h);

	QSize size(){return rect().size();};
	void setSize(const QSize& size);
	
	void setFileName(const QString& fn){d_fileName = fn;};
	QString getFileName(){return d_fileName;};
	
	QPixmap image(){return d_pic;};

	void setOrigin(const QPoint& p);
	QPoint getOrigin(){return rect().topLeft();};

	QwtDoubleRect boundingRect() const {return d_rect;};
	void setBoundingRect(const QwtDoubleRect& rect);

	void updateBoundingRect();

private:
	QPoint d_pos;
	QPixmap d_pic;
	QSize d_size;
	QString d_fileName;
	QwtDoubleRect d_rect;
};

#endif
