DEFINES += SCRIPTING_MUPARSER

HEADERS += ../3rdparty/muparser/muParser.h \
	       ../3rdparty/muparser/muParserBase.h \
	       ../3rdparty/muparser/muParserBytecode.h \
	       ../3rdparty/muparser/muParserCallback.h \
	       ../3rdparty/muparser/muParserDLL.h \
	       ../3rdparty/muparser/muParserDef.h \
	       ../3rdparty/muparser/muParserError.h \
	       ../3rdparty/muparser/muParserFixes.h \
	       ../3rdparty/muparser/muParserInt.h \
	       ../3rdparty/muparser/muParserStack.h \
	       ../3rdparty/muparser/muParserTemplateMagic.h \
	       ../3rdparty/muparser/muParserTest.h \
	       ../3rdparty/muparser/muParserToken.h \
	       ../3rdparty/muparser/muParserTokenReader.h \
           src/scripting/MuParserScript.h \
           src/scripting/MuParserScripting.h \

SOURCES += ../3rdparty/muparser/muParser.cpp \
	       ../3rdparty/muparser/muParserBase.cpp \
	       ../3rdparty/muparser/muParserBytecode.cpp \
	       ../3rdparty/muparser/muParserCallback.cpp \
	       ../3rdparty/muparser/muParserDLL.cpp \
	       ../3rdparty/muparser/muParserError.cpp \
	       ../3rdparty/muparser/muParserInt.cpp \
           ../3rdparty/muparser/muParserTest.cpp \
           ../3rdparty/muparser/muParserTokenReader.cpp \
           src/scripting/MuParserScript.cpp \
           src/scripting/MuParserScripting.cpp \

