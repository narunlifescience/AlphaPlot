################################################################################
### Choose between common configurations                                       #
################################################################################

unix:isEmpty(PRESET) {                     # command-line argument to override
  ### Link dynamically against system-wide installed libraries(default).
  if(!macx) {
    PRESET = linux_all_dynamic
    ### Link statically against Qwt and Qwtplot3D in 3rdparty folder.
    #PRESET = linux_qwt_qwtplot3d_static
    ### Link statically as much as possible.except Qt.
    #PRESET = linux_all_static
  } else {
    PRESET = osx_dist
  }
}

!contains(PRESET, linux_all_static) {                 # Linux Desktop integration
  unix:INSTALLS   += desktop_entry mime_package mime_link icons
}

unix {                                                       # Linux / MacOS X
  contains(PRESET, linux_all_static) {
    ### When installing into a self-contained directory structure use relative
    ### paths at runtime in order to support relocation.
    isEmpty(INSTALLBASE): INSTALLBASE = .
    target.path           = "$$INSTALLBASE"
    documentation.path    = "$$INSTALLBASE/doc"
    manual.path           = "$$INSTALLBASE/manual"
    translationfiles.path = "$$INSTALLBASE/translations"
    plugins.path          = "$$INSTALLBASE/plugins"
  } else {
    ### this is what is called "prefix" when using GNU autotools
    isEmpty(INSTALLBASE): INSTALLBASE = /usr
    target.path           = "$$INSTALLBASE/bin"
    documentation.path    = "$$INSTALLBASE/share/doc/alphaplot"
    manual.path = "$$INSTALLBASE/share/doc/alphaplot/manual"
    translationfiles.path = "$$INSTALLBASE/share/alphaplot/translations"
    plugins.path      = "$$INSTALLBASE/lib/alphaplot/plugins"
  }
}                                                            # Linux / MacOS X

win32 {                                                      # Windows
  isEmpty(INSTALLBASE): INSTALLBASE = .
  target.path            = "$$INSTALLBASE"
  documentation.path     = "$$INSTALLBASE"
  translationfiles.path  = "$$INSTALLBASE/translations"
  plugins.path           = "$$INSTALLBASE/fitPlugins"
}

### 64bit Linux only suffix
linux-g++-64: libsuff  = 64
unix:LIBS             += -L/usr/lib$${libsuff}

################################################################################
### Optional features                                                          #
################################################################################

### a console displaying output of scripts.
DEFINES         += SCRIPTING_CONSOLE
DEFINES         += SCRIPTING_MUPARSER
### a dialog for selecting the scripting language.
DEFINES         += SCRIPTING_DIALOG
DEFINES         += SEARCH_FOR_UPDATES

!contains(PRESET, linux_all_dynamic) {
  ### Enables choosing of help folder at runtime.
  DEFINES       += DYNAMIC_MANUAL_PATH
  ### Similar to DYNAMIC_MANUAL_PATH, but for the plugin folder
  DEFINES       += DYNAMIC_PLUGIN_PATH
  ### Provide links to manual/translations download in help menu
  DEFINES       += DOWNLOAD_LINKS
}

################################################################################
### Dependencies                                                               #
################################################################################

contains(PRESET, linux_all_dynamic) {
  ### dynamically link against Qwt(3D) installed system-wide
  message(Build configuration: Linux all dynamic)

  ### Debian suffix
  exists(/usr/include/qwt-qt4): qwtsuff = "-qt4"

  exists(/usr/include/qwt5) {
    INCLUDEPATH  += /usr/include/qwt5
    LIBS         += -lqwt5
  } else {
    INCLUDEPATH  += /usr/include/qwt$${qwtsuff}
    LIBS         += -lqwt$${qwtsuff}
  }

  INCLUDEPATH   = "$(HOME)/usr/include" $$INCLUDEPATH
  QMAKE_LIBDIR  = "$(HOME)/usr/lib" $$QMAKE_LIBDIR

  INCLUDEPATH  += /usr/include/qwtplot3d
  LIBS         += -lqwtplot3d$${qwtsuff}

  LIBS         += -lz -lGLU
  LIBS         += -lgsl -lgslcblas
}

contains(PRESET, linux_qwt_qwtplot3d_static) {
  ### Link statically against Qwt and Qwtplot3D dynamically against rest.
  message(Build configuration: Linux all dynamic except QWT & QwtPlot3D)
  INCLUDEPATH  += ../3rdparty/qwt/src
  LIBS         += ../3rdparty/qwt/lib/libqwt.a
  INCLUDEPATH  += ../3rdparty/qwtplot3d/include
  LIBS         += ../3rdparty/qwtplot3d/lib/libqwtplot3d.a
  LIBS         += -lgsl -lgslcblas -lz -lGLU
}

contains(PRESET, linux_all_static) {
  ### mostly static linking, for self-contained binaries
  message(Build configuration: Linux all static)

  INCLUDEPATH  += ../3rdparty/qwt/src
  LIBS         += ../3rdparty/qwt/lib/libqwt.a

  INCLUDEPATH  += ../3rdparty/qwtplot3d/include
  LIBS         += ../3rdparty/qwtplot3d/lib/libqwtplot3d.a

  LIBS         += /usr/lib/libgsl.a /usr/lib/libgslcblas.a
}

contains(PRESET, osx_dist) {
  # Uses HomeBrew supplied versions of the dependencies
  message(Build configuration: OSX Distro)

  INCLUDEPATH  += /usr/local/include

  INCLUDEPATH  += ../3rdparty/qwt/src
  LIBS         += ../3rdparty/qwt/lib/libqwt.a

  INCLUDEPATH  += ../3rdparty/qwtplot3d/include
  LIBS         += ../3rdparty/qwtplot3d/lib/libqwtplot3d.a

  LIBS         += -lgsl -lgslcblas -lz
}

win32: {
  !mxe {
    ### Static linking mostly, except Qt and QwtPlot3D.
    message(Build configuration: Win32)

    isEmpty(LIBPATH): LIBPATH = ../3rdparty

    INCLUDEPATH  += "$${LIBPATH}/qwt/src"
    LIBS         += "$${LIBPATH}/qwt/lib/libqwt.a"

    INCLUDEPATH  += "$${LIBPATH}/qwtplot3d/include"
    LIBS         += "$${LIBPATH}/qwtplot3d/lib/qwtplot3d.dll"

    INCLUDEPATH  += "$${LIBPATH}/gsl/include"
    LIBS         += "$${LIBPATH}/gsl/lib/libgsl.a"
    LIBS         += "$${LIBPATH}/gsl/lib/libgslcblas.a"
  }
}

mxe {
  ### Mingw cross compilation environment on Linux.
  message(Build configuration: Mxe all static)

  QMAKE_CXXFLAGS +=-g
  DEFINES        += CONSOLE
    
  INCLUDEPATH  += "../3rdparty/include"
  LIBS         += "../3rdparty/lib/libqwt.a"
  LIBS         += "../3rdparty/lib/libqwtplot3d.a"

  LIBS           +=  -mwindows -lgsl -lgslcblas

  # Qt libs specified here to get around a dependency bug in qmake
  LIBS += -lQtOpenGL -lQtGui -lQtNetwork -lQtCore
  LIBS += -lole32 -loleaut32 -limm32 -lcomdlg32 -luuid 
  LIBS += -lwinspool -lssl -lcrypto -lwinmm -lgdi32 -lws2_32
  LIBS += -ljpeg -lpng -lmng -ltiff -lz -llzma -llcms2
  LIBS += -lopengl32 -lglu32 
}

#############################################################################
### Names of the lupdate and lrelease programs                              #
#############################################################################

exists(/usr/bin/lupdate-qt4) {
  # Debian, Ubuntu, Fedora
  LUPDATE_BIN  = lupdate-qt4
  LRELEASE_BIN = lrelease-qt4
} else {
  # anything else
  LUPDATE_BIN  = lupdate
  LRELEASE_BIN = lrelease
}
