#include "widget.h"

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
