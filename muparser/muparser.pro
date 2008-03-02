include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static exceptions
INCLUDEPATH += ../core ../table ../matrix
TARGET = ../$$qtLibraryTarget(scidavis_muparser)

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

HEADERS += \
	MuParserScript.h \
	MuParserScriptingEngine.h \

SOURCES += \
	MuParserScript.cpp \
	MuParserScriptingEngine.cpp \
