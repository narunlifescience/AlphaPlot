TEMPLATE = lib
CONFIG += plugin static
QT += xml
INCLUDEPATH += .. ../core ../core/column ../lib ../core/datatypes
TARGET = $$qtLibraryTarget(scidavis_table)

MOC_DIR        = ../tmp
OBJECTS_DIR    = ../tmp

FORMS += optionstabs.ui \

HEADERS += \
	TableModule.h \
	TableView.h \
	TableItemDelegate.h \
	TableModel.h \
	Table.h \
	tablecommands.h \
	SortDialog.h \
	TableDoubleHeaderView.h \
	TableCommentsHeaderModel.h  \

SOURCES += \
	TableModule.cpp \
	TableView.cpp \
	TableItemDelegate.cpp \
	Table.cpp \
	tablecommands.cpp \
	TableModel.cpp \
	SortDialog.cpp \
	TableDoubleHeaderView.cpp \
	TableCommentsHeaderModel.cpp  \

