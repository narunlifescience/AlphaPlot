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
	nr_spreads=0;
	entry=0;
}

int OPJFile::compareSpreadnames(char *sname) {
	for(int i=0;i<nr_spreads;i++)
		if (spreadname[i] == sname)
			return i;
	return -1;
}

// set default name for columns starting from spreadsheet spread
void OPJFile::setColName(int spread) {
	for(int j=spread;j<nr_spreads;j++) {
		coltype[j].resize(nr_cols[j]);
		coltype[j][0]="X";	
		for (int k=1;k<nr_cols[j];k++)
			coltype[j][k]="Y";	
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
	if((f=fopen(filename,"r")) == NULL ) {
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
		char* sname = new char[26];
		sprintf(sname,"%s",strtok(name,"_"));	// spreadsheet name
		char* cname = strtok(NULL,"_");	// column name
		while(char* tmpstr = strtok(NULL,"_")) {	// get multiple-"_" title correct
			strcat(sname,"_");
			strcat(sname,cname);
			strcpy(cname,tmpstr);
		}
		if(nr_spreads == 0 || compareSpreadnames(sname) == -1) {
			fprintf(debug,"NEW SPREADSHEET\n");
			spreadname.resize(nr_spreads+1);
			spreadname[nr_spreads++] = sname;
			current_col=1;
			maxrows.resize(nr_spreads+1);
			maxrows[nr_spreads]=0;
		}
		else {
			current_col++;
			nr_cols.resize(nr_spreads);
			nr_cols[nr_spreads-1]=current_col;
		}
		fprintf(debug,"SPREADSHEET = %s COLUMN NAME = %s (%d) (@0x%X)\n",
			sname, cname,current_col,(unsigned int) ftell(f));
		fflush(debug);

		if(cname == 0) {
			fprintf(debug,"NO COLUMN NAME FOUND! Must be a matrix or function.\n");
////////////////////////////// READ MATRIX or FUNCTION ////////////////////////////////////
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
			nr_cols.resize(nr_spreads);
			nr_cols[nr_spreads-1]=size;
			data.resize(nr_spreads);
			data[nr_spreads-1].resize(size);
			nr_rows.resize(nr_spreads);
			nr_rows[nr_spreads-1].resize(size);
			maxrows[nr_spreads-1]=1;
			for(i=0;i<size;i++) {
				data[nr_spreads-1][i] = (double *) malloc(sizeof(double));
				nr_rows[nr_spreads-1][i] = 1;
			}

			double value=0;
			colname.resize(nr_spreads);
			colname[nr_spreads-1].resize(size);
			for(i=0;i<size;i++) {	// read data
				if(i<26)
					colname[nr_spreads-1][i] = i+0x41;
				else if(i<26*26) {
					colname[nr_spreads-1][i] = 0x40+i/26;
					colname[nr_spreads-1][i][1] = i%26+0x41;
				}
				else {
					colname[nr_spreads-1][i] = 0x40+i/26/26;
					colname[nr_spreads-1][i][1] = i/26%26+0x41;
					colname[nr_spreads-1][i][2] = i%26+0x41;
				}
				fread(&value,8,1,f);
				data[nr_spreads-1][i][0] = value;
				fprintf(debug,"%g ",value);
			}
			fprintf(debug,"\n");
			fflush(debug);
			
		}
		else {	// worksheet		
			colname.resize(nr_spreads);
			colname[nr_spreads-1].resize(current_col);
			colname[nr_spreads-1][current_col-1]=cname;

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

			nr_rows.resize(nr_spreads);
			nr_rows[nr_spreads-1].resize(current_col);
			nr_rows[nr_spreads-1][current_col-1]=nr;
			maxrows[nr_spreads-1]<nr?maxrows[nr_spreads-1]=nr:0;

////////////////////////////////////// DATA ////////////////////////////////////////////////
			fread(&c,1,1,f);	// skip '\n'
			if(valuesize != 8 && valuesize <= 16) {	// skip 0 0
				fread(&c,1,1,f);
				fread(&c,1,1,f);
			}
			fprintf(debug,"	[data @ 0x%X]\n",(unsigned int) ftell(f));
			fflush(debug);
			data.resize(nr_spreads);
			data[nr_spreads-1].resize(current_col);
			data[nr_spreads-1][current_col-1] = (double *) malloc(nr*sizeof(double));
			for (i=0;i<nr;i++) {
				if(valuesize <= 16) {	// value
					fread(&a,valuesize,1,f);
					if(IsBigEndian()) SwapBytes(a);
					fprintf(debug,"%g ",a);
					data[nr_spreads-1][(current_col-1)][i]=a;
				}
				else {			// label
					char *stmp = new char[valuesize+1];
					fread(stmp,valuesize,1,f);
					fprintf(debug,"%s ",stmp);
					sdata.resize(entry+1);
					sdata[entry].spread = nr_spreads-1;
					sdata[entry].column = current_col-1;
					sdata[entry].row = i;
					sdata[entry].name = stmp;
					entry++;
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
	fprintf(debug,"	nr_spreads = %d\n",nr_spreads);
	fprintf(debug,"	[position @ 0x%X]\n",POS);
	fflush(debug);

///////////////////// SPREADSHEET INFOS ////////////////////////////////////
	int LAYER=0;
	int COL_JUMP = 0x1ED;
	
	coltype.resize(nr_spreads);
	// do this before any problems occur
	for(i=0; i < nr_spreads; i++)
		coltype[i].resize(nr_cols[i]);

	for(i=0; i < nr_spreads; i++) {
		fprintf(debug,"		reading	Spreadsheet %d/%d properties\n",i+1,nr_spreads);
		fflush(debug);
		if(i > 0) {
			if(version == 750)
				POS = LAYER+0x2759;
			else if (version == 700 ) 
				POS += 0x2530 + nr_cols[i-1]*COL_JUMP;
			else if (version == 610 ) 
				POS += 0x25A4 + nr_cols[i-1]*COL_JUMP;
			else if (version == 604 ) 
				POS += 0x25A0 + nr_cols[i-1]*COL_JUMP;
			else if (version == 601 ) 
				POS += 0x2560 + nr_cols[i-1]*COL_JUMP;	// ?
			else if (version == 600 ) 
				POS += 0x2560 + nr_cols[i-1]*COL_JUMP;
			else if (version == 500 ) 
				POS += 0x92C + nr_cols[i-1]*COL_JUMP;
			else if (version == 410 ) 
				POS += 0x7FB + nr_cols[i-1]*COL_JUMP;
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

		for(j=0;j<nr_spreads;j++) {	// get index of spreadsheet
			if(spreadname[j] == name)
				spread=j;
		}
		
		fprintf(debug,"			SPREADSHEET %d NAME : %s	(@ 0x%X) has %d columns\n",
			spread+1,name,POS + 0x12,nr_cols[spread]);
		fflush(debug);
	
		int ATYPE=0;
		LAYER = POS;
		if(version == 750) {
			// LAYER section
			LAYER += 0x4AB;
		 	ATYPE = 0xCF;
			COL_JUMP = 0x1F2;

			// skip until '\n'
			fseek(f,LAYER, SEEK_SET);
			do{
				fread(&c,1,1,f);
				LAYER++;
			} while (c != '\n');
			LAYER--;
			fprintf(debug,"		[LAYER HEADER @ 0x%X]\n", (unsigned int) ftell(f)-1);
			fflush(debug);

			// check for "L"ayerInfoStorage in header section
			fseek(f,LAYER+0x53, SEEK_SET);
			fread(&c,1,1,f);
			if( c == 'L') {
				fprintf(debug,"			TEST : OK (LayerInfoStorage found @ 0x%X)\n",LAYER+0x53);
			} 
			else{
				fprintf(debug,"ERROR : LAYER %d SECTION not found @ 0x%X\n"ERROR_MSG,i+1,LAYER+0x53);
				setColName(spread);
				return -3;
			}
		}
		else if (version == 700)
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
		fprintf(debug,"			Spreadsheet has %d columns\n",nr_cols[spread]);
		for (j=0;j<nr_cols[spread];j++) {
			fprintf(debug,"			reading	COLUMN %d/%d type\n",j+1,nr_cols[spread]);
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
			//coltype[spread].resize(j+1);
			coltype[spread][j]=type;
			
			fprintf(debug,"				COLUMN \"%s\" type = %s (@ 0x%X)\n",
				colname[spread][j].c_str(),type,LAYER+ATYPE+j*COL_JUMP);
			fflush(debug);

			// check column name
			int max_length=11;	// only first 11 chars are saved here !
			int name_length = colname[spread][j].length();
                        int length = (name_length < max_length) ? name_length : max_length;

			if(colname[spread][j].substr(0,length) == name) {
				fprintf(debug,"				TEST : column name = \"%s\". OK!\n",
					colname[spread][j].c_str());
			}
			else {
				fprintf(debug,"				TEST : COLUMN %d name mismatch (\"%s\" != \"%s\")\n",
					j+1,name,colname[spread][j].c_str());
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
