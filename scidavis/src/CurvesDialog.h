/***************************************************************************
    File                 : CurvesDialog.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
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
class QCheckBox;
class Graph;

//! Add/remove curves dialog
class CurvesDialog : public QDialog
{
    Q_OBJECT

public:
    CurvesDialog( QWidget* parent = 0, Qt::WFlags fl = 0 );

	void setGraph(Graph *graph);

private slots:
	void addCurves();
	void removeCurves();
	int curveStyle();
	void showCurveRangeDialog();
	void showPlotAssociations();
	void showFunctionDialog();

	/**
  Enables ("yes")/disables ("no") the following buttons, which are on the right besides
  the graph contents list, depending on the selected item in this list:

  <table>
  <tr>  <td>Selected Item</td><td>btnEditFunction</td>  <td>btnAssociations</td>  <td>btnRange</td> </tr>
  <tr>  <td>Spectrogram</td>  <td>no</td> <td>no</td> <td>no</td>   </tr>
  <tr>  <td>Function</td>     <td>yes</td> <td>no</td> <td>no</td>   </tr>
  <tr>  <td>ErrorBars</td>    <td>no</td> <td>yes</td> <td>no</td>   </tr>
  <tr>  <td>all others</td>   <td>no</td> <td>yes</td> <td>yes</td>   </tr>
  </table>
	*/
	void showCurveBtn(int);
    void enableAddBtn();
	void enableRemoveBtn();
	void showCurveRange(bool);
	void updateCurveRange();
	void showCurrentFolder(bool);

private:
	void closeEvent(QCloseEvent*);

    void init();
    bool addCurve(const QString& name);
    QSize sizeHint() const;
    void contextMenuEvent(QContextMenuEvent *);

	int defaultCurveLineWidth, defaultSymbolSize;
	Graph *d_graph;

    QPushButton* btnAdd;
    QPushButton* btnRemove;
    QPushButton* btnOK;
    QPushButton* btnCancel;
	QPushButton* btnAssociations;
	QPushButton* btnEditFunction;
	QPushButton* btnRange;
    QListWidget* available;
    QListWidget* contents;
	QComboBox* boxStyle;
	QComboBox* boxMatrixStyle;
	QCheckBox* boxShowRange;
	QCheckBox* boxShowCurrentFolder;
};

#endif // CurvesDialog_H
