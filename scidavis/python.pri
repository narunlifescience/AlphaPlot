##################### PYTHON + SIP + PyQT #####################

  INSTALLS += pythonconfig
  pythonconfig.files += scidavisrc.py
  DEFINES       += PYTHON_CONFIG_PATH="\\\"$$replace(pythonconfig.path," ","\\ ")\\\""

  INSTALLS += pythonutils
  pythonutils.files += scidavisUtil.py

  DEFINES       += PYTHON_UTIL_PATH="\\\"$$replace(pythonutils.path," ","\\ ")\\\""

  SIP_DIR = ../tmp/scidavis

  DEFINES += SCRIPTING_PYTHON

  message("Making PyQt bindings via SIP")
  unix {
    INCLUDEPATH += $$system(python ../python-includepath.py)
    osx_dist {
      DEFINES += PYTHONHOME=/Applications/scidavis.app/Contents/Resources
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


