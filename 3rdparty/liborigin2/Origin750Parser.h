/***************************************************************************
    File                 : Origin750Parser.h
    --------------------------------------------------------------------
	Copyright            : (C) 2007-2008 Alex Kargovsky, Ion Vasilief
    Email (use @ for *)  : kargovsky*yumr.phys.msu.su, ion_vasilief*yahoo.fr
    Description          : Origin 7.5 file parser class

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


#ifndef ORIGIN_750_PARSER_H
#define ORIGIN_750_PARSER_H

#include "OriginParser.h"
#include "endianfstream.hh"
#include <cmath>
#include <string>
using namespace std;
using namespace Origin;

class Origin750Parser : public OriginParser
{
public:
	Origin750Parser(const string& fileName);
	bool parse();

protected:
	void skipObjectInfo();
	void readParameters();
	void readFunction(unsigned int colpos, char valuesize, unsigned int *oldpos);
	void readSpreadInfo();
	void readExcelInfo();
	void readMatrixInfo();
	void readColumnValues(unsigned int spread, unsigned int col, short data_type, char valuesize, unsigned int nr, bool validColumn = true);
	void readMatrixValues(short data_type, char data_type_u, char valuesize, unsigned int size, int mIndex = -1);
	virtual bool readGraphInfo();
	unsigned int readGraphAxisInfo(GraphAxis& axis);
	void readGraphGridInfo(GraphGrid& grid);
	void readGraphAxisBreakInfo(GraphAxisBreak& axis_break);
	void readGraphAxisFormatInfo(GraphAxisFormat& format);
	void readGraphAxisTickLabelsInfo(GraphAxisTick& tick);
	void readGraphAxisPrefixSuffixInfo(const string& sec_name, unsigned int size, GraphLayer& layer);
	void readProjectTree();
	virtual void readProjectTreeFolder(tree<ProjectNode>::iterator parent);
	void readWindowProperties(Window& window, unsigned int size);
	virtual void readColorMap(ColorMap& colorMap);
	void skipLine();

	inline double stringToDouble(const string& s)
	{
		string s1 = s;
		size_t pos = s.find(",");
		if (pos != string::npos)
			s1.replace(pos, 1, ".");
		return strtod(s1.c_str(), NULL);
	}

	inline time_t doubleToPosixTime(double jdt)
	{
		/* 2440587.5 is julian date for the unixtime epoch */
		return (time_t) floor((jdt - 2440587) * 86400. + 0.5);
	}

	unsigned int objectIndex;
	iendianfstream file;
	FILE *logfile;

	unsigned int d_colormap_offset;
};

#endif // ORIGIN_750_PARSER_H
