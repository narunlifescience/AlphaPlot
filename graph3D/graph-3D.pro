include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static opengl
INCLUDEPATH += .. ../core ../core/column ../lib ../core/datatypes
DEPENDPATH += .. ../core ../core/column ../lib ../core/datatypes
TARGET = ../$$qtLibraryTarget(scidavis_graph3D)
QT += xml opengl

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

HEADERS += \
	Graph3D.h \
	Graph3DModule.h \
	Bar3D.h \
	Cone3D.h \
	../core/MyParser.h

#	FunctionDialog3D.h \
#	PlotDialog3D.h \

SOURCES += \
	Graph3D.cpp \
	Graph3DModule.cpp \
	Bar3D.cpp \
	Cone3D.cpp \
	../core/MyParser.cpp
	
#	FunctionDialog3D.cpp \
#	PlotDialog3D.cpp \

