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

#include "multilayer.h"

class QGroupBox;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QTabWidget;
class QWidget;
class QComboBox;

//! Arrange layers dialog
class LayerDialog : public QDialog
{
    Q_OBJECT

public:
    LayerDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~LayerDialog();

	void setMultiLayer(MultiLayer *g);
	void initFonts(const QFont& titlefont, const QFont& axesfont, 
				   const QFont& numbersfont, const QFont& legendfont);

    QPushButton* buttonOk, *btnTitle, *btnAxesLabels, *btnAxesNumbers, *btnLegend;
	QPushButton* buttonCancel;
	QPushButton* buttonApply;
    QGroupBox *GroupCanvasSize, *GroupGrid;
    QSpinBox *boxX, *boxY, *boxColsGap, *boxRowsGap;
	QSpinBox *boxRightSpace, *boxLeftSpace, *boxTopSpace, *boxBottomSpace;
	QSpinBox *boxCanvasWidth, *boxCanvasHeight, *layersBox;
	QCheckBox *fitBox;
	QTabWidget* generalDialog;
	QWidget* layout, *fonts;	
	QComboBox *alignHorBox, *alignVertBox;

protected slots:
	void accept();
	void update();
	void setTitlesFont();
	void setAxesLabelsFont();
	void setAxesNumbersFont();
	void setLegendsFont();
	void enableLayoutOptions(bool ok);

private:
	QFont titleFont, legendFont, axesFont, numbersFont;
	MultiLayer *multi_layer;
};

#endif
