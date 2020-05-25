set( current_dir "table" )

set( HEADERS ${HEADERS}
  "${current_dir}/AsciiTableImportFilter.h"
  "${current_dir}/future_SortDialog.h"
  "${current_dir}/future_Table.h"
  "${current_dir}/tablecommands.h"
  "${current_dir}/TableCommentsHeaderModel.h"
  "${current_dir}/TableDoubleHeaderView.h"
  "${current_dir}/TableItemDelegate.h"
  "${current_dir}/TableModel.h"
  "${current_dir}/TableView.h"
  PARENT_SCOPE
  )
set( SRCS ${SRCS}
  "${current_dir}/AsciiTableImportFilter.cpp"
  "${current_dir}/future_SortDialog.cpp"
  "${current_dir}/future_Table.cpp"
  "${current_dir}/tablecommands.cpp"
  "${current_dir}/TableCommentsHeaderModel.cpp"
  "${current_dir}/TableDoubleHeaderView.cpp"
  "${current_dir}/TableItemDelegate.cpp"
  "${current_dir}/TableModel.cpp"
  "${current_dir}/TableView.cpp"
  PARENT_SCOPE
  )
set( FORMS ${FORMS}
  "${current_dir}/controltabs.ui"
  PARENT_SCOPE
  )
