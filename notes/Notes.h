#ifndef NOTES_H
#define NOTES_H

#include "AbstractPart.h"

class QTextDocument;

class Notes : public AbstractPart
{
	Q_OBJECT

	public:
		Notes(const QString &name);
		QWidget *view();
		QIcon icon() const;

	private:
		QTextDocument *d_model;
};

#endif // ifndef NOTES_H
