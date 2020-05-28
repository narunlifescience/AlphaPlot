/***************************************************************************
    File                 : FilterDialog.h
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Filter options dialog

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
#ifndef FILTERDIALOG_H
#define FILTERDIALOG_H

#include <QDialog>

class QPushButton;
class QLineEdit;
class QComboBox;
class QCheckBox;
class AxisRect2D;
class ColorBox;

//! Filter options dialog
class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    FilterDialog(int type, QWidget *parent = nullptr,
                 Qt::WindowFlags flag = Qt::Widget);
    ~FilterDialog() { }

    QPushButton *buttonFilter;
    QPushButton *buttonCancel;
    QComboBox *boxName;
    QCheckBox *boxOffset;
    QLineEdit *boxStart;
    QLineEdit *boxEnd;
    ColorBox *boxColor;

public slots:
    void setAxisRect(AxisRect2D *axisrect);
    void filter();

private:
    AxisRect2D *axisrect_;
    int filter_type;
};

#endif // FILTERDIALOG_H
