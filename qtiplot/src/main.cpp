#include <qapplication.h>
#include <qaction.h>
 #include "application.h"
 
// The following stuff is for the doxygen title page
/*!  \mainpage QtiPlot - Data analysis and scientific plotting - API 
documentation

  \section description Program description:
  As a scientist I often need to use data analysis and plotting 
software. On Windows systems there is a well known and widely used software 
called <a href="http://www.originlab.com">Origin</a>, which is not free, 
of course.  My purpose was to develop a free (open source), platform 
independent clone of Origin. The result is still far away from it's 
model, but there's a <a class="el" 
href="http://soft.proindependent.com/wishtolist.html">"wish to"</a> list 
which I'm constantly working on. All suggestions and contributions are most 
welcome!</a><br><br>
  
  \section libs QtiPlot uses the following libraries:
  <a href="http://www.trolltech.com/products/qt/index.html">Qt</a>, <a 
href="http://qwt.sourceforge.net/index.html">Qwt (4.2.0)</a>,  <a 
href="http://qwtplot3d.sourceforge.net/">QwtPlot3D</a>,
  <a href="http://sources.redhat.com/gsl/">GSL</a>, 
  <a href="http://muparser.sourceforge.net/">muParser</a>,
  <a href="http://www.zlib.net/">zlib (1.2.3)</a>,
  and <a 
href="http://sourceforge.net/projects/liborigin/">liborigin</a>.
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
		mw->setCaption("QtiPlot - untitled");
		mw->showMaximized();
		mw->saved=TRUE;
		mw->actionSaveProject->setEnabled(false);
		mw->showDonationDialog();
		}

    a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
    int res = a.exec();
    return res;	
}
