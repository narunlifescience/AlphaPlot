#############################################################################
###################### BASIC PROJECT PROPERTIES #############################
#############################################################################

QMAKE_PROJECT_DEPTH = 0

TARGET         = scidavis
TEMPLATE       = app
# main() is mentioned here so that sourcefiles.pri can be included in
# other qmake project files
SOURCES       += src/main.cpp
CONFIG        += qt warn_on exceptions opengl thread zlib

DEFINES       += QT_PLUGIN
DEFINES       += TS_PATH="\\\"$$replace(translationfiles.path," ","\\ ")\\\""
DEFINES       += DOC_PATH="\\\"$$replace(documentation.path," ","\\ ")\\\""

!isEmpty( manual.path ) {
  DEFINES     += MANUAL_PATH="\\\"$$replace(manual.path," ","\\ ")\\\""
}
!isEmpty(plugins.path): DEFINES += PLUGIN_PATH=\\\"$$replace(plugins.path," ","\\ ")\\\"

!mxe {
  win32:DEFINES += QT_DLL QT_THREAD_SUPPORT
}

QT            += opengl qt3support network svg xml

MOC_DIR        = ../tmp/scidavis
OBJECTS_DIR    = ../tmp/scidavis
DESTDIR        = ./

###################### ICONS ################################################
RESOURCES      += ../data/appicons.qrc
RESOURCES      += ../data/icons.qrc
RC_FILE        =  ../data/scidavis.rc

win32 {
  win_icon.files = ../data/icons/scidavis.ico
  win_icon.path  = "$$INSTALLBASE"
}

###################### TRANSLATIONS #########################################

TRANSLATIONS     = ../data/translations/scidavis_de.ts \
                   ../data/translations/scidavis_es.ts \
                   ../data/translations/scidavis_fr.ts \
                   ../data/translations/scidavis_ru.ts \
                   ../data/translations/scidavis_ja.ts \
                   ../data/translations/scidavis_sv.ts \
                   ../data/translations/scidavis_pt-br.ts \
                   ../data/translations/scidavis_cs.ts \
                   ../data/translations/scidavis_cs-alt.ts \

exists(../data/translations/scidavis_de.ts){
  translationfiles.files   = ../data/translations/scidavis_de.qm \
                             ../data/translations/scidavis_es.qm \
                             ../data/translations/scidavis_fr.qm \
                             ../data/translations/scidavis_ru.qm \
                             ../data/translations/scidavis_ja.qm \
                             ../data/translations/scidavis_sv.qm \
                             ../data/translations/scidavis_pt-br.qm \
                             ../data/translations/scidavis_cs.qm \
                             ../data/translations/scidavis_cs-alt.qm \

  unix: tstarget.target  = ../data/translations/scidavis_de.qm
  win32: tstarget.target = ..\data\translations\scidavis_de.qm
# note the translation files are not writable during AEGIS
# integration, so we don't want to perform an update then
  tstarget.commands = (! test -w ../data/translations/scidavis_de.ts || \
  $$LUPDATE_BIN ../libscidavis/src/*.cpp -ts ../data/translations/*.ts) && $$LRELEASE_BIN ../data/translations/*.ts

  QMAKE_EXTRA_TARGETS     += tstarget
  QMAKE_CLEAN             += $$translationfiles.files
  PRE_TARGETDEPS          += ../data/translations/scidavis_de.qm
}
!exists(../data/translations/scidavis_de.ts){
  message("=======================================")   
  message("Could not find translation (.ts) files.")   
  message("=======================================")   
}

###################### DOCUMENTATION ########################################

documentation.files += ../manual \
                       ../INSTALL.html \
                       ../scidavis.css \
                       ../scidavis-logo.png \
                       ../README \
                       ../CHANGES \
                       ../gpl.txt 
