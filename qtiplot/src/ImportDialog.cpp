/***************************************************************************
    File                 : ImportDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : ASCII import options dialog
                           
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
#include "ImportDialog.h"
#include "ApplicationWindow.h"

#include <QMessageBox>
#include <QRegExp>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>


ImportDialog::ImportDialog( QWidget* parent, Qt::WFlags fl )
: QDialog( parent, fl )
{
	setWindowTitle( tr( "QtiPlot - ASCII Import Options" ) );
	setSizeGripEnabled( true );
	
	QGridLayout * mainLayout = new QGridLayout(this);
	QHBoxLayout * l1 = new QHBoxLayout();
	QHBoxLayout * l2 = new QHBoxLayout();

	sepText = new QLabel(tr( "Separator"));
	l1->addWidget( sepText );

	boxSeparator = new QComboBox();
	boxSeparator->addItem(tr("TAB"));
	boxSeparator->addItem(tr("SPACE"));
	boxSeparator->addItem(";" + tr("TAB"));
	boxSeparator->addItem("," + tr("TAB"));
	boxSeparator->addItem(";" + tr("SPACE"));
	boxSeparator->addItem("," + tr("SPACE"));
	boxSeparator->addItem(";");
	boxSeparator->addItem(",");
	
	boxSeparator->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
	boxSeparator->setEditable( true );
	l1->addWidget( boxSeparator );
	mainLayout->addLayout( l1, 0, 0, 1, 4 );

	QString help = tr("The column separator can be customized. \nThe following special codes can be used:\n\\t for a TAB character \n\\s for a SPACE");
	help += "\n"+tr("The separator must not contain the following characters: \n0-9eE.+-");

	boxSeparator->setWhatsThis(help);
	sepText->setWhatsThis(help);
	boxSeparator->setToolTip(help);
	sepText->setToolTip(help);

	ignoreLabel = new QLabel(tr( "Ignore first" ));
	l2->addWidget( ignoreLabel );

	boxLines = new QSpinBox();
	boxLines->setRange( 0, 10000 );
	boxLines->setSuffix(" " + tr("lines"));
	boxLines->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
	l2->addWidget( boxLines );
	mainLayout->addLayout( l2, 1, 0, 1, 4 );

	boxRenameCols = new QCheckBox(tr("Use first row to &name columns"));
	mainLayout->addWidget( boxRenameCols, 2, 0, 1, 4 );
 
	boxStripSpaces = new QCheckBox(tr("&Remove white spaces from line ends"));

	help = tr("By checking this option all white spaces will be \nremoved from the beginning and the end of \nthe lines in the ASCII file.","when translating this check the what's this functions and tool tips to place the '\\n's correctly");
	help +="\n\n"+tr("Warning: checking this option leads to column \noverlaping if the columns in the ASCII file don't \nhave the same number of rows.");
	help +="\n"+tr("To avoid this problem you should precisely \ndefine the column separator using TAB and \nSPACE characters.","when translating this check the what's this functions and tool tips to place the '\\n's correctly");

	boxStripSpaces->setWhatsThis(help);
	boxStripSpaces->setToolTip(help);
	mainLayout->addWidget( boxStripSpaces, 3, 0, 1, 4 );

	boxSimplifySpaces = new QCheckBox(tr("&Simplify white spaces" ));

	help = tr("By checking this option all white spaces will be \nremoved from the beginning and the end of the \nlines and each sequence of internal \nwhitespaces (including the TAB character) will \nbe replaced with a single space.","when translating this check the what's this functions and tool tips to place the '\\n's correctly");
	help +="\n\n"+tr("Warning: checking this option leads to column \noverlaping if the columns in the ASCII file don't \nhave the same number of rows.","when translating this check the what's this functions and tool tips to place the '\\n's correctly");
	help +="\n"+tr("To avoid this problem you should precisely \ndefine the column separator using TAB and \nSPACE characters.","when translating this check the what's this functions and tool tips to place the '\\n's correctly");

	boxSimplifySpaces->setWhatsThis(help);
	boxSimplifySpaces->setToolTip(help);
	mainLayout->addWidget( boxSimplifySpaces, 4, 0, 1, 4 );

	buttonHelp = new QPushButton(tr( "&Help" ));
	mainLayout->addWidget( buttonHelp, 5, 0 );

	buttonOk = new QPushButton(tr("&Apply"));
	buttonOk->setDefault( true );
	mainLayout->addWidget( buttonOk, 5, 1 );
	buttonOk->setEnabled( false );

	buttonCancel = new QPushButton(tr("&Close"));
	mainLayout->addWidget( buttonCancel, 5, 2 );

	mainLayout->setColumnStretch( 3, 1 );

	ApplicationWindow *app = (ApplicationWindow *)parent;
	boxStripSpaces->setChecked(app->strip_spaces);
	boxSimplifySpaces->setChecked(app->simplify_spaces);
	boxLines->setValue(app->ignoredLines);
	boxRenameCols->setChecked(app->renameColumns);	
	setSeparator(app->columnSeparator);
	
	resize(minimumSize());
	
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( quit() ) );
	connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( help() ) );
	connect(boxSimplifySpaces, SIGNAL(stateChanged (int)), this, SLOT(enableApplyButton(int)));
	connect(boxStripSpaces, SIGNAL(stateChanged (int)), this, SLOT(enableApplyButton(int)));
	connect(boxRenameCols, SIGNAL(stateChanged (int)), this, SLOT(enableApplyButton(int)));
	connect(boxLines, SIGNAL(valueChanged (int)), this, SLOT(enableApplyButton(int)));
	connect(boxSeparator, SIGNAL(activated (int)), this, SLOT(enableApplyButton(int)));	
	connect(boxSeparator, SIGNAL(editTextChanged(const QString &)), this, SLOT(enableApplyButton(const QString &)));
}

ImportDialog::~ImportDialog()
{
}

void ImportDialog::help()
{
	QString s = tr("The column separator can be customized. The following special codes can be used:\n\\t for a TAB character \n\\s for a SPACE");
	s += "\n"+tr("The separator must not contain the following characters: 0-9eE.+-") + "\n\n";
	s += tr( "Remove white spaces from line ends" )+ ":\n";
	s += tr("By checking this option all white spaces will be removed from the beginning and the end of the lines in the ASCII file.") + "\n\n";
	s += tr( "Simplify white spaces" )+ ":\n";
	s += tr("By checking this option each sequence of internal whitespaces (including the TAB character) will be replaced with a single space.");
	s += tr("By checking this option all white spaces will be removed from the beginning and the end of the lines and each sequence of internal whitespaces (including the TAB character) will be replaced with a single space.");

	s +="\n\n"+tr("Warning: using these two last options leads to column overlaping if the columns in the ASCII file don't have the same number of rows.");
	s +="\n"+tr("To avoid this problem you should precisely define the column separator using TAB and SPACE characters.");

	QMessageBox::about(0, tr("QtiPlot - Help"),s);
}

void ImportDialog::setSeparator(const QString& sep)
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
		boxSeparator->setEditText(separator.replace(" ","\\s").replace("\t","\\t"));
	}
}

void ImportDialog::accept()
{
	QString sep = boxSeparator->currentText();
	
	if (boxSimplifySpaces->isChecked())
		sep.replace(tr("TAB"), " ", Qt::CaseInsensitive);
	else
		sep.replace(tr("TAB"), "\t", Qt::CaseInsensitive);

	sep.replace(tr("SPACE"), " ", Qt::CaseInsensitive);
	sep.replace("\\s", " ");
	sep.replace("\\t", "\t");
		
	if (sep.contains(QRegExp("[0-9.eE+-]")))
	{
		QMessageBox::warning(0, tr("QtiPlot - Import options error"),
				tr("The separator must not contain the following characters: 0-9eE.+-"));
	}
	else
	{
		ApplicationWindow *app = (ApplicationWindow *)this->parent();
		app->setImportOptions(sep, boxLines->value(), boxRenameCols->isChecked(),
				boxStripSpaces->isChecked(), boxSimplifySpaces->isChecked());

		buttonOk->setEnabled( false );
	}
}

void ImportDialog::quit()
{
	if (buttonOk->isEnabled())
	{
		switch( QMessageBox::information((QWidget *)this->parent(), tr("QtiPlot"),
					tr("Do you want to save the modifications to the ASCII import options before closing?"),
					tr("Yes"), tr("No"), tr("Cancel"), 0, 2)) 
		{
			case 0:
				accept();
				close();
				break;

			case 1:
				close();
				break;

			case 2:
				break;
		} 
	}
	else
		close();
}

void ImportDialog::enableApplyButton(int)
{
	buttonOk->setEnabled(true);
}

void ImportDialog::enableApplyButton(const QString &)
{
	buttonOk->setEnabled(true);
}
