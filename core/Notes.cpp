#include "Notes.h"
#include "AspectView.h"

#include <QTextDocument>
#include <QTextEdit>
#include <QIcon>

Notes::Notes(const QString &name)
	: AbstractAspect(name), d_model(new QTextDocument())
{
}

AspectView *Notes::view(QWidget *parent_widget)
{
	QTextEdit *editor = new QTextEdit(parent_widget);
	editor->setDocument(d_model);
	AspectView *aspect_view = new AspectView(this);
	aspect_view->setWidget(editor);
	return aspect_view;
}

QIcon Notes::icon() const
{
	return QPixmap(":/note.xpm");
}
