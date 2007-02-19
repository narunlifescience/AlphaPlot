/***************************************************************************
    File                 : ScriptWindow.cpp
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
#include "ScriptWindow.h"
#include "scriptedit.h"
#include "pixmaps.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QMessageBox>
#include <QPixmap>
#include <QCloseEvent>
#include <QTextStream>

ScriptWindow::ScriptWindow(ScriptingEnv *env)
: QMainWindow()
{
	initMenu();

	fileName = QString::null;

	te = new ScriptEdit(env, this, name());
	te->setContext(this);
	setCentralWidget(te);

	initActions();
	setIcon(QPixmap(logo_xpm));
	setWindowTitle(tr("QtiPlot - Script Window"));
	setFocusProxy(te);
	setFocusPolicy(Qt::StrongFocus);
	resize(QSize(500, 300));
}

void ScriptWindow::initMenu()
{
	file = new QMenu(tr("&File"), this);
	menuBar()->addMenu(file);

	edit = new QMenu(tr("&Edit"), this);
	menuBar()->addMenu(edit);

	run = new QMenu(tr("E&xecute"), this);
	menuBar()->addMenu(run);

	menuBar()->addAction(tr("&Hide"), this, SLOT(close()));
}

void ScriptWindow::initActions()
{
	actionNew = new QAction(QPixmap(new_xpm), tr("&New"), this);
	actionNew->setShortcut( tr("Ctrl+N") );
	connect(actionNew, SIGNAL(activated()), this, SLOT(newScript()));
	file->addAction(actionNew);

	actionOpen = new QAction(QPixmap(fileopen_xpm), tr("&Open..."), this);
	actionOpen->setShortcut( tr("Ctrl+O") );
	connect(actionOpen, SIGNAL(activated()), this, SLOT(open()));
	file->addAction(actionOpen);

	actionSave = new QAction(QPixmap(filesave_xpm), tr("&Save"), this);
	actionSave->setShortcut( tr("Ctrl+S") );
	connect(actionSave, SIGNAL(activated()), this, SLOT(save()));
	file->addAction(actionSave);

	actionSaveAs = new QAction(tr("Save &As..."), this);
	connect(actionSaveAs, SIGNAL(activated()), this, SLOT(saveAs()));
	file->addAction(actionSaveAs);

	actionPrint = new QAction(QPixmap(fileprint_xpm), tr("&Print"), this);
	actionPrint->setShortcut( tr("Ctrl+P") );
	connect(actionPrint, SIGNAL(activated()), te, SLOT(print()));
	file->addAction(actionPrint);

	actionUndo = new QAction(QPixmap(undo_xpm), tr("&Undo"), this);
	actionUndo->setShortcut( tr("Ctrl+Z") );
	connect(actionUndo, SIGNAL(activated()), te, SLOT(undo()));	
	edit->addAction(actionUndo);
	actionUndo->setEnabled(false);

	actionRedo = new QAction(QPixmap(redo_xpm), tr("&Redo"), this);
	actionRedo->setShortcut( tr("Ctrl+Y") );
	connect(actionRedo, SIGNAL(activated()), te, SLOT(redo()));	
	edit->addAction(actionRedo);
	actionRedo->setEnabled(false);
	edit->insertSeparator();

	actionCut = new QAction(QPixmap(cut_xpm), tr("&Cut"), this);
	actionCut->setShortcut( tr("Ctrl+x") );
	connect(actionCut, SIGNAL(activated()), te, SLOT(cut()));	
	edit->addAction(actionCut);
	actionCut->setEnabled(false);

	actionCopy = new QAction(QPixmap(copy_xpm), tr("&Copy"), this);
	actionCopy->setShortcut( tr("Ctrl+C") );
	connect(actionCopy, SIGNAL(activated()), te, SLOT(copy()));	
	edit->addAction(actionCopy);
	actionCopy->setEnabled(false);

	actionPaste = new QAction(QPixmap(paste_xpm), tr("&Paste"), this);
	actionPaste->setShortcut( tr("Ctrl+V") );
	connect(actionPaste, SIGNAL(activated()), te, SLOT(paste()));	
	edit->addAction(actionPaste);

	actionExecute = new QAction(tr("E&xecute"), this);
	actionExecute->setShortcut( tr("CTRL+J") );
	connect(actionExecute, SIGNAL(activated()), te, SLOT(execute()));
	run->addAction(actionExecute);

	actionExecuteAll = new QAction(tr("Execute &All"), this);
	actionExecuteAll->setShortcut( tr("CTRL+SHIFT+J") );
	connect(actionExecuteAll, SIGNAL(activated()), te, SLOT(executeAll()));
	run->addAction(actionExecuteAll);

	actionEval = new QAction(tr("&Evaluate Expression"), this);
	actionEval->setShortcut( tr("CTRL+Return") );
	connect(actionEval, SIGNAL(activated()), te, SLOT(evaluate()));
	run->addAction(actionEval);

	connect(te, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
	connect(te, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));
	connect(te, SIGNAL(undoAvailable(bool)), actionUndo, SLOT(setEnabled(bool)));
	connect(te, SIGNAL(redoAvailable(bool)), actionRedo, SLOT(setEnabled(bool)));
}

void ScriptWindow::languageChange()
{
	setWindowTitle(tr("QtiPlot - Python Script Window"));

	menuBar()->clear();
	menuBar()->addMenu(file);
	menuBar()->addMenu(edit);
	menuBar()->addMenu(run);

	file->setTitle(tr("&File"));
	edit->setTitle(tr("&Edit"));
	run->setTitle(tr("E&xecute"));

	menuBar()->addAction(tr("&Hide"), this, SLOT(close()));

	actionNew->setText(tr("&New"));
	actionNew->setShortcut(tr("Ctrl+N"));

	actionOpen->setText(tr("&Open..."));
	actionOpen->setShortcut(tr("Ctrl+O"));

	actionSave->setText(tr("&Save"));
	actionSave->setShortcut(tr("Ctrl+S"));

	actionSaveAs->setText(tr("Save &As..."));

	actionPrint->setText(tr("&Print"));
	actionPrint->setShortcut(tr("Ctrl+P"));

	actionUndo->setText(tr("&Undo"));
	actionUndo->setShortcut(tr("Ctrl+Z"));

	actionRedo->setText(tr("&Redo"));
	actionRedo->setShortcut(tr("Ctrl+Y"));

	actionCut->setText(tr("&Cut"));
	actionCut->setShortcut(tr("Ctrl+x"));

	actionCopy->setText(tr("&Copy"));
	actionCopy->setShortcut(tr("Ctrl+C"));

	actionPaste->setText(tr("&Paste"));
	actionPaste->setShortcut(tr("Ctrl+V"));

	actionExecute->setText(tr("E&xecute"));
	actionExecute->setShortcut(tr("CTRL+J"));

	actionExecuteAll->setText(tr("Execute &All"));
	actionExecuteAll->setShortcut(tr("CTRL+SHIFT+J"));

	actionEval->setText(tr("&Evaluate Expression"));
	actionEval->setShortcut(tr("CTRL+Return"));
}


void ScriptWindow::newScript()
{
	fileName = QString::null;
	te->clear();
}

void ScriptWindow::open()
{
	QString fn = te->importASCII();
	if (!fn.isEmpty())
		fileName = fn;
}

void ScriptWindow::saveAs()
{
	QString fn = te->exportASCII();
	if (!fn.isEmpty())
		fileName = fn;
}

void ScriptWindow::save()
{
	if (!fileName.isEmpty())
	{
		QFile f(fileName);
		if ( !f.open( QIODevice::WriteOnly ) )
		{
			QMessageBox::critical(0, tr("QtiPlot - File Save Error"),
					tr("Could not write to file: <br><h4> %1 </h4><p>Please verify that you have the right to write to this location!").arg(fileName));
			return;
		}
		QTextStream t( &f );
		t.setCodec("UTF-8");
		t << te->text();
		f.close();
	}
	else
		saveAs();
}

void ScriptWindow::setVisible(bool visible)
{
	if (visible == isVisible())
		return;
	QMainWindow::setVisible(visible);
	emit visibilityChanged(visible);
}
