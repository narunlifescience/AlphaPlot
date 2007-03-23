#############################################################################
###################### USER-SERVICEABLE PART ################################
#############################################################################

# building without muParser doesn't work yet
SCRIPTING_LANGS += muParser
SCRIPTING_LANGS += Python

# a console displaying output of scripts; particularly useful on Windows
# where running QtiPlot from a terminal is inconvenient
DEFINES         += SCRIPTING_CONSOLE
# a dialog for selecting the scripting language on a per-project basis
DEFINES         += SCRIPTING_DIALOG

CONFIG          += release
#CONFIG          += debug

# what to install and where
INSTALLS        += target
INSTALLS        += documentation
unix: target.path = /usr/bin
unix: documentation.path = /usr/share/doc/qtiplot

##################### 3rd PARTY HEADER FILES SECTION ########################
#!!! Warning: You must modify these paths according to your computer settings
#############################################################################

INCLUDEPATH       += ../3rdparty

INCLUDEPATH       += ../3rdparty/qwtplot3d/include
INCLUDEPATH       += ../3rdparty/qwt/src
#INCLUDEPATH       += /usr/include/qwtplot3d
#INCLUDEPATH       += /usr/include/qwt5

INCLUDEPATH       += ../3rdparty/liborigin
INCLUDEPATH       += ../3rdparty/gsl/include
INCLUDEPATH       += ../3rdparty/zlib123/include

##################### 3rd PARTY LIBRARIES SECTION ###########################
#!!! Warning: You must modify these paths according to your computer settings
#############################################################################

##################### Linux (Mac OS X) ######################################

#for dynamically linked libs
linux-g++-64: libsuff = 64
unix:LIBS         += -L /usr/lib$${libsuff}

# statically link against Qwt(3D) in 3rdparty
unix:LIBS         += ../3rdparty/qwtplot3d/lib/libqwtplot3d.a
unix:LIBS         += ../3rdparty/qwt/lib/libqwt.a
# dynamically link against Qwt(3D) installed system-wide
# WARNING: make sure they are compiled against Qt4
#unix:LIBS         += -lqwtplot3d
#unix:LIBS         += -lqwt

# statically link against liborigin in 3rdparty
unix:LIBS         += ../3rdparty/liborigin/liborigin.a
# dynamically link against liborigin installed system-wide
#unix:LIBS         += -lorigin

# statically link against GSL in 3rdparty
unix:LIBS         += ../3rdparty/gsl/lib/libgsl.a
unix:LIBS         += ../3rdparty/gsl/lib/libgslcblas.a
#dynamically link against GSL installed system-wide
#unix:LIBS         += -lgsl -lgslcblas


##################### Windows ###############################################

win32:LIBS        += ../3rdparty/qwtplot3d/lib/libqwtplot3d.a
win32:LIBS        += ../3rdparty/qwt/lib/libqwt.a
win32:LIBS        += ../3rdparty/gsl/lib/libgsl.a
win32:LIBS        += ../3rdparty/gsl/lib/libgslcblas.a
win32:LIBS        += ../3rdparty/zlib123/lib/zdll.lib
win32:LIBS        += ../3rdparty/liborigin/liborigin.a

#############################################################################
###################### END OF USER-SERVICEABLE PART #########################
#############################################################################

#############################################################################
###################### BASIC PROJECT PROPERTIES #############################
#############################################################################

QMAKE_PROJECT_DEPTH = 0

TARGET         = qtiplot
TEMPLATE       = app
CONFIG        += qt warn_on exceptions opengl
CONFIG        += assistant

DEFINES       += QT_PLUGIN
win32:DEFINES += QT_DLL QT_THREAD_SUPPORT
QT            += opengl qt3support network

MOC_DIR        = ../tmp/qtiplot
OBJECTS_DIR    = ../tmp/qtiplot
SIP_DIR        = ../tmp/qtiplot
DESTDIR        = ./

#############################################################################
###################### PROJECT FILES SECTION ################################
#############################################################################

###################### ICONS ################################################

win32:RC_FILE = icons/qtiplot.rc
mac:RC_FILE   = icons/qtiplot.icns

###################### TRANSLATIONS #########################################

TRANSLATIONS    = translations/qtiplot_de.ts \
                  translations/qtiplot_es.ts \
                  translations/qtiplot_fr.ts \
                  translations/qtiplot_ru.ts \
                  translations/qtiplot_ja.ts \
                  translations/qtiplot_sv.ts \

#system(lupdate -verbose qtiplot.pro)
#system(lrelease -verbose qtiplot.pro)

###################### DOCUMENTATION ########################################

documentation.files += ../manual/html \
                       ../README.html \
                       ../gpl_licence.txt \

###################### HEADERS ##############################################

HEADERS  += src/ApplicationWindow.h \
            src/globals.h\
            src/Graph.h \
            src/Graph3D.h \
            src/Table.h \
            src/CurvesDialog.h \
            src/SetColValuesDialog.h \
            src/PlotDialog.h \
            src/Plot3DDialog.h \
            src/PlotWizard.h \
            src/ExportDialog.h \
            src/ImportDialog.h \
            src/AxesDialog.h \
            src/PieDialog.h \
            src/PolynomFitDialog.h \
            src/ExpDecayDialog.h \
            src/FunctionDialog.h \
            src/FitDialog.h \
            src/SurfaceDialog.h \
            src/TableDialog.h \
            src/TextDialog.h \
            src/LineDialog.h \
            src/ScalePicker.h \
            src/TitlePicker.h \
            src/CanvasPicker.h \
            src/QwtErrorPlotCurve.h \
            src/QwtPieCurve.h \
            src/ErrDialog.h \
            src/LegendMarker.h \
            src/LineMarker.h\
            src/ImageMarker.h\
            src/ImageDialog.h \
            src/fit_gsl.h \
            src/nrutil.h\
            src/pixmaps.h\
            src/MultiLayer.h\
            src/LayerDialog.h \
            src/IntDialog.h \
            src/SortDialog.h\
            src/Bar.h \
            src/Cone3D.h \
            src/ConfigDialog.h \
            src/QwtBarCurve.h \
            src/BoxCurve.h \
            src/QwtHistogram.h \
            src/VectorCurve.h \
            src/ScaleDraw.h \
            src/ImageExportOptionsDialog.h \
            src/Matrix.h \
            src/MatrixDialog.h \
            src/MatrixSizeDialog.h \
            src/MatrixValuesDialog.h \
            src/DataSetDialog.h \
            src/MyParser.h \
            src/ColorBox.h \
            src/SymbolBox.h \
            src/PatternBox.h \
            src/importOPJ.h\
            src/SymbolDialog.h \
            src/Plot.h \
            src/ColorButton.h \
            src/AssociationsDialog.h \
            src/RenameWindowDialog.h \
            src/MyWidget.h \
            src/InterpolationDialog.h\
            src/ImportFilesDialog.h \
            src/ImageExportDialog.h\
            src/EpsExportDialog.h\
            src/SmoothCurveDialog.h\
            src/FilterDialog.h\
            src/FFTDialog.h\
            src/Note.h\
            src/Folder.h\
            src/FindDialog.h\
            src/ScriptingEnv.h\
            src/Script.h\
            src/ScriptEdit.h\
            src/FunctionCurve.h\
            src/Fit.h\
            src/fitclasses.h\
            src/customevents.h\
            src/ScriptingLangDialog.h\
            src/ScriptWindow.h\
            src/TextFormatButtons.h\
            src/TableStatistics.h\
            src/Spectrogram.h\
            src/ColorMapEditor.h\
            src/SelectionMoveResizer.h\
            src/Filter.h\
            src/Differentiation.h\
            src/Integration.h\
            src/Interpolation.h\
            src/SmoothFilter.h\
            src/FFTFilter.h\
            src/FFT.h\
			src/Convolution.h\
			src/Correlation.h

###################### SOURCES ##############################################

SOURCES  += src/ApplicationWindow.cpp \
            src/Graph.cpp \
            src/Graph3D.cpp \
            src/Table.cpp \
            src/SetColValuesDialog.cpp \
            src/CurvesDialog.cpp \
            src/PlotDialog.cpp \
            src/Plot3DDialog.cpp \
            src/PlotWizard.cpp \
            src/ExportDialog.cpp \
            src/ImportDialog.cpp \
            src/AxesDialog.cpp \
            src/PieDialog.cpp \
            src/PolynomFitDialog.cpp \
            src/TableDialog.cpp \
            src/TextDialog.cpp \
            src/ScalePicker.cpp\
            src/TitlePicker.cpp \
            src/CanvasPicker.cpp \
            src/ExpDecayDialog.cpp \
            src/FunctionDialog.cpp \
            src/FitDialog.cpp \
            src/SurfaceDialog.cpp \
            src/LineDialog.cpp \
            src/QwtErrorPlotCurve.cpp \
            src/QwtPieCurve.cpp \
            src/ErrDialog.cpp \
            src/LegendMarker.cpp \
            src/LineMarker.cpp \
            src/ImageMarker.cpp\
            src/ImageDialog.cpp \
            src/MultiLayer.cpp\
            src/LayerDialog.cpp \
            src/IntDialog.cpp \
            src/SortDialog.cpp\
            src/Bar.cpp \
            src/Cone3D.cpp \
            src/DataSetDialog.cpp \
            src/ConfigDialog.cpp \
            src/QwtBarCurve.cpp \
            src/BoxCurve.cpp \
            src/QwtHistogram.cpp \
            src/VectorCurve.cpp \
            src/ImageExportOptionsDialog.cpp \
            src/Matrix.cpp \
            src/MatrixDialog.cpp \
            src/MatrixSizeDialog.cpp \
            src/MatrixValuesDialog.cpp \
            src/MyParser.cpp\
            src/ColorBox.cpp \
            src/SymbolBox.cpp \
            src/PatternBox.cpp \
            src/importOPJ.cpp\
            src/main.cpp \
            src/SymbolDialog.cpp \
            src/Plot.cpp \
            src/ColorButton.cpp \
            src/AssociationsDialog.cpp \
            src/RenameWindowDialog.cpp \
            src/MyWidget.cpp\
            src/InterpolationDialog.cpp\
            src/EpsExportDialog.cpp\
            src/nrutil.cpp\
            src/fit_gsl.cpp\
            src/SmoothCurveDialog.cpp\
            src/FilterDialog.cpp\
            src/FFTDialog.cpp\
            src/Note.cpp\
            src/Folder.cpp\
            src/FindDialog.cpp\
            src/TextFormatButtons.cpp\
            src/ScriptEdit.cpp\
            src/ImportFilesDialog.cpp\
            src/ImageExportDialog.cpp\
            src/ScaleDraw.cpp\
            src/FunctionCurve.cpp\
            src/Fit.cpp\
            src/fitclasses.cpp\
            src/ScriptingEnv.cpp\
            src/Script.cpp\
            src/ScriptingLangDialog.cpp\
            src/ScriptWindow.cpp\
            src/TableStatistics.cpp\
            src/Spectrogram.cpp\
            src/ColorMapEditor.cpp\
            src/SelectionMoveResizer.cpp\
            src/Filter.cpp\
            src/Differentiation.cpp\
            src/Integration.cpp\
            src/Interpolation.cpp\
            src/SmoothFilter.cpp\
            src/FFTFilter.cpp\
            src/FFT.cpp\
			src/Convolution.cpp\
			src/Correlation.cpp

###############################################################
##################### Compression (zlib123) ###################
###############################################################

SOURCES += ../3rdparty/zlib123/minigzip.c

###############################################################
##################### SCRIPTING LANGUAGES SECTION #############
###############################################################

##################### Default: muParser v1.28 #################

contains(SCRIPTING_LANGS, muParser) {
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

}

##################### PYTHON + SIP + PyQT #####################

contains(SCRIPTING_LANGS, Python) {
  DEFINES += SCRIPTING_PYTHON
  HEADERS += src/PythonScript.h src/PythonScripting.h
  SOURCES += src/PythonScript.cpp src/PythonScripting.cpp

  unix {
    INCLUDEPATH += $$system(python python-includepath.py)
    LIBS        += $$system(python -c "\"from distutils import sysconfig; print '-lpython'+sysconfig.get_config_var('VERSION')\"")
    LIBS        += -lm
    system(mkdir -p $${SIP_DIR})
    system($$system(python python-sipcmd.py) -c $${SIP_DIR} src/qti.sip)
  }

  win32 {
    INCLUDEPATH += $$system(call python-includepath.py)
    LIBS        += $$system(call python-libs-win.py)
    system(md $${SIP_DIR})
    system($$system(call python-sipcmd.py) -c $${SIP_DIR} src/qti.sip)
  }

##################### SIP generated files #####################

  HEADERS += ../tmp/qtiplot/sipqtiApplicationWindow.h\
             ../tmp/qtiplot/sipqtiGraph.h\
             ../tmp/qtiplot/sipqtiLineMarker.h\
             ../tmp/qtiplot/sipqtiMultiLayer.h\
             ../tmp/qtiplot/sipqtiTable.h\
             ../tmp/qtiplot/sipqtiMatrix.h\
             ../tmp/qtiplot/sipqtiMyWidget.h\
             ../tmp/qtiplot/sipqtiScriptEdit.h\
             ../tmp/qtiplot/sipqtiNote.h\
             ../tmp/qtiplot/sipqtiPythonScript.h\
             ../tmp/qtiplot/sipqtiPythonScripting.h\
             ../tmp/qtiplot/sipqtiFolder.h\
             ../tmp/qtiplot/sipqtiQList.h\
             ../tmp/qtiplot/sipqtiFit.h \
             ../tmp/qtiplot/sipqtiExponentialFit.h \
             ../tmp/qtiplot/sipqtiTwoExpFit.h \
             ../tmp/qtiplot/sipqtiThreeExpFit.h \
             ../tmp/qtiplot/sipqtiSigmoidalFit.h \
             ../tmp/qtiplot/sipqtiGaussAmpFit.h \
             ../tmp/qtiplot/sipqtiLorentzFit.h \
             ../tmp/qtiplot/sipqtiNonLinearFit.h \
             ../tmp/qtiplot/sipqtiPluginFit.h \
             ../tmp/qtiplot/sipqtiMultiPeakFit.h \
             ../tmp/qtiplot/sipqtiPolynomialFit.h \
             ../tmp/qtiplot/sipqtiLinearFit.h \
             ../tmp/qtiplot/sipqtiGaussFit.h \

  SOURCES += ../tmp/qtiplot/sipqticmodule.cpp\
             ../tmp/qtiplot/sipqtiApplicationWindow.cpp\
             ../tmp/qtiplot/sipqtiGraph.cpp\
             ../tmp/qtiplot/sipqtiLineMarker.cpp\
             ../tmp/qtiplot/sipqtiMultiLayer.cpp\
             ../tmp/qtiplot/sipqtiTable.cpp\
             ../tmp/qtiplot/sipqtiMatrix.cpp\
             ../tmp/qtiplot/sipqtiMyWidget.cpp\
             ../tmp/qtiplot/sipqtiScriptEdit.cpp\
             ../tmp/qtiplot/sipqtiNote.cpp\
             ../tmp/qtiplot/sipqtiPythonScript.cpp\
             ../tmp/qtiplot/sipqtiPythonScripting.cpp\
             ../tmp/qtiplot/sipqtiFolder.cpp\
             ../tmp/qtiplot/sipqtiQList.cpp\
             ../tmp/qtiplot/sipqtiFit.cpp \
             ../tmp/qtiplot/sipqtiExponentialFit.cpp \
             ../tmp/qtiplot/sipqtiTwoExpFit.cpp \
             ../tmp/qtiplot/sipqtiThreeExpFit.cpp \
             ../tmp/qtiplot/sipqtiSigmoidalFit.cpp \
             ../tmp/qtiplot/sipqtiGaussAmpFit.cpp \
             ../tmp/qtiplot/sipqtiLorentzFit.cpp \
             ../tmp/qtiplot/sipqtiNonLinearFit.cpp \
             ../tmp/qtiplot/sipqtiPluginFit.cpp \
             ../tmp/qtiplot/sipqtiMultiPeakFit.cpp \
             ../tmp/qtiplot/sipqtiPolynomialFit.cpp \
             ../tmp/qtiplot/sipqtiLinearFit.cpp \
             ../tmp/qtiplot/sipqtiGaussFit.cpp \

}
###############################################################
