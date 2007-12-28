TEMPLATE = app
TARGET = column-test
CONFIG += debug
QT += xml
DEPENDPATH += . .. ../.. ../../lib ../../core ../../core/datatypes ../../core/column ../../core/filters
INCLUDEPATH += . .. ../.. ../../lib ../../core ../../core/datatypes ../../core/column ../../core/filters 
unix:LIBS += -lcppunit

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
			  SimpleMappingFilter.h \


SOURCES += \
			  AbstractAspect.cpp \
			  AspectPrivate.cpp \
			  globals.cpp \
			  String2DateTimeFilter.cpp \
			  AbstractFilter.cpp \
			  Column.cpp \
			  ColumnPrivate.cpp \
			  columncommands.cpp \
			  SimpleMappingFilter.cpp \
			  DateTime2StringFilter.cpp \


# test cases
HEADERS += \
	assertion_traits.h \

SOURCES += main.cpp \
	ColumnTest.cpp \
	


