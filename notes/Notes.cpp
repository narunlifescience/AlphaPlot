#include "Notes.h"

#include <QTextDocument>
#include <QTextEdit>
#include <QIcon>

struct Notes::Private {
	QTextDocument model;
};

Notes::Notes(const QString &name)
	: AbstractPart(name), d(new Private)
{
}

Notes::~Notes()
{
	delete d;
}

QWidget *Notes::view()
{
	QTextEdit *editor = new QTextEdit();
	editor->setDocument(&d->model);
	return editor;
}

QIcon Notes::icon() const
{
	return QPixmap(":/note.xpm");
}
