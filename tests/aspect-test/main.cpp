#include "Project.h"
#include "Notes.h"

#include <QApplication>
#include <QWidget>
#include <QtDebug>

int main(int argc, char **argv)
{
	QApplication app(argc, argv);

	shared_ptr<Project> p(new Project());
	p->view()->show();
	shared_ptr<Folder> f1(new Folder("test"));
	p->addChild(f1);
	f1->setComment("Hello World");
	shared_ptr<Folder> f2(new Folder("abba"));
	f1->setComment("Hello People");
	p->addChild(f2);
	shared_ptr<Notes> notes(new Notes("something"));
	p->addChild(notes);
	notes->setCaptionSpec("%n (%t)");
	shared_ptr<Notes> notes2(new Notes("something more"));
	f2->addChild(notes2);
	notes->setComment("A demo note window");

	for (int i=0; i<5; i++)
		f1->addChild(shared_ptr<Folder>(new Folder(QString::number(i))));

	app.exec();
}
