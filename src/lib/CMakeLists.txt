set( current_dir "lib" )

set( HEADERS ${HEADERS}
  "${current_dir}/ActionManager.h"
  "${current_dir}/ConfigPageWidget.h"
  "${current_dir}/Interval.h"
  "${current_dir}/IntervalAttribute.h"
  "${current_dir}/macros.h"
  "${current_dir}/XmlStreamReader.h"
  "${current_dir}/XmlStreamWriter.h"
  PARENT_SCOPE
  )
set( SRCS ${SRCS}
  "${current_dir}/ActionManager.cpp"
  "${current_dir}/ConfigPageWidget.cpp"
  "${current_dir}/XmlStreamReader.cpp"
  "${current_dir}/XmlStreamWriter.cpp"
  PARENT_SCOPE
  )
