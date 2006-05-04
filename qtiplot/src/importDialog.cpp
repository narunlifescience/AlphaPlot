#include "importDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qbuttongroup.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <qspinbox.h>
#include <qcheckbox.h>
#include <qvbox.h>
#include <qwhatsthis.h>

importDialog::importDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "importDialog" );
    setSizeGripEnabled( true );

	QVBox  *box=new QVBox (this, "box2"); 
	box->setMargin (5);
	box->setSpacing (5);

	QHBox  *hbox1=new QHBox (box, "hbox1"); 
	hbox1->setSpacing (5);

	sepText = new QLabel(hbox1, "TextLabel1",0 );
    boxSeparator = new QComboBox(true, hbox1, "boxSeparator" );
	boxSeparator->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

	QString help = tr("The column separator can be customized. The following special codes can be used:\n\\t for a TAB character \n\\s for a SPACE");
	help += "\n"+tr("The separator must not contain the following characters: 0-9eE.+-");
	QWhatsThis::add(boxSeparator, help);
	QWhatsThis::add(sepText, help);

	QHBox  *hbox2=new QHBox (box, "hbox2");
	hbox2->setSpacing (5);

	ignoreLabel = new QLabel(hbox2, "TextLabel2",0 );
    boxLines = new QSpinBox(0,10000, 1, hbox2, "boxLines" );
	boxLines->setSuffix(" " + tr("lines"));
	
    boxRenameCols = new QCheckBox(box, "boxRenameCols" );

	boxStripSpaces = new QCheckBox(box, "boxStripSpaces" );
	help = tr("By checking this option all white spaces will be removed from the beginning and the end of the lines in the ASCII file.");
	help +="\n\n"+tr("Warning: checking this option leads to column overlaping if the columns in the ASCII file don't have the same number of rows.");
	help +="\n"+tr("To avoid this problem you should precisely define the column separator using TAB and SPACE characters.");

	QWhatsThis::add(boxStripSpaces, help);

	boxSimplifySpaces = new QCheckBox(box, "boxSimplifySpaces" );
	help = tr("By checking this option all white spaces will be removed from the beginning and the end of the lines and each sequence of internal whitespaces (including the TAB character) will be replaced with a single space.");
	help +="\n\n"+tr("Warning: checking this option leads to column overlaping if the columns in the ASCII file don't have the same number of rows.");
	help +="\n"+tr("To avoid this problem you should precisely define the column separator using TAB and SPACE characters.");
	QWhatsThis::add(boxSimplifySpaces, help);

	GroupBox2 = new QButtonGroup(3,QGroupBox::Horizontal,QString::null,this,"GroupBox2" );
	GroupBox2->setFlat(TRUE);
	GroupBox2->setLineWidth(0);
	GroupBox2->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
	buttonHelp = new QPushButton(GroupBox2, "buttonHelp" );
	
	QVBoxLayout* hlayout = new QVBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(box);
	hlayout->addWidget(GroupBox2);

    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( help() ) );
}

importDialog::~importDialog()
{
}

void importDialog::help()
{
QString s = tr("The column separator can be customized. The following special codes can be used:\n\\t for a TAB character \n\\s for a SPACE");
s += "\n"+tr("The separator must not contain the following characters: 0-9eE.+-") + "\n\n";
s += tr( "Remove white spaces from line ends" )+ ":\n";
s += tr("By checking this option all white spaces will be removed from the beginning and the end of the lines in the ASCII file.") + "\n\n";
s += tr( "Simplify white spaces" )+ ":\n";
s += tr("By checking this option each sequence of internal whitespaces (including the TAB character) will be replaced with a single space.");

s +="\n\n"+tr("Warning: using these two last options leads to column overlaping if the columns in the ASCII file don't have the same number of rows.");
s +="\n"+tr("To avoid this problem you should precisely define the column separator using TAB and SPACE characters.");

QMessageBox::about(0, tr("QtiPlot - Help"),s);
}

void importDialog::languageChange()
{
    setCaption( tr( "QtiPlot - ASCII Import Options" ) );
    buttonOk->setText(tr("&Apply"));
	buttonCancel->setText( tr("&Close") );	
	buttonHelp->setText( tr( "&Help" ) );

	sepText->setText( tr( "Separator" ));
	boxSeparator->clear();
	boxSeparator->insertItem(tr("TAB"));
    boxSeparator->insertItem(tr("SPACE"));
	boxSeparator->insertItem(";" + tr("TAB"));
	boxSeparator->insertItem("," + tr("TAB"));
	boxSeparator->insertItem(";" + tr("SPACE"));
	boxSeparator->insertItem("," + tr("SPACE"));
    boxSeparator->insertItem(";");
    boxSeparator->insertItem(",");

	boxRenameCols->setText(tr("Use first row to &name columns"));
	boxStripSpaces->setText(tr("&Remove white spaces from line ends"));
	boxSimplifySpaces->setText(tr("&Simplify white spaces" ));
	ignoreLabel->setText( tr( "Ignore first" ));
}

void importDialog::renameCols(bool rename)
{
boxRenameCols->setChecked(rename);	
}

void importDialog::setLines(int lines)
{
boxLines->setValue(lines);
}

void importDialog::setSeparator(const QString& sep)
{
if (sep=="\t")
	boxSeparator->setCurrentItem(0);
else if (sep==" ")
	boxSeparator->setCurrentItem(1);
else if (sep==";\t")
	boxSeparator->setCurrentItem(2);
else if (sep==",\t")
	boxSeparator->setCurrentItem(3);
else if (sep=="; ")
	boxSeparator->setCurrentItem(4);
else if (sep==", ")
	boxSeparator->setCurrentItem(5);
else if (sep==";")
	boxSeparator->setCurrentItem(6);
else if (sep==",")
	boxSeparator->setCurrentItem(7);
else
	{
	QString separator = sep;
	boxSeparator->setCurrentText(separator.replace(" ","\\s").replace("\t","\\t"));
	}
}

void importDialog::setWhiteSpaceOptions(bool strip, bool simplify)
{
boxStripSpaces->setChecked(strip);
boxSimplifySpaces->setChecked(simplify);
}

void importDialog::accept()
{
QString sep = boxSeparator->currentText();
if (boxSimplifySpaces->isChecked())
	sep.replace(tr("TAB"), " ", false);
else
	sep.replace(tr("TAB"), "\t", false);

sep.replace(tr("SPACE"), " ");
sep.replace("\\s", " ");
sep.replace("\\t", "\t");

if (sep.contains(QRegExp("[0-9.eE+-]"))!=0)
	{
	QMessageBox::warning(0, tr("QtiPlot - Import options error"),
				tr("The separator must not contain the following characters: 0-9eE.+-"));
	return;
	}
emit options(sep, boxLines->value(), boxRenameCols->isChecked(),
			  boxStripSpaces->isChecked(), boxSimplifySpaces->isChecked());
}
