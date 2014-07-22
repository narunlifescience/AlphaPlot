/***************************************************************************
	File                 : Origin700Parser.cpp
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

#include "Origin700Parser.h"

Origin700Parser::Origin700Parser(const string& fileName)
:	Origin610Parser(fileName)
{}

bool Origin700Parser::parse()
{
	unsigned int dataIndex = 0;

#ifndef NO_CODE_GENERATION_FOR_LOG
	// append progress in log file
	logfile = fopen("opjfile.log","a");
#endif // NO_CODE_GENERATION_FOR_LOG
	// get length of file:
	file.seekg (0, ios::end);
	d_file_size = file.tellg();

	unsigned char c;
	/////////////////// find column ///////////////////////////////////////////////////////////
	file.seekg(0x10 + 1, ios_base::beg);
	unsigned int size;
	file >> size;
	file.seekg(1 + size + 1 + 5, ios_base::cur);

	file >> size;

	file.seekg(1, ios_base::cur);
	LOG_PRINT(logfile, "	[column found = %d/0x%X @ 0x%X]\n", size, size, (unsigned int) file.tellg())

	unsigned int colpos = file.tellg();
	unsigned int current_col = 1, nr = 0, nbytes = 0;

	while(size > 0 && size < 0x84) {	// should be 0x72, 0x73 or 0x83
		//////////////////////////////// COLUMN HEADER /////////////////////////////////////////////
		short data_type;
		char data_type_u;
		unsigned int oldpos = file.tellg();

		file.seekg(oldpos + 0x16, ios_base::beg);
		file >> data_type;

		file.seekg(oldpos + 0x3F, ios_base::beg);
		file >> data_type_u;

		char valuesize;
		file.seekg(oldpos + 0x3D, ios_base::beg);
		file >> valuesize;

		LOG_PRINT(logfile, "	[valuesize = %d @ 0x%X]\n", (int)valuesize, ((unsigned int) file.tellg()-1))
		if(valuesize <= 0)
		{
			valuesize = 10;
		}

		file.seekg(oldpos + 0x58, ios_base::beg);
		LOG_PRINT(logfile, "	[Spreadsheet @ 0x%X]\n", (unsigned int) file.tellg())

		string name(25, 0);
		file >> name;

		string::size_type pos = name.find_last_of("_");
		string columnname;
		if(pos != string::npos)
		{
			columnname = name.substr(pos + 1);
			name.resize(pos);
		}
		LOG_PRINT(logfile, "	NAME: %s\n", name.c_str())

		unsigned int spread = 0;
		if(columnname.empty())
		{
			LOG_PRINT(logfile, "NO COLUMN NAME FOUND! Must be a Matrix or Function.\n")
			////////////////////////////// READ matrixes or functions ////////////////////////////////////

			LOG_PRINT(logfile, "	[position @ 0x%X]\n", (unsigned int) file.tellg())
			// TODO
			short signature;
			file >> signature;
			LOG_PRINT(logfile, "	SIGNATURE : %02X\n", signature)


			file.seekg(oldpos + size + 1, ios_base::beg);
			file >> size;
			file.seekg(1, ios_base::cur);
			size /= valuesize;
			LOG_PRINT(logfile, "	SIZE = %d\n", size)

			switch(signature)
			{
			case 0x50CA:
			case 0x70CA:
			case 0x50F2:
			case 0x50E2:
			case 0x50C8:
			case 0x50E7:
			case 0x50DB:
			case 0x50DC:

				LOG_PRINT(logfile, "NEW MATRIX\n")
				matrixes.push_back(Matrix(name, dataIndex));
				++dataIndex;
				LOG_PRINT(logfile, "VALUES :\n")
				if (size >= 100)
					LOG_PRINT(logfile, " matrix too big...")

				switch(data_type)
				{
				case 0x6001://double
					for(unsigned int i = 0; i < size; ++i)
					{
						double value;
						file >> value;
						matrixes.back().data.push_back((double)value);
						if (size < 100)
							LOG_PRINT(logfile, "%g ", value)
					}
					break;
				case 0x6003://float
					for(unsigned int i = 0; i < size; ++i)
					{
						float value;
						file >> value;
						matrixes.back().data.push_back((double)value);
						if (size < 100)
							LOG_PRINT(logfile, "%g ", value)
					}
					break;
				case 0x6801://int
					if(data_type_u == 8)//unsigned
					{
						for(unsigned int i = 0; i < size; ++i)
						{
							unsigned int value;
							file >> value;
							matrixes.back().data.push_back((double)value);
							if (size < 100)
								LOG_PRINT(logfile, "%u ", value)
						}
					}
					else
					{
						for(unsigned int i = 0; i < size; ++i)
						{
							int value;
							file >> value;
							matrixes.back().data.push_back((double)value);
							if (size < 100)
								LOG_PRINT(logfile, "%d ", value)
						}
					}
					break;
				case 0x6803://short
					if(data_type_u == 8)//unsigned
					{
						for(unsigned int i = 0; i < size; ++i)
						{
							unsigned short value;
							file >> value;
							matrixes.back().data.push_back((double)value);
							if (size < 100)
								LOG_PRINT(logfile, "%u ", value)
						}
					}
					else
					{
						for(unsigned int i = 0; i < size; ++i)
						{
							short value;
							file >> value;
							matrixes.back().data.push_back((double)value);
							if (size < 100)
								LOG_PRINT(logfile, "%d ", value)
						}
					}
					break;
				case 0x6821://char
					if(data_type_u == 8)//unsigned
					{
						for(unsigned int i = 0; i < size; ++i)
						{
							unsigned char value;
							file >> value;
							matrixes.back().data.push_back((double)value);
							if (size < 100)
								LOG_PRINT(logfile, "%u ", value)
						}
					}
					else
					{
						for(unsigned int i = 0; i < size; ++i)
						{
							char value;
							file >> value;
							matrixes.back().data.push_back((double)value);
							if (size < 100)
								LOG_PRINT(logfile, "%d ", value)
						}
					}
					break;
				default:
					LOG_PRINT(logfile, "UNKNOWN MATRIX DATATYPE: %02X SKIP DATA\n", data_type)
					file.seekg(valuesize*size, ios_base::cur);
					matrixes.pop_back();
				}
				LOG_PRINT(logfile, "\n")
				break;
			case 0x10C8:
				LOG_PRINT(logfile, "NEW FUNCTION\n")
				functions.push_back(Function(name, dataIndex));
				++dataIndex;

				file >> functions.back().formula.assign(valuesize, 0);
				oldpos = file.tellg();
				short t;

				file.seekg(colpos + 0xA, ios_base::beg);
				file >> t;

				if(t == 0x1194)
					functions.back().type = Function::Polar;

				file.seekg(colpos + 0x21, ios_base::beg);
				file >> functions.back().totalPoints;

				file >> functions.back().begin;
				double d;
				file >> d;
				functions.back().end = functions.back().begin + d*(functions.back().totalPoints - 1);

				LOG_PRINT(logfile, "FUNCTION %s : %s\n", functions.back().name.c_str(), functions.back().formula.c_str())
				LOG_PRINT(logfile, " interval %g : %g, number of points %d\n", functions.back().begin, functions.back().end, functions.back().totalPoints)

				file.seekg(oldpos, ios_base::beg);
				break;
			default:
				LOG_PRINT(logfile, "UNKNOWN SIGNATURE: %.2X SKIP DATA\n", signature)
				file.seekg(valuesize*size, ios_base::cur);
				++dataIndex;

				if(valuesize != 8 && valuesize <= 16)
				{
					file.seekg(2, ios_base::cur);
				}
			}
		}
		else
		{	// worksheet
			if(speadSheets.size() == 0 || findSpreadByName(name) == -1)
			{
				LOG_PRINT(logfile, "NEW SPREADSHEET\n")
				current_col = 1;
				speadSheets.push_back(SpreadSheet(name));
				spread = speadSheets.size() - 1;
				speadSheets.back().maxRows = 0;
			}
			else
			{
				spread = findSpreadByName(name);

				current_col = speadSheets[spread].columns.size();

				if(!current_col)
					current_col = 1;
				++current_col;
			}
			LOG_PRINT(logfile, "SPREADSHEET = %s COLUMN NAME = %s (%d) (@0x%X)\n", name.c_str(), columnname.c_str(), current_col, (unsigned int)file.tellg())
			speadSheets[spread].columns.push_back(SpreadColumn(columnname, dataIndex));
			string::size_type sheetpos = speadSheets[spread].columns.back().name.find_last_of("@");
			if(!speadSheets[spread].multisheet && sheetpos != string::npos)
			{
				if(atoi(columnname.substr(sheetpos + 1).c_str()) > 1)
				{
					speadSheets[spread].multisheet = true;
					LOG_PRINT(logfile, "SPREADSHEET \"%s\" IS MULTISHEET\n", name.c_str())
				}
			}
			++dataIndex;

			////////////////////////////// SIZE of column /////////////////////////////////////////////
			file.seekg(oldpos + size + 1, ios_base::beg);

			file >> nbytes;
			if(fmod(nbytes, (double)valuesize)>0)
			{
				LOG_PRINT(logfile, "WARNING: data section could not be properly read")
			}
			nr = nbytes / valuesize;
			LOG_PRINT(logfile, "	[number of rows = %d (%d Bytes) @ 0x%X]\n", nr, nbytes, (unsigned int)file.tellg())

			speadSheets[spread].maxRows<nr ? speadSheets[spread].maxRows=nr : 0;

			////////////////////////////////////// DATA ////////////////////////////////////////////////
			file.seekg(1, ios_base::cur);

			LOG_PRINT(logfile, "	[data @ 0x%X]\n", (unsigned int)file.tellg())
			for(unsigned int i = 0; i < nr; ++i)
			{
				double value;
				if(valuesize <= 8)	// Numeric, Time, Date, Month, Day
				{
					file >> value;
					LOG_PRINT(logfile, "%g ", value)
					speadSheets[spread].columns[(current_col-1)].data.push_back(value);
				}
				else if((data_type & 0x100) == 0x100) // Text&Numeric
				{
					file >> c;
					file.seekg(1, ios_base::cur);
					if(c == 0) //value
					{
						file >> value;
						LOG_PRINT(logfile, "%g ", value)
						speadSheets[spread].columns[(current_col-1)].data.push_back(value);
						file.seekg(valuesize - 10, ios_base::cur);
					}
					else //text
					{
						string stmp(valuesize - 2, 0);
						file >> stmp;
						if(stmp.find(0x0E) != string::npos) // try find non-printable symbol - garbage test
							stmp = string();
						LOG_PRINT(logfile, "%s ", stmp.c_str())
						speadSheets[spread].columns[(current_col-1)].data.push_back(stmp);
					}
				}
				else //text
				{
					string stmp(valuesize, 0);
					file >> stmp;
					if(stmp.find(0x0E) != string::npos) // try find non-printable symbol - garbage test
						stmp = string();
					LOG_PRINT(logfile, "%s ", stmp.c_str())
					speadSheets[spread].columns[(current_col-1)].data.push_back(stmp);
				}
			}
			LOG_PRINT(logfile, "\n")
		}

		if(nbytes > 0 || columnname.empty())
		{
			file.seekg(1, ios_base::cur);
		}

		file >> size;
		file.seekg(1 + size + (size > 0 ? 1 : 0), ios_base::cur);

		file >> size;

		file.seekg(1, ios_base::cur);
		LOG_PRINT(logfile, "	[column found = %d/0x%X (@ 0x%X)]\n", size, size, ((unsigned int) file.tellg()-5))
		colpos = file.tellg();
	}

	////////////////////////////////////////////////////////////////////////////
	////////////////////// HEADER SECTION //////////////////////////////////////

	unsigned int POS = (unsigned int)file.tellg()-11;
	LOG_PRINT(logfile, "\nHEADER SECTION\n")
	LOG_PRINT(logfile, "	nr_spreads = %d\n", speadSheets.size())
	LOG_PRINT(logfile, "	[position @ 0x%X]\n", POS)

	//////////////////////// OBJECT INFOS //////////////////////////////////////
	POS += 0xB;
	file.seekg(POS, ios_base::beg);
	while(POS < d_file_size){
		POS = file.tellg();

		file >> size;
		if(size == 0)
			break;

		file.seekg(POS + 0x7, ios_base::beg);
		string name(25, 0);
		file >> name;

		file.seekg(POS, ios_base::beg);

		if(findSpreadByName(name) != -1)
			readSpreadInfo();
		else if(findMatrixByName(name) != -1)
			readMatrixInfo();
		else if(findExcelByName(name) != -1)
			readExcelInfo();
		else
			readGraphInfo();

		POS = file.tellg();
	}

	readNotes();
	// If file has no Note windows, last function will read to EOF,
	// skipping the info for ResultsLog and ProjectTree.
	// As we know there is always a ResultsLog window after the Note
	// windows, we better rewind to the start of Notes
	file.seekg(POS, ios_base::beg);
	readResultsLog();

	file.seekg(1 + 4*5 + 0x10 + 1, ios_base::cur);
	try {
		readProjectTree();
	} catch(...){}

	LOG_PRINT(logfile, "Done parsing\n")
#ifndef NO_CODE_GENERATION_FOR_LOG
	fclose(logfile);
#endif // NO_CODE_GENERATION_FOR_LOG
	return true;
}

void Origin700Parser::readMatrixInfo()
{
	unsigned int POS = file.tellg();

	unsigned int size;
	file >> size;

	POS += 5;

	string name(25, 0);
	file.seekg(POS + 0x2, ios_base::beg);
	file >> name;
	LOG_PRINT(logfile, "		MATRIX %s (@ 0x%X)]\n", name.c_str(), POS)

	int idx = findMatrixByName(name);
	matrixes[idx].name = name;
	file.seekg(POS, ios_base::beg);
	readWindowProperties(matrixes[idx], size);

	unsigned char h;
	file.seekg(POS + 0x87, ios_base::beg);
	file >> h;
	switch(h){
		case 1:
			matrixes[idx].view = Matrix::ImageView;
			break;
		case 2:
			matrixes[idx].header = Matrix::XY;
			break;
	}

	unsigned int LAYER = POS;
	LAYER += size + 0x1;
	file.seekg(LAYER, ios_base::beg);
	file >> size;

	// LAYER section
	LAYER += 0x5;

	unsigned short width;
	file.seekg(LAYER + 0x27, ios_base::beg);
	file >> width;
	if (width == 0)
		width = 8;
	matrixes[idx].width = width;
	LOG_PRINT(logfile, "			Width: %d (@ 0x%X)\n", matrixes[idx].width, (LAYER + 0x27))

	file.seekg(LAYER + 0x2B, ios_base::beg);
	file >> matrixes[idx].columnCount;
	LOG_PRINT(logfile, "			Columns: %d (@ 0x%X)\n", matrixes[idx].columnCount, (LAYER + 0x2B))

	file.seekg(LAYER + 0x52, ios_base::beg);
	file >> matrixes[idx].rowCount;
	LOG_PRINT(logfile, "			Rows: %d (@ 0x%X)\n", matrixes[idx].rowCount, (LAYER + 0x52))

	LAYER += size + 0x1;
	file.seekg(LAYER, ios_base::beg);
	file >> size;
	unsigned int sectionSize = size;
	while(LAYER < d_file_size){
		LAYER += 0x5;

		//section_header
		string sec_name(41, 0);
		file.seekg(LAYER + 0x46, ios_base::beg);
		file >> sec_name;
		LOG_PRINT(logfile, "				SECTION NAME: %s (@ 0x%X)\n", sec_name.c_str(), (LAYER + 0x46))

		//section_body_1_size
		LAYER += size + 0x1;
		file.seekg(LAYER, ios_base::beg);
		file >> size;

		//section_body_1
		LAYER += 0x5;
		//check if it is a formula
		if(sec_name == "MV"){
			file.seekg(LAYER, ios_base::beg);
			file >> matrixes[idx].command.assign(size, 0);
			LOG_PRINT(logfile, "				Formula: %s (@ 0x%X)\n", matrixes[idx].command.c_str(), LAYER)
		} else if (sec_name == "Y2"){
			string s(size, 0);
			file >> s;
			matrixes[idx].coordinates[0] = stringToDouble(s);
			LOG_PRINT(logfile, "				Y2: %g\n", matrixes[idx].coordinates[0])
		} else if (sec_name == "X2"){
			string s(size, 0);
			file >> s;
			matrixes[idx].coordinates[1] = stringToDouble(s);
			LOG_PRINT(logfile, "				X2: %g\n", matrixes[idx].coordinates[1])
		} else if (sec_name == "Y1"){
			string s(size, 0);
			file >> s;
			matrixes[idx].coordinates[2] = stringToDouble(s);
			LOG_PRINT(logfile, "				Y1: %g\n", matrixes[idx].coordinates[2])
		} else if (sec_name == "X1"){
			string s(size, 0);
			file >> s;
			matrixes[idx].coordinates[3] = stringToDouble(s);
			LOG_PRINT(logfile, "				X1: %g\n", matrixes[idx].coordinates[3])
		}

		//section_body_2_size
		LAYER += size + 0x1;
		file.seekg(LAYER, ios_base::beg);
		file >> size;

		//section_body_2
		LAYER += 0x5;
		if (sec_name == "COLORMAP"){
			file.seekg(LAYER + 0x14, ios_base::beg);
			readColorMap(matrixes[idx].colorMap);
		}

		//close section 00 00 00 00 0A
		LAYER += size + (size > 0 ? 0x1 : 0) + 0x5;
		file.seekg(LAYER, ios_base::beg);
		file >> size;
		if(size != sectionSize)
			break;
	}
	file.seekg(1, ios_base::cur);
	LAYER = file.tellg();
	file >> size;

	unsigned char c1, c2;
	file.seekg(LAYER + 0x23, ios_base::beg);
	file >> c1;
	file >> c2;

	matrixes[idx].valueTypeSpecification = c1/0x10;
	if(c2 >= 0x80){
		matrixes[idx].significantDigits = c2 - 0x80;
		matrixes[idx].numericDisplayType = SignificantDigits;
	} else if(c2 > 0){
		matrixes[idx].decimalPlaces = c2 - 0x03;
		matrixes[idx].numericDisplayType = DecimalPlaces;
	}

	LAYER += size + 0x06;
	file.seekg(LAYER, ios_base::beg);
	skipObjectInfo();
	LOG_PRINT(logfile, "		Done with matrix, pos @ 0x%X\n", (unsigned int)file.tellg())
}


void Origin700Parser::readGraphInfo()
{
	unsigned int POS = file.tellg();
	unsigned int size;
	file >> size;

	POS += 5;

	string name(25, 0);
	file.seekg(POS + 0x02, ios_base::beg);
	file >> name;
	LOG_PRINT(logfile, "		GRAPH name: %s cursor pos: 0x%X\n", name.c_str(), (unsigned int)file.tellg())

	graphs.push_back(Graph(name));
	file.seekg(POS, ios_base::beg);
	readWindowProperties(graphs.back(), size);

	file.seekg(POS + 0x23, ios_base::beg);
	file >> graphs.back().width;
	file >> graphs.back().height;

	unsigned int LAYER = POS;
	LAYER += size + 0x1;

	while(LAYER < d_file_size){// multilayer loop
		graphs.back().layers.push_back(GraphLayer());
		GraphLayer& layer(graphs.back().layers.back());

		// LAYER section
		file.seekg(LAYER, ios_base::beg);
		file >> size;

		LAYER += 0x05;

		file.seekg(LAYER + 0x0F, ios_base::beg);
		file >> layer.xAxis.min;
		file >> layer.xAxis.max;
		file >> layer.xAxis.step;

		file.seekg(LAYER + 0x2B, ios_base::beg);
		file >> layer.xAxis.majorTicks;

		file.seekg(LAYER + 0x37, ios_base::beg);
		file >> layer.xAxis.minorTicks;
		file >> layer.xAxis.scale;

		file.seekg(LAYER + 0x3A, ios_base::beg);
		file >> layer.yAxis.min;
		file >> layer.yAxis.max;
		file >> layer.yAxis.step;

		file.seekg(LAYER + 0x56, ios_base::beg);
		file >> layer.yAxis.majorTicks;

		file.seekg(LAYER + 0x62, ios_base::beg);
		file >> layer.yAxis.minorTicks;
		file >> layer.yAxis.scale;

		file.seekg(LAYER + 0x71, ios_base::beg);
		file.read(reinterpret_cast<char*>(&layer.clientRect), sizeof(Rect));

		unsigned char border;
		file.seekg(LAYER + 0x89, ios_base::beg);
		file >> border;
		layer.borderType = (BorderType)(border >= 0x80 ? border-0x80 : None);

		unsigned char col;
		file.seekg(LAYER + 0xA7, ios_base::beg);
		file >> col;
		layer.backgroundColor.type = (col & 0x01) ? Origin::Color::None : Origin::Color::Regular;
		file >> col;
		layer.backgroundColor.regular = col;

		LAYER += size + 0x1;
		file.seekg(LAYER, ios_base::beg);

		unsigned int sectionSize;
		file >> size;
		sectionSize = size;
		while(LAYER < d_file_size){
			LAYER += 0x5;

			//section_header

			string sec_name(41, 0);
			file.seekg(LAYER + 0x46, ios_base::beg);
			file >> sec_name;

			unsigned int sectionNamePos = LAYER + 0x46;
			LOG_PRINT(logfile, "				SECTION NAME: %s (@ 0x%X)\n", sec_name.c_str(), (LAYER + 0x46))

			Rect r;
			file.seekg(LAYER + 0x03, ios_base::beg);
			file.read(reinterpret_cast<char*>(&r), sizeof(Rect));

			unsigned char attach;
			file.seekg(LAYER + 0x28, ios_base::beg);
			file >> attach;

			unsigned char border;
			file >> border;

			Color color;
			file.seekg(LAYER + 0x33, ios_base::beg);
			file >> color;

			LAYER += size + 0x1;
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			//section_body_1
			LAYER += 0x5;
			unsigned int osize = size;

			file.seekg(LAYER, ios_base::beg);
			readGraphAxisPrefixSuffixInfo(sec_name, size, layer);

			unsigned char type;
			file >> type;

			//text properties
			short rotation;
			file.seekg(LAYER + 0x02, ios_base::beg);
			file >> rotation;

			unsigned char fontSize;
			file >> fontSize;

			unsigned char tab;
			file.seekg(LAYER + 0x0A, ios_base::beg);
			file >> tab;

			//line properties
			unsigned char lineStyle = 0;
			double width = 0.0;
			LineVertex begin, end;
			unsigned int w = 0;

			file.seekg(LAYER + 0x12, ios_base::beg);
			file >> lineStyle;

			unsigned short w1;
			file >> w1;
			width = (double)w1/500.0;

			file.seekg(LAYER + 0x20, ios_base::beg);
			file >> begin.x;
			file >> end.x;

			file.seekg(LAYER + 0x40, ios_base::beg);
			file >> begin.y;
			file >> end.y;

			file.seekg(LAYER + 0x60, ios_base::beg);
			file >> begin.shapeType;

			file.seekg(LAYER + 0x64, ios_base::beg);
			file >> w;
			begin.shapeWidth = (double)w/500.0;

			file >> w;
			begin.shapeLength = (double)w/500.0;

			file.seekg(LAYER + 0x6C, ios_base::beg);
			file >> end.shapeType;

			file.seekg(LAYER + 0x70, ios_base::beg);
			file >> w;
			end.shapeWidth = (double)w/500.0;

			file >> w;
			end.shapeLength = (double)w/500.0;

			Figure figure;
			file.seekg(LAYER + 0x05, ios_base::beg);
			file >> w1;
			figure.width = (double)w1/500.0;

			file.seekg(LAYER + 0x08, ios_base::beg);
			file >> figure.style;

			file.seekg(LAYER + 0x42, ios_base::beg);
			file >> figure.fillAreaColor;
			file >> w1;
			figure.fillAreaPatternWidth = (double)w1/500.0;

			file.seekg(LAYER + 0x4A, ios_base::beg);
			file >> figure.fillAreaPatternColor;
			file >> figure.fillAreaPattern;

			unsigned char h;
			file.seekg(LAYER + 0x57, ios_base::beg);
			file >> h;
			figure.useBorderColor = (h == 0x10);

			//section_body_2_size
			LAYER += size + 0x1;
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			//section_body_2
			LAYER += 0x5;

			file.seekg(1, ios_base::cur);
			if(sec_name == "XB")
			{
				string text(size, 0);
				file >> text;

				layer.xAxis.position = GraphAxis::Bottom;
				layer.xAxis.formatAxis[0].label = TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach);
			}
			else if(sec_name == "XT")
			{
				string text(size, 0);
				file >> text;

				layer.xAxis.position = GraphAxis::Top;
				layer.xAxis.formatAxis[1].label = TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach);
			}
			else if(sec_name == "YL")
			{
				string text(size, 0);
				file >> text;

				layer.yAxis.position = GraphAxis::Left;
				layer.yAxis.formatAxis[0].label = TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach);
			}
			else if(sec_name == "YR")
			{
				string text(size, 0);
				file >> text;

				layer.yAxis.position = GraphAxis::Right;
				layer.yAxis.formatAxis[1].label = TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach);
			}
			else if(sec_name == "ZF")
			{
				string text(size, 0);
				file >> text;

				layer.zAxis.position = GraphAxis::Front;
				layer.zAxis.formatAxis[0].label = TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach);
			}
			else if(sec_name == "ZB")
			{
				string text(size, 0);
				file >> text;

				layer.zAxis.position = GraphAxis::Back;
				layer.zAxis.formatAxis[1].label = TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach);
			}
			else if(sec_name == "3D")
			{
				file >> layer.zAxis.min;
				file >> layer.zAxis.max;
				file >> layer.zAxis.step;

				file.seekg(LAYER + 0x1C, ios_base::beg);
				file >> layer.zAxis.majorTicks;

				file.seekg(LAYER + 0x28, ios_base::beg);
				file >> layer.zAxis.minorTicks;
				file >> layer.zAxis.scale;

				file.seekg(LAYER + 0x218, ios_base::beg);
				file >> layer.xLength;
				file >> layer.yLength;
				file >> layer.zLength;

				layer.xLength /= 23.0;
				layer.yLength /= 23.0;
				layer.zLength /= 23.0;
			}
			else if(sec_name == "Legend")
			{
				string text(size, 0);
				file >> text;

				layer.legend = TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach);
			}
			else if(sec_name == "__BCO2") // histogram
			{
				file.seekg(LAYER + 0x10, ios_base::beg);
				file >> layer.histogramBin;

				file.seekg(LAYER + 0x20, ios_base::beg);
				file >> layer.histogramEnd;
				file >> layer.histogramBegin;

				unsigned int p = sectionNamePos + 81;
				file.seekg(p, ios_base::beg);

				file >> layer.percentile.p1SymbolType;
				file >> layer.percentile.p99SymbolType;
				file >> layer.percentile.meanSymbolType;
				file >> layer.percentile.maxSymbolType;
				file >> layer.percentile.minSymbolType;

				file.seekg(sectionNamePos + 94, ios_base::beg);
				file >> layer.percentile.whiskersRange;
				file >> layer.percentile.boxRange;

				file.seekg(sectionNamePos + 129, ios_base::beg);
				file >> layer.percentile.whiskersCoeff;
				file >> layer.percentile.boxCoeff;

				unsigned char h;
				file >> h;
				layer.percentile.diamondBox = (h == 0x82) ? true : false;

				p += 109;
				file.seekg(p, ios_base::beg);
				file >> layer.percentile.symbolSize;
				layer.percentile.symbolSize = layer.percentile.symbolSize/2 + 1;

				p += 163;
				file.seekg(p, ios_base::beg);
				file >> layer.percentile.symbolColor;
				file >> layer.percentile.symbolFillColor;
			}
			else if(sec_name == "vline") // Image profiles vertical cursor
			{
				file.seekg(sectionNamePos, ios_base::beg);
				for (int i = 0; i < 2; i++)
					skipLine();

				file.seekg(0x20, ios_base::cur);
				file >> layer.vLine;
				LOG_PRINT(logfile, "vLine: %g\n", layer.vLine)

				layer.imageProfileTool = true;
			}
			else if(sec_name == "hline") // Image profiles horizontal cursor
			{
				file.seekg(sectionNamePos, ios_base::beg);
				for (int i = 0; i < 2; i++)
					skipLine();

				file.seekg(0x40, ios_base::cur);
				file >> layer.hLine;
				LOG_PRINT(logfile, "hLine: %g @ 0x%X\n", layer.hLine, (unsigned int)file.tellg())

				layer.imageProfileTool = true;
			}
			else if(sec_name == "ZCOLORS")
			{
				layer.isXYY3D = true;
			}
			else if(sec_name == "SPECTRUM1")
			{
				layer.isXYY3D = false;

				unsigned char h;
				file.seekg(24, ios_base::cur);
				file >> h;
				layer.colorScale.reverseOrder = h;
				file.seekg(7, ios_base::cur);
				file >> layer.colorScale.colorBarThickness;
				file >> layer.colorScale.labelGap;
				file.seekg(56, ios_base::cur);
				file >> layer.colorScale.labelsColor;
			}
			else if(size && type == 0xE9) // text
			{
				string text(size, 0);
				file >> text;

				sec_name.resize(3);
				if (sec_name == "PIE")
					layer.pieTexts.push_back(TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach));
				else
					layer.texts.push_back(TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach));
			}
			else if(osize == 0x5E) // rectangle & circle
			{
				switch(type){
					case 0:
					case 1:
						figure.type = Figure::Rectangle;
						break;
					case 2:
					case 3:
						figure.type = Figure::Circle;
						break;
				}
				figure.clientRect = r;
				figure.attach = (Attach)attach;
				figure.color = color;

				layer.figures.push_back(figure);
			}
			else if(osize == 0x78 && type == 2) // line
			{
				layer.lines.push_back(Line());
				Line& line(layer.lines.back());
				line.color = color;
				line.clientRect = r;
				line.attach = (Attach)attach;
				line.width = width;
				line.style = lineStyle;
				line.begin = begin;
				line.end = end;
			}
			else if(osize == 0x28) // bitmap
			{
				if (type == 4){
					unsigned long filesize = size + 14;
					layer.bitmaps.push_back(Bitmap());
					Bitmap& bitmap(layer.bitmaps.back());
					bitmap.clientRect = r;
					bitmap.attach = (Attach)attach;
					bitmap.size = filesize;
					bitmap.borderType = (BorderType)(border >= 0x80 ? border-0x80 : None);
					bitmap.data = new unsigned char[filesize];
					unsigned char* data = bitmap.data;
					//add Bitmap header
					memcpy(data, "BM", 2);
					data += 2;
					memcpy(data, &filesize, 4);
					data += 4;
					unsigned int d = 0;
					memcpy(data, &d, 4);
					data += 4;
					d = 0x36;
					memcpy(data, &d, 4);
					data += 4;
					file.read(reinterpret_cast<char*>(data), size);
				} else if (type == 6){
					string gname(30, 0);
					file.seekg(sectionNamePos + 93, ios_base::beg);
					file >> gname;
					layer.bitmaps.push_back(Bitmap(gname));
					Bitmap& bitmap(layer.bitmaps.back());
					bitmap.clientRect = r;
					bitmap.attach = (Attach)attach;
					bitmap.size = 0;
					bitmap.borderType = (BorderType)(border >= 0x80 ? border-0x80 : None);
				}
			}

			//close section 00 00 00 00 0A
			LAYER += size + (size > 0 ? 0x1 : 0);

			//section_body_3_size
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			//section_body_3
			LAYER += 0x5;

			//close section 00 00 00 00 0A
			LAYER += size + (size > 0 ? 0x1 : 0);
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			if (!size || size != sectionSize)
				break;
		}

		LAYER += 0x5;
		unsigned char h;
		short w;

		file.seekg(LAYER, ios_base::beg);
		file >> size;
		if(size){//check layer is not empty
			while(LAYER < d_file_size){
				LAYER += 0x5;

				layer.curves.push_back(GraphCurve());
				GraphCurve& curve(layer.curves.back());
				file.seekg(LAYER + 0x4C, ios_base::beg);
				file >> curve.type;
				LOG_PRINT(logfile, "			graph %d layer %d curve %d type : %d\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), (int)curve.type)
				if (curve.type == GraphCurve::Mesh3D || curve.type == GraphCurve::Contour)
					layer.isXYY3D = false;

				file.seekg(LAYER + 0x04, ios_base::beg);
				file >> w;
				pair<string, string> column = findDataByIndex(w-1);
				short nColY = w;
				if(column.first.size() > 0){
					curve.dataName = column.first;
					if(layer.is3D()){
						LOG_PRINT(logfile, "			graph %d layer %d curve %d Z : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str())
						curve.zColumnName = column.second;
					} else {
						LOG_PRINT(logfile, "			graph %d layer %d curve %d Y : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str())
						curve.yColumnName = column.second;
					}
				}

				file.seekg(LAYER + 0x23, ios_base::beg);
				file >> w;
				column = findDataByIndex(w-1);
				if(column.first.size() > 0){
					if(curve.dataName != column.first)
						LOG_PRINT(logfile, "			graph %d X and Y from different tables\n", graphs.size())

					if(layer.is3D()){
						LOG_PRINT(logfile, "			graph %d layer %d curve %d Y : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str())
						curve.yColumnName = column.second;
					} else if (layer.isXYY3D){
						LOG_PRINT(logfile, "			graph %d layer %d curve %d X : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str())
						curve.xColumnName = column.second;
					} else {
						LOG_PRINT(logfile, "			graph %d layer %d curve %d X : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str())
						curve.xColumnName = column.second;
					}
				}

				file.seekg(LAYER + 0x4D, ios_base::beg);
				file >> w;
				column = findDataByIndex(w-1);
				if(column.first.size() > 0 && layer.is3D()){
					LOG_PRINT(logfile, "			graph %d layer %d curve %d X : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str())
					curve.xColumnName = column.second;
					if(curve.dataName != column.first)
						LOG_PRINT(logfile, "			graph %d X and Y from different tables\n", graphs.size())
				}

				if(layer.is3D() || layer.isXYY3D)
					graphs.back().is3D = true;

				file.seekg(LAYER + 0x11, ios_base::beg);
				file >> curve.lineConnect;
				file >> curve.lineStyle;

				file.seekg(1, ios_base::cur);
				file >> curve.boxWidth;

				file >> w;
				curve.lineWidth=(double)w/500.0;

				file.seekg(LAYER + 0x19, ios_base::beg);
				file >> w;
				curve.symbolSize=(double)w/500.0;

				file.seekg(LAYER + 0x1C, ios_base::beg);
				file >> h;
				curve.fillArea = (h==2);

				file.seekg(LAYER + 0x1E, ios_base::beg);
				file >> curve.fillAreaType;

				//text
				if(curve.type == GraphCurve::TextPlot){
					file.seekg(LAYER + 0x13, ios_base::beg);
					file >> curve.text.rotation;

					curve.text.rotation /= 10;
					file >> curve.text.fontSize;

					file.seekg(LAYER + 0x19, ios_base::beg);
					file >> h;
					switch(h){
						case 26:
							curve.text.justify = TextProperties::Center;
							break;
						case 2:
							curve.text.justify = TextProperties::Right;
							break;
						default:
							curve.text.justify = TextProperties::Left;
							break;
					}

					file >> h;
					curve.text.fontUnderline = (h & 0x1);
					curve.text.fontItalic = (h & 0x2);
					curve.text.fontBold = (h & 0x8);
					curve.text.whiteOut = (h & 0x20);

					char offset;
					file.seekg(LAYER + 0x37, ios_base::beg);
					file >> offset;
					curve.text.xOffset = offset * 5;
					file >> offset;
					curve.text.yOffset = offset * 5;
				}

				//vector
				if(curve.type == GraphCurve::FlowVector || curve.type == GraphCurve::Vector){
					file.seekg(LAYER + 0x56, ios_base::beg);
					file >> curve.vector.multiplier;

					file.seekg(LAYER + 0x5E, ios_base::beg);
					file >> h;

					column = findDataByIndex(nColY - 1 + h - 0x64);
					if(column.first.size() > 0)
						curve.vector.endXColumnName = column.second;

					file.seekg(LAYER + 0x62, ios_base::beg);
					file >> h;

					column = findDataByIndex(nColY - 1 + h - 0x64);
					if(column.first.size() > 0)
						curve.vector.endYColumnName = column.second;

					file.seekg(LAYER + 0x18, ios_base::beg);
					file >> h;

					if(h >= 0x64){
						column = findDataByIndex(nColY - 1 + h - 0x64);
						if(column.first.size() > 0)
							curve.vector.angleColumnName = column.second;
					} else if(h <= 0x08)
						curve.vector.constAngle = 45*h;

					file >> h;

					if(h >= 0x64){
						column = findDataByIndex(nColY - 1 + h - 0x64);
						if(column.first.size() > 0)
							curve.vector.magnitudeColumnName = column.second;
					} else
						curve.vector.constMagnitude = (int)curve.symbolSize;

					file.seekg(LAYER + 0x66, ios_base::beg);
					file >> curve.vector.arrowLenght;
					file >> curve.vector.arrowAngle;

					file >> h;
					curve.vector.arrowClosed = !(h & 0x1);

					file >> w;
					curve.vector.width=(double)w/500.0;

					file.seekg(LAYER + 0x142, ios_base::beg);
					file >> h;
					switch(h){
						case 2:
							curve.vector.position = VectorProperties::Midpoint;
							break;
						case 4:
							curve.vector.position = VectorProperties::Head;
							break;
						default:
							curve.vector.position = VectorProperties::Tail;
							break;
					}
				}

				//pie
				if (curve.type == GraphCurve::Pie){
					file.seekg(LAYER + 0x92, ios_base::beg);
					file >> h;

					curve.pie.formatPercentages = (h & 0x01);
					curve.pie.formatValues		= (h & 0x02);
					curve.pie.positionAssociate = (h & 0x08);
					curve.pie.clockwiseRotation = (h & 0x20);
					curve.pie.formatCategories	= (h & 0x80);

					file >> curve.pie.formatAutomatic;
					file >> curve.pie.distance;
					file >> curve.pie.viewAngle;

					file.seekg(LAYER + 0x98, ios_base::beg);
					file >> curve.pie.thickness;

					file.seekg(LAYER + 0x9A, ios_base::beg);
					file >> curve.pie.rotation;

					file.seekg(LAYER + 0x9E, ios_base::beg);
					file >> curve.pie.displacement;

					file.seekg(LAYER + 0xA0, ios_base::beg);
					file >> curve.pie.radius;
					file >> curve.pie.horizontalOffset;

					file.seekg(LAYER + 0xA6, ios_base::beg);
					file >> curve.pie.displacedSectionCount;
				}
				//surface
				if (layer.isXYY3D || curve.type == GraphCurve::Mesh3D){
					file.seekg(LAYER + 0x17, ios_base::beg);
					file >> curve.surface.type;
					file.seekg(LAYER + 0x1C, ios_base::beg);
					file >> h;
					if((h & 0x60) == 0x60)
						curve.surface.grids = SurfaceProperties::X;
					else if(h & 0x20)
						curve.surface.grids = SurfaceProperties::Y;
					else if(h & 0x40)
						curve.surface.grids = SurfaceProperties::None;
					else
						curve.surface.grids = SurfaceProperties::XY;

					curve.surface.sideWallEnabled = (h & 0x10);
					file >> curve.surface.frontColor;

					file.seekg(LAYER + 0x14C, ios_base::beg);
					file >> w;
					curve.surface.gridLineWidth = (double)w/500.0;
					file >> curve.surface.gridColor;

					file.seekg(LAYER + 0x13, ios_base::beg);
					file >> h;
					curve.surface.backColorEnabled = (h & 0x08);
					file.seekg(LAYER + 0x15A, ios_base::beg);
					file >> curve.surface.backColor;
					file >> curve.surface.xSideWallColor;
					file >> curve.surface.ySideWallColor;

					curve.surface.surface.fill = (h & 0x10);
					curve.surface.surface.contour = (h & 0x40);
					file.seekg(LAYER + 0x94, ios_base::beg);
					file >> w;
					curve.surface.surface.lineWidth = (double)w/500.0;
					file >> curve.surface.surface.lineColor;

					curve.surface.topContour.fill = (h & 0x02);
					curve.surface.topContour.contour = (h & 0x04);
					file.seekg(LAYER + 0xB4, ios_base::beg);
					file >> w;
					curve.surface.topContour.lineWidth = (double)w/500.0;
					file >> curve.surface.topContour.lineColor;

					curve.surface.bottomContour.fill = (h & 0x80);
					curve.surface.bottomContour.contour = (h & 0x01);
					file.seekg(LAYER + 0xA4, ios_base::beg);
					file >> w;
					curve.surface.bottomContour.lineWidth = (double)w/500.0;
					file >> curve.surface.bottomContour.lineColor;
				}

				if (curve.type == GraphCurve::Mesh3D || curve.type == GraphCurve::Contour){
					ColorMap& colorMap = (curve.type == GraphCurve::Mesh3D ? curve.surface.colorMap : curve.colorMap);
					file.seekg(LAYER + 0x13, ios_base::beg);
					file >> h;
					colorMap.fillEnabled = (h & 0x82);

					if (curve.type == GraphCurve::Contour){
						file.seekg(102, ios_base::cur);
						file >> curve.text.fontSize;

						file.seekg(7, ios_base::cur);
						file >> h;
						curve.text.fontUnderline = (h & 0x1);
						curve.text.fontItalic = (h & 0x2);
						curve.text.fontBold = (h & 0x8);
						curve.text.whiteOut = (h & 0x20);

						file.seekg(2, ios_base::cur);
						file >> curve.text.color;
					}
					file.seekg(LAYER + 0x258, ios_base::beg);
					readColorMap(colorMap);
				}

				file.seekg(LAYER + 0xC2, ios_base::beg);
				file >> curve.fillAreaColor;

				file >> w;
				curve.fillAreaPatternWidth=(double)w/500.0;

				file.seekg(LAYER + 0xCA, ios_base::beg);
				file >> curve.fillAreaPatternColor;

				file >> curve.fillAreaPattern;
				file >> curve.fillAreaPatternBorderStyle;
				file >> w;
				curve.fillAreaPatternBorderWidth=(double)w/500.0;
				file >> curve.fillAreaPatternBorderColor;

				file.seekg(LAYER + 0x16A, ios_base::beg);
				file >> curve.lineColor;
				if (curve.type != GraphCurve::Contour)
					curve.text.color = curve.lineColor;

				file.seekg(LAYER + 0x17, ios_base::beg);
				file >> curve.symbolType;

				file.seekg(LAYER + 0x12E, ios_base::beg);
				file >> curve.symbolFillColor;
				file >> curve.symbolColor;
				curve.vector.color = curve.symbolColor;

				file >> h;
				curve.symbolThickness = (h == 255 ? 1 : h);
				file >> curve.pointOffset;

				file.seekg(LAYER + 0x143, ios_base::beg);
				file >> h;
				curve.connectSymbols = (h&0x8);

				LAYER += size + 0x1;

				unsigned int newSize;
				file.seekg(LAYER, ios_base::beg);
				file >> newSize;

				LAYER += newSize + (newSize > 0 ? 0x1 : 0) + 0x5;

				file.seekg(LAYER, ios_base::beg);
				file >> newSize;

				if(newSize != size)
					break;
			}
		}

		LAYER += 0x5;
		//read axis breaks
		while(LAYER < d_file_size){
			file.seekg(LAYER, ios_base::beg);
			file >> size;
			if(size == 0x2D){
				LAYER += 0x5;
				file.seekg(LAYER + 2, ios_base::beg);
				file >> h;

				if(h == 2) {
					layer.xAxisBreak.minorTicksBefore = layer.xAxis.minorTicks;
					layer.xAxisBreak.scaleIncrementBefore = layer.xAxis.step;
					file.seekg(LAYER, ios_base::beg);
					readGraphAxisBreakInfo(layer.xAxisBreak);
				} else if(h == 4){
					layer.yAxisBreak.minorTicksBefore = layer.yAxis.minorTicks;
					layer.yAxisBreak.scaleIncrementBefore = layer.yAxis.step;
					file.seekg(LAYER, ios_base::beg);
					readGraphAxisBreakInfo(layer.yAxisBreak);
				}
				LAYER += 0x2D + 0x1;
			} else
				break;
		}

		LAYER += 0x5;

		file.seekg(LAYER, ios_base::beg);
		size = readGraphAxisInfo(layer.xAxis);
		LAYER += size*0x6;

		LAYER += 0x5;

		file.seekg(LAYER, ios_base::beg);
		readGraphAxisInfo(layer.yAxis);
		LAYER += size*0x6;

		LAYER += 0x5;

		file.seekg(LAYER, ios_base::beg);
		readGraphAxisInfo(layer.zAxis);
		LAYER += size*0x6;

		LAYER += 0x5;

		file.seekg(LAYER, ios_base::beg);
		file >> size;

		if(size == 0)
			break;
	}

	file.seekg(LAYER + 0x5, ios_base::beg);
}

OriginParser* createOrigin700Parser(const string& fileName)
{
	return new Origin700Parser(fileName);
}
