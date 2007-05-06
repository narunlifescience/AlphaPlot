/***************************************************************************
    File                 : LayerDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : ion_vasilief*yahoo.fr, thzs*gmx.net
    Description          : Arrange layers dialog

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
#include "LayerDialog.h"
#include "ApplicationWindow.h"

#include <QLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QGroupBox>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QFontDialog>
#include <QFont>
#include <QMessageBox>

LayerDialog::LayerDialog( QWidget* parent, const char* name, bool modal, Qt::WFlags fl )
: QDialog( parent, name, modal, fl )
{
	if ( !name )
		setName("LayerDialog");
	setWindowTitle(tr( "QtiPlot - Arrange Layers" ));

    QGroupBox *gb1 = new QGroupBox(tr("Layers"));
	QGridLayout *gl1 = new QGridLayout(gb1);
	gl1->addWidget(new QLabel(tr("Number")), 0, 0);
	layersBox = new QSpinBox();
	layersBox->setRange(0, 100);
	gl1->addWidget(layersBox, 0, 1);

	fitBox = new QCheckBox(tr("Automatic &layout"));
	fitBox->setChecked(false);
	gl1->addWidget(fitBox, 1, 1);
	gl1->setRowStretch(2, 1);

    QGroupBox *gb2 = new QGroupBox(tr("Alignement"));
	QGridLayout *gl2 = new QGridLayout(gb2);
	gl2->addWidget(new QLabel(tr("Horizontal")), 0, 0);

	alignHorBox = new QComboBox( false );
	alignHorBox->insertItem( tr( "Center" ) );
	alignHorBox->insertItem( tr( "Left" ) );
	alignHorBox->insertItem( tr( "Right" ) );
	gl2->addWidget(alignHorBox, 0, 1);

	gl2->addWidget(new QLabel( tr( "Vertical" )), 1, 0 );
	alignVertBox = new QComboBox( false );
	alignVertBox->insertItem( tr( "Center" ) );
	alignVertBox->insertItem( tr( "Top" ) );
	alignVertBox->insertItem( tr( "Bottom" ) );
	gl2->addWidget(alignVertBox, 1, 1);
	gl2->setRowStretch(2, 1);

    GroupGrid = new QGroupBox(tr("Grid"));
	QGridLayout *gl3 = new QGridLayout(GroupGrid);
	gl3->addWidget(new QLabel(tr("Columns")), 0, 0);
	boxX = new QSpinBox();
	boxX->setRange(1, 100);
	gl3->addWidget(boxX, 0, 1);
	gl3->addWidget(new QLabel( tr( "Rows" )), 1, 0);
	boxY = new QSpinBox();
	boxY->setRange(1, 100);
	gl3->addWidget(boxY, 1, 1);


	GroupCanvasSize = new QGroupBox(tr("&Layer Canvas Size"));
	GroupCanvasSize->setCheckable(true);
	GroupCanvasSize->setChecked(false);

	QGridLayout *gl5 = new QGridLayout(GroupCanvasSize);
	gl5->addWidget(new QLabel(tr("Width")), 0, 0);
	boxCanvasWidth = new QSpinBox();
	boxCanvasWidth->setRange(0, 10000);
	boxCanvasWidth->setSingleStep(50);
	boxCanvasWidth->setSuffix(tr(" pixels"));
	gl5->addWidget(boxCanvasWidth, 0, 1);
	gl5->addWidget(new QLabel( tr( "Height" )), 1, 0);
	boxCanvasHeight = new QSpinBox();
	boxCanvasHeight->setRange(0, 10000);
	boxCanvasHeight->setSingleStep(50);
	boxCanvasHeight->setSuffix(tr(" pixels"));
	gl5->addWidget(boxCanvasHeight, 1, 1);

    QGroupBox *gb4 = new QGroupBox(tr("Spacing"));
	QGridLayout *gl4 = new QGridLayout(gb4);
	gl4->addWidget(new QLabel(tr("Columns gap")), 0, 0);
	boxColsGap = new QSpinBox();
	boxColsGap->setRange(0, 1000);
	boxColsGap->setSingleStep(5);
	boxColsGap->setSuffix(tr(" pixels"));
	gl4->addWidget(boxColsGap, 0, 1);
	gl4->addWidget(new QLabel( tr( "Rows gap" )), 1, 0);
	boxRowsGap = new QSpinBox();
	boxRowsGap->setRange(0, 1000);
	boxRowsGap->setSingleStep(5);
	boxRowsGap->setSuffix(tr(" pixels"));
	gl4->addWidget(boxRowsGap, 1, 1);
	gl4->addWidget(new QLabel( tr( "Left margin" )), 2, 0);
	boxLeftSpace = new QSpinBox();
	boxLeftSpace->setRange(0, 1000);
	boxLeftSpace->setSingleStep(5);
	boxLeftSpace->setSuffix(tr(" pixels"));
	gl4->addWidget(boxLeftSpace, 2, 1);
	gl4->addWidget(new QLabel( tr( "Right margin" )), 3, 0);
	boxRightSpace = new QSpinBox();
	boxRightSpace->setRange(0, 1000);
	boxRightSpace->setSingleStep(5);
	boxRightSpace->setSuffix(tr(" pixels"));
	gl4->addWidget(boxRightSpace, 3, 1);
	gl4->addWidget(new QLabel( tr( "Top margin" )), 4, 0);
	boxTopSpace = new QSpinBox();
	boxTopSpace->setRange(0, 1000);
	boxTopSpace->setSingleStep(5);
	boxTopSpace->setSuffix(tr(" pixels"));
	gl4->addWidget(boxTopSpace, 4, 1);
	gl4->addWidget(new QLabel( tr( "Bottom margin") ), 5, 0);
	boxBottomSpace = new QSpinBox();
	boxBottomSpace->setRange(0, 1000);
	boxBottomSpace->setSingleStep(5);
	boxBottomSpace->setSuffix(tr(" pixels"));
	gl4->addWidget(boxBottomSpace, 5, 1);

	QVBoxLayout *vbox1 = new QVBoxLayout();
	vbox1->addWidget(GroupGrid);
	vbox1->addWidget(GroupCanvasSize);

	buttonApply = new QPushButton(tr( "&Apply" ));
	buttonOk = new QPushButton(tr( "&OK" ));
	buttonCancel = new QPushButton(tr( "&Cancel" ));

	QHBoxLayout *hbox1 = new QHBoxLayout();
    hbox1->addStretch();
	hbox1->addWidget(buttonApply);
	hbox1->addWidget(buttonOk);
	hbox1->addWidget(buttonCancel);

	QGridLayout *gl6 = new QGridLayout();
	gl6->addWidget(gb1, 0, 0);
	gl6->addWidget(gb2, 0, 1);
	gl6->addLayout(vbox1, 1, 0);
	gl6->addWidget(gb4, 1, 1);
	gl6->setRowStretch(2, 1);

	QVBoxLayout *vbox2 = new QVBoxLayout(this);
	vbox2->addLayout(gl6);
	vbox2->addLayout(hbox1);

	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT(update() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( fitBox, SIGNAL( toggled(bool) ), this, SLOT(enableLayoutOptions(bool) ) );
}

void LayerDialog::enableLayoutOptions(bool ok)
{
	GroupGrid->setEnabled(!ok);
	GroupCanvasSize->setEnabled(!ok);
}

void LayerDialog::setMultiLayer(MultiLayer *g)
{
	multi_layer = g;

	layersBox->setValue(g->layers());
	boxX->setValue(g->getCols());
	boxY->setValue(g->getRows());
	boxColsGap->setValue(g->colsSpacing());
	boxRowsGap->setValue(g->rowsSpacing());
	boxLeftSpace->setValue(g->leftMargin());
	boxRightSpace->setValue(g->rightMargin());
	boxTopSpace->setValue(g->topMargin());
	boxBottomSpace->setValue(g->bottomMargin());
	boxCanvasWidth->setValue(g->layerCanvasSize().width());
	boxCanvasHeight->setValue(g->layerCanvasSize().height());
	alignHorBox->setCurrentItem(g->horizontalAlignement());
	alignVertBox->setCurrentItem(g->verticalAlignement());
}

void LayerDialog::update()
{
        int graphs = layersBox->value();
		int old_graphs = multi_layer->layers();
		int dn = multi_layer->layers() - graphs;
		if (dn > 0 && QMessageBox::question(0, tr("QtiPlot - Delete Layers?"),
					tr("You are about to delete %1 existing layers.").arg(dn)+"\n"+
					tr("Are you sure you want to continue this operation?"),
					tr("&Continue"), tr("&Cancel"), QString(), 0, 1 )) return;

		multi_layer->setLayersNumber(graphs);

		if (!graphs)
			return;

		if (dn < 0)
		{// Customize new layers with user default settings
			ApplicationWindow *app = (ApplicationWindow *)this->parent();
			for (int i = old_graphs+1; i <= graphs; i++)
				app->customGraph(multi_layer->layer(i));
		}

		int cols=boxX->value();
		int rows=boxY->value();

		if (cols>graphs && !fitBox->isChecked())
		{
			QMessageBox::about(0,tr("QtiPlot - Columns input error"),
					tr("The number of columns you've entered is greater than the number of graphs (%1)!").arg(graphs));
			boxX->setFocus();
			return;
		}

		if (rows>graphs && !fitBox->isChecked())
		{
			QMessageBox::about(0,tr("QtiPlot - Rows input error"),
					tr("The number of rows you've entered is greater than the number of graphs (%1)!").arg(graphs));
			boxY->setFocus();
			return;
		}

		if (!fitBox->isChecked())
		{
			multi_layer->setCols(cols);
			multi_layer->setRows(rows);
		}

		if (GroupCanvasSize->isChecked())
			multi_layer->setLayerCanvasSize(boxCanvasWidth->value(), boxCanvasHeight->value());

		multi_layer->setAlignement(alignHorBox->currentItem(), alignVertBox->currentItem());

		multi_layer->setMargins(boxLeftSpace->value(), boxRightSpace->value(),
				boxTopSpace->value(), boxBottomSpace->value());

		multi_layer->setSpacing(boxRowsGap->value(), boxColsGap->value());
		multi_layer->arrangeLayers(fitBox->isChecked(), GroupCanvasSize->isChecked());

		if (!GroupCanvasSize->isChecked())
		{//show new layer canvas size
			boxCanvasWidth->setValue(multi_layer->layerCanvasSize().width());
			boxCanvasHeight->setValue(multi_layer->layerCanvasSize().height());
		}

		if (fitBox->isChecked())
		{//show new grid settings
			boxX->setValue(multi_layer->getCols());
			boxY->setValue(multi_layer->getRows());
		}
}

void LayerDialog::accept()
{
	update();
	close();
}

