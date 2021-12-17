// Copyright (C) 2007-2021  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email :
// webmaster.salome@opencascade.com
//

// File:      QtxDoubleSpinBox.cxx
// Author:    Sergey TELKOV
//
#include "QtxDoubleSpinBox.h"

#include <QDoubleValidator>
#include <QLineEdit>
#include <QVariant>
#include <limits>

const double PSEUDO_ZERO = 1.e-20;

/*!
  \class QtxDoubleSpinBox
  \brief Enhanced version of the Qt's double spin box.

  The QtxDoubleSpinBox class represents the widget for entering the
  floating point values. In addition to the functionality provided by
  QDoubleSpinBox, this class supports "cleared" state - this is the
  state corresponding to "None" (or empty) entered value.

  To set "cleared" state use setCleared() method. To check if the spin
  box stores "cleared" state, use isCleared() method.
  For example:
  \code
  if (myDblSpinBox->isCleared()) {
    ... // process "None" state
  }
  else {
    double value = myDblSpinBox->value();
    ... // process entered value
  }
  \endcode

  Another useful feature is possibility to use scientific notation
  (e.g. 1.234e+18) for the widegt text. To enable this, negative precision
  should be specified either through a constructor or using setPrecision()
  method.

  Note that "decimals" property of QDoubleSpinBox is almost completely
  substituted by "myPrecision" field of QtxDoubleSpinBox class. "decimals" is
  still used for proper size hint calculation and for rounding minimum and
  maximum bounds of the spin box range.
*/

/*!
  \brief Constructor.

  Constructs a spin box with 0.0 as minimum value and 99.99 as maximum value,
  a step value of 1.0 and a precision of 2 decimal places.
  The value is initially set to 0.00.

  \param parent parent object
*/
QtxDoubleSpinBox::QtxDoubleSpinBox(QWidget* parent)
    : QDoubleSpinBox(parent), myCleared(false) {
#if !defined(GLOBAL_DOUBLE_CONVERSION)
  // VSR 01/07/2010: Disable thousands separator for spin box
  // (to avoid incosistency of double-2-string and string-2-double conversion)
  // see issue 14540 (old id 21219)
  QLocale loc;
  loc.setNumberOptions(loc.numberOptions() | QLocale::OmitGroupSeparator |
                       QLocale::RejectGroupSeparator);
  setLocale(loc);
#endif

  // Use precision equal to default Qt decimals
  myPrecision = decimals();

  connect(lineEdit(), &QLineEdit::textChanged, this,
          &QtxDoubleSpinBox::onTextChanged);
}

/*!
  \brief Constructor.

  Constructs a spin box with specified minimum, maximum and step value.
  The precision is set to 2 decimal places.
  The value is initially set to the minimum value.

  \param min spin box minimum possible value
  \param max spin box maximum possible value
  \param step spin box increment/decrement value
  \param parent parent object
*/
QtxDoubleSpinBox::QtxDoubleSpinBox(double min, double max, double step,
                                   QWidget* parent)
    : QDoubleSpinBox(parent), myCleared(false) {
#if !defined(GLOBAL_DOUBLE_CONVERSION)
  // VSR 01/07/2010: Disable thousands separator for spin box
  // (to avoid incosistency of double-2-string and string-2-double conversion)
  // see issue 14540 (old id 21219)
  QLocale loc;
  loc.setNumberOptions(loc.numberOptions() | QLocale::OmitGroupSeparator |
                       QLocale::RejectGroupSeparator);
  setLocale(loc);
#endif

  // Use precision equal to default Qt decimals
  myPrecision = decimals();

  setMinimum(min);
  setMaximum(max);
  setSingleStep(step);

  connect(lineEdit(), &QLineEdit::textChanged, this,
          &QtxDoubleSpinBox::onTextChanged);
}

/*!
  \brief Constructor.

  Constructs a spin box with specified minimum, maximum and step value.
  The precision is set to <prec> decimal places.
  The value is initially set to the minimum value.

  \param min spin box minimum possible value
  \param max spin box maximum possible value
  \param step spin box increment/decrement value
  \param prec non-negative values means the number of digits after the decimal
  point, negative value means the maximum number of significant digits for the
  scientific notation \param dec number of digits after the decimal point passed
  to base Qt class (used for correct control sizing only!) \param parent parent
  object
*/
QtxDoubleSpinBox::QtxDoubleSpinBox(double min, double max, double step,
                                   int prec, int dec, QWidget* parent)
    : QDoubleSpinBox(parent), myCleared(false), myPrecision(prec) {
#if !defined(GLOBAL_DOUBLE_CONVERSION)
  // VSR 01/07/2010: Disable thousands separator for spin box
  // (to avoid incosistency of double-2-string and string-2-double conversion)
  // see issue 14540 (old id 21219)
  QLocale loc;
  loc.setNumberOptions(loc.numberOptions() | QLocale::OmitGroupSeparator |
                       QLocale::RejectGroupSeparator);
  setLocale(loc);
#endif

  setDecimals(dec);
  setMinimum(min);
  setMaximum(max);
  setSingleStep(step);

  connect(lineEdit(), &QLineEdit::textChanged, this,
          &QtxDoubleSpinBox::onTextChanged);
}

/*!
  \brief Destructor.
*/
QtxDoubleSpinBox::~QtxDoubleSpinBox() {}

/*!
  \brief Check if spin box is in the "cleared" state.
  \return \c true if spin box is cleared
  \sa setCleared()
*/
bool QtxDoubleSpinBox::isCleared() const { return myCleared; }

/*!
  \brief Change "cleared" status of the spin box.
  \param on new "cleared" status
  \sa isCleared()
*/
void QtxDoubleSpinBox::setCleared(const bool on) {
  if (myCleared == on) return;

  myCleared = on;
  setSpecialValueText(specialValueText());
}

/*!
  \brief Set precision of the spin box

  If precision value is less than 0, the 'g' format is used for value output,
  otherwise 'f' format is used.

  \param prec new precision value.
  \sa precision()
*/
void QtxDoubleSpinBox::setPrecision(const int prec) {
  int newPrec = qMax(prec, 0);
  int oldPrec = qMax(myPrecision, 0);
  myPrecision = prec;
  if (newPrec != oldPrec) update();
}

/*!
  \brief Get precision value of the spin box
  \return current prevision value
  \sa setPrecision()
*/
int QtxDoubleSpinBox::getPrecision() const { return myPrecision; }

/*!
  \brief Interpret text entered by the user as a value.
  \param text text entered by the user
  \return mapped value
  \sa textFromValue()
*/
double QtxDoubleSpinBox::valueFromText(const QString& text) const {
  if (myPrecision < 0) return text.toDouble();

  return QDoubleSpinBox::valueFromText(text);
}

/*!
  \brief This function is used by the spin box whenever it needs to display
  the given value.

  \param val spin box value
  \return text representation of the value
  \sa valueFromText()
*/
QString QtxDoubleSpinBox::textFromValue(double val) const {
  QString s =
      locale().toString(val, myPrecision >= 0 ? 'f' : 'g', qAbs(myPrecision));
  return removeTrailingZeroes(s);
}

/*!
  \brief Return source string with removed leading and trailing zeros.
  \param str source string
  \return resulting string
*/
QString QtxDoubleSpinBox::removeTrailingZeroes(const QString& src) const {
  QString delim(locale().decimalPoint());

  int idx = src.lastIndexOf(delim);
  if (idx == -1) return src;

  QString iPart = src.left(idx);
  QString fPart = src.mid(idx + 1);
  QString ePart = "";
  int idx1 = fPart.lastIndexOf(QRegExp("e[+|-]?[0-9]+"));
  if (idx1 >= 0) {
    ePart = fPart.mid(idx1);
    fPart = fPart.left(idx1);
  }

  fPart.remove(QRegExp("0+$"));

  QString res = iPart;
  if (!fPart.isEmpty()) res += delim + fPart;
  res += ePart;

  return res;
}

/*!
  \brief Perform \a steps increment/decrement steps.

  The \a steps value can be any integer number. If it is > 0,
  the value incrementing is done, otherwise value is decremented
  \a steps times.

  \param steps number of increment/decrement steps
*/
void QtxDoubleSpinBox::stepBy(int steps) {
  myCleared = false;

  QDoubleSpinBox::stepBy(steps);
  double tmpval = value();
  if (qAbs(tmpval) < PSEUDO_ZERO) tmpval = 0.;
  if (tmpval < minimum())
    tmpval = minimum();
  else if (tmpval > maximum())
    tmpval = maximum();
  setValue(tmpval);
}

/*!
  \brief This function is used to determine whether input is valid.
  \param str currently entered value
  \param pos cursor position in the string
  \return validating operation result
*/
QValidator::State QtxDoubleSpinBox::validate(QString& str, int& pos) const {
  QString pref = this->prefix();
  QString suff = this->suffix();
  int overhead = pref.length() + suff.length();
  QValidator::State state = QValidator::Invalid;

  QDoubleValidator v(NULL);

  // If 'g' format is used (myPrecision < 0), then
  // myPrecision - 1 digits are allowed after the decimal point.
  // Otherwise, expect myPrecision digits after the decimal point.
  int decs = myPrecision < 0 ? qAbs(myPrecision) - 1 : myPrecision;

  v.setDecimals(decs);
  v.setBottom(minimum());
  v.setTop(maximum());
  v.setNotation(myPrecision >= 0 ? QDoubleValidator::StandardNotation
                                 : QDoubleValidator::ScientificNotation);

  if (overhead == 0)
    state = v.validate(str, pos);
  else {
    if (str.length() >= overhead && str.startsWith(pref) &&
        str.right(suff.length()) == suff) {
      QString core = str.mid(pref.length(), str.length() - overhead);
      int corePos = pos - pref.length();
      state = v.validate(core, corePos);
      pos = corePos + pref.length();
      str.replace(pref.length(), str.length() - overhead, core);
    } else {
      state = v.validate(str, pos);
      if (state == QValidator::Invalid) {
        QString special = this->specialValueText().trimmed();
        QString candidate = str.trimmed();
        if (special.startsWith(candidate)) {
          if (candidate.length() == special.length())
            state = QValidator::Acceptable;
          else
            state = QValidator::Intermediate;
        }
      }
    }
  }

  // Treat values ouside (min; max) range as Invalid
  // This check is enabled by assigning "strict_validity_check" dynamic property
  // with value "true" to the spin box instance.
  if (state == QValidator::Intermediate) {
    bool isOk;
    double val = str.toDouble(&isOk);
    if (isOk) {
      QVariant propVal = property("strict_validity_check");
      if (propVal.isValid() && propVal.canConvert(QVariant::Bool) &&
          propVal.toBool()) {
        if (val < minimum() || val > maximum()) state = QValidator::Invalid;
      }
    } else if (myPrecision < 0) {
      // Consider too large negative exponent as Invalid
      QChar e(locale().exponential());
      int epos = str.indexOf(e, 0, Qt::CaseInsensitive);
      if (epos != -1) {
        epos++;  // Skip exponential symbol itself
        QString exponent = str.right(str.length() - epos);
        int expValue = exponent.toInt(&isOk);
        if (isOk && expValue < std::numeric_limits<double>::min_exponent10)
          state = QValidator::Invalid;
      }
    }
  }

  return state;
}

/*!
  \brief Called when user enters the text in the spin box.
  \param txt current spin box text (not used)
*/
void QtxDoubleSpinBox::onTextChanged(const QString& /*txt*/) {
  myCleared = false;
}
