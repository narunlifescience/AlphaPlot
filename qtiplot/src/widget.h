#ifndef WIDGET_H
#define WIDGET_H

#include <qwidget.h>

class myWidget: public QWidget
{
public:	
	
	myWidget(const QString& label, QWidget * parent, const char * name, WFlags f);
	~myWidget(){};

	enum CaptionPolicy{Name = 0, Label = 1, Both = 2};
	enum WidgetType{None, TableWidget, MatrixWidget, Plot2D, Plot3D, NoteWidget};

	QString windowLabel(){return QString(w_label);};
	void setWindowLabel(const QString& s){w_label = s;};

	CaptionPolicy captionPolicy(){return caption_policy;};
	void setCaptionPolicy(CaptionPolicy policy);

	QString birthDate(){return birthdate;};
	void setBirthDate(const QString& s){birthdate = s;};

	virtual QString saveAsTemplate(const QString& ){return QString::null;};
	virtual void restore(const QStringList& ){};

	virtual WidgetType rtti(){return None;};

	void askOnCloseEvent(bool ask){askOnClose = ask;};
	bool confirmClose(){return askOnClose;};

	virtual void print(){};

private:
	QString w_label, birthdate;
	CaptionPolicy caption_policy;
	bool askOnClose;
};

#endif
