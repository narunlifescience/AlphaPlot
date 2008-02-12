TEMPLATE = app
TARGET = scidavis
CONFIG += debug
QT += xml
DEPENDPATH += core lib core/datatypes core/column core/filters table
INCLUDEPATH += core lib core/datatypes core/column core/filters table

MOC_DIR        = tmp
OBJECTS_DIR    = tmp

RESOURCES += \
	appicons.qrc \
	icons.qrc \

FORMS += optionstabs.ui \

HEADERS += \
	Notes.h \
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
#	AbstractScriptingEngine.h \
	Project.h \
	Folder.h \
	ProjectWindow.h \
	AspectTreeModel.h \
	AspectView.h \
	ProjectExplorer.h \
	SortDialog.h \
	TableFormulaEditorView.h \
	TableFormulaEditorModel.h \
	TableDoubleHeaderView.h \
	TableCommentsHeaderModel.h  \
	SimpleMappingFilter.h \

SOURCES += \
	main.cpp \
	Notes.cpp \
	Folder.cpp \
	AbstractAspect.cpp \
	AspectPrivate.cpp \
	globals.cpp \
	AbstractFilter.cpp \
	AbstractSimpleFilter.cpp \
	Column.cpp \
	ColumnPrivate.cpp \
	columncommands.cpp \
	TableView.cpp \
	TableItemDelegate.cpp \
	Table.cpp \
	tablecommands.cpp \
	TableModel.cpp \
#	AbstractScriptingEngine.cpp \
	Project.cpp \
	ProjectWindow.cpp \
	AspectTreeModel.cpp \
	AspectView.cpp \
	ProjectExplorer.cpp \
	SortDialog.cpp \
	TableFormulaEditorView.cpp \
	TableFormulaEditorModel.cpp \
	TableDoubleHeaderView.cpp \
	TableCommentsHeaderModel.cpp  \
	SimpleMappingFilter.cpp \
	DateTime2StringFilter.cpp \
	String2DateTimeFilter.cpp \
	Double2StringFilter.cpp \

