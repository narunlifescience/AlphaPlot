TEMPLATE=lib
CONFIG+=staticlib uic
TARGET=scidavis

include(../config.pri)

INCLUDEPATH += ../scidavis

liborigin {
  INCLUDEPATH += ../3rdparty/liborigin
}

CONFIG        += qt warn_on exceptions opengl thread zlib
CONFIG        += assistant

DEFINES       += QT_PLUGIN
DEFINES       += TS_PATH="\\\"$$replace(translationfiles.path," ","\\ ")\\\""
DEFINES       += DOC_PATH="\\\"$$replace(documentation.path," ","\\ ")\\\""
!isEmpty( manual.path ) {
DEFINES       += MANUAL_PATH="\\\"$$replace(manual.path," ","\\ ")\\\""
}
!isEmpty(plugins.path): DEFINES += PLUGIN_PATH=\\\"$$replace(plugins.path," ","\\ ")\\\"

!mxe {
     win32:DEFINES += QT_DLL QT_THREAD_SUPPORT
}
QT            += opengl qt3support network svg xml

MOC_DIR        = ../tmp/scidavis
OBJECTS_DIR    = ../tmp/scidavis
DESTDIR        = ./

include( sourcefiles.pri )
include( muparser.pri )
python {include( python.pri )}



#############################################################################
#############################################################################
