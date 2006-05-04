/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_SCLDIV_H
#define QWT_SCLDIV_H

#include "qwt_global.h"
#include "qwt_array.h"

#if defined(QWT_TEMPLATEDLL)
// MOC_SKIP_BEGIN
template class QWT_EXPORT QwtArray<double>;
// MOC_SKIP_END
#endif

/*!
  \brief A class for building scale divisions
  
  The QwtScaleDiv class can build
  linear and logarithmic scale divisions for specified
  intervals. It uses an adjustable algorithm to
  generate the major and minor step widths automatically.
  A scale division has a minimum value, a maximum value,
  a vector of major marks, and a vector of minor marks.

  \warning QwtScaleDiv uses implicit sharing for the mark vectors.
  \par Example
  To build a logarithmic scale division from 0.01 to 1000
  and print out the major and minor marks. \verbatim
#include "../include/qwt_scldiv.h>
#include <iostream.h>

main()
{
  int i,k;
  QwtScaleDiv sd;

  sd.rebuild(0.01, 100, 10, 10, TRUE, 0.0);

  k=0;
  for (i=0;i<sd.majCnt();i++)
  {
    while(k < sd.minCnt())
    {
      if (sd.minMark(k) < sd.majMark(i))
      {
        cout << " - " << sd.minMark(i) << "\n";
    k++;
      }
      else
        break;
    }
    cout << "-- " << sd.majMark(i) << "\n";
  }
  while(k < sd.minCnt())
  {
    cout << " - " << sd.minMark(i) << "\n";
    k++;
  }
}\endverbatim
*/

class QWT_EXPORT QwtScaleDiv
{
public:
    QwtScaleDiv ();

    int operator== (const QwtScaleDiv &s) const;
    int operator!= (const QwtScaleDiv &s) const;
    
    /*! 
        \return left bound
        \sa QwtScaleDiv::rebuild
     */
    double lBound() const { return d_lBound; }
    /*! 
        \return right bound
        \sa QwtScaleDiv::rebuild
     */
    double hBound() const { return d_hBound; }
    /*! 
        \return minor mark count
        \sa QwtScaleDiv::rebuild
     */
    uint minCnt() const { return d_minMarks.size(); }
    /*! 
        \return major mark count
        \sa QwtScaleDiv::rebuild
     */
    uint majCnt() const { return d_majMarks.size(); }

    /*! 
        \return TRUE id ths scale div is logarithmic
        \sa QwtScaleDiv::rebuild
     */
    bool logScale() const { return d_log; }

    //! Return major mark at position i
    double majMark(int i) const { return d_majMarks[i]; }
    //! Return minor mark at position i
    double minMark(int i) const { return d_minMarks[i]; }

    /*! 
        \return major step size
        \sa QwtScaleDiv::rebuild
     */
    double majStep() const { return d_majStep; }
    void reset();

    bool rebuild(double lBound, double hBound, int maxMaj, int maxMin,
             bool log, double step = 0.0, bool ascend = TRUE);
 
private:
    bool buildLinDiv(int maxMajMark, int maxMinMark, double step = 0.0);
    bool buildLogDiv(int maxMajMark, int maxMinMark, double step = 0.0);
   
    double d_lBound;
    double d_hBound;
    double d_majStep;
    bool d_log;

    QwtArray<double> d_majMarks;
    QwtArray<double> d_minMarks;
};

#endif
