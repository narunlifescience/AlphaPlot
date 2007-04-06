// OPJFile.h
/*
Origin 7.5 column value display
Numeric, Text&Numeric: 
	Decimal:1000            = 0
	Scientific:1E3          = 1
	Engeneering:1k          = 2
	Decimal:1,000           = 3

Time:
	hh:mm                   = 0
	hh                      = 1
	hh:mm:ss                = 2
	hh:mm:ss.zz             = 3
	hh ap                   = 4
	hh:mm ap                = 5
	mm:ss                   = 6
	mm:ss.zz                = 7
	hhmm                    = 8
	hhmmss                  = 9
	hh:mm:ss.zzz            = 10

Date:
	dd.MM.yyyy              = 0
	y.                      = 1  (year abbreviation - for instance, 'ã.' in russian)
	MMM d                   = 2
	M/d                     = 3
	d                       = 4
	ddd                     = 5
	F                       = 6	 (first letter of day of week) 
	yyyy                    = 7
	yy                      = 8
	dd.MM.yyyy hh:mm        = 9
	dd.MM.yyyy hh:mm:ss     = 10
	yyMMdd                  = 11
	yyMMdd hh:mm            = 12
	yyMMdd hh:mm:ss         = 13
	yyMMdd hhmm             = 14
	yyMMdd hhmmss           = 15
	MMM                     = 16
	J                       = 17 (first letter of month)
	Q1                      = 18 (quartal)
	M-d-yyyy (Custom1)      = 19
	hh:mm:ss.zzzz (Custom2) = 20

Month:
	MMM                     = 0
	MMMM                    = 1
	J                       = 2	 (first letter of month)

Day of Week:
	ddd                     = 0
	dddd                    = 1
	F                       = 2	 (first letter of day of week)
*/
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

struct spreadColumn {
	string name;
	string type;
	int value_type;//Numeric = 0, Text = 1, Date = 2, Time = 3, Month = 4, Day = 5, Text&Numeric = 6 
	int value_type_specification; //see above
	int significant_digits;
	int decimal_places;
	string command;
	string comment;
	int width;
	vector <double> data;
	vector <string> sdata;
	spreadColumn(string _name="")
	:	name(_name)
	,	value_type(0)
	,	value_type_specification(0)
	,	significant_digits(6)
	,	decimal_places(6)
	,	width(8)
	{};
};

struct spreadSheet {
	string name;
	string label;
	int maxRows;
	bool bHidden;
	bool bLoose;
	vector <spreadColumn> column;
	spreadSheet(string _name=""):name(_name),bHidden(false),bLoose(true){};
};

struct matrix {
	string name;
	string label;
	int nr_rows;
	int nr_cols;
	vector <double> data;
};

class OPJFile
{
public:
	OPJFile(const char* filename);
	int Parse();
	double Version() { return version/100.0; }		//!< get version of project file
	int numSpreads() { return SPREADSHEET.size(); }			//!< get number of spreadsheets
	const char *spreadName(int s) { return SPREADSHEET[s].name.c_str(); }	//!< get name of spreadsheet s
	bool spreadHidden(int s) { return SPREADSHEET[s].bHidden; }	//!< is spreadsheet s hidden
	bool spreadLoose(int s) { return SPREADSHEET[s].bLoose; }	//!< is spreadsheet s loose
	const char *spreadLabel(int s) { return SPREADSHEET[s].label.c_str(); }	//!< get label of spreadsheet s
	int numCols(int s) { return SPREADSHEET[s].column.size(); }		//!< get number of columns of spreadsheet s
	int numRows(int s,int c) { return SPREADSHEET[s].column[c].value_type==1 ? SPREADSHEET[s].column[c].sdata.size() : SPREADSHEET[s].column[c].data.size(); }	//!< get number of rows of column c of spreadsheet s
	int maxRows(int s) { return SPREADSHEET[s].maxRows; }		//!< get maximum number of rows of spreadsheet s
	//column properties
	const char *colName(int s, int c) { printf("N"); return SPREADSHEET[s].column[c].name.c_str(); }	//!< get name of column c of spreadsheet s
	const char *colType(int s, int c) { printf("T"); return SPREADSHEET[s].column[c].type.c_str(); }	//!< get type of column c of spreadsheet s
	const char *colCommand(int s, int c) { printf("C"); return SPREADSHEET[s].column[c].command.c_str(); }	//!< get command of column c of spreadsheet s
	const char *colComment(int s, int c) { printf("C"); return SPREADSHEET[s].column[c].comment.c_str(); }	//!< get comment of column c of spreadsheet s
	int colValueType(int s, int c) { return SPREADSHEET[s].column[c].value_type; }
	int colValueTypeSpec(int s, int c) { return SPREADSHEET[s].column[c].value_type_specification; }
	int colSignificantDigits(int s, int c) { return SPREADSHEET[s].column[c].significant_digits; }
	int colDecPlaces(int s, int c) { return SPREADSHEET[s].column[c].decimal_places; }
	int colWidth(int s, int c) { return SPREADSHEET[s].column[c].width; }
	vector <double> Data(int s, int c) { return SPREADSHEET[s].column[c].data; }	//!< get data of column c of spreadsheet s
	const char* SData(int s, int c, int r) { 
		return SPREADSHEET[s].column[c].sdata[r].c_str();
	}	//!< get data strings of column c/row r of spreadsheet s
private:
	bool IsBigEndian() { return( htonl(1)==1 ); }
	void ByteSwap(unsigned char * b, int n);
	int  compareSpreadnames(char *sname);				//!< returns matching spread index
	int  compareColumnnames(int spread, char *sname);	//!< returns matching column index
	void readSpreadInfo(FILE *fopj, FILE *fdebug);
	void skipObjectInfo(FILE *fopj, FILE *fdebug);
	void setColName(int spread);		//!< set default column name starting from spreadsheet spread
	const char* filename;			//!< project file name
	int version;				//!< project version
	vector <spreadSheet> SPREADSHEET;
};

#endif // OPJFILE_H
