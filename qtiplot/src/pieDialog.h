/***************************************************************************
    File                 : pieDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Pie plot dialog
                           
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
#ifndef PIEDIALOG_H
#define PIEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <q3memarray.h>
#include <qstring.h>
//Added by qt3to4:
#include <QLabel>
 
class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class Q3ListBox;
class Q3ListBoxItem;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QTabWidget;
class QWidget;
class QStringList;
class ColorBox;
class PatternBox;
class ColorButton;
class MultiLayer;

//! Pie plot dialog
class pieDialog : public QDialog
{ 
    Q_OBJECT

public:
    pieDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~pieDialog();

	void initPiePage();
	void initBorderPage();

    QPushButton* buttonWrk;
    QPushButton* buttonApply;
    QPushButton* buttonOk;
    QPushButton* buttonCancel;
    QTabWidget* generalDialog;
	QWidget* frame, *pieOptions;

    Q3ListBox *curvesList;
    QSpinBox *boxFrameWidth;
	QSpinBox *boxRay, *boxMargin;
	QCheckBox *boxFramed, *boxAll;
    ColorBox *boxFrameColor, *boxLineColor, *boxFirstColor;
	QComboBox *boxLineStyle;
    PatternBox *boxPattern;
	QSpinBox *boxLineWidth, *boxBorderWidth;

	ColorButton *boxBackgroundColor, *boxBorderColor;

public slots:
	void accept();
	void updatePlot();
	void drawFrame(bool framed);
	void setFramed(bool ok);
	void setFrameColor(const QColor& c);
	void setFrameWidth(int w);
	void insertCurveName(const QString& name);
	void showPopupMenu(Q3ListBoxItem *it, const QPoint &point);
	void removeCurve();
	void setBorderColor(const QColor& c);
	void setBorderWidth(int width);
	void setPieSize(int size);
	void setFirstColor(int c);

	void pickBorderColor();
	void pickBackgroundColor();
	void changeMargin(int width);
	void updateBorder(int width);

	void setMultiLayerPlot(MultiLayer *m);

	Qt::PenStyle style();
	void setBorderStyle(const Qt::PenStyle& style);
	Qt::BrushStyle pattern();
	void setPattern(const Qt::BrushStyle& style);
	void showWorksheet();
	void showGeneralPage();

signals:
	void worksheet(const QString&);
	void drawFrame(bool,int,const QColor&);
	void toggleCurve();
	void updatePie(const QPen&, const Qt::BrushStyle &,int, int);

private:
	MultiLayer *mPlot;
};

#endif // GENDIALOG_H
