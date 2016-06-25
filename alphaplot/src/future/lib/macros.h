/***************************************************************************
    File                 : macros.h
    Project              : AlphaPlot
    Description          : Various preprocessor macros
    --------------------------------------------------------------------
    Copyright            : (C) 2008-2009 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2008-2009 Knut Franke (knut.franke*gmx.de)
                           (replace * with @ in the email addresses)

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

#ifndef MACROS_H
#define MACROS_H

#define BASIC_ACCESSOR(type, var, method, Method) \
  type method() const { return var; };            \
  void set##Method(const type value) { var = value; }
#define CLASS_ACCESSOR(type, var, method, Method) \
  type method() const { return var; };            \
  void set##Method(const type& value) { var = value; }

#endif  // MACROS_H
