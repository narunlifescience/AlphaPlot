/***************************************************************************
	File                 : ColorMapEditor.h
	Project              : QtiPlot
--------------------------------------------------------------------
	Copyright            : (C) 2006 by Ion Vasilief
	Email (use @ for *)  : ion_vasilief*yahoo.fr
	Description          : A QwtLinearColorMap Editor Widget
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
#ifndef COLORMAPEDITOR_H
#define COLORMAPEDITOR_H

#include <QWidget>
#include <qwt_color_map.h>

class QPushButton;
class QTableWidget;
class QCheckBox;

class ColorMapEditor: public QWidget
{
    Q_OBJECT

public:
	ColorMapEditor(QWidget* parent=0);
	
	QwtLinearColorMap colorMap(){return color_map;};
	void setColorMap(const QwtLinearColorMap& map);

	void setRange(double min, double max);

protected slots:
	void updateColorMap();
	void validateLevel(int row, int col);
	void enableButtons(int row, int col, int = 0, int = 0);
	void showColorDialog(int row, int col);
	void insertLevel();
	void deleteLevel();
	void setScaledColors(bool scale = true);

	bool eventFilter(QObject *object, QEvent *e);

private:
	QTableWidget *table;	
	QPushButton *insertBtn, *deleteBtn;
	QCheckBox *scaleColorsBox;

	//! Color map object
	QwtLinearColorMap color_map;

	//! Levels range
	double min_val, max_val;
};
   
#endif
