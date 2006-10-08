/***************************************************************************
    File                 : plot3DDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Surface plot options dialog
                           
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
#include "plot3DDialog.h"
#include "textDialog.h"
#include "parser.h"
#include "symbolDialog.h"

#include <qcheckbox.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <q3listbox.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <q3whatsthis.h>
#include <qimage.h>
#include <qpixmap.h>
#include <q3buttongroup.h>
#include <q3popupmenu.h>
#include <q3memarray.h>
#include <qinputdialog.h>
#include <qfont.h>
#include <qfontdialog.h> 
#include <qcolordialog.h>
#include <q3vbox.h> 
#include <qmessagebox.h> 
#include <q3widgetstack.h>

#include <QFileDialog>
#include <Q3HBoxLayout>
#include <Q3VBoxLayout>

#include <qwt3d_color.h> 

Plot3DDialog::Plot3DDialog( QWidget* parent,  const char* name, bool modal, Qt::WFlags fl )
    : QDialog( parent, name, modal, fl )
{
	if ( !name )
		setName( "Plot3DDialog" );
	setWindowTitle( tr( "QtiPlot - Surface Plot Options" ) );
	setSizeGripEnabled( true );

	bars=0; points=0;

	initScalesPage();
	initAxesPage();
	initTitlePage();
	initColorsPage();
	initGeneralPage();

	GroupBox1 = new Q3ButtonGroup(4,Qt::Horizontal,tr(""),this, "GroupBox1" );
	GroupBox1->setFlat (true);

	btnTable = new QPushButton( GroupBox1, "btnTable" );
	btnTable->hide();

	buttonApply = new QPushButton( GroupBox1, "buttonApply" );
	buttonApply->setText( tr( "&Apply" ) );

	buttonOk = new QPushButton(GroupBox1, "buttonOk" );
	buttonOk->setText( tr( "&OK" ) );
	buttonOk->setDefault( true );

	buttonCancel = new QPushButton(GroupBox1, "buttonCancel" );
	buttonCancel->setText( tr( "&Cancel" ) );

	Q3VBoxLayout* vl = new Q3VBoxLayout(this,5,5, "vl");
	vl->addWidget(generalDialog);
	vl->addWidget(GroupBox1);

	// signals and slots connections 
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT(updatePlot() ) );
	connect( btnTable, SIGNAL( clicked() ), this, SLOT(worksheet() ) );
}

void Plot3DDialog::initScalesPage()
{
	generalDialog = new QTabWidget( this, "generalDialog" );

	scale = new QWidget( generalDialog, "scale" );

	axesList = new Q3ListBox( scale, "axesList" );
	axesList->insertItem(tr( "X" ) );
	axesList->insertItem(tr( "Y" ) );
	axesList->insertItem(tr( "Z" ) );
	axesList->setFixedWidth(50);
	axesList->setCurrentItem (0);

	Q3VBox *vbox=new Q3VBox(scale,"vbox");
	vbox->setSpacing(10);

	Q3HBox *hbox=new Q3HBox(vbox,"hbox");
	hbox->setSpacing(10);
	GroupBox6 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "" ),hbox, "GroupBox6" );

	new QLabel( tr( "From" ), GroupBox6, "TextLabel48",0 );
	boxFrom=new QLineEdit(GroupBox6, "from");
	boxFrom->setMaximumWidth(150);

	new QLabel( tr( "To" ), GroupBox6, "TextLabel47",0 );
	boxTo=new QLineEdit(GroupBox6, "to");
	boxTo->setMaximumWidth(150);

	new QLabel( tr( "Type" ), GroupBox6, "TextLabel49",0 );
	boxType=new QComboBox(GroupBox6, "step");
	boxType->insertItem(tr("linear"));
	boxType->insertItem(tr("logarithmic"));
	boxType->setMaximumWidth(150);

	TicksGroupBox = new Q3ButtonGroup(2,Qt::Horizontal, QString(),hbox, "GroupBox9" );

	new QLabel( tr( "Major Ticks" ), TicksGroupBox , "TextLabel488",0 );
	boxMajors=new QSpinBox(TicksGroupBox , "major");

	new QLabel( tr( "MinorTicks" ), TicksGroupBox , "TextLabel477",0 );
	boxMinors=new QSpinBox(TicksGroupBox , "minors");

	Q3HBoxLayout* hlayout3 = new Q3HBoxLayout(scale,5,5, "hlayout3");
	hlayout3->addWidget(axesList);
	hlayout3->addWidget(vbox);

	generalDialog->insertTab(scale, tr( "&Scale" ) );
}

void Plot3DDialog::initAxesPage()
{
	axes = new QWidget( generalDialog, "axes" );

	axesList2 = new Q3ListBox( axes, "axesList2" );
	axesList2->insertItem(tr( "X" ) );
	axesList2->insertItem(tr( "Y" ) );
	axesList2->insertItem(tr( "Z" ) );
	axesList2->setFixedWidth(50);
	axesList2->setCurrentItem (0);

	GroupBox8 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "" ),axes, "GroupBox8" );

	new QLabel( tr( "Title" ), GroupBox8 , "TextLabel4880",0 );
	boxLabel=new QLineEdit(GroupBox8, "label");

	new QLabel( tr( "Axis Font" ), GroupBox8 , "TextLabel4881",0 );

	Q3HBox *hbox = new Q3HBox (GroupBox8, "hbox");
	hbox->setSpacing(5);

	btnLabelFont=new QPushButton(hbox, "from");
	btnLabelFont->setText( tr( "&Choose font" ) );

	buttonAxisLowerGreek = new QPushButton(QChar(0x3B1), hbox, "buttonLowerGreek" ); 
	buttonAxisUpperGreek = new QPushButton(QChar(0x393), hbox, "buttonUpperGreek" ); 

	new QLabel( tr( "Major Ticks Length" ), GroupBox8 , "TextLabel4888",0 );
	boxMajorLength=new QLineEdit(GroupBox8,"boxMajL");

	new QLabel( tr( "Minor Ticks Length" ), GroupBox8 , "TextLabel4889",0 );
	boxMinorLength=new QLineEdit(GroupBox8,"boxMinL");

	Q3HBoxLayout* hlayout4 = new Q3HBoxLayout(axes,5,5, "hlayout4");
	hlayout4->addWidget(axesList2);
	hlayout4->addWidget(GroupBox8);

	generalDialog->insertTab(axes, tr( "&Axis" ) );

	connect( buttonAxisLowerGreek, SIGNAL(clicked()), this, SLOT(showLowerGreek()));
	connect( buttonAxisUpperGreek, SIGNAL(clicked()), this, SLOT(showUpperGreek()));
	connect( axesList2, SIGNAL(highlighted (int) ), this, SLOT(viewAxisOptions(int) ) );
	connect( axesList, SIGNAL(highlighted (int) ), this, SLOT(viewScaleLimits(int) ) );
	connect( btnLabelFont, SIGNAL(clicked()), this, SLOT(pickAxisLabelFont() ) );
}

void Plot3DDialog::initTitlePage()
{
	title = new QWidget( generalDialog, "title" );

	GroupBox2 = new Q3ButtonGroup(1,Qt::Horizontal,tr( "" ),title, "GroupBox2" );

	boxTitle=new QLineEdit(GroupBox2,"title");

	GroupBox4 = new Q3ButtonGroup(4,Qt::Horizontal,tr( "" ),GroupBox2, "GroupBox4" );
	GroupBox4->setFlat (true);

	buttonLowerGreek = new QPushButton(QChar(0x3B1), GroupBox4, "buttonLowerGreek" ); 
	buttonLowerGreek->setMaximumWidth(40);

	buttonUpperGreek = new QPushButton(QChar(0x393), GroupBox4, "buttonUpperGreek" ); 
	buttonUpperGreek->setMaximumWidth(40);

	btnTitleColor = new QPushButton( GroupBox4, "btnTitleColor" );
	btnTitleColor->setText( tr( "&Color" ) );
	btnTitleColor->setMaximumWidth(80);

	btnTitleFont = new QPushButton( GroupBox4, "btnTitleFont" );
	btnTitleFont->setText( tr( "&Font" ) );
	btnTitleFont->setMaximumWidth(80);

	Q3VBoxLayout* hlayout0 = new Q3VBoxLayout(title,5,5, "hlayout0");
	hlayout0->addWidget(GroupBox2);

	generalDialog->insertTab(title, tr( "&Title" ) );

	connect( btnTitleColor, SIGNAL(clicked()), this, SLOT(pickTitleColor() ) );
	connect( btnTitleFont, SIGNAL(clicked()), this, SLOT(pickTitleFont() ) );
	connect( buttonLowerGreek, SIGNAL(clicked()), this, SLOT(showLowerGreek()));
	connect( buttonUpperGreek, SIGNAL(clicked()), this, SLOT(showUpperGreek()));
}

void Plot3DDialog::initColorsPage()
{
	colors = new QWidget( generalDialog, "colors" );

	Q3HBox  *box=new Q3HBox (colors, "box"); 
	box->setMargin(5);
	box->setSpacing (5);

	GroupBox7 = new Q3ButtonGroup(1,Qt::Horizontal,tr( "Data" ),box, "GroupBox7" );

	btnFromColor = new QPushButton( GroupBox7, "btnFrom" );
	btnFromColor->setText( tr( "Ma&x" ) );

	btnToColor = new QPushButton( GroupBox7, "btnTo" );
	btnToColor->setText( tr( "&Min" ) );
	
	btnColorMap = new QPushButton( GroupBox7);
    btnColorMap->setText( tr( "Color Ma&p" ) );

	GroupBox3 = new Q3ButtonGroup(1,Qt::Horizontal,tr( "General" ),box, "GroupBox3" );

	btnMesh = new QPushButton( GroupBox3, "btnMesh" );
	btnMesh->setText( tr( "&Line" ) );

	btnBackground = new QPushButton( GroupBox3, "btnBackground" );
	btnBackground->setText( tr( "&Background" ) );

	GroupBox10= new Q3ButtonGroup(2,Qt::Horizontal,tr( "Coordinate System" ),box, "GroupBox11" );

	btnAxes = new QPushButton( GroupBox10, "btnAxes" );
	btnAxes->setText( tr( "&Axes" ) );

	btnLabels = new QPushButton( GroupBox10, "btnLabels" );
	btnLabels->setText( tr( "Lab&els" ) );

	btnNumbers = new QPushButton( GroupBox10, "btnNumbers" );
	btnNumbers->setText( tr( "&Numbers" ) );

	btnGrid = new QPushButton( GroupBox10, "btnGrid" );
	btnGrid->setText( tr( "&Grid" ) );

	Q3HBox  *box2=new Q3HBox (colors, "box2"); 
	box2->setMargin(10);
	box2->setSpacing (5);

	new QLabel( tr( "Opacity" ), box2, "TextLabel466",0 );
	boxTransparency = new QSpinBox(0,100,5,box2,"boxTransparency");

	Q3VBoxLayout* hlayout2 = new Q3VBoxLayout(colors,5,5, "hlayout2");
	hlayout2->addWidget(box);
	hlayout2->addWidget(box2);

	generalDialog->insertTab(colors, tr( "&Colors" ) );

	connect( btnAxes, SIGNAL( clicked() ), this, SLOT(pickAxesColor() ) );
	connect( btnLabels, SIGNAL( clicked() ), this, SLOT(pickLabelColor() ) );
	connect( btnNumbers, SIGNAL( clicked() ), this, SLOT(pickNumberColor() ) );
	connect( btnBackground, SIGNAL( clicked() ), this, SLOT(pickBgColor() ) );
	connect( btnFromColor, SIGNAL( clicked() ), this, SLOT(pickFromColor() ) );
	connect( btnToColor, SIGNAL( clicked() ), this, SLOT(pickToColor() ) );
	connect( btnColorMap, SIGNAL( clicked() ), this, SLOT(pickDataColorMap() ) );
	connect( btnGrid, SIGNAL( clicked() ), this, SLOT(pickGridColor() ) );
	connect( btnMesh, SIGNAL( clicked() ), this, SLOT(pickMeshColor() ) );
	connect( boxTransparency, SIGNAL( valueChanged(int) ), 
			this, SLOT(changeTransparency(int) ) );
}

void Plot3DDialog::initGeneralPage()
{
	general = new QWidget( generalDialog);

	GroupBox5 = new Q3ButtonGroup(2, Qt::Horizontal, QString::null, general);

	boxLegend= new QCheckBox(tr("Show Legend"), GroupBox5);
	boxOrthogonal = new QCheckBox(tr("Orthogonal"), GroupBox5);

	new QLabel(tr( "Line Width" ), GroupBox5, "TextLabel4887",0);
	boxMeshLineWidth = new QSpinBox(1, 100, 1, GroupBox5,"boxWidth");

	new QLabel( tr( "Resolution" ), GroupBox5, "TextLabel46",0 );
	boxResolution= new QSpinBox(1, 100, 1, GroupBox5,"resolution");
	boxResolution->setSpecialValueText( "1 (all data)" );

	new QLabel( tr( "Numbers Font" ), GroupBox5, "TextLabel467",0 );
	btnNumbersFont=new QPushButton(GroupBox5, "numbersFont");
	btnNumbersFont->setText( tr( "&Choose Font" ) );

	new QLabel( tr( "Distance labels - axis" ), GroupBox5, "TextLabel468",0 );
	boxDistance=new QSpinBox(0, 1000, 5, GroupBox5, "numbersDistance");

	Q3ButtonGroup *GroupBox55 = new Q3ButtonGroup(2, Qt::Horizontal, QString::null, general);

	new QLabel(tr( "Zoom (%)" ), GroupBox55, "TextLabel4887",0);
	boxZoom=new QSpinBox(1,10000,10,GroupBox55,"Zoom");

	new QLabel( tr( "X Zoom (%)" ), GroupBox55, "TextLabel46",0 );
	boxXScale= new QSpinBox(1,10000,10,GroupBox55,"X Zoom");

	new QLabel( tr( "Y Zoom (%)" ), GroupBox55, "TextLabel46",0 );
	boxYScale= new QSpinBox(1,10000,10,GroupBox55,"Y Zoom");

	new QLabel( tr( "Z Zoom (%)" ), GroupBox55, "TextLabel46",0 );
	boxZScale= new QSpinBox(1,10000,10,GroupBox55,"Z Zoom");

	Q3HBoxLayout* hlayout1 = new Q3HBoxLayout(general,5,5, "hlayout1");
	hlayout1->addWidget(GroupBox5);
	hlayout1->addWidget(GroupBox55);

	generalDialog->insertTab(general, tr( "&General" ) );

	connect( boxResolution, SIGNAL(valueChanged(int)), this, SIGNAL(updateResolution(int)));
	connect( boxDistance, SIGNAL(valueChanged(int)), this, SIGNAL(adjustLabels(int)));
	connect( boxMeshLineWidth, SIGNAL(valueChanged(int)), 
			this, SIGNAL(updateMeshLineWidth(int)));
	connect( boxOrthogonal, SIGNAL(toggled(bool)), this, SIGNAL(setOrtho(bool)));
	connect( boxLegend, SIGNAL(toggled(bool)), this, SIGNAL(showColorLegend(bool)));
	connect( boxZoom, SIGNAL(valueChanged(int)), this, SLOT(changeZoom(int)));
	connect( boxXScale, SIGNAL(valueChanged(int)), this, SLOT(changeZoom(int)));
	connect( boxYScale, SIGNAL(valueChanged(int)), this, SLOT(changeZoom(int)));
	connect( boxZScale, SIGNAL(valueChanged(int)), this, SLOT(changeZoom(int)));
	connect( btnNumbersFont, SIGNAL(clicked()), this, SLOT(pickNumbersFont() ) );
}

void Plot3DDialog::initPointsOptionsStack()
{
	points = new QWidget( generalDialog, "points" );

	Q3ButtonGroup* GroupBox14 = new Q3ButtonGroup( 2,Qt::Horizontal,tr(""),points,"GroupBox1" );
	GroupBox14->setFlat(true);	

	new QLabel( tr( "Style" ), GroupBox14 , "TextLabel4080",0 );
	boxPointStyle = new QComboBox( GroupBox14, "btnPointStyle" );
	boxPointStyle->insertItem(tr("Dot"));
	boxPointStyle->insertItem(tr("Cross Hair"));
	boxPointStyle->insertItem(tr("Cone"));

	optionStack = new Q3WidgetStack( points, "optionStack" );
	optionStack->setFrameShape( QFrame::StyledPanel );
	optionStack->setFrameShadow( Q3WidgetStack::Plain );

	dotsPage = new QWidget( optionStack, "DotsPage" );

	Q3ButtonGroup *GroupBox11 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "" ),dotsPage, "GroupBox11" );
	GroupBox11->setFlat(true);

	new QLabel( tr( "Width" ), GroupBox11 , "TextLabel408",0 );
	boxSize = new QLineEdit( GroupBox11, "btnBarsRad" );
	boxSize->setText("5");
	new QLabel( tr( "Smooth angles" ), GroupBox11 , "TextLabel4018",0 );
	boxSmooth= new QCheckBox("",GroupBox11,"smooth");
	boxSmooth->setChecked(false);

	Q3HBoxLayout* hlayout = new Q3HBoxLayout(dotsPage,5,5, "hlayout");
	hlayout->addWidget(GroupBox11);

	optionStack->addWidget( dotsPage, 0 );

	crossPage = new QWidget( optionStack, "CrossPage" );
	Q3ButtonGroup *GroupBox13 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "" ),crossPage, "GroupBox11" );
	GroupBox13->setFlat(true);

	new QLabel( tr( "Radius" ), GroupBox13 , "TextLabel4080",0 );
	boxCrossRad = new QLineEdit( GroupBox13, "btnBarsRad" );
	boxCrossRad->setText("0.01");
	new QLabel( tr( "Line Width" ), GroupBox13 , "TextLabel408",0 );
	boxCrossLinewidth = new QLineEdit( GroupBox13, "btnLineWidth" );	
	boxCrossLinewidth->setText("1");
	new QLabel( tr( "Smooth line" ), GroupBox13 , "TextLabel4018",0 );
	boxCrossSmooth= new QCheckBox("",GroupBox13,"smooth");
	boxCrossSmooth->setChecked(true);
	new QLabel( tr( "Boxed" ), GroupBox13 , "TextLabel40108",0 );
	boxBoxed= new QCheckBox("",GroupBox13,"boxed");
	boxBoxed->setChecked(false);

	Q3VBoxLayout* hlayout01 = new Q3VBoxLayout(crossPage,5,5, "hlayout01");

	hlayout01->addWidget(GroupBox13);

	optionStack->addWidget( crossPage, 1 );

	conesPage = new QWidget( optionStack, "ConesPage" );

	Q3ButtonGroup *GroupBox12 = new Q3ButtonGroup(2,Qt::Horizontal,tr( "" ),conesPage, "GroupBox11" );
	GroupBox12->setFlat(true);

	new QLabel( tr( "Width" ), GroupBox12 , "TextLabel408",0 );
	boxConesRad= new QLineEdit( GroupBox12, "btnBarsRad" );
	boxConesRad->setText("0.5");
	new QLabel( tr( "Quality" ), GroupBox12 , "TextLabel4018",0 );
	boxQuality= new QSpinBox(0,40,1,GroupBox12,"quality");
	boxQuality->setValue(32);

	Q3VBoxLayout* hlayout00 = new Q3VBoxLayout(conesPage,5,5, "hlayout00");
	hlayout00->addWidget(GroupBox12);

	optionStack->addWidget( conesPage, 2 );

	Q3VBoxLayout* hlayout02 = new Q3VBoxLayout(points,5,5, "hlayout02");
	hlayout02->addWidget(GroupBox14);
	hlayout02->addWidget(optionStack);

	generalDialog->insertTab(points, tr( "Points" ),4 );
	connect( boxPointStyle, SIGNAL( activated(int) ), optionStack, SLOT( raiseWidget(int) ) );
}

void Plot3DDialog::showLowerGreek()
{
	SymbolDialog *greekLetters = new SymbolDialog(SymbolDialog::lowerGreek, this, Qt::Tool);
	greekLetters->setAttribute(Qt::WA_DeleteOnClose);
	connect(greekLetters, SIGNAL(addLetter(const QString&)), this, SLOT(addSymbol(const QString&)));
	greekLetters->show();
	greekLetters->setActiveWindow();
}

void Plot3DDialog::showUpperGreek()
{
	SymbolDialog *greekLetters = new SymbolDialog(SymbolDialog::upperGreek, this, Qt::Tool);
	greekLetters->setAttribute(Qt::WA_DeleteOnClose);
	connect(greekLetters, SIGNAL(addLetter(const QString&)), this, SLOT(addSymbol(const QString&)));
	greekLetters->show();
	greekLetters->setActiveWindow();
}

void Plot3DDialog::addSymbol(const QString& letter)
{
	if (generalDialog->currentPage()==(QWidget*)title)
		boxTitle->insert(letter);
	else if (generalDialog->currentPage()==(QWidget*)axes)
		boxLabel->insert(letter);
}

void Plot3DDialog::worksheet()
{
	emit showWorksheet();
	close();
}

void Plot3DDialog::customWorksheetBtn(const QString& text)
{
	if (!text.isEmpty())
	{
		btnTable->show();
		btnTable->setText(text);
	}
}

void Plot3DDialog::disableGridOptions()
{
	btnGrid->setDisabled(true);
}

void Plot3DDialog::disableAxesOptions()
{
	TicksGroupBox->setDisabled(true);
	generalDialog->setTabEnabled(axes,false);
	GroupBox10->setDisabled(true);
	boxDistance->setDisabled(true);
	btnNumbersFont->setDisabled(true);
}

void Plot3DDialog::showBarsTab(double rad)
{
	bars = new QWidget( generalDialog, "bars" );

	Q3ButtonGroup *GroupBox11 = new Q3ButtonGroup(2,Qt::Horizontal,QString(),bars, "GroupBox11" );

	new QLabel( tr( "Width" ), GroupBox11 , "TextLabel408",0 );
	boxBarsRad = new QLineEdit( GroupBox11, "btnBarsRad" );
	boxBarsRad->setText(QString::number(rad));

	Q3VBoxLayout* hlayout00 = new Q3VBoxLayout(bars,5,5, "hlayout00");
	hlayout00->addWidget(GroupBox11);

	generalDialog->insertTab(bars, tr( "Bars" ),4 );
}

void Plot3DDialog::showPointsTab(double rad, bool smooth)
{
	boxPointStyle->setCurrentItem(0);
	boxSize->setText(QString::number(rad));
	boxSmooth->setChecked(smooth);
	optionStack->raiseWidget(0);
}

void Plot3DDialog::showConesTab(double rad, int quality)
{
	boxPointStyle->setCurrentItem(2);
	boxConesRad->setText(QString::number(rad));
	boxQuality->setValue(quality);
	optionStack->raiseWidget(2);
}

void Plot3DDialog::showCrossHairTab(double rad, double linewidth, bool smooth, bool boxed)
{
	boxPointStyle->setCurrentItem(1);
	boxCrossRad->setText(QString::number(rad));
	boxCrossLinewidth->setText(QString::number(linewidth));
	boxCrossSmooth->setChecked(smooth);
	boxBoxed->setChecked(boxed);
	optionStack->raiseWidget(1);
}

void Plot3DDialog::disableMeshOptions()
{
	btnMesh->setDisabled(true);
	boxMeshLineWidth->setDisabled(true);
}

void Plot3DDialog::disableLegend()
{
	boxLegend->setDisabled(true);
}

void Plot3DDialog::setLabelsDistance(int dist)
{
	boxDistance->setValue(dist);
}

void Plot3DDialog::pickDataColorMap()
{
QString fn = QFileDialog::getOpenFileName(d_plot->colorMap(), tr("Colormap files") + " (*.map *.MAP)", this);
if (!fn.isEmpty())
   emit setDataColorMap(fn);
}
  	
QColor Plot3DDialog::pickFromColor()
{
	QColor c = QColorDialog::getColor(fromColor, this );
	if ( !c.isValid() )
		return fromColor;

	fromColor = c;
	return fromColor;
}

QColor Plot3DDialog::pickToColor()
{
	QColor c = QColorDialog::getColor(toColor, this );
	if ( !c.isValid() )
		return toColor;

	toColor = c;
	return toColor;
}

QColor Plot3DDialog::pickGridColor()
{
	QColor c = QColorDialog::getColor(gridColor, this );
	if ( !c.isValid() )
		return gridColor;

	gridColor = c;
	return gridColor;
}

QColor Plot3DDialog::pickAxesColor()
{
	QColor c = QColorDialog::getColor(axesColor, this );
	if ( !c.isValid() )
		return axesColor;

	axesColor = c;
	return axesColor;
}

QColor Plot3DDialog::pickBgColor()
{
	QColor c = QColorDialog::getColor(bgColor, this );
	if ( !c.isValid() )
		return bgColor;

	bgColor = c;
	return bgColor;
}

QColor Plot3DDialog::pickNumberColor()
{
	QColor c = QColorDialog::getColor(numColor, this );
	if ( !c.isValid() )
		return numColor;

	numColor = c;
	return numColor;
}

QColor Plot3DDialog::pickLabelColor()
{
	QColor c = QColorDialog::getColor(labelColor, this );
	if ( !c.isValid() )
		return labelColor;

	labelColor = c;
	return labelColor;
}

QColor Plot3DDialog::pickTitleColor()
{
	QColor c = QColorDialog::getColor(titleColor, this );
	if ( !c.isValid() )
		return titleColor;

	titleColor = c;
	return titleColor;
}

void Plot3DDialog::pickTitleFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,titleFont,this);
	if ( ok ) {
		titleFont = font;
	} else {
		return;
	}
}

void Plot3DDialog::pickNumbersFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,numbersFont,this);
	if ( ok ) {
		numbersFont = font;
	} else {
		return;
	}
}

void Plot3DDialog::viewAxisOptions(int axis)
{
	boxLabel->setText(labels[axis]);

	boxMajorLength->setText(tickLengths[2*axis+0]);
	boxMinorLength->setText(tickLengths[2*axis+1]);
}

void Plot3DDialog::setAxesLabels(const QStringList& list)
{
	labels=list;
	boxLabel->setText(labels[0]);
}

void Plot3DDialog::setScales(const QStringList& list)
{
	scales=list;
	boxFrom->setText(scales[0]);
	boxTo->setText(scales[1]);
	boxMajors->setValue(scales[2].toInt());
	boxMinors->setValue(scales[3].toInt());
	boxType->setCurrentItem(scales[4].toInt());
}

void Plot3DDialog::setAxesTickLengths(const QStringList& list)
{
	tickLengths=list;
	boxMajorLength->setText(list[0]);
	boxMinorLength->setText(list[1]);
}

void Plot3DDialog::viewScaleLimits(int axis)
{
	boxFrom->setText(scales[5*axis+0]);
	boxTo->setText(scales[5*axis+1]);
	boxMajors->setValue(scales[5*axis+2].toInt());
	boxMinors->setValue(scales[5*axis+3].toInt());
	boxType->setCurrentItem(scales[5*axis+4].toInt());
}

void Plot3DDialog::setTitle(const QString& title)
{
	boxTitle->setText(title);
}

void Plot3DDialog::setTitleFont(const QFont& font)
{
	titleFont=font;
}

QColor Plot3DDialog::pickMeshColor()
{

	QColor c = QColorDialog::getColor(meshColor, this );
	if ( !c.isValid() )
		return meshColor;

	meshColor=c;	
	return meshColor;
}

void Plot3DDialog::accept()
{
	if (updatePlot())
		close();
}

void Plot3DDialog::setDataColors(const QColor& minColor, const QColor& maxColor)
{
	fromColor=minColor;
	toColor=maxColor;
}

void Plot3DDialog::setColors(const QColor& title, const QColor& mesh,const QColor& axes,const QColor& num,
		const QColor& label,const QColor& bg,const QColor& grid)
{
	titleColor=title;
	meshColor=mesh;
	axesColor=axes;
	numColor=num;
	labelColor=label;
	bgColor=bg;
	gridColor=grid;
}

void Plot3DDialog::showLegend(bool show)
{
	boxLegend->setChecked(show);
}

void Plot3DDialog::changeZoom(int)
{
	if (generalDialog->currentPage() != (QWidget*)general)
		return;

	emit updateZoom(boxZoom->value()*0.01);
	emit updateScaling(boxXScale->value()*0.01,boxYScale->value()*0.01,
			boxZScale->value()*0.01);
}

void Plot3DDialog::changeTransparency(int val)
{
	if (generalDialog->currentPage() != (QWidget*)colors)
		return;

	emit updateTransparency(val*0.01);
}

bool Plot3DDialog::updatePlot()
{
	int axis=-1;

	if (generalDialog->currentPage()==(QWidget*)bars)
	{	
		emit updateBars(boxBarsRad->text().toDouble());
	}

	if (generalDialog->currentPage()==(QWidget*)points)
	{	
		if (boxPointStyle->currentItem() == 0)
			emit updatePoints(boxSize->text().toDouble(), boxSmooth->isChecked());
		else if (boxPointStyle->currentItem() == 1)
			emit updateCross(boxCrossRad->text().toDouble(), boxCrossLinewidth->text().toDouble(),
					boxCrossSmooth->isChecked(), boxBoxed->isChecked());
		else if (boxPointStyle->currentItem() == 2)
			emit updateCones(boxConesRad->text().toDouble(), boxQuality->value());
	}

	if (generalDialog->currentPage()==(QWidget*)title)
	{	
		emit updateTitle(boxTitle->text(),titleColor,titleFont);
	}

	if (generalDialog->currentPage()==(QWidget*)colors)
	{
		emit updateTransparency(boxTransparency->value()*0.01);
		emit updateDataColors(fromColor,toColor);
		emit updateColors(meshColor,axesColor,numColor,labelColor,bgColor,gridColor);
	}

	if (generalDialog->currentPage()==(QWidget*)general)
	{
		emit showColorLegend(boxLegend->isChecked());
		emit updateMeshLineWidth(boxMeshLineWidth->value());
		emit adjustLabels(boxDistance->value());
		emit updateResolution (boxResolution->value());
		emit showColorLegend(boxLegend->isChecked());
		emit setNumbersFont(numbersFont);
		emit updateZoom(boxZoom->value()*0.01);
		emit updateScaling(boxXScale->value()*0.01,boxYScale->value()*0.01,
				boxZScale->value()*0.01);
	}

	if (generalDialog->currentPage()==(QWidget*)scale)
	{
		axis=axesList->currentItem();
		QString from=boxFrom->text().lower();
		QString to=boxTo->text().lower();
		double start,end;
		bool error=false;	
		try
		{
			MyParser parser;
			parser.SetExpr(from.ascii());
			start=parser.Eval();
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(0,tr("QtiPlot - Start limit error"),  QString::fromStdString(e.GetMsg()));
			boxFrom->setFocus();
			error=true;
			return false;
		}	
		try
		{
			MyParser parser;
			parser.SetExpr(to.ascii());
			end=parser.Eval();
		}
		catch(mu::ParserError &e)
		{
			QMessageBox::critical(0,tr("QtiPlot - End limit error"), QString::fromStdString(e.GetMsg()));
			boxTo->setFocus();
			error=true;
			return false;
		}

		if (start>=end)
		{
			QMessageBox::critical(0,tr("QtiPlot - Input error"),
					tr("Please enter scale limits that satisfy: from < to!"));
			boxTo->setFocus();
			return false;
		}

		if (! error)
			emit updateScale(axis,scaleOptions(axis, start, end, 
						boxMajors->text(), boxMinors->text()));
	}

	if (generalDialog->currentPage()==(QWidget*)axes)
	{
		axis=axesList2->currentItem();
		labels[axis] = boxLabel->text();
		emit updateLabel(axis, boxLabel->text(),axisFont(axis));
		emit updateTickLength(axis,boxMajorLength->text().toDouble(),
				boxMinorLength->text().toDouble());
	}

	return true;
}

QStringList Plot3DDialog::scaleOptions(int axis, double start, double end, 
		const QString& majors, const QString& minors)
{
	QStringList l;
	l<<QString::number(start);
	l<<QString::number(end);
	l<<majors;
	l<<minors;
	l<<QString::number(boxType->currentItem());

	for (int i=0;i<5;i++)
		scales[5*axis+i]=l[i];
	return l;
}

void Plot3DDialog::setMeshLineWidth(double lw)
{
	boxMeshLineWidth->setValue(int(lw));
}

void Plot3DDialog::setAxesFonts(const QFont& xf, const QFont& yf, const QFont& zf)
{
	xAxisFont=xf;
	yAxisFont=yf;
	zAxisFont=zf;
}

void Plot3DDialog::pickAxisLabelFont()
{
	bool ok;
	QFont font;
	switch(axesList2->currentItem())
	{
		case 0:
			font= QFontDialog::getFont(&ok,xAxisFont,this);
			if ( ok ) 
				xAxisFont=font;
			else
				return;
			break;

		case 1:
			font= QFontDialog::getFont(&ok,yAxisFont,this);
			if ( ok ) 
				yAxisFont=font;
			else
				return;
			break;

		case 2:
			font= QFontDialog::getFont(&ok,zAxisFont,this);
			if ( ok ) 
				zAxisFont=font;
			else
				return;
			break;
	}
}

QFont Plot3DDialog::axisFont(int axis)
{
	QFont f;
	switch(axis)
	{
		case 0:
			f=xAxisFont;
			break;

		case 1:
			f=yAxisFont;
			break;

		case 2:
			f=zAxisFont;
			break;
	}
	return f;
}

void Plot3DDialog::setTransparency(double t)
{
	boxTransparency->setValue(int(100*t));
}

void Plot3DDialog::setResolution(int r)
{
	boxResolution->setValue( r );
}

void Plot3DDialog::setZoom(double zoom)
{
	boxZoom->setValue(int(zoom*100));
}

void Plot3DDialog::setScaling(double xVal, double yVal, double zVal)
{
	boxXScale->setValue(int(xVal*100));
	boxYScale->setValue(int(yVal*100));
	boxZScale->setValue(int(zVal*100));
}

void Plot3DDialog::showGeneralTab()
{
	generalDialog->showPage(general);
}

void Plot3DDialog::showTitleTab()
{
	generalDialog->setCurrentPage(2);
}

void Plot3DDialog::showAxisTab()
{
	generalDialog->setCurrentPage(1);
}

Plot3DDialog::~Plot3DDialog()
{
}
