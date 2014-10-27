/***************************************************************************
    File                 : TeXTableExportDialog.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2010 by Iurie Nistor
    Email (use @ for *)  : nistor*iurie.org
    Description          : TeX table export dialog

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
#include "TeXTableExportDialog.h"


TeXTableExportDialog::TeXTableExportDialog( QWidget* parent ): ExtensibleFileDialog( parent )
{
    setWindowTitle( tr("TeX table export dialog") );
    setAcceptMode( QFileDialog::AcceptSave );

    //Create the TeX table settigns widget
    tex_settings_Widget = new TeXTableSettingsWidget( this );   

    //Insert the TeX table settigns widget in the dialog layout
    setExtensionWidget( tex_settings_Widget );
}

TeXTableSettings TeXTableExportDialog::tex_TableSettings( void )
{
     return tex_settings_Widget->settings();
}

