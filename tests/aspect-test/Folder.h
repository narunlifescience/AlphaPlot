#ifndef FOLDER_H
#define FOLDER_H

#include "AbstractAspect.h"

class Folder : public QObject, public AbstractAspect
{
	Q_OBJECT

	public:
		Folder(const QString &name)
			: AbstractAspect(name) {}

		//! Currently, Folder does not have a default view (returns 0).
		virtual QWidget *view(QWidget *parent_widget = 0) {
			Q_UNUSED(parent_widget);
			return 0;
		}

		//! See QMetaObject::className().
		virtual const char* className() const { return metaObject()->className(); }
		//! See QObject::inherits().
		virtual bool inherits(const char *class_name) const { return QObject::inherits(class_name); }

		virtual QIcon icon() const;
};

#endif // ifndef FOLDER_H
