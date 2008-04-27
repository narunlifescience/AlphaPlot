#include "Notes.h"

#include <QTextDocument>
#include <QTextEdit>
#include <QIcon>
#include "lib/ActionManager.h"

struct Notes::Private {
	QTextDocument model;
};

Notes::Notes(const QString &name)
	: AbstractPart(name), d(new Private)
{
}

Notes::Notes()
	: AbstractPart("temp"), d(new Private)
{
	// TODO
	//	createActions();
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

/* ========================= static methods ======================= */
ActionManager * Notes::action_manager = 0;

ActionManager * Notes::actionManager()
{
	if (!action_manager)
		initActionManager();
	
	return action_manager;
}

void Notes::initActionManager()
{
	if (!action_manager)
		action_manager = new ActionManager();

	action_manager->setTitle(tr("Notes"));
	volatile Notes * action_creator = new Notes(); // initialize the action texts
	delete action_creator;
}

