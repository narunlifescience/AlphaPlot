#include "note.h"

#include <qdatetime.h>
#include <qlayout.h>
#include <qapplication.h>
#include <qprinter.h>
#include <qpainter.h>
#include <qpaintdevicemetrics.h>
#include <qsimplerichtext.h>

#include <math.h>

Note::Note(const QString& label, QWidget* parent, const char* name, WFlags f)
				: myWidget(label, parent, name, f)
{
init();	
}

void Note::init()
{
QDateTime dt = QDateTime::currentDateTime ();
setBirthDate(dt.toString(Qt::LocalDate));

te = new QTextEdit(this);
QVBoxLayout* hlayout = new QVBoxLayout(this,0,0, "hlayout1");
hlayout->addWidget(te);

setGeometry(0, 0, 500, 200);
connect(te, SIGNAL(textChanged()), this, SLOT(modifiedNote()));
}

void Note::modifiedNote()
{
emit modifiedWindow(this);
}

QString Note::saveToString(const QString &info)
{
QString s= "<note>\n";
s+= QString(name()) + "\t" + birthDate() + "\n";
s+= info;
s+= "WindowLabel\t" + windowLabel() + "\t" + QString::number(captionPolicy()) + "\n";
s+= te->text().stripWhiteSpace()+"\n";
s+="</note>\n";
return s;
}

void Note::print()
{
QPrinter printer;
printer.setColorMode (QPrinter::GrayScale);
if (printer.setup()) 
	{
    printer.setFullPage( TRUE );
    QPainter painter;
    if ( !painter.begin(&printer ) )
         return;

	QPaintDeviceMetrics metrics( painter.device() );
	int dpiy = metrics.logicalDpiY();
	int margin = (int) ( (1/2.54)*dpiy ); // 1 cm margins
	QRect body( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
	QSimpleRichText richText(QStyleSheet::convertFromPlainText(te->text()), QFont(), 
							te->context(), te->styleSheet(), te->mimeSourceFactory(), body.height());
	richText.setWidth( &painter, body.width() );
  	QRect view( body );
	int page = 1;
	do {
	    richText.draw( &painter, body.left(), body.top(), view, colorGroup() );
	    view.moveBy( 0, body.height() );
	    painter.translate( 0 , -body.height() );
	    painter.drawText( view.right() - painter.fontMetrics().width( QString::number( page ) ),
			view.bottom() + painter.fontMetrics().ascent() + 5, QString::number( page ) );
	    if ( view.top()  >= richText.height() )
			break;
	    printer.newPage();
	    page++;
		} 
	while (TRUE);
	}
}



