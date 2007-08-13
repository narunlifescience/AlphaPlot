#include "Notes.h"

#include <QTextDocument>
#include <QTextEdit>
#include <QIcon>

Notes::Notes(const QString &name)
	: d_model(new QTextDocument()), AbstractAspect(name)
{
}

QWidget *Notes::view(QWidget *parent_widget)
{
	QTextEdit *editor = new QTextEdit(parent_widget);
	editor->setDocument(d_model);
	return editor;
}

QIcon Notes::icon() const
{
	return QPixmap(":/note.xpm");
}
