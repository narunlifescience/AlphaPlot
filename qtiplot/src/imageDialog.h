#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qpushbutton.h>

class QButtonGroup;
class QSpinBox;
class ChainButton;

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
	ChainButton *linkButton;

protected slots:
    virtual void languageChange();
	void accept();
	void update();
	void adjustHeight(int width);
	void adjustWidth(int height);

signals:
	void options(int,int,int,int);

protected:
	double aspect_ratio;
};

class ChainButton : public QPushButton
{
  Q_OBJECT

public:
  ChainButton(QWidget *parent = 0);
  ~ChainButton(){};

  void enterEvent ( QEvent * ){setFlat (false);};
  void leaveEvent ( QEvent * ){setFlat (true);};

public slots:
  void changeLock();
  bool isLocked(){return locked;};

private:
	bool locked;
};

#endif // imageDialog_H
