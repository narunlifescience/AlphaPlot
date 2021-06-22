QMAKE_PROJECT_DEPTH = 0
linux-g++-64: libsuff=64

include(../../config.pri)
TARGET            = fitRational1
TEMPLATE          = lib

CONFIG           += thread
CONFIG           += warn_on 
CONFIG           += release
CONFIG           += dll 

DESTDIR           = ../
 
INSTALLS += target
# where to install the plugins
unix:target.path=/usr/lib$${libsuff}/AlphaPlot/plugins
win32: target.path = ../../output/plugins

win32:INCLUDEPATH += ../../3rdparty/gsl/include
win32:LIBS        += ../../3rdparty/gsl/lib/libgsl.a
win32:LIBS        += ../../3rdparty/gsl/lib/libgslcblas.a

unix:LIBS += -L/usr/lib$${libsuff} -lgsl -lgslcblas
#unix:LIBS += /usr/lib/libgsl.a /usr/lib/libgslcblas.a

SOURCES += fitRational1.cpp
