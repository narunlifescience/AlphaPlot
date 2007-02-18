/***************************************************************************
    File                 : ScriptWindow.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Python script window
                           
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
#ifndef SCRIPTWINDOW_H
#define SCRIPTWINDOW_H

#include "scriptedit.h"

#include <QMainWindow>
#include <QMenu>
#include <QCloseEvent>
class ScriptingEnv;
class QAction;

//! Python script window
class ScriptWindow: public QMainWindow
{
	Q_OBJECT

public:
		ScriptWindow(ScriptingEnv *env);

public slots:
		void newScript();
		void open();
		void save();
		void saveAs();
		void languageChange();
		virtual void setVisible(bool visible);

signals:
		void visibilityChanged(bool visible);

private:
		void initMenu();
		void initActions();
		ScriptEdit *te;

		QString fileName;

		QMenu *file, *edit, *run;
		QAction *actionNew, *actionUndo, *actionRedo, *actionCut, *actionCopy, *actionPaste, *actionDelete;
		QAction *actionExecute, *actionExecuteAll, *actionEval, *actionPrint, *actionOpen;
		QAction *actionSave, *actionSaveAs;
};

#endif
