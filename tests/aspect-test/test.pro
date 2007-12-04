TEMPLATE = app
CONFIG += debug
TARGET = 
QT += xml
DEPENDPATH += . ../../core
INCLUDEPATH += . ../../core

RESOURCES += ../../icons.qrc

HEADERS += \
	AbstractAspect.h \
	ProjectWindow.h \
	aspectcommands.h \
	AspectPrivate.h \
	AspectTreeModel.h \
	Notes.h \
	Project.h \
	MdiSubWindow.h \
	Folder.h \
	ProjectExplorer.h \

SOURCES += \
	AbstractAspect.cpp \
	ProjectWindow.cpp \
	AspectPrivate.cpp \
	AspectTreeModel.cpp \
	main.cpp \
	Notes.cpp \
	Project.cpp \
	MdiSubWindow.cpp \
	Folder.cpp \
	ProjectExplorer.cpp \

