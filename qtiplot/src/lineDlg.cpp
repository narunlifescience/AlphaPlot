/***************************************************************************
    File                 : lineDlg.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Line options dialog
                           
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
#include "lineDlg.h"
#include "colorButton.h"
#include "LineMarker.h"
#include "graph.h"
#include "plot.h"
#include "application.h"

#include <qwt_plot.h>

#include <qspinbox.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <q3buttongroup.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qcolordialog.h>
#include <qtabwidget.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3HBoxLayout>
#include <Q3HBox>

#include <QLineEdit>

LineDialog::LineDialog( LineMarker *line, QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    setWindowTitle( tr( "QtiPlot - Line options" ) );

	tw = new QTabWidget( this );
	options = new QWidget( tw );
    GroupBox1 = new Q3ButtonGroup( 2,Qt::Horizontal, QString(),options,"GroupBox1" );

	new QLabel(tr( "Color" ), GroupBox1);
	colorBox = new ColorButton(GroupBox1);
	colorBox->setColor(lm->color());

	new QLabel(tr( "Line type" ),GroupBox1);
    styleBox = new QComboBox( FALSE, GroupBox1);
	styleBox->insertItem("_____");
	styleBox->insertItem("- - -");
	styleBox->insertItem(".....");
	styleBox->insertItem("_._._");
	styleBox->insertItem("_.._..");
	
	setLineStyle(lm->style());

	new QLabel(tr( "Line width" ),GroupBox1);
    widthBox = new QComboBox( FALSE, GroupBox1, "widthBox" );
	widthBox->insertItem( tr( "1" ) );
    widthBox->insertItem( tr( "2" ) );
    widthBox->insertItem( tr( "3" ) );
    widthBox->insertItem( tr( "4" ) );
    widthBox->insertItem( tr( "5" ) );
	widthBox->setEditable (true);
	widthBox->setCurrentItem(0);
	widthBox->setEditText(QString::number(lm->width()));

	startBox = new QCheckBox(GroupBox1); 
    startBox->setText( tr( "Arrow at &start" ) );
	startBox->setChecked(lm->getStartArrow());

	endBox = new QCheckBox(GroupBox1, "endBox" );
    endBox->setText( tr( "Arrow at &end" ) );
	endBox->setChecked(lm->getEndArrow());

	Q3HBoxLayout* hl1 = new Q3HBoxLayout(options,5,5, "hl1");
    hl1->addWidget(GroupBox1);
	
	tw->insertTab(options, tr( "Opti&ons" ) );
	
	head = new QWidget( tw, "head" );
    Q3ButtonGroup *GroupBox4 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""),head,"GroupBox4" );

	new QLabel(tr( "Length" ), GroupBox4, "TextLabel111",0);
	boxHeadLength = new QSpinBox( 0,100,1,GroupBox4, "boxHeadLength" );
	boxHeadLength->setValue(lm->headLength());

	new QLabel(tr( "Angle" ),GroupBox4, "TextLabel1112",0 );
	boxHeadAngle = new QSpinBox(0,85,5,GroupBox4, "boxHeadAngle" );
	boxHeadAngle->setValue(lm->headAngle());

	filledBox = new QCheckBox(GroupBox4, "filledBox" ); 
    filledBox->setText( tr( "&Filled" ) );
	filledBox->setChecked(lm->filledArrowHead());

	Q3HBoxLayout* hl3 = new Q3HBoxLayout(head,5,5, "hl3");
    hl3->addWidget(GroupBox4);

	tw->insertTab(head, tr( "Arrow &Head" ) );

	initGeometryTab();

	GroupBox2 = new Q3ButtonGroup(4,Qt::Horizontal, QString::null,this,"GroupBox2" );
	
	buttonDefault = new QPushButton( GroupBox2);
	buttonDefault->setText( tr( "Set &Default" ) );
	
	btnApply = new QPushButton(GroupBox2, "btnApply" );
    btnApply->setText( tr( "&Apply" ) );

	btnOk = new QPushButton(GroupBox2, "btnOk" );
    btnOk->setText( tr( "&Ok" ) );
	
	Q3VBoxLayout* hlayout = new Q3VBoxLayout(this,5,5, "hlayout");
    hlayout->addWidget(tw);
	hlayout->addWidget(GroupBox2);

	enableHeadTab();

	connect( colorBox, SIGNAL( clicked() ), this, SLOT(pickColor() ) );
	connect( btnOk, SIGNAL( clicked() ), this, SLOT(accept() ) );
	connect( btnApply, SIGNAL( clicked() ), this, SLOT(apply() ) );
	connect( tw, SIGNAL(currentChanged (QWidget *)), this, SLOT(enableButtonDefault(QWidget *)));
	connect( buttonDefault, SIGNAL(clicked()), this, SLOT(setDefaultValues()));
}

void LineDialog::initGeometryTab()
{
	geometry = new QWidget( tw );

	Q3HBox *box1 = new Q3HBox (geometry);
	box1->setSpacing(5);
	box1->setMargin(5);
	
	new QLabel(tr( "Unit" ), box1);
	unitBox = new QComboBox(box1);
	unitBox->insertItem(tr("Pixels"));
	unitBox->insertItem(tr("Scale Coordinates"));

	Q3HBox *box2 = new Q3HBox (geometry);
	box2->setSpacing(5);

    Q3ButtonGroup *GroupBox2 = new Q3ButtonGroup( 2,Qt::Horizontal,tr("Start Point"),box2);

	new QLabel(tr( "X" ), GroupBox2, "TextLabel11",0);
	xStartBox = new QLineEdit(GroupBox2);
	
	new QLabel(tr( "Y" ),GroupBox2, "TextLabel111",0 );
	yStartBox = new QLineEdit(GroupBox2);

	Q3ButtonGroup *GroupBox3 = new Q3ButtonGroup( 2,Qt::Horizontal,tr("End Point"),box2);

	new QLabel(tr( "X" ), GroupBox3, "TextLabel11",0);
	xEndBox = new QLineEdit(GroupBox3);

	new QLabel(tr( "Y" ),GroupBox3, "TextLabel111",0 );
	yEndBox = new QLineEdit(GroupBox3);

	Q3VBoxLayout* hl2 = new Q3VBoxLayout(geometry, 5, 5);
    hl2->addWidget(box1);
	hl2->addWidget(box2);

	tw->insertTab(geometry, tr( "&Geometry" ) );

	connect( unitBox, SIGNAL( activated(int) ), this, SLOT(displayCoordinates(int) ) );
	displayCoordinates(0);
}

void LineDialog::displayCoordinates(int unit)
{
if (unit)
	{
	int prec;
	char f;

	Plot *plot = (Plot *)lm->plot();
	plot->axisLabelFormat(lm->xAxis(), f, prec);

	QwtDoublePoint sp = lm->coordStartPoint();
	xStartBox->setText(QString::number(sp.x(), f, prec));
	yStartBox->setText(QString::number(sp.y(), f, prec));

	plot->axisLabelFormat(lm->yAxis(), f, prec);
	QwtDoublePoint ep = lm->coordEndPoint();
	xEndBox->setText(QString::number(ep.x(), f, prec));
	yEndBox->setText(QString::number(ep.y(), f, prec));
	}
else
	{
	QPoint startPoint = lm->startPoint();
	QPoint endPoint = lm->endPoint();

	xStartBox->setText(QString::number(startPoint.x()));
	yStartBox->setText(QString::number(startPoint.y()));

	xEndBox->setText(QString::number(endPoint.x()));
	yEndBox->setText(QString::number(endPoint.y()));
	}
}

void LineDialog::setCoordinates(int unit)
{
if (unit)
	{
	lm->setCoordStartPoint(QwtDoublePoint(xStartBox->text().replace(",", ".").toDouble(), 
							yStartBox->text().replace(",", ".").toDouble()));
	lm->setCoordEndPoint(QwtDoublePoint(xEndBox->text().replace(",", ".").toDouble(), 
						yEndBox->text().replace(",", ".").toDouble()));
	}
else
	{
	lm->setStartPoint(QPoint(xStartBox->text().toInt(), yStartBox->text().toInt()));
	lm->setEndPoint(QPoint(xEndBox->text().toInt(), yEndBox->text().toInt()));
	}
}

void LineDialog::apply()
{
if (tw->currentPage()==(QWidget *)options)
	{
	lm->setStyle(Graph::getPenStyle(styleBox->currentItem()));
	lm->setColor(colorBox->color());
	lm->setWidth(widthBox->currentText().toInt());
	lm->setEndArrow(endBox->isChecked());
	lm->setStartArrow(startBox->isChecked());
	}
else if (tw->currentPage()==(QWidget *)head)
	{
	if (lm->headLength() != boxHeadLength->value())
		lm->setHeadLength( boxHeadLength->value() );

	if (lm->headAngle() != boxHeadAngle->value())
		lm->setHeadAngle( boxHeadAngle->value() );

	if (lm->filledArrowHead() != filledBox->isChecked())
		lm->fillArrowHead( filledBox->isChecked() );
	}
else if (tw->currentPage()==(QWidget *)geometry)
	setCoordinates(unitBox->currentItem());

QwtPlot *plot = lm->plot();
Graph *g = (Graph *)plot->parent();
plot->replot();
g->emitModified();

enableHeadTab();
}

void LineDialog::accept()
{
apply();
close();
}

void LineDialog::setLineStyle(Qt::PenStyle style)
{
if (style==Qt::SolidLine)
	styleBox->setCurrentItem(0);
else if (style==Qt::DashLine)
	styleBox->setCurrentItem(1);
else if (style==Qt::DotLine)
	styleBox->setCurrentItem(2);	
else if (style==Qt::DashDotLine)
	styleBox->setCurrentItem(3);		
else if (style==Qt::DashDotDotLine)
	styleBox->setCurrentItem(4);
}
	
void LineDialog::enableHeadTab()
{
if (startBox->isChecked() || endBox->isChecked())
	tw->setTabEnabled (head, true);
else
	tw->setTabEnabled (head, false);
}

void LineDialog::pickColor()
{
QColor c = QColorDialog::getColor(colorBox->color(), this);
if ( !c.isValid() || c == colorBox->color() )
	return;

colorBox->setColor ( c ) ;
}
	
void LineDialog::setDefaultValues()
{
ApplicationWindow *app = (ApplicationWindow *)this->parent();
if (!app)
	return;

app->setArrowDefaultSettings(widthBox->currentText().toInt(), colorBox->color(), 
							Graph::getPenStyle(styleBox->currentItem()),
							boxHeadLength->value(), boxHeadAngle->value(), filledBox->isChecked());
}

void LineDialog::enableButtonDefault(QWidget *w)
{
if (w == geometry)
	buttonDefault->setEnabled(false);
else
	buttonDefault->setEnabled(true);
}

LineDialog::~LineDialog()
{
}
