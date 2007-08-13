#ifndef NOTES_H
#define NOTES_H

#include "AbstractAspect.h"

class QTextDocument;

class Notes : public AbstractAspect
{
	Q_OBJECT

	public:
		Notes(const QString &name);
		QWidget *view(QWidget *parent_widget=0);
		QIcon icon() const;

	private:
		QTextDocument *d_model;
};

#endif // ifndef NOTES_H
