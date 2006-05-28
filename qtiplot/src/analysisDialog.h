/***************************************************************************
    File                 : analysisDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Analysis options dialog
                           
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
#ifndef ANALYSISDIALOG_H
#define ANALYSISDIALOG_H

#include <qvariant.h>
#include <qdialog.h>

class Q3ButtonGroup;
class QPushButton;
class QCheckBox;
class QLineEdit;
class QComboBox;
class Q3HButtonGroup;
	
//! Analysis options dialog
class analysisDialog : public QDialog
{
    Q_OBJECT

public:
    analysisDialog( QWidget* parent = 0, const QString& text="Curve", const char* name = 0, bool modal = FALSE, Qt::WFlags fl = 0 );
    ~analysisDialog();

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
    Q3ButtonGroup* GroupBox1;
    Q3HButtonGroup* GroupBox2;
    QCheckBox* boxShowFormula;
	QComboBox* boxName;

public slots:
    virtual void languageChange();
	void accept();
	void setCurveNames(const QStringList& names);
	void setOperationType(const QString& s){operation=s;};

signals:
	void options(const QString&);
	void analyse(const QString&, const QString&);

private:
	QString operation;
};

#endif 



