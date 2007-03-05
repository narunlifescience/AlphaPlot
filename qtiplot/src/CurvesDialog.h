/***************************************************************************
    File                 : CurvesDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Add/remove curves dialog
                           
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/
#ifndef CURVESDIALOG_H
#define CURVESDIALOG_H

#include <QDialog>

class QComboBox;
class QListWidget;
class QPushButton;

class Graph;
class Table;

//! Add/remove curves dialog
class CurvesDialog : public QDialog
{ 
    Q_OBJECT

public:
    CurvesDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~CurvesDialog();

	void initTablesList(QWidgetList* lst){tables = lst;};
	void setGraph(Graph *graph);

public slots:
	void insertCurvesToDialog(const QStringList& names);
	void addCurves();
	bool addCurve(const QString& name);
	void removeCurves();
	int curveStyle();
	void showPlotAssociations();
	void showFunctionDialog();
	void showCurveBtn(int);
	void setCurveDefaultSettings(int style, int width, int size);

private slots:
    void enableAddBtn();
	void enableRemoveBtn();

signals:
	void showPlotAssociations(int);
	void showFunctionDialog(Graph *, int);

private:
    Table* findTable(const QString& text);
    QSize sizeHint() const;
    void contextMenuEvent(QContextMenuEvent *);

	int defaultCurveLineWidth, defaultSymbolSize;
	QWidgetList* tables;
	Graph *g;

    QPushButton* btnAdd;
    QPushButton* btnRemove;
    QPushButton* btnOK;
    QPushButton* btnCancel;
	QPushButton* btnAssociations;
	QPushButton* btnEditFunction;
    QListWidget* available;
    QListWidget* contents;
	QComboBox* boxStyle;
};

#endif // CurvesDialog_H
