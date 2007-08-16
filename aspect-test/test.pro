TEMPLATE = app
CONFIG += debug
TARGET = 
DEPENDPATH += . ../core
INCLUDEPATH += . ../core

RESOURCES += ../icons.qrc

HEADERS += \
	AbstractAspect.h \
	ProjectWindow.h \
	aspectcommands.h \
	AspectModel.h \
	AspectTreeModel.h \
	Notes.h \
	Project.h \
	MdiSubWindow.h \
	Folder.h \
	ProjectExplorer.h \

SOURCES += \
	AbstractAspect.cpp \
	ProjectWindow.cpp \
	AspectModel.cpp \
	AspectTreeModel.cpp \
	main.cpp \
	Notes.cpp \
	Project.cpp \
	MdiSubWindow.cpp \
	Folder.cpp \
	ProjectExplorer.cpp \

