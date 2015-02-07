INCLUDEPATH += ../libscidavis ../libscidavis/src 
LIBS += -L ../libscidavis -lscidavis

POST_TARGETDEPS=../libscidavis/libscidavis.a

include(../config.pri)
include( basic.pri )
python {include( python.pri )}

### this is the program itself
INSTALLS        += target

### README, INSTALL.html, manual (if present in the manual subfolder), etc.
INSTALLS        += documentation

### translations
INSTALLS        += translationfiles

### icon file (for Windows installer)
win32:INSTALLS  += win_icon

###################### ICONS ################################################
RESOURCES    +=        appicons.qrc
RESOURCES    +=        icons.qrc
RC_FILE      =         scidavis.rc

win32 {
  win_icon.files = icons/scidavis.ico
  win_icon.path = "$$INSTALLBASE"
}

liborigin {
  LIBS += ../3rdparty/liborigin/liborigin.a
  POST_TARGETDEPS += ../3rdparty/liborigin/liborigin.a
}

########### Future code backported from the aspect framework ##################
DEFINES += LEGACY_CODE_0_2_x
INCLUDEPATH += ../libscidavis/src/future

