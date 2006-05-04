#ifndef LINEDIALOG_H
#define LINEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QCheckBox;
class QComboBox;
class QButtonGroup;
class QPushButton;
class QTabWidget;
class QWidget;
class QSpinBox;
class ColorButton;

class lineDialog : public QDialog
{ 
    Q_OBJECT

public:
    lineDialog(QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~lineDialog();

    QButtonGroup* GroupBox1, *GroupBox2;
    ColorButton* colorBox;
    QComboBox* styleBox;
    QComboBox* widthBox;
    QPushButton* btnOk;
    QPushButton* btnCancel;
    QPushButton* btnApply;
    QCheckBox* endBox;
    QCheckBox* startBox,  *filledBox;
	QTabWidget* tw;
	QWidget *options, *geometry, *head;
	QSpinBox *xStartBox, *yStartBox, *xEndBox, *yEndBox;
	QSpinBox *boxHeadAngle, *boxHeadLength;

public slots:
	void setColor(QColor c);
	void setWidth(int w);
	void setStyle(Qt::PenStyle style);
	void setStartArrow(bool on);
	void setEndArrow(bool on);
	void setStartPoint(const QPoint& p);
	void setEndPoint(const QPoint& p);
	void initHeadGeometry(int length, int angle, bool filled);
	void enableHeadTab();
	void pickColor();
	void accept();
	void apply();

signals:
	void values(const QColor&, int, Qt::PenStyle, bool, bool);
	void setLineGeometry(const QPoint&,const QPoint&);
	void setHeadGeometry(int, int, bool);
};

#endif // LINEDIALOG_H
