set( current_dir "matrix" )

set( HEADERS ${HEADERS}
  "${current_dir}/future_Matrix.h"
  "${current_dir}/matrixcommands.h"
  "${current_dir}/MatrixModel.h"
  "${current_dir}/MatrixView.h"
  PARENT_SCOPE
  )
set( SRCS ${SRCS}
  "${current_dir}/future_Matrix.cpp"
  "${current_dir}/matrixcommands.cpp"
  "${current_dir}/MatrixModel.cpp"
  "${current_dir}/MatrixView.cpp"
  PARENT_SCOPE
  )
set( FORMS ${FORMS}
  "${current_dir}/matrixcontroltabs.ui"
  PARENT_SCOPE
  )
