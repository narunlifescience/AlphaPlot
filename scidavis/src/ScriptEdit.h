/***************************************************************************
    File                 : ScriptEdit.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Benkert,
                           Knut Franke
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
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

#include "ScriptingEnv.h"
#include "Script.h"

#include <QMenu>
#include <QTextEdit>

class QAction;
class QMenu;

/*!\brief Editor widget with support for evaluating expressions and executing code.
 *
 * \section future_plans Future Plans
 * - Display line numbers.
 * - syntax highlighting, indentation, auto-completion etc. (maybe using QScintilla)
 */
class ScriptEdit: public QTextEdit, public scripted
{
  Q_OBJECT

  public:
    //! Constructor.
    ScriptEdit(ScriptingEnv *env, QWidget *parent=0, QString name="");

	 //! Handle changing of scripting environment.
    void customEvent(QEvent*);
	 //! Map cursor positions to line numbers.
    int lineNumber(int pos) const;

  public slots:
    void execute();
    void executeAll();
    void evaluate();
    void print();
    void exportPDF(const QString& fileName);
    QString exportASCII(const QString &file=QString::null);
    QString importASCII(const QString &file=QString::null);
    void insertFunction(const QString &);
    void insertFunction(QAction * action);
    void setContext(QObject *context) { myScript->setContext(context); }
    void scriptPrint(const QString&);
    void updateIndentation();

  protected:
    virtual void contextMenuEvent(QContextMenuEvent *e);
    virtual void keyPressEvent(QKeyEvent *e);

  private:
	 //! Script used for executing/evaluating code or expressions.
    Script *myScript;
    QAction *actionExecute, *actionExecuteAll, *actionEval, *actionPrint, *actionImport, *actionExport;
	 //! Submenu of context menu with mathematical functions.
    QMenu *functionsMenu;
	 //! Cursor used for output of evaluation results and error messages.
    QTextCursor printCursor;
	 //! Format used for resetting success/failure markers.
	 QTextBlockFormat d_fmt_default;
	 //! Format used for marking code that was executed or evaluated successfully.
	 QTextBlockFormat d_fmt_success;
	 //! Format used for marking code that resulted in an error.
	 QTextBlockFormat d_fmt_failure;
	 //! True iff we are inside evaluate(), execute() or executeAll() there were errors.
	 bool d_error;
	 //! True iff the text is programmatically changed and handleContentsChange() should do nothing.
	 bool d_changing_fmt;

  private slots:
    //! Insert an error message from the scripting system at printCursor.
	 /**
	  * After insertion, the text cursor will have the error message selected, allowing the user to
	  * delete it and fix the error.
	  */
    void insertErrorMsg(const QString &message);
    //! Called whenever the contents of the text document changes.
    void handleContentsChange(int position, int chars_removed, int chars_added);
};

#endif
