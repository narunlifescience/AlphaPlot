#############################################################################
###################### USER-SERVICEABLE PART ################################
#############################################################################

# building without muParser doesn't work yet (but will in a future version)
SCRIPTING_LANGS += muParser
#SCRIPTING_LANGS += Python

# a console displaying output of scripts; particularly useful on Windows
# where running SciDAVis from a terminal is inconvenient
DEFINES         += SCRIPTING_CONSOLE
# a dialog for selecting the scripting language on a per-project basis
DEFINES         += SCRIPTING_DIALOG

CONFIG          += release
#CONFIG          += debug

# what to install and where
INSTALLS        += target
INSTALLS        += documentation
unix: target.path = /usr/bin
unix: documentation.path = /usr/share/doc/scidavis

##################### 3rd PARTY HEADER FILES SECTION ########################

INCLUDEPATH       += ../3rdparty/muParser
INCLUDEPATH       += ../3rdparty/qwtplot3d/include
INCLUDEPATH       += ../3rdparty/qwt/src
INCLUDEPATH       += ../3rdparty/liborigin
INCLUDEPATH       += ../3rdparty/gsl/include
INCLUDEPATH       += ../3rdparty/zlib123/include

#INCLUDEPATH            += /usr/include/qwt5
#INCLUDEPATH            += /usr/include/qwtplot3d

##################### 3rd PARTY LIBRARIES SECTION ###########################

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

# statically link against GSL in 3rdparty
#unix:LIBS         += ../3rdparty/gsl/lib/libgsl.a
#unix:LIBS         += ../3rdparty/gsl/lib/libgslcblas.a
#dynamically link against GSL installed system-wide
unix:LIBS         += -lgsl -lgslcblas

##################### Windows ###############################################

win32:LIBS        += ../3rdparty/qwtplot3d/lib/libqwtplot3d.a
win32:LIBS        += ../3rdparty/qwt/lib/libqwt.a
win32:LIBS        += ../3rdparty/gsl/lib/libgsl.a
win32:LIBS        += ../3rdparty/gsl/lib/libgslcblas.a
win32:LIBS        += ../3rdparty/zlib123/lib/zdll.lib

#############################################################################
###################### END OF USER-SERVICEABLE PART #########################
#############################################################################

#############################################################################
###################### BASIC PROJECT PROPERTIES #############################
#############################################################################

QMAKE_PROJECT_DEPTH = 0

TARGET         = scidavis
TEMPLATE       = app
CONFIG        += qt warn_on exceptions opengl thread
CONFIG        += assistant

DEFINES       += QT_PLUGIN
win32:DEFINES += QT_DLL QT_THREAD_SUPPORT
QT            += opengl qt3support network svg

MOC_DIR        = ../tmp/scidavis
OBJECTS_DIR    = ../tmp/scidavis
SIP_DIR        = ../tmp/scidavis
DESTDIR        = ./

#############################################################################
###################### PROJECT FILES SECTION ################################
#############################################################################

###################### ICONS ################################################
RESOURCES    +=        appicons.qrc
RESOURCES    +=        icons.qrc

###################### TRANSLATIONS #########################################

TRANSLATIONS    = translations/scidavis_de.ts \
                  translations/scidavis_es.ts \
                  translations/scidavis_fr.ts \
                  translations/scidavis_ru.ts \
                  translations/scidavis_ja.ts \
                  translations/scidavis_sv.ts

#system(lupdate -verbose scidavis.pro)
#system(lrelease -verbose scidavis.pro)

###################### DOCUMENTATION ########################################

documentation.files += ../manual/html \
                       ../README.html \
                       ../gpl.txt \

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
            src/AxesDialog.h \
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
            src/PlotCurve.h \
            src/QwtErrorPlotCurve.h \
            src/QwtPieCurve.h \
            src/ErrDialog.h \
            src/Legend.h \
            src/ArrowMarker.h \
            src/ImageMarker.h \
            src/ImageDialog.h \
            src/fit_gsl.h \
            src/nrutil.h\
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
            src/ImportASCIIDialog.h \
            src/ImageExportDialog.h\
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
            src/MultiPeakFit.h\
            src/ExponentialFit.h\
            src/PolynomialFit.h\
            src/NonLinearFit.h\
            src/PluginFit.h\
            src/SigmoidalFit.h\
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
            src/Correlation.h\
            src/PlotToolInterface.h\
            src/ScreenPickerTool.h\
            src/DataPickerTool.h\
            src/RangeSelectorTool.h\
            src/TranslateCurveTool.h\
            src/MultiPeakFitTool.h\
            src/CurveRangeDialog.h\
            src/LineProfileTool.h\
            src/PlotEnrichement.h\
            src/ExtensibleFileDialog.h\
            src/OpenProjectDialog.h\

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
            src/AxesDialog.cpp \
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
            src/PlotCurve.cpp \
            src/QwtErrorPlotCurve.cpp \
            src/QwtPieCurve.cpp \
            src/ErrDialog.cpp \
            src/Legend.cpp \
            src/ArrowMarker.cpp \
            src/ImageMarker.cpp \
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
            src/ImportASCIIDialog.cpp\
            src/ImageExportDialog.cpp\
            src/ScaleDraw.cpp\
            src/FunctionCurve.cpp\
            src/Fit.cpp\
            src/MultiPeakFit.cpp\
            src/ExponentialFit.cpp\
            src/PolynomialFit.cpp\
            src/PluginFit.cpp\
            src/NonLinearFit.cpp\
            src/SigmoidalFit.cpp\
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
            src/Correlation.cpp\
            src/ScreenPickerTool.cpp\
            src/DataPickerTool.cpp\
            src/RangeSelectorTool.cpp\
            src/TranslateCurveTool.cpp\
            src/MultiPeakFitTool.cpp\
            src/CurveRangeDialog.cpp\
            src/LineProfileTool.cpp\
            src/PlotEnrichement.cpp\
            src/ExtensibleFileDialog.cpp\
            src/OpenProjectDialog.cpp\

###############################################################
##################### Compression (zlib123) ###################
###############################################################

SOURCES += ../3rdparty/zlib123/minigzip.c

###############################################################
################# Origin Import (liborigin) ###################
###############################################################

HEADERS += ../3rdparty/liborigin/OPJFile.h
SOURCES += ../3rdparty/liborigin/OPJFile.cpp

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
    system($$system(python python-sipcmd.py) -c $${SIP_DIR} src/scidavis.sip)
  }

  win32 {
    INCLUDEPATH += $$system(call python-includepath.py)
    LIBS        += $$system(call python-libs-win.py)
    system(md $${SIP_DIR})
    system($$system(call python-sipcmd.py) -c $${SIP_DIR} src/scidavis.sip)
  }

##################### SIP generated files #####################

  HEADERS += $${SIP_DIR}/sipscidavisApplicationWindow.h\
             $${SIP_DIR}/sipscidavisGraph.h\
             $${SIP_DIR}/sipscidavisArrowMarker.h\
             $${SIP_DIR}/sipscidavisImageMarker.h\
             $${SIP_DIR}/sipscidavisLegend.h\
             $${SIP_DIR}/sipscidavisMultiLayer.h\
             $${SIP_DIR}/sipscidavisTable.h\
             $${SIP_DIR}/sipscidavisMatrix.h\
             $${SIP_DIR}/sipscidavisMyWidget.h\
             $${SIP_DIR}/sipscidavisScriptEdit.h\
             $${SIP_DIR}/sipscidavisNote.h\
             $${SIP_DIR}/sipscidavisPythonScript.h\
             $${SIP_DIR}/sipscidavisPythonScripting.h\
             $${SIP_DIR}/sipscidavisFolder.h\
             $${SIP_DIR}/sipscidavisQList.h\
             $${SIP_DIR}/sipscidavisFit.h \
             $${SIP_DIR}/sipscidavisExponentialFit.h \
             $${SIP_DIR}/sipscidavisTwoExpFit.h \
             $${SIP_DIR}/sipscidavisThreeExpFit.h \
             $${SIP_DIR}/sipscidavisSigmoidalFit.h \
             $${SIP_DIR}/sipscidavisGaussAmpFit.h \
             $${SIP_DIR}/sipscidavisLorentzFit.h \
             $${SIP_DIR}/sipscidavisNonLinearFit.h \
             $${SIP_DIR}/sipscidavisPluginFit.h \
             $${SIP_DIR}/sipscidavisMultiPeakFit.h \
             $${SIP_DIR}/sipscidavisPolynomialFit.h \
             $${SIP_DIR}/sipscidavisLinearFit.h \
             $${SIP_DIR}/sipscidavisGaussFit.h \
             $${SIP_DIR}/sipscidavisFilter.h \
             $${SIP_DIR}/sipscidavisDifferentiation.h \
             $${SIP_DIR}/sipscidavisIntegration.h \
             $${SIP_DIR}/sipscidavisInterpolation.h \
             $${SIP_DIR}/sipscidavisSmoothFilter.h \
             $${SIP_DIR}/sipscidavisFFTFilter.h \
             $${SIP_DIR}/sipscidavisFFT.h \
             $${SIP_DIR}/sipscidavisCorrelation.h \
             $${SIP_DIR}/sipscidavisConvolution.h \
             $${SIP_DIR}/sipscidavisDeconvolution.h \

  SOURCES += $${SIP_DIR}/sipscidaviscmodule.cpp\
             $${SIP_DIR}/sipscidavisApplicationWindow.cpp\
             $${SIP_DIR}/sipscidavisGraph.cpp\
             $${SIP_DIR}/sipscidavisArrowMarker.cpp\
             $${SIP_DIR}/sipscidavisImageMarker.cpp\
             $${SIP_DIR}/sipscidavisLegend.cpp\
             $${SIP_DIR}/sipscidavisMultiLayer.cpp\
             $${SIP_DIR}/sipscidavisTable.cpp\
             $${SIP_DIR}/sipscidavisMatrix.cpp\
             $${SIP_DIR}/sipscidavisMyWidget.cpp\
             $${SIP_DIR}/sipscidavisScriptEdit.cpp\
             $${SIP_DIR}/sipscidavisNote.cpp\
             $${SIP_DIR}/sipscidavisPythonScript.cpp\
             $${SIP_DIR}/sipscidavisPythonScripting.cpp\
             $${SIP_DIR}/sipscidavisFolder.cpp\
             $${SIP_DIR}/sipscidavisQList.cpp\
             $${SIP_DIR}/sipscidavisFit.cpp \
             $${SIP_DIR}/sipscidavisExponentialFit.cpp \
             $${SIP_DIR}/sipscidavisTwoExpFit.cpp \
             $${SIP_DIR}/sipscidavisThreeExpFit.cpp \
             $${SIP_DIR}/sipscidavisSigmoidalFit.cpp \
             $${SIP_DIR}/sipscidavisGaussAmpFit.cpp \
             $${SIP_DIR}/sipscidavisLorentzFit.cpp \
             $${SIP_DIR}/sipscidavisNonLinearFit.cpp \
             $${SIP_DIR}/sipscidavisPluginFit.cpp \
             $${SIP_DIR}/sipscidavisMultiPeakFit.cpp \
             $${SIP_DIR}/sipscidavisPolynomialFit.cpp \
             $${SIP_DIR}/sipscidavisLinearFit.cpp \
             $${SIP_DIR}/sipscidavisGaussFit.cpp \
             $${SIP_DIR}/sipscidavisFilter.cpp \
             $${SIP_DIR}/sipscidavisDifferentiation.cpp \
             $${SIP_DIR}/sipscidavisIntegration.cpp \
             $${SIP_DIR}/sipscidavisInterpolation.cpp \
             $${SIP_DIR}/sipscidavisSmoothFilter.cpp \
             $${SIP_DIR}/sipscidavisFFTFilter.cpp \
             $${SIP_DIR}/sipscidavisFFT.cpp \
             $${SIP_DIR}/sipscidavisCorrelation.cpp \
             $${SIP_DIR}/sipscidavisConvolution.cpp \
             $${SIP_DIR}/sipscidavisDeconvolution.cpp \
}
###############################################################
