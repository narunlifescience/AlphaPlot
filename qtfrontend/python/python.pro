include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static
DEPENDPATH += . .. ../core ../../backend ../../backend/core ../../backend/python $$SIP_DIR
INCLUDEPATH += .. ../../backend $$SIP_DIR
TARGET = ../$$qtLibraryTarget(scidavis_python)

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

python_config.files += scidavisrc.py
python_shared.files += python/scidavisUtil.py

HEADERS += PythonScript.h PythonScriptingEngine.h
SOURCES += PythonScript.cpp PythonScriptingEngine.cpp

# TODO: port SIP stuff
sip.target = $${SIP_DIR}/sipscidaviscmodule.cpp
sip.depends = scidavis.sip

unix {
INCLUDEPATH += $$system(python python-includepath.py)
LIBS        += $$system(python -c "\"from distutils import sysconfig; print '-lpython'+sysconfig.get_config_var('VERSION')\"")
LIBS        += -lm
#system(mkdir -p $${SIP_DIR})
#system($$system(python python-sipcmd.py) -c $${SIP_DIR} scidavis.sip)
sip.commands = $system(python python-sipcmd.py) -c ${SIP_DIR} scidavis.sip
}

win32 {
INCLUDEPATH += $$system(call python-includepath.py)
LIBS        += $$system(call python-libs-win.py)
# TODO: fix the command below (only really necessary if SIP_DIR != MOC/OBJECTS_DIR)
#system(md $${SIP_DIR})
#system($$system(call python-sipcmd.py) -c $${SIP_DIR} scidavis.sip)
sip.commands = $system(call python-sipcmd.py) -c ${SIP_DIR} scidavis.sip
}

##################### SIP generated files #####################

#HEADERS += $${SIP_DIR}/sipscidavisApplicationWindow.h\
#			  $${SIP_DIR}/sipscidavisLayer.h\
#			  $${SIP_DIR}/sipscidavisLineEnrichment.h\
#			  $${SIP_DIR}/sipscidavisImageEnrichment.h\
#			  $${SIP_DIR}/sipscidavisTextEnrichment.h\
#			  $${SIP_DIR}/sipscidavisGraph.h\
#			  $${SIP_DIR}/sipscidavisTable.h\
#			  $${SIP_DIR}/sipscidavisMatrix.h\
#			  $${SIP_DIR}/sipscidavisMyWidget.h\
#			  $${SIP_DIR}/sipscidavisScriptEdit.h\
#			  $${SIP_DIR}/sipscidavisNote.h\
#			  $${SIP_DIR}/sipscidavisPythonScript.h\
#			  $${SIP_DIR}/sipscidavisPythonScriptingEngine.h\
#			  $${SIP_DIR}/sipscidavisFolder.h\
#			  $${SIP_DIR}/sipscidavisQList.h\
#			  $${SIP_DIR}/sipscidavisFit.h \
#			  $${SIP_DIR}/sipscidavisExponentialFit.h \
#			  $${SIP_DIR}/sipscidavisTwoExpFit.h \
#			  $${SIP_DIR}/sipscidavisThreeExpFit.h \
#			  $${SIP_DIR}/sipscidavisSigmoidalFit.h \
#			  $${SIP_DIR}/sipscidavisGaussAmpFit.h \
#			  $${SIP_DIR}/sipscidavisLorentzFit.h \
#			  $${SIP_DIR}/sipscidavisUserFunctionFit.h \
#			  $${SIP_DIR}/sipscidavisPluginFit.h \
#			  $${SIP_DIR}/sipscidavisMultiPeakFit.h \
#			  $${SIP_DIR}/sipscidavisPolynomialFit.h \
#			  $${SIP_DIR}/sipscidavisLinearFit.h \
#			  $${SIP_DIR}/sipscidavisGaussFit.h \
#			  $${SIP_DIR}/sipscidavisFilter.h \
#			  $${SIP_DIR}/sipscidavisDifferentiation.h \
#			  $${SIP_DIR}/sipscidavisIntegration.h \
#			  $${SIP_DIR}/sipscidavisInterpolation.h \
#			  $${SIP_DIR}/sipscidavisSmoothFilter.h \
#			  $${SIP_DIR}/sipscidavisFFTFilter.h \
#			  $${SIP_DIR}/sipscidavisFFT.h \
#			  $${SIP_DIR}/sipscidavisCorrelation.h \
#			  $${SIP_DIR}/sipscidavisConvolution.h \
#			  $${SIP_DIR}/sipscidavisDeconvolution.h \
#
#SOURCES += $${SIP_DIR}/sipscidaviscmodule.cpp\
#			  $${SIP_DIR}/sipscidavisApplicationWindow.cpp\
#			  $${SIP_DIR}/sipscidavisLayer.cpp\
#			  $${SIP_DIR}/sipscidavisLineEnrichment.cpp\
#			  $${SIP_DIR}/sipscidavisImageEnrichment.cpp\
#			  $${SIP_DIR}/sipscidavisTextEnrichment.cpp\
#			  $${SIP_DIR}/sipscidavisGraph.cpp\
#			  $${SIP_DIR}/sipscidavisTable.cpp\
#			  $${SIP_DIR}/sipscidavisMatrix.cpp\
#			  $${SIP_DIR}/sipscidavisMyWidget.cpp\
#			  $${SIP_DIR}/sipscidavisScriptEdit.cpp\
#			  $${SIP_DIR}/sipscidavisNote.cpp\
#			  $${SIP_DIR}/sipscidavisPythonScript.cpp\
#			  $${SIP_DIR}/sipscidavisPythonScriptingEngine.cpp\
#			  $${SIP_DIR}/sipscidavisFolder.cpp\
#			  $${SIP_DIR}/sipscidavisQList.cpp\
#			  $${SIP_DIR}/sipscidavisFit.cpp \
#			  $${SIP_DIR}/sipscidavisExponentialFit.cpp \
#			  $${SIP_DIR}/sipscidavisTwoExpFit.cpp \
#			  $${SIP_DIR}/sipscidavisThreeExpFit.cpp \
#			  $${SIP_DIR}/sipscidavisSigmoidalFit.cpp \
#			  $${SIP_DIR}/sipscidavisGaussAmpFit.cpp \
#			  $${SIP_DIR}/sipscidavisLorentzFit.cpp \
#			  $${SIP_DIR}/sipscidavisUserFunctionFit.cpp \
#			  $${SIP_DIR}/sipscidavisPluginFit.cpp \
#			  $${SIP_DIR}/sipscidavisMultiPeakFit.cpp \
#			  $${SIP_DIR}/sipscidavisPolynomialFit.cpp \
#			  $${SIP_DIR}/sipscidavisLinearFit.cpp \
#			  $${SIP_DIR}/sipscidavisGaussFit.cpp \
#			  $${SIP_DIR}/sipscidavisFilter.cpp \
#			  $${SIP_DIR}/sipscidavisDifferentiation.cpp \
#			  $${SIP_DIR}/sipscidavisIntegration.cpp \
#			  $${SIP_DIR}/sipscidavisInterpolation.cpp \
#			  $${SIP_DIR}/sipscidavisSmoothFilter.cpp \
#			  $${SIP_DIR}/sipscidavisFFTFilter.cpp \
#			  $${SIP_DIR}/sipscidavisFFT.cpp \
#			  $${SIP_DIR}/sipscidavisCorrelation.cpp \
#			  $${SIP_DIR}/sipscidavisConvolution.cpp \
#			  $${SIP_DIR}/sipscidavisDeconvolution.cpp \
