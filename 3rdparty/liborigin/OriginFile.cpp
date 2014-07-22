/***************************************************************************
    File                 : OriginFile.cpp
    --------------------------------------------------------------------
    Copyright            : (C) 2005-2008 Stefan Gerlach
						   (C) 2007-2008 Alex Kargovsky, Ion Vasilief
    Email (use @ for *)  : kargovsky*yumr.phys.msu.su, ion_vasilief*yahoo.fr
    Description          : Origin file import class

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

#include "OriginFile.h"
#include <cstdio>  // for fprintf
#include <cstdlib> // for atoi
#include <fstream>

OriginFile::OriginFile(const string& fileName)
:	fileVersion(0)
{
	unsigned int ioret;  // return value of io functions
	ifstream file(fileName.c_str(), ios_base::binary);

	if (!file.is_open())
	{
		ioret = fprintf(stderr, "Could not open %s!\n", fileName.c_str());
		return;
	}

#ifndef NO_CODE_GENERATION_FOR_LOG
	FILE *logfile = NULL;
	logfile = fopen("./opjfile.log", "w");
	if (logfile == NULL)
	{
		ioret = fprintf(stderr, "Could not open opjfile.log !\n");
		return;
	}
#endif // NO_CODE_GENERATION_FOR_LOG

	string vers(4, 0);
	file.seekg(0x7, ios_base::beg);
	file >> vers;
	fileVersion = atoi(vers.c_str());
	file.close();

	LOG_PRINT(logfile, "	[version = %d]\n", fileVersion)

	buildVersion = fileVersion;
	// translate version
	if(fileVersion >= 130 && fileVersion <= 140) // 4.1
		fileVersion = 410;
	else if(fileVersion == 210) // 5.0
		fileVersion = 500;
	else if(fileVersion == 2625) // 6.0
		fileVersion = 600;
	else if(fileVersion == 2627) // 6.0 SR1
		fileVersion = 601;
	else if(fileVersion == 2630) // 6.0 SR4
		fileVersion = 604;
	else if(fileVersion == 2635) // 6.1
		fileVersion = 610;
	else if(fileVersion >= 2656 && fileVersion <= 2664) // 7.0
		fileVersion = 700;
	else if(fileVersion == 2672) // 7.0 SR3
		fileVersion = 703;
	else if(fileVersion == 2673) // 7.0 E
		fileVersion = 704;
	else if(fileVersion >= 2766 && fileVersion <= 2769) // 7.5
		fileVersion = 750;
	else if(fileVersion >= 2876 && fileVersion <= 2906) // 8.0
		fileVersion = 800;
	else if(fileVersion >= 2907) // 8.1
		fileVersion = 810;
	else {
		LOG_PRINT(logfile, "Found unknown project version %d\n", fileVersion)
		LOG_PRINT(logfile, "Please contact the authors of liborigin")
#ifndef NO_CODE_GENERATION_FOR_LOG
		unsigned int ioret;
		ioret = fclose(logfile);
#endif // NO_CODE_GENERATION_FOR_LOG
		throw std::logic_error("Unknown project version");
	}
	LOG_PRINT(logfile, "Found project version %.2f\n", fileVersion/100.0)
	// Close logfile, will be reopened in parser routine.
	// There are ways to keep logfile open and pass it to parser routine,
	// but I choose to do the same as with 'file', close it and reopen in 'parse'
	// routines.
#ifndef NO_CODE_GENERATION_FOR_LOG
	fclose(logfile);
#endif // NO_CODE_GENERATION_FOR_LOG
	switch(fileVersion){
		case 810:
			parser.reset(createOrigin810Parser(fileName));
			break;
		case 800:
			parser.reset(createOrigin800Parser(fileName));
			break;
		case 750:
			parser.reset(createOrigin750Parser(fileName));
			break;
		case 700:
		case 703:
		case 704:
			parser.reset(createOrigin700Parser(fileName));
			break;
		case 610:
			parser.reset(createOrigin610Parser(fileName));
			break;
		case 600:
		case 601:
		case 604:
			parser.reset(createOrigin600Parser(fileName));
			break;
		default:
			parser.reset(createOriginDefaultParser(fileName));
			break;
	}
}

bool OriginFile::parse()
{
	parser->setFileVersion(buildVersion);
	return parser->parse();
}

double OriginFile::version() const
{
	return fileVersion/100.0;
}

const tree<Origin::ProjectNode>* OriginFile::project() const
{
	return &parser->projectTree;
}

vector<Origin::SpreadSheet>::size_type OriginFile::spreadCount() const
{
	return parser->speadSheets.size();
}

Origin::SpreadSheet& OriginFile::spread(vector<Origin::SpreadSheet>::size_type s) const
{
	return parser->speadSheets[s];
}

vector<Origin::Matrix>::size_type OriginFile::matrixCount() const
{
	return parser->matrixes.size();
}

Origin::Matrix& OriginFile::matrix(vector<Origin::Matrix>::size_type m) const
{
	return parser->matrixes[m];
}

vector<Origin::Function>::size_type OriginFile::functionCount() const
{
	return parser->functions.size();
}

vector<Origin::Function>::size_type OriginFile::functionIndex(const string& name) const
{
	return parser->findFunctionByName(name);
}

Origin::Function& OriginFile::function(vector<Origin::Function>::size_type f) const
{
	return parser->functions[f];
}

vector<Origin::Graph>::size_type OriginFile::graphCount() const
{
	return parser->graphs.size();
}

Origin::Graph& OriginFile::graph(vector<Origin::Graph>::size_type g) const
{
	return parser->graphs[g];
}

vector<Origin::Note>::size_type OriginFile::noteCount() const
{
	return parser->notes.size();
}

Origin::Note& OriginFile::note(vector<Origin::Note>::size_type n) const
{
	return parser->notes[n];
}

string OriginFile::resultsLogString() const
{
	return parser->resultsLog;
}
