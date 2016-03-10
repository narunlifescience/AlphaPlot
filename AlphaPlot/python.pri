##################### PYTHON + SIP + PyQT #####################

  INSTALLS += pythonconfig
  pythonconfig.files += AlphaPlotrc.py AlphaPlotrc.pyc
  DEFINES       += PYTHON_CONFIG_PATH="\\\"$$replace(pythonconfig.path," ","\\ ")\\\""

  INSTALLS += pythonutils
  pythonutils.files += AlphaPlotUtil.py AlphaPlotUtil.pyc

  compiledAlphaPlotrc.target = AlphaPlotrc.pyc
  compiledAlphaPlotrc.depends = AlphaPlotrc.py
  compiledAlphaPlotrc.commands = python -m py_compile AlphaPlotrc.py
  compiledAlphaPlotUtil.target = AlphaPlotUtil.pyc
  compiledAlphaPlotUtil.depends = AlphaPlotUtil.py
  compiledAlphaPlotUtil.commands = python -m py_compile AlphaPlotUtil.py
  QMAKE_EXTRA_TARGETS += compiledAlphaPlotrc compiledAlphaPlotUtil
  PRE_TARGETDEPS += AlphaPlotrc.pyc AlphaPlotUtil.pyc

  DEFINES       += PYTHON_UTIL_PATH="\\\"$$replace(pythonutils.path," ","\\ ")\\\""

  SIP_DIR = ../tmp/AlphaPlot

  DEFINES += SCRIPTING_PYTHON

  message("Making PyQt bindings via SIP")
  unix {
    INCLUDEPATH += $$system(python ../python-includepath.py)
    osx_dist {
      DEFINES += PYTHONHOME=/Applications/AlphaPlot.app/Contents/Resources
    } else {
      macx {
        LIBS += -framework Python
      } else {
        LIBS += $$system(python -c "\"from distutils import sysconfig; print '-lpython'+sysconfig.get_config_var('VERSION')\"")
      }
    }     
    LIBS        += -lm
  }

  win32 {
  mxe {
     DEFINES += SIP_STATIC_MODULE
#    QMAKE_LIBPATH += "$$(HOME)/usr/mxe/PyQt4/"
    LIBS += -L"$$(HOME)/usr/mxe/PyQt4/" -lPyQtCore -lPyQtGui -lqpygui -lqpycore -lsip -lpython27
  } else {
    INCLUDEPATH += $$system(call ../python-includepath.py)
    LIBS        += $$system(call ../python-libs-win.py)
  }
}

##################### PYTHON + SIP + PyQT #####################

  INSTALLS += pythonconfig
  pythonconfig.files += AlphaPlotrc.py
  DEFINES       += PYTHON_CONFIG_PATH="\\\"$$replace(pythonconfig.path," ","\\ ")\\\""

  INSTALLS += pythonutils
  pythonutils.files += AlphaPlotUtil.py

  DEFINES       += PYTHON_UTIL_PATH="\\\"$$replace(pythonutils.path," ","\\ ")\\\""

  SIP_DIR = ../tmp/AlphaPlot

  DEFINES += SCRIPTING_PYTHON
  HEADERS += src/PythonScript.h src/PythonScripting.h
  SOURCES += src/PythonScript.cpp src/PythonScripting.cpp

  unix {
    INCLUDEPATH += $$system(python ../python-includepath.py)
    osx_dist {
      DEFINES += PYTHONHOME=/Applications/AlphaPlot.app/Contents/Resources
    } 
    system(mkdir -p $${SIP_DIR})
    system($$system(python python-sipcmd.py) -c $${SIP_DIR} src/AlphaPlot.sip)
  }

  win32 {
  mxe {
     DEFINES += SIP_STATIC_MODULE
    system($$system(python python-sipcmd.py) -c $${SIP_DIR} src/AlphaPlot.sip)
  } else {
    INCLUDEPATH += $$system(call ../python-includepath.py)
    # TODO: fix the command below (only really necessary if SIP_DIR != MOC/OBJECTS_DIR)
    system($$system(call python-sipcmd.py) -c $${SIP_DIR} src/AlphaPlot.sip)
  }
}


##################### SIP generated files #####################

  HEADERS += $${SIP_DIR}/sipAPIAlphaPlot.h

  SOURCES += $${SIP_DIR}/sipAlphaPlotcmodule.cpp\
             $${SIP_DIR}/sipAlphaPlotApplicationWindow.cpp\
             $${SIP_DIR}/sipAlphaPlotGraph.cpp\
             $${SIP_DIR}/sipAlphaPlotArrowMarker.cpp\
             $${SIP_DIR}/sipAlphaPlotImageMarker.cpp\
             $${SIP_DIR}/sipAlphaPlotLegend.cpp\
             $${SIP_DIR}/sipAlphaPlotMultiLayer.cpp\
             $${SIP_DIR}/sipAlphaPlotTable.cpp\
             $${SIP_DIR}/sipAlphaPlotMatrix.cpp\
             $${SIP_DIR}/sipAlphaPlotMyWidget.cpp\
             $${SIP_DIR}/sipAlphaPlotScriptEdit.cpp\
             $${SIP_DIR}/sipAlphaPlotNote.cpp\
             $${SIP_DIR}/sipAlphaPlotPythonScript.cpp\
             $${SIP_DIR}/sipAlphaPlotPythonScripting.cpp\
             $${SIP_DIR}/sipAlphaPlotFolder.cpp\
             $${SIP_DIR}/sipAlphaPlotFit.cpp \
             $${SIP_DIR}/sipAlphaPlotExponentialFit.cpp \
             $${SIP_DIR}/sipAlphaPlotTwoExpFit.cpp \
             $${SIP_DIR}/sipAlphaPlotThreeExpFit.cpp \
             $${SIP_DIR}/sipAlphaPlotSigmoidalFit.cpp \
             $${SIP_DIR}/sipAlphaPlotGaussAmpFit.cpp \
             $${SIP_DIR}/sipAlphaPlotLorentzFit.cpp \
             $${SIP_DIR}/sipAlphaPlotNonLinearFit.cpp \
             $${SIP_DIR}/sipAlphaPlotPluginFit.cpp \
             $${SIP_DIR}/sipAlphaPlotMultiPeakFit.cpp \
             $${SIP_DIR}/sipAlphaPlotPolynomialFit.cpp \
             $${SIP_DIR}/sipAlphaPlotLinearFit.cpp \
             $${SIP_DIR}/sipAlphaPlotGaussFit.cpp \
             $${SIP_DIR}/sipAlphaPlotFilter.cpp \
             $${SIP_DIR}/sipAlphaPlotDifferentiation.cpp \
             $${SIP_DIR}/sipAlphaPlotIntegration.cpp \
             $${SIP_DIR}/sipAlphaPlotInterpolation.cpp \
             $${SIP_DIR}/sipAlphaPlotSmoothFilter.cpp \
             $${SIP_DIR}/sipAlphaPlotFFTFilter.cpp \
             $${SIP_DIR}/sipAlphaPlotFFT.cpp \
             $${SIP_DIR}/sipAlphaPlotCorrelation.cpp \
             $${SIP_DIR}/sipAlphaPlotConvolution.cpp \
             $${SIP_DIR}/sipAlphaPlotDeconvolution.cpp \
             $${SIP_DIR}/sipAlphaPlotAbstractAspect.cpp \
             $${SIP_DIR}/sipAlphaPlotColumn.cpp \
         $${SIP_DIR}/sipAlphaPlotQwtSymbol.cpp \
         $${SIP_DIR}/sipAlphaPlotQwtPlotCurve.cpp \
             $${SIP_DIR}/sipAlphaPlotQwtPlot.cpp \
         $${SIP_DIR}/sipAlphaPlotGrid.cpp \

exists($${SIP_DIR}/sipAlphaPlotQList.cpp) {
# SIP < 4.9
  SOURCES += $${SIP_DIR}/sipAlphaPlotQList.cpp
} else {
  SOURCES += \
         $${SIP_DIR}/sipAlphaPlotQList0100QDateTime.cpp\
         $${SIP_DIR}/sipAlphaPlotQList0101Folder.cpp\
         $${SIP_DIR}/sipAlphaPlotQList0101MyWidget.cpp\
         $${SIP_DIR}/sipAlphaPlotQList0101QwtPlotCurve.cpp\
}

###############################################################
