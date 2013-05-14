/***************************************************************************
    File                 : globals.cpp
    Description          : Definition of global constants and enums
    --------------------------------------------------------------------
    Copyright            : (C) 2006-2009 Tilman Benkert (thzs*gmx.net)
    Copyright            : (C) 2006-2007 Ion Vasilief (ion_vasilief*yahoo.fr)
                           (replace * with @ in the email addresses) 

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

#include "globals.h"
#include <QMessageBox>
#include <QIcon>
#include <QObject>
#include <QMetaObject>
#include <QMetaEnum>
#include <QtDebug>
#include "ui_SciDAVisAbout.h"

//  Don't forget to change the Doxyfile, the splash screen and the Windows
//  installer when changing these!
const int SciDAVis::scidavis_version = 0x000204;

const char * SciDAVis::extra_version = "";

const char * SciDAVis::copyright_string = "\
=== Credits ===\n\
\n\
--- Developers ---\n\
\n\
The following people have written parts of the SciDAVis source code, ranging from a few lines to large chunks.\n\
In alphabetical order.\n\
\n\
Tilman Benkert[1], Knut Franke\n\
\n\
--- Documentation ---\n\
\n\
The following people have written parts of the manual and/or other documentation.\n\
In alphabetical order.\n\
\n\
Knut Franke, Roger Gadiou\n\
\n\
--- Translations ---\n\
\n\
The following people have contributed translations or parts thereof.\n\
In alphabetical order.\n\
\n\
Tilman Benkert[1], Markus Bongard, Tobias Burnus, Rémy Claverie, f0ma, José Antonio Lorenzo Fernández, \
Pavel Fric, Jan Helebrant, Daniel Klaer, Peter Landgren, Fellype do Nascimento, Tomomasa Ohkubo, \
Mikhail Shevyakov, Mauricio Troviano\n\
\n\
--- Packagers ---\n\
\n\
The following people have made installing SciDAVis easier by providing specialized binary packages.\n\
In alphabetical order.\n\
\n\
Burkhard Bunk (Debian), Quentin Denis (SUSE), Yu-Hung Lien (Mac OS X), Fellype do Nascimento (Slackware), \
Eric Tanguy (Fedora), Mauricio Troviano (Windows installer)\n\
\n\
--- QtiPlot ---\n\
\n\
SciDAVis uses code from QtiPlot, which consisted (at the time of the fork, i.e. QtiPlot 0.9-rc2) of code by the following people:\n\
\n\
Tilman Benkert[1], Shen Chen, Borries Demeler, José Antonio Lorenzo Fernández, Knut Franke, Vasileios Gkanis, Gudjon Gudjonsson, \
Alex Kargovsky, Michael Mac-Vicar, Tomomasa Ohkubo, Aaron Van Tassle, Branimir Vasilic, Ion Vasilief, Vincent Wagelaar\n\
\n\
The SciDAVis manual is based on the QtiPlot manual, written by (in alphabetical order):\n\
\n\
Knut Franke, Roger Gadiou, Ion Vasilief\n\
\n\
footnotes:\n\
[1] birth name: Tilman Höner zu Siederdissen\n\
\n\
=== Special Thanks ===\n\
\n\
We also want to acknowledge the people having helped us indirectly by contributing to the following\n\
fine pieces of software. In no particular order.\n\
\n\
Qt (http://doc.trolltech.com/4.3/credits.html),\n\
Qwt (http://qwt.sourceforge.net/#credits),\n\
Qwtplot3D (http://qwtplot3d.sourceforge.net/),\n\
muParser (http://muparser.sourceforge.net/),\n\
Python (http://www.python.org/),\n\
liborigin (http://sourceforge.net/projects/liborigin/),\n\
Vim (http://www.vim.org/thanks.php/),\n\
webgen (http://webgen.rubyforge.org/),\n\
Doxygen (http://www.doxygen.org/),\n\
Subversion (http://subversion.tigris.org/),\n\
GSL (http://www.gnu.org/software/gsl/)\n\
\n\
... and many more we just forgot to mention.\n";

const char * SciDAVis::release_date = " 2010-03-12";

int SciDAVis::version()
{
	return scidavis_version;
}

QString SciDAVis::versionString()
{
	return "SciDAVis " + 
			QString::number((scidavis_version & 0xFF0000) >> 16)+"."+ 
			QString::number((scidavis_version & 0x00FF00) >> 8)+"."+
			QString::number(scidavis_version & 0x0000FF);
}
			
QString SciDAVis::extraVersion()
{
	return	QString(extra_version);
}

void SciDAVis::about()
{
	QString text = QString(SciDAVis::copyright_string);
	text.replace(QRegExp("\\[1\\]"), "<sup>1</sup>");
	text.replace("é","&eacute;");
	text.replace("á","&aacute;");
	text.replace("ö", "&ouml;");
	text.replace("\n", "<br>");
	text.replace("=== ", "<h1>");
	text.replace(" ===","</h1>");
	text.replace("--- ", "<h2>");
	text.replace(" ---","</h2>");
	text.replace(" ---","</h2>");
	text.replace("</h1><br><br>", "</h1>");
	text.replace("</h2><br><br>", "</h2>");
	text.replace("<br><h1>", "<h1>");
	text.replace("<br><h2>", "<h2>");

	Qt::WindowFlags flags = Qt::WindowTitleHint | Qt::WindowSystemMenuHint;
#if QT_VERSION >= 0x040500
	flags |= Qt::WindowCloseButtonHint;
#endif
	QDialog *dialog = new QDialog(0, flags);
	Ui::SciDAVisAbout ui;
	ui.setupUi(dialog);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
	dialog->setWindowTitle(QObject::tr("About SciDAVis"));
	ui.version_label->setText(versionString() + extraVersion());
	ui.release_date_label->setText(QObject::tr("Released") + ": " + QString(SciDAVis::release_date));
	ui.credits_box->setHtml(text);

	dialog->exec();
}

QString SciDAVis::copyrightString()
{
	return copyright_string;
}

QString SciDAVis::releaseDateString()
{
	return release_date;
}

QString SciDAVis::enumValueToString(int key, const QString& enum_name)
{
	int index = staticMetaObject.indexOfEnumerator(enum_name.toAscii());
	if(index == -1) return QString("invalid");
	QMetaEnum meta_enum = staticMetaObject.enumerator(index);
	return QString(meta_enum.valueToKey(key));
}

int SciDAVis::enumStringToValue(const QString& string, const QString& enum_name)
{
	int index = staticMetaObject.indexOfEnumerator(enum_name.toAscii());
	if(index == -1) return -1;
	QMetaEnum meta_enum = staticMetaObject.enumerator(index);
	return meta_enum.keyToValue(string.toAscii());
}

