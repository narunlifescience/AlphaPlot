#################################################
# SciDAVis main project file
# run "qmake scidavis.pro" to generate the Makefile
# In most cases you don't have to make changes to
# this file but to the "scidavis.pro" file in the
# "scidavis" subdirectory.
#################################################
TEMPLATE = subdirs

SUBDIRS = 3rdparty/liborigin scidavis test
CONFIG+=ordered

!mxe {
# optional fit plugins
SUBDIRS += fitPlugins/fitRational0\
           fitPlugins/fitRational1\
           fitPlugins/explin\
           fitPlugins/exp_saturation\
           fitPlugins/planck_wavelength\
}
