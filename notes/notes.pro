include(../common.pri)
TEMPLATE = lib
CONFIG += plugin static
INCLUDEPATH += .. ../core
TARGET = ../$$qtLibraryTarget(scidavis_notes)

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

HEADERS += \
	NotesModule.h \
	Notes.h \

SOURCES += \
	NotesModule.cpp \
	Notes.cpp \

