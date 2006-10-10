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

#include <qdialog.h>
#include <QEvent>

#include <QSpinBox.h>
#include <QPushButton.h>

class ChainButton;

//! Image geometry dialog
class ImageDialog : public QDialog
{
    Q_OBJECT

public:
    ImageDialog( QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WFlags fl = 0 );
    ~ImageDialog();
	void setOrigin(const QPoint& o);
	void setSize(const QSize& size);

protected slots:
	void accept();
	void update();
	void adjustHeight(int width);
	void adjustWidth(int height);

signals:
	void setGeometry(int, int, int, int);

protected:
	double aspect_ratio;
	
private:
    QPushButton* buttonOk;
	QPushButton* buttonCancel;
	QPushButton* buttonApply;
    QSpinBox* boxX, *boxY, *boxWidth, *boxHeight;
	ChainButton *linkButton;
};

//! A special button to connect two values
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

#endif // IMAGEDIALOG_H
