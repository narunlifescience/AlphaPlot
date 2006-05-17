#include "renameWindowDialog.h"
#include "application.h"
#include "worksheet.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qlineedit.h>
#include <qtextedit.h>
#include <qradiobutton.h>
#include <qmessagebox.h>
#include <qaction.h>
#include <qregexp.h>

renameWindowDialog::renameWindowDialog(QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "renameWindowDialog" );
		
	setCaption(tr("QtiPlot - Rename Window"));
	
	GroupBox1 = new QButtonGroup( 2, QGroupBox::Horizontal,tr("Window Title"),this,"GroupBox1" );

	boxName = new QRadioButton(tr("&Name (single word)"), GroupBox1, "boxName" );
	boxNameLine = new QLineEdit(GroupBox1, "boxNameLine");
	setFocusProxy(boxNameLine);

	boxLabel = new QRadioButton(tr("&Label"), GroupBox1, "boxLabel" );
	boxLabelEdit = new QTextEdit(GroupBox1, "boxLabelEdit");
	boxLabelEdit->setMaximumHeight(100);

	boxBoth = new QRadioButton(tr("&Both Name and Label"), GroupBox1, "boxBoth" );
	
	GroupBox2 = new QButtonGroup(1,QGroupBox::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	GroupBox2->setLineWidth (0);
	
	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
	
	QHBoxLayout* hlayout = new QHBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(GroupBox2);

    languageChange();

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void renameWindowDialog::setWidget(myWidget *w)
{
window = w;
boxNameLine->setText(w->name());
boxLabelEdit->setText(w->windowLabel());
switch (w->captionPolicy())
	{
	case myWidget::Name:
		boxName->setChecked(true);
	break;

	case myWidget::Label:
		boxLabel->setChecked(true);
	break;

	case myWidget::Both:
		boxBoth->setChecked(true);
	break;
	}
}

myWidget::CaptionPolicy renameWindowDialog::getCaptionPolicy()
{
myWidget::CaptionPolicy policy = myWidget::Name;
if (boxLabel->isChecked())
	policy = myWidget::Label;
else if (boxBoth->isChecked())
	policy = myWidget::Both;
		
return policy;
}

renameWindowDialog::~renameWindowDialog()
{
}

void renameWindowDialog::languageChange()
{
buttonOk->setText( tr( "&OK" ) );
buttonCancel->setText( tr( "&Cancel" ) );
}

void renameWindowDialog::accept()
{
QString name = window->name();
QString text = boxNameLine->text().remove("_").remove("=").remove(QRegExp("\\s"));
QString label = boxLabelEdit->text();

myWidget::CaptionPolicy policy = getCaptionPolicy();
if (text == name && label == window->windowLabel() && window->captionPolicy() == policy)
	close();

ApplicationWindow *app = (ApplicationWindow *)parentWidget();
if (!app)
	return;

if (text != name)
	{
	if(!app->renameWindow(window, text))
		return;

	app->renameListViewItem(name,text);
	}

label.replace("\n"," ").replace("\t"," ");
window->setWindowLabel(label);
window->setCaptionPolicy(policy);
app->setListViewLabel(window->name(), label);
app->modifiedProject(window);
close();
}
