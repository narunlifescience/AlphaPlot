/***************************************************************************
	File                 : Origin850Parser.cpp
    --------------------------------------------------------------------
	Copyright            : (C) 2011 Ion Vasilief
	Email (use @ for *)  : ion_vasilief*yahoo.fr
	Description          : Origin 8.5 file parser class
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

#include "Origin850Parser.h"

Origin850Parser::Origin850Parser(const string& fileName)
:	Origin810Parser(fileName)
{
}

OriginParser* createOrigin850Parser(const string& fileName)
{
	return new Origin850Parser(fileName);
}

bool Origin850Parser::readGraphInfo()
{
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
	if (templateName == "LAYOUT")
		graphs.back().isLayout = true;

	unsigned int LAYER = POS;
	LAYER += size + 0x1;

	while(!file.eof())// multilayer loop
	{
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

		file.seekg(LAYER + 0x105, ios_base::beg);
		file >> layer.backgroundColor;

		LAYER += size + 0x1;
		//now structure is next : section_header_size=0x6F(4 bytes) + '\n' + section_header(0x6F bytes) + section_body_1_size(4 bytes) + '\n' + section_body_1 + section_body_2_size(maybe=0)(4 bytes) + '\n' + section_body_2 + '\n'
		//possible sections: axes, legend, __BC02, _202, _231, _232, __LayerInfoStorage etc
		//section name starts with 0x46 position
		while(!file.eof()){
			//section_header_size=0x6F(4 bytes) + '\n'
			LAYER += 0x5;

			//section_header

			string sec_name(41, 0);
			file.seekg(LAYER + 0x46, ios_base::beg);
			file >> sec_name;

			unsigned int sectionNamePos = LAYER + 0x46;
			LOG_PRINT(logfile, "				SECTION NAME: %s (@ 0x%X)\n", sec_name.c_str(), (LAYER + 0x46));

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
			unsigned int SECTION_BODY1_POS = LAYER;

			file.seekg(LAYER, ios_base::beg);
			readGraphAxisPrefixSuffixInfo(sec_name, size, layer);

			file.seekg(LAYER, ios_base::beg);
			unsigned char type;
			file >> type;

			LineVertex begin, end;
			if (size == 120){//Line/Arrow
				if (attach == Origin::Scale){
					if (type == 2){
						file.seekg(LAYER + 0x20, ios_base::beg);
						file >> begin.x;
						file >> end.x;
						file.seekg(LAYER + 0x40, ios_base::beg);
						file >> begin.y;
						file >> end.y;
					} else if (type == 4){//curved arrow: start point, 2 middle points and end point
						file.seekg(LAYER + 0x20, ios_base::beg);
						file >> begin.x;
						file >> end.x;
						file >> end.x;
						file >> end.x;
						file >> begin.y;
						file >> end.y;
						file >> end.y;
						file >> end.y;
					}
				} else {
					short x1, x2, y1, y2;
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

					double dx = fabs(x2 - x1);
					double dy = fabs(y2 - y1);
					double minx = (x1 <= x2) ? x1 : x2;
					double miny = (y1 <= y2) ? y1 : y2;

					begin.x = (x1 == x2) ? r.left + 0.5*r.width() : r.left + (x1 - minx)/dx*r.width();
					end.x = (x1 == x2) ? r.left + 0.5*r.width() : r.left + (x2 - minx)/dx*r.width();
					begin.y = (y1 == y2) ? r.top + 0.5*r.height(): r.top + (y1 - miny)/dy*r.height();
					end.y = (y1 == y2) ? r.top + 0.5*r.height() : r.top + (y2 - miny)/dy*r.height();
				}

				file.seekg(LAYER + 0x11, ios_base::beg);
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

				file.seekg(LAYER + 0x60, ios_base::beg);
				file >> begin.shapeType;

				file.seekg(LAYER + 0x64, ios_base::beg);
				unsigned int w = 0;
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

				file.seekg(p + 65, ios_base::beg);
				file >> layer.percentile.labels;

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
			else if(sec_name == "_206") // box plot labels
			{
				file.seekg(sectionNamePos, ios_base::beg);
				//LOG_PRINT(logfile, "box plot labels pos @ 0x%X\n", (unsigned int)file.tellg());

				/*file >> curve.text.fontSize;

				file.seekg(7, ios_base::cur);
				file >> h;
				curve.text.fontUnderline = (h & 0x1);
				curve.text.fontItalic = (h & 0x2);
				curve.text.fontBold = (h & 0x8);
				curve.text.whiteOut = (h & 0x20);

				file.seekg(2, ios_base::cur);*/

				file.seekg(33, ios_base::cur);
				//LOG_PRINT(logfile, "box plot labels color @ 0x%X\n", (unsigned int)file.tellg());
				//file >> curve.text.color;
			}
			else if(sec_name == "VLine") // Image profiles vertical cursor
			{
				file.seekg(SECTION_BODY1_POS + 0x0A, ios_base::beg);
				double start;
				file >> start;

				file.seekg(SECTION_BODY1_POS + 0x1A, ios_base::beg);
				double width;
				file >> width;

				layer.vLine = start + 0.5*width;
				layer.imageProfileTool = 2;
			}
			else if(sec_name == "HLine") // Image profiles horizontal cursor
			{
				file.seekg(SECTION_BODY1_POS + 0x12, ios_base::beg);
				double start;
				file >> start;

				file.seekg(SECTION_BODY1_POS + 0x22, ios_base::beg);
				double width;
				file >> width;

				layer.hLine = start + 0.5*width;
				layer.imageProfileTool = 2;
			}
			else if(sec_name == "vline") // Image profiles vertical cursor
			{
				file.seekg(sectionNamePos, ios_base::beg);
				for (int i = 0; i < 2; i++)
					skipLine();

				file.seekg(0x20, ios_base::cur);
				file >> layer.vLine;
				LOG_PRINT(logfile, "					vLine: %g\n", layer.vLine);

				layer.imageProfileTool = 1;
			}
			else if(sec_name == "hline") // Image profiles horizontal cursor
			{
				file.seekg(sectionNamePos, ios_base::beg);
				for (int i = 0; i < 2; i++)
					skipLine();

				file.seekg(0x40, ios_base::cur);
				file >> layer.hLine;
				LOG_PRINT(logfile, "					hLine: %g @ 0x%X\n", layer.hLine, (unsigned int)file.tellg());

				layer.imageProfileTool = 1;
			}
			else if(sec_name == "ZCOLORS")
			{
				layer.isXYY3D = true;
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
			else if ((osize == 0x3E || (osize == 78 && type == 0)) && sec_name != "DelData") // text
			{
				string text(size, 0);
				file >> text;

				sec_name.resize(3);
				if (sec_name == "PIE")
					layer.pieTexts.push_back(TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach));
				else
					layer.texts.push_back(TextBox(text, r, color, fontSize, rotation/10, tab, (BorderType)(border >= 0x80 ? border-0x80 : None), (Attach)attach));
			}
			else if (osize == 0x5E) // rectangle & circle
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
			else if (osize == 0x78 && (type == 2 || type == 4) && sec_name != "sLine" && sec_name != "sline") // line/arrow
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
				if (curve.type == GraphCurve::XYZContour || curve.type == GraphCurve::Contour)
					layer.isXYY3D = false;

				LOG_PRINT(logfile, "			graph %d layer %d curve %d type : %d\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), (int)curve.type);

				file.seekg(LAYER + 0x04, ios_base::beg);
				file >> w;
				pair<string, string> column = findDataByIndex(w-1);
				short nColY = w;
				if (column.first.size() > 0){
					curve.dataName = column.first;
					if(layer.is3D() || (curve.type == GraphCurve::XYZContour)){
						LOG_PRINT(logfile, "			graph %d layer %d curve %d Z : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str());
						curve.zColumnName = column.second;
					} else {
						LOG_PRINT(logfile, "			graph %d layer %d curve %d Y : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str());
						curve.yColumnName = column.second;
					}
				}

				file.seekg(LAYER + 0x23, ios_base::beg);
				file >> w;
				column = findDataByIndex(w-1);
				if (column.first.size() > 0){
					curve.xDataName = (curve.dataName != column.first) ? column.first : "";

					if(layer.is3D() || (curve.type == GraphCurve::XYZContour)){
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
				if (column.first.size() > 0 && (layer.is3D() || (curve.type == GraphCurve::XYZContour))){
					LOG_PRINT(logfile, "			graph %d layer %d curve %d X : %s.%s\n", graphs.size(), graphs.back().layers.size(), layer.curves.size(), column.first.c_str(), column.second.c_str());
					curve.xColumnName = column.second;
					if(curve.dataName != column.first)
						LOG_PRINT(logfile, "			graph %d X and Y from different tables\n", graphs.size());
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

				file.seekg(LAYER + 0x9C, ios_base::beg);
				file >> curve.lineTransparency;

				file >> h;
				curve.fillAreaWithLineTransparency = !h;

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

					if(h >= 0x64 && h < 0x1F4){
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

				if (curve.type == GraphCurve::Mesh3D || curve.type == GraphCurve::Contour || curve.type == GraphCurve::XYZContour){
					if (curve.type == GraphCurve::Contour || curve.type == GraphCurve::XYZContour)
						layer.isXYY3D = false;

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

					file.seekg(LAYER + d_colormap_offset, ios_base::beg);
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

				file.seekg(LAYER + 0x11E, ios_base::beg);
				file >> curve.fillAreaTransparency;

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
				file >> h;
				file >> curve.symbolFillTransparency;

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
		LAYER += size*0x6 + 0x5;

		file.seekg(LAYER, ios_base::beg);
		readGraphAxisInfo(layer.yAxis);
		LAYER += size*0x6 + 0x5;

		file.seekg(LAYER, ios_base::beg);
		readGraphAxisInfo(layer.zAxis);
		LAYER += size*0x6 + 0x5;

		file.seekg(LAYER, ios_base::beg);
		file >> size;

		if(size == 0)
			break;
	}

	file.seekg(LAYER + 0x5, ios_base::beg);
	return true;
}
