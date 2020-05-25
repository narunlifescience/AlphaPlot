set( current_dir "core" )

set( HEADERS ${HEADERS}
  "${current_dir}/AbstractAspect.h"
  "${current_dir}/AbstractColumn.h"
  "${current_dir}/AbstractFilter.h"
  "${current_dir}/AbstractImportFilter.h"
  "${current_dir}/AbstractPart.h"
  "${current_dir}/AbstractSimpleFilter.h"
  "${current_dir}/AspectPrivate.h"
  "${current_dir}/aspectcommands.h"
  "${current_dir}/future_Folder.h"
  "${current_dir}/interfaces.h"
  "${current_dir}/PartMdiView.h"
  "${current_dir}/Project.h"
  "${current_dir}/ProjectConfigPage.h"
  "${current_dir}/column/Column.h"
  "${current_dir}/column/ColumnPrivate.h"
  "${current_dir}/column/columncommands.h"
  "${current_dir}/datatypes/DateTime2DoubleFilter.h"
  "${current_dir}/datatypes/DateTime2StringFilter.h"
  "${current_dir}/datatypes/DayOfWeek2DoubleFilter.h"
  "${current_dir}/datatypes/Double2DateTimeFilter.h"
  "${current_dir}/datatypes/Double2DayOfWeekFilter.h"
  "${current_dir}/datatypes/Double2MonthFilter.h"
  "${current_dir}/datatypes/Double2StringFilter.h"
  "${current_dir}/datatypes/Month2DoubleFilter.h"
  "${current_dir}/datatypes/String2DateTimeFilter.h"
  "${current_dir}/datatypes/String2DayOfWeekFilter.h"
  "${current_dir}/datatypes/String2DoubleFilter.h"
  "${current_dir}/datatypes/String2MonthFilter.h"
  "${current_dir}/datatypes/SimpleCopyThroughFilter.h"
  PARENT_SCOPE
  )
set( SRCS ${SRCS}
  "${current_dir}/AbstractAspect.cpp"
  "${current_dir}/AbstractPart.cpp"
  "${current_dir}/AbstractSimpleFilter.cpp"
  "${current_dir}/AbstractFilter.cpp"
  "${current_dir}/AspectPrivate.cpp"
  "${current_dir}/future_Folder.cpp"
  "${current_dir}/PartMdiView.cpp"
  "${current_dir}/Project.cpp"
  "${current_dir}/ProjectConfigPage.cpp"
  "${current_dir}/column/Column.cpp"
  "${current_dir}/column/columncommands.cpp"
  "${current_dir}/column/ColumnPrivate.cpp"
  "${current_dir}/datatypes/DateTime2StringFilter.cpp"
  "${current_dir}/datatypes/Double2DateTimeFilter.cpp"
  "${current_dir}/datatypes/Double2StringFilter.cpp"
  "${current_dir}/datatypes/String2DateTimeFilter.cpp"
  PARENT_SCOPE
  )
set( FORMS ${FORMS}
  "${current_dir}/ProjectConfigPage.ui"
  PARENT_SCOPE
  )
