TEMPLATE = app
TARGET = table-test
CONFIG += debug
QT += xml
DEPENDPATH += . ../.. ../../lib ../../core ../../core/datatypes ../../core/filters ../../core/column ../../table ..
INCLUDEPATH += . ../.. ../../lib ../../core ../../core/datatypes ../../core/filters ../../core/column ../../table ..
unix:LIBS += -lcppunit
QMAKE_CXX = distcc

RESOURCES += \
	appicons.qrc \
	icons.qrc \

FORMS += optionstabs.ui \


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
			  TableView.h \
			  TableItemDelegate.h \
			  TableModel.h \
			  Table.h \
			  tablecommands.h \
			  assertion_traits.h\
			  AbstractScriptingEngine.h \
			  test_wrappers.h \
			  Project.h \
			  aspect-test/Folder.h \
			  ProjectWindow.h \
			  AspectTreeModel.h \
			  ProjectExplorer.h \
			  SortDialog.h \
			  TableDoubleHeaderView.h \
			  TableCommentsHeaderModel.h  \
			  SimpleMappingFilter.h \



SOURCES += \
			  AbstractAspect.cpp \
			  AspectPrivate.cpp \
			  globals.cpp \
			  AbstractFilter.cpp \
			  Column.cpp \
			  ColumnPrivate.cpp \
			  columncommands.cpp \
			  TableView.cpp \
			  TableItemDelegate.cpp \
			  Table.cpp \
			  tablecommands.cpp \
			  TableModel.cpp \
			  AbstractScriptingEngine.cpp \
			  Project.cpp \
			  aspect-test/Folder.cpp \
			  ProjectWindow.cpp \
			  AspectTreeModel.cpp \
			  ProjectExplorer.cpp \
			  SortDialog.cpp \
			  TableDoubleHeaderView.cpp \
			  TableCommentsHeaderModel.cpp  \
			  SimpleMappingFilter.cpp \
			  DateTime2StringFilter.cpp \
			  String2DateTimeFilter.cpp \
			  Double2StringFilter.cpp \
		


SOURCES += main.cpp \
	TableTest.cpp \
	


