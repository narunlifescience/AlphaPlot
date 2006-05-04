#ifndef SYMBOLDIALOG_H
#define SYMBOLDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;

class symbolDialog : public QDialog
{
    Q_OBJECT

public:
	enum CharSet{minGreek = 0, majGreek = 1};

    symbolDialog(CharSet charsSet, QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~symbolDialog();

	void initMinGreekChars();
	void initMajGreekChars();

    QButtonGroup *GroupBox1;

public slots:
    virtual void languageChange();
	void getChar(int btnIndex);
	void addCurrentChar();

signals:
	void addLetter(const QString&);
};

#endif // exportDialog_H
