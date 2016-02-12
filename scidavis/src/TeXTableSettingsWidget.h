/***************************************************************************
    File                 : TeXTableSettingsWidget.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2010 by Iurie Nistor
    Email (use @ for *)  : nistor*iurie.org
    Description          : TeX table settings Widget

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
#ifndef TEX_TABLE_SETTINGS_WIDGET_H
#define TEX_TABLE_SETTINGS_WIDGET_H


#include <QWidget>

class TeXTableSettings;
class QCheckBox;
class QRadioButton;

//!TeXTableSettingsWidget class 
/**
 * The TeXTableSettingsWidget is a simple widget which provides the possibility to  
 * choose TeX table parameters. It can be inserted into the other 
 * widgets( dialogs ) layout, for example, in the TeX table export dialog.  
 */
class TeXTableSettingsWidget: public QWidget
{
    Q_OBJECT
    
    public:
        TeXTableSettingsWidget( QWidget* parent = 0 );
        //! Return the parameters settings of the TeX table
        TeXTableSettings settings( void );
        //! Sets the all parameters settings of the TeX table
        void setSettings( TeXTableSettings & settings );

    private:
         QCheckBox* with_caption_CB;
         QCheckBox* with_labels_CB;
         QRadioButton* align_left_RB;
         QRadioButton* align_center_RB;
         QRadioButton* align_right_RB;
};

#endif // ifndef TEX_TABLE_SETTINGS_WIDGET_H
