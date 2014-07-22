/***************************************************************************
	File                 : Origin410Parser.h
    --------------------------------------------------------------------
	Copyright            : (C) 2011 Ion Vasilief
	Email (use @ for *)  : ion_vasilief*yahoo.fr
	Description          : Origin 4.1 file parser class

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

#ifndef ORIGIN_410_PARSER_H
#define ORIGIN_410_PARSER_H

#include "Origin500Parser.h"

class Origin410Parser : public Origin500Parser
{
public:
	Origin410Parser(const string& fileName);
	bool parse();

protected:
	bool readGraphInfo();
};

#endif // ORIGIN_410_PARSER_H
