// OPJFile.cc

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "OPJFile.h"

#define MAX_LEVEL 20

OPJFile::OPJFile(char *filename) 
	: filename(filename) 
{
	version=0;
	nr_spreads=0;
	for(int i=0;i<MAX_SPREADS;i++) {
		spreadname[i] = new char[25];
		spreadname[i][0]=0;
		nr_cols[i]=0;
		maxrows[i]=0;
		for(int j=0;j<MAX_COLUMNS;j++) {
			nr_rows[i][j]=0;
			colname[i][j] = new char[25];
			colname[i][j][0]=0x41+j;
			colname[i][j][1]=0;
			coltype[i][j] = new char[25];
			if(j==0)
				coltype[i][j][0]='X';
			else
				coltype[i][j][0]='Y';
			coltype[i][j][1]=0;
		}
	}
}

/* File Structure :
filepre +
	+ pre + head + data	col A
	+ pre + head + data	col B
*/

/* parse file filename complete and save values */
int OPJFile::Parse() {
	FILE *f, *debug;
	if((f=fopen(filename,"r")) == NULL ) {
		printf("Could not open %s",filename);
		return -1;
	}
	
	if((debug=fopen("opjfile.log","w")) == NULL ) {
		printf("Could not open log file!\n");
		return -1;
	}

////////////////////////////// check version from header ///////////////////////////////
	char header[14];
	header[13]=0;
	fread(&header,13,1,f);
	fprintf(debug,"	[header : %s]\n",header);
	char vers[5];
	vers[4]=0;

	// get version
	fseek(f,0x7,SEEK_SET);
	fread(&vers,4,1,f);
	version = atoi(vers);
	fprintf(debug,"	[version = %d]\n",version);
	
	int FILEPRE=0x3e;	// file header
	int PRE=0x62;		// pre column 
	int HEAD;		// column header
	int NEW_COL;		// value for new column
	int COL_SIZE;		// value for col size
	// TODO : valuesize depends also on column type!
	int valuesize=10;
	if(version == 130) {	// 4.1
		version=410;
		FILEPRE=0x1D;
		HEAD=0x20;
		NEW_COL=0x72;
		COL_SIZE=0x1b;
		valuesize=8;
	}
	else if(version == 210) {	// 5.0
		version=500;
		FILEPRE=0x25;
		HEAD=0x22;
		NEW_COL=0x72;
		COL_SIZE=0x1b;
		valuesize=11;
	}
	else if(version == 2625) {	// 6.0
		version=600;
		FILEPRE=0x2f;
		HEAD=0x22;
		NEW_COL=0x72;
		COL_SIZE=0x1b;
		valuesize=11;
	} 
	else if(version == 2630 ) {	// 6.0 SR4
		version=604;
		FILEPRE=0x2f;
		HEAD=0x22;
		NEW_COL=0x72;
		COL_SIZE=0x1b;
	}
	else if(version == 2635 ) {	// 6.1
		version=610;
		FILEPRE=0x3a;
		HEAD=0x22;
		NEW_COL=0x72;
		COL_SIZE=0x1b;
	}
	else if(version == 2656) {	// 7.0
		version=700;
		HEAD=0x23;
		NEW_COL=0x73;
		COL_SIZE=0x1c;
	}
	else if(version == 2769) {	// 7.5
		version=750;
		HEAD=0x33;
		NEW_COL=0x83;
		COL_SIZE=0x2c;
	}
	else {
		fprintf(debug,"Found unknown project version %d\n",version);
		fprintf(debug,"Please contact the author of opj2dat\n");
		return -2;
	}
	fprintf(debug,"Found project version %.2f\n",version/100.0);
	
/////////////////// find column ///////////////////////////////////////////////////////////7
	fseek(f,FILEPRE + 0x05,SEEK_SET);
	int col_found;
	fread(&col_found,4,1,f);
	fprintf(debug,"	[column found = 0x%X (0x%X : YES) @ 0x%X]\n",col_found,NEW_COL,FILEPRE + 0x05);

	int current_col=1, nr=0, POS=FILEPRE, DATA=0;
	double a;
	char name[25];
	while(col_found == NEW_COL) {
//////////////////////////////// COLUMN HEADER /////////////////////////////////////////////
		// TODO : data isn't the same for all spreads !
		fprintf(debug,"	[column header @ 0x%X]\n",POS);
		fflush(debug);
		fseek(f,POS + PRE,SEEK_SET);
		fread(&name,25,1,f);
		char* sname = strtok(name,"_");	// spreadsheet name
		char* cname = strtok(NULL,"_");	// column name
		if(nr_spreads == 0 || strcmp(sname,spreadname[nr_spreads-1])) {
			fprintf(debug,"		NEW SPREADSHEET\n");
			sprintf(spreadname[nr_spreads++],"%s",sname);
			current_col=1;
			maxrows[nr_spreads]=0;
		}
		else {
			current_col++;
			nr_cols[nr_spreads-1]=current_col;
		}
		fprintf(debug,"		SPREADSHEET = %s COLUMN NAME = %s (%d) (@0x%X)\n",
			sname, cname,current_col,POS+PRE);
				
		sprintf(colname[nr_spreads-1][current_col-1],"%s",cname);
		
////////////////////////////// SIZE of column /////////////////////////////////////////////
		fseek(f,POS+PRE+COL_SIZE,SEEK_SET);
		fread(&nr,4,1,f);
		nr /= valuesize;
		fprintf(debug,"	[number of rows = %d @ 0x%X]\n",nr,POS+PRE+COL_SIZE);
		fflush(debug);
		nr_rows[nr_spreads-1][current_col-1]=nr;
		maxrows[nr_spreads-1]<nr?maxrows[nr_spreads-1]=nr:0;

////////////////////////////////////// DATA ////////////////////////////////////////////////
		fseek(f,POS+PRE+HEAD,SEEK_SET);
		fprintf(debug,"	[data @ 0x%X]\n",POS+PRE+HEAD);
		data[nr_spreads-1][current_col-1] = (double *) malloc(nr*sizeof(double));
		for (int i=0;i<nr;i++) {
			fread(&a,valuesize,1,f);
			//if(fabs(a)<1.0e-100) a=0;
			fprintf(debug,"%g ",a);
			data[nr_spreads-1][(current_col-1)][i]=a;
		}
		fprintf(debug,"\n");
		fflush(debug);

		DATA = valuesize*nr - 1;
		if(version== 410)
			POS += 2;
		int pos = POS + PRE + DATA + HEAD + 0x05;
		fseek(f,pos,SEEK_SET);
		fread(&col_found,4,1,f);
		fprintf(debug,"	[column found = 0x%X (0x%X : YES) (@ 0x%X)]\n",col_found,NEW_COL,pos);
		fflush(debug);
		
		POS += (DATA + HEAD + PRE);
	}

	POS-=1;
	fprintf(debug,"\n[position @ 0x%X]\n",POS);
	fprintf(debug,"		nr_spreads = %d\n",nr_spreads);
	fflush(debug);
///////////////////// SPREADSHEET INFOS ////////////////////////////////////
	int LAYER;
	int COL_JUMP = 0x1ED;
	for(int i=0; i < nr_spreads; i++) {
		if(i > 0) {
			if(version == 750)
				POS = LAYER+0x2759;
			else if (version == 700 ) 
				POS += 0x2530 + nr_cols[i-1]*COL_JUMP;
			else if (version == 610 ) 
				POS += 0x25A4 + nr_cols[i-1]*COL_JUMP;
			else if (version == 604 ) 
				POS += 0x25A0 + nr_cols[i-1]*COL_JUMP;
			else if (version == 600 ) 
				POS += 0x2560 + nr_cols[i-1]*COL_JUMP;
			else if (version == 500 ) 
				POS += 0x92C + nr_cols[i-1]*COL_JUMP;
			else if (version == 410 ) 
				POS += 0x7FB + nr_cols[i-1]*COL_JUMP;
		}
		fprintf(debug,"\n");
		
		// HEADER
		// check header
		int ORIGIN = 0x55;
		if(version == 500)
			ORIGIN = 0x58;
		fseek(f,POS + ORIGIN,SEEK_SET);	// check for 'O'RIGIN
		char c;
		fread(&c,1,1,f);
		int jump=0;
		while( c != 'O' && jump < MAX_LEVEL) {	// no inf loop
			fprintf(debug,"	TRY %d	\"O\"RIGIN not found ! : %c (@ 0x%X)",jump+1,c,POS+ORIGIN);
			fprintf(debug,"		POS=0x%X | ORIGIN = 0x%X\n",POS,ORIGIN);
			POS+=0x1F2;
			fseek(f,POS + ORIGIN,SEEK_SET);
			fread(&c,1,1,f);
			jump++;
		}
		
		if(jump == MAX_LEVEL){
			fprintf(debug,"	Spreadsheet SECTION not found ! 	(@ 0x%X)\n",POS-10*0x1F2+0x55);
			return -5;
		}
		
		fprintf(debug,"	OK. Spreadsheet SECTION found	(@ 0x%X)\n",POS);
		fflush(debug);
	
		// check spreadsheet name
		fseek(f,POS + 0x12,SEEK_SET);
		fread(&name,25,1,f);

		int spread=i, j;
		for(j=0;j<nr_spreads;j++) {	// get index of spreadsheet
			if(strncmp(name,spreadname[j],strlen(spreadname[j])) == 0)
				spread=j;
		}
		
		fprintf(debug,"		SPREADSHEET %d NAME : %s	(@ 0x%X) has %d columns\n",
			spread+1,name,POS + 0x12,nr_cols[spread]);
	
		int ATYPE;
		LAYER = POS;
		if(version == 750) {
			// LAYER section
			LAYER += 0x4AB;
		 	ATYPE = 0xCF;
			COL_JUMP = 0x1F2;
			// seek for "L"ayerInfoStorage to find layer section
			fseek(f,LAYER+0x53, SEEK_SET);
			fread(&c,1,1,f);
			while( c != 'L' && jump < MAX_LEVEL) {	// no inf loop; number of "set column value"
				LAYER += 0x99;
				fseek(f,LAYER+0x53, SEEK_SET);
				fread(&c,1,1,f);
				jump++;
			} 
	
			if(jump == MAX_LEVEL) {
				fprintf(debug,"		LAYER %d SECTION not found !\nGiving up.",i+1);
				return -3;
			}
	
			fprintf(debug,"		[LAYER %d @ 0x%X]\n",i+1,LAYER);
		}
		else if (version == 700)
			ATYPE = 0x2E4;
		else if (version == 610)
			ATYPE = 0x358;
		else if (version == 604)
			ATYPE = 0x354;
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
		for (j=0;j<nr_cols[spread];j++) {
			fseek(f,LAYER+ATYPE+j*COL_JUMP, SEEK_SET);
			fread(&name,25,1,f);
			if(strncmp(name,colname[spread][j],strlen(colname[spread][j])) == 0) {
				fseek(f,LAYER+ATYPE+j*COL_JUMP-1, SEEK_SET);
				fread(&c,1,1,f);
				char type[7];
				switch(c) {
				case 3: sprintf(type,"X");break;
				case 0: sprintf(type,"Y");break;
				case 5: sprintf(type,"Z");break;
				case 6: sprintf(type,"DX");break;
				case 2: sprintf(type,"DY");break;
				case 4: sprintf(type,"LABEL");break;
				default: sprintf(type,"UNKNOWN");break;
				}
				sprintf(coltype[spread][j],"%s",type);
				fprintf(debug,"		COLUMN %s type = %s (@ 0x%X)\n",colname[spread][j],type,LAYER+ATYPE+j*COL_JUMP);
			}
			else {
				fprintf(debug,"		COLUMN %d (%c) ? (@ 0x%X)\n",j+1,c,LAYER+ATYPE+j*COL_JUMP);
			}
		}
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
	fclose(debug);

	return 0;
}
