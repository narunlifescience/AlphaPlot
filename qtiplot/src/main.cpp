#include <qapplication.h>
#include <qaction.h>
#include "application.h"

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
