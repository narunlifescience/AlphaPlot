#############################################################################
###################### USER-SERVICEABLE PART ################################
#############################################################################

# building without muParser does not work yet (but will in a future version)
SCRIPTING_LANGS += muParser
# comment out the following line to deactivate Python scripting support
SCRIPTING_LANGS += Python  

# a console displaying output of scripts; particularly useful on Windows
# where running SciDAVis from a terminal is inconvenient
DEFINES         += SCRIPTING_CONSOLE
# a dialog for selecting the scripting language on a per-project basis
DEFINES         += SCRIPTING_DIALOG

CONFIG          += release
# use the next line to compile with debugging symbols instead of the line obove
#CONFIG          += debug

# what to install and where
INSTALLS        += target
INSTALLS        += documentation
unix: target.path = /usr/bin
unix: documentation.path = /usr/share/doc/scidavis
win32: target.path = c:/scidavis
win32: documentation.path = c:/scidavis/doc

linux-g++-64: libsuff = 64

#############################################################################
############ Default settings for Linux #####################################
############ (dynamic linking) ##############################################
############ should also work on Mac OS X ###################################
#############################################################################
unix:INCLUDEPATH       += ../3rdparty/liborigin
unix:LIBS         += -L /usr/lib$${libsuff}
# dynamically link against Qwt(3D) installed system-wide
# WARNING: make sure they are compiled against >= Qt4.2
# Mixing Qt 4.2 and Qt >= 4.3 compiled stuff may also 
# cause problems.
unix:LIBS         += -lqwtplot3d
unix:LIBS         += -lqwt
#dynamically link against GSL installed system-wide
unix:LIBS         += -lgsl -lgslcblas -lz

#############################################################################
#############################################################################

#############################################################################
############ Default settings for Windows ###################################
############ (static linking mostly, except Qt and Python) ##################
#############################################################################

win32:INCLUDEPATH       += c:/qwtplot3d/include
win32:INCLUDEPATH       += c:/qwt-5.0.2/include
win32:INCLUDEPATH       += ../3rdparty/liborigin
win32:INCLUDEPATH       += c:/gsl/include
win32:INCLUDEPATH       += d:/zlib/include

win32:LIBS        += c:/qwtplot3d/lib/libqwtplot3d.a
win32:LIBS        += c:/qwt-5.0.2/lib/libqwt.a
win32:LIBS        += c:/gsl/lib/libgsl.a
win32:LIBS        += c:/gsl/lib/libgslcblas.a
win32:LIBS        += c:/zlib/lib/libz.a

#############################################################################
#############################################################################

#############################################################################
##################### Another example configuration #########################
##################### for mixed dynamic/static linking ######################
##################### on Linux ##############################################
#############################################################################

#INCLUDEPATH       += ../3rdparty/liborigin
#LIBS         += -L /usr/lib$${libsuff}
#INCLUDEPATH       += ../3rdparty/qwtplot3d/include
#INCLUDEPATH       += ../3rdparty/qwt/src
#LIBS         += ../3rdparty/qwtplot3d/lib/libqwtplot3d.a
#LIBS         += ../3rdparty/qwt/lib/libqwt.a
#LIBS         += -lgsl -lgslcblas -lz

#############################################################################
###################### END OF USER-SERVICEABLE PART #########################
#############################################################################


#############################################################################
#############################################################################

include( basic.pri )

include( sourcefiles.pri )

contains(SCRIPTING_LANGS, muParser) { 
INCLUDEPATH       += ../3rdparty/muParser
include( muparser.pri )
}

contains(SCRIPTING_LANGS, Python) {
include( python.pri )
}

#############################################################################
#############################################################################
