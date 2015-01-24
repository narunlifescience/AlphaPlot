INCLUDEPATH += ../libscidavis ../libscidavis/src 
LIBS += -L ../libscidavis -lscidavis

POST_TARGETDEPS=../libscidavis/libscidavis.a

include(../config.pri)
include( basic.pri )
python {include( python.pri )}

liborigin {
  LIBS += ../3rdparty/liborigin/liborigin.a
  POST_TARGETDEPS += ../3rdparty/liborigin/liborigin.a
}

########### Future code backported from the aspect framework ##################
DEFINES += LEGACY_CODE_0_2_x
INCLUDEPATH += ../libscidavis/src/future

