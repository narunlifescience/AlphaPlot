/***************************************************************************
    File                 : scriptedit.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, 
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : TODO
                           
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
#include "scriptedit.h"
#include "note.h"

#include <qaction.h>
#include <q3popupmenu.h>
#include <qprinter.h>
#include <qpainter.h>
#include <q3paintdevicemetrics.h>
#include <q3simplerichtext.h>

#include <QPrintDialog>
#include <QPrinter>

ScriptEdit::ScriptEdit(ScriptingEnv *env, QWidget *parent, const char *name)
  : QTextEdit(parent, name), scriptEnv(env)
{
	myScript = env->newScript("", this, name);
	connect(myScript, SIGNAL(error(const QString&,const QString&,int)), this, SLOT(insertErrorMsg(const QString&)));

	setWordWrapMode(QTextOption::NoWrap);
	setTextFormat(Qt::PlainText);

	actionExecute = new QAction(tr("E&xecute"), this);
	actionExecute->setShortcut( tr("Ctrl+J") );
	connect(actionExecute, SIGNAL(activated()), this, SLOT(execute()));

	actionExecuteAll = new QAction(tr("Execute &All"), this);
	actionExecuteAll->setShortcut( tr("Ctrl+Shift+J") );
	connect(actionExecuteAll, SIGNAL(activated()), this, SLOT(executeAll()));

	actionEval = new QAction(tr("&Evaluate Expression"), this);
	actionEval->setShortcut( tr("Ctrl+Return") );
	connect(actionEval, SIGNAL(activated()), this, SLOT(evaluate()));

	actionPrint = new QAction(tr("&Print"), this);
	connect(actionPrint, SIGNAL(activated()), this, SLOT(print()));

	//TODO: CTRL+Key_I -> inspect (currently "Open image file". other shortcut?)

	functionsMenu = new QMenu(this);
	Q_CHECK_PTR(functionsMenu);
}

QMenu *ScriptEdit::createStandardContextMenu ()
{
	QMenu *menu = QTextEdit::createStandardContextMenu();
	Q_CHECK_PTR(menu);

	menu->addAction(actionPrint);
	menu->insertSeparator();

	menu->addAction(actionExecute);
	menu->addAction(actionExecuteAll);
	menu->addAction(actionEval);

	if (parent()->isA("Note"))
	{
		Note *sp = (Note*) parent();
		QAction *actionAutoexec = new QAction( tr("Auto&exec"), menu );
		actionAutoexec->setToggleAction(true);
		actionAutoexec->setOn(sp->autoexec());
		connect(actionAutoexec, SIGNAL(toggled(bool)), sp, SLOT(setAutoexec(bool)));
		menu->addAction(actionAutoexec);
	}

	functionsMenu->clear();
	functionsMenu->setTearOffEnabled(true);
	QStringList flist = scriptEnv->mathFunctions();
	for (int i=0; i<flist.size(); i++)
	{
		QAction * newAction = functionsMenu->addAction(flist[i]);
		newAction->setData(i);
		newAction->setWhatsThis(scriptEnv->mathFunctionDoc(flist[i]));
		connect(functionsMenu, SIGNAL(triggered(QAction *)), this, SLOT(insertFunction(QAction *)));
	}
	functionsMenu->setTitle(tr("&Functions"));
	menu->addMenu(functionsMenu);

	return menu;
}

void ScriptEdit::insertErrorMsg(const QString &message)
{
	QString err = message;
	err.prepend("\n").replace("\n","\n#> ");
	textCursor().movePosition(QTextCursor::End);
	textCursor().insertText(err);
}

void ScriptEdit::insertFunction(const QString &fname)
{
	QTextCursor cursor = textCursor();
	QString markedText = cursor.selectedText();
	cursor.insertText(fname+"("+markedText+")");
	if(markedText.isEmpty())
	{
		// if no text is selected, place cursor inside the ()
		// instead of after it
		cursor.movePosition(QTextCursor::PreviousCharacter,QTextCursor::MoveAnchor,1);
		// the next line makes the selection visible to the user 
		// (the line above only changes the selection in the
		// underlying QTextDocument)
		setTextCursor(cursor);
	}
}

void ScriptEdit::insertFunction(QAction *action)
{
	insertFunction(scriptEnv->mathFunctions()[action->data().toInt()]);
}

void ScriptEdit::execute()
{
	// FIXME: see FIXME below
	// int paraFrom, indexFrom, paraTo, indexTo;
	QString fname = "<%1:%2>";
	fname = fname.arg(name());
	QTextCursor cursor = textCursor();
	if (cursor.selectedText().isEmpty())
	{
		cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
		cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
	}
	setTextCursor(cursor);
	// FIXME: thzs: Don't know the meaning of paraFrom+1
	// therefore cannot port the next two lines to Qt4
	// getSelection(&paraFrom, &indexFrom, &paraTo, &indexTo);
	// fname = fname.arg(paraFrom+1);
	
	// dummy line
	fname = fname.arg(cursor.position()+1);

	myScript->setName(fname);
	myScript->setCode(cursor.selectedText());
	myScript->exec();
}

void ScriptEdit::executeAll()
{
	QString fname = "<%1>";
	fname = fname.arg(name());
	myScript->setName(fname);
	myScript->setCode(text());
	myScript->exec();
}

void ScriptEdit::evaluate()
{
	// FIXME: see FIXME below
	// int paraFrom, indexFrom, paraTo, indexTo;
	QString fname = "<%1:%2>";
	fname = fname.arg(name());
	QTextCursor cursor = textCursor();
	if (cursor.selectedText().isEmpty())
	{
		cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
		cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
	}
	setTextCursor(cursor);
	// FIXME: thzs: Don't know the meaning of paraFrom+1
	// therefore cannot port the next two lines to Qt4
	// getSelection(&paraFrom, &indexFrom, &paraTo, &indexTo);
	// fname = fname.arg(paraFrom+1);
	
	// dummy line
	fname = fname.arg(cursor.position()+1);

	myScript->setName(fname);
	myScript->setCode(selectedText());
	myScript->setEmitErrors(false);
	QVariant res = myScript->eval();
	myScript->setEmitErrors(true);
	if (res.isValid() && res.canCast(QVariant::String))
	{
		cursor.insertText("\n#> "+res.toString()+"\n");
		cursor.clearSelection();
	} else { // statement or invalid
		if (myScript->exec())
		{
			cursor.clearSelection();
			cursor.insertText("\n");
			cursor.clearSelection();
		}
	}
}

ScriptEdit::~ScriptEdit()
{
}

void ScriptEdit::print()
{
	QTextDocument *doc = document();
	QPrinter printer;
	printer.setColorMode(QPrinter::GrayScale);
	QPrintDialog printDialog(&printer);
	// TODO: Write a dialog to use more features of Qt4's QPrinter class
	if (printDialog.exec() == QDialog::Accepted) 
	{
		doc->print(&printer);
	}

	// old code (Qt3):
	/*
	   QPrinter printer;
	   printer.setColorMode (QPrinter::GrayScale);
	   if (printer.setup()) 
	   {
	   printer.setFullPage( TRUE );
	   QPainter painter;
	   if ( !painter.begin(&printer ) )
	   return;

	   QPaintDeviceMetrics metrics( painter.device() );
	   int dpiy = metrics.logicalDpiY();
	   int margin = (int) ( (1/2.54)*dpiy ); // 1 cm margins
	   QRect body( margin, margin, metrics.width() - 2*margin, metrics.height() - 2*margin );
	   QSimpleRichText richText(QStyleSheet::convertFromPlainText(text()), QFont(), 
	   context(), styleSheet(), mimeSourceFactory(), body.height());
	   richText.setWidth( &painter, body.width() );
	   QRect view( body );
	   int page = 1;
	   do {
	   richText.draw( &painter, body.left(), body.top(), view, colorGroup() );
	   view.moveBy( 0, body.height() );
	   painter.translate( 0 , -body.height() );
	   painter.drawText( view.right() - painter.fontMetrics().width( QString::number( page ) ),
	   view.bottom() + painter.fontMetrics().ascent() + 5, QString::number( page ) );
	   if ( view.top()  >= richText.height() )
	   break;
	   printer.newPage();
	   page++;
	   } 
	   while (TRUE);
	   }
	   */
}

