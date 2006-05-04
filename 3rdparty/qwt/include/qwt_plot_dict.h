/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_PLOT_DICT
#define QWT_PLOT_DICT

#include "qwt_global.h"
#include "qwt_plot_classes.h"
#include <qintdict.h>

//
//   Template classes used by QwtPlot
//

template<class type>
class QWT_EXPORT QwtSeqDict : public QIntDict<type>
{
public:
    QwtSeqDict(): QIntDict<type>() {}
    void insert(long key, const type *item) 
    {
        uint prime;
        if ((uint(key) >= this->size()) && (prime = nextPrime(uint(key))))
            this->resize(prime);
        QIntDict<type>::insert(key, item);
    }
private:
    uint nextPrime(uint i) 
    {
        // first primes bigger than 1<<n for n = 2, 3, .., 15
        uint primes[] = 
        {
            17,
            37,
            67,
            131,
            257,
            521,
            1031,
            2053,
            4099,
            8209,
            16411,
            32771,
        };

        for (uint j=0; j<sizeof(primes); j++)
        {
            if (i<primes[j])
                return primes[j];
        }
        
        return 0;
    }
};

class QWT_EXPORT QwtCurveDict : public QwtSeqDict<QwtPlotCurve>
{
public:
    QwtCurveDict() { setAutoDelete(TRUE); }
};

class QWT_EXPORT QwtMarkerDict : public QwtSeqDict<QwtPlotMarker>
{
public:
    QwtMarkerDict() { setAutoDelete(TRUE); }
};

typedef QIntDictIterator<QwtPlotCurve> QwtPlotCurveIterator;
typedef QIntDictIterator<QwtPlotMarker> QwtPlotMarkerIterator;

#endif
