#ifndef WIDGET_H
#define WIDGET_H

#include <qwidget.h>

class myWidget: public QWidget
{
	Q_OBJECT

public:	
	
	myWidget(const QString& label, QWidget * parent, const char * name, WFlags f);
	~myWidget(){};

	enum CaptionPolicy{Name = 0, Label = 1, Both = 2};

	QString windowLabel(){return QString(w_label);};
	void setWindowLabel(const QString& s){w_label = s;};

	CaptionPolicy captionPolicy(){return caption_policy;};
	void setCaptionPolicy(CaptionPolicy policy);

	QString birthDate(){return birthdate;};
	void setBirthDate(const QString& s){birthdate = s;};

	virtual QString saveAsTemplate(const QString& ){return QString::null;};
	virtual void restore(const QStringList& ){};

	virtual void print(){};
	virtual QString saveToString(const QString &info){return QString::null;};

	//event handlers
	void resizeEvent(QResizeEvent *){emit resizedWindow(this);};
	void closeEvent( QCloseEvent *);
	void askOnCloseEvent(bool ask){askOnClose = ask;};

signals:  
	void closedWindow(QWidget *);
	void hiddenWindow(QWidget *);
	void modifiedWindow(QWidget *);
	void resizedWindow(QWidget *);

private:
	QString w_label, birthdate;
	CaptionPolicy caption_policy;
	bool askOnClose;
};

#endif
