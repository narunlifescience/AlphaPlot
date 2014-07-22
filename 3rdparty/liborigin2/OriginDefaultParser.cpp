/***************************************************************************
    File                 : OriginDefaultParser.cpp
    --------------------------------------------------------------------
    Copyright            : (C) 2005-2008 Stefan Gerlach
						   (C) 2007-2008 Alex Kargovsky, Ion Vasilief
    Email (use @ for *)  : kargovsky*yumr.phys.msu.su, ion_vasilief*yahoo.fr
    Description          : Default Origin file parser class

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

#include "OriginDefaultParser.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <cstring>

using namespace Origin;

#define MAX_LEVEL 20
#define ERROR_MSG "Please send the OPJ file and the opjfile.log to the author of liborigin!\n"

#define SwapBytes(x) ByteSwap((unsigned char *) &x,sizeof(x))

void ByteSwap(unsigned char * b, int n) {
	register int i = 0;
	register int j = n-1;
	while (i<j) {
		std::swap(b[i], b[j]);
		i++, j--;
	}
}

bool IsBigEndian()
{
	short word = 0x4321;
	if((*(char *)& word) != 0x21 )
		return true;
	else
		return false;
}

OriginDefaultParser::OriginDefaultParser(const string& fileName)
:	fileName(fileName)
{
}

bool OriginDefaultParser::parse()
{
	int i;
	FILE *f, *debug;
	if((f=fopen(fileName.c_str(),"rb")) == NULL ) {
		printf("Could not open %s!\n", fileName.c_str());
		return false;
	}

	if((debug=fopen("opjfile.log","a")) == NULL ) {
		printf("Could not open log file!\n");
		return false;
	}

	////////////////////////////// check version from header ///////////////////////////////
	char vers[5];
	vers[4]=0;

	// get version
	fseek(f,0x7,SEEK_SET);
	fread(&vers,4,1,f);
	int version = atoi(vers);

	unsigned char c=0;	// tmp char

	fprintf(debug,"HEADER :\n");
	for(i=0;i<0x16;i++) {	// skip header + 5 Bytes ("27")
		fread(&c,1,1,f);
		fprintf(debug,"%.2X ",c);
		if(!((i+1)%16)) fprintf(debug,"\n");
	}
	fprintf(debug,"\n");

	do{
		fread(&c,1,1,f);
	} while (c != '\n');
	fprintf(debug,"	[file header @ 0x%X]\n", (unsigned int) ftell(f));

	/////////////////// find column ///////////////////////////////////////////////////////////
	if(version>410)
		for(i=0;i<5;i++)	// skip "0"
			fread(&c,1,1,f);

	int col_found;
	fread(&col_found,4,1,f);
	if(IsBigEndian()) SwapBytes(col_found);

	fread(&c,1,1,f);	// skip '\n'
	fprintf(debug,"	[column found = %d/0x%X @ 0x%X]\n",col_found,col_found,(unsigned int) ftell(f));

	int current_col=1, nr=0, nbytes=0;
	double a;
	char name[25], valuesize;
	while(col_found > 0 && col_found < 0x84) {	// should be 0x72, 0x73 or 0x83
		//////////////////////////////// COLUMN HEADER /////////////////////////////////////////////
		fprintf(debug,"COLUMN HEADER :\n");
		for(i=0;i < 0x3D;i++) {	// skip 0x3C chars to value size
			fread(&c,1,1,f);
			fprintf(debug,"%.2X ",c);
			if(!((i+1)%16)) fprintf(debug,"\n");
		}
		fprintf(debug,"\n");

		fread(&valuesize,1,1,f);
		fprintf(debug,"	[valuesize = %d @ 0x%X]\n",valuesize,(unsigned int) ftell(f)-1);
		if(valuesize <= 0) {
			fprintf(debug,"	WARNING : found strange valuesize of %d\n",valuesize);
			valuesize=10;
		}

		fprintf(debug,"SKIP :\n");
		for(i=0;i<0x1A;i++) {	// skip to name
			fread(&c,1,1,f);
			fprintf(debug,"%.2X ",c);
			if(!((i+1)%16)) fprintf(debug,"\n");
		}
		fprintf(debug,"\n");

		// read name
		fprintf(debug,"	[Spreadsheet @ 0x%X]\n",(unsigned int) ftell(f));
		fflush(debug);
		fread(&name,25,1,f);
		char sname[26];
		sprintf(sname,"%s",strtok(name,"_"));	// spreadsheet name
		char* cname = strtok(NULL,"_");	// column name
		while(char* tmpstr = strtok(NULL,"_")) {	// get multiple-"_" title correct
			strcat(sname,"_");
			strcat(sname,cname);
			strcpy(cname,tmpstr);
		}
		int spread=0;
		if(speadSheets.size() == 0 || findSpreadByName(sname) == -1) {
			fprintf(debug,"NEW SPREADSHEET\n");
			current_col=1;
			speadSheets.push_back(SpreadSheet(sname));
			spread=speadSheets.size()-1;
			speadSheets.back().maxRows=0;
		}
		else {

			spread = findSpreadByName(sname);

			current_col=speadSheets[spread].columns.size();

			if(!current_col)
				current_col=1;
			current_col++;
		}
		fprintf(debug,"SPREADSHEET = %s COLUMN %d NAME = %s (@0x%X)\n",
			sname, current_col, cname, (unsigned int) ftell(f));
		fflush(debug);

		if(cname == 0) {
			fprintf(debug,"NO COLUMN NAME FOUND! Must be a Matrix or Function.\n");
			////////////////////////////// READ matrixes or functions ////////////////////////////////////
			fprintf(debug,"Reading MATRIX.\n");
			fflush(debug);

			fprintf(debug,"	[position @ 0x%X]\n",(unsigned int) ftell(f));
			// TODO
			fprintf(debug,"	SIGNATURE : ");
			for(i=0;i<2;i++) {	// skip header
				fread(&c,1,1,f);
				fprintf(debug,"%.2X ",c);
			}
			fflush(debug);

			do{	// skip until '\n'
				fread(&c,1,1,f);
			} while (c != '\n');
			fprintf(debug,"\n");
			fflush(debug);

			// read size
			int size;
			fread(&size,4,1,f);
			fread(&c,1,1,f);	// skip '\n'
			// TODO : use entry size : double, float, ...
			size /= 8;
			fprintf(debug,"	SIZE = %d\n",size);
			fflush(debug);

			// catch exception
			if(size>10000)
				size=1000;

			fprintf(debug,"VALUES :\n");
			speadSheets[speadSheets.size()-1].maxRows=1;

			double value=0;
			for(i=0;i<size;i++) {	// read data
				string stmp;
				if(i<26)
					stmp=i+0x41;
				else if(i<26*26) {
					stmp = 0x40+i/26;
					stmp[1] = i%26+0x41;
				}
				else {
					stmp = 0x40+i/26/26;
					stmp[1] = i/26%26+0x41;
					stmp[2] = i%26+0x41;
				}
				speadSheets[speadSheets.size()-1].columns.push_back(stmp);
				fread(&value,8,1,f);
				speadSheets[speadSheets.size()-1].columns[i].data.push_back(/*Data(value)*/value);

				fprintf(debug,"%g ",value);
			}
			fprintf(debug,"\n");
			fflush(debug);

		}
		else {	// worksheet
			speadSheets[spread].columns.push_back(SpreadColumn(cname));

			////////////////////////////// SIZE of column /////////////////////////////////////////////
			do{	// skip until '\n'
				fread(&c,1,1,f);
			} while (c != '\n');

			fread(&nbytes,4,1,f);
			if(IsBigEndian()) SwapBytes(nbytes);
			if(fmod(nbytes,(double)valuesize)>0)
				fprintf(debug,"WARNING: data section could not be read correct\n");
			nr = nbytes / valuesize;
			fprintf(debug,"	[number of rows = %d (%d Bytes) @ 0x%X]\n",nr,nbytes,(unsigned int) ftell(f));
			fflush(debug);

			speadSheets[spread].maxRows<nr?speadSheets[spread].maxRows=nr:0;

			////////////////////////////////////// DATA ////////////////////////////////////////////////
			fread(&c,1,1,f);	// skip '\n'
			if(valuesize != 8 && valuesize <= 16) {	// skip 0 0
				fread(&c,1,1,f);
				fread(&c,1,1,f);
			}
			fprintf(debug,"	[data @ 0x%X]\n",(unsigned int) ftell(f));
			fflush(debug);

			for (i=0;i<nr;i++) {
				if(valuesize <= 16) {	// value
					fread(&a,valuesize,1,f);
					if(IsBigEndian()) SwapBytes(a);
					fprintf(debug,"%g ",a);
					speadSheets[spread].columns[(current_col-1)].data.push_back(/*Data(a)*/a);
				}
				else {			// label
					char *stmp = new char[valuesize+1];
					fread(stmp,valuesize,1,f);
					fprintf(debug,"%s ",stmp);
					speadSheets[spread].columns[(current_col-1)].data.push_back(/*Data(stmp)*/string(stmp));
					delete stmp;
				}
			}
		}
		fprintf(debug,"\n");
		fflush(debug);

		for(i=0;i<4;i++)	// skip "0"
			fread(&c,1,1,f);
		if(valuesize == 8 || valuesize > 16) {	// skip 0 0
			fread(&c,1,1,f);
			fread(&c,1,1,f);
		}
		fread(&col_found,4,1,f);
		if(IsBigEndian()) SwapBytes(col_found);
		fread(&c,1,1,f);	// skip '\n'
		fprintf(debug,"	[column found = %d/0x%X (@ 0x%X)]\n",col_found,col_found,(unsigned int) ftell(f)-5);
		fflush(debug);
	}

	////////////////////// HEADER SECTION //////////////////////////////////////
	// TODO : use new method ('\n')

	int POS = ftell(f)-11;
	fprintf(debug,"\nHEADER SECTION\n");
	fprintf(debug,"	nr_spreads = %d\n",speadSheets.size());
	fprintf(debug,"	[position @ 0x%X]\n",POS);
	fflush(debug);

	///////////////////// speadSheets INFOS ////////////////////////////////////
	int LAYER=0;
	int COL_JUMP = 0x1ED;
	for(unsigned int i=0; i < speadSheets.size(); i++) {
		fprintf(debug,"		reading	Spreadsheet %d/%d properties\n", i+1, speadSheets.size());
		fflush(debug);
		if(i > 0) {
			if (version == 700 )
				POS += 0x2530 + speadSheets[i-1].columns.size()*COL_JUMP;
			else if (version == 610 )
				POS += 0x25A4 + speadSheets[i-1].columns.size()*COL_JUMP;
			else if (version == 604 )
				POS += 0x25A0 + speadSheets[i-1].columns.size()*COL_JUMP;
			else if (version == 601 )
				POS += 0x2560 + speadSheets[i-1].columns.size()*COL_JUMP;	// ?
			else if (version == 600 )
				POS += 0x2560 + speadSheets[i-1].columns.size()*COL_JUMP;
			else if (version == 500 )
				POS += 0x92C + speadSheets[i-1].columns.size()*COL_JUMP;
			else if (version == 410 )
				POS += 0x7FB + speadSheets[i-1].columns.size()*COL_JUMP;
		}

		fprintf(debug,"			reading	Header\n");
		fflush(debug);
		// HEADER
		// check header
		int ORIGIN = 0x55;
		if(version == 500)
			ORIGIN = 0x58;
		fseek(f,POS + ORIGIN,SEEK_SET);	// check for 'O'RIGIN
		char c;
		fread(&c,1,1,f);
		int jump=0;
		if( c == 'O')
			fprintf(debug,"			\"ORIGIN\" found ! (@ 0x%X)\n",POS+ORIGIN);
		while( c != 'O' && jump < MAX_LEVEL) {	// no inf loop
			fprintf(debug,"		TRY %d	\"O\"RIGIN not found ! : %c (@ 0x%X)",jump+1,c,POS+ORIGIN);
			fprintf(debug,"			POS=0x%X | ORIGIN = 0x%X\n",POS,ORIGIN);
			fflush(debug);
			POS+=0x1F2;
			fseek(f,POS + ORIGIN,SEEK_SET);
			fread(&c,1,1,f);
			jump++;
		}

		int spread=i;
		if(jump == MAX_LEVEL){
			fprintf(debug,"		Spreadsheet SECTION not found ! 	(@ 0x%X)\n",POS-10*0x1F2+0x55);
			return -5;
		}

		fprintf(debug,"			[Spreadsheet SECTION (@ 0x%X)]\n",POS);
		fflush(debug);

		// check spreadsheet name
		fseek(f,POS + 0x12,SEEK_SET);
		fread(&name,25,1,f);

		spread=findSpreadByName(name);
		if(spread == -1)
			spread=i;

		fprintf(debug,"			SPREADSHEET %d NAME : %s	(@ 0x%X) has %d columns\n",
			spread+1,name,POS + 0x12,speadSheets[spread].columns.size());
		fflush(debug);

		int ATYPE=0;
		LAYER = POS;
		if (version == 700)
			ATYPE = 0x2E4;
		else if (version == 610)
			ATYPE = 0x358;
		else if (version == 604)
			ATYPE = 0x354;
		else if (version == 601)
			ATYPE = 0x500;	// ?
		else if (version == 600)
			ATYPE = 0x314;
		else if (version == 500) {
			COL_JUMP=0x5D;
			ATYPE = 0x300;
		}
		else if (version == 410) {
			COL_JUMP = 0x58;
			ATYPE = 0x229;
		}
		fflush(debug);

		/////////////// COLUMN Types ///////////////////////////////////////////
		fprintf(debug,"			Spreadsheet has %d columns\n",speadSheets[spread].columns.size());
		for (unsigned int j=0; j<speadSheets[spread].columns.size(); j++) {
			fprintf(debug,"			reading	COLUMN %d/%d type\n", j + 1, speadSheets[spread].columns.size());
			fflush(debug);
			fseek(f,LAYER+ATYPE+j*COL_JUMP, SEEK_SET);
			fread(&name,25,1,f);

			fseek(f,LAYER+ATYPE+j*COL_JUMP-1, SEEK_SET);
			fread(&c, 1, 1, f);

			SpreadColumn::ColumnType type;
			switch(c) {
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

			speadSheets[spread].columns[j].type = type;

			fprintf(debug,"				COLUMN \"%s\" type = %d (@ 0x%X)\n",
				speadSheets[spread].columns[j].name.c_str(), type, LAYER+ATYPE+j*COL_JUMP);
			fflush(debug);

			// check column name
			int max_length=11;	// only first 11 chars are saved here !
			int name_length = speadSheets[spread].columns[j].name.length();
			int length = (name_length < max_length) ? name_length : max_length;

			if(speadSheets[spread].columns[j].name.substr(0,length) == name) {
				fprintf(debug,"				TEST : column name = \"%s\". OK!\n",
					speadSheets[spread].columns[j].name.c_str());
			}
			else {
				fprintf(debug,"				TEST : COLUMN %d name mismatch (\"%s\" != \"%s\")\n",
					j+1,name,speadSheets[spread].columns[j].name.c_str());
			}
			fflush(debug);
		}
		fprintf(debug,"		Done with spreadsheet %d\n",spread);
		fflush(debug);
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	// TODO : GRAPHS

	fprintf(debug,"Done parsing\n");
	fclose(debug);

	return true;
}

OriginParser* createOriginDefaultParser(const string& fileName)
{
	return new OriginDefaultParser(fileName);
}
