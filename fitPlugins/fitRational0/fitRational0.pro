QMAKE_PROJECT_DEPTH = 0
linux-g++-64: libsuff=64

TARGET            = fitRational0
TEMPLATE          = lib

CONFIG           += thread
CONFIG           += warn_on 
CONFIG           += release
CONFIG           += dll 

DESTDIR           = ../
unix:LIBS += -L /usr/lib$${libsuff} -lgsl
 
win32:INCLUDEPATH += C:/Qt/WinGsl
win32:LIBS     += C:/Qt/WinGsl/Lib/WinGsl.lib 
win32:DEFINES  += GSL_DLL 

#target.path=/usr/lib$${libsuff}
#INSTALLS += target

SOURCES += fitRational0.cpp
