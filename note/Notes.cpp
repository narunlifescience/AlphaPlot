#include "Notes.h"

#include <QTextDocument>
#include <QTextEdit>
#include <QIcon>

Notes::Notes(const QString &name)
	: AbstractPart(name), d_model(new QTextDocument())
{
}

QWidget *Notes::view()
{
	QTextEdit *editor = new QTextEdit();
	editor->setDocument(d_model);
	return editor;
}

QIcon Notes::icon() const
{
	return QPixmap(":/note.xpm");
}
