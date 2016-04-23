#############################################################################
###################### BASIC PROJECT PROPERTIES #############################
#############################################################################

QMAKE_PROJECT_DEPTH = 0

TARGET         = AlphaPlot
TEMPLATE       = app

CONFIG        += qt warn_on exceptions opengl thread zlib

DEFINES       += QT_PLUGIN
DEFINES       += TS_PATH="\\\"$$replace(translationfiles.path," ","\\ ")\\\""
DEFINES       += DOC_PATH="\\\"$$replace(documentation.path," ","\\ ")\\\""

!isEmpty( manual.path ) {
  DEFINES     += MANUAL_PATH="\\\"$$replace(manual.path," ","\\ ")\\\""
}

!isEmpty(plugins.path) {
  DEFINES += PLUGIN_PATH=\\\"$$replace(plugins.path," ","\\ ")\\\"
}

!mxe {
  win32:DEFINES += QT_DLL QT_THREAD_SUPPORT
}

QT             += opengl qt3support network svg xml script scripttools

MOC_DIR         = ../tmp/AlphaPlot
OBJECTS_DIR     = ../tmp/AlphaPlot
DESTDIR         = ./

###################### ICONS ################################################
RESOURCES      += ../data/icons.qrc
RC_FILE         =  ../data/AlphaPlot.rc

win32 {
  win_icon.files = ../data/icons/app/AlphaPlot.ico
  win_icon.path  = "$$INSTALLBASE"
}

###################### TRANSLATIONS #########################################

TRANSLATIONS     = ../data/translations/AlphaPlot_de.ts \
                   ../data/translations/AlphaPlot_es.ts \
                   ../data/translations/AlphaPlot_fr.ts \
                   ../data/translations/AlphaPlot_ru.ts \
                   ../data/translations/AlphaPlot_ja.ts \
                   ../data/translations/AlphaPlot_sv.ts \
                   ../data/translations/AlphaPlot_pt-br.ts \
                   ../data/translations/AlphaPlot_cs.ts \
                   ../data/translations/AlphaPlot_cs-alt.ts \

exists(../data/translations/AlphaPlot_de.ts) {
  translationfiles.files   = ../data/translations/AlphaPlot_de.qm \
                             ../data/translations/AlphaPlot_es.qm \
                             ../data/translations/AlphaPlot_fr.qm \
                             ../data/translations/AlphaPlot_ru.qm \
                             ../data/translations/AlphaPlot_ja.qm \
                             ../data/translations/AlphaPlot_sv.qm \
                             ../data/translations/AlphaPlot_pt-br.qm \
                             ../data/translations/AlphaPlot_cs.qm \
                             ../data/translations/AlphaPlot_cs-alt.qm \

  unix: tstarget.target  = ../data/translations/AlphaPlot_de.qm
  win32: tstarget.target = ..\data\translations\AlphaPlot_de.qm

  tstarget.commands = (! test -w ../data/translations/AlphaPlot_de.ts || \
  $$LUPDATE_BIN src/*.cpp -ts ../data/translations/*.ts) && \
  $$LRELEASE_BIN ../data/translations/*.ts

  QMAKE_EXTRA_TARGETS     += tstarget
  QMAKE_CLEAN             += $$translationfiles.files

# comment out for now to get rid of AlphaPlot compile time error
#  PRE_TARGETDEPS          += ../data/translations/AlphaPlot_de.qm
}

!exists(../data/translations/AlphaPlot_de.ts){
  message("=======================================")
  message("Could not find translation (.ts) files.")
  message("=======================================")
}

###################### DOCUMENTATION ########################################

documentation.files += ../data/manual \
                       ../data/INSTALL.md \
                       ../data/AlphaPlot.css \
                       ../data/AlphaPlot-logo.png \
                       ../data/README.md \
                       ../data/CHANGES \
                       ../data/gpl.txt 

unix {
  ###################### DESKTOP INTEGRATION ##################################

  desktop_entry.files = ../data/AlphaPlot.desktop
  desktop_entry.path  = "$$INSTALLBASE/share/applications"

  mime_package.files  = ../data/AlphaPlot.xml
  mime_package.path   = "$$INSTALLBASE/share/mime/packages"

  #deprecated
  mime_link.files     = ../data/x-aproj.desktop
  mime_link.path      = "$$INSTALLBASE/share/mimelnk/application"

  contains(INSTALLS, icons) {
    # scalable icon
    icons.files       = ../data/icons/app/AlphaPlot.svg
    icons.path        = "$$INSTALLBASE/share/icons/hicolor/scalable/apps"

    # hicolor icons for different resolutions
    resolutions       = 16 22 32 48 64 96 128
    for(res, resolutions) {
      eval(icon_hicolor_$${res}.files = ../data/icons/app/$${res}/AlphaPlot.png)
      eval(icon_hicolor_$${res}.path = "$$INSTALLBASE/share/icons/hicolor/$${res}x$${res}/apps")
      INSTALLS                       += icon_hicolor_$${res}
    }

    # locolor icons for different resolutions
    resolutions        = 16 22 32 48 64 96 128
    for(res, resolutions) {
      eval(icon_locolor_$${res}.files = ../data/icons/app/$${res}/AlphaPlot.png)
      eval(icon_locolor_$${res}.path = "$$INSTALLBASE/share/icons/locolor/$${res}x$${res}/apps")
      INSTALLS                       += icon_locolor_$${res}
    }
  }
}
