TARGET            = fitRational0
TEMPLATE          = lib

CONFIG           += thread
CONFIG           += warn_on 
CONFIG           += release
CONFIG           += dll 

DESTDIR           = ../
unix:LIBS += -L /usr/lib -lgsl
 
win32:INCLUDEPATH += C:/Qt/WinGsl
win32:LIBS     += C:/Qt/WinGsl/Lib/WinGsl.lib 
win32:DEFINES  += GSL_DLL 

#target.path=/usr/lib
#INSTALLS += target

SOURCES += fitRational0.cpp
