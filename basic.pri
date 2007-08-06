#############################################################################
###################### BASIC PROJECT PROPERTIES #############################
#############################################################################

QMAKE_PROJECT_DEPTH = 0

TARGET         = scidavis
TEMPLATE       = app
CONFIG        += qt warn_on exceptions opengl thread
CONFIG        += assistant

DEFINES       += QT_PLUGIN
win32:DEFINES += QT_DLL QT_THREAD_SUPPORT
QT            += opengl qt3support network svg

MOC_DIR        = ../tmp/scidavis
OBJECTS_DIR    = ../tmp/scidavis
DESTDIR        = ./

