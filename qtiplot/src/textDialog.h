/***************************************************************************
    File                 : textDialog.h
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

#ifndef TEXTDLG_H
#define TEXTDLG_H

#include <QDialog>
#include <QGroupBox>
#include <QTextEdit>
#include <QTextCursor>
#include <QComboBox>
#include <QLabel>

#include "colorButton.h"

//! Options dialog for text labels/axes labels
class TextDialog : public QDialog
{ 
	Q_OBJECT

public:
	//! Label types
	enum TextType{
		TextMarker, /*!< normal text label */ 
		AxisTitle   /*!< axis label */ 
	};

	//! Constructor
	/**
	 * \param type text type (TextMarker | AxisTitle)
	 * \param parent parent widget
	 * \param fl window flags
	 */
	TextDialog(TextType type, QWidget* parent = 0, Qt::WFlags fl = 0 );
	//! Destructor
	~TextDialog();

	//! Return axis label alignment
	/**
	 * \sa setAlignment()
	 */
	int alignment();
	//! Return the current font
	QFont font();
	//! Get label background type
	/**
	 * \sa setBackgroundType()
	 */
	int backgroundType();
	//! Return rotation angle (not yet implemented)
	int angle();

public slots:
	//! Set label background type
	/**
	 * \param bkg label background type
	 * 0 -> plain, no border
	 * 1 -> with border
	 * 2 -> border + shadow
	 */
	void setBackgroundType(int bkg);
	//! Set rotation angle (not yet implemented)
	void setAngle(int angle);
	//! Set the background color to 'c'
	void setBackgroundColor(QColor c);
	//! Set the text color to 'c'
	void setTextColor(QColor c);
	//! Set the current font to 'fnt'
	void setFont(const QFont& fnt);
	//! Set the contents of the text editor box
	void setText(const QString & t);
	//! Set axis label alignment
	/**
	 * \param align alignment (can be -1 for invalid,
	 *  Qt::AlignHCenter, Qt::AlignLeft, or Qt::AlignRight)
	 */
	void setAlignment(int align);


private slots:
	//! Let the user select another font
	void customFont();
	//! Let the user pick the background color
	void pickBackgroundColor();
	//! Let the user pick the text color
	void pickTextColor();
	//! Accept changes and close dialog
	void accept();
	//! Apply changes
	void apply();
	
	//! Format seleted text to subscript
	void addIndex();
	//! Format seleted text to superscript
	void addExp();
	//! Format seleted text to underlined
	void addUnderline();
	//! Format seleted text to italics
	void addItalic();
	//! Format seleted text to bold
	void addBold();
	//! Insert curve marker into the text
	void addCurve();

	//! Let the user insert lower case greek letters
	void showLowerGreek();
	//! Let the user insert capital greek letters
	void showUpperGreek();
	//! Let the user insert mathematical symbols
	void showMathSymbols();
	//! Let the user insert arrow symbols
	void showArrowSymbols();
	//! Insert 'letter' into the text
	void addSymbol(const QString& letter);

signals:
	//! Emit all current values
	/**
	 * \param text the label text
	 * \param angle the rotation angle
	 * \param bkg the background type
	 * \param fnt the text font
	 * \param textColor the text color
	 * \param backgroundColor the backgroundcolor
	 */
	void values(const QString& text, int angle, int bkg, const QFont& fnt, const QColor& textColor, const QColor& backgroundColor);
	
	//! Signal for axes labels: change text
	void changeText(const QString &);
	//! Signal for axes labels: change text color
	void changeColor(const QColor &);
	//! Signal for axes labels: change text alignment
	void changeAlignment(int);
	//! Signal for axes labels: change font
	void changeFont(const QFont &);

protected:
	QFont selectedFont;
	TextType textType;

	ColorButton *colorBtn, *backgroundBtn;
	QPushButton* buttonFont;
	QComboBox* frameBox;
	QPushButton* buttonOk;
	QPushButton* buttonApply;
	QComboBox* rotateBox;
	QPushButton* buttonCancel;
	QPushButton* buttonIndex;
	QPushButton* buttonExp;
	QPushButton* buttonSym;
	QPushButton* buttonB;
	QPushButton* buttonI;
	QPushButton* buttonU;
	QPushButton* buttonCurve;
	QTextEdit* lineEdit;
	QGroupBox *groupBox1, *groupBox2;
	QPushButton* buttonLowerGreek, *buttonUpperGreek;
	QPushButton* buttonMathSymbols, *buttonArrowSymbols;
	QComboBox* alignmentBox;

private:
	//! Internal function: format selected text with prefix and postfix
	void formatText(const QString & prefix, const QString & postfix);

};

#endif // TEXTDLG_H
