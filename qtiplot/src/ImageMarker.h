#ifndef IMAGEMARKER_H
#define IMAGEMARKER_H

#include <qpaintdevicemetrics.h>
#include <qpixmap.h>
#include <qwt_plot_classes.h>
#include <qwt_marker.h>
	
class ImageMarker: public QwtPlotMarker
{
public:
	ImageMarker(const QPixmap& p, QwtPlot *plot);
    virtual void draw(QPainter *p, int x, int y, const QRect &);
	
	QRect rect(){return QRect(origin,picSize);};

	QSize size(){return picSize;};
	void setSize(const QSize& size)
		{
		if (picSize == size) return;
		picSize=size;
		}
	
	void setFileName(const QString& fn){fileName=fn;};
	QString getFileName(){return fileName;};
	
	QPixmap image(){return pic;};

	void setOrigin(const QPoint& p){origin=p;};
	QPoint getOrigin(){return origin;};

private:
	QPoint origin;
	QPixmap pic;
	QSize picSize;
	QString fileName;
};

#endif
