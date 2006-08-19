TARGET  = qtiplot
TEMPLATE     = app
CONFIG      += qt warn_on release thread opengl
MOC_DIR      = ../tmp/qtiplot
OBJECTS_DIR  = ../tmp/qtiplot
DESTDIR           = ./
DEFINES += QT_PLUGIN
win32:DEFINES  += QT_DLL QT_THREAD_SUPPORT GSL_DLL QWTPLOT3D_DLL 
QT +=  opengl qt3support network

TRANSLATIONS = translations/qtiplot_de.ts \
			   translations/qtiplot_es.ts \
			   translations/qtiplot_fr.ts 

INCLUDEPATH       += ../3rdparty/qwt/include
unix:INCLUDEPATH += -I /usr/include/qwtplot3d

win32:INCLUDEPATH += ../3rdparty/qwtplot3d/include
win32:INCLUDEPATH += C:/WinGsl
win32:INCLUDEPATH += ../3rdparty/zlib123/include

unix:LIBS         += ../3rdparty/qwt/lib/libqwt.a
unix:LIBS += -L /usr/lib -lgsl -lgslcblas -lqwtplot3d -lz

win32:LIBS        += ../3rdparty/qwtplot3d/lib/qwtplot3d.lib
win32:LIBS        += ../3rdparty/qwt/lib/qwt.lib  
win32:LIBS        += C:/WinGsl/Lib/WinGsl.lib
win32:LIBS		  += ../3rdparty/zlib123/lib/zdll.lib

unix:target.path=/usr/bin
unix:INSTALLS += target

unix:documentation.path = /usr/share/doc/qtiplot
unix:documentation.files = doc/*
unix:INSTALLS += documentation
 
win32:RC_FILE     = src/iPlot.rc
HEADERS  = src/application.h \
     src/graph.h \
     src/graph3D.h \
     src/worksheet.h \
     src/curvesDialog.h \
     src/valuesDialog.h \
     src/plotDialog.h \
     src/plot3DDialog.h \
     src/plotWizard.h \
     src/exportDialog.h \
     src/importDialog.h \
     src/axesDialog.h \
     src/pieDialog.h \
     src/polynomFitDialog.h \
     src/expDecayDialog.h \
     src/functionDialog.h \
     src/functionDialogui.h \
     src/fitDialog.h \
     src/surfaceDialog.h \
     src/tableDialog.h \
     src/textDialog.h \
     src/lineDlg.h \
     src/scalePicker.h \
     src/canvaspicker.h \
     src/ErrorBar.h \
     src/pie.h \
     src/errDlg.h \
     src/LegendMarker.h \
     src/LineMarker.h\
     src/ImageMarker.h\
     src/imageDialog.h \
     src/fit.h \
     src/nrutil.h\
     src/pixmaps.h\
     src/multilayer.h\
     src/layerDialog.h \
     src/intDialog.h \
     src/sortDialog.h\
	 src/bars.h \
	 src/cones.h \
	 src/configDialog.h \
	 src/BarCurve.h \
	 src/BoxCurve.h \
	 src/Histogram.h \
	 src/VectorCurve.h \
     src/scales.h \
	 src/imageExportOptionsDialog.h \
	 src/matrix.h \
	 src/matrixDialog.h \
	 src/matrixSizeDialog.h \
	 src/matrixValuesDialog.h \
     src/dataSetDialog.h \
	 src/parser.h \
	 src/colorBox.h \
	 src/symbolBox.h \
	 src/patternBox.h \
	 src/importOPJ.h\
	 src/symbolDialog.h \
	 src/plot.h \
	 src/colorButton.h \
	 src/associationsDialog.h \
	 src/renameWindowDialog.h \
	 src/widget.h \
	 src/interpolationDialog.h\
	 src/fileDialogs.h\
	 src/epsExportDialog.h\
	 src/smoothCurveDialog.h\
	 src/filterDialog.h\
	 src/fftDialog.h\
	 src/note.h\
	 src/folder.h\
	 src/findDialog.h\
	 src/textformatbuttons.h
     
SOURCES  = src/application.cpp \
     src/graph.cpp \
     src/analysis.cpp \
     src/graph3D.cpp \
     src/worksheet.cpp \
     src/valuesDialog.cpp \
     src/curvesDialog.cpp \
     src/plotDialog.cpp \
     src/plot3DDialog.cpp \
     src/plotWizard.cpp \
     src/exportDialog.cpp \
     src/importDialog.cpp \
     src/axesDialog.cpp \
     src/pieDialog.cpp \
     src/polynomFitDialog.cpp \
     src/tableDialog.cpp \
     src/textDialog.cpp \
     src/scalePicker.cpp \
     src/canvaspicker.cpp \
     src/expDecayDialog.cpp \
     src/functionDialog.cpp \
     src/functionDialogui.cpp \
     src/fitDialog.cpp \
     src/surfaceDialog.cpp \
     src/lineDlg.cpp \
     src/ErrorBar.cpp \
     src/pie.cpp \
     src/errDlg.cpp \
     src/LegendMarker.cpp \
     src/LineMarker.cpp \
     src/ImageMarker.cpp\
     src/imageDialog.cpp \
     src/multilayer.cpp\
     src/layerDialog.cpp \
     src/intDialog.cpp \
     src/sortDialog.cpp\
	 src/bars.cpp \
	 src/cones.cpp \
     src/dataSetDialog.cpp \
	 src/configDialog.cpp \
	 src/BarCurve.cpp \
	 src/BoxCurve.cpp \
	 src/Histogram.cpp \
	 src/VectorCurve.cpp \
	 src/imageExportOptionsDialog.cpp \
	 src/matrix.cpp \
	 src/matrixDialog.cpp \
	 src/matrixSizeDialog.cpp \
	 src/matrixValuesDialog.cpp \
	 src/parser.cpp\
	 src/colorBox.cpp \
	 src/symbolBox.cpp \
	 src/patternBox.cpp \
	 src/importOPJ.cpp\
	 src/main.cpp \
	 src/symbolDialog.cpp \
	 src/plot.cpp \
	 src/colorButton.cpp \
	 src/associationsDialog.cpp \
	 src/renameWindowDialog.cpp \
	 src/widget.cpp\
	 src/interpolationDialog.cpp\
	 src/epsExportDialog.cpp\
	 src/nrutil.cpp\
     src/fit.cpp\
	 src/smoothCurveDialog.cpp\
	 src/filterDialog.cpp\
	 src/fftDialog.cpp\
	 src/note.cpp\
	 src/folder.cpp\
	 src/findDialog.cpp\
	 src/textformatbuttons.cpp\
	 src/scales.cpp

#parser (muParser_v1.26)
HEADERS+=../3rdparty/muParser/muParser.h \
         ../3rdparty/muParser/muParserBase.h \ 
	     ../3rdparty/muParser/muParserInt.h \
	     ../3rdparty/muParser/muParserError.h \ 
         ../3rdparty/muParser/muParserStack.h \ 
         ../3rdparty/muParser/muParserToken.h \
		 ../3rdparty/muParser/muParserBytecode.h \
		 ../3rdparty/muParser/muParserCallback.h \
		 ../3rdparty/muParser/muParserTokenReader.h \ 
		 ../3rdparty/muParser/muParserFixes.h \
#../3rdparty/muParser/muParserDLL.h \
		 ../3rdparty/muParser/muParserDef.h  	   
SOURCES+=../3rdparty/muParser/muParser.cpp \
         ../3rdparty/muParser/muParserBase.cpp \ 
         ../3rdparty/muParser/muParserInt.cpp \
		 ../3rdparty/muParser/muParserBytecode.cpp \
		 ../3rdparty/muParser/muParserCallback.cpp \
		 ../3rdparty/muParser/muParserTokenReader.cpp \
#../3rdparty/muParser/muParserDLL.cpp \
		 ../3rdparty/muParser/muParserError.cpp

#Origin Import (liborigin)
HEADERS+=../3rdparty/liborigin/OPJFile.h   
SOURCES+=../3rdparty/liborigin/OPJFile.cpp

#Compression (zlib123)
SOURCES+=../3rdparty/zlib123/minigzip.c

