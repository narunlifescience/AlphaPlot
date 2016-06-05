/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_VALUELIST_H
#define QWT_VALUELIST_H

#include "qwt_global.h"

/*!
  \def QwtValueList
 */

#if QT_VERSION < 0x040000

#include <qvaluelist.h>

typedef QValueList<double> QwtValueList;

#else // QT_VERSION >= 0x040000

#include <qlist.h>

typedef QList<double> QwtValueList;

#endif

#endif
