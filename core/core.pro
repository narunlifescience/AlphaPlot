include(../common.pri)
TEMPLATE = app
TARGET = ../scidavis
DEPENDPATH += .. ../lib datatypes column filters
INCLUDEPATH += .. ../lib datatypes column filters

# For debugging purposes, link modules dynamically and make sure they are found in the directory
# containing the executable. This allows testing changes to one module without re-linking the
# application.
debug:unix:LIBS += -Wl,-rpath,\'\$$ORIGIN\'

# link in modules
LIBS += -L..
for(mod, MODULES):LIBS += -lscidavis_$${mod}
# make moules known to Qt's plugin system (also see staticplugins.cpp)
for(mod, MODULES):mods += Q_IMPORT_PLUGIN(scidavis_$${mod})
DEFINES += IMPORT_SCIDAVIS_MODULES='\'$${mods}\''

RESOURCES += \
	../appicons.qrc \
	../icons.qrc \

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
	AbstractImportFilter.h \
	AbstractExportFilter.h \
	ExtensibleFileDialog.h \
	ImportDialog.h \

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
	ExtensibleFileDialog.cpp \
	ImportDialog.cpp \
