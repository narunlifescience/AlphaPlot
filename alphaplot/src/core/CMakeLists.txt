set( current_dir "core" )

set( HEADERS ${HEADERS}
  "${current_dir}/AprojHandler.h"
  "${current_dir}/IconLoader.h"
  "${current_dir}/Utilities.h"
  PARENT_SCOPE
  )
set( SRCS ${SRCS}
  "${current_dir}/AprojHandler.cpp"
  "${current_dir}/IconLoader.cpp"
  "${current_dir}/Utilities.cpp"
  PARENT_SCOPE
  )
