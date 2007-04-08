// OPJFile.cpp

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <algorithm> //required for std::swap
#include "OPJFile.h"

#define MAX_LEVEL 20
#define ERROR_MSG "Please send the OPJ file and the opjfile.log to the author of liborigin!\n"

#define SwapBytes(x) ByteSwap((unsigned char *) &x,sizeof(x))

void OPJFile::ByteSwap(unsigned char * b, int n) {
	register int i = 0;
	register int j = n-1;
	while (i<j) {       
		std::swap(b[i], b[j]);
		i++, j--;
	}       
}

OPJFile::OPJFile(const char *filename) 
	: filename(filename) 
{
	version=0;
//	nr_spreads=0;
//	entry=0;
}

int OPJFile::compareSpreadnames(char *sname) {
	for(int i=0;i<SPREADSHEET.size();i++)
		if (SPREADSHEET[i].name == sname)
			return i;
	return -1;
}

int OPJFile::compareColumnnames(int spread, char *sname) {
	for(int i=0;i<SPREADSHEET[spread].column.size();i++)
		if (SPREADSHEET[spread].column[i].name == sname)
			return i;
	return -1;
}

int OPJFile::compareMatrixnames(char *sname) {
	for(int i=0;i<MATRIX.size();i++)
		if (MATRIX[i].name == sname)
			return i;
	return -1;
}

// set default name for columns starting from spreadsheet spread
void OPJFile::setColName(int spread) {
	for(int j=spread;j<SPREADSHEET.size();j++) {
		SPREADSHEET[j].column[0].type="X";	
		for (int k=1;k<SPREADSHEET[j].column.size();k++)
			SPREADSHEET[j].column[k].type="Y";
	}
}

/* File Structure :
filepre +
	+ pre + head + data	col A
	+ pre + head + data	col B
*/

/* parse file "filename" completely and save values */
int OPJFile::Parse() {
	int i,j;
	FILE *f, *debug;
	if((f=fopen(filename,"rb")) == NULL ) {
		printf("Could not open %s!\n",filename);
		return -1;
	}
	
	if((debug=fopen("opjfile.log","w")) == NULL ) {
		printf("Could not open log file!\n");
		return -1;
	}

////////////////////////////// check version from header ///////////////////////////////
	char vers[5];
	vers[4]=0;

	// get version
	fseek(f,0x7,SEEK_SET);
	fread(&vers,4,1,f);
	version = atoi(vers);
	fprintf(debug,"	[version = %d]\n",version);
	
	// translate version
	if(version >= 130 && version <= 140) 		// 4.1
		version=410;
	else if(version == 210) 	// 5.0
		version=500;
	else if(version == 2625) 	// 6.0
		version=600;
	else if(version == 2627) 	// 6.0 SR1
		version=601;
	else if(version == 2630 ) 	// 6.0 SR4
		version=604;
	else if(version == 2635 ) 	// 6.1
		version=610;
	else if(version == 2656) 	// 7.0
		version=700;
	else if(version == 2672) 	// 7.0 SR3
		version=703;
	else if(version >= 2766 && version <= 2769) 	// 7.5
		version=750;
	else {
		fprintf(debug,"Found unknown project version %d\n",version);
		fprintf(debug,"Please contact the author of opj2dat\n");
	}
	fprintf(debug,"Found project version %.2f\n",version/100.0);
	
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
		short data_type;
		char data_type_u;
		int oldpos=ftell(f);
		fseek(f,oldpos+0x16,SEEK_SET);
		fread(&data_type,2,1,f);
		fseek(f,oldpos+0x3F,SEEK_SET);
		fread(&data_type_u,1,1,f);
		fseek(f,oldpos,SEEK_SET);

		fprintf(debug,"COLUMN HEADER :\n");
		for(i=0;i < 0x3D;i++) {	// skip 0x3C chars to value size
			fread(&c,1,1,f);
			//if(i>21 && i<27) {
				fprintf(debug,"%.2X ",c);
				if(!((i+1)%16)) fprintf(debug,"\n");
			//}
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
		//char* sname = new char[26];
		char sname[26];
		sprintf(sname,"%s",strtok(name,"_"));	// spreadsheet name
		char* cname = strtok(NULL,"_");	// column name
		while(char* tmpstr = strtok(NULL,"_")) {	// get multiple-"_" title correct
			strcat(sname,"_");
			strcat(sname,cname);
			strcpy(cname,tmpstr);
		}
		int spread=0;
		if(cname == 0) {
			fprintf(debug,"NO COLUMN NAME FOUND! Must be a matrix or function.\n");
////////////////////////////// READ MATRIX or FUNCTION ////////////////////////////////////
			fprintf(debug,"Reading MATRIX.\n");
			fflush(debug);

			fprintf(debug,"	[position @ 0x%X]\n",(unsigned int) ftell(f));
			// TODO
			if(version==750)
			{
				fprintf(debug,"	SIGNATURE : ");
				for(i=0;i<2;i++) {	// skip header
					fread(&c,1,1,f);
					fprintf(debug,"%.2X ",c);
				}
				fflush(debug);
		
				do{	// skip until '\n'
					fread(&c,1,1,f);
					// fprintf(debug,"%.2X ",c);
				} while (c != '\n');
				fprintf(debug,"\n");
				fflush(debug);
				
				// read size
				int size;
				fread(&size,4,1,f);
				fread(&c,1,1,f);	// skip '\n'
				// TODO : use entry size : double, float, ...
				size /= valuesize;
				fprintf(debug,"	SIZE = %d\n",size);
				fflush(debug);

				// catch exception
				if(size>10000)
					size=1000;

				fprintf(debug,"NEW MATRIX\n");
				MATRIX.push_back(matrix(sname));
		
				fprintf(debug,"VALUES :\n");

				switch(data_type)
				{
				case 0x6001://double
					for(i=0;i<size;i++) {
						double value;
						fread(&value,valuesize,1,f);
						MATRIX.back().data.push_back((double)value);
						fprintf(debug,"%g ",MATRIX.back().data.back());
					}
					break;
				case 0x6003://float
					for(i=0;i<size;i++) {
						float value;
						fread(&value,valuesize,1,f);
						MATRIX.back().data.push_back((double)value);
						fprintf(debug,"%g ",MATRIX.back().data.back());
					}
					break;
				case 0x6801://int
					if(data_type_u==8)//unsigned
						for(i=0;i<size;i++) {
							unsigned int value;
							fread(&value,valuesize,1,f);
							MATRIX.back().data.push_back((double)value);
							fprintf(debug,"%g ",MATRIX.back().data.back());
						}
					else
						for(i=0;i<size;i++) {
							int value;
							fread(&value,valuesize,1,f);
							MATRIX.back().data.push_back((double)value);
							fprintf(debug,"%g ",MATRIX.back().data.back());
						}
					break;
				case 0x6803://short
					if(data_type_u==8)//unsigned
						for(i=0;i<size;i++) {
							unsigned short value;
							fread(&value,valuesize,1,f);
							MATRIX.back().data.push_back((double)value);
							fprintf(debug,"%g ",MATRIX.back().data.back());
						}
					else
						for(i=0;i<size;i++) {
							short value;
							fread(&value,valuesize,1,f);
							MATRIX.back().data.push_back((double)value);
							fprintf(debug,"%g ",MATRIX.back().data.back());
						}
					break;
				case 0x6821://char
					if(data_type_u==8)//unsigned
						for(i=0;i<size;i++) {
							unsigned char value;
							fread(&value,valuesize,1,f);
							MATRIX.back().data.push_back((double)value);
							fprintf(debug,"%g ",MATRIX.back().data.back());
						}
					else
						for(i=0;i<size;i++) {
							char value;
							fread(&value,valuesize,1,f);
							MATRIX.back().data.push_back((double)value);
							fprintf(debug,"%g ",MATRIX.back().data.back());
						}
					break;
				}
				if(valuesize!=8)
					fseek(f, ftell(f)+2, SEEK_SET); //ugly trick - need to be fixed

				fprintf(debug,"\n");
				fflush(debug);
			}
			else
			{
				fprintf(debug,"NEW SPREADSHEET\n");
				current_col=1;
				SPREADSHEET.push_back(spreadSheet(sname));
				spread=SPREADSHEET.size()-1;
				SPREADSHEET.back().maxRows=0;
				// TODO
				fprintf(debug,"	SIGNATURE : ");
				for(i=0;i<2;i++) {	// skip header
					fread(&c,1,1,f);
					fprintf(debug,"%.2X ",c);
				}
				fflush(debug);
		
				do{	// skip until '\n'
					fread(&c,1,1,f);
					// fprintf(debug,"%.2X ",c);
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
				SPREADSHEET[SPREADSHEET.size()-1].maxRows=1;

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
					SPREADSHEET[SPREADSHEET.size()-1].column.push_back(stmp);
					fread(&value,8,1,f);
					SPREADSHEET[SPREADSHEET.size()-1].column[i].data.push_back(value);

					fprintf(debug,"%g ",value);
				}
				fprintf(debug,"\n");
				fflush(debug);
			}
			
		}
		else {	// worksheet
			if(SPREADSHEET.size() == 0 || compareSpreadnames(sname) == -1) {
				fprintf(debug,"NEW SPREADSHEET\n");
				current_col=1;
				SPREADSHEET.push_back(spreadSheet(sname));
				spread=SPREADSHEET.size()-1;
				SPREADSHEET.back().maxRows=0;
			}
			else {

				spread=compareSpreadnames(sname);

				current_col=SPREADSHEET[spread].column.size();

				if(!current_col)
					current_col=1;
				current_col++;
			}
			fprintf(debug,"SPREADSHEET = %s COLUMN NAME = %s (%d) (@0x%X)\n",
				sname, cname,current_col,(unsigned int) ftell(f));
			fflush(debug);
			SPREADSHEET[spread].column.push_back(spreadColumn(cname));

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

			SPREADSHEET[spread].maxRows<nr?SPREADSHEET[spread].maxRows=nr:0;

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
					SPREADSHEET[spread].column[(current_col-1)].data.push_back(a);
				}
				else {			// label or text
					char *stmp = new char[valuesize+1];
					fread(stmp,valuesize,1,f);
					if(strchr(stmp,0x0E)) // try find non-printable symbol - garbage test
						stmp[0]='\0';
					SPREADSHEET[spread].column[(current_col-1)].sdata.push_back(stmp);
					fprintf(debug,"%s ",stmp);
					delete stmp;
				}
			}
		}	// else
//		fprintf(debug,"	[now @ 0x%X]\n",ftell(f));
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
	fprintf(debug,"	nr_spreads = %d\n",SPREADSHEET.size());
	fprintf(debug,"	[position @ 0x%X]\n",POS);
	fflush(debug);

///////////////////// SPREADSHEET INFOS ////////////////////////////////////
	int LAYER=0;
	int COL_JUMP = 0x1ED;
	if(version == 750)
	{
		POS+=0xB;
		fseek(f,POS,SEEK_SET);
		while(1) {
			
			fprintf(debug,"			reading	Header\n");
			fflush(debug);
			// HEADER
			// check header
			POS=ftell(f);
			int headersize;
			fread(&headersize,4,1,f);
			if(headersize==0)
				break;
			char object_type[10];
			char object_name[25];
			fseek(f,POS + 0x7,SEEK_SET);
			fread(&object_name,25,1,f);
			fseek(f,POS + 0x4A,SEEK_SET);
			fread(&object_type,10,1,f);
			
			fseek(f,POS,SEEK_SET);
			if(0==strcmp(object_type,"ORIGIN")||0==strcmp(object_type,"CREATE")||0==strcmp(object_type,"FFT"))
			{
				if(compareSpreadnames(object_name)!=-1)
					readSpreadInfo(f, debug);
				else if(compareMatrixnames(object_name)!=-1)
					readMatrixInfo(f, debug);
			}
			else
			{
				skipObjectInfo(f, debug);
				i--;
			}
		}
	}
	else
		for(i=0; i < SPREADSHEET.size(); i++) {
		fprintf(debug,"		reading	Spreadsheet %d/%d properties\n",i+1,SPREADSHEET.size());
		fflush(debug);
		if(i > 0) {
			if (version == 700 ) 
				POS += 0x2530 + SPREADSHEET[i-1].column.size()*COL_JUMP;
			else if (version == 610 ) 
				POS += 0x25A4 + SPREADSHEET[i-1].column.size()*COL_JUMP;
			else if (version == 604 ) 
				POS += 0x25A0 + SPREADSHEET[i-1].column.size()*COL_JUMP;
			else if (version == 601 ) 
				POS += 0x2560 + SPREADSHEET[i-1].column.size()*COL_JUMP;	// ?
			else if (version == 600 ) 
				POS += 0x2560 + SPREADSHEET[i-1].column.size()*COL_JUMP;
			else if (version == 500 ) 
				POS += 0x92C + SPREADSHEET[i-1].column.size()*COL_JUMP;
			else if (version == 410 ) 
				POS += 0x7FB + SPREADSHEET[i-1].column.size()*COL_JUMP;
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
			// setColName(spread);
			return -5;
		}
		
		fprintf(debug,"			[Spreadsheet SECTION (@ 0x%X)]\n",POS);
		fflush(debug);
	
		// check spreadsheet name
		fseek(f,POS + 0x12,SEEK_SET);
		fread(&name,25,1,f);

		spread=compareSpreadnames(name);
		
		fprintf(debug,"			SPREADSHEET %d NAME : %s	(@ 0x%X) has %d columns\n",
			spread+1,name,POS + 0x12,SPREADSHEET[spread].column.size());
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
		fprintf(debug,"			Spreadsheet has %d columns\n",SPREADSHEET[spread].column.size());
		for (j=0;j<SPREADSHEET[spread].column.size();j++) {
			fprintf(debug,"			reading	COLUMN %d/%d type\n",j+1,SPREADSHEET[spread].column.size());
			fflush(debug);
			fseek(f,LAYER+ATYPE+j*COL_JUMP, SEEK_SET);
			fread(&name,25,1,f);

			fseek(f,LAYER+ATYPE+j*COL_JUMP-1, SEEK_SET);
			fread(&c,1,1,f);
			char type[5];
			switch(c) {
			case 3: sprintf(type,"X");break;
			case 0: sprintf(type,"Y");break;
			case 5: sprintf(type,"Z");break;
			case 6: sprintf(type,"DX");break;
			case 2: sprintf(type,"DY");break;
			case 4: sprintf(type,"LABEL");break;
			default: sprintf(type,"NONE");break;
			}
			
			SPREADSHEET[spread].column[j].type=type;
			
			fprintf(debug,"				COLUMN \"%s\" type = %s (@ 0x%X)\n",
				SPREADSHEET[spread].column[j].name.c_str(),type,LAYER+ATYPE+j*COL_JUMP);
			fflush(debug);

			// check column name
			int max_length=11;	// only first 11 chars are saved here !
			int name_length = SPREADSHEET[spread].column[j].name.length();
                        int length = (name_length < max_length) ? name_length : max_length;

			if(SPREADSHEET[spread].column[j].name.substr(0,length) == name) {
				fprintf(debug,"				TEST : column name = \"%s\". OK!\n",
					SPREADSHEET[spread].column[j].name.c_str());
			}
			else {
				fprintf(debug,"				TEST : COLUMN %d name mismatch (\"%s\" != \"%s\")\n",
					j+1,name,SPREADSHEET[spread].column[j].name.c_str());
				//fprintf(debug,"ERROR : column name mismatch! Continue anyway.\n"ERROR_MSG);
			}
			fflush(debug);
		}
		fprintf(debug,"		Done with spreadsheet %d\n",spread);
		fflush(debug);
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// TODO : GRAPHS
/*	int graph = 0x2fc1;
	int pre_graph = 0x12;
	fseek(f,graph + pre_graph,SEEK_SET);
	fread(&name,25,1,f);
	printf("GRAPH : %s\n",name);

	fseek(f,graph + pre_graph + 0x43, SEEK_SET);
	fread(&name,25,1,f);
	printf("TYPE : %s\n",name);

	fseek(f,graph + pre_graph + 0x2b3, SEEK_SET);
	fread(&name,25,1,f);
	printf("Y AXIS TITLE : %s\n",name);
	fseek(f,graph + pre_graph + 0x38d, SEEK_SET);
	fread(&name,25,1,f);
	printf("X AXIS TITLE : %s\n",name);

	fseek(f,graph + pre_graph + 0xadb, SEEK_SET);
	fread(&name,25,1,f);
	printf("LEGEND : %s\n",name);
*/

	fprintf(debug,"Done parsing\n");
	fclose(debug);

	return 0;
}

void OPJFile::readSpreadInfo(FILE *f, FILE *debug)
{
	int POS=ftell(f);
	
	int headersize;
	fread(&headersize,4,1,f);
	POS+=5;

	fprintf(debug,"			[Spreadsheet SECTION (@ 0x%X)]\n",POS);
	fflush(debug);

	// check spreadsheet name
	char name[25];
	fseek(f,POS + 0x2,SEEK_SET);
	fread(&name,25,1,f);

	int spread=compareSpreadnames(name);
	
	fprintf(debug,"			SPREADSHEET %d NAME : %s	(@ 0x%X) has %d columns\n",
		spread+1,name,POS + 0x2,SPREADSHEET[spread].column.size());
	fflush(debug);

	char c;
	fseek(f,POS + 0x69,SEEK_SET);
	fread(&c,1,1,f);
	if(c==0x5F)
	{
		SPREADSHEET[spread].bHidden=true;
		fprintf(debug,"			SPREADSHEET %d NAME : %s	is hidden\n", spread+1,name);
		fflush(debug);
	}
	SPREADSHEET[spread].bLoose=false;

	if(headersize>0xC3)
	{
		int labellen=0;
		fseek(f,POS + 0xC3,SEEK_SET);
		fread(&c,1,1,f);
		while (c != '@'){
			fread(&c,1,1,f);
			labellen++;
		}
		if(labellen>0)
		{
			char label[255];
			label[labellen]='\0';
			fseek(f,POS + 0xC3,SEEK_SET);
			fread(&label,labellen,1,f);
			SPREADSHEET[spread].label=label;
		}
		else
			SPREADSHEET[spread].label="";
		fprintf(debug,"			SPREADSHEET %d LABEL : %s\n",spread+1,SPREADSHEET[spread].label.c_str());
		fflush(debug);
	}

	int LAYER = POS;
	{
		// LAYER section
		LAYER += headersize + 0x1 + 0x5/* length of block = 0x12D + '\n'*/ + 0x12D + 0x1;
		//now structure is next : section_header_size=0x6F(4 bytes) + '\n' + section_header(0x6F bytes) + section_body_1_size(4 bytes) + '\n' + section_body_1 + section_body_2_size(maybe=0)(4 bytes) + '\n' + section_body_2 + '\n'
		//possible sections: column formulas, __WIPR, __WIOTN, __LayerInfoStorage etc
		//section name(column name in formula case) starts with 0x46 position
		while(1)
		{
			int sec_size;
		//section_header_size=0x6F(4 bytes) + '\n'
			LAYER+=0x5;
							
		//section_header
			fseek(f,LAYER+0x46,SEEK_SET);
			char sec_name[42];
			sec_name[41]='\0';
			fread(&sec_name,41,1,f);

		//section_body_1_size
			LAYER+=0x6F+0x1;
			fseek(f,LAYER,SEEK_SET);
			fread(&sec_size,4,1,f);

		//section_body_1
			LAYER+=0x5;
			fseek(f,LAYER,SEEK_SET);
			//check if it is a formula
			int col_index=compareColumnnames(spread,sec_name);
			if(col_index!=-1)
			{
				char stmp[255];
				stmp[sec_size]='\0';
				fread(&stmp,sec_size,1,f);
				SPREADSHEET[spread].column[col_index].command=stmp;
				/*delete stmp;*/
			}

		//section_body_2_size
			LAYER+=sec_size+0x1;
			fseek(f,LAYER,SEEK_SET);
			fread(&sec_size,4,1,f);

		//section_body_2
			LAYER+=0x5;

		//close section 00 00 00 00 0A
			LAYER+=sec_size+(sec_size>0?0x1:0)+0x5;

			if(0==strcmp(sec_name,"__LayerInfoStorage"))
				break;

		}
		LAYER+=0x5;

	}
	
	fflush(debug);

	/////////////// COLUMN Types ///////////////////////////////////////////
	fprintf(debug,"			Spreadsheet has %d columns\n",SPREADSHEET[spread].column.size());

	while(1)
	{
		LAYER+=0x5;
		fseek(f,LAYER+0x12, SEEK_SET);
		fread(&name,12,1,f);

		fseek(f,LAYER+0x11, SEEK_SET);
		fread(&c,1,1,f);
		unsigned char width=0;
		fseek(f,LAYER+0x4A, SEEK_SET);
		fread(&width,2,1,f);
		int col_index=compareColumnnames(spread,name);
		if(col_index!=-1)
		{
			char type[5];
			switch(c) {
				case 3: sprintf(type,"X");break;
				case 0: sprintf(type,"Y");break;
				case 5: sprintf(type,"Z");break;
				case 6: sprintf(type,"DX");break;
				case 2: sprintf(type,"DY");break;
				case 4: sprintf(type,"LABEL");break;
				default: sprintf(type,"NONE");break;
			}
			SPREADSHEET[spread].column[col_index].type=type;
			width/=0xA;
			if(width==0)
				width=8;
			SPREADSHEET[spread].column[col_index].width=width;
			fseek(f,LAYER+0x1E, SEEK_SET);
			unsigned char c1,c2;
			fread(&c1,1,1,f);
			fread(&c2,1,1,f);
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
				SPREADSHEET[spread].column[col_index].value_type=(c1%0x10==0x9)?6:0;
				SPREADSHEET[spread].column[col_index].value_type_specification=c1/0x10;
				if(c2>=0x80)
				{
					SPREADSHEET[spread].column[col_index].significant_digits=c2-0x80;
					SPREADSHEET[spread].column[col_index].numeric_display_type=2;
				}
				else if(c2>0)
				{
					SPREADSHEET[spread].column[col_index].decimal_places=c2-0x03;
					SPREADSHEET[spread].column[col_index].numeric_display_type=1;
				}
				break;
			case 0x02: // Time
				SPREADSHEET[spread].column[col_index].value_type=3;
				SPREADSHEET[spread].column[col_index].value_type_specification=c2-0x80;
				break;
			case 0x03: // Date
				SPREADSHEET[spread].column[col_index].value_type=2;
				SPREADSHEET[spread].column[col_index].value_type_specification=c2-0x80;
				break;
			case 0x31: // Text
				SPREADSHEET[spread].column[col_index].value_type=1;
				break;
			case 0x34: // Month
				SPREADSHEET[spread].column[col_index].value_type=4;
				SPREADSHEET[spread].column[col_index].value_type_specification=c2;
				break;
			case 0x35: // Day
				SPREADSHEET[spread].column[col_index].value_type=5;
				SPREADSHEET[spread].column[col_index].value_type_specification=c2;
				break;
			default: // Text
				SPREADSHEET[spread].column[col_index].value_type=1;
				break;
			}
			fprintf(debug,"				COLUMN \"%s\" type = %s (@ 0x%X)\n",
				SPREADSHEET[spread].column[col_index].name.c_str(),type,LAYER+0x46);
			fflush(debug);
		}
		LAYER+=0x1E7+0x1;
		fseek(f,LAYER,SEEK_SET);
		int comm_size=0;
		fread(&comm_size,4,1,f);
		LAYER+=0x5;
		if(comm_size>0)
		{
			char comment[255];
			comment[comm_size]='\0';
			fseek(f,LAYER,SEEK_SET);
			fread(&comment,comm_size,1,f);
			if(col_index!=-1)
				SPREADSHEET[spread].column[col_index].comment=comment;
			LAYER+=comm_size+0x1;
		}
		fseek(f,LAYER,SEEK_SET);
		int ntmp;
		fread(&ntmp,4,1,f);
		if(ntmp!=0x1E7)
			break;
	}
	fprintf(debug,"		Done with spreadsheet %d\n",spread);
	fflush(debug);

	POS = LAYER+0x5*0x6+0x1ED*0x12;
	fseek(f,POS,SEEK_SET);
}

void OPJFile::readMatrixInfo(FILE *f, FILE *debug)
{
	int POS=ftell(f);
	
	int headersize;
	fread(&headersize,4,1,f);
	POS+=5;

	fprintf(debug,"			[Matrix SECTION (@ 0x%X)]\n",POS);
	fflush(debug);

	// check spreadsheet name
	char name[25];
	fseek(f,POS + 0x2,SEEK_SET);
	fread(&name,25,1,f);

	int idx=compareMatrixnames(name);
	
	fprintf(debug,"			MATRIX %d NAME : %s	(@ 0x%X) \n", idx+1,name,POS + 0x2);
	fflush(debug);

	if(headersize>0xC3)
	{
		int labellen=0;
		char c=0;
		fseek(f,POS + 0xC3,SEEK_SET);
		fread(&c,1,1,f);
		while (c != '@'){
			fread(&c,1,1,f);
			labellen++;
		}
		if(labellen>0)
		{
			char label[255];
			label[labellen]='\0';
			fseek(f,POS + 0xC3,SEEK_SET);
			fread(&label,labellen,1,f);
			MATRIX[idx].label=label;
		}
		else
			MATRIX[idx].label="";
		fprintf(debug,"			MATRIX %d LABEL : %s\n",idx+1,MATRIX[idx].label.c_str());
		fflush(debug);
	}

	int LAYER = POS;
	LAYER += headersize + 0x1; 
	int sec_size;
	// LAYER section
	LAYER +=0x5;
	fseek(f,LAYER+0x2B,SEEK_SET);
	unsigned char c=0;
	fread(&c,2,1,f);
	MATRIX[idx].nr_cols=c;
	fseek(f,LAYER+0x52,SEEK_SET);
	fread(&c,2,1,f);
	MATRIX[idx].nr_rows=c;
	LAYER +=0x12D + 0x1;
	//now structure is next : section_header_size=0x6F(4 bytes) + '\n' + section_header(0x6F bytes) + section_body_1_size(4 bytes) + '\n' + section_body_1 + section_body_2_size(maybe=0)(4 bytes) + '\n' + section_body_2 + '\n'
	//possible sections: column formulas, __WIPR, __WIOTN, __LayerInfoStorage
	//section name(column name in formula case) starts with 0x46 position
	while(1)
	{
	//section_header_size=0x6F(4 bytes) + '\n'
		LAYER+=0x5;
						
	//section_header
		fseek(f,LAYER+0x46,SEEK_SET);
		char sec_name[42];
		sec_name[41]='\0';
		fread(&sec_name,41,1,f);

	//section_body_1_size
		LAYER+=0x6F+0x1;
		fseek(f,LAYER,SEEK_SET);
		fread(&sec_size,4,1,f);

	//section_body_1
		LAYER+=0x5;
		//check if it is a formula
		if(0==strcmp(sec_name,"MV"))
		{
			fseek(f,LAYER,SEEK_SET);
			char stmp[255];
			stmp[sec_size]='\0';
			fread(&stmp,sec_size,1,f);
			MATRIX[idx].command=stmp;
		}

	//section_body_2_size
		LAYER+=sec_size+0x1;
		fseek(f,LAYER,SEEK_SET);
		fread(&sec_size,4,1,f);

	//section_body_2
		LAYER+=0x5;

	//close section 00 00 00 00 0A
		LAYER+=sec_size+(sec_size>0?0x1:0)+0x5;

		if(0==strcmp(sec_name,"__LayerInfoStorage"))
			break;

	}
	LAYER+=0x5;

	while(1)
	{
		LAYER+=0x5;

		unsigned char width=0;
		fseek(f,LAYER+0x2B, SEEK_SET);
		fread(&width,2,1,f);
		width=(width-55)/0xA;
		if(width==0)
			width=8;
		MATRIX[idx].width=width;
		fseek(f,LAYER+0x1E, SEEK_SET);
		unsigned char c1,c2;
		fread(&c1,1,1,f);
		fread(&c2,1,1,f);

		MATRIX[idx].value_type_specification=c1/0x10;
		if(c2>=0x80)
		{
			MATRIX[idx].significant_digits=c2-0x80;
			MATRIX[idx].numeric_display_type=2;
		}
		else if(c2>0)
		{
			MATRIX[idx].decimal_places=c2-0x03;
			MATRIX[idx].numeric_display_type=1;
		}
	
		LAYER+=0x1E7+0x1;
		fseek(f,LAYER,SEEK_SET);
		int comm_size=0;
		fread(&comm_size,4,1,f);
		LAYER+=0x5;
		if(comm_size>0)
		{
			LAYER+=comm_size+0x1;
		}
		fseek(f,LAYER,SEEK_SET);
		int ntmp;
		fread(&ntmp,4,1,f);
		if(ntmp!=0x1E7)
			break;
	}

	LAYER+=0x5*0x5+0x1ED*0x12;
	POS = LAYER+0x5;

	fseek(f,POS,SEEK_SET);
}

void OPJFile::skipObjectInfo(FILE *f, FILE *debug)
{
	int POS=ftell(f);
	
	int headersize;
	fread(&headersize,4,1,f);
	POS+=5;

	int LAYER = POS;
	LAYER += headersize + 0x1; 
	int sec_size;
	while(1)// multilayer loop
	{
		// LAYER section
		LAYER +=0x5/* length of block = 0x12D + '\n'*/ + 0x12D + 0x1;
		//now structure is next : section_header_size=0x6F(4 bytes) + '\n' + section_header(0x6F bytes) + section_body_1_size(4 bytes) + '\n' + section_body_1 + section_body_2_size(maybe=0)(4 bytes) + '\n' + section_body_2 + '\n'
		//possible sections: column formulas, __WIPR, __WIOTN, __LayerInfoStorage
		//section name(column name in formula case) starts with 0x46 position
		while(1)
		{
		//section_header_size=0x6F(4 bytes) + '\n'
			LAYER+=0x5;
							
		//section_header
			fseek(f,LAYER+0x46,SEEK_SET);
			char sec_name[42];
			sec_name[41]='\0';
			fread(&sec_name,41,1,f);

		//section_body_1_size
			LAYER+=0x6F+0x1;
			fseek(f,LAYER,SEEK_SET);
			fread(&sec_size,4,1,f);

		//section_body_1
			LAYER+=0x5;

		//section_body_2_size
			LAYER+=sec_size+0x1;
			fseek(f,LAYER,SEEK_SET);
			fread(&sec_size,4,1,f);

		//section_body_2
			LAYER+=0x5;

		//close section 00 00 00 00 0A
			LAYER+=sec_size+(sec_size>0?0x1:0)+0x5;

			if(0==strcmp(sec_name,"__LayerInfoStorage"))
				break;

		}
		LAYER+=0x5;

		while(1)
		{
			LAYER+=0x5;

			LAYER+=0x1E7+0x1;
			fseek(f,LAYER,SEEK_SET);
			int comm_size=0;
			fread(&comm_size,4,1,f);
			LAYER+=0x5;
			if(comm_size>0)
			{
				LAYER+=comm_size+0x1;
			}
			fseek(f,LAYER,SEEK_SET);
			int ntmp;
			fread(&ntmp,4,1,f);
			if(ntmp!=0x1E7)
				break;
		}

		LAYER+=0x5*0x5+0x1ED*0x12;
		fseek(f,LAYER,SEEK_SET);
		fread(&sec_size,4,1,f);
		if(sec_size==0)
			break;	
	}
	POS = LAYER+0x5;

	fseek(f,POS,SEEK_SET);
}
