#include "colorButton.h"

ColorButton::ColorButton(QWidget *parent) : QPushButton(parent)
{
init();
}

void ColorButton::init()
{
setPaletteBackgroundColor ( QColor(white) ) ;
setAutoDefault(false);
}

void ColorButton::setColor(const QColor& c)
{
if ( paletteBackgroundColor() == c)
	return;

setPaletteBackgroundColor ( c ) ;

#ifdef Q_OS_MAC // Mac 
	return;
#else
	{
	if (qGray (c.red(), c.green(), c.blue()) > 128)
		setPaletteForegroundColor (QColor(black));
	else
		setPaletteForegroundColor (QColor(white));
	}
#endif
}

QColor ColorButton::color() const
{
return paletteBackgroundColor();
}
