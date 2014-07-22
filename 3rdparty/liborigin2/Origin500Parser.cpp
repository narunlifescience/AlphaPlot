/***************************************************************************
	File                 : Origin500Parser.cpp
    --------------------------------------------------------------------
	Copyright            : (C) 2011 Ion Vasilief
	Email (use @ for *)  : ion_vasilief*yahoo.fr
	Description          : Origin 5.0 file parser class
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

#include "Origin500Parser.h"

Origin500Parser::Origin500Parser(const string& fileName)
:	Origin610Parser(fileName)
{}

OriginParser* createOrigin500Parser(const string& fileName)
{
	return new Origin500Parser(fileName);
}

bool Origin500Parser::parse()
{
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

	while(size > 0 && size <= 0x8B){// should be 0x72, 0x73 or 0x83 ?
		//////////////////////////////// COLUMN HEADER /////////////////////////////////////////////

		short data_type;
		char data_type_u;
		unsigned int oldpos = (unsigned int)file.tellg();

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

		LOG_PRINT(logfile, "	NAME: %s\n", name.c_str());

		unsigned int spread = 0;
		if(columnname.empty()){
			LOG_PRINT(logfile, "NO COLUMN NAME FOUND! Must be a Matrix or Function.");
			////////////////////////////// READ matrices or functions ////////////////////////////////////

			LOG_PRINT(logfile, "	[position @ 0x%X]\n", (unsigned int) file.tellg());
			// TODO
			short signature;
			file >> signature;
			LOG_PRINT(logfile, "	SIGNATURE : %02X @ 0x%X\n", signature, (unsigned int) file.tellg());

			file.seekg(oldpos + size + 1, ios_base::beg);
			file >> size;
			LOG_PRINT(logfile, "	size : %d @ 0x%X\n", size, (unsigned int) file.tellg());

			file.seekg(1, ios_base::cur);
			size /= valuesize;
			LOG_PRINT(logfile, "	SIZE = %d\n", size);

			if (signature == 0xA00 && size == 1)
				signature = 0xAC8;

			switch(signature){
				case 0x50CA:
				case 0x70CA:
				case 0x50F2:
				case 0x50E2:
				case 0x50E7:
				case 0x50DB:
				case 0x50DC:
				case 0xA00:
				case 0xAE2:
				case 0xAF2:
				case 0xACA:
					if (size){
						matrices.push_back(Matrix(name));
						matrices.back().sheets.push_back(MatrixSheet(name, dataIndex));
					}
					++dataIndex;
					readMatrixValues(data_type, data_type_u, valuesize, size);
					break;
	
				case 0xAC8:
					functions.push_back(Function(name, dataIndex));
					++dataIndex;
					readFunction(colpos, valuesize, &oldpos);
					break;
	
				default:
					LOG_PRINT(logfile, "UNKNOWN SIGNATURE: %.2X SKIP DATA\n", signature);
					file.seekg(valuesize*size, ios_base::cur);
					++dataIndex;
	
					if(valuesize != 8 && valuesize <= 16)
						file.seekg(2, ios_base::cur);
			}
		}
		else
		{	// worksheet
			if(speadSheets.size() == 0 || findSpreadByName(name) == -1)
			{
				LOG_PRINT(logfile, "NEW SPREADSHEET\n");
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

			LOG_PRINT(logfile, "SPREADSHEET = %s COLUMN NAME = %s (@0x%X)\n", name.c_str(), columnname.c_str(), (unsigned int)file.tellg());
			bool validColumn = (columnname.find(".PTL") == string::npos);
			if (validColumn){
				speadSheets[spread].columns.push_back(SpreadColumn(columnname, dataIndex));
				string::size_type sheetpos = speadSheets[spread].columns.back().name.find_last_of("@");
				if (sheetpos != string::npos){
					unsigned int sheet = atoi(columnname.substr(sheetpos + 1).c_str());
					if (sheet > 1){
						speadSheets[spread].columns.back().name = columnname;
						speadSheets[spread].columns.back().sheet = sheet - 1;

						if (speadSheets[spread].sheets < sheet)
							speadSheets[spread].sheets = sheet;
					}
				}
				++dataIndex;
			} else
				current_col--;

			////////////////////////////// SIZE of column /////////////////////////////////////////////
			file.seekg(oldpos + size + 1, ios_base::beg);

			file >> nbytes;
			if (fmod(nbytes, (double)valuesize) > 0)
				LOG_PRINT(logfile, "WARNING: data section could not be read correct");

			nr = nbytes / valuesize;
			LOG_PRINT(logfile, "	[number of rows = %d (%d Bytes) @ 0x%X]\n", nr, nbytes, (unsigned int)file.tellg());

			speadSheets[spread].maxRows < nr ? speadSheets[spread].maxRows = nr : 0;
			readColumnValues(spread, current_col - 1, data_type, valuesize, nr, validColumn);
		}

		if (nbytes > 0 || (columnname.empty() && size))
			file.seekg(1, ios_base::cur);

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
		else if (!readGraphInfo()){
			LOG_PRINT(logfile, "		%s is NOT A GRAPH!\n", name.c_str());
			graphs.pop_back();
		}
	}

	file.seekg(1, ios_base::cur);
	readParameters();

	file.seekg(1 + 5, ios_base::cur);
	readNotes();

	LOG_PRINT(logfile, "Done parsing\n")
#ifndef NO_LOG_FILE
	fclose(logfile);
#endif

	return true;
}

void Origin500Parser::readNotes()
{
	while(!file.eof()){
		unsigned int size;
		file >> size;
		if(size != 28)
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

		file.seekg(4, ios_base::cur);
		file >> size;

		file.seekg(1, ios_base::cur);

		string name(size, 0);
		file >> name;

		notes.push_back(Note(name));
		notes.back().objectID = objectIndex;
		notes.back().frameRect = rect;

		if(state == 0x04)
			notes.back().state = Window::Minimized;
		else if(state == 0x0a)
			notes.back().state = Window::Maximized;

		++objectIndex;

		file.seekg(1, ios_base::cur);
		file >> size;

		file.seekg(1, ios_base::cur);
		file >> notes.back().text.assign(size, 0);

		LOG_PRINT(logfile, "NOTE %d NAME: %s\n", notes.size(), notes.back().name.c_str());

		file.seekg(1, ios_base::cur);
	}
}

bool Origin500Parser::readGraphInfo()
{
	bool error = false;
	unsigned int POS = file.tellg();

	unsigned int size;
	file >> size;

	POS += 5;

	string name(25, 0);
	file.seekg(POS + 0x02, ios_base::beg);
	file >> name;
	LOG_PRINT(logfile, "		GRAPH name: %s @ 0x%X\n", name.c_str(), (unsigned int)file.tellg());

	graphs.push_back(Graph(name));
	file.seekg(POS, ios_base::beg);
	readWindowProperties(graphs.back(), size);

	file.seekg(POS + 0x23, ios_base::beg);
	file >> graphs.back().width;
	file >> graphs.back().height;

	file.seekg(POS + 0x38, ios_base::beg);
	unsigned char c;
	file >> c;
	graphs.back().connectMissingData = (c & 0x40);

	file.seekg(POS + 0x45, ios_base::beg);
	string templateName(20, 0);
	file >> templateName;
	graphs.back().templateName = templateName;
	LOG_PRINT(logfile, "			TEMPLATE: %s pos: 0x%X\n", templateName.c_str(), (POS + 0x45));

	unsigned int LAYER = POS;
	LAYER += size + 0x1;

	while(!file.eof()){// multilayer loop
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

		unsigned char g;
		file >> g; file >> g;
		layer.xAxis.zeroLine = (g & 0x80);
		layer.xAxis.oppositeLine = (g & 0x40);

		file.seekg(LAYER + 0x37, ios_base::beg);
		file >> layer.xAxis.minorTicks;
		file >> layer.xAxis.scale;

		file.seekg(LAYER + 0x3A, ios_base::beg);
		file >> layer.yAxis.min;
		file >> layer.yAxis.max;
		file >> layer.yAxis.step;

		file.seekg(LAYER + 0x56, ios_base::beg);
		file >> layer.yAxis.majorTicks;

		file >> g; file >> g;
		layer.yAxis.zeroLine = (g & 0x80);
		layer.yAxis.oppositeLine = (g & 0x40);

		file.seekg(LAYER + 0x62, ios_base::beg);
		file >> layer.yAxis.minorTicks;
		file >> layer.yAxis.scale;

		file.seekg(LAYER + 0x68, ios_base::beg);
		file >> g;
		layer.gridOnTop = (g & 0x04);
		layer.exchangedAxes = (g & 0x40);

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

		ColorMap colorMap;

		//now structure is next : section_header_size=0x6F(4 bytes) + '\n' + section_header(0x6F bytes) + section_body_1_size(4 bytes) + '\n' + section_body_1 + section_body_2_size(maybe=0)(4 bytes) + '\n' + section_body_2 + '\n'
		//possible sections: axes, legend, __BC02, _202, _231, _232, etc
		//section name starts with 0x46 position
		while(size && !file.eof()){
			//section_header_size=0x6F(4 bytes) + '\n'
			LAYER += 0x5;

			//section_header

			unsigned int sectionNamePos = LAYER + 0x46;
			string sec_name(41, 0);
			file.seekg(sectionNamePos, ios_base::beg);
			file >> sec_name;
			if (!sec_name.empty())
				LOG_PRINT(logfile, "				SECTION NAME: %s (@ 0x%X)\n", sec_name.c_str(), sectionNamePos);

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

			LAYER += size + 0x01;
			file.seekg(LAYER, ios_base::beg);

			file >> size;
			//LOG_PRINT(logfile, "				osize = %d (@ 0x%X)\n", size, (unsigned int)file.tellg());

			//section_body_1
			LAYER += 0x5;
			unsigned int osize = size;
			unsigned int SECTION_BODY1_POS = LAYER;

			file.seekg(LAYER, ios_base::beg);
			readGraphAxisPrefixSuffixInfo(sec_name, size, layer);

			unsigned char type;
			file >> type;

			LineVertex begin, end;
			if (size == 24){//Line/Arrow
				unsigned short x1, x2, y1, y2;
				if (type == 2){//straight line/arrow
					file >> x1;
					file >> x2;
					file.seekg(4, ios_base::cur);
					file >> y1;
					file >> y2;
					file.seekg(4, ios_base::cur);
				} else if (type == 4){//curved line/arrow has 4 points
					file >> x1;
					file.seekg(4, ios_base::cur);
					file >> x2;
					file >> y1;
					file.seekg(4, ios_base::cur);
					file >> y2;
				}

				double maxx = (x1 <= x2) ? x2 : x1;
				if (!x1 && !x2)
					maxx = 1.0;
				double maxy = (y1 <= y2) ? y2 : y1;
				if (!y1 && !y2)
					maxy = 1.0;

				begin.x = r.left + (double)x1/maxx*r.width();
				begin.y = r.top + (double)y1/maxy*r.height();
				end.x = r.left + (double)x2/maxx*r.width();
				end.y = r.top + (double)y2/maxy*r.height();

				unsigned char arrows;
				file >> arrows;
				switch (arrows){
					case 0:
						begin.shapeType = 0;
						end.shapeType = 0;
					break;
					case 1:
						begin.shapeType = 1;
						end.shapeType = 0;
					break;
					case 2:
						begin.shapeType = 0;
						end.shapeType = 1;
					break;
					case 3:
						begin.shapeType = 1;
						end.shapeType = 1;
					break;
				}

				file.seekg(3, ios_base::cur);
				unsigned char sw;
				file >> sw;
				end.shapeLength = (double)sw;
				begin.shapeLength = (double)sw;
				file >> sw;
				end.shapeWidth = (double)sw;
				begin.shapeWidth = (double)sw;
			}

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

			file.seekg(LAYER + 0x12, ios_base::beg);
			file >> lineStyle;

			unsigned short w1;
			file >> w1;
			width = (double)w1/500.0;

			Figure figure;
			file.seekg(LAYER + 0x05, ios_base::beg);
			file >> w1;
			figure.width = (double)w1/500.0;

			file.seekg(LAYER + 0x07, ios_base::beg);
			unsigned char fillIndex, fillType;
			file >> fillIndex;
			file >> figure.style;
			file >> fillType;
			switch(fillType){
				case 0:
					figure.useBorderColor = (fillIndex >= 5);
					if (fillIndex < 5){
						figure.fillAreaPattern = Origin::NoFill;
						figure.fillAreaColor.type = Origin::Color::Regular;
						if (fillIndex == 0)
							figure.fillAreaColor.regular = 0;
						else if (fillIndex == 1)
							figure.fillAreaColor.regular = 18;
						else if (fillIndex == 2)
							figure.fillAreaColor.regular = 23;
						else if (fillIndex == 3)
							figure.fillAreaColor.regular = 17;
						else
							figure.fillAreaColor.regular = 19;
					} else {
						figure.fillAreaColor.type = Origin::Color::None;
						if (fillIndex == 0x05)
							figure.fillAreaPattern = Origin::BDiagMedium;
						else if (fillIndex == 0x06)
							figure.fillAreaPattern = Origin::DiagCrossMedium;
						else if (fillIndex == 0x07)
							figure.fillAreaPattern = Origin::FDiagMedium;
						else if (fillIndex == 0x08)
							figure.fillAreaPattern = Origin::HorizontalMedium;
						else if (fillIndex == 0x09)
							figure.fillAreaPattern = Origin::VerticalMedium;
					}
				break;
				case 1:
					figure.fillAreaPattern = Origin::NoFill;
					figure.fillAreaColor.regular = fillIndex;
					figure.fillAreaColor.type = Origin::Color::Regular;
				break;
				case 2:
					figure.fillAreaColor.type = Origin::Color::None;
					figure.fillAreaPatternColor.type = Origin::Color::None;
					figure.useBorderColor = false;
					figure.fillAreaPattern = Origin::NoFill;
				break;
			}

			//section_body_2_size
			LAYER += size + 0x1;

			file.seekg(LAYER, ios_base::beg);
			file >> size;

			//section_body_2
			LAYER += 0x5;
			//check if it is an axis or a legend

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

				file.seekg(LAYER + 0x5A, ios_base::beg);
				file >> layer.xAngle;
				file >> layer.yAngle;
				file >> layer.zAngle;

				file.seekg(LAYER + 0x218, ios_base::beg);
				file >> layer.xLength;
				file >> layer.yLength;
				file >> layer.zLength;

				layer.xLength /= 23.0;
				layer.yLength /= 23.0;
				layer.zLength /= 23.0;

				file.seekg(LAYER + 0x240, ios_base::beg);
				file >> layer.orthographic3D;
			}
			else if(sec_name == "Legend")
			{
				string text(size, 0);
				file >> text;

				layer.legend = TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach);
			}
			else if(sec_name == "ZCOLORS")
			{
				layer.isXYY3D = true;
				file.seekg(LAYER + 0xE, ios_base::beg);
				readColorMap(colorMap);
			}
			else if(sec_name == "SPECTRUM1")
			{
				layer.isXYY3D = false;
				layer.colorScale.visible = true;

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
			else if(sec_name == "&0")
			{
				layer.isWaterfall = true;
				file.seekg(SECTION_BODY1_POS, ios_base::beg);
				string text(osize, 0);
				file >> text;
				size_t commaPos = text.find_first_of(',');
				layer.xOffset = atoi(text.substr(0, commaPos).c_str());
				layer.yOffset = atoi(text.substr(commaPos + 1).c_str());
			}
			else if (osize == 22 || (size && !type))
			{
				string text(size, 0);
				file >> text;

				sec_name.resize(3);
				if (sec_name == "PIE")
					layer.pieTexts.push_back(TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach));
				else
					layer.texts.push_back(TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach));
			}
			else if(osize == 0xA) // rectangle & circle
			{
				switch(type){
					case 0:
					case 1:
						figure.type = Figure::Rectangle;
						LOG_PRINT(logfile, "				Rectangle\n");
						break;
					case 2:
					case 3:
						figure.type = Figure::Circle;
						LOG_PRINT(logfile, "				Circle\n");
						break;
				}
				figure.clientRect = r;
				figure.attach = (Attach)attach;
				figure.color = color;

				layer.figures.push_back(figure);
			}
			else if (osize == 24) // line
			{
				LOG_PRINT(logfile, "				Line/Arrow\n");
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
			else if(osize == 40) // bitmap
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

		if(size)//check layer is not empty
		{
			while(!file.eof()){
				LAYER += 0x5;

				layer.curves.push_back(GraphCurve());
				GraphCurve& curve(layer.curves.back());

				file.seekg(LAYER + 0x26, ios_base::beg);
				file >> h;
				curve.hidden = (h == 33);
				LOG_PRINT(logfile, "			hidden curve: %d\n", curve.hidden);

				file.seekg(LAYER + 0x4C, ios_base::beg);
				file >> curve.type;
				if (!curve.type){
					LOG_PRINT(logfile, "			Found unknown curve type (%d)!\n", (int)curve.type);
				} else
					LOG_PRINT(logfile, "			graph %d layer %d curve %d type : %d\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), (int)curve.type);
				if (curve.type == GraphCurve::Mesh3D || curve.type == GraphCurve::Contour)
					layer.isXYY3D = false;

				file.seekg(LAYER + 0x04, ios_base::beg);
				file >> w;
				pair<string, string> column = findDataByIndex(w-1);
				short nColY = w;
				if(column.first.size() > 0){
					curve.dataName = column.first;
					if(layer.is3D()){
						LOG_PRINT(logfile, "			graph %d layer %d curve %d Z : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str());
						curve.zColumnName = column.second;
					} else {
						LOG_PRINT(logfile, "			graph %d layer %d curve %d Y : %s.%s\n", graphs.size(),graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str());
						curve.yColumnName = column.second;
					}
				}

				file.seekg(LAYER + 0x23, ios_base::beg);
				file >> w;
				column = findDataByIndex(w-1);
				if (column.first.size() > 0){
					curve.xDataName = (curve.dataName != column.first) ? column.first : "";

					if(layer.is3D()){
						LOG_PRINT(logfile, "			graph %d layer %d curve %d Y : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str());
						curve.yColumnName = column.second;
					} else if (layer.isXYY3D){
						LOG_PRINT(logfile, "			graph %d layer %d curve %d X : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str());
						curve.xColumnName = column.second;
					} else {
						LOG_PRINT(logfile, "			graph %d layer %d curve %d X : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str());
						curve.xColumnName = column.second;
					}
				}

				file.seekg(LAYER + 0x4D, ios_base::beg);
				file >> w;
				column = findDataByIndex(w-1);
				if(column.first.size() > 0 && layer.is3D()){
					LOG_PRINT(logfile, "			graph %d layer %d curve %d X : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str());
					curve.xColumnName = column.second;
					if(curve.dataName != column.first)
						LOG_PRINT(logfile, "			graph %d X and Y from different tables\n", graphs.size());
				}

				if(layer.is3D() || layer.isXYY3D)
					graphs.back().is3D = true;

				if (curve.type == GraphCurve::Contour){
					file.seekg(LAYER + 0x12, ios_base::beg);
					file >> curve.lineColor.regular;
					curve.lineColor.type = Origin::Color::Regular;

					file >> h;
					curve.lineConnect = (h & 0x40) ? GraphCurve::Straight : GraphCurve::NoLine;
					colorMap.fillEnabled = (h & 0x82);
					if (h & 0x01)
						curve.lineColor.type = Origin::Color::None;//use color map color;

					file.seekg(1, ios_base::cur);
					file >> h;
					curve.lineWidth = h/10.0;
				} else {
					file.seekg(LAYER + 0xf, ios_base::beg);
					file >> curve.lineColor.regular;
					curve.lineColor.type = Origin::Color::Regular;

					file.seekg(LAYER + 0x11, ios_base::beg);
					file >> curve.lineConnect;
					file >> curve.lineStyle;

					file.seekg(1, ios_base::cur);
					file >> curve.boxWidth;

					file >> w;
					curve.lineWidth=(double)w/500.0;
				}

				file.seekg(LAYER + 0x19, ios_base::beg);
				file >> w;
				curve.symbolSize=(double)w/500.0;

				if (curve.type == GraphCurve::Box ||
					curve.type == Origin::GraphCurve::Column || curve.type == Origin::GraphCurve::ColumnStack ||
					curve.type == Origin::GraphCurve::Bar || curve.type == Origin::GraphCurve::BarStack){
					curve.fillAreaPatternBorderColor = curve.lineColor;
					curve.fillAreaPatternBorderWidth = curve.lineWidth;
					unsigned char fillIndex, fillType;
					file.seekg(LAYER + 0x17, ios_base::beg);
					file >> fillIndex;
					file >> fillType;
					switch(fillType){
						case 0:
							if (fillIndex < 5){
								curve.fillAreaPattern = Origin::NoFill;
								curve.fillAreaColor.type = Origin::Color::Regular;
								if (fillIndex == 0)
									curve.fillAreaColor.regular = 0;
								else if (fillIndex == 1)
									curve.fillAreaColor.regular = 18;
								else if (fillIndex == 2)
									curve.fillAreaColor.regular = 23;
								else if (fillIndex == 3)
									curve.fillAreaColor.regular = 17;
								else
									curve.fillAreaColor.regular = 19;
							} else {
								curve.fillAreaColor.type = Origin::Color::None;
								curve.fillAreaPatternColor = curve.lineColor;
								if (fillIndex == 0x05)
									curve.fillAreaPattern = Origin::BDiagMedium;
								else if (fillIndex == 0x06)
									curve.fillAreaPattern = Origin::DiagCrossMedium;
								else if (fillIndex == 0x07)
									curve.fillAreaPattern = Origin::FDiagMedium;
								else if (fillIndex == 0x08)
									curve.fillAreaPattern = Origin::HorizontalMedium;
								else if (fillIndex == 0x09)
									curve.fillAreaPattern = Origin::VerticalMedium;
							}
						break;
						case 1:
							curve.fillAreaPattern = Origin::NoFill;
							curve.fillAreaColor.regular = fillIndex;
							curve.fillAreaColor.type = Origin::Color::Regular;
						break;
						case 2:
							curve.fillAreaColor.type = Origin::Color::None;
							curve.fillAreaPatternColor.type = Origin::Color::None;
							curve.fillAreaPattern = Origin::NoFill;
						break;
					}

					if (curve.type == GraphCurve::Box){
						layer.percentile.symbolSize = 2;
						layer.percentile.diamondBox = false;
						layer.percentile.boxRange = 3; //BoxCurve::r25_75
						layer.percentile.whiskersRange = 2; //BoxCurve::r5_95
						layer.percentile.p1SymbolType = 7;
						layer.percentile.p99SymbolType = 7;
						layer.percentile.meanSymbolType = 1;
						layer.percentile.maxSymbolType = 9;
						layer.percentile.minSymbolType = 9;
						layer.percentile.labels = false;
						layer.percentile.symbolSize = 5;
						layer.percentile.symbolColor = curve.lineColor;
						layer.percentile.symbolFillColor.type = Origin::Color::None;
					}
				} else {
					file.seekg(LAYER + 0x1C, ios_base::beg);
					file >> h;
					curve.fillArea = (h==2);
					file >> curve.fillAreaColor.regular;
					curve.fillAreaPattern = Origin::NoFill;
				}

				//text
				if(!curve.type || curve.type == GraphCurve::TextPlot){
					file.seekg(LAYER + 0xf, ios_base::beg);
					file >> curve.text.color.regular;
					curve.text.color.type = Origin::Color::Regular;

					file.seekg(LAYER + 0x13, ios_base::beg);
					file >> curve.text.rotation;
					curve.text.rotation /= 10;

					file >> curve.text.fontSize;
					if (curve.text.fontSize < 250)
						curve.type = GraphCurve::TextPlot;

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

				if (curve.type == GraphCurve::Vector){
					curve.vector.multiplier = 1.0;
					curve.vector.position = VectorProperties::Tail;
					curve.vector.arrowLenght = 25;
					curve.vector.arrowAngle = 40;
					curve.vector.arrowClosed = true;
					curve.vector.width = curve.lineWidth;

					file.seekg(LAYER + 0x18, ios_base::beg);
					file >> h;
					if (h >= 0x64){
						column = findDataByIndex(nColY - 1 + h - 0x64);
						if(column.first.size() > 0)
							curve.vector.angleColumnName = column.second;
					} else if (h <= 0x08)
						curve.vector.constAngle = 45*h;

					file >> h;
					if (h >= 0x64 && h < 0x1F4){
						column = findDataByIndex(nColY - 1 + h - 0x64);
						if(column.first.size() > 0)
							curve.vector.magnitudeColumnName = column.second;
					} else
						curve.vector.constMagnitude = (int)curve.symbolSize;
				}

				//pie
				if (curve.type == GraphCurve::Pie){
					file.seekg(LAYER + 0x14, ios_base::beg);
					file >> h;

					curve.pie.formatPercentages = (h & 0x08);
					curve.pie.formatValues = !curve.pie.formatPercentages;
					curve.pie.positionAssociate = (h & 0x80);
					curve.pie.formatCategories = (h & 0x20);

					file.seekg(LAYER + 0x19, ios_base::beg);
					file >> h;
					curve.pie.radius = 100 - h;

					file >> h;
					curve.pie.distance = h;
					curve.pie.formatAutomatic = true;
					curve.pie.viewAngle = 90;
					curve.pie.thickness = 33;
					curve.pie.rotation = 0;
					curve.pie.horizontalOffset = 0;

					/*file.seekg(LAYER + 0x9E, ios_base::beg);
					file >> curve.pie.displacement;
					file.seekg(LAYER + 0xA6, ios_base::beg);
					file >> curve.pie.displacedSectionCount;*/
				}

				if (curve.type == GraphCurve::Mesh3D || curve.type == GraphCurve::Contour || curve.type == GraphCurve::XYZContour){
					if (curve.type == GraphCurve::Contour || curve.type == GraphCurve::XYZContour)
						layer.isXYY3D = false;

					if (curve.type == GraphCurve::Mesh3D)
						curve.surface.colorMap = colorMap;
					else
						curve.colorMap = colorMap;

					/*if (curve.type == GraphCurve::Contour){
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
					}*/
				}
				
				//surface
				if (layer.isXYY3D || curve.type == GraphCurve::Mesh3D){
					file.seekg(LAYER + 0x15, ios_base::beg);
					file >> w;
					curve.surface.gridLineWidth = (double)w/500.0;

					file.seekg(8, ios_base::cur);
					file >> curve.surface.gridColor;

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

				file.seekg(LAYER + 0x17, ios_base::beg);
				file >> curve.symbolType;

				curve.symbolFillColor = curve.lineColor;
				curve.symbolColor = curve.lineColor;
				curve.vector.color = curve.symbolColor;

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
		while(!file.eof()){
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

	if (error)
		return false;
	return true;
}

void Origin500Parser::readColorMap(ColorMap& colorMap)
{
	Color lowColor;//color bellow
	lowColor.type = Origin::Color::Custom;
	file >> lowColor.custom[0];
	file >> lowColor.custom[1];
	file >> lowColor.custom[2];

	unsigned char h;
	file >> h;

	Color highColor;//color above
	highColor.type = Origin::Color::Custom;
	file >> highColor.custom[0];
	file >> highColor.custom[1];
	file >> highColor.custom[2];
	file >> h;

	unsigned short colorMapSize;
	file >> colorMapSize;

	file.seekg(2, ios_base::cur);

	for(unsigned int i = 0; i < 4; ++i){//low, high, middle and missing data colors
		Color color;
		color.type = Origin::Color::Custom;
		file >> color.custom[0];
		file >> color.custom[1];
		file >> color.custom[2];
		file >> h;
	}

	double zmin;
	file >> zmin;

	double zmax;
	file >> zmax;

	file.seekg(0x40, ios_base::cur);

	short val;
	for(unsigned int i = 0; i < 2; ++i){
		Color color;
		color.type = Origin::Color::Custom;
		file >> color.custom[0];
		file >> color.custom[1];
		file >> color.custom[2];
		file >> h;
		file >> val;
		file.seekg(4, ios_base::cur);
	}

	ColorMapLevel level;
	level.fillColor = lowColor;
	colorMap.levels.push_back(make_pair(zmin, level));

	for(unsigned short i = 0; i < colorMapSize + 1; ++i){
		Color color;
		color.type = Origin::Color::Custom;
		file >> color.custom[0];
		file >> color.custom[1];
		file >> color.custom[2];
		file >> h;
		file >> val;
		file.seekg(4, ios_base::cur);

		level.fillColor = color;
		colorMap.levels.push_back(make_pair(val, level));
	}

	level.fillColor = highColor;
	colorMap.levels.push_back(make_pair(zmax, level));
}
