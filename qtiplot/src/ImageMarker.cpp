#include "ImageMarker.h"
#include <QPainter>

ImageMarker::ImageMarker(const QPixmap& p):
    d_pic(p),
	d_pos(QPoint(0,0)),
	d_size(p.size())
{
}

void ImageMarker::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &r) const
{
p->drawPixmap(transform(xMap, yMap, d_rect), d_pic);
}

void ImageMarker::setSize(const QSize& size)
{
d_size = size;
updateBoundingRect();
}

void ImageMarker::setOrigin(const QPoint& p)
{
d_pos = p;

if (!plot())
	return;

d_rect.moveTo(plot()->invTransform(xAxis(), p.x()), plot()->invTransform(yAxis(), p.y()));
d_size = size();
updateBoundingRect();
}

void ImageMarker::setRect(int x, int y, int w, int h)
{
if (d_pos == QPoint(x, y) && d_size == QSize(w, h))
	return;

d_pos = QPoint(x, y);
d_size = QSize(w, h);
updateBoundingRect();
}

void ImageMarker::setBoundingRect(const QwtDoubleRect& rect)
{
if (d_rect == rect)
	return;

d_rect = rect;

if (!plot())
	return;

plot()->updateLayout();

QRect r = this->rect();
d_pos = r.topLeft();
d_size = r.size();
}

void ImageMarker::updateBoundingRect()
{
if (!plot())
	return;

d_rect = invTransform(plot()->canvasMap(xAxis()), plot()->canvasMap(yAxis()), QRect(d_pos, d_size));
}
