#include "Project.h"
#include "Notes.h"

#include <QApplication>
#include <QWidget>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	Project p;
	p.view()->show();
	Folder f1("test");
	p.addChild(&f1);
	//f1.setComment("Hello World");
	Folder f2("abba");
	p.addChild(&f2);
	f2.setCaptionSpec("%n (%t)");
	Notes notes("something");
	f2.addChild(&notes);
	notes.setComment("A demo note window");

	for (int i=0; i<50; i++)
		f1.addChild(new Folder(QString::number(i)));

	app.exec();
}
