/***************************************************************************
	File                 : Origin800Parser.h
    --------------------------------------------------------------------
	Copyright            : (C) 2010 Ion Vasilief
	Email (use @ for *)  : ion_vasilief*yahoo.fr
	Description          : Origin 8.0 file parser class

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

#ifndef ORIGIN_800_PARSER_H
#define ORIGIN_800_PARSER_H

#include "Origin750Parser.h"

class Origin800Parser : public Origin750Parser
{
public:
	Origin800Parser(const string& fileName);
	bool parse();

protected:
	void readSpreadInfo();
	void readMatrixInfo();
	void readResultsLog();
	virtual void readNotes();

	unsigned int findStringPos(const string& name);

	string notes_pos_mark;
};

#endif // ORIGIN_800_PARSER_H
