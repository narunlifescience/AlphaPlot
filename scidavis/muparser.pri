INCLUDEPATH       += ../3rdparty/muParser
DEFINES += SCRIPTING_MUPARSER

HEADERS += src/muParserScript.h \
			 src/muParserScripting.h \
			 ../3rdparty/muParser/muParser.h \
			 ../3rdparty/muParser/muParserBase.h \
			 ../3rdparty/muParser/muParserInt.h \
			 ../3rdparty/muParser/muParserError.h \
			 ../3rdparty/muParser/muParserStack.h \
			 ../3rdparty/muParser/muParserToken.h \
			 ../3rdparty/muParser/muParserBytecode.h \
			 ../3rdparty/muParser/muParserCallback.h \
			 ../3rdparty/muParser/muParserTokenReader.h \
			 ../3rdparty/muParser/muParserFixes.h \
			 ../3rdparty/muParser/muParserDef.h \

SOURCES += src/muParserScript.cpp \
			 src/muParserScripting.cpp \
			 ../3rdparty/muParser/muParser.cpp \
			 ../3rdparty/muParser/muParserBase.cpp \
			 ../3rdparty/muParser/muParserInt.cpp \
			 ../3rdparty/muParser/muParserBytecode.cpp \
			 ../3rdparty/muParser/muParserCallback.cpp \
			 ../3rdparty/muParser/muParserTokenReader.cpp \
			 ../3rdparty/muParser/muParserError.cpp \

