cmake_minimum_required( VERSION 3.16 )

project( AlphaPlot CXX C )

set( CMAKE_CXX_STANDARD 11)
set( CMAKE_CXX_STANDARD_REQUIRED TRUE )
set( CMAKE_CXX_EXTENSIONS OFF )

find_package( Qt5
  COMPONENTS
  Core
  Gui
  Widgets
  OpenGL
  Svg
  Xml
  Script
  ScriptTools
  PrintSupport
  DataVisualization
  XmlPatterns
  # LinguistTools
  REQUIRED
  )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -Wall -Wextra" )

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra" )

# Searching for updates
option( SEARCH_FOR_UPDATES "Enable Searching for updates" ON )
# Download links
option( DOWNLOAD_LINKS "Enable Download links" ON )
if( SEARCH_FOR_UPDATES OR DOWNLOAD_LINKS )
  find_package( Qt5 COMPONENTS Network REQUIRED )
endif()

# GSL
find_library( GSL_FOUND gsl REQUIRED )
find_library( GSLCBLAS_FOUND gslcblas REQUIRED )
if( NOT GSL_FOUND OR NOT GSLCBLAS_FOUND )
  message( FATAL_ERROR "gsl library not found" )
endif()

# ZLIB
find_package( ZLIB "1.2.3" REQUIRED )

add_subdirectory( 3rdparty )

add_subdirectory( alphaplot/src )

add_subdirectory( fitPlugins )

# enable_testing()
# add_subdirectory( test )

# Documentation
set( DOC_FILES
  data/manual/index.html
  data/alphaplot-logo.png
  data/CHANGES.txt
  data/gpl.txt
  data/license.rtf
  )

if( WIN32 )
  install( FILES ${DOC_FILES} DESTINATION . )
else()
  install( FILES ${DOC_FILES} DESTINATION share/doc/AlphaPlot )
endif()

# Icons
install( FILES data/icons/app/alphaplot.svg DESTINATION share/icons/hicolor/scalable/apps )

foreach( res IN ITEMS 16 22 32 48 64 96 128 )
  install( FILES data/icons/app/${res}/alphaplot.png
    DESTINATION share/icons/locolor/${res}x${res}/apps )
endforeach()

if( WIN32 )
  install( FILES data/icons/app/alphaplot.ico DESTINATION . )
endif()

if( UNIX )
  install( FILES data/alphaplot.desktop DESTINATION share/applications )
  install( FILES data/alphaplot.xml DESTINATION share/mime/packages )
endif()
