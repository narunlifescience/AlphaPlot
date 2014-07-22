/***************************************************************************
	File                 : Origin800Parser.cpp
    --------------------------------------------------------------------
	Copyright            : (C) 2010 - 2011 Ion Vasilief
	Email (use @ for *)  : ion_vasilief*yahoo.fr
	Description          : Origin 8.0 file parser class (uses code from file
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

#include "Origin800Parser.h"

Origin800Parser::Origin800Parser(const string& fileName)
:	Origin750Parser(fileName)
{
	d_colormap_offset = 0x25B;
	notes_pos_mark = "H";
}

bool Origin800Parser::parse()
{
	if (fileVersion >= 2881)
		d_colormap_offset = 0x25F;

	unsigned int dataIndex = 0;

#ifndef NO_LOG_FILE
	// append progress in log file
	logfile = fopen("opjfile.log", "a");
#endif

	/////////////////// find column ///////////////////////////////////////////////////////////
	skipLine();

	unsigned int size;
	file >> size;

	file.seekg(1 + size + 1 + 5, ios_base::cur);
	file >> size;

	file.seekg(1, ios_base::cur);
	LOG_PRINT(logfile, "	[column found = %d/0x%X @ 0x%X]\n", size, size, (unsigned int) file.tellg());

	unsigned int colpos = file.tellg();
	unsigned int current_col = 1, nr = 0, nbytes = 0;
	unsigned int col_index = 0;
	unsigned int current_sheet = 0;
	
	while(size > 0 && size <= 0x8C){// should be 0x72, 0x73 or 0x83 ?
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

		LOG_PRINT(logfile, "	[valuesize = %d @ 0x%X]\n", (int)valuesize, ((unsigned int) file.tellg()-1));
		if(valuesize <= 0){
			LOG_PRINT(logfile, "	WARNING : found strange valuesize of %d\n", (int)valuesize);
			valuesize = 10;
		}

		file.seekg(oldpos + 0x58, ios_base::beg);
		LOG_PRINT(logfile, "	[Spreadsheet @ 0x%X]\n", (unsigned int) file.tellg());

		string name(25, 0);
		file >> name;

		string::size_type pos = name.find_last_of("_");
		string columnname;
		if(pos != string::npos){
			columnname = name.substr(pos + 1);
			name.resize(pos);
		}

		LOG_PRINT(logfile, "NAME: %s\n", name.c_str());

		unsigned int spread = 0;
		if(columnname.empty()){
			LOG_PRINT(logfile, "	NO COLUMN NAME FOUND! Must be a Matrix or Function.");
			////////////////////////////// READ matrices or functions ////////////////////////////////////

			LOG_PRINT(logfile, "	[position @ 0x%X]\n", (unsigned int) file.tellg());
			// TODO
			short signature;
			file >> signature;
			LOG_PRINT(logfile, "	SIGNATURE : %02X \n", signature);


			file.seekg(oldpos + size + 1, ios_base::beg);
			file >> size;
			file.seekg(1, ios_base::cur);
			size /= valuesize;
			LOG_PRINT(logfile, "	SIZE = %d\n", size);

			if (signature == 0x50C8 && size != 1)
				signature = 0x50CA;//is a matrix

			switch(signature)
			{
			case 0x50CA:
			case 0x51CA:
			case 0x70CA:
			case 0x50F2:
			case 0x51E2:
			case 0x51F2:
			case 0x51DC:
			case 0x50E2:
			case 0x50E7:
			case 0x50DB:
			case 0x50DC:
			case 0x70E2:
			{
				int mIndex = -1;
				pos = name.find_first_of("@");
				if (pos != string::npos){
					string sheetName = name;
					name.resize(pos);
					mIndex = findMatrixByName(name);
					if (mIndex != -1){
						LOG_PRINT(logfile, "	NEW MATRIX SHEET\n");
						matrices[mIndex].sheets.push_back(MatrixSheet(sheetName, dataIndex));
					}
				} else {
					LOG_PRINT(logfile, "	NEW MATRIX\n");
					matrices.push_back(Matrix(name));
					matrices.back().sheets.push_back(MatrixSheet(name, dataIndex));
				}

				++dataIndex;
				readMatrixValues(data_type, data_type_u, valuesize, size, mIndex);
				break;
			}

			case 0x10C8:
			case 0x50C8:
				functions.push_back(Function(name, dataIndex));
				++dataIndex;
				readFunction(colpos, valuesize, &oldpos);
				break;

			default:
				LOG_PRINT(logfile, "	UNKNOWN SIGNATURE: %.2X SKIP DATA\n", signature);
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
				LOG_PRINT(logfile, "	NEW SPREADSHEET\n");
				current_col = 1;
				speadSheets.push_back(SpreadSheet(name));
				spread = speadSheets.size() - 1;
				speadSheets.back().maxRows = 0;
				current_sheet = 0;
			}
			else
			{
				spread = findSpreadByName(name);
				current_col = speadSheets[spread].columns.size();
				if(!current_col)
					current_col = 1;
				++current_col;
			}
			speadSheets[spread].columns.push_back(SpreadColumn(columnname, dataIndex));
			speadSheets[spread].columns.back().colIndex = ++col_index;

			string::size_type sheetpos = speadSheets[spread].columns.back().name.find_last_of("@");
			if(sheetpos != string::npos){
				unsigned int sheet = atoi(columnname.substr(sheetpos + 1).c_str());
				if( sheet > 1){
					speadSheets[spread].columns.back().name = columnname;

					if (current_sheet != (sheet - 1))
						current_sheet = sheet - 1;

					speadSheets[spread].columns.back().sheet = current_sheet;
					if (speadSheets[spread].sheets < sheet)
						speadSheets[spread].sheets = sheet;
				}
			}
			LOG_PRINT(logfile, "	SPREADSHEET = %s SHEET = %d COLUMN NAME = %s (%d) INDEX = %d (@0x%X)\n", name.c_str(), speadSheets[spread].columns.back().sheet, columnname.c_str(), current_col, speadSheets[spread].columns.back().colIndex, (unsigned int)file.tellg());

			++dataIndex;

			////////////////////////////// SIZE of column /////////////////////////////////////////////
			file.seekg(oldpos + size + 1, ios_base::beg);

			file >> nbytes;
			if(fmod(nbytes, (double)valuesize)>0){
				LOG_PRINT(logfile, "WARNING: data section could not be read correctly");
			}
			nr = nbytes / valuesize;
			LOG_PRINT(logfile, "	[number of rows = %d (%d Bytes) @ 0x%X]\n", nr, nbytes, (unsigned int)file.tellg());

			speadSheets[spread].maxRows<nr ? speadSheets[spread].maxRows=nr : 0;

			////////////////////////////////////// DATA ////////////////////////////////////////////////
			file.seekg(1, ios_base::cur);

			LOG_PRINT(logfile, "	[data @ 0x%X]\n", (unsigned int)file.tellg());
			for(unsigned int i = 0; i < nr; ++i){
				double value;
				if(valuesize <= 8) // Numeric, Time, Date, Month, Day
				{
					file >> value;
					LOG_PRINT(logfile, "%g ", value);
					speadSheets[spread].columns[current_col - 1].data.push_back(value);
				}
				else if((data_type & 0x100) == 0x100) // Text&Numeric
				{
					unsigned char c;
					file >> c;
					file.seekg(1, ios_base::cur);
					if(c == 0) //value
					{
						file >> value;
						LOG_PRINT(logfile, "%g ", value);
						speadSheets[spread].columns[current_col - 1].data.push_back(value);
						file.seekg(valuesize - 10, ios_base::cur);
					}
					else //text
					{
						string stmp(valuesize - 2, 0);
						file >> stmp;
						if(stmp.find(0x0E) != string::npos) // try find non-printable symbol - garbage test
							stmp = string();
						speadSheets[spread].columns[current_col - 1].data.push_back(stmp);
						LOG_PRINT(logfile, "%s ", stmp.c_str());
					}
				}
				else //text
				{
					string stmp(valuesize, 0);
					file >> stmp;
					if(stmp.find(0x0E) != string::npos) // try find non-printable symbol - garbage test
						stmp = string();
					speadSheets[spread].columns[current_col - 1].data.push_back(stmp);
					LOG_PRINT(logfile, "%s ", stmp.c_str());
				}
			}
		}

		if(nbytes > 0 || columnname.empty())
		{
			file.seekg(1, ios_base::cur);
		}

		file >> size;
		file.seekg(1 + size + (size > 0 ? 1 : 0), ios_base::cur);

		file >> size;

		file.seekg(1, ios_base::cur);
		LOG_PRINT(logfile, "\n	[column found = %d/0x%X (@ 0x%X)]\n", size, size, ((unsigned int) file.tellg()-5));
		colpos = file.tellg();
	}

	////////////////////////////////////////////////////////////////////////////
	////////////////////// HEADER SECTION //////////////////////////////////////

	unsigned int POS = (unsigned int)file.tellg()-11;
	LOG_PRINT(logfile, "\nHEADER SECTION");
	LOG_PRINT(logfile, "	nr_spreads = %d\n", speadSheets.size());
	LOG_PRINT(logfile, "	[position @ 0x%X]\n", POS);

	POS += 0xB;
	file.seekg(POS, ios_base::beg);
	while(!file.eof()){
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

	file.seekg(1, ios_base::cur);
	readParameters();

	file.seekg(1 + 5, ios_base::cur);
	readNotes();
	// If file has no Note windows, last function will read to EOF, skipping the info for ResultsLog and ProjectTree.
	// As we know there is always a ResultsLog window after the Note windows, we better rewind to the start of Notes.
	file.seekg(POS, ios_base::beg);
	readResultsLog();

	file.seekg(1 + 4*5 + 0x10 + 1, ios_base::cur);
	try {
		readProjectTree();
	} catch(...) {}

	LOG_PRINT(logfile, "Done parsing\n")
#ifndef NO_LOG_FILE
	fclose(logfile);
#endif

	return true;
}

void Origin800Parser::readNotes()
{
	if (file.eof())
		return;

	unsigned int pos = findStringPos(notes_pos_mark);
	file.seekg(pos, ios_base::beg);

	unsigned int sectionSize;
	file >> sectionSize;
	while(!file.eof()){
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

		if (!rect.bottom || !rect.right)
			break;

		unsigned char state;
		file.seekg(0x8, ios_base::cur);
		file >> state;

		double creationDate, modificationDate;
		file.seekg(0x7, ios_base::cur);
		file >> creationDate;
		file >> modificationDate;

		file.seekg(0x8, ios_base::cur);
		unsigned char c;
		file >> c;

		unsigned int labellen;
		file.seekg(0x3, ios_base::cur);
		file >> labellen;

		skipLine();

		unsigned int size;
		file >> size;
		file.seekg(1, ios_base::cur);

		string name(size, 0);
		file >> name;

		notes.push_back(Note(name));
		notes.back().objectID = objectIndex;
		++objectIndex;

		notes.back().frameRect = rect;
		if (creationDate >= 1e10)
			return;
		notes.back().creationDate = doubleToPosixTime(creationDate);
		if (modificationDate >= 1e10)
			return;
		notes.back().modificationDate = doubleToPosixTime(modificationDate);

		if(c == 0x01)
			notes.back().title = Window::Label;
		else if(c == 0x02)
			notes.back().title = Window::Name;
		else
			notes.back().title = Window::Both;

		if(state == 0x07)
			notes.back().state = Window::Minimized;
		else if(state == 0x0b)
			notes.back().state = Window::Maximized;

		notes.back().hidden = (state & 0x40);

		file.seekg(1, ios_base::cur);
		file >> size;

		file.seekg(1, ios_base::cur);

		if(labellen > 1){
			file >> notes.back().label.assign(labellen - 1, 0);
			file.seekg(1, ios_base::cur);
		}

		file >> notes.back().text.assign(size - labellen, 0);

		LOG_PRINT(logfile, "NOTE %d NAME: %s\n", notes.size(), notes.back().name.c_str());
		LOG_PRINT(logfile, "NOTE %d LABEL: %s\n", notes.size(), notes.back().label.c_str());
		LOG_PRINT(logfile, "NOTE %d TEXT: %s\n", notes.size(), notes.back().text.c_str());

		file.seekg(1, ios_base::cur);

		file >> size;
		if(size != sectionSize)
			break;
	}
}

void Origin800Parser::readResultsLog()
{
	int pos = findStringPos("ResultsLog");
	if (pos < 0)
		return;

	file.seekg(pos + 12, ios_base::beg);
	unsigned int size;
	file >> size;

	file.seekg(1, ios_base::cur);
	resultsLog.resize(size);
	file >> resultsLog;
	LOG_PRINT(logfile, "Results Log: %s\n", resultsLog.c_str());
}

void Origin800Parser::readSpreadInfo()
{
	unsigned int POS = file.tellg();
	unsigned int size;
	file >> size;

	POS += 5;

	// check spreadsheet name
	file.seekg(POS + 0x2, ios_base::beg);
	string name(25, 0);
	file >> name;
	LOG_PRINT(logfile, "		SPREADSHEET: %s (@ 0x%X)]\n", name.c_str(), (unsigned int)file.tellg());

	int spread = findSpreadByName(name);
	speadSheets[spread].name = name;
	file.seekg(POS, ios_base::beg);
	readWindowProperties(speadSheets[spread], size);
	speadSheets[spread].loose = false;
	char c = 0;

	unsigned int LAYER = POS + size + 0x1;
	file.seekg(LAYER, ios_base::beg);
	file >> size;

	vector<SpreadColumn> header;
	int sheets = speadSheets[spread].sheets;
	for (int i = 0; i < sheets; i++){
		LAYER += size + 0x6;
		file.seekg(LAYER, ios_base::beg);
		file >> size;
		LOG_PRINT(logfile, "			SHEET: %d  (@ 0x%X)\n", (i + 1), (unsigned int)file.tellg());

		// LAYER section
		unsigned int sectionSize = size;
		while(size && !file.eof()){
			//section_header_size=0x6F(4 bytes) + '\n'
			LAYER += 0x5;

			//section_header
			file.seekg(LAYER + 0x46, ios_base::beg);
			string sec_name(41, 0);
			file >> sec_name;

			LOG_PRINT(logfile, "				SECTION NAME: %s (@ 0x%X)\n", sec_name.c_str(), (LAYER + 0x46));

			//section_body_1_size
			LAYER += size + 0x1;
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			//section_body_1
			LAYER += 0x5;
			file.seekg(LAYER, ios_base::beg);

			int col_index = findColumnByName(spread, sec_name);
			if(col_index != -1){//check if it is a formula
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
			LAYER += size + (size > 0 ? 0x1 : 0);// + 0x5;
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			if (!size){
				LAYER += 0x5;
				file.seekg(LAYER, ios_base::beg);
				file >> size;
			}

			if(size && size != sectionSize){
				LAYER += size + 0x6;
				file.seekg(LAYER, ios_base::beg);
				file >> size;
			}
		}

		file.seekg(1, ios_base::cur);
		file >> size;
		LAYER += 0x5;
		sectionSize = size;

		LOG_PRINT(logfile, "				POS (@ 0x%X)\n", (unsigned int)file.tellg());

		while(!file.eof()){
			LAYER += 0x5;
			file.seekg(LAYER + 0x4, ios_base::beg);

			short index;
			file >> index;
			LOG_PRINT(logfile, "				Index: %d (@ 0x%X)\n", index, (unsigned int)file.tellg());
			if (index < 0)
				break;

			file.seekg(LAYER + 0x12, ios_base::beg);
			name.resize(12);
			file >> name;
			LOG_PRINT(logfile, "				Column: %s (@ 0x%X)\n", name.c_str(), (LAYER + 0x12));

			file.seekg(LAYER + 0x11, ios_base::beg);
			file >> c;

			short width = 0;
			file.seekg(LAYER + 0x4A, ios_base::beg);
			file >> width;

			int col_index = findColumnByIndexAndName(spread, index, name);
			if (col_index != -1){
				SpreadColumn::ColumnType type;
				switch(c){
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

				switch(c1){
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
						if(c2 >= 0x80){
							speadSheets[spread].columns[col_index].significantDigits = c2 - 0x80;
							speadSheets[spread].columns[col_index].numericDisplayType = SignificantDigits;
						} else if(c2 > 0) {
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
			}
			LAYER += sectionSize + 0x1;
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			LAYER += 0x5;
			if(size > 0){
				if(col_index != -1){
					file.seekg(LAYER, ios_base::beg);
					file >> speadSheets[spread].columns[col_index].comment.assign(size, 0);

					string comment = speadSheets[spread].columns[col_index].comment;
					string::size_type pos = comment.find_first_of("@");
					if (pos != string::npos){
						comment.resize(pos);
						speadSheets[spread].columns[col_index].comment = comment;
					}

					LOG_PRINT(logfile, "			comment: %s (@ 0x%X)\n", comment.c_str(), LAYER);
				}
				LAYER += size + 0x1;
			}

			if (sheets == 1 && col_index != -1)
				header.push_back(speadSheets[spread].columns[col_index]);

			file.seekg(LAYER, ios_base::beg);
			file >> size;
			if(size != sectionSize)
				break;
		}

		file.seekg(0x6, ios_base::cur);
		LAYER += 0x6;

		skipObjectInfo();//go to layer end position

		if (sheets > 1){
			file.seekg(-5, ios_base::cur);
			LAYER = file.tellg();
			file >> size;
			if(!size){
				file.seekg(1, ios_base::cur);
				break;
			}
		}
	}

	for (unsigned int i = 0; i < header.size(); i++)
		speadSheets[spread].columns[i] = header[i];

	LOG_PRINT(logfile, "		Done with spreadsheet %d POS (@ 0x%X)\n", spread, (unsigned int)file.tellg());
}

void Origin800Parser::readMatrixInfo()
{
	unsigned int POS = file.tellg();

	unsigned int size;
	file >> size;

	POS += 5;

	LOG_PRINT(logfile, "			[Matrix SECTION (@ 0x%X)]\n", POS);

	string name(25, 0);
	file.seekg(POS + 0x2, ios_base::beg);
	file >> name;

	int idx = findMatrixByName(name);
	matrices[idx].name = name;
	file.seekg(POS, ios_base::beg);
	readWindowProperties(matrices[idx], size);

	unsigned int LAYER = POS;
	LAYER += size + 0x1;

	unsigned char h;
	file.seekg(POS + 0x29, ios_base::beg);
	file >> h;
	matrices[idx].activeSheet = h;
	LOG_PRINT(logfile, "				Active sheet: %d (@ 0x%X)\n", matrices[idx].activeSheet, (POS + 0x29));

	file.seekg(POS + 0x87, ios_base::beg);
	file >> h;
	LOG_PRINT(logfile, "				Header: %d (@ 0x%X)\n", h, (POS + 0x87));
	matrices[idx].header = (h == 194) ? Matrix::XY : Matrix::ColumnRow;

	int sheets = matrices[idx].sheets.size();
	LOG_PRINT(logfile, "				Sheets: %d\n", sheets);
	for (int i = 0; i < sheets; i++){
		MatrixSheet sheet = matrices[idx].sheets[i];
		LOG_PRINT(logfile, "				Parsing sheet %d ...\n", i + 1);

		file.seekg(LAYER, ios_base::beg);
		file >> size;

		// LAYER section
		LAYER += 0x5;

		unsigned short width = 8;
		file.seekg(LAYER + 0x27, ios_base::beg);
		file >> width;
		if (width == 0)
			width = 8;
		sheet.width = width;
		LOG_PRINT(logfile, "					Width: %d (@ 0x%X)\n", sheet.width, (LAYER + 0x27));

		file.seekg(LAYER + 0x2B, ios_base::beg);
		file >> sheet.columnCount;
		LOG_PRINT(logfile, "					Columns: %d (@ 0x%X)\n", sheet.columnCount, (LAYER + 0x2B));

		file.seekg(LAYER + 0x52, ios_base::beg);
		file >> sheet.rowCount;
		LOG_PRINT(logfile, "					Rows: %d (@ 0x%X)\n", sheet.rowCount, (LAYER + 0x52));

		file.seekg(LAYER + 0x71, ios_base::beg);
		unsigned char view;
		file >> view;
		if (view != 0x32 && view != 0x28){
			sheet.view = MatrixSheet::ImageView;
			LOG_PRINT(logfile, "					View: Image (%d @ 0x%X)\n", view, (LAYER + 0x71));
		} else
			LOG_PRINT(logfile, "					View: Data (%d @ 0x%X)\n", view, (LAYER + 0x71));

		for (int j = 0; j < 4; j++)
			skipLine();
		file.seekg(9, ios_base::cur);
		file >> sheet.name.assign(32, 0);
		LOG_PRINT(logfile, "					Name: %s (@ 0x%X)\n", sheet.name.c_str(), (unsigned int)file.tellg() - 32);

		LAYER += size + 0x1;
		file.seekg(LAYER, ios_base::beg);
		file >> size;

		unsigned int sectionSize = size;
		while(!file.eof()){
			//section_header_size=0x6F(4 bytes) + '\n'
			LAYER += 0x5;

			//section_header
			string sec_name(41, 0);
			file.seekg(LAYER + 0x46, ios_base::beg);
			file >> sec_name;

			//section_body_1_size
			LAYER += sectionSize + 0x1;
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			//section_body_1
			LAYER += 0x5;
			if (sec_name == "1"){//check if it is a formula
				file.seekg(LAYER, ios_base::beg);
				file >> sheet.command.assign(size, 0);
				LOG_PRINT(logfile, "					Formula: %s @ 0x%X\n", sheet.command.c_str(), (unsigned int)file.tellg());
			} else if (sec_name == "Y2"){
				string s(size, 0);
				file >> s;
				sheet.coordinates[0] = stringToDouble(s);
				LOG_PRINT(logfile, "					Y2: %g\n", sheet.coordinates[0]);
			} else if (sec_name == "X2"){
				string s(size, 0);
				file >> s;
				sheet.coordinates[1] = stringToDouble(s);
				LOG_PRINT(logfile, "					X2: %g\n", sheet.coordinates[1]);
			} else if (sec_name == "Y1"){
				string s(size, 0);
				file >> s;
				sheet.coordinates[2] = stringToDouble(s);
				LOG_PRINT(logfile, "					Y1: %g\n", sheet.coordinates[2]);
			} else if (sec_name == "X1"){
				string s(size, 0);
				file >> s;
				sheet.coordinates[3] = stringToDouble(s);
				LOG_PRINT(logfile, "					X1: %g\n", sheet.coordinates[3]);
			}

			//section_body_2_size
			LAYER += size + 0x1;
			file.seekg(LAYER, ios_base::beg);
			file >> size;

			//section_body_2
			LAYER += 0x5;
			if(sec_name == "COLORMAP"){
				file.seekg(LAYER + 0x14, ios_base::beg);
				readColorMap(sheet.colorMap);
			}

			//close section 00 00 00 00 0A
			LAYER += size + (size > 0 ? 0x1 : 0) + 0x5;

			if(sec_name == "__LayerInfoStorage")
				break;

		}
		LAYER += 0x5;
		file.seekg(LAYER, ios_base::beg);
		file >> size;
		sectionSize = size;

		while(!file.eof()){
			LAYER += 0x5;

			unsigned char c1,c2;
			file.seekg(LAYER + 0x1E, ios_base::beg);
			file >> c1;
			file >> c2;

			sheet.valueTypeSpecification = c1/0x10;
			if(c2 >= 0x80){
				sheet.significantDigits = c2-0x80;
				sheet.numericDisplayType = SignificantDigits;
			} else if (c2 > 0){
				sheet.decimalPlaces = c2-0x03;
				sheet.numericDisplayType = DecimalPlaces;
			}

			LAYER += sectionSize + 0x1;

			file.seekg(LAYER, ios_base::beg);
			file >> size;

			LAYER += size + (size > 0 ? 0x1 : 0) + 0x5;

			file.seekg(LAYER, ios_base::beg);
			file >> size;

			if(size != sectionSize)
				break;
		}

		file.seekg(1, ios_base::cur);
		skipObjectInfo();

		file.seekg(-5, ios_base::cur);
		POS = file.tellg();
		LAYER = POS;

		matrices[idx].sheets[i] = sheet;
	}

	file.seekg(5, ios_base::cur);
	LOG_PRINT(logfile, "				Done with matrix %s (@ 0x%X)\n", name.c_str(), (unsigned int)file.tellg());
}

OriginParser* createOrigin800Parser(const string& fileName)
{
	return new Origin800Parser(fileName);
}

unsigned int Origin800Parser::findStringPos(const string& name)
{
	char c = 0;
	unsigned int startPos = file.tellg();
	unsigned int pos = startPos;
	while(!file.eof()){
		file >> c;

		if (c == name[0]){
			pos = file.tellg();

			file.seekg(pos - 0x3, ios_base::beg);
			file >> c;

			file.seekg(pos - 0x1, ios_base::beg);
			string s = string(name.size(), 0);
			file >> s;

			char end;
			file >> end;

			if (!c && !end && name == s){
				pos -= 0x1;
				file.seekg(startPos, ios_base::beg);
				//LOG_PRINT(logfile, "Found string: %s (@ 0x%X)\n", name, pos);
				return pos;
			}
		}
		pos++;
	}
	return pos;
}
