##################### PYTHON + SIP + PyQT #####################

  INSTALLS += pythonconfig
  pythonconfig.files += scidavisrc.py
  DEFINES       += PYTHON_CONFIG_PATH="\\\"$$replace(pythonconfig.path," ","\\ ")\\\""

  INSTALLS += pythonutils
  pythonutils.files += scidavisUtil.py

  DEFINES       += PYTHON_UTIL_PATH="\\\"$$replace(pythonutils.path," ","\\ ")\\\""

  SIP_DIR = ../tmp/scidavis

  DEFINES += SCRIPTING_PYTHON
  HEADERS += src/PythonScript.h src/PythonScripting.h
  SOURCES += src/PythonScript.cpp src/PythonScripting.cpp

  unix {
    INCLUDEPATH += $$system(python ../python-includepath.py)
    osx_dist {
      DEFINES += PYTHONHOME=/Applications/scidavis.app/Contents/Resources
    } 
    system(mkdir -p $${SIP_DIR})
    system($$system(python python-sipcmd.py) -c $${SIP_DIR} src/scidavis.sip)
  }

  win32 {
  mxe {
     DEFINES += SIP_STATIC_MODULE
    system($$system(python python-sipcmd.py) -c $${SIP_DIR} src/scidavis.sip)
  } else {
    INCLUDEPATH += $$system(call ../python-includepath.py)
    # TODO: fix the command below (only really necessary if SIP_DIR != MOC/OBJECTS_DIR)
    system($$system(call python-sipcmd.py) -c $${SIP_DIR} src/scidavis.sip)
  }
}


##################### SIP generated files #####################

  HEADERS += $${SIP_DIR}/sipAPIscidavis.h

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
             $${SIP_DIR}/sipscidavisAbstractAspect.cpp \
             $${SIP_DIR}/sipscidavisColumn.cpp \
				 $${SIP_DIR}/sipscidavisQwtSymbol.cpp \
				 $${SIP_DIR}/sipscidavisQwtPlotCurve.cpp \
             $${SIP_DIR}/sipscidavisQwtPlot.cpp \
				 $${SIP_DIR}/sipscidavisGrid.cpp \

exists($${SIP_DIR}/sipscidavisQList.cpp) {
# SIP < 4.9
	SOURCES += $${SIP_DIR}/sipscidavisQList.cpp
} else {
	SOURCES += \
			   $${SIP_DIR}/sipscidavisQList0100QDateTime.cpp\
			   $${SIP_DIR}/sipscidavisQList0101Folder.cpp\
			   $${SIP_DIR}/sipscidavisQList0101MyWidget.cpp\
			   $${SIP_DIR}/sipscidavisQList0101QwtPlotCurve.cpp\
}

###############################################################
