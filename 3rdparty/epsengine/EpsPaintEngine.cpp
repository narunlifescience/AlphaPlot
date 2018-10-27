/***************************************************************************
File                 : EpsPaintEngine.cpp
Project              : EpsEngine
--------------------------------------------------------------------
Copyright            : (C) 2014 by Ion Vasilief
Email (use @ for *)  : ion_vasilief*yahoo.fr
Description          : Enables the export of QPainter grafics to .eps files
***************************************************************************/
#include "EpsEngine.h"
#include <math.h>
#include <QDate>
#include <QFile>
#include <QFileInfo>
#include <QImageWriter>
#include <QPixmap>
#include <QMap>
#include <QTextStream>
#include <QBuffer>

#ifdef Q_OS_WIN
	#define M_PI 3.14159265358979323846
#endif

EpsPaintEngine::EpsPaintEngine(const QString& f)
: QPaintEngine(QPaintEngine::AllFeatures),
fname(f),
d_gray_scale(false),
d_current_brush(QBrush()),
d_current_path(QPainterPath()),
d_current_font(QFont("Helvetica")),
d_preview_pix(QPixmap()),
d_preview_compression(1),
d_creator_name("")
{}

bool EpsPaintEngine::begin(QPaintDevice* p)
{
	setPaintDevice(p);
	file = new QFile(fname);
	if (file->open(QIODevice::WriteOnly)){
		QTextStream t(file);
		t.setCodec("UTF-8");

		t << "%!PS-Adobe-3.0 EPSF-3.0\n";
		t << "%%BoundingBox: 0 0 ";
		t << QString::number(qRound(resFactorX()*p->width())) + " ";
		t << QString::number(qRound(resFactorY()*p->height())) + "\n";
		t << "%%Pages: 0\n";
		if (!d_creator_name.isEmpty()){
			t << "%%Creator: ";
			t << d_creator_name + "\n";
		}
		t << "%%CreationDate: " + QDate::currentDate().toString(Qt::ISODate) + "\n";
		t << "%%Title: " + fname + "\n";
		t << "%%BeginProlog\n";
		t << "/bd {bind def} bind def\n";
		t << "/c {curveto} bd\n";
		t << "/f {findfont} bd\n";
		t << "/l {lineto} bd\n";
		t << "/m {moveto} bd\n";
		t << "/r {rotate} bd\n";
		t << "/s {show} bd\n";
		t << "/cp {closepath clip} bd\n";
		t << "/fl {fill} bd\n";
		t << "/rf {rectfill} bd\n";
		t << "/np {newpath} bd\n";
		t << "/lw {setlinewidth} bd\n";
		t << "/lc {setlinecap} bd\n";
		t << "/lj {setlinejoin} bd\n";
		t << "/ml {setmiterlimit} bd\n";
		t << "/ld {setdash} bd\n";
		t << "/gl {glyphshow} bd\n";
		t << "/gs {gsave} bd\n";
		t << "/gr {grestore} bd\n";
		t << "/st {stroke} bd\n";
		t << "/sf {scalefont setfont} bd\n";
		t << "/tr {translate} bd\n";
		t << "/sw {stringwidth pop neg 0 rmoveto} bind def\n";
		if (d_gray_scale)
			t << "/col {setgray} bd\n";
		else
			t << "/col {setrgbcolor} bd\n";
		t << "%%EndProlog\n";
		return true;
	}

	delete file;
	return false;
}

bool EpsPaintEngine::end()
{
	QTextStream t(file);
	t.setCodec("UTF-8");
	t << "%%EOF";
	file->close();

	if (!d_preview_pix.isNull())
		writePreview();

	return true;
}

void EpsPaintEngine::drawPoints(const QPointF* points, int pointCount)
{
	if (emptyStringOperation())
		return;

	QTransform m = painter()->combinedTransform();
	QString lw = QString::number(painter()->pen().widthF()*resFactorX());
	QString s = psColor(painter()->pen().color());
	s += "[";
	for (int i = 0; i < pointCount; i++){
		QPointF p = m.map(points[i]);
		s += psPoint(convertPoint(p)) + " " + lw + " " + lw;
		if (i < pointCount - 1)
			s += " ";
	}
	s += "] rf\n";
	writeToFile(s);
}

void EpsPaintEngine::drawLines(const QLineF* lines, int lineCount)
{
	if (painter()->pen().style() == Qt::NoPen)
		return;

	QString s;
	for (int i = 0; i < lineCount; i++){
		QPainterPath path(lines[i].p1());
		path.lineTo(lines[i].p2());

		s += drawShape(Line, path);
	}

	writeToFile(s);
}

void EpsPaintEngine::drawPolygon(const QPointF* points, int pointCount, PolygonDrawMode mode)
{
	if (emptyStringOperation())
		return;

	QVector<QPointF> pts;
	for (int i = 0; i < pointCount; i++)
		pts << points[i];

	QPainterPath path;
	path.addPolygon(QPolygonF(pts));
	if (mode != QPaintEngine::PolylineMode)
		path.closeSubpath ();

	QString s;
	if (mode != QPaintEngine::PolylineMode){
		path.closeSubpath ();
		s += drawShape(Polygon, path);
	} else
		s += drawShape(Polyline, path);

	writeToFile(s);
}

QString EpsPaintEngine::psFont(const QFont& font)
{
	QString name = "Helvetica", family = font.family();
	bool italic = font.italic(), bold = (font.weight() >= QFont::DemiBold);
	if (family.contains("Times")){
		name = "Times-Roman";
		if (bold && italic)
			name = "Times-BoldItalic";
		else if (italic)
			name = "Times-Italic";
		else if (bold)
			name = "Times-Bold";
	} else if (family.contains("Courier")){
		name = "Courier";
		if (bold && italic)
			name = "Courier-BoldOblique";
		else if (italic)
			name = "Courier-Oblique";
		else if (bold)
			name = "Courier-Bold";
	} else if (family.contains("Bookman")){
		name = "Bookman-Light";
		if (bold && italic)
			name = "Bookman-DemiItalic";
		else if (italic)
			name = "Bookman-LightItalic";
		else if (bold)
			name = "Bookman-Demi";
	} else if (family.contains("Century") || family.contains("Schoolbook")){
		name = "NewCenturySchlbk-Roman";
		if (bold && italic)
			name = "NewCenturySchlbk-BoldItalic";
		else if (italic)
			name = "NewCenturySchlbk-Italic";
		else if (bold)
			name = "NewCenturySchlbk-Bold";
	} else if (family == "Palatino"){
		name = "Palatino-Roman";
		if (bold && italic)
			name = "Palatino-BoldItalic";
		else if (italic)
			name = "Palatino-Italic";
		else if (bold)
			name = "Palatino-Bold";
	} else if (font.family().contains("Symbol")){
		name = "Symbol";
	} else {
		if (bold && italic)
			name = "Helvetica-BoldOblique";
		else if (italic)
			name = "Helvetica-Oblique";
		else if (bold)
			name = "Helvetica-Bold";
	}
	return name;
}

QString EpsPaintEngine::psRencode()
{
	return "/ReEncode { % inFont outFont encoding | -\n\
 /MyEncoding exch def\n\
 exch findfont\n\
 dup length dict\n\
 begin\n\
	{def} forall\n\
	/Encoding MyEncoding def\n\
	currentdict\n\
 end\n\
 definefont\n\
} def\n";
}

QString EpsPaintEngine::replaceGlyphs(const QString& text)
{
	QMap<int, QString> glyphs;
	//glyphs.insert(0x00B7, "middot");
	//glyphs.insert(0x00D7, "multiply");
	glyphs.insert(0x0102, "Abreve");
	glyphs.insert(0x0103, "abreve");
	glyphs.insert(0x0152, "OE");
	glyphs.insert(0x0153, "oe");
	glyphs.insert(0x015e, "Scedilla");
	glyphs.insert(0x015f, "scedilla");
	glyphs.insert(0x0162, "Tcedilla");
	glyphs.insert(0x0163, "tcedilla");
	glyphs.insert(0x2030, "perthousand");
	glyphs.insert(0x210F, "hbar");
	glyphs.insert(0x212B, "Aring");
	glyphs.insert(0x221E, "infinity");

	QString s = text;
	QMapIterator<int, QString> i(glyphs);
	while (i.hasNext()){
		i.next();
		s = replaceGlyph(s, i.value(), i.key());
	}

	return s;
}

QString EpsPaintEngine::replaceGlyph(const QString& text, const QString& name, int unicode)
{
	QChar symbol = QChar(unicode);
	if (!text.contains(symbol))
		return text;

	QStringList strings = QString(text).split(symbol, QString::KeepEmptyParts);
	int size = strings.size();
	QString s = strings.first();
	for (int i = 1; i < size; i++)
		s.append(") s\n/" + name + " gl\n(" + strings.at(i));

	return s;
}

QString EpsPaintEngine::replaceSymbols(const QString& text, const QString& fontName, double fontSize)
{
	if (fontName == "Symbol")
		return text;

	QMap<int, QString> glyphs;
	//double arrows
	glyphs.insert(0x21D0, "334");
	glyphs.insert(0x21D1, "335");
	glyphs.insert(0x21D2, "336");
	glyphs.insert(0x21D3, "337");
	glyphs.insert(0x21D4, "333");

	//Uppercase Greek letters
	glyphs.insert(0x393, "107");
	glyphs.insert(0x394, "104");
	glyphs.insert(0x398, "121");
	glyphs.insert(0x39B, "114");
	glyphs.insert(0x39E, "130");
	glyphs.insert(0x39F, "117");//omicron
	glyphs.insert(0x3A0, "120");
	glyphs.insert(0x3A3, "123");
	glyphs.insert(0x3A6, "106");
	glyphs.insert(0x3A8, "131");
	glyphs.insert(0x3A9, "127");

	//Lowercase Greek letters
	glyphs.insert(0x3B1, "141");
	glyphs.insert(0x3B2, "142");
	glyphs.insert(0x3B3, "147");
	glyphs.insert(0x3B4, "144");
	glyphs.insert(0x3B5, "145");
	glyphs.insert(0x3B6, "172");
	glyphs.insert(0x3B7, "150");
	glyphs.insert(0x3B8, "161");
	glyphs.insert(0x3B9, "151");
	glyphs.insert(0x3BA, "153");
	glyphs.insert(0x3BB, "154");
	glyphs.insert(0x3BC, "155");
	glyphs.insert(0x3BD, "156");
	glyphs.insert(0x3BE, "170");
	glyphs.insert(0x3BF, "157");//omicron
	glyphs.insert(0x3C0, "160");
	glyphs.insert(0x3C1, "162");
	glyphs.insert(0x3C2, "126");
	glyphs.insert(0x3C3, "163");
	glyphs.insert(0x3C4, "164");
	glyphs.insert(0x3C5, "165");
	glyphs.insert(0x3C6, "152");
	glyphs.insert(0x3C7, "143");
	glyphs.insert(0x3C8, "171");
	glyphs.insert(0x3C9, "167");

	glyphs.insert(0x2200, "042");
	glyphs.insert(0x2202, "266");
	glyphs.insert(0x2203, "044");
	glyphs.insert(0x2205, "306");
	glyphs.insert(0x2207, "321");
	glyphs.insert(0x2208, "316");
	glyphs.insert(0x2209, "317");
	glyphs.insert(0x220D, "047");
	glyphs.insert(0x2242, "273");
	glyphs.insert(0x2245, "100");
	glyphs.insert(0x22A5, "136");
	glyphs.insert(0x2260, "271");
	glyphs.insert(0x2261, "272");
	glyphs.insert(0x2264, "243");
	glyphs.insert(0x2265, "263");
	glyphs.insert(0x2282, "314");
	glyphs.insert(0x2283, "311");
	glyphs.insert(0x2284, "313");
	glyphs.insert(0x2286, "315");
	glyphs.insert(0x2287, "312");
	glyphs.insert(0x22EF, "274");
	glyphs.insert(0x221D, "265");
	glyphs.insert(0x2220, "320");
	glyphs.insert(0x222B, "362");
	glyphs.insert(0x2234, "134");
	glyphs.insert(0x2295, "305");
	glyphs.insert(0x2297, "304");
	glyphs.insert(0x221A, "326");
	glyphs.insert(0x22C4, "340");
	glyphs.insert(0x2227, "331");
	glyphs.insert(0x2228, "332");
	glyphs.insert(0x2229, "307");
	glyphs.insert(0x222A, "310");

	QString s = text;
	QMapIterator<int, QString> i(glyphs);
	while (i.hasNext()){
		i.next();
		s = replaceSymbol(s, i.key(), i.value(), fontName, fontSize);
	}

	return s;
}

QString EpsPaintEngine::replaceSymbol(const QString& text, int unicode, const QString& octalCode, const QString& fontName, double fontSize)
{
	QString symbol = QChar(unicode);
	if (!text.contains(symbol))
		return text;

	QStringList strings = QString(text).split(symbol, QString::KeepEmptyParts);
	QString s = strings.first(), ssize = QString::number(fontSize);
	int size = strings.size();
	for (int i = 1; i < size; i++){
		s.append(") s\n");
		s.append("/Symbol f ");
		s.append(ssize + " sf\n(\\" + octalCode + ") s\n");
		s.append("/" + fontName + " f " + ssize + " sf\n(");
		s.append(strings.at(i));
	}
	return s;
}

int EpsPaintEngine::dingbatOctalCode(int unicode)
{
	bool arrow = (unicode >= 0x2798);
	int du = unicode - (arrow ? 0x2798 : 0x2700), q = du/8, offset = du % 8;
	int dec = arrow ? 3 + q : 4 + q;

	return arrow ? 300 + (dec % 8)*10 + offset : (dec/8)*100 + (dec % 8)*10 + offset;
}

QString EpsPaintEngine::replaceDingbats(const QString& text, const QString& fontName, double fontSize)
{
	if (fontName == "ZapfDingbats")
		return text;

	QString s = text;

	s = replaceDingbat(s, 0x2756, QString::number(166), fontName, fontSize);

	for (int i = 0x2701; i <= 0x274B; i++)
		s = replaceDingbat(s, i, QString::number(dingbatOctalCode(i)), fontName, fontSize);

	for (int i = 0x2762; i <= 0x2767; i++)
		s = replaceDingbat(s, i, QString::number(242 + i - 0x2762), fontName, fontSize);

	s = replaceDingbat(s, 0x2794, QString::number(324), fontName, fontSize);

	for (int i = 0x2798; i <= 0x27BE; i++)
		s = replaceDingbat(s, i, QString::number(dingbatOctalCode(i)), fontName, fontSize);

	return s;
}

QString EpsPaintEngine::replaceDingbat(const QString& text, int unicode, const QString& octalCode, const QString& fontName, double fontSize)
{
	QString symbol = QChar(unicode);
	if (!text.contains(symbol))
		return text;

	QStringList strings = QString(text).split(symbol, QString::KeepEmptyParts);
	QString s = strings.first(), ssize = QString::number(fontSize);
	int size = strings.size();
	for (int i = 1; i < size; i++){
		s.append(") s\n");
		s.append("/ZapfDingbats f ");
		s.append(ssize + " sf\n(\\" + octalCode + ") s\n");
		s.append("/" + fontName + " f " + ssize + " sf\n(");
		s.append(strings.at(i));
	}
	return s;
}

QString EpsPaintEngine::escapeParentheses(const QString& s)
{
	QString text = QString(s);

	text.replace("(", "\\(");
	text.replace(")", "\\)");

	return text;
}

QString EpsPaintEngine::replaceUnicodeCharacters(const QString& s, bool *reencode, bool *rmove)
{
	QString text = QString(s);

	QChar minus = QChar(0x2212);
	if (text.contains(minus)){
		text.replace(minus, "\\055");
		*reencode = true;
		*rmove = true;
	}

	text.replace(QChar(0x2219), QChar(0x00B7));//bullet operator

	if (text.contains(QChar(0x00B7)) || text.contains(QChar(0x00D7)))
		*rmove = true;

	int size = 7, unicode = 0x00C0;
	for (int j = 0; j <= size; j++){
		for (int i = 0; i <= size; i++){
			QChar ch = QChar(unicode + i);
			if (text.contains(ch)){
				text.replace(ch, "\\3" + QString::number(j) + QString::number(i));
				*reencode = true;
			}
		}
		unicode += 8;
	}

	unicode = 0x00A0;
	for (int j = 4; j <= size; j++){
		for (int i = 0; i <= size; i++){
			QChar ch = QChar(unicode + i);
			if (text.contains(ch)){
				text.replace(ch, "\\2" + QString::number(j) + QString::number(i));
				*reencode = true;
			}
		}
		unicode += 8;
	}

	return text;
}

int EpsPaintEngine::dingbatOctalCode(const QString& s)
{
	int unicode = s.unicode()->unicode();
	if (unicode == 0x2756)
		return 166;
	else if (unicode > 0x2700 && unicode <= 0x274B)
		return dingbatOctalCode(unicode);
	else if (unicode >= 0x2762 && unicode <= 0x2767)
		return 242 + unicode - 0x2762;
	else if ((unicode >= 0x25A2 && unicode <= (0x25A2 + 12)) ||
		(unicode >= 0x25AF && unicode <= 0x25B1) ||
		(unicode >= 0x25C8 && unicode <= 0x25CE) ||
		(unicode >= 0x25D0 && unicode <= 0x25D9) ||
		(unicode >= 0x25E2 && unicode <= 0x25EE) ||
		(unicode >= 0x25F0 && unicode <= 0x25FF))
		return -1;

	return 0;
}

void EpsPaintEngine::drawDingbatPixmap(const QString& letter, const QFont& f, const QPen& pen, const QRectF& r)
{
	int w = qRound(r.width()), h = qRound(r.height());
	QPixmap pix = QPixmap(w, h);
	pix.fill(QColor(Qt::transparent));

	QPainter p(&pix);
	p.setFont(f);
	p.setPen(pen);
	p.drawText(QRect(0, 0, w, h), letter, QTextOption(Qt::AlignCenter));
	p.end();

	QString name = "u" + QString::number(letter.unicode()->unicode()) + "-" + QString::number(w);
	if (!d_dingbat_symbols.contains(name)){
		d_dingbat_symbols.append(name);

		QTextStream t(file);
		t.setCodec("UTF-8");
		t << "/" + name + "\n";
		t << "currentfile\n<< /Filter /ASCII85Decode >>\n/ReusableStreamDecode filter\n";

		t.flush();
		writeImageData(pix.toImage(), w, h);

		t << " def\n";
		t.flush();
	}

	drawPixmap(pix, r, name);
}

void EpsPaintEngine::drawTextItem(const QPointF& p, const QTextItem& textItem)
{
	if (painter()->hasClipping() && !painter()->clipPath().boundingRect().toRect().contains(p.toPoint()))
		return;

	QString text = textItem.text();
	QFont f = textItem.font();
	QPen pen = painter()->pen();

	int dingbat = (text.size() == 1) ? dingbatOctalCode(text) : 0;
	if (dingbat == -1){
		int w = qRound(textItem.width()) + 1, h = w + 2;
		drawDingbatPixmap(text, f, pen, QRectF(p.x(), p.y() - h*resFactorY(), w, h));
		return;
	}

	bool reencode = false, rmove = false;
	QString name, s = psColor(pen.color());
	if (dingbat){
		text = (dingbat < 100) ? "\\0" : "\\";
		text += QString::number(dingbat);
		name = "ZapfDingbats";
	} else {
		text = escapeParentheses(text);
		text = replaceUnicodeCharacters(text, &reencode, &rmove);

		name = psFont(f);
		if (reencode){
			if (d_reencoded_fonts.isEmpty())
				s += psRencode();

			QString rname = QString(name + "-Latin1");
			if (!d_reencoded_fonts.contains(rname)){
				d_reencoded_fonts.append(rname);
				s += "/" + name + " /" + rname + " ISOLatin1Encoding ReEncode\n";
			}
			name = rname;
		}
	}

	QTransform m = painter()->combinedTransform();

	double fntSize = f.pointSizeF();
	if (fntSize == -1){
		fntSize = resFactorY()*f.pixelSize();
		if (m.isScaling())
			fntSize *= m.m22();//multiply font size by vertical scaling factor
		//printf("%s fntSize: %f size: %d isScaling: %d %f\n", text.toLatin1().constData(), fntSize, size, m.isScaling(), m.m22());
	}

	if (f != d_current_font || reencode || dingbat){
		d_current_font = f;

		s += "/" + name + " f ";
		s += QString::number(fntSize) + " sf\n";
	}

	//if (!rmove){
		text = replaceGlyphs(text);
		text = replaceSymbols(text, name, fntSize);
		text = replaceDingbats(text, name, fntSize);
	//}

	QPointF o = convertPoint(m.map(p));
	if (rmove)
		o.setX(o.x() + textItem.width()*resFactorX());

	s += psPoint(o) + " m";

	if (painter()->transform().isRotating()){
		s += "\ngs\n";
		if (rmove)
			s += "(" + text + ") sw\n";

		double angle = 180.0/M_PI*acos(m.m11());
		if (m.m11() != 0.0 && m.m12() > 0)
			angle = -angle;

		s += QString::number(angle) + " r\n(" + text + ") s\ngr\n";
	} else if (rmove)
		s += " (" + text + ") dup sw s\n";
	else
		s += " (" + text + ") s\n";

	writeToFile(s);
}

void EpsPaintEngine::drawRects(const QRectF* rects, int rectCount)
{
	if (emptyStringOperation())
		return;

	QString s;
	for (int i = 0; i < rectCount; i++){
		QPainterPath path;
		path.addPolygon(QPolygonF(rects[i]));
		s += drawShape(Path, path);
	}

	writeToFile(s);
}

void EpsPaintEngine::drawEllipse(const QRectF& rect)
{
	if (emptyStringOperation())
		return;

	QPointF p = painter()->combinedTransform().map(rect.bottomLeft());

	double xrad = 0.5*rect.width(), yrad = 0.5*rect.height(), scale = yrad/xrad;
	QPointF center = convertPoint(QPointF(p.x() + xrad, p.y() - yrad));

	QString path = "np ";
	if (scale != 1.0){
		path += QString::number(center.x()) + " " + QString::number(center.y()) + " tr\n";
		path += "1 " + QString::number(scale) + " scale\n";
		path += "0 0 " + QString::number(xrad*resFactorX()) + " 0 360 arc\n";
		path += "1 " + QString::number(1.0/scale) + " scale\n";
		path += QString::number(-center.x()) + " " + QString::number(-center.y()) + " tr\n";
	} else {
		path += QString::number(center.x()) + " " + QString::number(center.y()) + " ";
		path += QString::number(xrad*resFactorX()) + " 0 360 arc\n";
	}
	writeToFile(drawShape(Ellipse, path));
}

void EpsPaintEngine::drawPath(const QPainterPath& path)
{
	if (emptyStringOperation())
		return;

	writeToFile(drawShape(Path, path));
}

QString EpsPaintEngine::drawShape(Shape shape, const QString& path)
{
	QString s = QString::null;
	if (path.isEmpty())
		return s;

	QBrush br = painter()->brush();
	if (shape != Line && shape != Polyline && br.style() != Qt::NoBrush){// fill the background
		QString brush = psBrush(br);
		if (!brush.isEmpty())
			s += path + brush + "fl\n";
	}

	QPen pen = painter()->pen();
	if (pen.style() != Qt::NoPen)// draw the contour
		s += psPen(pen) + path + "st\n";

	return s;
}

QString EpsPaintEngine::drawShape(Shape shape, const QPainterPath& ppath)
{
	QString s = QString::null;
	if (ppath.isEmpty())
		return s;

	QString path = psPath(ppath);
	if (path.isEmpty())
		return s;

	QBrush br = painter()->brush();
	if (shape != Line && shape != Polyline && br.style() != Qt::NoBrush){// fill the background
		QString brush = psBrush(br, ppath);
		if (!brush.isEmpty())
			s += path + brush + "fl\n";
	}

	QPen pen = painter()->pen();
	if (pen.style() != Qt::NoPen)// draw the contour
		s += psPen(pen) + path + "st\n";

	return s;
}

void EpsPaintEngine::drawImage(const QRectF& r, const QImage &image, const QRectF& sr, Qt::ImageConversionFlags flags)
{
	drawPixmap(QPixmap::fromImage(image, flags).copy(sr.toAlignedRect()), r);
}

void EpsPaintEngine::drawPixmap(const QRectF &r, const QPixmap &pm, const QRectF &sr)
{
	drawPixmap(pm.copy(sr.toAlignedRect()), r);
}

void EpsPaintEngine::drawPixmap(const QPixmap &pix, const QRectF &r, const QString& def)
{
	bool tile = !def.isEmpty();

	QTextStream t(file);
	t.setCodec("UTF-8");

	QTransform m = painter()->combinedTransform();
	QRectF tr = m.mapRect(r);
	QPointF orig = convertPoint(tr.bottomLeft());
	int x = qRound(orig.x()), y = qRound(orig.y()), w = qRound(r.width()), h = qRound(r.height());
	bool alpha = pix.hasAlpha();//true;

	t << "gs\n";
	t << QString::number(x) + " " + QString::number(y) + " tr\n";
	t << QString::number(w*resFactorX()) + " " + QString::number(h*resFactorY()) + " scale\n";

	if (painter()->transform().isRotating()){
		double angle = 180.0/M_PI*acos(m.m11());
		if (m.m11() != 0.0 && m.m12() > 0)
			angle = -angle;
		t << QString::number(angle) + " r\n";
	}

	if (tile)
		t << QString(def +	" 0 setfileposition\n");
	t << "/Device" + QString(d_gray_scale ? "Gray" : "RGB") + " setcolorspace\n<<\n  /ImageType ";
	t << (alpha ? "4\n" : "1\n");

	if (alpha)
		t << (d_gray_scale ? "  /MaskColor [255]\n" : "  /MaskColor [255 255 255]\n");

	QImage image = pix.toImage();
	bool encodeDCT = (image.depth() != 1 && !d_gray_scale && QImageWriter::supportedImageFormats().contains("jpeg") && (d_creator_name != "QtiPlot"));
	t << "  /Width " + QString::number(w) + "\n";
	t << "  /Height " + QString::number(h) + "\n";
	t << "  /BitsPerComponent 8\n";
	t << (d_gray_scale ? "  /Decode [0 1]\n" : "  /Decode [0 1 0 1 0 1]\n");
	t << "  /ImageMatrix [" + QString::number(w) + " 0 0 -" + QString::number(h) + " 0 " + QString::number(h) + "]\n";
	t << "  /DataSource ";
	if (tile)
		t << QString(def);
	else {
		t << "currentfile /ASCII85Decode filter";
		if (encodeDCT)
			t << " /DCTDecode filter";
	}
	t << "\n>> image\n";

	if (!tile){
		t.flush();
		writeImageData(image, w, h, encodeDCT);
		t << "\n";
	}

	t << "gr\n";
}

void EpsPaintEngine::drawTiledPixmap(const QRectF& r, const QPixmap& pix, const QPointF& p)
{
	int w = pix.width(), h = pix.height();

	QString def = "tile";
	QTextStream t(file);
	t.setCodec("UTF-8");
	t << "/" + def + "\n";
	t << "currentfile\n<< /Filter /ASCII85Decode >>\n/ReusableStreamDecode filter\n";

	t.flush();
	writeImageData(pix.toImage(), w, h);

	t << " def\n";
	t.flush();

	QSizeF sz = QSizeF(w, h);
	int rx = ceil(r.width()/w), ry = ceil(r.height()/h);
	double x0 = r.x() + p.x(), y0 = r.y() + p.y();
	for (int i = 0; i < ry; i++){
		double y = y0 + i*h;
		for (int j = 0; j < rx; j++){
			QRectF dr = QRectF(QPointF(x0 + j*w, y), sz);
			drawPixmap(pix, dr, def);
		}
	}
}

QByteArray ascii85Encode(const QByteArray &input)
{
	int isize = input.size()/4*4;
	QByteArray output;
	output.resize(input.size()*5/4+7);
	char *out = output.data();
	const uchar *in = (const uchar *)input.constData();
	for (int i = 0; i < isize; i += 4) {
		uint val = (((uint)in[i])<<24) + (((uint)in[i+1])<<16) + (((uint)in[i+2])<<8) + (uint)in[i+3];
		if (val == 0) {
			*out = 'z';
			++out;
		} else {
			char base[5];
			base[4] = val % 85;
			val /= 85;
			base[3] = val % 85;
			val /= 85;
			base[2] = val % 85;
			val /= 85;
			base[1] = val % 85;
			val /= 85;
			base[0] = val % 85;
			*(out++) = base[0] + '!';
			*(out++) = base[1] + '!';
			*(out++) = base[2] + '!';
			*(out++) = base[3] + '!';
			*(out++) = base[4] + '!';
		}
	}
	//write the last few bytes
	int remaining = input.size() - isize;
	if (remaining) {
		uint val = 0;
		for (int i = isize; i < input.size(); ++i)
			val = (val << 8) + in[i];
		val <<= 8*(4-remaining);
		char base[5];
		base[4] = val % 85;
		val /= 85;
		base[3] = val % 85;
		val /= 85;
		base[2] = val % 85;
		val /= 85;
		base[1] = val % 85;
		val /= 85;
		base[0] = val % 85;
		for (int i = 0; i < remaining+1; ++i)
			*(out++) = base[i] + '!';
	}
	*(out++) = '~';
	*(out++) = '>';
	output.resize(out-output.data());
	return output;
}

void EpsPaintEngine::writeImageData(const QImage& img, int w, int h, bool encodeDCT)
{
	QImage image = img;
	QTextStream t(file);
	QByteArray pixelData;
	if (encodeDCT){
		if (image.hasAlphaChannel()){
			QRgb white = qRgb(255, 255, 255);
			for (int y = 0; y < h; y++){
				QRgb * s = (QRgb*)(image.scanLine(y));
				for(int x = 0; x < w; x++){
					QRgb rgb = (*s++);
					if (qAlpha(rgb) < 255)
						image.setPixel(x, y, white);
				}
			}
		}
		QBuffer buffer(&pixelData);
		QImageWriter writer(&buffer, "jpeg");
		writer.setQuality(100);
		writer.write(image);
	} else {
		int size = w*h;
		if (!d_gray_scale)
			size *= 3;
		pixelData.resize(size);
		uchar *pixel = (uchar *)pixelData.data();
		int i = 0;
		if (d_gray_scale){
			for (int y = 0; y < h; y++){
				QRgb * s = (QRgb*)(image.scanLine(y));
				for(int x = 0; x < w; x++){
					QRgb rgb = (*s++);
					pixel[i++] = (qAlpha(rgb) < 255) ? 255 : (unsigned char) qGray(rgb);
				}
			}
		} else {
			for(int y = 0; y < h; y++){
				QRgb * s = (QRgb*)(image.scanLine(y));
				for(int x = 0; x < w; x++){
					QRgb rgb = (*s++);
					bool hasAlpha = (qAlpha(rgb) < 255);
					pixel[i] = hasAlpha ? 255 : (unsigned char) qRed(rgb);
					pixel[i + 1] = hasAlpha ? 255 : (unsigned char) qGreen(rgb);
					pixel[i + 2] = hasAlpha ? 255 : (unsigned char) qBlue(rgb);
					i += 3;
				}
			}
		}
	}
	t << ascii85Encode(pixelData);
	t.flush();
}

void EpsPaintEngine::setPreviewPixmap(const QPixmap& pix, int compression)
{
	d_preview_pix = pix;
	d_preview_compression = compression;
}

void EpsPaintEngine::writePreview()
{
	QString previewName = fname + ".tiff", tempName = previewName + "2.eps";

	if (d_gray_scale){
		QImage image = d_preview_pix.toImage();
		int width = image.width(), height = image.height();
		for (int i = 0; i < width; ++i){
			for (int j = 0; j < height; ++j){
				int gray = qGray(image.pixel(i, j));
				image.setPixel(i, j, qRgb(gray, gray, gray));
			}
		}

		if (d_preview_compression){
			QImageWriter writer(previewName);
			writer.setCompression(d_preview_compression);
			writer.write(image);
		} else
			image.save(previewName);
	} else if (d_preview_compression){
		QImageWriter writer(previewName);
		writer.setCompression(d_preview_compression);
		writer.write(d_preview_pix.toImage());
	} else
		d_preview_pix.save(previewName);

	QFile *tempFile = new QFile(tempName);
	if (tempFile->open(QIODevice::Append)){
		tempFile->write(QByteArray::fromHex("C5D0D3C6").data());

		QDataStream ds(tempFile);

		short word = 0x4321;
		bool bigEndian = (*(char*)& word) != 0x21;
		if (!bigEndian)
			ds.setByteOrder(QDataStream::LittleEndian);

		ds << (qint32)30;

		qint32 psSize = QFileInfo(fname).size();
		ds << (qint32)psSize;
		ds << (qint32)0;
		ds << (qint32)0;
		ds << (qint32)(psSize + 30);
		ds << (qint32)QFileInfo(previewName).size();

		tempFile->write(QByteArray::fromHex("FFFF").data());

		file = new QFile(fname);
		if (file->open(QIODevice::ReadOnly)){
			tempFile->write(file->readAll());
			file->remove();
		}

		file = new QFile(previewName);
		if (file->open(QIODevice::ReadOnly)){
			tempFile->write(file->readAll());
			file->remove();
		}

		tempFile->close();
		QFile::rename(tempName, fname);
	}
}

QString EpsPaintEngine::patternStringStartHelper(int x, int width, int dx, int dy)
{
	QString s, sx = QString::number(x), sw = QString::number(width), sdx = QString::number(dx), sdy = QString::number(dy);
	s += "<< /PatternType 1\n";
	s += "/PaintType 1\n";
	s += "/TilingType 1\n";
	s += "/BBox [" + sx + " " + sx + " " + sw + " " + sw + "]\n";
	s += "/XStep " + sdx + "\n";
	s += "/YStep " + sdy + "\n";
	s += "/PaintProc {\n";
	return s;
}

QString EpsPaintEngine::patternStringEndHelper()
{
	QString s = "0.1 lw [] 0 ld st\n";
	return s;
}

QString EpsPaintEngine::patternProcString(const Qt::BrushStyle& style)
{
	QString s;
#ifdef Q_OS_MAC
	switch (style){
		case Qt::Dense1Pattern:
			s += "[0 0 6 6] rf\n";
			s += "1 1 1 col\n";
			s += "[0 0 1 1] rf\n";
		break;
		case Qt::Dense2Pattern:
			s += "[0 0 6 6] rf\n";
			s += "1 1 1 col\n";
			s += "[0 0 1 1 2 2 1 1] rf\n";
		break;
		case Qt::Dense3Pattern:
			s += "[0 0 4 4] rf\n";
			s += "1 1 1 col\n";
			s += "[0 0 1 1 2 2 1 1 1 1 1 1 0 2 1 1 2 0 1 1 3 3 1 1] rf\n";
		break;
		case Qt::Dense4Pattern:
			s += "[0 0 1 1 1 1 1 1] rf\n";
		break;
		case Qt::Dense5Pattern:
			s += "[0 0 1 1 2 2 1 1 1 1 1 1 0 2 1 1 2 0 1 1 3 3 1 1] rf\n";
		break;
		case Qt::Dense6Pattern:
			s += "[0 0 1 1 2 2 1 1] rf\n";
		break;
		case Qt::Dense7Pattern:
			s += "[0 0 1 1] rf\n";
		break;
		case Qt::HorPattern:
			s += "np 0 0 m 10 0 l\n";
		break;
		case Qt::VerPattern:
			s += "np 0 0 m 0 10 l\n";
		break;
		case Qt::CrossPattern:
			s += "np 0 0 m 10 0 l 0 0 m 0 10 l\n";
		break;
		case Qt::FDiagPattern:
			s += "np 0 10 m 10 0 l\n";
		break;
		case Qt::BDiagPattern:
			s += "np 0 0 m 10 10 l\n";
		break;
		case Qt::DiagCrossPattern:
			s += "np 0 10 m 10 0 l 0 0 m 10 10 l\n";
		break;
		default:
			break;
	}
#else
	switch (style){
		case Qt::Dense1Pattern:
			s += "[0 0 3 3] rf\n";
			s += "1 1 1 col\n";
			s += "[0 0 0.75 0.75] rf\n";
		break;
		case Qt::Dense2Pattern:
			s += "[0 0 3 3] rf\n";
			s += "1 1 1 col\n";
			s += "[0 0 0.75 0.75 1.5 1.5 0.75 0.75] rf\n";
		break;
		case Qt::Dense3Pattern:
			s += "[0 0 4 4] rf\n";
			s += "1 1 1 col\n";
			s += "[0 0 1 1 2 2 1 1 1 1 1 1 0 2 1 1 2 0 1 1 3 3 1 1] rf\n";
		break;
		case Qt::Dense4Pattern:
			s += "[0 0 1 1 1 1 1 1] rf\n";
		break;
		case Qt::Dense5Pattern:
			s += "[0 0 1 1 2 2 1 1 1 1 1 1 0 2 1 1 2 0 1 1 3 3 1 1] rf\n";
		break;
		case Qt::Dense6Pattern:
			s += "[0 0 0.75 0.75 1.5 1.5 0.75 0.75] rf\n";
		break;
		case Qt::Dense7Pattern:
			s += "[0 0 0.75 0.75] rf\n";
		break;
		case Qt::HorPattern:
			s += "np 0 0 m 6 0 l\n";
		break;
		case Qt::VerPattern:
			s += "np 0 0 m 0 6 l\n";
		break;
		case Qt::CrossPattern:
			s += "np 0 0 m 6 0 l 0 0 m 0 6 l\n";
		break;
		case Qt::FDiagPattern:
			s += "np 0 6 m 6 0 l\n";
		break;
		case Qt::BDiagPattern:
			s += "np 0 0 m 6 6 l\n";
		break;
		case Qt::DiagCrossPattern:
			s += "np 0 6 m 6 0 l 0 0 m 6 6 l\n";
		break;
		default:
			break;
	}
#endif
	return s;
}

QString EpsPaintEngine::patternString(const QBrush& brush)
{
	Qt::BrushStyle style = brush.style();

#ifdef Q_OS_MAC
	int x = -1, w = 11, dx = 10, dy = 10;
	if (style == Qt::Dense4Pattern){
	   dx = 2; dy = 2; x = 0; w = 2;
	} else if (style == Qt::Dense1Pattern || style == Qt::Dense2Pattern || style == Qt::Dense6Pattern || style == Qt::Dense7Pattern){
		dx = 4; dy = 4; x = 0; w = 6;
	} else if (style == Qt::Dense3Pattern || style == Qt::Dense5Pattern){
		dx = 4; dy = 4; x = 0; w = 4;
	} else if (style == Qt::VerPattern){
		dx = 8; dy = 2; x = 0; w = 10;
	} else if (style == Qt::HorPattern){
		dx = 2; dy = 8; x = 0; w = 10;
	} else if (style == Qt::CrossPattern){
		dx = 8; dy = 8; x = 0; w = 8;
	}
#else
	int x = 0, w = 4, dx = 4, dy = 4;
	if (style == Qt::Dense4Pattern){
	  dx = 2; dy = 2; x = 0; w = 2;
	} else if (style == Qt::Dense1Pattern || style == Qt::Dense2Pattern || style == Qt::Dense6Pattern || style == Qt::Dense7Pattern){
		dx = 3; dy = 3; x = 0; w = 3;
	} else if (style >= Qt::HorPattern){
		dx = 6; dy = 6; x = 0; w = 6;
	}
#endif

	QString name = "pattern" + QString::number(d_brushes.size() + 1);
	QString s = patternStringStartHelper(x, w, dx, dy);
	s += psColor(brush.color());
	s += patternProcString(style);
	if (style >= Qt::HorPattern)
		s += patternStringEndHelper();
	s += "} bind\n>> matrix makepattern /" + name + " exch def\n";
	s += name + " setpattern\n";
	return s;
}

QString EpsPaintEngine::psBrush(const QBrush& brush, const QPainterPath& path)
{
	if (brush.style() == Qt::NoBrush)
		return "";

	if (d_current_path != path){
		if (d_current_path.contains(path) && (brush == d_current_brush))//Same brush and current path contains path in psBrush --> skip filling operation
			return "";
		d_current_path = path;
	}

	if (d_current_brush != brush)
		d_current_brush = brush;

	int index = d_brushes.indexOf(brush);
	if (index >= 0)
		return "pattern" + QString::number(index + 1) + " setpattern\n";

	QString s = "";
	Qt::BrushStyle style = brush.style();
	switch (style){
		case Qt::NoBrush:
		default:
			break;

		case Qt::SolidPattern:
			s = psColor(brush.color());
		break;

		case Qt::Dense1Pattern:
		case Qt::Dense2Pattern:
		case Qt::Dense3Pattern:
		case Qt::Dense4Pattern:
		case Qt::Dense5Pattern:
		case Qt::Dense6Pattern:
		case Qt::Dense7Pattern:
		case Qt::HorPattern:
		case Qt::VerPattern:
		case Qt::CrossPattern:
		case Qt::BDiagPattern:
		case Qt::FDiagPattern:
		case Qt::DiagCrossPattern:
			s += patternString(brush);
		break;

		case Qt::LinearGradientPattern:
		{
			const QLinearGradient *g = (const QLinearGradient *)brush.gradient();

			QGradient::CoordinateMode mode = g->coordinateMode();

			QGradientStops stops = g->stops();
			QColor c0 = stops.first().second, c1 = stops.last().second;
			QPointF sp = g->start(), ep = g->finalStop();
			QPointF p0 = convertPoint(sp), p1 = convertPoint(ep);
			if (mode == QGradient::StretchToDeviceMode){
				//printf("QGradient::StretchToDeviceMode!\n");
				double w = paintDevice()->width(), h = paintDevice()->height();
				p0 = convertPoint(QPointF(w*sp.x(), h*sp.y()));
				p1 = convertPoint(QPointF(w*ep.x(), h*ep.y()));
			} else if (mode == QGradient::ObjectBoundingMode){
				QRectF rect	= painter()->combinedTransform().map(path).boundingRect();
				double x = rect.left(), y = rect.top();
				double w = rect.width(), h = rect.height();
				p0 = convertPoint(QPointF(x + w*sp.x(), y + h*sp.y()));
				p1 = convertPoint(QPointF(x + w*ep.x(), y + h*ep.y()));
			}

			QString name = "pattern" + QString::number(d_brushes.size() + 1);
			s += "<< /PatternType 2   /Shading\n";
			s += "<< /ShadingType 2\n";
			s += "/Coords [" + QString::number(p0.x()) + " " + QString::number(p0.y()) + " " + QString::number(p1.x()) + " " + QString::number(p1.y()) + "]\n";
			s += "/ColorSpace /Device";
			s += d_gray_scale ? "Gray\n" : "RGB\n";
			s += "/Function\n";
			s += "<<\n";
			s += "/FunctionType 2 /Domain [0 1]\n";
			if (d_gray_scale){
				s += "/C0 [" + QString::number((double)qGray(c0.rgb())/255.0) + "]\n";
				s += "/C1 [" + QString::number((double)qGray(c1.rgb())/255.0) + "]\n";
			} else {
				s += "/C0 [" + QString::number(c0.redF()) + " " + QString::number(c0.greenF()) + " " + QString::number(c0.blueF()) + "]\n";
				s += "/C1 [" + QString::number(c1.redF()) + " " + QString::number(c1.greenF()) + " " + QString::number(c1.blueF()) + "]\n";
			}
			s += "/N 1.0000\n";
			s += ">>\n";
			s += ">>\n";
			s += ">> matrix makepattern /" + name + " exch def\n";
			s += name + " setpattern\n";
		}
		break;

		case Qt::RadialGradientPattern:
		{
			const QRadialGradient *g = (const QRadialGradient *)brush.gradient();

			QTransform m = painter()->combinedTransform();
			QPointF	p0 = convertPoint(m.map(g->focalPoint())), p1 = convertPoint(m.map(g->center()));

			QGradientStops stops = g->stops();
			QColor c0 = stops.first().second, c1 = stops.last().second;

			QString name = "pattern" + QString::number(d_brushes.size() + 1);
			s += "<< /PatternType 2   /Shading\n";
			s += "<< /ShadingType 3\n";
			s += "/Coords [" + QString::number(p0.x()) + " " + QString::number(p0.y()) + " 0 ";
			s += QString::number(p1.x()) + " " + QString::number(p1.y()) + " " + QString::number(resFactorX()*g->radius()) + "]\n";
			s += "/Extend [true false]\n";
			s += "/ColorSpace /Device";
			s += d_gray_scale ? "Gray\n" : "RGB\n";
			s += "/Function\n";
			s += "<<\n";
			s += "/FunctionType 2 /Domain [0 1]\n";
			if (d_gray_scale){
				s += "/C0 [" + QString::number((double)qGray(c0.rgb())/255.0) + "]\n";
				s += "/C1 [" + QString::number((double)qGray(c1.rgb())/255.0) + "]\n";
			} else {
				s += "/C0 [" + QString::number(c0.redF()) + " " + QString::number(c0.greenF()) + " " + QString::number(c0.blueF()) + "]\n";
				s += "/C1 [" + QString::number(c1.redF()) + " " + QString::number(c1.greenF()) + " " + QString::number(c1.blueF()) + "]\n";
			}
			s += "/N 1.0000\n";
			s += ">>\n";
			s += ">>\n";
			s += ">> matrix makepattern /" + name + " exch def\n";
			s += name + " setpattern\n";
		}
		break;

		case Qt::ConicalGradientPattern:
		{
			double x = 0, y = 0, w = 0, h = 0;
			if (!path.isEmpty()){
				QRectF rect = path.boundingRect();//Get the path's bounding rectangle.
				x = rect.left();
				y = rect.top();
				w = rect.width();
				h = rect.height();

				QPixmap pix = QPixmap((int)w, (int)h);
				QPainter paint(&pix);
				paint.translate(-x, -y);
				paint.fillRect(QRect(0, 0, x + w, y + h), brush);
				paint.end();

				QTextStream t(file);
				t.setCodec("UTF-8");
				t << "gs\n" + psPath(path) + "cp\n";//clip original path
				t.flush();

				drawPixmap(pix, rect);

				t << "gr\n";
				t.flush();
				return "";
			}
		}
		break;

		case Qt::TexturePattern:
		{
			if (!path.isEmpty()){
				QTextStream t(file);
				t.setCodec("UTF-8");
				t << "gs\n" + psPath(path) + "cp\n";//clip original path
				t.flush();

				QRectF r = path.boundingRect();
				double x = r.x(), y = r.y();
				drawTiledPixmap(QRectF(-x, -y, x + r.width(), y + r.height()), brush.texture(), QPoint(x, y));

				t << "gr\n";
				t.flush();
				return "";
			}
		}
		break;
	}

	if (style <= Qt::SolidPattern || s.isEmpty())
		return s;

	d_brushes.append(brush);
	return s;
}

QString EpsPaintEngine::psPath(const QPainterPath& path)
{
	QString s = QString::null;
	if (path.isEmpty())
		return s;

	s += "np ";
	int length = 3;
	int points = path.elementCount();
	QTransform m = painter()->combinedTransform();
	int curvePoints = 0;
	for (int i = 0; i < points; i++){
		QPainterPath::Element el = path.elementAt(i);
		QPointF p = m.map(QPointF(el.x, el.y));
		QString aux = psPoint(convertPoint(p)) + " ";
		switch (el.type){
			case QPainterPath::MoveToElement:
				aux += "m ";
			break;
			case QPainterPath::LineToElement:
				aux += "l ";
			break;
			case QPainterPath::CurveToElement:
				curvePoints = 0;
			break;
			case QPainterPath::CurveToDataElement:
				curvePoints++;
				if (curvePoints == 2)
					aux += "c ";
			break;
		}

		int alength = aux.length();
		if (length + alength >= 255){
			s += "\n";
			length = 0;
		}

		s += aux;
		length += alength;
	}
	return s + "\n";
}

QPointF EpsPaintEngine::convertPoint(const QPointF& p)
{
	return QPointF(resFactorX()*p.x(), resFactorY()*(paintDevice()->height() - p.y()));
}

double EpsPaintEngine::resFactorX()
{
	return 72./(double)paintDevice()->logicalDpiX();
}

double EpsPaintEngine::resFactorY()
{
	return 72./(double)paintDevice()->logicalDpiY();
}

QString EpsPaintEngine::psPoint(const QPointF & p)
{
	QString s = QString::number(p.x());
	s += " " + QString::number(p.y());
	return s;
}

QString EpsPaintEngine::psColor(const QColor& col)
{
	QString s;
	if (d_gray_scale)
		s = QString::number(qGray(col.rgb())/255.0) + " col\n";
	else {
		s += QString::number(col.redF()) + " ";
		s += QString::number(col.greenF()) + " ";
		s += QString::number(col.blueF()) + " col\n";
	}
	return s;
}

QString EpsPaintEngine::psPen(const QPen& pen)
{
	if (pen.style() == Qt::NoPen || !pen.color().alpha())
		return QString::null;

	QString col = psColor(pen.color());

	double lw = resFactorY()*painter()->pen().widthF();
	QString options = QString::number(lw) + " lw";

	Qt::PenStyle penStyle = pen.style();
	if (penStyle != Qt::SolidLine){
		QVector<qreal> pattern = pen.dashPattern();
		if (!pattern.isEmpty()){
			options += " [";
			options += QString::number(lw*pattern.first());
			int count = pattern.count();
			for (int i = 1; i < count; i++)
				options += " " + QString::number(lw*pattern[i]);

			options += "] " + QString::number(pen.dashOffset()) + " ld";
		}
	} else
		options += " [] 0 ld";

	bool mitterJoin = false;
	switch (pen.joinStyle()){
		case Qt::MiterJoin:
		case Qt::SvgMiterJoin:
			options += " 0";
			mitterJoin = true;
		break;
		case Qt::BevelJoin:
			options += " 2";
		break;
		case Qt::RoundJoin:
			options += " 1";
		break;
		default:
			break;
	}
	options += " lj";

	if (mitterJoin)
		options += " " + QString::number(pen.miterLimit()) + " ml";

	switch (pen.capStyle()){
		case Qt::FlatCap:
			options += " 0";
		break;
		case Qt::SquareCap:
			options += " 2";
		break;
		case Qt::RoundCap:
			options += " 1";
		break;
		default:
			break;
	}
	options += " lc";
	return col + options + "\n";
}

void EpsPaintEngine::writeToFile(const QString& s)
{
	QTextStream t(file);
	t.setCodec("UTF-8");

	/*bool clipping = painter()->hasClipping();
	if (clipping)
		t << "gs\n" + psPath(painter()->clipPath().simplified()) + "cp\n";*/

	t << s;

	//if (clipping) t << "gr\n";
}

bool EpsPaintEngine::emptyStringOperation()
{
	if ((painter()->brush().style() == Qt::NoBrush || (painter()->brush().color().alpha() == 0)) && painter()->pen().style() == Qt::NoPen)
		return true;

	return false;
}
