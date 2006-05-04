/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

// vim: expandtab

#ifndef QWT_GLOBAL_H
#define QWT_GLOBAL_H

#include <qglobal.h>
#if QT_VERSION < 400
#include <qmodules.h>
#endif

#define QWT_VERSION       0x040200
#define QWT_VERSION_STR   "4.2.0"

//
// Create Qwt DLL if QWT_DLL is defined (Windows only)
//

#if defined(Q_WS_WIN)

#if defined(_MSC_VER) /* MSVC Compiler */
/* template-class specialization 'identifier' is already instantiated */
#pragma warning(disable: 4660)
#endif

#if defined(QWT_NODLL)
#undef QWT_MAKEDLL
#undef QWT_DLL
#undef QWT_TEMPLATEDLL
#endif

#ifdef QWT_DLL
#if defined(QWT_MAKEDLL)     /* create a Qwt DLL library */
#undef QWT_DLL
#define QWT_EXPORT  __declspec(dllexport)
#define QWT_TEMPLATEDLL
#endif
#endif

#if defined(QWT_DLL)     /* use a Qwt DLL library */
#define QWT_EXPORT  __declspec(dllimport)
#define QWT_TEMPLATEDLL
#endif

#else // ! Q_WS_WIN
#undef QWT_MAKEDLL       /* ignore these for other platforms */
#undef QWT_DLL
#undef QWT_TEMPLATEDLL
#endif

#ifndef QWT_EXPORT
#define QWT_EXPORT
#endif

#if QT_VERSION < 300
// Use old QArray instead of QMemArray
#define QWT_NO_MEMARRAY 1
#endif

#if defined(Q_WS_X11) 
#if (QT_VERSION >= 300) && (QT_VERSION < 0x040000)

// Raster operations with texts don´t work together with antialiasing.
// On X11 it doesn´t work with Xft at all, even if antialiasing
// is disabled. QWT_BROKEN_RASTEROP_FONT disables font antialiasing
// whenever raster operations are set. All other texts are not affected.
// We hope that TrollTech will have added a similar workaround 
// to Qt >= 4.x. 

#ifndef QT_NO_XFTFREETYPE
#define QWT_BROKEN_RASTEROP_FONT 1
#endif

#endif
#endif

// #define QWT_NO_COMPAT 1 // disable withdrawn functionality

#endif // QWT_GLOBAL_H
