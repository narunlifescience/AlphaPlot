set( current_dir "3Dplot" )

set( HEADERS ${HEADERS}
  "${current_dir}/Bar3D.h"
  "${current_dir}/Custom3DInteractions.h"
  "${current_dir}/DataManager3D.h"
  "${current_dir}/Graph3DCommon.h"
  "${current_dir}/Layout3D.h"
  "${current_dir}/Scatter3D.h"
  "${current_dir}/Surface3D.h"
  "${current_dir}/SurfaceDialog.h"
  PARENT_SCOPE
  )
set( SRCS ${SRCS}
  "${current_dir}/Bar3D.cpp"
  "${current_dir}/Custom3DInteractions.cpp"
  "${current_dir}/DataManager3D.cpp"
  "${current_dir}/Layout3D.cpp"
  "${current_dir}/Scatter3D.cpp"
  "${current_dir}/Surface3D.cpp"
  "${current_dir}/SurfaceDialog.cpp"
  PARENT_SCOPE
  )
