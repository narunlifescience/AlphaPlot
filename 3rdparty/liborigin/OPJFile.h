// OPJFile.h

#ifndef OPJFILE_H
#define OPJFILE_H

// for htonl
#if defined (_MSC_VER) || defined (_WIN32)
#include <windows.h>
#else
#include <sys/types.h>
#include <netinet/in.h>
#include <inttypes.h>
#endif

#include <string>
#include <vector>

using namespace std;

// for string entries
struct Entry {
	short spread;
	short column;
	short row;
	char *name;
};

typedef Entry Entry;

class OPJFile
{
public:
	OPJFile(const char* filename);
	int Parse();
	double Version() { return version/100.0; }		//!< get version of project file
	int numSpreads() { return nr_spreads; }			//!< get number of spreadsheets
	const char *spreadName(int s) { return spreadname[s].c_str(); }	//!< get name of spreadsheet s
	int numCols(int s) { return nr_cols[s]; }		//!< get number of columns of spreadsheet s
	int numRows(int s,int c) { return nr_rows[s][c]; }	//!< get number of rows of column c of spreadsheet s
	int maxRows(int s) { return maxrows[s]; }		//!< get maximum number of rows of spreadsheet s
	const char *colName(int s, int c) { printf("N"); return colname[s][c].c_str(); }	//!< get name of column c of spreadsheet s
	const char *colType(int s, int c) { printf("T"); return coltype[s][c].c_str(); }	//!< get type of column c of spreadsheet s
	double* Data(int s, int c) { return data[s][c]; }	//!< get data of column c of spreadsheet s
	char* SData(int s, int c, int r) { 
		for (unsigned int i=0;i<sdata.size();i++) {
			if(sdata[i].spread != s) continue;
			if(sdata[i].column != c) continue;
			if(sdata[i].row != r) continue;
			return sdata[i].name;
		}					
		return 0;
	}	//!< get data strings of column c/row r of spreadsheet s
private:
	bool IsBigEndian() { return( htonl(1)==1 ); }
	void ByteSwap(unsigned char * b, int n);
	int compareSpreadnames(char *sname);	//!< returns matching spread index
	void setColName(int spread);		//!< set default column name starting from spreadsheet spread
	const char* filename;			//!< project file name
	int version;				//!< project version
	int nr_spreads;				//!< number of spreadsheets
	vector <string> spreadname;		//!< spreadsheet names
	vector <int> nr_cols;			//!< number of cols per spreadsheet
	vector < vector <int> > nr_rows;	//!< number of rows per column of spreadsheet
	vector <int> maxrows;			//!< max number of rows of spreadsheet
	vector < vector <double *> > data;	//!< data per column and spreadsheet
	int entry;
	vector <Entry> sdata;			//!< label entries
	vector < vector <string> > colname;	//!< column names
	vector < vector <string> > coltype;	//!< column types
};

#endif // OPJFILE_H
