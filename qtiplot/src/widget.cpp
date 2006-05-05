#include "widget.h"
#include <qmessagebox.h>

myWidget::myWidget(const QString& label, QWidget * parent, const char * name, WFlags f):
		QWidget (parent, name, f)
{
w_label = label;
caption_policy = Both;
askOnClose = true;
}

void myWidget::setCaptionPolicy(CaptionPolicy policy)
{
caption_policy = policy;
switch (caption_policy)
	{
	case Name:
		setCaption(name());
	break;

	case Label:
		if (!w_label.isEmpty())
			setCaption(w_label);
		else
			setCaption(name());
	break;

	case Both:
		if (!w_label.isEmpty())
			setCaption(QString(name()) + " - " + w_label);
		else
			setCaption(name());
	break;
	}
};

void myWidget::closeEvent( QCloseEvent *e )
{
if (askOnClose)
    {
    switch( QMessageBox::information(0,tr("QtiPlot"),
			tr("Do you want to hide or delete") + "<p><b>'" + QString(name()) + "'</b> ?",
				      tr("Delete"), tr("Hide"), tr("Cancel"), 0,2)) 
		{
		case 0:	
			e->accept();
			emit closedWindow(this);
		break;

		case 1:
			e->ignore();
			emit hiddenWindow(this);
		break;

		case 2:
			e->ignore();
		break;
		} 
    }
else 
    {
    e->accept();
    emit closedWindow(this);
    }
}

