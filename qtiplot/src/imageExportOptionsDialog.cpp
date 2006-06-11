/***************************************************************************
    File                 : imageExportDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Image export options dialog
                           
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
#include "imageExportOptionsDialog.h"

#include <QPushButton>
#include <QCheckBox>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QGroupBox>
#include <QImage>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImageWriter>

ImageExportOptionsDialog::ImageExportOptionsDialog( bool exportAllPlots, QWidget* parent, Qt::WFlags fl )
    : QDialog( parent, fl )
{
	int row = 0;
	groupBox1 = new QGroupBox();
	QGridLayout * groupBoxLayout = new QGridLayout( groupBox1 );

	expAll = exportAllPlots;
	if (expAll)
		{
		formatLabel = new QLabel( tr( "Image format" ) );
		groupBoxLayout->addWidget( formatLabel, row, 0 );
	
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
				
		boxFormat= new QComboBox();
		boxFormat->insertStringList (outputFormatList);
		groupBoxLayout->addWidget( boxFormat, row, 1 );
		connect( boxFormat, SIGNAL( activated(int) ), this, SLOT( enableTransparency(int) ) );
		row++;
		}
	
	groupBoxLayout->addWidget( new QLabel(tr( "Image quality" )), row, 0 );
	boxQuality= new QSpinBox();
	boxQuality->setRange(1,100);
	boxQuality->setValue(100);
	groupBoxLayout->addWidget( boxQuality, row, 1 );
	row++;
	
    boxTransparency = new QCheckBox();
	boxTransparency->setText( tr("Save transparency") );
    boxTransparency->setChecked( false );
	boxTransparency->setEnabled( false );
	groupBoxLayout->addWidget( boxTransparency, row, 0 );
	
	QVBoxLayout * rightLayout = new QVBoxLayout();

	buttonOk = new QPushButton();
    buttonOk->setAutoDefault( true );
    buttonOk->setDefault( true );
	rightLayout->addWidget( buttonOk );
   
    buttonCancel = new QPushButton();
    buttonCancel->setAutoDefault( true );
	rightLayout->addWidget( buttonCancel );
	
	QHBoxLayout * mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(groupBox1);
	mainLayout->addLayout(rightLayout);

    languageChange();
   
    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
}

void ImageExportOptionsDialog::languageChange()
{
	setWindowTitle( tr( "QtiPlot - Export options" ) );
	buttonOk->setText( tr( "&OK" ) );
	buttonCancel->setText( tr( "&Cancel" ) );
}

void ImageExportOptionsDialog::enableTransparency(int index)
{
	QString type = boxFormat->itemText(index).toLower();

	if (type == "tif" || type == "tiff" || type == "png" || type == "xpm")
		boxTransparency->setEnabled(true);
	else
		boxTransparency->setEnabled(false);
}

void ImageExportOptionsDialog::enableTransparency()
{
	QString type = f_type.toLower();

	if (type == "tif" || type == "tiff" || type == "png" || type == "xpm")
		boxTransparency->setEnabled(true);
	else
		boxTransparency->setEnabled(false);
}

void ImageExportOptionsDialog::accept()
{
	if (expAll)
		emit exportAll(f_dir, boxFormat->currentText(), boxQuality->value(), boxTransparency->isChecked());
	else
		emit options(f_name, f_type, boxQuality->value(), boxTransparency->isChecked());
	close();
}


ImageExportOptionsDialog::~ImageExportOptionsDialog()
{
}
