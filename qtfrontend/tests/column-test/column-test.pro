TEMPLATE = app
TARGET = column-test
CONFIG += debug
QT += xml network
DEFINES += SUPPRESS_SCRIPTING_INIT
DEPENDPATH += . .. ../.. ../../lib ../../core ../../core/datatypes ../../core/column ../../core/filters
INCLUDEPATH += . .. ../.. ../../lib ../../core ../../core/datatypes ../../core/column ../../core/filters 
unix:LIBS += -lcppunit

FORMS += \
	ProjectConfigPage.ui \

# units used
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
#			  SimpleMappingFilter.h \
			  Project.h \
			  Folder.h \
			  ProjectWindow.h \
			  AbstractPart.h \
			  PartMdiView.h \
			  ProjectExplorer.h \
			  AspectTreeModel.h \
			  XmlStreamReader.h \
			  ScriptingEngineManager.h \
			  ProjectConfigPage.h \
    		  ConfigPageWidget.h \
			  ShortcutsDialogModel.h \
			  RecordShortcutDelegate.h \
			  ActionManager.h \
			  ShortcutsDialog.h \
			  ImportDialog.h \
			  ExtensibleFileDialog.h \


SOURCES += \
			  AbstractAspect.cpp \
			  AspectPrivate.cpp \
			  globals.cpp \
			  String2DateTimeFilter.cpp \
			  AbstractFilter.cpp \
			  AbstractSimpleFilter.cpp \
			  Column.cpp \
			  ColumnPrivate.cpp \
			  columncommands.cpp \
#			  SimpleMappingFilter.cpp \
			  DateTime2StringFilter.cpp \
			  Double2StringFilter.cpp \
			  Project.cpp \
			  Folder.cpp \
			  ProjectWindow.cpp \
			  AbstractPart.cpp \
			  PartMdiView.cpp \
			  ProjectExplorer.cpp \
			  AspectTreeModel.cpp \
			  XmlStreamReader.cpp \
			  ScriptingEngineManager.cpp \
			  ProjectConfigPage.cpp \
    		  ConfigPageWidget.cpp \
			  ShortcutsDialogModel.cpp \
			  RecordShortcutDelegate.cpp \
			  ActionManager.cpp \
			  ShortcutsDialog.cpp \
			  ImportDialog.cpp \
			  ExtensibleFileDialog.cpp \

# test cases
HEADERS += \
	assertion_traits.h \

SOURCES += main.cpp \
	ColumnTest.cpp \
	


