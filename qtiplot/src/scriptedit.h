/***************************************************************************
    File                 : scriptedit.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Scripting classes
                           
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
#ifndef SCRIPTEDIT_H
#define SCRIPTEDIT_H

#include <q3textedit.h>
//Added by qt3to4:
#include <QMenu>
#include "Scripting.h"
#include <QTextEdit>

class QAction;
class QMenu;

class ScriptEdit: public QTextEdit
{
  Q_OBJECT
    
  public:
    ScriptEdit(ScriptingEnv *env, QWidget *parent=0, const char *name=0);
    ~ScriptEdit();
    
  public slots:
    void execute();
    void executeAll();
    void evaluate();
    void print();
    void insertFunction(const QString &);
    void insertFunction(QAction * action);
    void setContext(QObject *context) { myScript->setContext(context); }

  protected:
    QMenu * createStandardContextMenu();

  private:
    ScriptingEnv *scriptEnv;
    Script *myScript;
    QAction *actionExecute, *actionExecuteAll, *actionEval, *actionPrint;
    QMenu *functionsMenu;

  private slots:
    void insertErrorMsg(const QString &message);
};

#endif
