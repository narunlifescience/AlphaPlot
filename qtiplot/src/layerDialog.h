/***************************************************************************
    File                 : layerDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Arrange layers dialog
                           
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
#ifndef LAYERDIALOG_H
#define LAYERDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class Q3ButtonGroup;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QTabWidget;
class QWidget;
class QComboBox;
class MultiLayer;

//! Arrange layers dialog
class layerDialog : public QDialog
{
    Q_OBJECT

public:
    layerDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~layerDialog();

	void setMultiLayer(MultiLayer *g);
	void initFonts(const QFont& titlefont, const QFont& axesfont, 
				   const QFont& numbersfont, const QFont& legendfont);

    QPushButton* buttonOk, *btnTitle, *btnAxisLegend, *btnAxisNumbers, *btnLegend;
	QPushButton* buttonCancel;
	QPushButton* buttonApply;
    Q3ButtonGroup* GroupBox1, *GroupBox2, *GroupBox4, *GroupCanvasSize;
    QSpinBox *boxX, *boxY, *boxColsGap, *boxRowsGap;
	QSpinBox *boxRightSpace, *boxLeftSpace, *boxTopSpace, *boxBottomSpace;
	QSpinBox *boxCanvasWidth, *boxCanvasHeight, *layersBox;
	QCheckBox *fitBox;
	QTabWidget* generalDialog;
	QWidget* layout, *fonts;	
	QComboBox *alignHorBox, *alignVertBox;

protected slots:
    virtual void languageChange();
	void accept();
	void update();
	void setTitlesFont();
	void setAxisLegendFont();
	void setAxisNumbersFont();
	void setLegendsFont();
	void enableLayoutOptions(bool ok);

private:
	QFont titleFont, legendFont, axesFont, numbersFont;
	MultiLayer *multi_layer;
};

#endif
