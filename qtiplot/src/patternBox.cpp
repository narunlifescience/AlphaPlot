#include "patternBox.h"

#include <algorithm>
#include <qpixmap.h>
#include <qpainter.h>

const Qt::BrushStyle PatternBox::patterns[] = {
  Qt::SolidPattern,
  Qt::HorPattern,
  Qt::VerPattern,
  Qt::CrossPattern,
  Qt::BDiagPattern,
  Qt::FDiagPattern,
  Qt::DiagCrossPattern,
  Qt::Dense1Pattern,
  Qt::Dense2Pattern,
  Qt::Dense3Pattern,
  Qt::Dense4Pattern,
  Qt::Dense5Pattern,
  Qt::Dense6Pattern,
  Qt::Dense7Pattern,
};

PatternBox::PatternBox(bool rw, QWidget *parent) : QComboBox(rw, parent)
{
  init();
}

PatternBox::PatternBox(QWidget *parent) : QComboBox(parent)
{
  init();
}

void PatternBox::init()
{

  QPixmap icon = QPixmap(28, 14);
  icon.fill ( QColor (Qt::white) );
  const QRect r= QRect(0, 0, 28, 14);
  QPainter p(&icon);
  QBrush br = QBrush(QColor(darkGray), Qt::SolidPattern);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "Solid" ) );

  br = QBrush(QColor(darkGray), Qt::HorPattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "Horizontal" ) );

  br = QBrush(QColor(darkGray), Qt::VerPattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "Vertical" ) );

  br = QBrush(QColor(darkGray), Qt::CrossPattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "Cross" ) );

  br = QBrush(QColor(darkGray), Qt::BDiagPattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "BDiagonal" ) );

  br = QBrush(QColor(darkGray), Qt::FDiagPattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "FDiagonal" ) );

  br = QBrush(QColor(darkGray), Qt::DiagCrossPattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "DiagCross" ) );

  br = QBrush(QColor(darkGray), Qt::Dense1Pattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "Dense1" ) );

  br = QBrush(QColor(darkGray), Qt::Dense2Pattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "Dense2" ) );

  br = QBrush(QColor(darkGray), Qt::Dense3Pattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "Dense3" ) );

  br = QBrush(QColor(darkGray), Qt::Dense4Pattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "Dense4" ) );

  br = QBrush(QColor(darkGray), Qt::Dense5Pattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "Dense5" ) );

  br = QBrush(QColor(darkGray), Qt::Dense6Pattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "Dense6" ) );

  br = QBrush(QColor(darkGray), Qt::Dense7Pattern);
  p.eraseRect(r);
  p.fillRect(r, br);
  p.drawRect(r);
  this->insertItem(icon, tr( "Dense7" ) );

  p.end();
}

void PatternBox::setPattern(const Qt::BrushStyle& style)
{
  const Qt::BrushStyle*ite = std::find(patterns, patterns + sizeof(patterns), style);
  if (ite == patterns + sizeof(patterns))
    this->setCurrentItem(0); // default pattern is solid.
  else
    this->setCurrentItem(ite - patterns);
}

Qt::BrushStyle PatternBox::getSelectedPattern() const
{
  size_t i = this->currentItem();
  if (i < sizeof(patterns))
    return patterns[this->currentItem()];
  else
    return Qt::SolidPattern; // default patterns is solid. 
}

int PatternBox::patternIndex(const Qt::BrushStyle& style)
{
  const Qt::BrushStyle*ite = std::find(patterns, patterns + sizeof(patterns), style);
  if (ite == patterns + sizeof(patterns))
    return 0; // default pattern is solid.
  else
    return (ite - patterns);
}
