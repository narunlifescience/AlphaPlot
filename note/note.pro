TEMPLATE = lib
CONFIG += plugin static

INCLUDEPATH += .. ../core
TARGET = $$qtLibraryTarget(scidavis_notes)

MOC_DIR        = ../tmp
OBJECTS_DIR    = ../tmp

HEADERS += \
	NotesModule.h \
	Notes.h \

SOURCES += \
	NotesModule.cpp \
	Notes.cpp \

