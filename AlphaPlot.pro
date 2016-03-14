#################################################
# AlphaPlot main project file
# run "qmake AlphaPlot.pro" to generate the Makefile
# In most cases you don't have to make changes to
# this file but to the "config.pri" file 
#################################################
TEMPLATE = subdirs

# AEGIS build with tests 
aegis {CONFIG += test}

SUBDIRS += AlphaPlot 

test {SUBDIRS += test}

CONFIG+=ordered

!mxe {
# optional fit plugins
# mxe is a static build, so these are included elsewhere
SUBDIRS += fitPlugins/fitRational0\
           fitPlugins/fitRational1\
           fitPlugins/explin\
           fitPlugins/exp_saturation\
           fitPlugins/planck_wavelength\
}
