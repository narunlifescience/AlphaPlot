/***************************************************************************
    File                 : PlotWizard.cpp
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : A wizard type dialog to create new plots

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
#include "PlotWizard.h"

#include <QApplication>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QSizePolicy>
#include <QVBoxLayout>

#include "3Dplot/Layout3D.h"
#include "ApplicationWindow.h"
#include "Table.h"

PlotWizard::PlotWizard(QWidget *parent, Qt::WindowFlags fl)
    : QDialog(parent, fl) {
  setWindowTitle(tr("Select Columns to Plot"));

  setSizeGripEnabled(true);

  // top part starts here
  groupBox1 = new QGroupBox();

  QGridLayout *gl1 = new QGridLayout();
  buttonX = new QPushButton("<->" + tr("&X"));
  buttonX->setAutoDefault(false);
  gl1->addWidget(buttonX, 0, 0);

  buttonXErr = new QPushButton("<->" + tr("x&Err"));
  buttonXErr->setAutoDefault(false);
  gl1->addWidget(buttonXErr, 0, 1);

  buttonY = new QPushButton("<->" + tr("&Y"));
  buttonY->setAutoDefault(false);
  gl1->addWidget(buttonY, 1, 0);

  buttonYErr = new QPushButton("<->" + tr("yE&rr"));
  buttonYErr->setAutoDefault(false);
  gl1->addWidget(buttonYErr, 1, 1);

  buttonZ = new QPushButton("<->" + tr("&Z"));
  buttonZ->setAutoDefault(false);
  gl1->addWidget(buttonZ, 2, 0);
  gl1->setRowStretch(3, 1);

  QHBoxLayout *hl2 = new QHBoxLayout();
  buttonNew = new QPushButton(tr("&New curve"));
  buttonNew->setDefault(true);
  buttonNew->setAutoDefault(true);
  hl2->addWidget(buttonNew);

  buttonDelete = new QPushButton(tr("&Delete curve"));
  buttonDelete->setAutoDefault(false);
  hl2->addWidget(buttonDelete);

  QVBoxLayout *vl = new QVBoxLayout();
  vl->addLayout(gl1);
  vl->addStretch();
  vl->addLayout(hl2);

  QGridLayout *gl2 = new QGridLayout(groupBox1);
  gl2->addWidget(new QLabel(tr("Worksheet")), 0, 0);
  boxTables = new QComboBox();
  gl2->addWidget(boxTables, 0, 1);
  columnsList = new QListWidget();
  gl2->addWidget(columnsList, 1, 0);
  gl2->addLayout(vl, 1, 1);

  // middle part is only one widget
  plotAssociations = new QListWidget();

  // bottom part starts here
  QHBoxLayout *bottomLayout = new QHBoxLayout();
  bottomLayout->addStretch();

  buttonOk = new QPushButton(tr("&Plot"));
  buttonOk->setAutoDefault(false);
  bottomLayout->addWidget(buttonOk);

  buttonCancel = new QPushButton(tr("&Close"));
  buttonCancel->setAutoDefault(false);
  bottomLayout->addWidget(buttonCancel);

  QVBoxLayout *vlayout = new QVBoxLayout(this);
  vlayout->addWidget(groupBox1);
  vlayout->addWidget(plotAssociations);
  vlayout->addLayout(bottomLayout);

  // signals and slots connections
  connect(boxTables, &QComboBox::textActivated, this,
          &PlotWizard::changeColumnsList);
  connect(buttonOk, &QPushButton::clicked, this, &PlotWizard::accept);
  connect(buttonCancel, &QPushButton::clicked, this, &PlotWizard::reject);
  connect(buttonNew, &QPushButton::clicked, this, &PlotWizard::addCurve);
  connect(buttonDelete, &QPushButton::clicked, this, &PlotWizard::removeCurve);
  connect(buttonX, &QPushButton::clicked, this, &PlotWizard::addXCol);
  connect(buttonY, &QPushButton::clicked, this, &PlotWizard::addYCol);
  connect(buttonXErr, &QPushButton::clicked, this, &PlotWizard::addXErrCol);
  connect(buttonYErr, &QPushButton::clicked, this, &PlotWizard::addYErrCol);
  connect(buttonZ, &QPushButton::clicked, this, &PlotWizard::addZCol);
}

QSize PlotWizard::sizeHint() const { return QSize(350, 400); }

void PlotWizard::accept() {
  QStringList curves, curves3D, ribbons;
  for (int i = 0; i < plotAssociations->count(); i++) {
    QString text = plotAssociations->item(i)->text();
    if (text.endsWith("(X)")) {
      QMessageBox::critical(
          this, tr("Error"),
          tr("Please define a Y column for the following curve") + ":\n\n" +
              text);
      return;
    }

    if (text.contains("(Z)")) {
      if (text.contains("(Y)") && !curves3D.contains(text))
        curves3D << text;
      else if (!text.contains("(Y)") && !ribbons.contains(text))
        ribbons << text;
    } else if (text.contains("(xErr)") || text.contains("(yErr)")) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
      QStringList lst = text.split(",", Qt::SkipEmptyParts);
#else
      QStringList lst = text.split(",", QString::SkipEmptyParts);
#endif
      lst.pop_back();
      QString master_curve = lst.join(",");

      if (!curves.contains(master_curve)) curves.prepend(master_curve);

      if (!curves.contains(text))
        curves << text;  // add error bars at the end of the list.
    } else if (!curves.contains(text))
      curves.prepend(text);
  }

  if (curves.count() > 0) emit plot(curves);

  if (curves3D.count() > 0) plot3D(curves3D);

  if (ribbons.count() > 0) plot3DRibbon(ribbons);

  if (!noCurves()) close();
}

void PlotWizard::changeColumnsList(const QString &table) {
  QStringList newList;

  for (int i = 0; i < columns.count(); i++) {
    QString s = columns[i];
    if (s.contains(table)) newList << s.remove(table + "_");
  }
  setColumnsListBoxContents(newList);
}

void PlotWizard::addXCol() {
  if (noCurves()) return;

  QString text = plotAssociations->currentItem()->text();
  if (text.contains("(X)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("You have already defined a X column!"));
  else {
    plotAssociations->currentItem()->setText(
        text + columnsList->currentItem()->text() + "(X)");
  }
}

void PlotWizard::addYCol() {
  if (noCurves()) return;

  QString text = plotAssociations->currentItem()->text();
  if (!text.contains("(X)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("You must define a X column first!"));
  else if (text.contains("(Y)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("You have already defined a Y column!"));
  else {
    plotAssociations->currentItem()->setText(
        text + ", " + columnsList->currentItem()->text() + "(Y)");
  }
}

void PlotWizard::addZCol() {
  if (noCurves()) return;

  QString text = plotAssociations->currentItem()->text();
  if (text.contains("(xErr)") || text.contains("(yErr)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("This kind of curve is not handled by AlphaPlot!"));
  else if (!text.contains("(X)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("You must define a X column first!"));
  else if (text.contains("(Z)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("You have already defined a Z column!"));
  else {
    plotAssociations->currentItem()->setText(
        text + ", " + columnsList->currentItem()->text() + "(Z)");
  }
}

void PlotWizard::addXErrCol() {
  if (noCurves()) return;

  QString text = plotAssociations->currentItem()->text();
  if (text.contains("(Z)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("This kind of curve is not handled by AlphaPlot!"));
  else if (!text.contains("(X)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("You must define a X column first!"));
  else if (!text.contains("(Y)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("You must define a Y column first!"));
  else if (text.contains("(xErr)") || text.contains("(yErr)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("You have already defined an error-bars column!"));
  else {
    plotAssociations->currentItem()->setText(
        text + ", " + columnsList->currentItem()->text() + "(xErr)");
  }
}

void PlotWizard::addYErrCol() {
  if (noCurves()) return;

  QString text = plotAssociations->currentItem()->text();
  if (text.contains("(Z)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("This kind of curve is not handled by AlphaPlot!"));
  else if (!text.contains("(X)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("You must define a X column first!"));
  else if (!text.contains("(Y)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("You must define a Y column first!"));
  else if (text.contains("(xErr)") || text.contains("(yErr)"))
    QMessageBox::warning(this, tr("Error"),
                         tr("You have already defined an error-bars column!"));
  else {
    plotAssociations->currentItem()->setText(
        text + ", " + columnsList->currentItem()->text() + "(yErr)");
  }
}

void PlotWizard::addCurve() {
  plotAssociations->addItem(boxTables->currentText() + ": ");
  plotAssociations->setCurrentRow(plotAssociations->count() - 1);
}

void PlotWizard::removeCurve() {
  plotAssociations->takeItem(plotAssociations->currentRow());
}

void PlotWizard::insertTablesList(const QStringList &tables) {
  boxTables->addItems(tables);
}

void PlotWizard::setColumnsListBoxContents(const QStringList &cols) {
  columnsList->clear();
  columnsList->insertItems(0, cols);
  columnsList->setCurrentRow(0);
}

void PlotWizard::setColumnsList(const QStringList &cols) { columns = cols; }

bool PlotWizard::noCurves() {
  if (plotAssociations->count() == 0) {
    QMessageBox::warning(0, tr("Error"), tr("You must add a new curve first!"));
    return true;
  } else
    return false;
}

void PlotWizard::plot3DRibbon(const QStringList &lst) {
  Q_UNUSED(lst)
  /*ApplicationWindow *app = (ApplicationWindow *)this->parent();
  if (!app) return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  for (int i = 0; i < lst.count(); i++) {
    QString s = lst[i];
    int pos = s.indexOf(":", 0);
    QString table_name = s.left(pos) + "_";
    Table *t = app->table(table_name);
    if (t) {
      int posX = s.indexOf("(", pos);
      QString xColName = table_name + s.mid(pos + 2, posX - pos - 2);

      posX = s.indexOf(",", posX);
      int posY = s.indexOf("(", posX);
      QString yColName = table_name + s.mid(posX + 2, posY - posX - 2);

      Graph3D *g = app->newPlot3D();
      if (g) {
        g->addData(t, xColName, yColName);
        g->update();
      }
    }
  }
  QApplication::restoreOverrideCursor();*/
}

void PlotWizard::plot3D(const QStringList &lst) {
  Q_UNUSED(lst)
  /*ApplicationWindow *app = (ApplicationWindow *)this->parent();
  if (!app) return;

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

  for (int i = 0; i < lst.count(); i++) {
    QString s = lst[i];
    int pos = s.indexOf(":", 0);
    QString table_name = s.left(pos) + "_";
    Table *t = app->table(table_name);
    if (t) {
      int posX = s.indexOf("(", pos);
      QString xColName = table_name + s.mid(pos + 2, posX - pos - 2);

      posX = s.indexOf(",", posX);
      int posY = s.indexOf("(", posX);
      QString yColName = table_name + s.mid(posX + 2, posY - posX - 2);

      posY = s.indexOf(",", posY);
      int posZ = s.indexOf("(", posY);
      QString zColName = table_name + s.mid(posY + 2, posZ - posY - 2);

      int xCol = t->colIndex(xColName);
      int yCol = t->colIndex(yColName);
      int zCol = t->colIndex(zColName);
      if (xCol >= 0 && yCol >= 0 && zCol >= 0) {
        Graph3D *g = app->newPlot3D();
        if (g) {
          g->addData(t, xCol, yCol, zCol, 1);
          g->update();
        }
      }
    }
  }
  QApplication::restoreOverrideCursor();*/
}

PlotWizard::~PlotWizard() {}
