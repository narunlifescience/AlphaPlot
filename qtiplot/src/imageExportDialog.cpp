/***************************************************************************
    File                 : imageExportDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Image export dialog
                           
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
#include "imageExportDialog.h"

#include <qvariant.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlayout.h>
#include <q3buttongroup.h>
#include <qimage.h>
//Added by qt3to4:
#include <Q3HBoxLayout>

#include <QImageWriter>

imageExportDialog::imageExportDialog( bool exportAllPlots, QWidget* parent, 
																const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    if ( !name )
	setName( "imageExportDialog" );
    setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, 0, 0, sizePolicy().hasHeightForWidth() ) );
    setMinimumSize( QSize( 310, 140 ) );
	setMaximumSize( QSize( 310, 140 ) );
    setMouseTracking( TRUE );
    setSizeGripEnabled( FALSE );
	
	GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""),this,"GroupBox1" );

	expAll = exportAllPlots;
	if (expAll)
		{
		formatLabel = new QLabel( tr( "Image format" ), GroupBox1, "TextLabel11",0 );
	
		QList<QByteArray> list = QImageWriter::supportedImageFormats();
		QByteArray temp;
		QStringList outputFormatList;
		foreach(temp,list)
			outputFormatList.append(QString(temp));

		#ifdef Q_OS_WIN
			;
 		#else
			outputFormatList.prepend("EPS");
		#endif
				
		boxFormat= new QComboBox(GroupBox1, "boxFormat" );
		boxFormat->insertStringList (outputFormatList);
		connect( boxFormat, SIGNAL( activated(int) ), this, SLOT( enableTransparency(int) ) );
		}
	
	new QLabel( tr( "Image quality" ), GroupBox1, "TextLabel1",0 );
	boxQuality= new QSpinBox(0, 100, 1, GroupBox1, "boxQuality" );
	boxQuality->setValue(100);
	
    boxTransparency = new QCheckBox(GroupBox1, "boxTransparency" );
	boxTransparency->setText( "Save transparency" );
    boxTransparency->setChecked( false );
	boxTransparency->setEnabled(false);
	
	GroupBox2 = new Q3ButtonGroup(1,Qt::Horizontal,tr(""),this,"GroupBox2" );
	GroupBox2->setFlat (TRUE);
	
	buttonOk = new QPushButton(GroupBox2, "buttonOk" );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
   
    buttonCancel = new QPushButton(GroupBox2, "buttonCancel" );
    buttonCancel->setAutoDefault( TRUE );
	
	Q3HBoxLayout* hlayout = new Q3HBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(GroupBox1);
	hlayout->addWidget(GroupBox2);

    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void imageExportDialog::languageChange()
{
    setWindowTitle( tr( "QtiPlot - Export options" ) );
    buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
}

void imageExportDialog::enableTransparency(int)
{
QString type = boxFormat->currentText().lower();
	
if (type == "tif" || type == "tiff" || type == "png" || type == "xpm")
	boxTransparency->setEnabled(true);
}

void imageExportDialog::enableTransparency()
{
QString type = f_type.lower();
	
if (type == "tif" || type == "tiff" || type == "png" || type == "xpm")
	boxTransparency->setEnabled(true);
}

void imageExportDialog::accept()
{
if (expAll)
	emit exportAll(f_dir, boxFormat->currentText(), boxQuality->value(), boxTransparency->isChecked());
else
	emit options(f_name, f_type, boxQuality->value(), boxTransparency->isChecked());
close();
}


imageExportDialog::~imageExportDialog()
{
}
