/***************************************************************************
    File                 : ImageDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief
    Email (use @ for *)  : ion_vasilief*yahoo.fr
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
#include "ImageDialog.h"

#include <QLayout>
#include <QGroupBox>
#include <QLabel>

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
	
    QGridLayout *gl1 = new QGridLayout(gb1);
    gl1->addWidget(new QLabel( tr("X= ")), 0, 0);
    gl1->addWidget(boxX, 0, 1);
    gl1->addWidget(new QLabel(tr("Y= ")), 1, 0);
    gl1->addWidget(boxY, 1, 1);
    gl1->setRowStretch(2, 1);
    
    QGroupBox *gb2 = new QGroupBox(tr("Size"));
    boxWidth = new QSpinBox();
	boxWidth->setRange(0, 2000);
	boxWidth->setSuffix(tr(" pixels"));
	
	boxHeight = new QSpinBox();
	boxHeight->setRange(0, 2000);
	boxHeight->setSuffix(tr(" pixels"));
	
    QGridLayout *gl2 = new QGridLayout(gb2);
    gl2->addWidget(new QLabel( tr("width= ")), 0, 0);
    gl2->addWidget(boxWidth, 0, 1);

    gl2->addWidget(new QLabel(tr("height= ")), 2, 0);
    gl2->addWidget(boxHeight, 2, 1);

	keepRatioBox = new QCheckBox(tr("Keep aspect ratio"));
	keepRatioBox->setChecked(true);
    gl2->addWidget(keepRatioBox, 3, 1);

    gl2->setRowStretch(4, 1);
	
    QBoxLayout *bl1 = new QBoxLayout (QBoxLayout::LeftToRight);
	bl1->addWidget(gb1);
	bl1->addWidget(gb2);

	buttonApply = new QPushButton( tr( "&Apply" ) );
	buttonOk = new QPushButton(tr( "&Ok" ) );
    buttonCancel = new QPushButton(tr( "&Cancel" ) );
    
    QBoxLayout *bl2 = new QBoxLayout (QBoxLayout::LeftToRight);
    bl2->addStretch();
	bl2->addWidget(buttonApply);
	bl2->addWidget(buttonOk);
	bl2->addWidget(buttonCancel);

	QVBoxLayout* vl = new QVBoxLayout(this);
    vl->addLayout(bl1);
	vl->addLayout(bl2);
   
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT(update() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
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
if (keepRatioBox->isChecked())
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
if (keepRatioBox->isChecked())
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
emit setGeometry(boxX->value(), boxY->value(), boxWidth->value(), boxHeight->value());
}

void ImageDialog::accept()
{
update();
close();
}
