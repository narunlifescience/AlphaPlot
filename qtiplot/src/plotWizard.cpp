/***************************************************************************
    File                 : PlotWizard.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : A wizard type dialog to create new plots
                           
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
#include "plotWizard.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QSizePolicy>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>
#include <QPushButton>
#include <QListWidget>
#include <QComboBox>

PlotWizard::PlotWizard( QWidget* parent, Qt::WFlags fl )
: QDialog( parent, fl )
{
	setWindowTitle( tr("QtiPlot - Select Columns to Plot") );

	setSizeGripEnabled( true );

	// top part starts here
	groupBox1 = new QGroupBox( QString() );

	QGridLayout * topLayout = new QGridLayout( groupBox1 );

	topLayout->addWidget(new QLabel(tr( "Worksheet" )), 0, 0);

	boxTables = new QComboBox();
	topLayout->addWidget( boxTables, 0, 2, 1, 2 );

	columnsList = new QListWidget();
	topLayout->addWidget( columnsList, 1, 0, 3, 2 );

	buttonX = new QPushButton();
	buttonX->setText("<->&X");
	topLayout->addWidget( buttonX, 1, 2 );

	buttonXErr = new QPushButton();
	buttonXErr->setText("<->x&Err");
	topLayout->addWidget( buttonXErr, 1, 3 );

	buttonY = new QPushButton();
	buttonY->setText("<->&Y");
	topLayout->addWidget( buttonY, 2, 2 );

	buttonYErr = new QPushButton();
	buttonYErr->setText("<->yE&rr");
	topLayout->addWidget( buttonYErr, 2, 3 );

	buttonZ = new QPushButton();
	buttonZ->setText("<->&Z");
	topLayout->addWidget( buttonZ, 3, 2 );

	buttonNew = new QPushButton();
	buttonNew->setText(tr("&New curve"));
	topLayout->addWidget( buttonNew, 4, 0, 1, 2 );

	buttonDelete = new QPushButton();
	buttonDelete->setText(tr("&Delete curve"));
	topLayout->addWidget( buttonDelete, 4, 2, 1, 2 );

	topLayout->setColumnStretch(1, 1);
	
	// middle part is only one widget
	plotAssociations = new QListWidget();

	// bottom part starts here
	QHBoxLayout * bottomLayout = new QHBoxLayout();

	buttonOk = new QPushButton();
	buttonOk->setText(tr("&Plot"));
	buttonOk->setDefault( true );
	bottomLayout->addWidget( buttonOk );

	buttonCancel = new QPushButton();
	buttonCancel->setText(tr("&Cancel"));
	bottomLayout->addWidget( buttonCancel );

	QVBoxLayout* vlayout = new QVBoxLayout( this );
	vlayout->addWidget( groupBox1 );
	vlayout->addWidget( plotAssociations );
	vlayout->addLayout( bottomLayout );

	// signals and slots connections
	connect( boxTables, SIGNAL(activated(const QString &)),this, SLOT(changeColumnsList(const QString &)));
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( buttonNew, SIGNAL( clicked() ), this, SLOT( addCurve() ) );
	connect( buttonDelete, SIGNAL( clicked() ), this, SLOT( removeCurve() ) );
	connect( buttonX, SIGNAL( clicked() ), this, SLOT(addXCol()));
	connect( buttonY, SIGNAL( clicked() ), this, SLOT(addYCol()));
	connect( buttonXErr, SIGNAL( clicked() ), this, SLOT(addXErrCol()));
	connect( buttonYErr, SIGNAL( clicked() ), this, SLOT(addYErrCol()));
	connect( buttonZ, SIGNAL( clicked() ), this, SLOT(addZCol()));
}

void PlotWizard::accept()
{
	QStringList curves, curves3D, ribbons;
	bool multiple = false;
	int i=0;

	for ( i=0 ; i < plotAssociations->count() ; i++)
	{
		QString text = plotAssociations->item(i)->text();

		if ( text.contains("(Z)") )
		{
			if ( text.contains("(Y)") && !curves3D.contains(text) )
				curves3D << text;
			else if ( !text.contains("(Y)") && ribbons.contains(text) )
				ribbons << text;
		}
		else
		{
			if ( curves.contains(text) )
				multiple = true;
			else
				curves << text;
		}
	}

	if ( multiple )
		QMessageBox::warning(this,tr("QtiPlot - Warning"),
				tr("Redefinitions of the same curve are ignored!"));

	if ( curves.count()>0 )	
		emit plot(curves);

	for( i=0 ; i < curves3D.count() ; i++)
		emit plot3D(curves3D[i]);

	for ( i=0 ; i< ribbons.count() ; i++)
		emit plot3DRibbon(ribbons[i]);

	if( !noCurves() )
		close();
}

void PlotWizard::changeColumnsList(const QString &table)
{	
	QStringList newList;

	for( int i=0 ; i<columns.count() ; i++)
	{
		QString s = columns[i];
		if ( s.contains(table) )					
			newList << s.remove(table+"_");
	}
	setColumnsListBoxContents(newList);
}

void PlotWizard::addXCol()
{
	if (noCurves())
		return;

	QString text = plotAssociations->currentItem()->text();
	if ( text.contains("(X)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You have allready defined a X column!"));
	else
	{
		plotAssociations->currentItem()->setText(text+columnsList->currentItem()->text()+"(X)");
	}
}

void PlotWizard::addYCol()
{
	if (noCurves())
		return;

	QString text = plotAssociations->currentItem()->text();
	if ( !text.contains("(X)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"),tr("You must define a X column first!"));
	else if ( text.contains("(Y)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You have allready defined a Y column!"));
	else
	{
		plotAssociations->currentItem()->setText(text+", "+columnsList->currentItem()->text()+"(Y)");
	}
}

void PlotWizard::addZCol()
{
	if (noCurves())
		return;

	QString text = plotAssociations->currentItem()->text();
	if ( text.contains("(xErr)") || text.contains("(yErr)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("This kind of curve is not handled by QtiPlot!"));
	else if ( !text.contains("(X)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You must define a X column first!"));
	else if ( text.contains("(Z)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You have allready defined a Z column!"));
	else
	{
		plotAssociations->currentItem()->setText(text+", "+columnsList->currentItem()->text()+"(Z)");
	}
}

void PlotWizard::addXErrCol()
{
	if (noCurves())
		return;

	QString text = plotAssociations->currentItem()->text();
	if ( text.contains("(Z)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("This kind of curve is not handled by QtiPlot!"));
	else if ( !text.contains("(X)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You must define a X column first!"));
	else if ( !text.contains("(Y)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You must define a Y column first!"));
	else if ( text.contains("(xErr)") || text.contains("(yErr)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You have allready defined an error-bars column!"));
	else
	{
		plotAssociations->currentItem()->setText(text+", "+columnsList->currentItem()->text()+"(xErr)");
	}
}

void PlotWizard::addYErrCol()
{
	if (noCurves())
		return;

	QString text = plotAssociations->currentItem()->text();
	if ( text.contains("(Z)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("This kind of curve is not handled by QtiPlot!"));
	else if ( !text.contains("(X)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You must define a X column first!"));
	else if ( !text.contains("(Y)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You must define a Y column first!"));
	else if ( text.contains("(xErr)") || text.contains("(yErr)") )
		QMessageBox::warning(this, tr("QtiPlot - Error"), tr("You have allready defined an error-bars column!"));
	else
	{
		plotAssociations->currentItem()->setText(text+", "+columnsList->currentItem()->text()+"(yErr)");
	}
}

void PlotWizard::addCurve()
{
	plotAssociations->addItem( boxTables->currentText()+": " );
	plotAssociations->setCurrentRow( plotAssociations->count()-1 );
}

void PlotWizard::removeCurve()
{
	plotAssociations->takeItem( plotAssociations->currentRow() );
}

void PlotWizard::insertTablesList(const QStringList& tables)
{
	boxTables->addItems(tables);
}

void PlotWizard::setColumnsListBoxContents(const QStringList& cols)
{
	columnsList->clear();
	columnsList->insertItems(0, cols);
	columnsList->setCurrentRow(0);
}

void PlotWizard::setColumnsList(const QStringList& cols)
{
	columns = cols;
}

bool PlotWizard::noCurves()
{
	if ( plotAssociations->count() == 0 )
	{
		QMessageBox::warning(0, tr("QtiPlot - Error"), tr("You must add a new curve first!"));
		return true;
	}
	else
		return false;
}

PlotWizard::~PlotWizard()
{
}

