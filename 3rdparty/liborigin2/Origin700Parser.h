/***************************************************************************
	File                 : Origin700Parser.h
    --------------------------------------------------------------------
	Copyright            : (C) 2010 Ion Vasilief
	Email (use @ for *)  : ion_vasilief*yahoo.fr
	Description          : Origin 7.0 file parser class (uses code from file
							Origin750Parser.cpp written by Alex Kargovsky)

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
#ifndef ORIGIN_700_PARSER_H
#define ORIGIN_700_PARSER_H

#include "Origin610Parser.h"

class Origin700Parser : public Origin610Parser
{
public:
	Origin700Parser(const string& fileName);
	bool parse();

protected:
	void readMatrixInfo();
	bool readGraphInfo();
};

#endif // ORIGIN_700_PARSER_H
