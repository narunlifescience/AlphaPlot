QMAKE_PROJECT_DEPTH = 0
linux-g++-64: libsuff=64

TARGET            = fitRational0
TEMPLATE          = lib

CONFIG           += thread
CONFIG           += warn_on 
CONFIG           += release
CONFIG           += dll 

DESTDIR           = ../
 
INSTALLS += target

# Statically link against GSL in 3rdparty.
# This is used as default on Windows.
# To use this on Linux or Mac OS X, remove
# the "win32" and prepend '#' to the "LIBS"
# line in the dynamic linking section.
win32:INCLUDEPATH       += c:/gsl/include
win32:LIBS        += c:/gsl/lib/libgsl.a
win32:LIBS        += c:/gsl/lib/libgslcblas.a

# Dynamically link against GSL installed system-wide.
# This is used as default on unix systems such as
# Linux, Mac OS X and *BSD.
unix:LIBS += -L /usr/lib$${libsuff} -lgsl -lgslcblas

# where to install the plugins
unix:target.path=/usr/lib$${libsuff}/scidavis/plugins
win32: target.path = c:/scidavis/plugins

SOURCES += fitRational0.cpp
