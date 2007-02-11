/***************************************************************************
    File                 : main.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : QtiPlot main function
                           
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
#include <qapplication.h>
#include <qaction.h>
 #include "application.h"
 
// The following stuff is for the doxygen title page
/*!  \mainpage QtiPlot - Data analysis and scientific plotting - API documentation

  \section description Program description:
Scientists often need to use data analysis and plotting software. 
For Windows systems there is a well known and widely used software 
called <a href="http://www.originlab.com">Origin</a>, which is not 
free, of course. The purpose of this project is to develop 
a free (open source), platform independent alternative to 
Origin. QtiPlot is still far from this goal, but 
there's a <a class="el" href="http://soft.proindependent.com/wishtolist.html">"wish to"</a> list we are constantly working on. 
All suggestions and contributions are most welcome!
<br>
 
 The QtiPlot web page can be found at<br>
 <a href="http://soft.proindependent.com/qtiplot.html">http://soft.proindependent.com/qtiplot.html</a><br>
  
  \section libs QtiPlot uses the following libraries:
  <a href="http://www.trolltech.com/products/qt/index.html">Qt</a>, 
  <a href="http://qwt.sourceforge.net/index.html">Qwt (5.0.0)</a>,  
  <a href="http://qwtplot3d.sourceforge.net/">QwtPlot3D</a>,
  <a href="http://sources.redhat.com/gsl/">GSL</a>, 
  <a href="http://muparser.sourceforge.net/">muParser</a>,
  <a href="http://www.zlib.net/">zlib (1.2.3)</a>,
  and <a href="http://sourceforge.net/projects/liborigin/">liborigin</a>.
  <br>
*/

int main( int argc, char ** argv ) 
	{
    QApplication a( argc, argv );

	ApplicationWindow * mw;
	if (a.argc() > 1)
		{
		QStringList arg;
		for (int i=1; i < a.argc(); i++)
			arg << QString(a.argv()[i]);

		mw = new ApplicationWindow(arg);
		}
	else
		{
		mw = new ApplicationWindow();
		mw->applyUserSettings();
		mw->newTable();
		mw->showMaximized();
		mw->savedProject();
		mw->showDonationDialog();
		if (mw->autoSearchUpdates)
			{
			mw->autoSearchUpdatesRequest = true;
			mw->searchForUpdates();
			}
		}

    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    int res = a.exec();
    return res;	
}
