QMAKE_PROJECT_DEPTH = 0
linux-g++-64: libsuff=64

TARGET            = fitRational0
TEMPLATE          = lib

CONFIG           += thread
CONFIG           += warn_on 
CONFIG           += release
CONFIG           += dll 

DESTDIR           = ../
 
INCLUDEPATH += ../../3rdparty/gsl
LIBS        += ../../3rdparty/gsl/lib/libgsl.a
LIBS        += ../../3rdparty/gsl/lib/libgslcblas.a

#target.path=/usr/lib$${libsuff}
#INSTALLS += target

SOURCES += fitRational0.cpp
