
include_directories(
  "${CMAKE_SOURCE_DIR}/alphaplot"
  "${CMAKE_SOURCE_DIR}/alphaplot/src"
  "${CMAKE_SOURCE_DIR}/alphaplot/src/future"
  "${CMAKE_BINARY_DIR}/alphaplot/src/alphaplot_autogen/include"
  )

find_package( Qt5 COMPONENTS Test REQUIRED )

add_executable( readWriteProject "readWriteProject.cpp" )
target_link_libraries( readWriteProject
  Qt5::Widgets
  Qt5::Script
  Qt5::Test
  )
target_compile_definitions( readWriteProject PRIVATE LEGACY_CODE_0_2_x )

add_test( NAME readWriteProject
  COMMAND "./readWriteProject" )
