#ifndef NOTES_H
#define NOTES_H

#include "AbstractAspect.h"

class QTextDocument;

class Notes : public QObject, public AbstractAspect
{
	Q_OBJECT

	public:
		Notes(const QString &name);
		QWidget *view(QWidget *parent_widget=0);
		QIcon icon() const;

		//! Return the QObject that is responsible for emitting signals
		virtual const QObject *signalEmitter() const { return this; }
		//! Return the QObject that is responsible for receiving signals
		virtual const QObject *signalReceiver() const { return this; }
		//! See QMetaObject::className().
		virtual const char* className() const { return metaObject()->className(); }
		//! See QObject::inherits().
		virtual bool inherits(const char *class_name) const { return QObject::inherits(class_name); }

	private:
		QTextDocument *d_model;
};

#endif // ifndef NOTES_H
