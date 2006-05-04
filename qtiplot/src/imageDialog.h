#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QButtonGroup;
class QPushButton;
class QSpinBox;

class imageDialog : public QDialog
{
    Q_OBJECT

public:
    imageDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~imageDialog();
	void setOrigin(const QPoint& o);
	void setSize(const QSize& size);

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonApply;
    QButtonGroup* GroupBox1, *GroupBox2, *GroupBox3, *GroupBox4;
    QSpinBox* boxX, *boxY, *boxWidth, *boxHeight;


protected slots:
    virtual void languageChange();
	void accept();
	void update();

signals:
	void options(int,int,int,int);

};

#endif // imageDialog_H
