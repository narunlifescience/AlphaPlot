include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static
INCLUDEPATH += .. ../core
TARGET = ../$$qtLibraryTarget(scidavis_notes)
QT += xml

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

HEADERS += \
	NotesModule.h \
	Notes.h \
	# TODO: port missing features from Note.h and delete it

SOURCES += \
	NotesModule.cpp \
	Notes.cpp \
	# TODO: port missing features from Note.cpp and delete it

