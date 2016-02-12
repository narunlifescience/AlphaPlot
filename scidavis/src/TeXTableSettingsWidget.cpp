/***************************************************************************
    File                 : TeXTableSettingsWidget.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2010 by Iurie Nistor
    Email (use @ for *)  : nistor*iurie.org
    Description          : TeX settings Widget

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 3 of the License, or      *
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

#include "TeXTableSettings.h"
#include "TeXTableSettingsWidget.h"

#include <QCheckBox>
#include <QRadioButton>
#include <QGroupBox>
#include <QHBoxLayout>


TeXTableSettingsWidget::TeXTableSettingsWidget( QWidget* parent ) : QWidget( parent )
{
     with_caption_CB = new QCheckBox( tr("Table caption") );
     with_labels_CB = new QCheckBox( tr("Table labes") );

  
     QGroupBox*  alignGroupBox = new QGroupBox( tr("TeX table columns alignment"), this );    

     align_left_RB = new QRadioButton( tr("Left") );
     align_center_RB = new QRadioButton( tr("Center") );
     align_right_RB = new QRadioButton( tr("Right") );
     align_center_RB->setChecked(true);
     
     QHBoxLayout* alignBoxLayout = new QHBoxLayout;
     alignBoxLayout->addWidget(align_left_RB);
     alignBoxLayout->addWidget(align_center_RB);
     alignBoxLayout->addWidget(align_right_RB);
     
     alignGroupBox->setLayout(alignBoxLayout);

     // Main widget layout
     QGridLayout* mainLayout = new QGridLayout(this);

     mainLayout->addWidget(with_caption_CB, 0,0);
     mainLayout->addWidget(with_labels_CB, 1,0);
     mainLayout->addWidget( alignGroupBox, 2,0);

     setLayout(mainLayout);

}

TeXTableSettings TeXTableSettingsWidget::settings( void )
{
    TeXTableSettings selected_settings;

    // TeX table caption
    if( with_caption_CB->isChecked() ) selected_settings.set_with_caption( true );
    else selected_settings.set_with_caption( false );

    // TeX columns labels
    if( with_caption_CB->isChecked() ) selected_settings.set_with_labels( true );
    else selected_settings.set_with_labels( false );

    // TeX columns alignment
    if( align_left_RB->isChecked() ) selected_settings.set_columnsAlignment( ALIGN_LEFT );
    else if( align_right_RB->isChecked() ) selected_settings.set_columnsAlignment( ALIGN_RIGHT );
    else selected_settings.set_columnsAlignment( ALIGN_CENTER );

    return selected_settings;
}


