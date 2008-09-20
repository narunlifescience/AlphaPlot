include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static
DEPENDPATH += . .. ../../backend/notes .. ../../backend ../core ../../backend/core
INCLUDEPATH += .. ../../backend
TARGET = ../$$qtLibraryTarget(scidavis_notes)
QT += xml

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

FORMS += NotesConfigPage.ui

HEADERS += \
	NotesModule.h \
	Notes.h \
	# TODO: port missing features from Note.h and delete it

SOURCES += \
	NotesModule.cpp \
	Notes.cpp \
	# TODO: port missing features from Note.cpp and delete it

