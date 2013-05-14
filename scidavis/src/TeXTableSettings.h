/***************************************************************************
    File                 : TeXTableSettings.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2010 by Iurie Nistor
    Email (use @ for *)  : nistor*iurie.org
    Description          : TeX table settings

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

#ifndef TEX_TABLE_SETTINGS_H
#define TEX_TABLE_SETTINGS_H

//! Definition of TeX table column alignment.
enum ColumAlignment{ ALIGN_LEFT = 0, ALIGN_CENTER = 1, ALIGN_RIGHT = 2 };

//! TeXTableSettings class
/**
 *  The TeXTableSettings calss stores the information about the parameters of a TeX table, for example, 
 *  the columns aligment or whether the TeX table contains the columns labels, etc. 
**/
class TeXTableSettings
{
    public:

        TeXTableSettings( void );
        //! Sets if the TeX table contain or not the caption.
        void set_with_caption( bool b = false ){ table_caption = b; }
        //! Sets if the TeX table contain columns labels or not.
        void set_with_labels( bool b = false ){ table_labels = b; }
        //! Sets columns alignment of the TeX table.
        /**
         *  In the future it will be designed to set column alignment for each column in the TeX table.
         */
        void set_columnsAlignment( ColumAlignment alignment = ALIGN_CENTER ) { columns_alignment = alignment; }
        //! Returns if the TeX table contains caption.
        bool with_caption( void ){ return table_caption; }
        //! Returns if the Tex table contains labels.
        bool with_labels( void ){ return table_labels; }
        //! Returns columns alignment of the TeX table.
        /**
         *  In the future it will be designed to return column alignment for each column in the TeX table.
         */
        ColumAlignment columnsAlignment( void ){ return columns_alignment; }

    private:     
         //! The TeX table caption variable. It stores the information about if the TeX table contains the caption or not.    
         bool table_caption;  
         //! The TeX table caption variable. It stores the information about if the TeX table contains the labels or not.    
         bool table_labels;
         //! Columns alignmet variable. 
         /**
          *  It would be nice to separate for each colum in the future.   
          */ 
         ColumAlignment columns_alignment;   
};

#endif // ifndef TEX_TABLE_SETTINGS_H
