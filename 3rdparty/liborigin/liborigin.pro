# this file is not part of the liborigin library, which uses the cmake
# system, but must be maintained separately of liborigin

TEMPLATE = lib
CONFIG += staticlib
TARGET = origin
# following define required to prevent the catastrophic logging when
# large files are imported
DEFINE += NO_CODE_GENERATION_FOR_LOG

HEADERS  += \
        config.h \
	OriginObj.h\
	OriginFile.h\
	OriginParser.h\
	tree.hh

SOURCES += \
	OriginFile.cpp\
	OriginParser.cpp\
	OriginDefaultParser.cpp\
	Origin600Parser.cpp\
	Origin610Parser.cpp\
	Origin700Parser.cpp\
	Origin750Parser.cpp\
	Origin800Parser.cpp\
	Origin810Parser.cpp
