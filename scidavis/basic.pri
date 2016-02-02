#############################################################################
###################### BASIC PROJECT PROPERTIES #############################
#############################################################################

QMAKE_PROJECT_DEPTH = 0

TARGET         = scidavis
TEMPLATE       = app
# main() is mentioned here so that sourcefiles.pri can be included in
# other qmake project files
SOURCES += src/main.cpp
CONFIG        += qt warn_on exceptions opengl thread zlib

DEFINES       += QT_PLUGIN
DEFINES       += TS_PATH="\\\"$$replace(translationfiles.path," ","\\ ")\\\""
DEFINES       += DOC_PATH="\\\"$$replace(documentation.path," ","\\ ")\\\""
!isEmpty( manual.path ) {
DEFINES       += MANUAL_PATH="\\\"$$replace(manual.path," ","\\ ")\\\""
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
RESOURCES    +=        appicons.qrc
RESOURCES    +=        icons.qrc
RC_FILE      =         scidavis.rc

win32 {
  win_icon.files = icons/scidavis.ico
  win_icon.path = "$$INSTALLBASE"
}

###################### TRANSLATIONS #########################################

TRANSLATIONS    = translations/scidavis_de.ts \
                  translations/scidavis_es.ts \
                  translations/scidavis_fr.ts \
                  translations/scidavis_ru.ts \
                  translations/scidavis_ja.ts \
                  translations/scidavis_sv.ts \
                  translations/scidavis_pt-br.ts \
                  translations/scidavis_cs.ts \
                  translations/scidavis_cs-alt.ts \

    exists(translations/scidavis_de.ts){
        translationfiles.files =     translations/scidavis_de.qm \
                             translations/scidavis_es.qm \
                             translations/scidavis_fr.qm \
                             translations/scidavis_ru.qm \
                             translations/scidavis_ja.qm \
                             translations/scidavis_sv.qm \
                             translations/scidavis_pt-br.qm \
                             translations/scidavis_cs.qm \
                             translations/scidavis_cs-alt.qm \

        unix: tstarget.target = translations/scidavis_de.qm
        win32: tstarget.target = translations\scidavis_de.qm
# note the translation files are not writable during AEGIS
# integration, so we don't want to perform an update then
        tstarget.commands = (! test -w translations/scidavis_de.ts || \
$$LUPDATE_BIN ../libscidavis/src/*.cpp -ts translations/*.ts) && $$LRELEASE_BIN translations/*.ts

        QMAKE_EXTRA_TARGETS += tstarget
        QMAKE_CLEAN += $$translationfiles.files
        PRE_TARGETDEPS += translations/scidavis_de.qm
    }
    !exists(translations/scidavis_de.ts){
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
