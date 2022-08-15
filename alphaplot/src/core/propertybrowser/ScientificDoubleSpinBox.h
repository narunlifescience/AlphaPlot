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

// Author:    Arun Narayanankutty
//
#ifndef SCIENTIFICDOUBLESPINBOX_H
#define SCIENTIFICDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include <QValidator>

static bool isIntermediateValueHelper(qint64 num, qint64 minimum,
                                      qint64 maximum, qint64 *match = 0);

class ScientificDoubleSpinBox : public QDoubleSpinBox {
  Q_OBJECT

 public:
  ScientificDoubleSpinBox(QWidget *parent = nullptr);
  virtual ~ScientificDoubleSpinBox();

  int decimals() const;
  void setDecimals(int value);

  QString textFromValue(double value) const;
  double valueFromText(const QString &text) const;

 signals:
  void scientificdoubleValueChanged(const double value);

 private:
  int dispDecimals;
  QChar delimiter, thousand;
  QDoubleValidator *v;
  bool valuechanged_;
  double value_;

 private:
  void setScientificDoubleValue(const double val) { value_ = val; }
  void initLocalValues(QWidget *parent);
  bool isIntermediateValue(const QString &str) const;
  QVariant validateAndInterpret(QString &input, int &pos,
                                QValidator::State &state) const;
  QValidator::State validate(QString &text, int &pos) const;
  void fixup(QString &input) const;
  QString stripped(const QString &t, int *pos) const;
  double round(double value) const;
  void stepBy(int steps);

 public slots:
  void stepDown();
  void stepUp();
};

#endif
