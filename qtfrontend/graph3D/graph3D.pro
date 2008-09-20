include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static opengl
INCLUDEPATH += .. ../../backend
DEPENDPATH += . .. ../../backend ../../backend/graph3D ../../backend/core ../core ../lib ../../backend/lib
TARGET = ../$$qtLibraryTarget(scidavis_graph3D)
QT += xml opengl

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

FORMS += Graph3DConfigPage.ui

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

