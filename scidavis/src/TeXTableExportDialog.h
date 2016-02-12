/***************************************************************************
    File                 : TeXTableExportDialog.h
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

#ifndef TEX_TABLE_EXPORT_DIALOG_H
#define TEX_TABLE_EXPORT_DIALOG_H


#include "ExtensibleFileDialog.h"

class TeXTableSettingsWidget;
class TeXTableSettings;

//!TeXTableExportDialog
/*
 * The TeXTableExportDialog class is a dialog window which that allow users to select 
 * the TeX table parameters and file in wich the table data will be exported in the TeX format.
 */ 
class TeXTableExportDialog: public ExtensibleFileDialog
{
    Q_OBJECT
    
    public:
        TeXTableExportDialog( QWidget* parent = 0 );
        //! Returns the TeX table settings choosed by the user
        TeXTableSettings tex_TableSettings( void );
  
    private:
        //! tex_settings_Widget is a pointer to the a TeX table settings widget.
        /**
         *  The TeX table settings widget is inserted in the current layout.
         */ 
        TeXTableSettingsWidget* tex_settings_Widget;
};

#endif // ifndef TEX_TABLE_EXPORT_DIALOG_H
