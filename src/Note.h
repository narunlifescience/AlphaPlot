/***************************************************************************
    File                 : Note.h
    Project              : AlphaPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Benkert
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Notes window class

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
#ifndef NOTE_H
#define NOTE_H

#include <qtextedit.h>
#include "MyWidget.h"
#include "scripting/ScriptEdit.h"

class ScriptingEnv;
class QXmlStreamWriter;
class XmlStreamReader;

/*!\brief Notes window class.
 *
 * \section future_plans Future Plans
 * - Search and replace
 */
class Note : public MyWidget
{
    Q_OBJECT

public:
    Note(ScriptingEnv *env, const QString &label, QWidget *parent = nullptr,
         const char *name = 0, Qt::WindowFlags f = Qt::SubWindow);
    ~Note();

    void init(ScriptingEnv *env);
    QString getText();

public slots:
    QString saveToString(const QString &info);
    void restore(const QStringList &);

    QTextEdit *textWidget() { return qobject_cast<QTextEdit *>(textedit_); }
    bool autoexec() const { return autoExec; }
    void setAutoexec(bool);
    void modifiedNote();

    // ScriptEdit methods
    QString text() { return textedit_->toPlainText(); }
    void setText(const QString &s) { textedit_->setText(s); }
    void save(QXmlStreamWriter *xmlwriter);
    bool load(XmlStreamReader *xmlreader);
    void print() { textedit_->print(); }
    void exportPDF(const QString &fileName) { textedit_->exportPDF(fileName); }
    QString exportASCII(const QString &file = QString())
    {
        return textedit_->exportASCII(file);
    }
    QString importASCII(const QString &file = QString())
    {
        return textedit_->importASCII(file);
    }
    void execute() { textedit_->execute(); }
    void executeAll() { textedit_->executeAll(); }
    void evaluate() { textedit_->evaluate(); }

private:
    ScriptEdit *textedit_;
    bool autoExec;
};

#endif // NOTE_H
