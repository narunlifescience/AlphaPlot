#include "colorBox.h"

#include <qpixmap.h>
#include <algorithm>

const QColor ColorBox::colors[] = {
  QColor(black),
  QColor(red),
  QColor(green),
  QColor(blue),
  QColor(cyan),
  QColor(magenta),
  QColor(yellow),
  QColor(darkYellow),
  QColor(darkBlue),
  QColor(darkMagenta),
  QColor(darkRed),
  QColor(darkGreen),
  QColor(darkCyan),
  QColor(white),
  QColor(lightGray),
  QColor(darkGray),
};

ColorBox::ColorBox(bool rw, QWidget *parent) : QComboBox(rw, parent)
{
  init();
}

ColorBox::ColorBox(QWidget *parent) : QComboBox(parent)
{
  init();
}

void ColorBox::init()
{
  QPixmap icon = QPixmap(28, 14);

  icon.fill ( QColor (Qt::black) );
  this->insertItem(icon, tr( "black" ) );
  icon.fill (QColor (Qt::red) );
  this->insertItem(icon, tr( "red" ) );
  icon.fill (QColor (Qt::green) );
  this->insertItem(icon, tr( "green" ) );
  icon.fill (QColor (Qt::blue) );
  this->insertItem(icon, tr( "blue" ) );
  icon.fill (QColor (Qt::cyan) );
  this->insertItem(icon, tr( "cyan" ) );
  icon.fill (QColor (Qt::magenta) );
  this->insertItem(icon, tr( "magenta" ) );
  icon.fill (QColor (Qt::yellow) );
  this->insertItem(icon, tr( "yellow" ) );
  icon.fill (QColor (Qt::darkYellow) );
  this->insertItem(icon, tr( "dark yellow" ) );
  icon.fill (QColor (Qt::darkBlue) );
  this->insertItem(icon, tr( "navy" ) );
  icon.fill (QColor (Qt::darkMagenta) );
  this->insertItem(icon, tr( "purple" ) );
  icon.fill (QColor (Qt::darkRed) );
  this->insertItem(icon, tr( "wine" ) );
  icon.fill (QColor (Qt::darkGreen) );
  this->insertItem(icon, tr( "olive" ) );
  icon.fill (QColor (Qt::darkCyan) );
  this->insertItem(icon, tr( "dark cyan" ) );
  icon.fill (QColor (Qt::white) );
  this->insertItem(icon,tr( "white" ) );
  icon.fill (QColor (Qt::lightGray) );
  this->insertItem(icon, tr( "light gray" ) );
  icon.fill (QColor (Qt::darkGray) );
  this->insertItem(icon, tr( "dark gray" ) );    
}

void ColorBox::setColor(const QColor& c)
{
  const QColor *ite = std::find(colors, colors + sizeof(colors), c);
  if (ite == colors + sizeof(colors))
    this->setCurrentItem(0); // default color is black. 
  else
    this->setCurrentItem(ite - colors);
}

QColor ColorBox::color() const
{
  size_t i = this->currentItem();
  if (i < sizeof(colors))
    return colors[this->currentItem()];
  else
    return QColor(black); // default color is black.
}

int ColorBox::colorIndex(const QColor& c)
{
const QColor *ite = std::find(colors, colors + sizeof(colors), c);
return (ite - colors);
}

QColor ColorBox::color(int colorIndex)
{
  if (colorIndex < (int)sizeof(colors))
    return colors[colorIndex];
  else
    return QColor(black); // default color is black.
}
