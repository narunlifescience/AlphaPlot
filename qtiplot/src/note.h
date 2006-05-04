#ifndef NOTE_H
#define NOTE_H

#include "widget.h"	
#include <qtextedit.h>

class Note: public myWidget
{
    Q_OBJECT

public:

	Note(const QString& label, QWidget* parent=0, const char* name=0, WFlags f=0);
	~Note(){};

	QTextEdit *te;

	virtual WidgetType rtti(){return NoteWidget;};
		
	void init();

	//event handlers
	void closeEvent( QCloseEvent *e );
	void resizeEvent(QResizeEvent *);
	void contextMenuEvent(QContextMenuEvent *e);

public slots:
	QString saveToString(const QString &info);

	QTextEdit* textWidget(){return te;};
	QString text(){return te->text();};
	void setText(const QString &s){te->setText(s);};
	void modifiedNote();
	void print();

signals:
	void modifiedNote(QWidget *);
	void closedNote(QWidget*);
	void hiddenNote(QWidget*);
	void resizedNote(QWidget*);
	void showContextMenu();
};
   
#endif
