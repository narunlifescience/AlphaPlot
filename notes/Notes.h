#ifndef NOTES_H
#define NOTES_H

#include "AbstractPart.h"

class Notes : public AbstractPart
{
	Q_OBJECT

	public:
		Notes(const QString &name);
		~Notes();
		QWidget *view();
		QIcon icon() const;

	private:
		struct Private;
		Private *d;
};

#endif // ifndef NOTES_H
