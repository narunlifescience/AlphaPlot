#include "exportDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qregexp.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qmessagebox.h>
#include <qwhatsthis.h>

exportDialog::exportDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
		setName( "exportDialog" );
	setSizeGripEnabled( true );

	QVBox *GroupBox1 = new QVBox(this, "GroupBox1" );
	GroupBox1->setSpacing (5);
	GroupBox1->setMargin (5);

	QHBox *hbox1 = new QHBox(GroupBox1, "hbox1");
	hbox1->setSpacing (5);

	new QLabel( tr( "Table" ), hbox1, "TextLabel1", 0 );
	boxTable = new QComboBox( FALSE, hbox1, "boxTable" );
	boxTable->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));

	boxAllTables = new QCheckBox(hbox1, "boxAllTables" );
    boxAllTables->setChecked(false);

    boxNames = new QCheckBox(GroupBox1, "boxNames" );
    boxNames->setChecked( TRUE );
	
    boxSelection = new QCheckBox(GroupBox1, "boxSelection" );
    boxSelection->setChecked( FALSE );
	
	QHBox *hbox2 = new QHBox(GroupBox1, "hbox2");
	hbox2->setSpacing (5);

    QLabel *sepText = new QLabel( tr( "Separator" ), hbox2, "TextLabel3",0 );
    boxSeparator = new QComboBox( true, hbox2, "boxSeparator" );
	boxSeparator->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	
	QString help = tr("The column separator can be customized. The following special codes can be used:\n\\t for a TAB character \n\\s for a SPACE");
	help += "\n"+tr("The separator must not contain the following characters: 0-9eE.+-");
	QWhatsThis::add(boxSeparator, help);
	QWhatsThis::add(sepText, help);

	QHBox *hbox3 = new QHBox(GroupBox1, "hbox3" );
	hbox3->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	hbox3->setSpacing(10);
	hbox3->setMargin (10);
	
	buttonOk = new QPushButton(hbox3, "buttonOk" );
    buttonOk->setDefault( TRUE );
   
    buttonCancel = new QPushButton(hbox3, "buttonCancel" );
	buttonHelp = new QPushButton(hbox3, "buttonHelp" );
	
	QVBoxLayout* hlayout = new QVBoxLayout(this, 5, 5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(hbox3);

    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( help() ) );
	connect( boxAllTables, SIGNAL( toggled(bool) ), this, SLOT( enableTableName(bool) ) );
}

void exportDialog::help()
{
QString s = tr("The column separator can be customized. The following special codes can be used:\n\\t for a TAB character \n\\s for a SPACE");
s += "\n"+tr("The separator must not contain the following characters: 0-9eE.+-");
QMessageBox::about(0, tr("QtiPlot - Help"),s);
}

void exportDialog::languageChange()
{
    setCaption( tr( "QtiPlot - Export ASCII" ) );
    buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
	buttonHelp->setText( tr( "&Help" ) );
    boxNames->setText( tr( "Include Column &Names" ) );
    boxSelection->setText( tr( "Export &Selection" ) );
	boxAllTables->setText( tr( "&All" ) );
    
	boxSeparator->clear();
	boxSeparator->insertItem(tr("TAB"));
    boxSeparator->insertItem(tr("SPACE"));
	boxSeparator->insertItem(";" + tr("TAB"));
	boxSeparator->insertItem("," + tr("TAB"));
	boxSeparator->insertItem(";" + tr("SPACE"));
	boxSeparator->insertItem("," + tr("SPACE"));
    boxSeparator->insertItem(";");
    boxSeparator->insertItem(",");
}

void exportDialog::setTableNames(const QStringList& names)
{
boxTable->insertStringList (names,-1);
}

void exportDialog::setActiveTableName(const QString& name)
{
boxTable->setCurrentText(name);
}

void exportDialog::enableTableName(bool ok)
{
boxTable->setEnabled(!ok);
}

void exportDialog::accept()
{
QString sep = boxSeparator->currentText();
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

hide();
if (boxAllTables->isChecked())
	emit exportAllTables(sep, boxNames->isChecked(), boxSelection->isChecked());
else
	emit exportTable(boxTable->currentText(), sep, 
					 boxNames->isChecked(), boxSelection->isChecked());
close();
}

void exportDialog::setColumnSeparator(const QString& sep)
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

exportDialog::~exportDialog()
{
}
