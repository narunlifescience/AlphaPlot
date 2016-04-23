#######################################################
# AlphaPlot main project file                         #
# run "qmake AlphaPlot.pro" to generate the Makefile  #
#######################################################

TEMPLATE = subdirs

SUBDIRS += AlphaPlot 
CONFIG+=ordered

!mxe {
# optional fit plugins (mxe is a static build, so included elsewhere)
SUBDIRS += fitPlugins/fitRational0\
           fitPlugins/fitRational1\
           fitPlugins/explin\
           fitPlugins/exp_saturation\
           fitPlugins/planck_wavelength\
}
