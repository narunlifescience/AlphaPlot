/***************************************************************************
File                 : EpsPaintDevice.cpp
Project              : EpsEngine
--------------------------------------------------------------------
Copyright            : (C) 2014 by Ion Vasilief
Email (use @ for *)  : ion_vasilief*yahoo.fr
Description          : Enables the export of QPainter grafics to .eps files
***************************************************************************/
#include "EpsEngine.h"

#include <QApplication>
#include <QDesktopWidget>

EpsPaintDevice::EpsPaintDevice(const QString& fileName, const QSize& s)
: QPaintDevice()
{
	d_size = s;

	if (!d_size.isValid())
		d_size = QSize(500, 400);

	engine = new EpsPaintEngine(fileName);
}

EpsPaintDevice::~EpsPaintDevice()
{
	delete engine;
}

QPaintEngine * EpsPaintDevice::paintEngine () const
{
	return engine;
}

void EpsPaintDevice::setColorMode(const QPrinter::ColorMode& mode)
{
	engine->setGrayScale(mode == QPrinter::GrayScale);
}

void EpsPaintDevice::setPreviewPixmap(const QPixmap& pix, int compression)
{
	engine->setPreviewPixmap(pix, compression);
}

void EpsPaintDevice::setCreator(const QString& s)
{
	engine->setCreator(s);
}

int EpsPaintDevice::metric(PaintDeviceMetric metric) const
{
	QDesktopWidget *desktop = QApplication::desktop();
	int dpi_x = desktop->logicalDpiX();
	int dpi_y = desktop->logicalDpiY();
	switch (metric){
		case QPaintDevice::PdmWidth:
			return d_size.width();
		case QPaintDevice::PdmHeight:
			return d_size.height();
		case QPaintDevice::PdmWidthMM:
			return int(25.4*d_size.width()/(double)dpi_x);
		case QPaintDevice::PdmHeightMM:
			return int(25.4*d_size.height()/(double)dpi_y);
		case QPaintDevice::PdmNumColors:
			return 65536;//should it be millions?
		case QPaintDevice::PdmDepth:
			return 32;
		case QPaintDevice::PdmDpiX:
		case QPaintDevice::PdmPhysicalDpiX:
			return dpi_x;
		case QPaintDevice::PdmDpiY:
		case QPaintDevice::PdmPhysicalDpiY:
			return dpi_y;
	#if QT_VERSION >= 0x050000
		case QPaintDevice::PdmDevicePixelRatio:
			return (int)qApp->devicePixelRatio();
	#endif
		default:
			qWarning ("EpsPaintDevice::Unknown metric asked");
			return 0;
	}
}
