#ifndef CURVESDIALOG_H
#define CURVESDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListBox;
class QListBoxItem;
class QPushButton;
class QComboBox;
class QWidgetList;

class Graph;
class Table;

class curvesDialog : public QDialog
{ 
    Q_OBJECT

public:
    curvesDialog( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~curvesDialog();

	QSize sizeHint() const;

    QLabel* TextLabel2;
    QPushButton* btnAdd;
    QPushButton* btnRemove;
    QPushButton* btnOK;
    QPushButton* btnCancel;
	QPushButton* btnAssociations;
	QPushButton* btnEditFunction;
    QLabel* TextLabel1;
    QListBox* available;
    QListBox* contents;
	QComboBox* boxStyle;

	void initTablesList(QWidgetList* lst){tables = lst;};
	void setGraph(Graph *graph);

public slots:
	Table * findTable(const QString& text);

	void insertCurvesToDialog(const QStringList& names);
	void addCurve();
	bool addCurve(const QString& name);
	void addSelectedCurve();
	void removeCurve();
	void removeSelectedCurve();
	void clear();
	void enableRemoveBtn();
	void deletePopupMenu(QListBoxItem *it, const QPoint &point);
	void addPopupMenu(QListBoxItem *it, const QPoint &point);
	int curveStyle();
	void showPlotAssociations();
	void showFunctionDialog();
	void showCurveBtn(int);
	void setCurveDefaultSettings(int style, int width, int size);

signals:
	void showPlotAssociations(int);
	void showFunctionDialog(const QString&, int);

private:
	int selectedCurve, defaultCurveLineWidth, defaultSymbolSize;
	QWidgetList* tables;
	Graph *g;
};

#endif // curvesDialog_H
