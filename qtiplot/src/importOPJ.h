#ifndef IMPORTOPJ_H
#define IMPORTOPJ_H

#include <qobject.h>

class ApplicationWindow;
class OPJFile;

class ImportOPJ : public QObject
{
public:
	ImportOPJ(ApplicationWindow *mw, const QString& filename);
	~ImportOPJ(){};

	bool importTables(OPJFile opj);
	int error(){return parse_error;};

private:
	int parse_error;
};

#endif //IMPORTOPJ_H
