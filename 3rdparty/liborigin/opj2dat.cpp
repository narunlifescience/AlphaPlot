/***************************************************************************
    File                 : opj2dat.cpp
    --------------------------------------------------------------------
    Copyright            : (C) 2008 Stefan Gerlach
    Email (use @ for *)  : stefan.gerlach*uni-konstanz.de
    Description          : Origin project converter

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
#include <cstdio>
#include <cmath>
#include <cstring>
#include <string.h>

int main(int argc, char *argv[]) {
	if(argc != 2) {
		printf("Usage : ./opj2dat <file.opj>\n");
		return -1;
	}

	printf("opj2dat %s, Copyright (C) 2008 Stefan Gerlach\n",LIBORIGIN_VERSION_STRING);

	if(!strcmp(argv[1],"-v"))
		return 0;

	OriginFile opj(argv[1]);
	int status = opj.parse();
	printf("Parsing status = %d\n",status);
	printf("OPJ PROJECT \"%s\" VERSION = %.2f\n",argv[1],opj.version());

	printf("number of spreadsheets = %d\n",opj.spreadCount());
	printf("number of matrixes     = %d\n",opj.matrixCount());
	printf("number of functions    = %d\n",opj.functionCount());
	printf("number of graphs       = %d\n",opj.graphCount());
	printf("number of notes        = %d\n",opj.noteCount());
	for (unsigned int s=0;s<opj.spreadCount();s++) {
		Origin::SpreadSheet spread = opj.spread(s);
		int columnCount=spread.columns.size();
		printf("Spreadsheet %d :\n", s+1);
		printf(" Name: %s\n",spread.name.c_str());
		printf(" Label: %s\n",spread.label.c_str());
		printf("	Columns: %d\n",columnCount);
		for (int j=0;j<columnCount;j++) {
			Origin::SpreadColumn column = spread.columns[j];
			printf("	Column %d : %s / type : %d, rows : %d\n",
				j+1,column.name.c_str(),column.type,spread.maxRows);
		}
		FILE *out;
		char * filename;
		int ioret;
#ifndef WIN32
		ioret=asprintf(&filename,"%s.%d.dat",argv[1],s+1);
#else
		ioret=asprintf(&filename,"%s.%d.dat",basename(argv[1]),s+1);
#endif
		printf("saved to %s\n",filename);
		if((out=fopen(filename,"w")) == NULL ) {
			printf("Could not open %s",filename);
			return -1;
		}
		// header
		for (int j=0;j<columnCount;j++) {
		    fprintf(out,"%s ",spread.columns[j].name.c_str());
		    printf("%s ",spread.columns[j].name.c_str());
		}
		fprintf(out,"\n");
		printf("\n Data: \n");
		// data
		for (int i=0;i<(int)spread.maxRows;i++) {
			for (int j=0;j<columnCount;j++) {
				if (i<(int)spread.columns[j].data.size()) {
					Origin::variant value=spread.columns[j].data[i];
					if(spread.columns[j].type ==  Origin::SpreadColumn::Label) {
						fprintf(out,"%s ",boost::get<string>(spread.columns[j].data[i]).c_str());
					} else {
					   double v=0.;
					   if (value.type() == typeid(double)) {
							v = boost::get<double>(value);
							if(fabs(v)>2.0e-300) {
							   fprintf(out,"%g ",v);
							}
						}
					   if (value.type() == typeid(string)) {
							fprintf(out,"%s ",boost::get<string>(value).c_str());
						}
					}
				}
			}
		   fprintf(out,"\n");
		}
		fclose(out);
	}
	return 0;
}

