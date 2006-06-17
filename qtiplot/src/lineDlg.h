/***************************************************************************
    File                 : lineDlg.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Line options dialog
                           
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
#ifndef LINEDIALOG_H
#define LINEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class QCheckBox;
class QComboBox;
class Q3ButtonGroup;
class QPushButton;
class QTabWidget;
class QWidget;
class QSpinBox;
class ColorButton;

//! Line options dialog
class LineDialog : public QDialog
{ 
    Q_OBJECT

public:
    LineDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0);
    ~LineDialog();

    Q3ButtonGroup* GroupBox1, *GroupBox2;
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
