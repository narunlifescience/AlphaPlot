/***************************************************************************
    File                 : ErrDialog.h
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Add error bars dialog

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
#ifndef ERRDIALOG_H
#define ERRDIALOG_H

#include <2Dplot/Graph2DCommon.h>

#include <QDialog>
#include <QList>

class ApplicationWindow;
class QLabel;
class QComboBox;
class QRadioButton;
class QLineEdit;
class QPushButton;
class QGroupBox;
class QButtonGroup;
class QWidget;
class QMdiSubWindow;
class AxisRect2D;
class Table;
class Column;

//! Add error bars dialog
class ErrDialog : public QDialog {
  Q_OBJECT

 public:
  //! Constructor
  /**
   * \param parent parent widget
   * \param fl window flags
   */
  ErrDialog(QWidget* parent, AxisRect2D* axisrect,
            Qt::WindowFlags fl = Qt::Widget);
  //! Destructor
  ~ErrDialog();

 private:
  Column* getErrorCol(Table* parent);
  QLabel* textLabel1;
  QComboBox *nameLabel, *tableNamesBox;
  QGroupBox* groupBox2;
  QGroupBox *groupBox1, *groupBox3;
  QButtonGroup *buttonGroup1, *buttonGroup2;
  QRadioButton *standardBox, *columnBox;
  QRadioButton* percentBox;
  QLineEdit* valueBox;
  QRadioButton* xErrBox;
  QRadioButton* yErrBox;
  QPushButton* buttonAdd;
  QPushButton* buttonCancel;
  QList<QMdiSubWindow*>* srcTables;

 public slots:
  //! Add a plot definition
  void add();
  //! Supply the dialog with a curves list
  void plotNames();
  void errorColumnNames();

 signals:
  //! This is usually connected to the main window's defineErrorBars() slot
  void options(const QString& curveName, int type, const QString& percent,
               int direction);
  //! This is usually connected to the main window's defineErrorBars() slot
  void options(const QString& curveName, const QString& errColumnName,
               int direction);

 private:
  AxisRect2D* axisrect_;
  ApplicationWindow* app_;
  QList<PlotData::AssociatedData> plotted_columns_;
  QList<QPair<Table*, Column*>> error_columns_;
};

#endif  // ERRDIALOG_H
