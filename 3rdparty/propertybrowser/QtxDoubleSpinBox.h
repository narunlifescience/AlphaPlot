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

// File:      QtxDoubleSpinBox.h
// Author:    Sergey TELKOV
//
#ifndef QTXDOUBLESPINBOX_H
#define QTXDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include <QValidator>

class QtxDoubleSpinBox : public QDoubleSpinBox {
  Q_OBJECT

 public:
  QtxDoubleSpinBox(QWidget* = nullptr);
  QtxDoubleSpinBox(double, double, double = 1, QWidget* = nullptr);
  QtxDoubleSpinBox(double, double, double, int, int, QWidget* = nullptr);
  virtual ~QtxDoubleSpinBox();

  bool isCleared() const;
  virtual void setCleared(const bool);

  int getPrecision() const;
  void setPrecision(const int);

  virtual void stepBy(int);

  virtual double valueFromText(const QString&) const;
  virtual QString textFromValue(double) const;

  virtual QValidator::State validate(QString&, int&) const;

 protected slots:
  virtual void onTextChanged(const QString&);

 protected:
  QString removeTrailingZeroes(const QString&) const;

 private:
  bool myCleared;
  int myPrecision;
};

#endif
