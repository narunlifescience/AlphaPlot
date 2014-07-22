/***************************************************************************
    File                 : Origin750Parser.cpp
    --------------------------------------------------------------------
    Copyright            : (C) 2007-2008 Alex Kargovsky, Stefan Gerlach,
						   Ion Vasilief
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

#include "Origin750Parser.h"

const char* colTypeNames[] = {"X", "Y", "Z", "XErr", "YErr", "Label", "None"};

Origin750Parser::Origin750Parser(const string& fileName)
:	file(fileName.c_str(), ios::binary)
{
	objectIndex = 0;
	d_colormap_offset = 0x258;
	windowsCount = 0;
}

bool Origin750Parser::parse()
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
			LOG_PRINT(logfile , "	WARNING : found strange valuesize of %d\n", (int)valuesize)
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

			// catch exception
			/*if(size>10000)
			size=1000;*/

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
			case 0x70E2:

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
				spread = findSpreadByName(/*sname*/name);

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
	for(unsigned int i = 0; i < speadSheets.size(); ++i)
	{
		if(speadSheets[i].multisheet)
		{
			LOG_PRINT(logfile, "		CONVERT SPREADSHEET \"%s\" to EXCEL\n", speadSheets[i].name.c_str())
			convertSpreadToExcel(i);
			--i;
		}
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
	while(1)
	{
		LOG_PRINT(logfile, "			reading	Header\n")
		// HEADER
		// check header
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
	}

	file.seekg(1, ios_base::cur);
	LOG_PRINT(logfile, "Some Origin params @ 0x%X:\n", (unsigned int)file.tellg())

	file >> c;
	while(c != 0)
	{
		LOG_PRINT(logfile, "		")
		while(c != '\n')
		{
			LOG_PRINT(logfile, "%c", c)
			file >> c;
		}
		double parvalue;
		file >> parvalue;
		LOG_PRINT(logfile, ": %g\n", parvalue);

		file.seekg(1, ios_base::cur);
		file >> c;
	}
	file.seekg(1 + 5, ios_base::cur);
	while(1)
	{
		//fseek(f,5+0x40+1,SEEK_CUR);
		int size;
		file >> size;
		if(size != 0x40)
			break;

		file.seekg(1, ios_base::cur);
		Rect rect;
		unsigned int coord;
		file >> coord;
		rect.left = coord;
		file >> coord;
		rect.top = coord;
		file >> coord;
		rect.right = coord;
		file >> coord;
		rect.bottom = coord;

		unsigned char state;
		file.seekg(0x8, ios_base::cur);
		file >> state;

		double creationDate, modificationDate;
		file.seekg(0x7, ios_base::cur);
		file >> creationDate;
		file >> modificationDate;

		file.seekg(0x8, ios_base::cur);
		file >> c;

		unsigned char labellen;
		file.seekg(0x3, ios_base::cur);
		file >> labellen;

		file.seekg(4, ios_base::cur);
		file >> size;

		file.seekg(1, ios_base::cur);

		string name(size, 0);
		file >> name;

		if(name == "ResultsLog")
		{
			file.seekg(1, ios_base::cur);
			file >> size;

			file.seekg(1, ios_base::cur);
			resultsLog.resize(size);
			file >> resultsLog;

			LOG_PRINT(logfile, "Results Log: %s\n", resultsLog.c_str())
			break;
		}
		else
		{
			notes.push_back(Note(name));
			notes.back().objectID = objectIndex;
			notes.back().frameRect = rect;
			notes.back().creationDate = doubleToPosixTime(creationDate);
			notes.back().modificationDate = doubleToPosixTime(modificationDate);

			if(c & 0x01)
				notes.back().title = Window::Label;
			else if(c & 0x02)
				notes.back().title = Window::Name;
			else
				notes.back().title = Window::Both;

			notes.back().hidden = (state & 0x40);

			++objectIndex;

			file.seekg(1, ios_base::cur);
			file >> size;

			file.seekg(1, ios_base::cur);

			if(labellen > 1)
			{
				file >> notes.back().label.assign(labellen-1, 0);
				file.seekg(1, ios_base::cur);
			}

			file >> notes.back().text.assign(size - labellen, 0);

			LOG_PRINT(logfile, "NOTE %d NAME: %s\n", notes.size(), notes.back().name.c_str())
			LOG_PRINT(logfile, "NOTE %d LABEL: %s\n", notes.size(), notes.back().label.c_str())
			LOG_PRINT(logfile, "NOTE %d TEXT: %s\n", notes.size(), notes.back().text.c_str())

			file.seekg(1, ios_base::cur);
		}
	}

	file.seekg(1 + 4*5 + 0x10 + 1, ios_base::cur);
	try
	{
		readProjectTree();
	}
	catch(...)
	{}
	LOG_PRINT(logfile, "Done parsing\n")
#ifndef NO_CODE_GENERATION_FOR_LOG
	fclose(logfile);
#endif // NO_CODE_GENERATION_FOR_LOG

	return true;
}

void Origin750Parser::readSpreadInfo()
{
	unsigned int POS = file.tellg();

	unsigned int size;
	file >> size;

	POS += 5;

	LOG_PRINT(logfile, "			[Spreadsheet SECTION (@ 0x%X)]\n", POS)

	// check spreadsheet name
	file.seekg(POS + 0x2, ios_base::beg);
	string name(25, 0);
	file >> name;

	int spread = findSpreadByName(name);
	speadSheets[spread].name = name;
	file.seekg(POS, ios_base::beg);
	readWindowProperties(speadSheets[spread], size);
	speadSheets[spread].loose = false;
	char c = 0;

	unsigned int LAYER = POS;
	{
		// LAYER section
		LAYER += size + 0x1 + 0x5/* length of block = 0x12D + '\n'*/ + 0x12D + 0x1;
		//now structure is next : section_header_size=0x6F(4 bytes) + '\n' + section_header(0x6F bytes) + section_body_1_size(4 bytes) + '\n' + section_body_1 + section_body_2_size(maybe=0)(4 bytes) + '\n' + section_body_2 + '\n'
		//possible sections: column formulas, __WIPR, __WIOTN, __LayerInfoStorage etc
		//section name(column name in formula case) starts with 0x46 position
		while(1)
		{
			//section_header_size=0x6F(4 bytes) + '\n'
			LAYER += 0x5;

			//section_header
			file.seekg(LAYER + 0x46, ios_base::beg);
			string sec_name(41, 0);
			file >> sec_name;

			LOG_PRINT(logfile, "				SECTION NAME: %s (@ 0x%X)\n", sec_name.c_str(), (LAYER + 0x46))

			//section_body_1_size
			LAYER += 0x6F + 0x1;
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			//section_body_1
			LAYER += 0x5;
			file.seekg(LAYER, ios_base::beg);
			//check if it is a formula
			int col_index = findSpreadColumnByName(spread, sec_name);
			if(col_index != -1)
			{
				file >> speadSheets[spread].columns[col_index].command.assign(size, 0);
				LOG_PRINT(logfile, "				Column: %s has formula: %s\n", sec_name.c_str(), speadSheets[spread].columns[col_index].command.c_str());
			}

			//section_body_2_size
			LAYER += size + 0x1;
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			//section_body_2
			LAYER += 0x5;

			//close section 00 00 00 00 0A
			LAYER += size + (size > 0 ? 0x1 : 0) + 0x5;

			if(sec_name == "__LayerInfoStorage")
				break;

		}
		LAYER += 0x5;
	}

	/////////////// COLUMN Types ///////////////////////////////////////////
	LOG_PRINT(logfile, "			Spreadsheet has %d columns\n", speadSheets[spread].columns.size())

	vector<SpreadColumn> header;
	while(1)
	{
		LAYER += 0x5;
		file.seekg(LAYER + 0x12, ios_base::beg);
		name.resize(12);
		file >> name;

		file.seekg(LAYER + 0x11, ios_base::beg);
		file >> c;

		short width=0;
		file.seekg(LAYER + 0x4A, ios_base::beg);
		file >> width;
		int col_index = findColumnByName(spread, name);
		if(col_index != -1)
		{
			SpreadColumn::ColumnType type;
			switch(c)
			{
				case 3:
					type = SpreadColumn::X;
					break;
				case 0:
					type = SpreadColumn::Y;
					break;
				case 5:
					type = SpreadColumn::Z;
					break;
				case 6:
					type = SpreadColumn::XErr;
					break;
				case 2:
					type = SpreadColumn::YErr;
					break;
				case 4:
					type = SpreadColumn::Label;
					break;
				default:
					type = SpreadColumn::NONE;
					break;
			}
			speadSheets[spread].columns[col_index].type = type;
			width/=0xA;
			if(width == 0)
				width = 8;
			speadSheets[spread].columns[col_index].width = width;

			unsigned char c1,c2;
			file.seekg(LAYER + 0x1E, ios_base::beg);
			file >> c1;
			file >> c2;

			switch(c1)
			{
			case 0x00: // Numeric	   - Dec1000
			case 0x09: // Text&Numeric - Dec1000
			case 0x10: // Numeric	   - Scientific
			case 0x19: // Text&Numeric - Scientific
			case 0x20: // Numeric	   - Engeneering
			case 0x29: // Text&Numeric - Engeneering
			case 0x30: // Numeric	   - Dec1,000
			case 0x39: // Text&Numeric - Dec1,000
				speadSheets[spread].columns[col_index].valueType = (c1%0x10 == 0x9) ? TextNumeric : Numeric;
				speadSheets[spread].columns[col_index].valueTypeSpecification = c1 / 0x10;
				if(c2 >= 0x80)
				{
					speadSheets[spread].columns[col_index].significantDigits = c2 - 0x80;
					speadSheets[spread].columns[col_index].numericDisplayType = SignificantDigits;
				}
				else if(c2 > 0)
				{
					speadSheets[spread].columns[col_index].decimalPlaces = c2 - 0x03;
					speadSheets[spread].columns[col_index].numericDisplayType = DecimalPlaces;
				}
				break;
			case 0x02: // Time
				speadSheets[spread].columns[col_index].valueType = Time;
				speadSheets[spread].columns[col_index].valueTypeSpecification = c2 - 0x80;
				break;
			case 0x03: // Date
				speadSheets[spread].columns[col_index].valueType = Date;
				speadSheets[spread].columns[col_index].valueTypeSpecification= c2 - 0x80;
				break;
			case 0x31: // Text
				speadSheets[spread].columns[col_index].valueType = Text;
				break;
			case 0x4: // Month
			case 0x34:
				speadSheets[spread].columns[col_index].valueType = Month;
				speadSheets[spread].columns[col_index].valueTypeSpecification = c2;
				break;
			case 0x5: // Day
			case 0x35:
				speadSheets[spread].columns[col_index].valueType = Day;
				speadSheets[spread].columns[col_index].valueTypeSpecification = c2;
				break;
			default: // Text
				speadSheets[spread].columns[col_index].valueType = Text;
				break;
			}
			LOG_PRINT(logfile, "				COLUMN \"%s\" type = %s(%d) (@ 0x%X)\n", speadSheets[spread].columns[col_index].name.c_str(), colTypeNames[type], (int)c, (LAYER + 0x11))
		}
		LAYER += 0x1E7 + 0x1;

		int size;
		file.seekg(LAYER, ios_base::beg);
		file >> size;

		LAYER += 0x5;
		if(size > 0)
		{
			if(col_index != -1)
			{
				file.seekg(LAYER, ios_base::beg);
				file >> speadSheets[spread].columns[col_index].comment.assign(size, 0);
			}
			LAYER += size + 0x1;
		}

		if(col_index != -1)
			header.push_back(speadSheets[spread].columns[col_index]);

		file.seekg(LAYER, ios_base::beg);
		file >> size;
		if(size != 0x1E7)
			break;
	}

	for (unsigned int i = 0; i < header.size(); i++)
		speadSheets[spread].columns[i] = header[i];

	LOG_PRINT(logfile, "		Done with spreadsheet %d POS (@ 0x%X)\n", spread, (unsigned int)file.tellg())

	file.seekg(LAYER + 0x5*0x6 + 0x1ED*0x12, ios_base::beg);
}

void Origin750Parser::readExcelInfo()
{
	unsigned int POS = file.tellg();

	unsigned int size;
	file >> size;

	POS += 5;

	LOG_PRINT(logfile, "			[EXCEL SECTION (@ 0x%X)]\n", POS)

	// check spreadsheet name
	string name(25, 0);
	file.seekg(POS + 0x2, ios_base::beg);
	file >> name;

	int iexcel = findExcelByName(name);
	excels[iexcel].name = name;
	file.seekg(POS, ios_base::beg);
	readWindowProperties(excels[iexcel], size);
	excels[iexcel].loose = false;
	char c = 0;

	unsigned int LAYER = POS;
	LAYER += size + 0x1;
	int isheet = 0;
	while(1)// multisheet loop
	{
		// LAYER section
		LAYER += 0x5/* length of block = 0x12D + '\n'*/ + 0x12D + 0x1;
		//now structure is next : section_header_size=0x6F(4 bytes) + '\n' + section_header(0x6F bytes) + section_body_1_size(4 bytes) + '\n' + section_body_1 + section_body_2_size(maybe=0)(4 bytes) + '\n' + section_body_2 + '\n'
		//possible sections: column formulas, __WIPR, __WIOTN, __LayerInfoStorage etc
		//section name(column name in formula case) starts with 0x46 position
		while(1)
		{
			//section_header_size=0x6F(4 bytes) + '\n'
			LAYER += 0x5;

			//section_header

			string sec_name(41, 0);
			file.seekg(LAYER + 0x46, ios_base::beg);
			file >> sec_name;

			LOG_PRINT(logfile, "				SECTION NAME: %s (@ 0x%X)", sec_name.c_str(), (LAYER + 0x46))

			//section_body_1_size
			LAYER += 0x6F + 0x1;
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			//section_body_1
			LAYER += 0x5;
			file.seekg(LAYER, ios_base::beg);
			//check if it is a formula
			int col_index = findExcelColumnByName(iexcel, isheet, sec_name);
			if(col_index!=-1)
			{
				file >> excels[iexcel].sheets[isheet].columns[col_index].command.assign(size, 0);
			}

			//section_body_2_size
			LAYER += size + 0x1;
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			//section_body_2
			LAYER += 0x5;

			//close section 00 00 00 00 0A
			LAYER += size + (size > 0 ? 0x1 : 0) + 0x5;

			if(sec_name == "__LayerInfoStorage")
				break;

		}
		LAYER += 0x5;

		/////////////// COLUMN Types ///////////////////////////////////////////
		LOG_PRINT(logfile, "			Excel sheet %d has %d columns\n", isheet, excels[iexcel].sheets[isheet].columns.size())

		while(1)
		{
			LAYER += 0x5;
			file.seekg(LAYER + 0x12, ios_base::beg);
			name.resize(12);
			file >> name;

			file.seekg(LAYER + 0x11, ios_base::beg);
			file >> c;

			short width=0;
			file.seekg(LAYER + 0x4A, ios_base::beg);
			file >> width;

			int col_index = findExcelColumnByName(iexcel, isheet, name);
			if(col_index != -1)
			{
				SpreadColumn::ColumnType type;
				switch(c)
				{
					case 3:
						type = SpreadColumn::X;
						break;
					case 0:
						type = SpreadColumn::Y;
						break;
					case 5:
						type = SpreadColumn::Z;
						break;
					case 6:
						type = SpreadColumn::XErr;
						break;
					case 2:
						type = SpreadColumn::YErr;
						break;
					case 4:
						type = SpreadColumn::Label;
						break;
					default:
						type = SpreadColumn::NONE;
						break;
				}
				excels[iexcel].sheets[isheet].columns[col_index].type = type;
				width/=0xA;
				if(width == 0)
					width = 8;
				excels[iexcel].sheets[isheet].columns[col_index].width = width;

				unsigned char c1,c2;
				file.seekg(LAYER + 0x1E, ios_base::beg);
				file >> c1;
				file >> c2;
				switch(c1)
				{
				case 0x00: // Numeric	   - Dec1000
				case 0x09: // Text&Numeric - Dec1000
				case 0x10: // Numeric	   - Scientific
				case 0x19: // Text&Numeric - Scientific
				case 0x20: // Numeric	   - Engeneering
				case 0x29: // Text&Numeric - Engeneering
				case 0x30: // Numeric	   - Dec1,000
				case 0x39: // Text&Numeric - Dec1,000
					excels[iexcel].sheets[isheet].columns[col_index].valueType = (c1%0x10 == 0x9) ? TextNumeric : Numeric;
					excels[iexcel].sheets[isheet].columns[col_index].valueTypeSpecification = c1 / 0x10;
					if(c2>=0x80)
					{
						excels[iexcel].sheets[isheet].columns[col_index].significantDigits = c2 - 0x80;
						excels[iexcel].sheets[isheet].columns[col_index].numericDisplayType = SignificantDigits;
					}
					else if(c2>0)
					{
						excels[iexcel].sheets[isheet].columns[col_index].decimalPlaces = c2 - 0x03;
						excels[iexcel].sheets[isheet].columns[col_index].numericDisplayType = DecimalPlaces;
					}
					break;
				case 0x02: // Time
					excels[iexcel].sheets[isheet].columns[col_index].valueType = Time;
					excels[iexcel].sheets[isheet].columns[col_index].valueTypeSpecification = c2 - 0x80;
					break;
				case 0x03: // Date
					excels[iexcel].sheets[isheet].columns[col_index].valueType = Date;
					excels[iexcel].sheets[isheet].columns[col_index].valueTypeSpecification = c2 - 0x80;
					break;
				case 0x31: // Text
					excels[iexcel].sheets[isheet].columns[col_index].valueType = Text;
					break;
				case 0x4: // Month
				case 0x34:
					excels[iexcel].sheets[isheet].columns[col_index].valueType = Month;
					excels[iexcel].sheets[isheet].columns[col_index].valueTypeSpecification = c2;
					break;
				case 0x5: // Day
				case 0x35:
					excels[iexcel].sheets[isheet].columns[col_index].valueType = Day;
					excels[iexcel].sheets[isheet].columns[col_index].valueTypeSpecification = c2;
					break;
				default: // Text
					excels[iexcel].sheets[isheet].columns[col_index].valueType = Text;
					break;
				}
				LOG_PRINT(logfile, "				COLUMN \"%s\" type = %d(%d) (@ 0x%X)\n", excels[iexcel].sheets[isheet].columns[col_index].name.c_str(), type, c, (LAYER + 0x11))
			}
			LAYER += 0x1E7 + 0x1;

			file.seekg(LAYER, ios_base::beg);
			file >> size;

			LAYER += 0x5;
			if(size > 0)
			{
				if(col_index != -1)
				{
					file.seekg(LAYER, ios_base::beg);
					file >> excels[iexcel].sheets[isheet].columns[col_index].comment.assign(size, 0);
				}
				LAYER += size + 0x1;
			}

			file.seekg(LAYER, ios_base::beg);
			file >> size;

			if(size != 0x1E7)
				break;
		}
		LOG_PRINT(logfile, "		Done with Excel %d\n", iexcel)

		//POS = LAYER+0x5*0x6+0x1ED*0x12;
		LAYER += 0x5*0x5 + 0x1ED*0x12;
		file.seekg(LAYER, ios_base::beg);
		file >> size;

		if(size == 0)
			break;

		++isheet;
	}

	file.seekg(LAYER + 0x5, ios_base::beg);
}

void Origin750Parser::readMatrixInfo()
{
	unsigned int POS = file.tellg();

	unsigned int size;
	file >> size;

	POS+=5;

	LOG_PRINT(logfile, "			[Matrix SECTION (@ 0x%X)]\n", POS)

	string name(25, 0);
	file.seekg(POS + 0x2, ios_base::beg);
	file >> name;

	int idx = findMatrixByName(name);
	matrixes[idx].name = name;
	file.seekg(POS, ios_base::beg);
	readWindowProperties(matrixes[idx], size);

	unsigned char h;
	file.seekg(POS + 0x87, ios_base::beg);
	file >> h;
	switch(h)
	{
	case 1:
		matrixes[idx].view = Matrix::ImageView;
		break;
	case 2:
		matrixes[idx].header = Matrix::XY;
		break;
	}

	unsigned int LAYER = POS;
	LAYER += size + 0x1;

	// LAYER section
	LAYER += 0x5;

	file.seekg(LAYER + 0x2B, ios_base::beg);
	file >> matrixes[idx].columnCount;

	file.seekg(LAYER + 0x52, ios_base::beg);
	file >> matrixes[idx].rowCount;

	LAYER += 0x12D + 0x1;
	//now structure is next : section_header_size=0x6F(4 bytes) + '\n' + section_header(0x6F bytes) + section_body_1_size(4 bytes) + '\n' + section_body_1 + section_body_2_size(maybe=0)(4 bytes) + '\n' + section_body_2 + '\n'
	//possible sections: column formulas, __WIPR, __WIOTN, __LayerInfoStorage
	//section name(column name in formula case) starts with 0x46 position
	while(1)
	{
		//section_header_size=0x6F(4 bytes) + '\n'
		LAYER += 0x5;

		//section_header
		string sec_name(41, 0);
		file.seekg(LAYER + 0x46, ios_base::beg);
		file >> sec_name;

		//section_body_1_size
		LAYER += 0x6F+0x1;
		file.seekg(LAYER, ios_base::beg);
		file >> size;

		//section_body_1
		LAYER += 0x5;
		//check if it is a formula
		if (sec_name == "MV"){
			file.seekg(LAYER, ios_base::beg);
			file >> matrixes[idx].command.assign(size, 0);
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
		if(sec_name == "COLORMAP")
		{
			file.seekg(LAYER + 0x14, ios_base::beg);
			readColorMap(matrixes[idx].colorMap);
		}

		//close section 00 00 00 00 0A
		LAYER += size + (size > 0 ? 0x1 : 0) + 0x5;

		if(sec_name == "__LayerInfoStorage")
			break;

	}
	LAYER += 0x5;

	while(1)
	{
		LAYER+=0x5;

		unsigned short width;
		file.seekg(LAYER + 0x2B, ios_base::beg);
		file >> width;

		width = (width-55)/0xA;
		if(width == 0)
			width = 8;
		matrixes[idx].width = width;

		unsigned char c1,c2;
		file.seekg(LAYER + 0x1E, ios_base::beg);
		file >> c1;
		file >> c2;

		matrixes[idx].valueTypeSpecification = c1/0x10;
		if(c2 >= 0x80)
		{
			matrixes[idx].significantDigits = c2-0x80;
			matrixes[idx].numericDisplayType = SignificantDigits;
		}
		else if(c2 > 0)
		{
			matrixes[idx].decimalPlaces = c2-0x03;
			matrixes[idx].numericDisplayType = DecimalPlaces;
		}

		LAYER += 0x1E7 + 0x1;

		file.seekg(LAYER, ios_base::beg);
		file >> size;

		LAYER += size + (size > 0 ? 0x1 : 0) + 0x5;

		file.seekg(LAYER, ios_base::beg);
		file >> size;

		if(size != 0x1E7)
			break;
	}

	file.seekg(LAYER + 0x5*0x5 + 0x1ED*0x12 + 0x5, ios_base::beg);
}

void Origin750Parser::readGraphInfo()
{
	unsigned int POS = file.tellg();

	unsigned int size;
	file >> size;
	POS += 5;

	LOG_PRINT(logfile, "			[Graph SECTION (@ 0x%X)]\n", POS)

	string name(25, 0);
	file.seekg(POS + 0x02, ios_base::beg);
	file >> name;

	graphs.push_back(Graph(name));
	file.seekg(POS, ios_base::beg);
	readWindowProperties(graphs.back(), size);

	file.seekg(POS + 0x23, ios_base::beg);
	file >> graphs.back().width;
	file >> graphs.back().height;

	unsigned int LAYER = POS;
	LAYER += size + 0x1;

	while(1)// multilayer loop
	{
		graphs.back().layers.push_back(GraphLayer());
		GraphLayer& layer(graphs.back().layers.back());
		// LAYER section
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

		file.seekg(LAYER + 0x105, ios_base::beg);
		file >> layer.backgroundColor;

		LAYER += 0x12D + 0x1;
		//now structure is next : section_header_size=0x6F(4 bytes) + '\n' + section_header(0x6F bytes) + section_body_1_size(4 bytes) + '\n' + section_body_1 + section_body_2_size(maybe=0)(4 bytes) + '\n' + section_body_2 + '\n'
		//possible sections: axes, legend, __BC02, _202, _231, _232, __LayerInfoStorage etc
		//section name starts with 0x46 position
		while(1)
		{
			//section_header_size=0x6F(4 bytes) + '\n'
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

			//section_body_1_size
			LAYER += 0x6F + 0x1;
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
			//check if it is a axis or legend

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

				unsigned int p = sectionNamePos + 93;
				file.seekg(p, ios_base::beg);

				file >> layer.percentile.p1SymbolType;
				file >> layer.percentile.p99SymbolType;
				file >> layer.percentile.meanSymbolType;
				file >> layer.percentile.maxSymbolType;
				file >> layer.percentile.minSymbolType;

				file.seekg(sectionNamePos + 106, ios_base::beg);
				file >> layer.percentile.whiskersRange;
				file >> layer.percentile.boxRange;

				file.seekg(sectionNamePos + 141, ios_base::beg);
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
			else if(osize == 0x3E) // text
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
				switch(type)
				{
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

			if(sec_name == "__LayerInfoStorage")
				break;

		}
		LAYER += 0x5;
		unsigned char h;
		short w;

		file.seekg(LAYER, ios_base::beg);
		file >> size;

		if(size == 0x1E7)//check layer is not empty
		{
			while(1){
				LAYER += 0x5;

				layer.curves.push_back(GraphCurve());
				GraphCurve& curve(layer.curves.back());
				file.seekg(LAYER + 0x4C, ios_base::beg);
				file >> curve.type;
				LOG_PRINT(logfile, "			graph %d layer %d curve %d type : %d\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), (int)curve.type)

				file.seekg(LAYER + 0x04, ios_base::beg);
				file >> w;
				pair<string, string> column = findDataByIndex(w-1);
				short nColY = w;
				if (column.first.size() > 0){
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
				if (column.first.size() > 0){
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
				if (column.first.size() > 0 && layer.is3D()){
					LOG_PRINT(logfile, "			graph %d layer %d curve %d X : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str())
					curve.xColumnName = column.second;
					if(curve.dataName != column.first) {
						LOG_PRINT(logfile, "			graph %d X and Y from different tables\n", graphs.size())
					}
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

					file.seekg(LAYER + 0x259, ios_base::beg);
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

				LAYER += 0x1E7 + 0x1;

				int size;
				file.seekg(LAYER, ios_base::beg);
				file >> size;

				LAYER += size + (size > 0 ? 0x1 : 0) + 0x5;

				file.seekg(LAYER, ios_base::beg);
				file >> size;

				if(size != 0x1E7)
					break;
			}
		}

		LAYER += 0x5;
		//read axis breaks
		while(1){
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			if(size == 0x2D){
				LAYER += 0x5;
				file.seekg(LAYER + 2, ios_base::beg);
				file >> h;

				if (h == 2) {
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
			}
			else
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

void Origin750Parser::readGraphGridInfo(GraphGrid& grid)
{
	unsigned int POS = file.tellg();

	unsigned char h;
	short w;

	file.seekg(POS + 0x26, ios_base::beg);
	file >> h;
	grid.hidden = (h == 0);

	file.seekg(POS + 0x0F, ios_base::beg);
	file >> grid.color;

	file.seekg(POS + 0x12, ios_base::beg);
	file >> grid.style;

	file.seekg(POS + 0x15, ios_base::beg);
	file >> w;
	grid.width = (double)w/500.0;
}

void Origin750Parser::readGraphAxisBreakInfo(GraphAxisBreak& axis_break)
{
	unsigned int POS = file.tellg();

	axis_break.show = true;

	file.seekg(POS + 0x0B, ios_base::beg);
	file >> axis_break.from;

	file >> axis_break.to;

	file >> axis_break.scaleIncrementAfter;

	file >> axis_break.position;

	unsigned char h;
	file >> h;
	axis_break.log10 = (h == 1);

	file >> axis_break.minorTicksAfter;
}

void Origin750Parser::readGraphAxisFormatInfo(GraphAxisFormat& format)
{
	unsigned int POS = file.tellg();

	unsigned char h;
	short w;

	file.seekg(POS + 0x26, ios_base::beg);
	file >> h;
	format.hidden = (h == 0);

	file.seekg(POS + 0x0F, ios_base::beg);
	file >> format.color;

	file.seekg(POS + 0x4A, ios_base::beg);
	file >> w;
	format.majorTickLength = (double)w/10.0;

	file.seekg(POS + 0x15, ios_base::beg);
	file >> w;
	format.thickness = (double)w/500.0;

	file.seekg(POS + 0x25, ios_base::beg);
	file >> h;

	format.minorTicksType = (h>>6);
	format.majorTicksType = ((h>>4) & 3);
	format.axisPosition = (h & 0x0F);
	switch(format.axisPosition) // need for testing
	{
	case 1:
		file.seekg(POS + 0x37, ios_base::beg);
		file >> h;
		format.axisPositionValue = (double)h;
		break;
	case 2:
		file.seekg(POS + 0x2F, ios_base::beg);
		file >> format.axisPositionValue;
		break;
	}
}

void Origin750Parser::readGraphAxisTickLabelsInfo(GraphAxisTick& tick)
{
	unsigned int POS = file.tellg();

	unsigned char h;
	unsigned char h1;
	short w;

	file.seekg(POS + 0x26, ios_base::beg);
	file >> h;
	tick.hidden = (h == 0);

	file.seekg(POS + 0x0F, ios_base::beg);
	file >> tick.color;

	file.seekg(POS + 0x13, ios_base::beg);
	file >> w;
	tick.rotation = w/10;

	file >> tick.fontSize;

	file.seekg(POS + 0x1A, ios_base::beg);
	file >> h;
	tick.fontBold = (h & 0x08);

	file.seekg(POS + 0x23, ios_base::beg);
	file >> w;
	file >> h;
	file >> h1;
	tick.valueType = (ValueType)(h & 0x0F);

	pair<string, string> column;
	switch(tick.valueType)
	{
	case Numeric:

		/*switch((h>>4))
		{
		case 0x9:
		tick.valueTypeSpecification=1;
		break;
		case 0xA:
		tick.valueTypeSpecification=2;
		break;
		case 0xB:
		tick.valueTypeSpecification=3;
		break;
		default:
		tick.valueTypeSpecification=0;
		}*/
		if((h>>4) > 7)
		{
			tick.valueTypeSpecification = (h>>4) - 8;
			tick.decimalPlaces = h1 - 0x40;
		}
		else
		{
			tick.valueTypeSpecification = (h>>4);
			tick.decimalPlaces = -1;
		}

		break;
	case Time:
	case Date:
	case Month:
	case Day:
	case ColumnHeading:
		tick.valueTypeSpecification = h1 - 0x40;
		break;
	case Text:
	case TickIndexedDataset:
	case Categorical:
		column = findDataByIndex(w-1);
		if(column.first.size() > 0)
		{
			tick.dataName = column.first;
			tick.columnName = column.second;
		}
		break;
	default: // Numeric Decimal 1.000
		tick.valueType = Numeric;
		tick.valueTypeSpecification = 0;
		break;
	}
}

unsigned int Origin750Parser::readGraphAxisInfo(GraphAxis& axis)
{
	unsigned int POS = file.tellg();
	unsigned int size;
	file >> size;

	POS += 0x5;
	file.seekg(POS, ios_base::beg);
	readGraphGridInfo(axis.minorGrid);
	POS += size + 1;

	POS += 0x5;
	file.seekg(POS, ios_base::beg);
	readGraphGridInfo(axis.majorGrid);
	POS += size + 1;

	POS += 0x5;
	file.seekg(POS, ios_base::beg);
	readGraphAxisTickLabelsInfo(axis.tickAxis[0]);
	POS += size + 1;

	POS += 0x5;
	file.seekg(POS, ios_base::beg);
	readGraphAxisFormatInfo(axis.formatAxis[0]);
	POS += size + 1;

	POS += 0x5;
	file.seekg(POS, ios_base::beg);
	readGraphAxisTickLabelsInfo(axis.tickAxis[1]);
	POS += size + 1;

	POS += 0x5;
	file.seekg(POS, ios_base::beg);
	readGraphAxisFormatInfo(axis.formatAxis[1]);

	return (size + 1 + 0x5);
}

void Origin750Parser::readProjectTree()
{
	readProjectTreeFolder(projectTree.begin());
	LOG_PRINT(logfile, "Project has %d windows\n", windowsCount)
	LOG_PRINT(logfile, "Origin project Tree\n")

	for(tree<ProjectNode>::iterator it = projectTree.begin(projectTree.begin()); it != projectTree.end(projectTree.begin()); ++it)
	{
		LOG_PRINT(logfile, "%s\n", (string(projectTree.depth(it) - 1, ' ') + (*it).name).c_str())
	}

	vector<Origin::Matrix> validMatrices;
	for(unsigned int i = 0; i < matrixes.size(); ++i){
		Matrix m = matrixes[i];
		if (m.objectID >= 0)
			validMatrices.push_back(m);
	}
	matrixes.clear();
	matrixes = validMatrices;
}

void Origin750Parser::readProjectTreeFolder(tree<ProjectNode>::iterator parent)
{
	unsigned int POS = file.tellg();

	double creationDate, modificationDate;
	POS += 5;

	file.seekg(POS + 0x10, ios_base::beg);
	file >> creationDate;
	if (creationDate >= 1e10)
		return;

	file >> modificationDate;
	if (modificationDate >= 1e10)
		return;

	POS += 0x20 + 1 + 5;
	unsigned int size;
	file.seekg(POS, ios_base::beg);
	file >> size;

	POS += 5;

	// read folder name
	string name(size, 0);
	file.seekg(POS, ios_base::beg);
	file >> name;

	tree<ProjectNode>::iterator current_folder = projectTree.append_child(parent, ProjectNode(name, ProjectNode::Folder, doubleToPosixTime(creationDate), doubleToPosixTime(modificationDate)));
	POS += size + 1 + 5 + 5;

	unsigned int objectcount;
	file.seekg(POS, ios_base::beg);
	file >> objectcount;

	windowsCount += objectcount;

	POS += 5 + 5;

	for(unsigned int i = 0; i < objectcount; ++i){
		POS += 5;
		char c;
		file.seekg(POS + 0x2, ios_base::beg);
		file >> c;

		unsigned int objectID;
		file.seekg(POS + 0x4, ios_base::beg);
		file >> objectID;

		if (c == 0x10)
			projectTree.append_child(current_folder, ProjectNode(notes[objectID].name, ProjectNode::Note));
		else {
			pair<ProjectNode::NodeType, string> object = findObjectByIndex(objectID);
			projectTree.append_child(current_folder, ProjectNode(object.second, object.first));
		}

		POS += 8 + 1 + 5 + 5;
	}

	file.seekg(POS, ios_base::beg);
	file >> objectcount;

	file.seekg(1, ios_base::cur);
	for(unsigned int i = 0; i < objectcount; ++i)
		readProjectTreeFolder(current_folder);
}

void Origin750Parser::readWindowProperties(Window& window, unsigned int size)
{
	unsigned int POS = file.tellg();

	window.objectID = objectIndex;
	++objectIndex;

	file.seekg(POS + 0x1B, ios_base::beg);
	file.read(reinterpret_cast<char*>(&window.frameRect), sizeof(window.frameRect));

	char c;
	file.seekg(POS + 0x32, ios_base::beg);
	file >> c;

	if(c & 0x01)
		window.state = Window::Minimized;
	else if(c & 0x02)
		window.state = Window::Maximized;

	file.seekg(POS + 0x69, ios_base::beg);
	file >> c;

	if(c & 0x01)
		window.title = Window::Label;
	else if(c & 0x02)
		window.title = Window::Name;
	else
		window.title = Window::Both;

	window.hidden = (c & 0x08);
	if(window.hidden)
	{
		LOG_PRINT(logfile, "			WINDOW %d NAME : %s	is hidden\n", objectIndex, window.name.c_str())
	}

	double creationDate, modificationDate;
	file.seekg(POS + 0x73, ios_base::beg);
	file >> creationDate;
	if (creationDate > 1e4 && creationDate < 1e8)
		window.creationDate = doubleToPosixTime(creationDate);
	else
		return;

	file >> modificationDate;
	if (modificationDate > 1e4 && modificationDate < 1e8)
		window.modificationDate = doubleToPosixTime(modificationDate);
	else
		return;

	if(size > 0xC3)
	{
		unsigned int labellen = 0;
		file.seekg(POS + 0xC3, ios_base::beg);
		file >> c;
		while(c != '@')
		{
			file >> c;
			++labellen;
		}
		if(labellen > 0)
		{
			file.seekg(POS + 0xC3, ios_base::beg);
			file >> window.label.assign(labellen, 0);
		}

		LOG_PRINT(logfile, "			WINDOW %d LABEL: %s\n", objectIndex, window.label.c_str())
	}
}

void Origin750Parser::readColorMap(ColorMap& colorMap)
{
	unsigned char h;
	short w;
	unsigned int colorMapSize;
	file >> colorMapSize;

	file.seekg(0x110, ios_base::cur);
	for(unsigned int i = 0; i < colorMapSize + 3; ++i){
		ColorMapLevel level;
		file >> level.fillPattern;

		file.seekg(0x03, ios_base::cur);
		file >> level.fillPatternColor;
		file >> w;
		level.fillPatternLineWidth = (double)w/500.0;

		file.seekg(0x06, ios_base::cur);
		file >> level.lineStyle;

		file.seekg(0x01, ios_base::cur);
		file >> w;
		level.lineWidth = (double)w/500.0;
		file >> level.lineColor;

		file.seekg(0x02, ios_base::cur);
		file >> h;
		level.labelVisible = (h & 0x1);
		level.lineVisible = !(h & 0x2);

		file.seekg(0x0D, ios_base::cur);
		file >> level.fillColor;
		file.seekg(0x04, ios_base::cur);
		double value;
		file >> value;

		colorMap.levels.push_back(make_pair(value, level));
	}
}

void Origin750Parser::readGraphAxisPrefixSuffixInfo(const string& sec_name, unsigned int size, GraphLayer& layer)
{
	if(sec_name == "PL"){
		string text(size, 0);
		file >> text;
		layer.yAxis.formatAxis[0].prefix = text;
	} else if(sec_name == "PR"){
		string text(size, 0);
		file >> text;
		layer.yAxis.formatAxis[1].prefix = text;
	} else if(sec_name == "PB"){
		string text(size, 0);
		file >> text;
		layer.xAxis.formatAxis[0].prefix = text;
	} else if(sec_name == "PT"){
		string text(size, 0);
		file >> text;
		layer.xAxis.formatAxis[1].prefix = text;
	} if(sec_name == "SL"){
		string text(size, 0);
		file >> text;
		layer.yAxis.formatAxis[0].suffix = text;
	} else if(sec_name == "SR"){
		string text(size, 0);
		file >> text;
		layer.yAxis.formatAxis[1].suffix = text;
	} else if(sec_name == "SB"){
		string text(size, 0);
		file >> text;
		layer.xAxis.formatAxis[0].suffix = text;
	} else if(sec_name == "ST"){
		string text(size, 0);
		file >> text;
		layer.xAxis.formatAxis[1].suffix = text;
	}
}

void Origin750Parser::skipLine()
{
	unsigned char c;
	file >> c;
	unsigned int POS = file.tellg();

	while(c != '\n'){
		file >> c;
		POS++;
		if (POS >= d_file_size)
			break;
	}
}

OriginParser* createOrigin750Parser(const string& fileName)
{
	return new Origin750Parser(fileName);
}
