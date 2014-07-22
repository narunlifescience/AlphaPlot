/***************************************************************************
	File                 : Origin810Parser.cpp
    --------------------------------------------------------------------
	Copyright            : (C) 2010 - 2011 Ion Vasilief
	Email (use @ for *)  : ion_vasilief*yahoo.fr
	Description          : Origin 8.1 file parser class (uses code from file
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

#include "Origin810Parser.h"
#include <sstream>

Origin810Parser::Origin810Parser(const string& fileName)
	:	Origin800Parser(fileName)
{
	d_colormap_offset = 0x25F;
	notes_pos_mark = "P";
}

void Origin810Parser::readProjectTreeFolder(tree<ProjectNode>::iterator parent)
{
	unsigned int POS = file.tellg();

	double creationDate, modificationDate;
	POS += 5;

	file.seekg(POS + 0x02, ios_base::beg);
	unsigned char a;
	file >> a;
	bool activeFolder = (a == 1);

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

	tree<ProjectNode>::iterator current_folder = projectTree.append_child(parent, ProjectNode(name, ProjectNode::Folder, doubleToPosixTime(creationDate), doubleToPosixTime(modificationDate), activeFolder));

	file.seekg(1, ios_base::cur);
	for (int i = 0; i < 6; i++)
		skipLine();

	POS = file.tellg();

	unsigned int objectcount;
	file >> objectcount;

	windowsCount += objectcount;

	POS += 5 + 5;

	for (unsigned int i = 0; i < objectcount; ++i){
		POS += 5;
		char c;
		file.seekg(POS + 0x2, ios_base::beg);
		file >> c;

		unsigned int objectID;
		file.seekg(POS + 0x4, ios_base::beg);
		file >> objectID;

		if(c == 0x10){
			projectTree.append_child(current_folder, ProjectNode(notes[objectID].name, ProjectNode::Note));
		} else {
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

void Origin810Parser::readColorMap(ColorMap& colorMap)
{
	unsigned int colorMapSize;
	file >> colorMapSize;

	file.seekg(0x140, ios_base::cur);
	for(unsigned int i = 0; i < colorMapSize + 3; ++i){
		ColorMapLevel level;
		file >> level.fillPattern;

		file.seekg(0x03, ios_base::cur);
		file >> level.fillPatternColor;

		short w;
		file >> w;
		level.fillPatternLineWidth = (double)w/500.0;

		file.seekg(0x06, ios_base::cur);
		file >> level.lineStyle;

		file.seekg(0x01, ios_base::cur);
		file >> w;
		level.lineWidth = (double)w/500.0;
		file >> level.lineColor;

		file.seekg(0x02, ios_base::cur);
		unsigned char h;
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

OriginParser* createOrigin810Parser(const string& fileName)
{
	return new Origin810Parser(fileName);
}
