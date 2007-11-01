#include "Folder.h"

#include <QIcon>

QIcon Folder::icon() const
{
	QIcon result;
	result.addFile(":/folder_closed.xpm", QSize(), QIcon::Normal, QIcon::Off);
	result.addFile(":/folder_open.xpm", QSize(), QIcon::Normal, QIcon::On);
	return result;
}

