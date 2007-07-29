### building without muParser does not work yet (but will in a future version)
include( muparser.pri )
### remove the comment char "#" from the following line to activate Python scripting support
#include( python.pri )

### a console displaying output of scripts; particularly useful on Windows
### where running SciDAVis from a terminal is inconvenient
DEFINES         += SCRIPTING_CONSOLE
### a dialog for selecting the scripting language on a per-project basis
DEFINES         += SCRIPTING_DIALOG

CONFIG          += release
### use the next line to compile with debugging symbols instead of the line obove
#CONFIG          += debug

### what to install and where
INSTALLS        += target
INSTALLS        += documentation
unix: INSTALLBASE = /usr
win32: INSTALLBASE = c:/scidavis

unix: target.path = $$INSTALLBASE/bin
unix: documentation.path = $$INSTALLBASE/share/doc/scidavis
win32: target.path = $$INSTALLBASE
win32: documentation.path = $$INSTALLBASE/doc

linux-g++-64: libsuff = 64

### Note to packagers: If you want to use systemwide installed liborigin
### instead of the one provided in "3rdparty", uncomment the following 2 lines:
# CONFIG += dynamic_liborigin
# LIBS += -lorigin
### Unfortunately, due to liborigin being in alpha stage, we cannot guarantee
### that SciDAVis works with any other version that the one in "3rdparty".

#############################################################################
### Default settings for Linux / Mac OS X
#############################################################################
### Link statically against Qwt and Qwtplot3D (in order to make sure they
### are compiled against Qt4), dynamically against everything else.
#############################################################################

unix:INCLUDEPATH  += ../3rdparty/qwtplot3d/include
unix:LIBS         += ../3rdparty/qwtplot3d/lib/libqwtplot3d.a

unix:INCLUDEPATH  += ../3rdparty/qwt/src
unix:LIBS         += ../3rdparty/qwt/lib/libqwt.a

unix:LIBS         += -L /usr/lib$${libsuff}
unix:LIBS         += -lgsl -lgslcblas -lz -lmuparser

#############################################################################
### Link everything dynamically
#############################################################################

#unix:INCLUDEPATH  += /usr/include/qwt5
#unix:LIBS         += -L /usr/lib$${libsuff}
## dynamically link against Qwt(3D) installed system-wide
## WARNING: make sure they are compiled against >= Qt4.2
## Mixing Qt 4.2 and Qt >= 4.3 compiled stuff may also 
## cause problems.
#unix:LIBS         += -lqwtplot3d
#unix:LIBS         += -lqwt
##dynamically link against GSL and zlib installed system-wide
#unix:LIBS         += -lgsl -lgslcblas -lz -lmuparser

#############################################################################
### Default settings for Windows
#############################################################################
### Static linking mostly, except Qt, Python and QwtPlot3D.
### The latter seems to be impossible to link statically on Windows.
#############################################################################

win32:INCLUDEPATH       += c:/qwtplot3d/include
win32:INCLUDEPATH       += c:/qwt-5.0.2/include
win32:INCLUDEPATH       += c:/gsl/include
win32:INCLUDEPATH       += c:/zlib/include
win32:INCLUDEPATH       += c:/muparser/include

win32:LIBS        += c:/qwtplot3d/lib/qwtplot3d.dll
win32:LIBS        += c:/qwt-5.0.2/lib/libqwt.a
win32:LIBS        += c:/gsl/lib/libgsl.a
win32:LIBS        += c:/gsl/lib/libgslcblas.a
win32:LIBS        += c:/zlib/lib/libz.a
win32:LIBS        += c:/muparser/lib/libmuparser.a

#############################################################################
###                    END OF USER-SERVICEABLE PART                       ###
#############################################################################

include( basic.pri )
include( sourcefiles.pri )

#############################################################################
#############################################################################
