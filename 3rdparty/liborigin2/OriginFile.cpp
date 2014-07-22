/***************************************************************************
    File                 : OriginFile.cpp
    --------------------------------------------------------------------
    Copyright            : (C) 2005-2008 Stefan Gerlach
						   (C) 2007-2008 Alex Kargovsky
						   (C) 2009-2011 Ion Vasilief
	Email (use @ for *)  : ion_vasilief*yahoo.fr
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
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>

using namespace boost;

OriginFile::OriginFile(const string& fileName)
:	fileVersion(0)
{
	ifstream file(fileName.c_str(), ios_base::binary);
	if(!file.is_open())
	{
		cerr << "Could not open " << fileName << "!" << endl;
		return;
	}

#ifndef NO_LOG_FILE
	FILE *logfile = NULL;
	logfile = fopen("./opjfile.log", "w");
	if (logfile == NULL){
		cerr << "Could not open opjfile.log !" << endl;
		return;
	}
#endif

	unsigned char majVers;
	file.seekg(0x5, ios_base::beg);
	file >> majVers;
	int majVersion = lexical_cast<unsigned int>(majVers);

	string vers(4, 0);
	file.seekg(0x7, ios_base::beg);
	file >> vers;
	fileVersion = atoi(vers.c_str());
	file.close();

	LOG_PRINT(logfile, "	[version = %d.%d]\n", majVersion, fileVersion);

	buildVersion = fileVersion;
	// translate version
	if (majVersion == 3){
		if (fileVersion > 520)								// 3.5 ?
			fileVersion = 350;
	} else if (majVersion == 4){
		if (fileVersion < 110)								// 4.0 ?
			fileVersion = 400;
		else if(fileVersion >= 110 && fileVersion < 140)	// 4.1 ?
			fileVersion = 410;
		else if(fileVersion == 140 || fileVersion == 141)	// 4.1 Patch 1
			fileVersion = 410;
		else if(fileVersion > 141 && fileVersion < 210)		// 5.0
			fileVersion = 500;
		else if(fileVersion >= 210 && fileVersion < 2625)	// 5.0 SR2
			fileVersion = 502;
		else if(fileVersion == 2625)						// 6.0
			fileVersion = 600;
		else if(fileVersion == 2627)						// 6.0 SR1
			fileVersion = 601;
		else if(fileVersion == 2630)						// 6.0 SR4
			fileVersion = 604;
		else if(fileVersion == 2635)						// 6.1
			fileVersion = 610;
		else if(fileVersion >= 2656 && fileVersion < 2659)	// 7.0
			fileVersion = 700;
		else if(fileVersion >= 2659 && fileVersion <= 2664)	// 7.0 SR1
			fileVersion = 701;
		else if(fileVersion == 2672)						// 7.0 SR3
			fileVersion = 703;
		else if(fileVersion == 2673)						// 7.0 E
			fileVersion = 704;
		else if(fileVersion >= 2766 && fileVersion <= 2769)	// 7.5
			fileVersion = 750;
		else if(fileVersion >= 2876 && fileVersion <= 2906)	// 8.0
			fileVersion = 800;
		else if(fileVersion >= 2907 && fileVersion < 2944)	// 8.1
			fileVersion = 810;
		else if(fileVersion >= 2944 && fileVersion < 2962)	// 8.5 SR1
			fileVersion = 850;
		else if(fileVersion >= 2962)						// 8.5.1 SR2
			fileVersion = 851;
		else {
			LOG_PRINT(logfile, "Found unknown project version %d\n", fileVersion);
			LOG_PRINT(logfile, "Please contact the authors of liborigin2");
	#ifndef NO_LOG_FILE
		unsigned int ioret;
		ioret = fclose(logfile);
	#endif
			throw std::logic_error("Unknown project version");
		}
	}
	LOG_PRINT(logfile, "Found project version %.2f\n", (fileVersion/100.0));
#ifndef NO_LOG_FILE
	fclose(logfile);
#endif
	switch(fileVersion){
		case 850:
		case 851:
			parser.reset(createOrigin850Parser(fileName));
			break;
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
		case 701:
		case 703:
		case 704:
			parser.reset(createOrigin700Parser(fileName));
			break;
		case 600:
		case 601:
		case 604:
		case 610:
			parser.reset(createOrigin610Parser(fileName));
			break;
		case 500:
		case 502:
			parser.reset(createOrigin500Parser(fileName));
			break;
		case 350:
		case 400:
		case 410:
		default:
			parser.reset(createOrigin410Parser(fileName));
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
	return parser->matrices.size();
}

Origin::Matrix& OriginFile::matrix(vector<Origin::Matrix>::size_type m) const
{
	return parser->matrices[m];
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
