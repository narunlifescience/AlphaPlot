// OPJFile.h

#ifndef OPJFILE_H
#define OPJFILE_H

#if defined(_MSC_VER) //MSVC Compiler
#define MAX_SPREADS 123
#else
#define MAX_SPREADS 255
#endif

#define MAX_COLUMNS 255

class OPJFile
{
public:
	OPJFile(char* filename);
	int Parse();
	double Version() { return version/100.0; }		//!< get version of project file
	int numSpreads() { return nr_spreads; }			//!< get number of spreadsheets
	char *spreadName(int s) { return spreadname[s]; }	//!< get name of spreadsheet s
	int numCols(int s) { return nr_cols[s]; }		//!< get number of columns of spreadsheet s
	int numRows(int s,int c) { return nr_rows[s][c]; }	//!< get number of rows of column c of spreadsheet s
	int maxRows(int s) { return maxrows[s]; }		//!< get maximum number of rows of spreadsheet s
	char *colName(int s, int c) { return colname[s][c]; }	//!< get name of column c of spreadsheet s
	char *colType(int s, int c) { return coltype[s][c]; }	//!< get type of column c of spreadsheet s
	double* Data(int s,int c) { return data[s][c]; }	//!< get data of column c of spreadsheet s
private:
	char* filename;				//!< project file name
	int version;				//!< project version
	int nr_spreads;				//!< number of spreadsheets
	char *spreadname[MAX_SPREADS];		//!< spreadsheet names
	int nr_cols[MAX_SPREADS];		//!< number of cols per spreadsheet
	int nr_rows[MAX_SPREADS][MAX_COLUMNS];	//!< number of rows per column of spreadsheet
	int maxrows[MAX_SPREADS];		//!< max number of rows of spreadsheet
	double *data[MAX_SPREADS][MAX_COLUMNS];	//!< data per column per spreadsheet
	char *colname[MAX_SPREADS][MAX_COLUMNS];	//!< column names
	char *coltype[MAX_SPREADS][MAX_COLUMNS];	//!< column types

};

#endif // OPJFILE_H
