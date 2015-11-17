QMAKE_PROJECT_DEPTH = 0
linux-g++-64: libsuff=64

include(../../config.pri)
TARGET            = fitRational0
TEMPLATE          = lib

CONFIG           += thread
CONFIG           += warn_on 
CONFIG           += release
CONFIG           += dll 

DESTDIR           = ../
 
INSTALLS += target

# Dynamically link against GSL installed system-wide.
# This is used as default on unix systems such as
# Linux, Mac OS X and *BSD.
unix:LIBS += -L/usr/lib$${libsuff} -lgsl -lgslcblas
#unix:LIBS         += /usr/lib/libgsl.a /usr/lib/libgslcblas.a

# where to install the plugins
unix:target.path=/usr/lib$${libsuff}/scidavis/plugins
win32: target.path = ../../output/plugins

SOURCES += fitRational0.cpp
