/***************************************************************************
    File                 : MyWidget.cpp
    Project              : AlphaPlot
    Description          : MDI window widget
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2009 Knut Franke (knut.franke*gmx.de)
    Copyright            : (C) 2006-2009 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2006-2007 by Ion Vasilief (ion_vasilief*yahoo.fr)
                           (replace * with @ in the email address)

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
#include "MyWidget.h"
#include "Folder.h"

#include <QCloseEvent>
#include <QEvent>
#include <QIcon>
#include <QLocale>
#include <QMessageBox>
#include <QString>
#include <QtDebug>

MyWidget::MyWidget(const QString &label, QWidget *parent, const QString name,
                   Qt::WFlags f)
    : QWidget(parent, f) {
  w_label = label;
  caption_policy = Both;
  askOnClose = true;
  w_status = Normal;
  titleBar = nullptr;
  setObjectName(QString(name));
}

void MyWidget::updateCaption() {
  switch (caption_policy) {
    case Name:
      setWindowTitle(name());
      break;

    case Label:
      if (!windowLabel().isEmpty())
        setWindowTitle(windowLabel());
      else
        setWindowTitle(name());
      break;

    case Both:
      if (!windowLabel().isEmpty())
        setWindowTitle(name() + " - " + windowLabel());
      else
        setWindowTitle(name());
      break;
    }
}

void MyWidget::focusInEvent(QFocusEvent *)
{
  emit focusinWindow(this);
}

void MyWidget::focusOutEvent(QFocusEvent *)
{
  emit focusoutWindow(this);
};

void MyWidget::closeEvent(QCloseEvent *e) {
  if (askOnClose) {
    switch (QMessageBox::information(
        this, "AlphaPlot", tr("Do you want to hide or delete") + "<p><b>'" +
                               objectName() + "'</b> ?",
        tr("Delete"), tr("Hide"), tr("Cancel"), 0, 2)) {
      case 0:
        emit closedWindow(this);
        e->accept();
        break;

      case 1:
        e->ignore();
        emit hiddenWindow(this);
        break;

      case 2:
        e->ignore();
        break;
    }
  } else {
    emit closedWindow(this);
    e->accept();
  }
}

QString MyWidget::aspect() {
  QString status = tr("Normal");
  switch (w_status) {
    case Hidden:
      return tr("Hidden");

    case Normal:
      break;

    case Minimized:
      return tr("Minimized");

    case Maximized:
      return tr("Maximized");
  }
  return status;
}

// Modifying the title bar menu is somewhat more complicated in Qt4.
// Apart from the trivial change in how we intercept the reparenting,
// in Qt4 the title bar doesn't exist yet at this point.
// Thus, we now also have to intercept the creation of the title bar
// in MyWidget::eventFilter.
void MyWidget::changeEvent(QEvent *event) {
  if (event->type() == QEvent::ParentChange) {
    titleBar = 0;
    if (parent()) parent()->installEventFilter(this);
  } else if (!isHidden() && event->type() == QEvent::WindowStateChange) {
    if (static_cast<QWindowStateChangeEvent *>(event)->oldState() ==
        windowState())
      return;

    if (windowState() & Qt::WindowMinimized)
      w_status = Minimized;
    else if (windowState() & Qt::WindowMaximized)
      w_status = Maximized;
    else
      w_status = Normal;
    emit statusChanged(this);
  }
  QWidget::changeEvent(event);
}

bool MyWidget::eventFilter(QObject *object, QEvent *event) {
  QWidget *tmp;
  if (event->type() == QEvent::ContextMenu && object == titleBar) {
    emit showTitleBarMenu();
    static_cast<QContextMenuEvent *>(event)->accept();
    return true;
  } else if (event->type() == QEvent::ChildAdded && object == parent() &&
             (tmp = qobject_cast<QWidget *>(
                  static_cast<QChildEvent *>(event)->child()))) {
    (titleBar = tmp)->installEventFilter(this);
    parent()->removeEventFilter(this);
    return true;
  }
  return false;
}

void MyWidget::setStatus(Status status) {
  if (w_status == status) return;

  w_status = status;
  emit statusChanged(this);
}

void MyWidget::setHidden() {
  w_status = Hidden;
  emit statusChanged(this);
  hide();
}

void MyWidget::setNormal() {
  showNormal();
  w_status = Normal;
  emit statusChanged(this);
}

void MyWidget::setMinimized() {
  showMinimized();
  w_status = Minimized;
  emit statusChanged(this);
}

void MyWidget::setMaximized() {
  showMaximized();
  w_status = Maximized;
  emit statusChanged(this);
}
