/***************************************************************************
    File                 : main.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : SciDAVis main function

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
#include <QApplication>
#include <QAction>
#include "ApplicationWindow.h"
#include <QSplashScreen>
#include <QTimer>

// The following stuff is for the doxygen title page
/*!  \mainpage SciDAVis - Scientific Data Analysis and Visualization - API documentation

  \section description Program description:
SciDAVis is a user-friendly data analysis and visualization program primarily aimed at high-quality plotting of scientific data. It strives to combine an intuitive, easy-to-use graphical user interface with powerful features such as Python scriptability.

 The SciDAVis web page can be found at<br>
 <a href="http://scidavis.sourceforge.net">http://scidavis.sourceforge.net</a><br>
 
All suggestions and contributions are most welcome!<br>
If you want to contribute code, please read the notes on \ref style "coding style" first.
There is also a section with some notes about \ref future "future plans".
<br>

  \section libs SciDAVis uses the following libraries:
  <a href="http://www.trolltech.com/products/qt/index.html">Qt</a>,
  <a href="http://qwt.sourceforge.net/index.html">Qwt</a>,
  <a href="http://qwtplot3d.sourceforge.net/">QwtPlot3D</a>,
  <a href="http://sources.redhat.com/gsl/">GSL</a>,
  <a href="http://muparser.sourceforge.net/">muParser</a>,
  <a href="http://www.zlib.net/">zlib</a>,
  and <a href="http://sourceforge.net/projects/liborigin/">liborigin</a>.
  <br>

  \page future Future Plans

  - Make SciDAVis easier to extend and maintain by modularizing it and add 
    generic plug-in support, support for implementing extensions in Python. 
	For example, ideally it should be possible to implement new plot or marker
    types without modifying existing classes.
  - A full-featured multi-level undo/redo framework (based on QUndoCommand).
	 [ undo/redo assigned to thzs; plug-in/Python extension support assigned to knut_f ]
  - Revise some internal APIs that use undocumented protocols for the
    strings that get passed around. Wherever possible, consistency should be
    checked at compile-time. This would also probably eliminate the need to
    place restrictions on object/column names.
  - Switch file format to a completely XML-based one so we can write a
    more robust parser based on Qt's XML support. Legacy support for the old
    format could be a plug-in. Use the new XML reader/write in Qt 4.3 for 
	this since it is very fast.
  - Document as much as possible using doxygen and write simple demo
    plug-ins. Also a collection of demo projects, tutorials etc. would
	be nice. The latter could be contributed by non-developers.
  - Modularize the build process so that Python support could use SIP's
    build system instead of the current hacks. Support for muParser,
    liborigin and zlib could also be split out into plug-ins on the basis
    that you shouldn't have to install libraries you don't use. Also reduces
    compile times during bugfixing.
	 [ assigned to knut_f ]
  - Change some internal APIs that depend on the notion of a selected/current item.
    This will lead to a cleaner and simpler interface for plug-ins and Python scripts. 
	This will have to wait until the model/view implementation is done since it
	will extend the handling of selections to non-contiguous selections.
  - Replace or fork Qwt by a system based on Qt4's features and optimize it for
    optimum export quality. Very important: Eliminate the restriction to integer
	coordinates and implement true plot scaling.
  - Add support for custom page sizes once Trolltech fixes this issue:
    http://trolltech.com/developer/task-tracker/index_html?method=entry&id=99441
  .

  %Note about modularization: this is mainly about internal reorganizations.
  Most of the current features should remain part of the main executable, but use interfaces similar or
  identical to those used by plug-ins. This should ease maintanance and make adding new features
  to the core application a no-brainer once they're available as plug-ins.
  Support for Python, liborigin and zlib could be real, external plug-ins since many people probably
  don't need them, but this can be decided shortly before distribution and ideally by
  modifying just a few lines in a build script.

  \section features Other Planned Features
  - Waterfall plots
  - Polar charts
  - Ternary plots
  - Improved data analysis: base-line detection tool, more built-in fit functions...
  - Improved import of Origin project files
  - More import/export filters including Open Document spreadsheets (plugin)

  \sa ApplicationWindow, Graph, Layer, Graph3D, ImageMarker, Legend, MyWidget, SelectionMoveResizer, MyParser, Note, ScriptEdit, Table

  \page style Coding Style
  The following rules are not used everywhere (yet), but are intended as guidelines for new code and eventually
  old code should be adapted as well.

  - Files use Unix-style line endings ("\n").
  - Class names start with a capital letter, object/variable names with a lower case letter.
  - Methods and functions are named likeThis(), variables like_this and instance variables are prefixed with "d_".
  - Property access methods use Qt style: property() and setProperty().
  - Every file should be named exactly like the class inside and there should be only one class per file, with the exception of
    really short classes like LayerButton.
  - For indentations, tabs are preferred because they allow everyone to choose the indentation depth for him/herself.
*/

int main( int argc, char ** argv )
{
    QApplication app( argc, argv );

	QStringList args = app.arguments();
	args.removeFirst(); // remove application name

	if( (args.count() == 1) && (args[0] == "-m" || args[0] == "--manual") )
		ApplicationWindow::showStandAloneHelp();
	else if ( (args.count() == 1) && (args[0] == "-a" || args[0] == "--about") ) {
		ApplicationWindow::about();
		exit(0);
	} else {
		QSplashScreen *splash = new QSplashScreen(QPixmap(":/appsplash"));
		splash->show();
		QTimer *timer = new QTimer(&app);
		app.connect( timer, SIGNAL(timeout()), splash, SLOT(close()) );
		app.connect( timer, SIGNAL(timeout()), timer, SLOT(stop()) );
		timer->start(5000);
		ApplicationWindow *mw = new ApplicationWindow();
		mw->applyUserSettings();
		mw->newTable();
		mw->showMaximized();
		mw->savedProject();
		if (mw->autoSearchUpdates){
			mw->autoSearchUpdatesRequest = true;
			mw->searchForUpdates();
		}
		mw->parseCommandLineArguments(args);
	}
	app.connect( &app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );
	return app.exec();
}
