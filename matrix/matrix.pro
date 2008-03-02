include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static
INCLUDEPATH += ../core ../core/datatypes
TARGET = ../$$qtLibraryTarget(scidavis_matrix)
QT += xml svg

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

SOURCES += \
	Matrix.cpp \
	MatrixDialog.cpp \
	MatrixSizeDialog.cpp \
	MatrixValuesDialog.cpp \

HEADERS += \
	Matrix.h \
	MatrixDialog.h \
	MatrixSizeDialog.h \
	MatrixValuesDialog.h \

