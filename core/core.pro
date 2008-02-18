TEMPLATE = app
TARGET = ../scidavis
CONFIG += debug
QT += xml
DEPENDPATH += .. ../lib datatypes column filters
INCLUDEPATH += .. ../lib datatypes column filters

MOC_DIR        = ../tmp
OBJECTS_DIR    = ../tmp

RESOURCES += \
	../appicons.qrc \
	../icons.qrc \

LIBS += -L../table -lscidavis_table
LIBS += -L../note -lscidavis_notes

HEADERS += \
	globals.h \
	AbstractAspect.h \
	aspectcommands.h \
	AspectPrivate.h \
	Interval.h \
	AbstractColumn.h \
	Column.h \
	ColumnPrivate.h \
	columncommands.h \
	IntervalAttribute.h \
	AbstractFilter.h \
	AbstractSimpleFilter.h \
	SimpleCopyThroughFilter.h \
	DateTime2DoubleFilter.h \
	DateTime2StringFilter.h \
	DayOfWeek2DoubleFilter.h \
	Double2DateTimeFilter.h \
	Double2DayOfWeekFilter.h \
	Double2MonthFilter.h \
	Double2StringFilter.h \
	Month2DoubleFilter.h \
	String2DateTimeFilter.h \
	String2DayOfWeekFilter.h \
	String2DoubleFilter.h \
	String2MonthFilter.h \
	interfaces.h \
#	AbstractScriptingEngine.h \
	Project.h \
	Folder.h \
	ProjectWindow.h \
	AspectTreeModel.h \
	AbstractPart.h \
	PartMdiView.h \
	ProjectExplorer.h \
	SimpleMappingFilter.h \

SOURCES += \
	main.cpp \
	Folder.cpp \
	AbstractAspect.cpp \
	AspectPrivate.cpp \
	globals.cpp \
	AbstractFilter.cpp \
	AbstractSimpleFilter.cpp \
	Column.cpp \
	ColumnPrivate.cpp \
	columncommands.cpp \
#	AbstractScriptingEngine.cpp \
	Project.cpp \
	ProjectWindow.cpp \
	AspectTreeModel.cpp \
	AbstractPart.cpp \
	PartMdiView.cpp \
	ProjectExplorer.cpp \
	SimpleMappingFilter.cpp \
	DateTime2StringFilter.cpp \
	String2DateTimeFilter.cpp \
	Double2StringFilter.cpp \
	staticplugins.cpp \
