#include "Notes.h"

#include <QTextDocument>
#include <QTextEdit>
#include <QIcon>
#include "lib/ActionManager.h"
#include "lib/XmlStreamReader.h"

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

QWidget *Notes::view() const
{
	QTextEdit *editor = new QTextEdit();
	editor->setDocument(&d->model);
	return editor;
}

QIcon Notes::icon() const
{
	return QPixmap(":/note.xpm");
}

QString Notes::text() const
{
	return d->model.toPlainText();
}

void Notes::setText(const QString & new_text)
{
	// TODO: use commands
	d->model.setPlainText(new_text);
}

void Notes::save(QXmlStreamWriter * writer) const
{
	writer->writeStartElement("notes");
	writeBasicAttributes(writer);
	writeCommentElement(writer);
	writer->writeStartElement("text");
	QString contents = text();
	contents.replace(QString("]]>"), QString("] ]>")); // just in case ...
	writer->writeCDATA(contents);
	writer->writeEndElement(); // "text"
	writer->writeEndElement(); // "notes"
}

bool Notes::load(XmlStreamReader * reader)
{
	if(reader->isStartElement() && reader->name() == "notes") 
	{
		if (!readBasicAttributes(reader)) return false;

		// read child elements
		while (!reader->atEnd()) 
		{
			reader->readNext();

			if (reader->isEndElement()) break;

			if (reader->isStartElement()) 
			{
				if (reader->name() == "comment")
				{
					if (!readCommentElement(reader)) return false;
				}
				else if(reader->name() == "text")
				{
					setText(reader->readElementText());
				}
				else // unknown element
				{
					reader->raiseWarning(tr("unknown element '%1'").arg(reader->name().toString()));
					if (!reader->skipToEndElement()) return false;
				}
			} 
		}
	}
	else // no notes element
		reader->raiseError(tr("no notes element found"));

	return !reader->hasError();
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

