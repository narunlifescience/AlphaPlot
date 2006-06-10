/***************************************************************************
    File                 : textDialog.cpp
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Text label/axis label options dialog
                           
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

#include "textDialog.h"
#include "txt_icons.h"
#include "symbolDialog.h"

#include <QFontDialog>
#include <QColorDialog>
#include <QFont>
#include <QGroupBox>
#include <QTextEdit>
#include <QTextCursor>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

static const char * lineSymbol_xpm[] = {
"16 16 4 1",
" 	c None",
".	c #8C2727",
"+	c #272787",
"@	c #FFFFFF",
"                ",
"                ",
"                ",
"      ...       ",
"  ............  ",
"      ...       ",
"                ",
"                ",
"                ",
"      +++       ",
"  +++++@++++++  ",
"      +++       ",
"                ",
"                ",
"                ",
"                "};


TextDialog::TextDialog(TextType type, QWidget* parent, Qt::WFlags fl )
	: QDialog( parent, fl )
{
	setWindowTitle( tr( "QtiPlot - Text options" ) );
	setSizeGripEnabled( true );

	textType = type;

	// initialize selectedFont with something useful to
	// prevent error if setFont is not called before show()
	selectedFont = this->font();
	
	// top groupbox
	groupBox1 = new QGroupBox(QString());

	// grid layout for top groupbox
	QGridLayout * topLayout = new QGridLayout();
	// add text color label
	topLayout->addWidget(new QLabel(tr("Text Color")), 0, 0);

	colorBtn = new ColorButton();
	// add color button
	topLayout->addWidget(colorBtn, 0, 1);

	buttonOk = new QPushButton(tr("&OK"));
	buttonOk->setAutoDefault( true );
	buttonOk->setDefault( true );

	// add ok button
	topLayout->addWidget(buttonOk, 0, 3);

	// add font label
	topLayout->addWidget(new QLabel(tr("Font")), 1, 0);

	buttonFont = new QPushButton(tr( "&Font" ));
	buttonFont->setAutoDefault( true );

	// add font button
	topLayout->addWidget(buttonFont, 1, 1);

	buttonApply = new QPushButton(tr( "&Apply" ));
	buttonApply->setAutoDefault( true );
	buttonApply->setDefault( true );
	
	// add apply button
	topLayout->addWidget( buttonApply, 1, 3 );

	if (textType == TextDialog::AxisTitle)
	{
		// add label "alignment"
		topLayout->addWidget(new QLabel(tr("Alignment")), 2, 0);
		alignmentBox = new QComboBox();
		alignmentBox->addItem( tr( "Center" ) );
		alignmentBox->addItem( tr( "Left" ) );
		alignmentBox->addItem( tr( "Right" ) );
		// add alignment combo box
		topLayout->addWidget(alignmentBox, 2, 1);
	}
	else
	{
		// add label "frame"
		topLayout->addWidget(new QLabel(tr("Frame")), 2, 0);
		frameBox = new QComboBox();
		frameBox->addItem( tr( "None" ) );
		frameBox->addItem( tr( "Rectangle" ) );
		frameBox->addItem( tr( "Shadow" ) );
		topLayout->addWidget(frameBox, 2, 1);
	}

	buttonCancel = new QPushButton( tr( "&Cancel" ) );
	buttonCancel->setAutoDefault( true );	
	// add cancel button
	topLayout->addWidget( buttonCancel, 2, 3 );

	if (textType == TextDialog::TextMarker)
	{ //TODO: Sometime background features for axes lables should be implemented
		// add label "background color"	
		topLayout->addWidget(new QLabel(tr("Background color")), 3, 0);
		backgroundBtn = new ColorButton(groupBox1);
		// add background button
		topLayout->addWidget( backgroundBtn );	

		connect(backgroundBtn, SIGNAL(clicked()), this, SLOT(pickBackgroundColor()));
	}

	// align the OK, Apply, and Cancel buttons to the right
	topLayout->setColumnStretch(2, 1);
	
	groupBox1->setLayout( topLayout );

	/* TODO: Angle feature not implemented, yet
	 * caution: This code is still the old Qt3 code
	   QLabel* rotate=new QLabel(tr( "Rotate (deg.)" ),GroupBox1, "TextLabel1_2",0);
	   rotate->hide();

	   rotateBox = new QComboBox( false, GroupBox1, "rotateBox" );
	   rotateBox->insertItem( tr( "0" ) );
	   rotateBox->insertItem( tr( "45" ) );
	   rotateBox->insertItem( tr( "90" ) );
	   rotateBox->insertItem( tr( "135" ) );
	   rotateBox->insertItem( tr( "180" ) );
	   rotateBox->insertItem( tr( "225" ) );
	   rotateBox->insertItem( tr( "270" ) );
	   rotateBox->insertItem( tr( "315" ) );
	   rotateBox->setEditable (true);
	   rotateBox->setCurrentItem(0);
	   rotateBox->hide();
	   */

	// middle group box
	groupBox2 = new QGroupBox(QString());

	// layout for middle group box
	QHBoxLayout * layoutMiddle = new QHBoxLayout();

	// add the buttons
	if (textType == TextDialog::AxisTitle)
	{
		buttonCurve = new QPushButton( QPixmap(lineSymbol_xpm), QString());
		buttonCurve->setMaximumWidth(40);
		buttonCurve->setMinimumHeight(35);
		connect( buttonCurve, SIGNAL( clicked() ), this, SLOT(addCurve() ) );
		layoutMiddle->addWidget(buttonCurve);
	}

	QFont font = this->font();
	font.setPointSize(14);
		
	buttonIndex = new QPushButton(QPixmap(index_xpm),QString());
	buttonIndex->setMaximumWidth(40);
	buttonIndex->setMinimumHeight(35);
	buttonIndex->setFont(font);
	layoutMiddle->addWidget(buttonIndex);

	buttonExp = new QPushButton(QPixmap(exp_xpm),QString());
	buttonExp->setMaximumWidth(40);
	buttonExp->setMinimumHeight(35);
	buttonExp->setFont(font);
	layoutMiddle->addWidget(buttonExp);

	buttonLowerGreek = new QPushButton(QString(QChar(0x3B1))); 
	buttonLowerGreek->setFont(font);
	buttonLowerGreek->setMaximumWidth(40);
	layoutMiddle->addWidget(buttonLowerGreek);

	buttonUpperGreek = new QPushButton(QString(QChar(0x393))); 
	buttonUpperGreek->setFont(font);
	buttonUpperGreek->setMaximumWidth(40);
	layoutMiddle->addWidget(buttonUpperGreek);

	buttonMathSymbols = new QPushButton(QString(QChar(0x222B))); 
	buttonMathSymbols->setFont(font);
	buttonMathSymbols->setMaximumWidth(40);
	layoutMiddle->addWidget(buttonMathSymbols);

	buttonArrowSymbols = new QPushButton(QString(QChar(0x2192))); 
	buttonArrowSymbols->setFont(font);
	buttonArrowSymbols->setMaximumWidth(40);
	layoutMiddle->addWidget(buttonArrowSymbols);

	font = this->font();
	font.setBold(true);
	font.setPointSize(14);

	buttonB = new QPushButton(tr("B")); 
	buttonB->setFont(font);
	buttonB->setMaximumWidth(40);
	layoutMiddle->addWidget(buttonB);

	font = this->font();
	font.setItalic(true);
	font.setPointSize(14);
	
	buttonI = new QPushButton(tr("It"));
	buttonI->setFont(font);
	buttonI->setMaximumWidth(40);
	layoutMiddle->addWidget(buttonI);

	font = this->font();
	font.setUnderline(true);
	font.setPointSize(14);

	buttonU = new QPushButton(tr("U"));
	buttonU->setFont(font);
	buttonU->setMaximumWidth(40);
	layoutMiddle->addWidget(buttonU);

	lineEdit = new QTextEdit();

	setFocusPolicy(Qt::StrongFocus);
	setFocusProxy(lineEdit);

	// set middle layout
	groupBox2->setLayout( layoutMiddle );
	
	// put everything together
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(groupBox1);
	mainLayout->addWidget(groupBox2);
	mainLayout->addWidget(lineEdit);
	setLayout( mainLayout );


	// signals and slots connections
	connect( colorBtn, SIGNAL( clicked() ), this, SLOT( pickTextColor() ) );
	connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
	connect( buttonApply, SIGNAL( clicked() ), this, SLOT( apply() ) );
	connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
	connect( buttonFont, SIGNAL( clicked() ), this, SLOT(customFont() ) );
	connect( buttonExp, SIGNAL( clicked() ), this, SLOT(addExp() ) );
	connect( buttonIndex, SIGNAL( clicked() ), this, SLOT(addIndex() ) );
	connect( buttonU, SIGNAL( clicked() ), this, SLOT(addUnderline() ) );
	connect( buttonI, SIGNAL( clicked() ), this, SLOT(addItalic() ) );
	connect( buttonB, SIGNAL( clicked() ), this, SLOT(addBold() ) );
	connect(buttonLowerGreek, SIGNAL(clicked()), this, SLOT(showLowerGreek()));
	connect(buttonUpperGreek, SIGNAL(clicked()), this, SLOT(showUpperGreek()));
	connect(buttonMathSymbols, SIGNAL(clicked()), this, SLOT(showMathSymbols()));
	connect(buttonArrowSymbols, SIGNAL(clicked()), this, SLOT(showArrowSymbols()));
}

void TextDialog::showLowerGreek()
{
	SymbolDialog *greekLetters = new SymbolDialog(SymbolDialog::lowerGreek, this, Qt::Tool);
	greekLetters->setAttribute(Qt::WA_DeleteOnClose);
	QFont f = selectedFont;
	if(f.pointSize()<14)
		f.setPointSize(14);
	greekLetters->setFont(f);
	connect(greekLetters, SIGNAL(addLetter(const QString&)), this, SLOT(addSymbol(const QString&)));
	greekLetters->show();
	greekLetters->setFocus();
}

void TextDialog::showUpperGreek()
{
	SymbolDialog *greekLetters = new SymbolDialog(SymbolDialog::upperGreek, this, Qt::Tool);
	greekLetters->setAttribute(Qt::WA_DeleteOnClose);
	QFont f = selectedFont;
	if(f.pointSize()<14)
		f.setPointSize(14);
	greekLetters->setFont(f);
	connect(greekLetters, SIGNAL(addLetter(const QString&)), this, SLOT(addSymbol(const QString&)));
	greekLetters->show();
	greekLetters->setFocus();
}

void TextDialog::showMathSymbols()
{
	SymbolDialog *mathSymbols = new SymbolDialog(SymbolDialog::mathSymbols, this, Qt::Tool);
	mathSymbols->setAttribute(Qt::WA_DeleteOnClose);
	QFont f = selectedFont;
	if(f.pointSize()<14)
		f.setPointSize(14);
	mathSymbols->setFont(f);
	connect(mathSymbols, SIGNAL(addLetter(const QString&)), this, SLOT(addSymbol(const QString&)));
	mathSymbols->show();
	mathSymbols->setFocus();
}

void TextDialog::showArrowSymbols()
{
	SymbolDialog *arrowSymbols = new SymbolDialog(SymbolDialog::arrowSymbols, this, Qt::Tool);
	arrowSymbols->setAttribute(Qt::WA_DeleteOnClose);
	arrowSymbols->setFont(selectedFont);
	QFont f = selectedFont;
	if(f.pointSize()<14)
		f.setPointSize(14);
	arrowSymbols->setFont(f);
	connect(arrowSymbols, SIGNAL(addLetter(const QString&)), this, SLOT(addSymbol(const QString&)));
	arrowSymbols->show();
	arrowSymbols->setFocus();
}

void TextDialog::addSymbol(const QString & letter)
{
	lineEdit->textCursor().insertText(letter);
}

void TextDialog::addCurve()
{
	formatText("\\c{","}");
}

void TextDialog::addUnderline()
{
	formatText("<u>","</u>");
}

void TextDialog::addItalic()
{
	formatText("<i>","</i>");
}

void TextDialog::addBold()
{
	formatText("<b>","</b>");
}

void TextDialog::addIndex()
{
	formatText("<sub>","</sub>");
}

void TextDialog::addExp()
{
	formatText("<sup>","</sup>");
}

void TextDialog::formatText(const QString & prefix, const QString & postfix)
{
	QTextCursor cursor = lineEdit->textCursor();
	QString markedText = lineEdit->textCursor().selectedText();
	cursor.insertText(prefix+markedText+postfix);
	if(markedText.isEmpty())
	{
		// if no text is marked, place cursor inside the <..></..> statement
		// instead of after it
		cursor.movePosition(QTextCursor::PreviousCharacter,QTextCursor::MoveAnchor,postfix.size());
		// the next line makes the selection visible to the user 
		// (the line above only changes the selection in the
		// underlying QTextDocument)
		lineEdit->setTextCursor(cursor);
	}
	// give focus back to text edit
	lineEdit->setFocus();
}

int TextDialog::backgroundType()
{
	return frameBox->currentIndex();
}

void TextDialog::apply()
{
	if (textType == TextDialog::AxisTitle)
	{
		emit changeAlignment(alignment());
		emit changeText(lineEdit->toPlainText());
		emit changeColor(colorBtn->color());
	}
	else
		emit values(lineEdit->text(),angle(),backgroundType(),selectedFont, colorBtn->color(), backgroundBtn->color());
}

void TextDialog::accept()
{
	apply();
	close();
}

void TextDialog::setBackgroundType(int bkg)
{
	frameBox->setCurrentIndex(bkg);
}

int TextDialog::alignment()
{
	int align=-1;
	switch (alignmentBox->currentIndex())
	{
		case 0:
			align = Qt::AlignHCenter;
			break;

		case 1:
			align = Qt::AlignLeft;
			break;

		case 2:
			align = Qt::AlignRight;
			break;
	}
	return align;
}

void TextDialog::setAlignment(int align)
{	
	switch(align)
	{
		case Qt::AlignHCenter:
			alignmentBox->setCurrentIndex(0);
			break;
		case Qt::AlignLeft:
			alignmentBox->setCurrentIndex(1);
			break;
		case Qt::AlignRight:
			alignmentBox->setCurrentIndex(2);
			break;
	}
}

void TextDialog::customFont()
{
	bool okF;
	QFont fnt = QFontDialog::getFont( &okF,selectedFont,this);
	if (okF)
	{
		selectedFont = fnt;
		buttonFont->setFont(fnt);
		fnt.setPointSize(12);
		lineEdit->setFont(fnt);
	}
	emit changeFont (fnt);
}

void TextDialog::setAngle(int /*angle*/)
{
	//TODO: Implement angle feature 
//X	rotateBox-> ...
}

int TextDialog::angle()
{
	//TODO: Implement angle feature
//X	return rotateBox-> ...
	return 0;
}

void TextDialog::setText(const QString & t)
{
	QTextCursor cursor = lineEdit->textCursor();
	// select the whole (old) text 
	cursor.movePosition(QTextCursor::Start);
	cursor.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
	// replace old text
	cursor.insertText(t);
	// select the whole (new) text
	cursor.movePosition(QTextCursor::Start);
	cursor.movePosition(QTextCursor::End,QTextCursor::KeepAnchor);
	// this line makes the selection visible to the user 
	// (the 2 lines above only change the selection in the
	// underlying QTextDocument)
	lineEdit->setTextCursor(cursor);
	// give focus back to text edit
	lineEdit->setFocus();
}

void TextDialog::setTextColor(QColor c)
{
	colorBtn->setColor(c);
}

void TextDialog::pickTextColor()
{
	QColor c = QColorDialog::getColor( colorBtn->color(), this);
	if ( !c.isValid() || c ==  colorBtn->color() )
		return;

	colorBtn->setColor ( c ) ;
}

void TextDialog::setBackgroundColor(QColor c)
{
	backgroundBtn->setColor(c);
}

void TextDialog::pickBackgroundColor()
{
	QColor c = QColorDialog::getColor( backgroundBtn->color(), this);
	if ( !c.isValid() || c ==  backgroundBtn->color() )
		return;

	backgroundBtn->setColor ( c ) ;
}

QFont TextDialog::font()
{
	return selectedFont;
}

void TextDialog::setFont(const QFont & fnt)
{
	selectedFont = fnt;
	buttonFont->setFont(fnt);
	QFont ftemp = fnt;
	ftemp.setPointSize(12);
	lineEdit->setFont(ftemp);
}
	
TextDialog::~TextDialog()
{
}
