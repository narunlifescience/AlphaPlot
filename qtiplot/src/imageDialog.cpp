/***************************************************************************
    File                 : imageDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief
    Email                : ion_vasilief@yahoo.fr
    Description          : Image geometry dialog
                           
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#include "imageDialog.h"

#include <QLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPixmap>

/* XPM */
static const char * up_xpm[] = {
"6 4 3 1",
" 	c None",
".	c #8F8B6D",
"+	c #EFEEE4",
"..... ",
"+++++.",
"    +.",
"    +."};

/* XPM */
static const char * down_xpm[] = {
"6 5 3 1",
" 	c None",
".	c #EFEEE4",
"+	c #8F8B6D",
"    .+",
"    .+",
"    .+",
".....+",
"+++++ "};

/* XPM */
static const char * chain_xpm[] = {
"7 20 9 1",
" 	c None",
".	c #020204",
"+	c #6E6E6E",
"@	c #D0D0D1",
"#	c #B5B5B6",
"$	c #5A5A5C",
"%	c #9A9A98",
"&	c #E8E8E9",
"*	c #8F8F91",
" ..... ",
".+@@#$.",
".%...&.",
".@. .@.",
".@. .@.",
".#. .@.",
".%...@.",
".*.+.*.",
" ..#.. ",
"  .@.  ",
"  .@.  ",
" ..@.. ",
".+.#.*.",
".#...#.",
".#. .&.",
".#. .&.",
".#. .&.",
".@...&.",
".+&@&#.",
" ..... "};

/* XPM */
static const char * unchain_xpm[] = {
"7 24 9 1",
" 	c None",
".	c #020204",
"+	c #6E6E6E",
"@	c #D0D0D1",
"#	c #B5B5B6",
"$	c #5A5A5C",
"%	c #9A9A98",
"&	c #E8E8E9",
"*	c #8F8F91",
" ..... ",
".+@@#$.",
".%...&.",
".@. .@.",
".@. .@.",
".#. .@.",
".%...@.",
".*.+.*.",
" ..#.. ",
"  .@.  ",
"       ",
"       ",
"       ",
"       ",
"  .@.  ",
" ..@.. ",
".+.#.*.",
".#...#.",
".#. .&.",
".#. .&.",
".#. .&.",
".@...&.",
".+&@&#.",
" ..... "};

ImageDialog::ImageDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "ImageDialog" );
	setWindowTitle( tr( "QtiPlot - Image Geometry" ) );
	
	QGroupBox *gb1 = new QGroupBox(tr("Origin"));
	boxX = new QSpinBox();
	boxX->setRange(0, 2000);
	boxX->setSuffix(tr(" pixels"));
	
	boxY = new QSpinBox();
	boxY->setRange(0, 2000);
	boxY->setSuffix(tr(" pixels"));
	
    QGridLayout *gl1 = new QGridLayout();
    gl1->addWidget(new QLabel( tr("X= ")), 0, 0);
    gl1->addWidget(boxX, 0, 1);
    gl1->addWidget(new QLabel(tr("Y= ")), 1, 0);
    gl1->addWidget(boxY, 1, 1);
    gb1->setLayout(gl1);
    
    QGroupBox *gb2 = new QGroupBox(tr("Size"));
    boxWidth = new QSpinBox();
	boxWidth->setRange(0, 2000);
	boxWidth->setSuffix(tr(" pixels"));
	
	boxHeight = new QSpinBox();
	boxHeight->setRange(0, 2000);
	boxHeight->setSuffix(tr(" pixels"));
	
    QGridLayout *gl2 = new QGridLayout();
    gl2->addWidget(new QLabel( tr("width= ")), 0, 0);
    gl2->addWidget(boxWidth, 0, 1);
    gl2->addWidget(new QLabel(tr("height= ")), 1, 0);
    gl2->addWidget(boxHeight, 1, 1);
    gb2->setLayout(gl2);

	QLabel *up = new QLabel();
	up->setPixmap(QPixmap(up_xpm));

	linkButton = new ChainButton();

	QLabel *down = new QLabel();
	down->setPixmap(QPixmap(down_xpm));
	
	QBoxLayout *bl = new QBoxLayout (QBoxLayout::TopToBottom);
	bl->addWidget(up);
	bl->addWidget(linkButton);
	bl->addWidget(down);
	
    QBoxLayout *bl1 = new QBoxLayout (QBoxLayout::LeftToRight);
	bl1->addWidget(gb1);
	bl1->addWidget(gb2);
	bl1->addLayout(bl);

	buttonApply = new QPushButton( tr( "&Apply" ) );
	buttonOk = new QPushButton(tr( "&Ok" ) );
    buttonCancel = new QPushButton(tr( "&Cancel" ) );
    
    QBoxLayout *bl2 = new QBoxLayout (QBoxLayout::LeftToRight);
	bl2->addWidget(buttonApply);
	bl2->addWidget(buttonOk);
	bl2->addWidget(buttonCancel);
	bl2->addStretch();
	
	QVBoxLayout* vl = new QVBoxLayout();
    vl->addLayout(bl1);
	vl->addLayout(bl2);
	vl->addStretch();
	setLayout(vl);
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT(update() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

ImageDialog::~ImageDialog()
{
}

void ImageDialog::setOrigin(const QPoint& o)
{
boxX->setValue(o.x());
boxY->setValue(o.y());
}

void ImageDialog::setSize(const QSize& size)
{
boxWidth->setValue(size.width());
boxHeight->setValue(size.height());
aspect_ratio = (double)size.width()/(double)size.height();

connect( boxWidth, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustHeight(int) ) );
connect( boxHeight, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustWidth(int) ) );
}

void ImageDialog::adjustHeight(int width)
{
if (linkButton->isLocked())
	{
	disconnect( boxHeight, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustWidth(int) ) );
	boxHeight->setValue(int(width/aspect_ratio));
	connect( boxHeight, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustWidth(int) ) );
	}
else
	aspect_ratio = (double)width/double(boxHeight->value());
}

void ImageDialog::adjustWidth(int height)
{
if (linkButton->isLocked())
	{
	disconnect( boxWidth, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustHeight(int) ) );
	boxWidth->setValue(int(height*aspect_ratio));
	connect( boxWidth, SIGNAL( valueChanged ( int ) ), this, SLOT( adjustHeight(int) ) );
	}
else
	aspect_ratio = double(boxWidth->value())/(double)height;
}

void ImageDialog::update()
{
emit setGeometry(boxX->value(),boxY->value(),boxWidth->value(),boxHeight->value());
}

void ImageDialog::accept()
{
update();
close();
}

ChainButton::ChainButton(QWidget *parent) : QPushButton(parent)
{
locked = true;
setFlat (true);
setAutoDefault (false);
setIconSet (QPixmap(chain_xpm));
setMaximumWidth(20);

connect (this, SIGNAL(clicked()), this, SLOT(changeLock()));
}

void ChainButton::changeLock() 
{
locked = !locked;
if (locked)
	setIconSet (QPixmap(chain_xpm));
else
	setIconSet (QPixmap(unchain_xpm));
}
