include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static
INCLUDEPATH += .. ../core ../table ../matrix ../graph
TARGET = ../$$qtLibraryTarget(scidavis_origin)
QT += xml

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

!dynamic_liborigin{
INCLUDEPATH  += ../3rdparty/liborigin

HEADERS += ../3rdparty/liborigin/OPJFile.h
SOURCES += ../3rdparty/liborigin/OPJFile.cpp
}

HEADERS += \
	OpjImporter.h \

SOURCES += \
	OpjImporter.cpp \

