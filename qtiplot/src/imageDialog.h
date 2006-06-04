/***************************************************************************
    File                 : imageDialog.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Image geometry dialog
                           
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
#ifndef IMAGEDIALOG_H
#define IMAGEDIALOG_H

#include <qvariant.h>
#include <qdialog.h>
#include <qpushbutton.h>
//Added by qt3to4:
#include <QEvent>

class Q3ButtonGroup;
class QSpinBox;
class ChainButton;

//! Image geometry dialog
class imageDialog : public QDialog
{
    Q_OBJECT

public:
    imageDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~imageDialog();
	void setOrigin(const QPoint& o);
	void setSize(const QSize& size);

    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonApply;
    Q3ButtonGroup* GroupBox1, *GroupBox2, *GroupBox3, *GroupBox4;
    QSpinBox* boxX, *boxY, *boxWidth, *boxHeight;
	ChainButton *linkButton;

protected slots:
    virtual void languageChange();
	void accept();
	void update();
	void adjustHeight(int width);
	void adjustWidth(int height);

signals:
	void options(int,int,int,int);

protected:
	double aspect_ratio;
};

class ChainButton : public QPushButton
{
  Q_OBJECT

public:
  ChainButton(QWidget *parent = 0);
  ~ChainButton(){};

  void enterEvent ( QEvent * ){setFlat (false);};
  void leaveEvent ( QEvent * ){setFlat (true);};

public slots:
  void changeLock();
  bool isLocked(){return locked;};

private:
	bool locked;
};

#endif // imageDialog_H
