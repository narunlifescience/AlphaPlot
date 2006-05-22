#ifndef VALUESDIALOG_H
#define VALUESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QComboBox;
class QTextEdit;
class QSpinBox;
class QPushButton;
class QLabel;
class Table;
	
class setColValuesDialog : public QDialog
{ 
    Q_OBJECT

public:
    setColValuesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~setColValuesDialog();
	
	QSize sizeHint() const ;

    QComboBox* functions;
    QComboBox* boxColumn;
    QPushButton* PushButton3; 
    QPushButton* PushButton4;
    QPushButton* btnOk;
    QPushButton* btnCancel;
    QTextEdit* commandes;
    QTextEdit* explain;
	QSpinBox* start, *end;
	QPushButton *buttonPrev, *buttonNext, *addCellButton, *btnApply;
	QLabel *colNameLabel;

public slots:
	void accept();
	bool apply();
	void prevColumn();
	void nextColumn();
	void setFunctions();
	void insertFunction();
	void insertCol();
	void insertCell();
	void insertExplain(int index);
	void setTable(Table* w);
	void updateColumn(int sc);

private:
	Table* table;
};

#endif //
