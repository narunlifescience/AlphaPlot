TARGET       = origin2
TEMPLATE     = lib
CONFIG      += warn_on release thread

# Uncomment the following line if you want to build statically.
#CONFIG      += staticlib

MOC_DIR      = ./tmp
OBJECTS_DIR  = ./tmp

DESTDIR      = ./

# Uncomment the following line if you want to disable logging.
#DEFINES += NO_LOG_FILE

# Path to the folder where the header files of the BOOST C++ libraries are installed
INCLUDEPATH += ../boost

include(liborigin2.pri)
