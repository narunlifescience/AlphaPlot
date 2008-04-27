#ifndef NOTES_H
#define NOTES_H

#include "AbstractPart.h"

class ActionManager;

class Notes : public AbstractPart
{
	Q_OBJECT

	public:
		Notes(const QString &name);
		~Notes();
		QWidget *view();
		QIcon icon() const;

	public:
		static ActionManager * actionManager();
		static void initActionManager();
	private:
		static ActionManager * action_manager;
		//! Private ctor for initActionManager() only
		Notes();

	private:
		struct Private;
		Private *d;
};

#endif // ifndef NOTES_H
