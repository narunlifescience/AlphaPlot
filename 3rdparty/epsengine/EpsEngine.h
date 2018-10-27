/***************************************************************************
File                 : EpsEngine.h
Project              : EpsEngine
--------------------------------------------------------------------
Copyright            : (C) 2014 by Ion Vasilief
Email (use @ for *)  : ion_vasilief*yahoo.fr
Description          : Enables the export of QPainter grafics to .eps files
***************************************************************************/
#ifndef EPS_ENGINE_H
#define EPS_ENGINE_H

#include <QPaintDevice>
#include <QPaintEngine>
#include <QPrinter>

class QFile;
class EpsPaintEngine;

class EpsPaintDevice : public QPaintDevice
{
public:
	EpsPaintDevice(const QString& fileName, const QSize& s = QSize());
	~EpsPaintDevice();

	virtual QPaintEngine * paintEngine() const;
	//! Set color mode (Color or GrayScale)
	void setColorMode(const QPrinter::ColorMode &);
	//! Set size
	void setSize(const QSize& s){d_size = s;}
	//! Enables the Device-Independent Screen Preview
	void setPreviewPixmap(const QPixmap &, int = 1);
	//! Set creator information
	void setCreator(const QString&);

protected:
	virtual int metric(PaintDeviceMetric) const;

private:
	//! Size in pixels
	QSize d_size;
	EpsPaintEngine* engine;
};

class EpsPaintEngine : public QPaintEngine
{
public:
	EpsPaintEngine(const QString&);
	~EpsPaintEngine(){};
	virtual bool begin(QPaintDevice*);
	virtual bool end();
	virtual void updateState(const QPaintEngineState &){};
	virtual void drawEllipse(const QRectF &);
	virtual QPaintEngine::Type type() const {return QPaintEngine::User;};
	virtual void drawPoints(const QPointF *, int);
	virtual void drawLines(const QLineF * , int);
	virtual void drawPath(const QPainterPath&);
	virtual void drawPolygon(const QPointF *, int, PolygonDrawMode);
	virtual void drawTextItem(const QPointF &, const QTextItem &);
	virtual void drawRects(const QRectF *, int);
	virtual void drawPixmap(const QRectF &, const QPixmap &, const QRectF &);
	virtual void drawTiledPixmap(const QRectF&, const QPixmap&, const QPointF&);
	virtual void drawImage(const QRectF &, const QImage &, const QRectF &, Qt::ImageConversionFlags);

	//! Enables/Disables gray scale output
	void setGrayScale(bool on = true){d_gray_scale = on;}
	//! Enables the Device-Independent Screen Preview
	void setPreviewPixmap(const QPixmap&, int = 1);
	//! Set creator information
	void setCreator(const QString&s){d_creator_name = s;}

private:
	enum Shape{Line, Polygon, Polyline, Rect, Ellipse, Path, Points};
	//! Returns true if draw operation has NoBrush and NoPen
	bool emptyStringOperation();
	double resFactorX();
	double resFactorY();

	QPointF convertPoint(const QPointF&);

	QString psBrush(const QBrush&, const QPainterPath& = QPainterPath());
	QString psColor(const QColor&);
	QString psFont(const QFont&);
	QString psPath(const QPainterPath&);
	QString psPen(const QPen&);
	QString psPoint(const QPointF&);

	QString drawShape(Shape, const QPainterPath&);
	QString drawShape(Shape, const QString&);

	//! Draws pixmap pix in a given rectangle
	void drawPixmap(const QPixmap &, const QRectF &, const QString& = "");
	void writeToFile(const QString& s);
	void writeImageData(const QImage&, int, int, bool = false);
	void writePreview();

	static QString psRencode();
	static QString patternStringStartHelper(int, int, int, int);
	static QString patternStringEndHelper();
	static QString patternProcString(const Qt::BrushStyle&);
	QString patternString(const QBrush&);
	static QString escapeParentheses(const QString&);
	static QString replaceUnicodeCharacters(const QString&, bool *, bool *);
	static QString replaceGlyphs(const QString&);
	static QString replaceGlyph(const QString&, const QString&, int);
	static QString replaceSymbols(const QString&, const QString&, double);
	static QString replaceSymbol(const QString&, int, const QString&, const QString&, double);
	static int dingbatOctalCode(const QString&);
	static int dingbatOctalCode(int);
	void drawDingbatPixmap(const QString&, const QFont&, const QPen&, const QRectF&);

	QString replaceDingbat(const QString&, int, const QString&, const QString&, double);
	QString replaceDingbats(const QString&, const QString&, double);

	QFile *file;
	//! Name of the output file
	QString fname;
	bool d_gray_scale;
	QList<QBrush> d_brushes;
	QBrush d_current_brush;
	QPainterPath d_current_path;
	QStringList d_reencoded_fonts, d_dingbat_symbols;
	QFont d_current_font;
	QPixmap d_preview_pix;
	int d_preview_compression;
	QString d_creator_name;
};
#endif
