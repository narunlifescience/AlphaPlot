/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is QWMF - Qt WMF export class.
 *
 * The Initial Developer of the Original Code is
 * Tobias Burnus <burnus_at-sign_net-b_dot_de>.
 * Portions created by the Initial Developer are Copyright (C) 2004
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include "qwmf.h"
#include <qpaintdevicemetrics.h>
#include <qcolor.h>
#include <qbrush.h>
#include <qpen.h>
#include <qfont.h>
#include <qpointarray.h>

#include <gsl/gsl_math.h>

/** Build DWORD (Q_UINT32) together from four BYTES (Q_UINT8)
 * @param a  First read byte
 * @param b  Second read byte
 * @param c  Third read byte
 * @param d  Fourth read byte
 *
 * Since Microsoft has desided to store WORDs/DWORDs in WMFs in the reversed order,
 * we need to revert the order.
 * 
 * @return Created DWORD (uint32)
 */
inline Q_UINT32 QWMF::wmf_4int_dword(unsigned a, unsigned b, unsigned c, unsigned d) {
  return  (((Q_UINT32) d) << 24) + (((Q_UINT32) c) << 16) + (((Q_UINT32) b) << 8) + a;
}

/** Build WORD (Q_UINT16) together from two BYTES (Q_UINT8)
 * @param a  First read byte
 * @param b  Second read byte
 *
 * Since Microsoft has desided to store WORDs/DWORDs in WMFs in the reversed order,
 * we need to revert the order.
 * 
 * @return Created WORD (uint16)
 */
inline Q_UINT32 QWMF::wmf_2int_word(unsigned a, unsigned b) {
  return (((Q_UINT32) b) << 8) + a;
}

void QWMF::Init()  {
  myFile    = new QFile();
  myInch    = 72*3;
  myWidth   = 4*myInch;
  myHeight  = 4*myInch;
  myWMatrix.reset();
  myFontWMat.reset();
  usingWMat = false;
}

QWMF::QWMF(const QString & fileName) : QPaintDevice(QInternal::ExternalDevice)  {
   Init();
   myFile->setName(fileName);
}

QWMF::QWMF() : QPaintDevice(QInternal::ExternalDevice) {
   Init();
}

QWMF::~QWMF() {
   delete(myFile);
}

void QWMF::setOutputFileName(const QString &fileName) {
   myFile->close();
   myFile->setName(fileName);
}

// FIXME: Speedup by using integer arithmetic
void QWMF::transform_xy(QPoint &pt) {
  if(usingWMat && !myWMatrix.isIdentity()) {
    pt.setX((int)(myWMatrix.dx()+(double)pt.x()*myWMatrix.m11()+myWMatrix.m12()*pt.y()));
    pt.setY((int)(myWMatrix.dy()+(double)pt.y()*myWMatrix.m22()+myWMatrix.m21()*pt.x()));
  }
}

// FIXME: Speedup by using integer arithmetic
void QWMF::transform_rect(QRect &rect) {
//  if(!usingWMat || myWMatrix.isIdentity()) return;
//  if(myWMatrix.transformationMode == QWMatrix::Points) {
  if(usingWMat && !myWMatrix.isIdentity()) {
  rect.setLeft(  (int)(myWMatrix.dx()+(double)rect.left()  *myWMatrix.m11()+myWMatrix.m12()*rect.left()));
  rect.setTop(   (int)(myWMatrix.dy()+(double)rect.top()   *myWMatrix.m22()+myWMatrix.m21()*rect.top()));
  rect.setRight( (int)(myWMatrix.dx()+(double)rect.right() *myWMatrix.m11()+myWMatrix.m12()*rect.right()));
  rect.setBottom((int)(myWMatrix.dy()+(double)rect.bottom()*myWMatrix.m22()+myWMatrix.m21()*rect.bottom()));
//  } else {
  }
}

bool QWMF::setFont(const QFont *font) {
    qWarning("Setting font");
    Q_UINT32 angle = 0;
    Q_UINT16 w;
    Q_UINT8 b1,b2;
    myFontWMat = myWMatrix;
    if(!myWMatrix.isIdentity()) {
       // We assume no shearing for now
       // since the first line is sx*cos(a) sx*sin(a), we can remove the
       // scaling factor by using sx² = (sx*cos(a))²+(sx*sin(a))²
       angle = (Q_UINT32)(10*180/M_PI
                *acos(myWMatrix.m11()
                        /sqrt(myWMatrix.m11()*myWMatrix.m11()+myWMatrix.m12()*myWMatrix.m12())));
    }

    myRecSize  = max(myRecSize, 28);
    write_dword(28);
    write_word(0x02fb);
    if(font->pointSize() > 0) // 1 pt = 1/72.27 inch
       w = (Q_UINT32)((float)myInch/72.27*(float)font->pointSize());
    else
       w = font->pixelSize();
    write_word(w); // height
    write_word(0x0); // width
    write_word(angle); // escapement
    write_word(angle); // orientation
    write_word(font->weight()*10); // weight
    write_word(wmf_2int_word(font->italic() ? 1 : 0, font->underline() ? 1 : 0));
    write_word(wmf_2int_word(font->strikeOut() ? 1 : 0, 0)); // strikeOut and ANSI_CHARSET
    write_word(wmf_2int_word(0,0)); // OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS
    write_word(wmf_2int_word(0, //DEFAULT_QUALITY
                             font->fixedPitch()? 1:2)); // FIXED_PITCH vs. VARIABLE_PITCH
    // Write first 32 characters of the fontname
    QString fnt = font->family();
    for(Q_UINT32 dw = 0; dw < 32; dw +=2) {
      b1 = fnt.at(dw).latin1(); // returns '\0' when longer than string
      b2 = fnt.at(dw+1).latin1();
      write_word(wmf_2int_word(b1,b2));
    }
    // META_SELECTOBJECT
    write_dword(4);
    write_word(0x012d);
    write_word(ObjectCounter);
    ObjectCounter++;
    return true;
}

bool QWMF::cmd (int c, QPainter *, QPDevCmdParam *p) {
  Q_UINT32 dw;
  Q_UINT16 w, w2;
  Q_UINT8  b1,b2;
  QRect rect;
  QPoint pt;
  int i;
  if(myFile->name().isEmpty()) return false;
  switch(c) {
  case PdcBegin:      // begin; write header
    if (!myFile->open(IO_Truncate|IO_WriteOnly)) return false;
    ObjectCounter = 0;
    myRecSize  = 5;
    writeHeader();
    return true;
  case PdcEnd: // end write header
    // Write empty record
    write_dword(3);
    write_word(0x0);
    writeHeader();
    myFile->close();
    return true;
  case PdcSave:
    write_dword(3);
    write_word(0x001E);
    return true;
  case PdcRestore:
    write_dword(3);
    write_word(0x0127);
    return true;
  case PdcSetROP: // Raster operations
    write_dword(4);
    write_word(0x104);
    switch(p[0].ival) {
      case Qt::CopyROP: w = 13; break; // R2_COPYPEN
//      case Qt::OrROP:
      case Qt::XorROP:  w = 7;  break; // R2_XORPEN
//      case Qt::NotAndROP:
//      case Qt::EraseROP:
      default:          w = 13;
         qWarning( "QWMF::cmd: CopyROP unsupported: %d", c );
                         break; // R2_COPYPEN
    }
    write_word(w); 
    return true;

/*  case PdcSetVXform:
  case PdcSetWindow:
  case PdcRestoreWMatrix:
  case PdcSetViewport:*/
  case PdcSetWXform:
    usingWMat = p[0].ival;
    qWarning( "QWMF::cmd: PdcSetWXform: %s", usingWMat?"enabled":"disabled");
    return true;
  case PdcSetWMatrix:
    myWMatrix = *p[0].matrix;
    qWarning( "QWMF::cmd: PdcSetWMatrix called");
    usingWMat = true;
    return true;
  case PdcSetFont:
    myFont = p[0].font;
    return setFont(myFont);
  case PdcSetBkMode:
    write_dword(4);
    write_word(0x102);
    write_word(Qt::TransparentMode == p[0].ival ? 1 : 2); // TRANSPARENT = 1; OPARQUE = 2
    return true;
  case PdcDrawText2:
  case PdcDrawText:
    qWarning("PdcDrawText: Writing %s", p[1].str->ascii());
    if(myWMatrix != myFontWMat) {
        setFont(myFont);
    }
    //Set text alignment
    write_dword(4);
    write_word(0x012E);
    write_word(0x18); // TA_BASELINE

    myRecSize  = max(myRecSize, 6+(int)(p[1].str->length()+1)/2);
    write_dword(6+(int)(p[1].str->length()+1)/2);
    write_word(0x0521);
    write_word(p[1].str->length());
    for(dw = 0; dw < p[1].str->length(); dw +=2) {
      b1 = p[1].str->at(dw).latin1(); // returns '\0' when longer than string
      b2 = p[1].str->at(dw+1).latin1();
      write_word(wmf_2int_word(b1,b2));
    }
    pt = *p[0].point;
    transform_xy(pt);
    write_word(pt.y());
    write_word(pt.x());
    return true;
  case PdcDrawText2Formatted:
  case PdcDrawTextFormatted:
    if(myWMatrix != myFontWMat) {
        setFont(myFont);
    }
    //Set text alignment
    w = 0x0;
    qWarning("PdcDrawTextFormatted: Writing %s", p[2].str->ascii());
    if(p[1].ival & Qt::AlignLeft)    w = 0x00;
    if(p[1].ival & Qt::AlignRight)   w = 0x02;
    if(p[1].ival & Qt::AlignCenter)  w = 0x06;
    if(p[1].ival & Qt::AlignJustify)
       qWarning( "QWMF::cmd: PdcDrawText2Formatted Qt::AlignJustify not supported");
    if(p[1].ival & Qt::AlignTop)     w |= 0x00;
    if(p[1].ival & Qt::AlignBottom)  w |= 0x08;
    if(p[1].ival & Qt::AlignVCenter) {
       qWarning( "QWMF::cmd: PdcDrawText2Formatted Qt::AlignVCenter not supported, using TA_BASELINE");
       w |= 0x18;
    }
    write_dword(4);
    write_word(0x012E);
    write_word(w);

    if(p[1].ival & Qt::DontClip) {
       myRecSize  = max(myRecSize, 7+(int)(p[2].str->length()+1)/2);
       write_dword(7+4+(p[2].str->length()+1)/2);
    } else {
       myRecSize  = max(myRecSize, 11+(int)(p[2].str->length()+1)/2);
       write_dword(11+(p[2].str->length()+1)/2);
    }
    rect = *p[0].rect;
    pt = QPoint(rect.x(),rect.y());
    transform_rect(rect);
    write_word(0x0a32);
    write_word(pt.y());  // Is this the right (x,y) ?
    write_word(pt.x());
    write_word(p[2].str->length());
    if(p[1].ival & Qt::DontClip) {
       write_word(0x000);
    } else { // ETO_CLIPPED
       write_word(0x0002);
       write_word(rect.bottom());
       write_word(rect.right());
       write_word(rect.top());
       write_word(rect.left());
    }
    for(dw = 0; dw < p[2].str->length(); dw +=2) {
      b1 = p[2].str->at(dw).latin1(); // returns '\0' when longer than string
      b2 = p[2].str->at(dw+1).latin1();
      write_word(wmf_2int_word(b1,b2));
    }
    return true;
  case PdcDrawPie:
    rect = *p[0].rect;
    double rad,height,width;
    myRecSize  = max(myRecSize, 11);
    write_dword(11);
    write_word(0x081A);
    rad = sqrt( (rect.top()-rect.bottom())*(rect.top()-rect.bottom())
             +(rect.right()-rect.left())*(rect.right()-rect.left()));
    height = 0.5*rect.top()+0.5*rect.bottom();
    width  = 0.5*rect.left()+0.5*rect.right();
    write_word((int)(-sin((p[2].ival+p[1].ival)/16.*M_PI/180.)*rad+height));
    write_word((int)(cos((p[2].ival+p[1].ival)/16.*M_PI/180.)*rad+width));
    write_word((int)(-sin(p[1].ival/16.*M_PI/180.)*rad+height));
    write_word((int)(cos(p[1].ival/16.*M_PI/180.)*rad+width));
    write_word(rect.bottom());
    write_word(rect.right());
    write_word(rect.top());
    write_word(rect.left());
    return true;
  case PdcDrawRect:
    rect = *p[0].rect;
    qWarning("PdcDrawRect: before (%d,%d)-(%d,%d)",rect.left(),rect.top(),rect.right(),rect.bottom());
    transform_rect(rect);
    qWarning("PdcDrawRect: after  (%d,%d)-(%d,%d)",rect.left(),rect.top(),rect.right(),rect.bottom());
    myRecSize  = max(myRecSize, 7);
    write_dword(7);
    write_word(0x041B);
    write_word(rect.bottom());
    write_word(rect.right());
    write_word(rect.top());
    write_word(rect.left());
    return true;
  case PdcDrawPolygon:
    if ( p[0].ptarr->size() > 0 ) {
      QPointArray a = *p[0].ptarr;
      myRecSize  = max(myRecSize, 7);
      // SETPOLYFILLMODE
      write_dword(4);
      write_word(0x0106);
      write_word(p[0].ival); // 1 = ALTERNATE, 2 = WINDING
      // POLYGON
      write_dword(4+2*a.size());
      write_word(0x0324);
      write_word(a.size());
      for(i = 0; i < (int)a.size(); i++) {
        pt = a.point(i);
        transform_xy(pt);
        write_word(pt.x());
        write_word(pt.y());
      }
    }
    return true;
  case PdcDrawEllipse:
    myRecSize  = max(myRecSize, 7);
    write_dword(7);
    write_word(0x0418);
    write_word(p[0].rect->bottom());
    write_word(p[0].rect->right());
    write_word(p[0].rect->top());
    write_word(p[0].rect->left());
    return true;
  case PdcMoveTo:
    pt = *p[0].point;
    transform_xy(pt); 
    myRecSize  = max(myRecSize, 5);
    write_dword(5); // MoveTo
    write_word(0x0214);
    write_word(pt.y());
    write_word(pt.x());
    return true;
  case PdcLineTo:
    pt = *p[0].point;
    transform_xy(pt); 
    myRecSize  = max(myRecSize, 5);
    write_dword(5); // LineTo
    write_word(0x0213);
    write_word(pt.y()); // FIXME: Check wether this point is included or not
    write_word(pt.x()); // In Windows/WMF it is _NOT_
    return true;
  case PdcDrawPoint:
    pt = *p[0].point;
    transform_xy(pt); 
    myRecSize  = max(myRecSize, 5);
    write_dword(5); // MoveTo
    write_word(0x0214);
    write_word(pt.y());
    write_word(pt.x());

    write_dword(5); // LineTo (this point is excluded)
    write_word(0x0213);
    write_word(pt.y()+1);
    write_word(pt.x());
    return true;
  case PdcDrawLine:
    pt = *p[0].point;
    transform_xy(pt); 
    myRecSize  = max(myRecSize, 5);
    write_dword(5); // MoveTo
    write_word(0x0214);
    write_word(pt.y());
    write_word(pt.x());

    pt = *p[1].point;
    transform_xy(pt); 
    write_dword(5); // LineTo
    write_word(0x0213);
    write_word(pt.y());
    write_word(pt.x());
 
    write_dword(5); // LineTo -- correct for off by one error
    write_word(0x0213);
    write_word(pt.y()+1);
    write_word(pt.x());

    return true;
  case PdcDrawPolyline: {
        QPointArray a = *p[0].ptarr;
        myRecSize  = max(myRecSize, 4+2*a.size());
        write_dword(4+2*a.size());
        write_word(0x0325);
        write_word(a.size());
        for (dw=0; dw < a.size(); dw++ ) {
            pt = a.point(dw);
            transform_xy(pt);
            write_word(pt.x());
            write_word(pt.y());
        }
    }
    return true;
  case PdcDrawLineSegments:
    if ( p[0].ptarr->size() > 0 ) {
        QPointArray a = *p[0].ptarr;
        myRecSize  = max(myRecSize, 5);
        for (dw=0; dw < a.size(); dw+=2 ) {

            pt = a.point(dw);
            transform_xy(pt);
            write_dword(5); // MoveTo
            write_word(0x0214);
            write_word(pt.y());
            write_word(pt.x());

            pt = a.point(dw+1);
            transform_xy(pt);
            write_dword(5); // LineTo
            write_word(0x0213);
            write_word(pt.y());
            write_word(pt.x());

            write_dword(5); // LineTo -- correct for off-by-one error
            write_word(0x0213);
            write_word(pt.y()+1);
            write_word(pt.x());
        }
    }
    return true;
  case PdcSetBkColor:
    myRecSize  = max(myRecSize, 5);
    write_dword(5);
    write_word(0x0201);
    write_dword(winrgb(*p[0].color));
    return true;
/*  case PdcDrawImage) { // FIXME: Should work along these lines
     PdcDrawPixmap should be analogous ; need to test
     
     write_word(0x0d33);//META_SETDIBTODEV  ???
     QImage setFormat( "BMP" ); and skip 14(?) byte long header
*/
  case PdcSetBrush:
    myRecSize  = max(myRecSize, 7);
    write_dword(7);
    write_word(0x02FC);
    w  = 0; // BS_SOLID
    w2 = 0; // HS_HORIZONTAL
    switch(p[0].brush->style()) {
       case Qt::NoBrush:           w = 1; break; // BS_HOLLOW = BS_NULL
       case Qt::SolidPattern:      w = 0; break; // BS_SOLID
       case Qt::HorPattern:        w2 = 0; w = 2; break; // HS_HORIZONTAL
       case Qt::VerPattern:        w2 = 1; w = 2; break; // HS_VERTICAL
       case Qt::FDiagPattern:      w2 = 2; w = 2; break; // HS_FDIAGONAL
       case Qt::BDiagPattern:      w2 = 3; w = 2; break; // HS_FDIAGONAL
       case Qt::CrossPattern:      w2 = 4; w = 2; break; // HS_CROSS
       case Qt::DiagCrossPattern:  w2 = 5; w = 2; break; // HS_CROSS
       default:
         //FIXME: Handle Qt::Dense*Pattern
         qWarning( "QWMF::cmd: BrushStyle not handled: %d", p[0].brush->style());
 
    }
    write_word(w); //style
    write_dword(winrgb(p[0].brush->color()));
    write_word(w2); // Hatch
    // META_SELECTOBJECT
    write_dword(4);
    write_word(0x012d);
    write_word(ObjectCounter);
    ObjectCounter++;
    return true; 
  case PdcSetPen:
    myRecSize  = max(myRecSize, 8);
    write_dword(8);
    write_word(0x02FA);
    write_word(p[0].pen->width()); // x width
    write_word(0); // y width (is ignored!)
    w = 0; // BS_SOLID
    switch(p[0].pen->style()) { 
      case Qt::NoPen:          w = 5; break;
      case Qt::SolidLine:      w = 0; break;
      case Qt::DashLine:       w = 1; break; 
      case Qt::DotLine:        w = 2; break;
      case Qt::DashDotLine:    w = 3; break;
      case Qt::DashDotDotLine: w =  4; break;
      case Qt::MPenStyle:
         qWarning( "QWMF::cmd: PdcSetPen: Qt::MPenStyle not supported");
      default:
         qWarning( "QWMF::cmd: PdcSetPen: style %c not supported", p[0].pen->style());
    }
    write_word(0);
    write_dword(winrgb(p[0].pen->color()));
    // META_SELECTOBJECT
    write_dword(4);
    write_word(0x012d);
    write_word(ObjectCounter);
    ObjectCounter++;
    // META_SETTEXTCOLOR
    write_dword(5);
    write_word(0x0209);
    write_dword(winrgb(p[0].pen->color()));
    return true; 
  case PdcDrawRoundRect:
      qWarning( "QWMF::cmd: unsupported command: PdcDrawRoundRect - %d", c ); break;
  case PdcDrawArc:
      qWarning( "QWMF::cmd: unsupported command: PdcDrawArc - %d", c ); break;
  case PdcDrawChord:
      qWarning( "QWMF::cmd: unsupported command: PdcDrawChord - %d", c ); break;
  case PdcDrawCubicBezier:
      qWarning( "QWMF::cmd: unsupported command: PdcDrawCubicBezier - %d", c ); break;
  case PdcDrawPixmap:
      qWarning( "QWMF::cmd: unsupported command: PdcDrawPixmap - %d", c ); break;
  case PdcDrawImage:
      qWarning( "QWMF::cmd: unsupported command: PdcDrawImage - %d", c ); break;
  case PdcDrawTextItem:
      qWarning( "QWMF::cmd: unsupported command: PdcDrawTextItem - %d", c ); break;

  case PdcSetBrushOrigin:
      qWarning( "QWMF::cmd: unsupported command: PdcSetBrushOrigin - %d", c ); break;
  case PdcSetTabStops:
      qWarning( "QWMF::cmd: unsupported command: PdcSetTabStops - %d", c ); break;
  case PdcSetTabArray:
      qWarning( "QWMF::cmd: unsupported command: PdcSetTabArray - %d", c ); break;
  case PdcSetdev:
      qWarning( "QWMF::cmd: unsupported command: PdcSetdev - %d", c ); break;
  case PdcSetUnit:
      qWarning( "QWMF::cmd: unsupported command: PdcSetUnit - %d", c ); break;
  case PdcSetVXform:
      qWarning( "QWMF::cmd: unsupported command: PdcSetVXform - %d", c ); break;
  case PdcSetWindow:
      qWarning( "QWMF::cmd: unsupported command: PdcSetWindow - %d", c ); break;
  case PdcSetViewport:
      qWarning( "QWMF::cmd: unsupported command: PdcSetViewport - %d", c ); break;
  case PdcSaveWMatrix:
      qWarning( "QWMF::cmd: unsupported command: PdcSaveWMatrix - %d", c ); break;
  case PdcRestoreWMatrix:
      qWarning( "QWMF::cmd: unsupported command: PdcRestoreWMatrix - %d", c ); break;
  case PdcSetClip:
      qWarning( "QWMF::cmd: unsupported command: PdcSetClip - %d", c ); break;
  case PdcSetClipRegion:
      qWarning( "QWMF::cmd: unsupported command: PdcSetClipRegion - %d", c );
      return false; // use META_CREATEREGION, META_SELECTCLIPREGION
  default:
      qWarning( "QWMF::cmd: unsupported command: %d", c ); break;
  }
  return false;
}

inline Q_UINT32 QWMF::max(Q_UINT32 a, Q_UINT32 b) {
   return (a > b)?a:b;
}

/*int QWMF::metric(int m) const
{
    switch(m) {
        case QPaintDeviceMetrics::PdmWidth:
            return myWidth;
        case QPaintDeviceMetrics::PdmHeight:
            return myHeight;
        case QPaintDeviceMetrics::PdmWidthMM:
            return (int)(myWidth*25.40/(double)myInch);
        case QPaintDeviceMetrics::PdmHeightMM:
            return (int)(myHeight*25.40/(double)myInch);
        case QPaintDeviceMetrics::PdmDpiX:
        case QPaintDeviceMetrics::PdmPhysicalDpiX:
            return myInch;
        case QPaintDeviceMetrics::PdmDpiY:
        case QPaintDeviceMetrics::PdmPhysicalDpiY:
            return myInch;
        case QPaintDeviceMetrics::PdmNumColors:
            return 16777216; // 2^24
        case QPaintDeviceMetrics::PdmDepth:
            return 24; // 24 bit = 0x 00 rr gg bb
        default:
#if defined(QT_CHECK_RANGE)
            qWarning( "QWMF::metric: Invalid metric command" );
#endif
            return 0;
    }
}*/

int QWMF::metric( int m ) const
{
    int val;
    switch ( m ) {
    case QPaintDeviceMetrics::PdmWidth:
        val = myWidth;
//        if ( myInch != 72 )
//            val = (val * myInch + 36) / 72;
        break;
    case QPaintDeviceMetrics::PdmHeight:
        val = myHeight;
//        if ( myInch != 72 )
//            val = (val * myInch + 36) / 72;
        break;
    case QPaintDeviceMetrics::PdmDpiX:
        val = myInch;
        break;
    case QPaintDeviceMetrics::PdmDpiY:
        val = myInch;
        break;
        case QPaintDeviceMetrics::PdmPhysicalDpiX:
        case QPaintDeviceMetrics::PdmPhysicalDpiY:
            val = 72;
            break;
    case QPaintDeviceMetrics::PdmWidthMM:
        // double rounding error here.  hooray.
        val = metric( QPaintDeviceMetrics::PdmWidth );
        val = (val * 254 + 5*myInch) / (10*myInch);
        break;
    case QPaintDeviceMetrics::PdmHeightMM:
        val = metric( QPaintDeviceMetrics::PdmHeight );
        val = (val * 254 + 5*myInch) / (10*myInch);
        break;
    case QPaintDeviceMetrics::PdmNumColors:
        val = 16777216;
        break;
    case QPaintDeviceMetrics::PdmDepth:
        val = 24;
        break;
    default:
        val = 0;
#if defined(QT_CHECK_RANGE)
        qWarning( "QPixmap::metric: Invalid metric command" );
#endif
    }
    return val;
}

void QWMF::setWidth(int width) {
    myWidth = width;
}

void QWMF::setHeight(int height) {
    myHeight = height;
}

void QWMF::setInch(int inch) {
    myInch = inch;
}

int QWMF::width() {
    return myWidth;
}

int QWMF::height() {
    return myHeight;
}

int QWMF::inch() {
    return myInch;
}

int QWMF::read_dword() {
  int a, b, c, d;
  a = myFile->getch();
  if(a < 0) return -1;
  b = myFile->getch();
  if(b < 0) return -1;
  c = myFile->getch();
  if(c < 0) return -1;
  d = myFile->getch();
  if(d < 0) return -1;
  return wmf_4int_dword(a,b,c,d);
}

int QWMF::read_word() {
  int a, b;
  a = myFile->getch();
  if(a < 0) return -1;
  b = myFile->getch();
  if(b < 0) return -1;
  return wmf_2int_word(a,b);
}

int QWMF::write_dword(Q_UINT32 val) {
  Q_UINT32 a, b, c, d;
  int i;
  d = ((val >> 24) & 0x000000FF);
  c = ((val >> 16) & 0x000000FF);
  b = ((val >>  8) & 0x000000FF);
  a = ((val      ) & 0x000000FF);
  i = myFile->putch(a);
  if(i < 0) return -1;
  i = myFile->putch(b);
  if(i < 0) return -1;
  i = myFile->putch(c);
  if(i < 0) return -1;
  i = myFile->putch(d);
  if(i < 0) return -1;
  return val;
}

int QWMF::write_word(Q_UINT32 val) {
  Q_UINT16 a, b;
  int i;
  b = ((val >>  8) & 0x000000FF);
  a = ((val      ) & 0x000000FF);
  i = myFile->putch(a);
  if(i < -1) return -1;
  i = myFile->putch(b);
  if(i < 0) return -1;
  return val;
}

Q_UINT32 QWMF::winrgb(QColor color) {
    Q_UINT32 c;

    c =    (color.red()   & 0xFF);
    c += ( (color.green() & 0xFF) << 8 );
    c += ( (color.blue()  & 0xFF) << 16 );

    return c;
}
/*Q_UINT32 QWMF::winrgb(const QColor *color) {
    return  (((Q_UINT32) color->blue() ) << 16)& 0x00FF0000
           +(((Q_UINT32) color->green()) << 8) & 0x0000FF00
           +  (Q_UINT32) color->red()          & 0x000000FF;
}*/

bool QWMF::writeHeader() {
   Q_UINT16 w;
   // Emulate seek: myFile->seek(0L);
   myFile->close(); myFile->open(IO_Raw|IO_WriteOnly);
   // Aldus Placeable Metafile header
   write_dword(0x9AC6CDD7); // Key:    Magic Number
   write_word(0);           // Handle: always 0 
   write_word(0);           // Left;   FIXME: Should be unsigned
   write_word(0);           // Top;    FIXME: Should be unsigned
   write_word(myWidth);           // Right;  FIXME: Should be unsigned
   write_word(myHeight);           // Bottom; FIXME: Should be unsigned
   write_word(myInch);      // Inch;   Number of metafile units per inch 
   write_dword(0);          // Reserved
   //calculate checksum
   w  = ((0x9AC6CDD7 & 0x0000FFFFUL) ^ ((0x9AC6CDD7 & 0xFFFF0000UL) >> 16));
   w ^= (myWidth ^ myHeight ^ myInch);
   w ^= ((0 & 0x0000FFFFUL) ^ ((0 & 0x0000FFFFUL) >> 16));
   write_word(w);
   // WindowsMetaHeader
   write_word(1);           // FileType; 0=memory, 1=disk
   write_word(9);           // HeaderSize; always 9
   write_word(0x0300);      // Version; of Windows
   write_dword(myFile->size()/2-11); //FileSize in WORDs w/o Aldus Placeable Metafile header
   write_word(ObjectCounter);  // Number of objects in the file
   write_dword(myRecSize);  // MaxRecordSize; largest record in WORDs
   write_word(0);           // NumOfParams; not used and thus always 0
   // META_SETWINDOWORG
   write_dword(5);
   write_word(0x20b);
   write_word(0); // y
   write_word(0); // x
   // META_SETWINDOWEXT
   write_dword(5);
   write_word(0x20c);
   write_word(myHeight); // height
   write_word(myWidth); // width
// Emulate seek: myFile->seek(myFile->size());
   myFile->close(); myFile->open(IO_Append | IO_WriteOnly);
   return true;
}
