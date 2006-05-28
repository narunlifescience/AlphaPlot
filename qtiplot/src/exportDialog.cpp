/***************************************************************************
    File                 : exportDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Export ASCII dialog
                           
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
#include "exportDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

ExportDialog::ExportDialog( QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
	setSizeGripEnabled( true );

	QVBoxLayout * groupBox1 = new QVBoxLayout();
	groupBox1->setSpacing(5);
	groupBox1->setMargin(5);

	QHBoxLayout * hbox1 = new QHBoxLayout();
	hbox1->setSpacing (5);
	groupBox1->addLayout( hbox1 );

	hbox1->addWidget( new QLabel(tr("Table")) );
	boxTable = new QComboBox();
	boxTable->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	hbox1->addWidget( boxTable );

	boxAllTables = new QCheckBox();
    boxAllTables->setChecked(false);
	hbox1->addWidget( boxAllTables );

    boxNames = new QCheckBox();
    boxNames->setChecked( true );
	groupBox1->addWidget( boxNames );
	
    boxSelection = new QCheckBox();
    boxSelection->setChecked( false );
	groupBox1->addWidget( boxSelection );
	
	QHBoxLayout * hbox2 = new QHBoxLayout();
	hbox2->setSpacing (5);
	groupBox1->addLayout( hbox2 );

    QLabel * sepText = new QLabel( tr( "Separator" ) );
	hbox2->addWidget( sepText );

    boxSeparator = new QComboBox();
	boxSeparator->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	boxSeparator->setEditable( true );
	hbox2->addWidget( boxSeparator );
	
	QString help = tr("The column separator can be customized. The following special codes can be used:\n\\t for a TAB character \n\\s for a SPACE");
	help += "\n"+tr("The separator must not contain the following characters: 0-9eE.+-");

	boxSeparator->setWhatsThis(help);
	sepText->setWhatsThis(help);
	boxSeparator->setToolTip(help);
	sepText->setToolTip(help);

	QHBoxLayout * hbox3 = new QHBoxLayout();
	hbox3->setSpacing(10);
	hbox3->setMargin(10);
	
	buttonOk = new QPushButton();
    buttonOk->setDefault( true );
	hbox3->addWidget( buttonOk );
   
    buttonCancel = new QPushButton();
	hbox3->addWidget( buttonCancel );
	buttonHelp = new QPushButton();
	hbox3->addWidget( buttonHelp );
	
	QVBoxLayout * hlayout = new QVBoxLayout( this );
    hlayout->addLayout(groupBox1);
	hlayout->addLayout(hbox3);

    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
    connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( help() ) );
	connect( boxAllTables, SIGNAL( toggled(bool) ), this, SLOT( enableTableName(bool) ) );
}

void ExportDialog::help()
{
	QString s = tr("The column separator can be customized. The following special codes can be used:\n\\t for a TAB character \n\\s for a SPACE");
	s += "\n"+tr("The separator must not contain the following characters: 0-9eE.+-");
	QMessageBox::about(0, tr("QtiPlot - Help"),s);
}

void ExportDialog::languageChange()
{
    setWindowTitle( tr( "QtiPlot - Export ASCII" ) );
    buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
	buttonHelp->setText( tr( "&Help" ) );
    boxNames->setText( tr( "Include Column &Names" ) );
    boxSelection->setText( tr( "Export &Selection" ) );
	boxAllTables->setText( tr( "&All" ) );
    
	boxSeparator->clear();
	boxSeparator->addItem(tr("TAB"));
    boxSeparator->addItem(tr("SPACE"));
	boxSeparator->addItem(";" + tr("TAB"));
	boxSeparator->addItem("," + tr("TAB"));
	boxSeparator->addItem(";" + tr("SPACE"));
	boxSeparator->addItem("," + tr("SPACE"));
    boxSeparator->addItem(";");
    boxSeparator->addItem(",");
}

void ExportDialog::setTableNames(const QStringList& names)
{
	boxTable->addItems(names);
}

void ExportDialog::setActiveTableName(const QString& name)
{
	boxTable->setCurrentIndex(boxTable->findText(name));
}

void ExportDialog::enableTableName(bool ok)
{
	boxTable->setEnabled(!ok);
}

void ExportDialog::accept()
{
	QString sep = boxSeparator->currentText();
	sep.replace(tr("TAB"), "\t", Qt::CaseInsensitive);
	sep.replace(tr("SPACE"), " ");
	sep.replace("\\s", " ");
	sep.replace("\\t", "\t");

	if (sep.contains(QRegExp("[0-9.eE+-]")))
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

void ExportDialog::setColumnSeparator(const QString& sep)
{
	if (sep=="\t")
		boxSeparator->setCurrentIndex(0);
	else if (sep==" ")
		boxSeparator->setCurrentIndex(1);
	else if (sep==";\t")
		boxSeparator->setCurrentIndex(2);
	else if (sep==",\t")
		boxSeparator->setCurrentIndex(3);
	else if (sep=="; ")
		boxSeparator->setCurrentIndex(4);
	else if (sep==", ")
		boxSeparator->setCurrentIndex(5);
	else if (sep==";")
		boxSeparator->setCurrentIndex(6);
	else if (sep==",")
		boxSeparator->setCurrentIndex(7);
	else
	{
		QString separator = sep;
		boxSeparator->setItemText(boxSeparator->currentIndex(),separator.replace(" ","\\s").replace("\t","\\t"));
	}
}

ExportDialog::~ExportDialog()
{
}

