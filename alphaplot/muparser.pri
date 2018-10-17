DEFINES += SCRIPTING_MUPARSER

HEADERS += ../3rdparty/muparser/muParser.h \
	       ../3rdparty/muparser/muParserBase.h \
	       ../3rdparty/muparser/muParserBytecode.h \
	       ../3rdparty/muparser/muParserCallback.h \
	       ../3rdparty/muparser/muParserDef.h \
	       ../3rdparty/muparser/muParserError.h \
	       ../3rdparty/muparser/muParserFixes.h \
	       ../3rdparty/muparser/muParserStack.h \
	       ../3rdparty/muparser/muParserToken.h \
	       ../3rdparty/muparser/muParserTokenReader.h \
	       ../3rdparty/muparser/muParserTemplateMagic.h \
           src/scripting/MuParserScript.h \
           src/scripting/MuParserScripting.h \

SOURCES += ../3rdparty/muparser/muParser.cpp \
	       ../3rdparty/muparser/muParserBase.cpp \
	       ../3rdparty/muparser/muParserBytecode.cpp \
	       ../3rdparty/muparser/muParserCallback.cpp \
	       ../3rdparty/muparser/muParserError.cpp \
           ../3rdparty/muparser/muParserTokenReader.cpp \
           src/scripting/MuParserScript.cpp \
           src/scripting/MuParserScripting.cpp \

SOURCES += ../3rdparty/qwtplot3d/qwt3d_axis.cpp \
           ../3rdparty/qwtplot3d/qwt3d_color.cpp \
           ../3rdparty/qwtplot3d/qwt3d_coordsys.cpp \
           ../3rdparty/qwtplot3d/qwt3d_drawable.cpp \
           ../3rdparty/qwtplot3d/qwt3d_mousekeyboard.cpp \
           ../3rdparty/qwtplot3d/qwt3d_movements.cpp \
           ../3rdparty/qwtplot3d/qwt3d_lighting.cpp \
           ../3rdparty/qwtplot3d/qwt3d_colorlegend.cpp \
           ../3rdparty/qwtplot3d/qwt3d_plot.cpp \
           ../3rdparty/qwtplot3d/qwt3d_label.cpp \
           ../3rdparty/qwtplot3d/qwt3d_types.cpp \
           ../3rdparty/qwtplot3d/qwt3d_enrichment_std.cpp \
           ../3rdparty/qwtplot3d/qwt3d_autoscaler.cpp \
           ../3rdparty/qwtplot3d/qwt3d_io_reader.cpp \
           ../3rdparty/qwtplot3d/qwt3d_io.cpp \
           ../3rdparty/qwtplot3d/qwt3d_scale.cpp

SOURCES += ../3rdparty/qwtplot3d/qwt3d_gridmapping.cpp \
		   ../3rdparty/qwtplot3d/qwt3d_parametricsurface.cpp \
           ../3rdparty/qwtplot3d/qwt3d_function.cpp

SOURCES += ../3rdparty/qwtplot3d/qwt3d_surfaceplot.cpp \
           ../3rdparty/qwtplot3d/qwt3d_gridplot.cpp \
           ../3rdparty/qwtplot3d/qwt3d_meshplot.cpp
          

HEADERS += ../3rdparty/qwtplot3d/qwt3d_color.h \
           ../3rdparty/qwtplot3d/qwt3d_global.h \
           ../3rdparty/qwtplot3d/qwt3d_types.h \
	       ../3rdparty/qwtplot3d/qwt3d_axis.h \
           ../3rdparty/qwtplot3d/qwt3d_coordsys.h \
           ../3rdparty/qwtplot3d/qwt3d_drawable.h \
           ../3rdparty/qwtplot3d/qwt3d_helper.h \
           ../3rdparty/qwtplot3d/qwt3d_label.h \
           ../3rdparty/qwtplot3d/qwt3d_openglhelper.h \
           ../3rdparty/qwtplot3d/qwt3d_colorlegend.h \
           ../3rdparty/qwtplot3d/qwt3d_plot.h \
           ../3rdparty/qwtplot3d/qwt3d_enrichment.h \
           ../3rdparty/qwtplot3d/qwt3d_enrichment_std.h \
           ../3rdparty/qwtplot3d/qwt3d_autoscaler.h \
           ../3rdparty/qwtplot3d/qwt3d_autoptr.h \
           ../3rdparty/qwtplot3d/qwt3d_io.h \
           ../3rdparty/qwtplot3d/qwt3d_io_reader.h \
           ../3rdparty/qwtplot3d/qwt3d_scale.h \
           ../3rdparty/qwtplot3d/qwt3d_portability.h
						
HEADERS += ../3rdparty/qwtplot3d/qwt3d_mapping.h \
		   ../3rdparty/qwtplot3d/qwt3d_gridmapping.h \
           ../3rdparty/qwtplot3d/qwt3d_parametricsurface.h \
           ../3rdparty/qwtplot3d/qwt3d_function.h

HEADERS += ../3rdparty/qwtplot3d/qwt3d_surfaceplot.h \
           ../3rdparty/qwtplot3d/qwt3d_volumeplot.h \
           ../3rdparty/qwtplot3d/qwt3d_graphplot.h \
           ../3rdparty/qwtplot3d/qwt3d_multiplot.h

# gl2ps support
HEADERS+=../3rdparty/qwtplot3d/3rdparty/gl2ps/gl2ps.h \
         ../3rdparty/qwtplot3d/qwt3d_io_gl2ps.h
         
SOURCES+=../3rdparty/qwtplot3d/qwt3d_io_gl2ps.cpp \
         ../3rdparty/qwtplot3d/3rdparty/gl2ps/gl2ps.c

