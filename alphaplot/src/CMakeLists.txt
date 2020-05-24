
include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/.."
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/future"
  )

add_subdirectory( analysis )

add_subdirectory( core )

add_subdirectory( 2Dplot )

add_subdirectory( 3Dplot )

add_subdirectory( future )

add_subdirectory( plotcommon )

add_subdirectory( scripting )

add_subdirectory( ui )

set( current_dir  "." )

set( HEADERS ${HEADERS}
  "${current_dir}/About.h"
  "${current_dir}/AssociationsDialog.h"
  "${current_dir}/ApplicationWindow.h"
  "${current_dir}/ConfigDialog.h"
  "${current_dir}/ColorBox.h"
  "${current_dir}/ColorButton.h"
  "${current_dir}/CurveRangeDialog.h"
  "${current_dir}/DataSetDialog.h"
  "${current_dir}/FindDialog.h"
  "${current_dir}/Folder.h"
  "${current_dir}/globals.h"
  "${current_dir}/ImageExportDlg.h"
  "${current_dir}/ImportASCIIDialog.h"
  "${current_dir}/LayerDialog.h"
  "${current_dir}/Matrix.h"
  "${current_dir}/MyWidget.h"
  "${current_dir}/Note.h"
  "${current_dir}/OpenProjectDialog.h"
  "${current_dir}/PlotWizard.h"
  "${current_dir}/PatternBox.h"
  "${current_dir}/SymbolDialog.h"
  "${current_dir}/Spectrogram.h"
  "${current_dir}/Table.h"
  "${current_dir}/TableStatistics.h"
  "${current_dir}/TeXTableExportDialog.h"
  "${current_dir}/TeXTableSettings.h"
  "${current_dir}/TeXTableSettingsWidget.h"
  "${current_dir}/TextFormatButtons.h"
  "${current_dir}/TranslateCurveTool.h"
  "${current_dir}/widgets/aSettingsListView.h"
  )
set( SRCS ${SRCS}
  "${current_dir}/About.cpp"
  "${current_dir}/ApplicationWindow.cpp"
  "${current_dir}/AssociationsDialog.cpp"
  "${current_dir}/ColorBox.cpp"
  "${current_dir}/ColorButton.cpp"
  "${current_dir}/ConfigDialog.cpp"
  "${current_dir}/CurveRangeDialog.cpp"
  "${current_dir}/DataSetDialog.cpp"
  "${current_dir}/FindDialog.cpp"
  "${current_dir}/Folder.cpp"
  "${current_dir}/globals.cpp"
  "${current_dir}/ImageExportDlg.cpp"
  "${current_dir}/ImportASCIIDialog.cpp"
  "${current_dir}/LayerDialog.cpp"
  "${current_dir}/main.cpp"
  "${current_dir}/Matrix.cpp"
  "${current_dir}/MyWidget.cpp"
  "${current_dir}/Note.cpp"
  "${current_dir}/OpenProjectDialog.cpp"
  "${current_dir}/PatternBox.cpp"
  "${current_dir}/PlotWizard.cpp"
  "${current_dir}/Spectrogram.cpp"
  "${current_dir}/SymbolDialog.cpp"
  "${current_dir}/Table.cpp"
  "${current_dir}/TableStatistics.cpp"
  "${current_dir}/TextFormatButtons.cpp"
  "${current_dir}/TeXTableExportDialog.cpp"
  "${current_dir}/TeXTableSettings.cpp"
  "${current_dir}/TeXTableSettingsWidget.cpp"
  "${current_dir}/TranslateCurveTool.cpp"
  "${current_dir}/widgets/aSettingsListView.cpp"
  )
set( FORMS ${FORMS}
  "${current_dir}/About.ui"
  "${current_dir}/ApplicationWindow.ui"
  )
set( RESOURCES
  "${CMAKE_SOURCE_DIR}/data/styles/style.qrc"
  "${CMAKE_SOURCE_DIR}/data/icons.qrc"
  "${CMAKE_SOURCE_DIR}/data/xmlschema.qrc"
  )

if( WIN32 )
  set( RESOURCES ${RESOURCES} "${CMAKE_SOURCE_DIR}/data/alphaplot.rc"  )
endif()

# Translation Files
# set( TS_FILES
  # "${CMAKE_SOURCE_DIR}/data/translations/alphaplot_cs.ts"
  # "${CMAKE_SOURCE_DIR}/data/translations/alphaplot_cs-alt.ts"
  # "${CMAKE_SOURCE_DIR}/data/translations/alphaplot_de.ts"
  # "${CMAKE_SOURCE_DIR}/data/translations/alphaplot_es.ts"
  # "${CMAKE_SOURCE_DIR}/data/translations/alphaplot_fr.ts"
  # "${CMAKE_SOURCE_DIR}/data/translations/alphaplot_ja.ts"
  # "${CMAKE_SOURCE_DIR}/data/translations/alphaplot_pt-br.ts"
  # "${CMAKE_SOURCE_DIR}/data/translations/alphaplot_ru.ts"
  # "${CMAKE_SOURCE_DIR}/data/translations/alphaplot_sv.ts"
  # )
# qt5_create_translation( QM_FILES ${CMAKE_CURRENT_SOURCE_DIR} ${TS_FILES} )

# AlphaPlot
add_executable( AlphaPlot
  WIN32
  ${SRCS}
  ${FORMS}
  ${HEADERS}
  ${RESOURCES}
  # ${QM_FILES}
  )
target_link_libraries( AlphaPlot
  ZLIB::ZLIB
  GSL::gsl
  GSL::gslcblas
  minigzip
  qcustomplot
  propertybrowser
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  Qt5::OpenGL
  Qt5::DataVisualization
  Qt5::PrintSupport
  Qt5::ScriptTools
  Qt5::Svg
  Qt5::Xml
  Qt5::XmlPatterns
  Qt5::Script
  )

target_compile_definitions( AlphaPlot PRIVATE
  SCRIPTING_DIALOG
  SCRIPTING_CONSOLE
  LEGACY_CODE_0_2_x
  )
set_target_properties( AlphaPlot
  PROPERTIES
  AUTOUIC ON
  AUTOMOC ON
  AUTORCC ON
  )

if( SEARCH_FOR_UPDATES OR DOWNLOAD_LINKS )
  target_link_libraries( AlphaPlot Qt5::Network )
endif()
if( SCRIPTING_MUPARSER AND (NOT MUPARSER_LIB) )
  target_link_libraries( AlphaPlot muparser )
elseif( SCRIPTING_MUPARSER )
  target_link_libraries( AlphaPlot ${MUPARSER_LIB} )
endif()

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  target_compile_definitions( AlphaPlot PRIVATE Q_BUILD_64 )
else()
  target_compile_definitions( AlphaPlot PRIVATE Q_BUILD_32 )
endif()

if( SEARCH_FOR_UPDATES )
  target_compile_definitions( AlphaPlot PRIVATE SEARCH_FOR_UPDATES )
endif()
if( DOWNLOAD_LINKS )
  target_compile_definitions( AlphaPlot PRIVATE DOWNLOAD_LINKS )
endif()
if( SCRIPTING_MUPARSER )
  target_compile_definitions( AlphaPlot PRIVATE SCRIPTING_MUPARSER )
endif()

if( WIN32 )
  install( TARGETS AlphaPlot RUNTIME DESTINATION . )
  # install( FILES ${QM_FILES} DESTINATION translations )
else()
  install( TARGETS AlphaPlot RUNTIME DESTINATION bin )
  # install( FILES ${QM_FILES}  DESTINATION share/AlphaPlot/translations )
endif()
