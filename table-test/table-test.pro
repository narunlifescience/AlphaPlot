TEMPLATE = app
TARGET = table-test
CONFIG += debug
DEPENDPATH += . .. ../lib ../core ../core/datatypes ../core/column ../table
INCLUDEPATH += . .. ../lib ../core ../core/datatypes ../core/column ../table
unix:LIBS += -lcppunit

HEADERS += \
			  globals.h \
			  AbstractAspect.h \
			  aspectcommands.h \
			  AspectModel.h \
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
			  TableModel.h \


SOURCES += \
			  AbstractAspect.cpp \
			  AspectModel.cpp \
			  globals.cpp \
			  String2DateTimeFilter.cpp \
			  AbstractFilter.cpp \
			  Column.cpp \
			  ColumnPrivate.cpp \
			  columncommands.cpp \
			  TableModel.cpp


SOURCES += main.cpp \
	TableTest.cpp \
	


