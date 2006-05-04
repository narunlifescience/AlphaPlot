/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_H
#define QWT_H

#include "qwt_global.h"

#undef Left
#undef Right
#undef Top
#undef Bottom
#undef Center

/*!
  Some constants for use within Qwt.
*/
class QWT_EXPORT Qwt 
{
public:
  /*!
    \brief The position of a widget.
    These constants can be OR'ed together.
  */
  enum Position {
    Left = 0x01, 
    Right = 0x02, 
    Top = 0x04, 
    Bottom = 0x08, 
    Center = 0x10
  };
  
#ifndef QWT_NO_COMPAT
  /*!
    \todo Documentation
  */
  enum Shape
    {
      HLine,
      VLine,
      Cross,
      DiagCross,
      Rect,
      Ellipse,
      Triangle,
      UTriangle,
      DTriangle,
      LTriangle,
      RTriangle
    };
#endif
  
  /*!
    \todo Documentation
  */
  enum Errors {
    ErrNone, // No Error
    ErrNoMem, // Not enough memory
    ErrMono // Non-monotonic sequence
  };
};

#endif
