include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static
INCLUDEPATH += .. ../core ../core/datatypes
DEPENDPATH += .. ../core
TARGET = ../$$qtLibraryTarget(scidavis_matrix)
QT += xml svg

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

FORMS += matrixcontroltabs.ui \

SOURCES += \
	Matrix.cpp \
	MatrixModule.cpp \
	MatrixView.cpp \
	matrixcommands.cpp \
	MatrixModel.cpp \
	AbstractScriptingEngine.cpp \

	# TODO: port or remove the following files
    # MatrixDialog.cpp \
    # MatrixSizeDialog.cpp \
    # MatrixValuesDialog.cpp \

HEADERS += \
	Matrix.h \
	MatrixModule.h \
	MatrixView.h \
	MatrixModel.h \
	matrixcommands.h \
	AbstractScriptingEngine.h \

	# TODO: port or remove the following files
    # MatrixDialog.h \
    # MatrixSizeDialog.h \
    # MatrixValuesDialog.h \

