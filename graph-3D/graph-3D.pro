include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static opengl
INCLUDEPATH += ../core ../core/datatypes
TARGET = ../$$qtLibraryTarget(scidavis_graph3d)
QT += xml opengl

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

HEADERS += \
	Graph3D.h \
	Bar3D.h \
	Cone3D.h \
	FunctionDialog3D.h \
	PlotDialog3D.h \

SOURCES += \
	Graph3D.cpp \
	Bar3D.cpp \
	Cone3D.cpp \
	FunctionDialog3D.cpp \
	PlotDialog3D.cpp \

