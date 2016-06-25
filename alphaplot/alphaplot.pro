# enable C++11 support
greaterThan(QT_MAJOR_VERSION, 4){
  CONFIG += c++11
} else {
  QMAKE_CXXFLAGS += -std=c++0x
}

INCLUDEPATH += alphaplot alphaplot/src ../data/translations

include(sourcefiles.pri)
include(muparser.pri)
include(../config.pri)

#############################################################################
###################### BASIC PROJECT PROPERTIES #############################
#############################################################################

# Aspect framework legacy code
DEFINES += LEGACY_CODE_0_2_x

TARGET         = alphaplot
TEMPLATE       = app

CONFIG        += qt warn_on exceptions opengl thread zlib

DEFINES       += QT_PLUGIN
DEFINES       += TS_PATH="\\\"$$replace(translationfiles.path," ","\\ ")\\\""
DEFINES       += DOC_PATH="\\\"$$replace(documentation.path," ","\\ ")\\\""

!isEmpty(manual.path) {
  DEFINES     += MANUAL_PATH="\\\"$$replace(manual.path," ","\\ ")\\\""
}

!isEmpty(plugins.path) {
  DEFINES += PLUGIN_PATH=\\\"$$replace(plugins.path," ","\\ ")\\\"
}

!mxe {
  win32:DEFINES += QT_DLL QT_THREAD_SUPPORT
}

QT             += opengl qt3support network svg xml script scripttools

MOC_DIR         = ../tmp/alphaplot
OBJECTS_DIR     = ../tmp/alphaplot
DESTDIR         = ./

INSTALLS        += target

###################### STYLES ###############################################
RESOURCES      += ../data/styles/style.qrc

###################### ICONS ################################################
RESOURCES      += ../data/icons.qrc
RC_FILE         = ../data/alphaplot.rc
macx:ICON       = ../data/icons/app/alphaplot.icns

win32 {
  win_icon.files = ../data/icons/app/alphaplot.ico
  win_icon.path  = "$$INSTALLBASE"
  INSTALLS  += win_icon
}

###################### TRANSLATIONS #########################################
TRANSLATIONS     = ../data/translations/alphaplot_de.ts \
                   ../data/translations/alphaplot_es.ts \
                   ../data/translations/alphaplot_fr.ts \
                   ../data/translations/alphaplot_ru.ts \
                   ../data/translations/alphaplot_ja.ts \
                   ../data/translations/alphaplot_sv.ts \
                   ../data/translations/alphaplot_pt-br.ts \
                   ../data/translations/alphaplot_cs.ts \
                   ../data/translations/alphaplot_cs-alt.ts \

exists(../data/translations/alphaplot_de.ts) {
  translationfiles.files   = ../data/translations/alphaplot_de.qm \
                             ../data/translations/alphaplot_es.qm \
                             ../data/translations/alphaplot_fr.qm \
                             ../data/translations/alphaplot_ru.qm \
                             ../data/translations/alphaplot_ja.qm \
                             ../data/translations/alphaplot_sv.qm \
                             ../data/translations/alphaplot_pt-br.qm \
                             ../data/translations/alphaplot_cs.qm \
                             ../data/translations/alphaplot_cs-alt.qm \

  unix: tstarget.target  = ../data/translations/alphaplot_de.qm
  win32: tstarget.target = ..\data\translations\alphaplot_de.qm

  tstarget.commands = (! test -w ../data/translations/alphaplot_de.ts || \
  $$LUPDATE_BIN src/*.cpp -ts ../data/translations/*.ts) && \
  $$LRELEASE_BIN ../data/translations/*.ts

  QMAKE_EXTRA_TARGETS     += tstarget
  QMAKE_CLEAN             += $$translationfiles.files

# comment out for now to get rid of AlphaPlot compile time error
#  PRE_TARGETDEPS          += ../data/translations/alphaplot_de.qm
}

!exists(../data/translations/alphaplot_de.ts){
  message("=======================================")
  message("Could not find translation (.ts) files.")
  message("=======================================")
}

INSTALLS        += translationfiles

###################### DOCUMENTATION ########################################
documentation.files += ../data/manual \
                       ../data/INSTALL.md \
                       ../data/alphaplot.css \
                       ../data/alphaplot-logo.png \
                       ../data/README.md \
                       ../data/CHANGES \
                       ../data/gpl.txt

INSTALLS        += documentation

unix {
  ###################### DESKTOP INTEGRATION ##################################

  #VARIABLES
  isEmpty(PREFIX) {
    PREFIX = /usr
  }

  desktop_entry.files = ../data/alphaplot.desktop
  desktop_entry.path  = "$$PREFIX/share/applications"

  mime_package.files  = ../data/alphaplot.xml
  mime_package.path   = "$$PREFIX/share/mime/packages"

  #deprecated
  mime_link.files     = ../data/x-aproj.desktop
  mime_link.path      = "$$PREFIX/share/mimelnk/application"

  contains(INSTALLS, icons) {
    # scalable icon
    icons.files       = ../data/icons/app/alphaplot.svg
    icons.path        = "$$PREFIX/share/icons/hicolor/scalable/apps"

    # hicolor icons for different resolutions
    resolutions       = 16 22 32 48 64 96 128
    for(res, resolutions) {
      eval(icon_hicolor_$${res}.files = ../data/icons/app/$${res}/alphaplot.png)
      eval(icon_hicolor_$${res}.path = "$$PREFIX/share/icons/hicolor/$${res}x$${res}/apps")
      INSTALLS                       += icon_hicolor_$${res}
    }

    # locolor icons for different resolutions
    resolutions        = 16 22 32 48 64 96 128
    for(res, resolutions) {
      eval(icon_locolor_$${res}.files = ../data/icons/app/$${res}/alphaplot.png)
      eval(icon_locolor_$${res}.path = "$$PREFIX/share/icons/locolor/$${res}x$${res}/apps")
      INSTALLS                       += icon_locolor_$${res}
    }
  }
}
