#ifndef IMPORTOPJ_H
#define IMPORTOPJ_H

#include <qstring.h>

#include "application.h"
#include "../3rdparty/liborigin/OPJFile.h"

class ImportOPJ
{
public:
	ImportOPJ(ApplicationWindow *mw, const QString& filename);
	bool importTables(OPJFile opj);
	int error(){return parse_error;};

private:
	int parse_error;
	ApplicationWindow *mw;
	QString filename;
};

#endif //IMPORTOPJ_H
