/***************************************************************************
    File                 : renameWindowDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Rename window dialog
                           
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
#ifndef RENAMEDIALOG_H
#define RENAMEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

#include "widget.h"

class Q3ButtonGroup;
class QPushButton;
class QLineEdit;
class QRadioButton;
class Q3TextEdit;
class MyWidget;

//! Rename window dialog
class renameWindowDialog : public QDialog
{
    Q_OBJECT

public:
    renameWindowDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~renameWindowDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
    Q3ButtonGroup* GroupBox1, *GroupBox2;
	QRadioButton* boxName;
	QRadioButton* boxLabel;
	QRadioButton* boxBoth;
	QLineEdit* boxNameLine;
	Q3TextEdit* boxLabelEdit;

public slots:
	void setWidget(MyWidget *w);
	MyWidget::CaptionPolicy getCaptionPolicy();
    virtual void languageChange();
	void accept();

signals:

private:
	MyWidget *window;
};

#endif // EXPORTDIALOG_H
