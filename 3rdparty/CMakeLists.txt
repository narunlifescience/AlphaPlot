# MuParser
find_library( MUPARSER_FOUND muparser )
if( NOT MUPARSER_FOUND )
  set( muparser_dir ${CMAKE_CURRENT_SOURCE_DIR}/muparser )
  set( HEADERS
    ${muparser_dir}/muParser.h
    ${muparser_dir}/muParserBase.h
    ${muparser_dir}/muParserBytecode.h
    ${muparser_dir}/muParserCallback.h
    ${muparser_dir}/muParserDef.h
    ${muparser_dir}/muParserError.h
    ${muparser_dir}/muParserFixes.h
    ${muparser_dir}/muParserStack.h
    ${muparser_dir}/muParserToken.h
    ${muparser_dir}/muParserTokenReader.h
    ${muparser_dir}/muParserTemplateMagic.h
    )
  set_source_files_properties( ${HEADERS} PROPERTIES HEADER_FILE_ONLY ON )
  set( SRCS
    ${muparser_dir}/muParser.cpp
    ${muparser_dir}/muParserBase.cpp
    ${muparser_dir}/muParserBytecode.cpp
    ${muparser_dir}/muParserCallback.cpp
    ${muparser_dir}/muParserError.cpp
    ${muparser_dir}/muParserTokenReader.cpp
    )
  add_library( muparser STATIC ${SRCS} )
endif()

# minigzip
add_library( minigzip STATIC "${CMAKE_CURRENT_SOURCE_DIR}/minigzip/minigzip.c" )
target_link_libraries( minigzip ZLIB::ZLIB )

# QCustomPlot
set( HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot/qcustomplot.h" )
set_source_files_properties( ${HEADERS} PROPERTIES HEADER_FILE_ONLY ON )
set( SRCS "${CMAKE_CURRENT_SOURCE_DIR}/qcustomplot/qcustomplot.cpp" )
add_library( qcustomplot STATIC ${SRCS} )
target_link_libraries( qcustomplot
  Qt5::Widgets
  Qt5::PrintSupport
  )
set_target_properties( qcustomplot PROPERTIES AUTOMOC ON )
# OpenGL
find_package( OpenGL COMPONENTS OpenGL )
if( OPENGL_FOUND )
  target_compile_definitions( qcustomplot PUBLIC QCUSTOMPLOT_USE_OPENGL )
  target_link_libraries( qcustomplot OpenGL::GL )
endif()

# PropertyBrowser
set( propertybrowser_dir ${CMAKE_CURRENT_SOURCE_DIR}/propertybrowser )
set( HEADERS
  ${propertybrowser_dir}/qteditorfactory.h
  ${propertybrowser_dir}/qtpropertybrowser.h
  ${propertybrowser_dir}/qtpropertybrowserutils_p.h
  ${propertybrowser_dir}/qtpropertymanager.h
  ${propertybrowser_dir}/qttreepropertybrowser.h
  )
set_source_files_properties( ${HEADERS} PROPERTIES HEADER_FILE_ONLY ON )
set( SRCS
  ${propertybrowser_dir}/qteditorfactory.cpp
  ${propertybrowser_dir}/qtpropertybrowser.cpp
  ${propertybrowser_dir}/qtpropertybrowserutils.cpp
  ${propertybrowser_dir}/qtpropertymanager.cpp
  ${propertybrowser_dir}/qttreepropertybrowser.cpp
  )
set( RESOURCES "${propertybrowser_dir}/qtpropertybrowser.qrc" )
add_library( propertybrowser STATIC
  ${SRCS}
  ${RESOURCES}
  )
target_link_libraries( propertybrowser Qt5::Widgets )
set_target_properties( propertybrowser
  PROPERTIES
  AUTOMOC ON
  AUTORCC ON
  )
